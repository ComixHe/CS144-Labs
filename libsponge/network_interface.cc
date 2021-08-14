#include "network_interface.hh"

#include "arp_message.hh"
#include "ethernet_frame.hh"

#include <iostream>

// Dummy implementation of a network interface
// Translates from {IP datagram, next hop address} to link-layer frame, and from link-layer frame to IP datagram

// For Lab 5, please replace with a real implementation that passes the
// automated checks run by `make check_lab5`.

// You will need to add private members to the class declaration in `network_interface.hh`

//! \param[in] ethernet_address Ethernet (what ARP calls "hardware") address of the interface
//! \param[in] ip_address IP (what ARP calls "protocol") address of the interface
NetworkInterface::NetworkInterface(const EthernetAddress &ethernet_address, const Address &ip_address)
    : _ethernet_address(ethernet_address), _ip_address(ip_address) {
    std::cerr << "DEBUG: Network interface has Ethernet address " << to_string(_ethernet_address) << " and IP address "
         << ip_address.ip() << "\n";
}

//! \param[in] dgram the IPv4 datagram to be sent
//! \param[in] next_hop the IP address of the interface to send it to (typically a router or default gateway, but may also be another host if directly connected to the same network as the destination)
//! (Note: the Address type can be converted to a uint32_t (raw 32-bit IP address) with the Address::ipv4_numeric() method.)
void NetworkInterface::send_datagram(const InternetDatagram &dgram, const Address &next_hop) {
    // convert IP address of next hop to raw 32-bit representation (used in ARP header)
    const uint32_t next_hop_ip = next_hop.ipv4_numeric();
    EthernetFrame frame;
    frame.header().type = EthernetHeader::TYPE_IPv4;
    frame.header().src = _ethernet_address;
    frame.payload() = std::move(dgram.serialize());
    if(auto it = _mapping_table.find(next_hop_ip); it != _mapping_table.end() && it->second.ttl <= expire_time){ //映射查询成功，直接发送
        frame.header().dst = it->second.mac;
        _frames_out.emplace(std::move(frame));
    } else {
        _first_flag = true;
        for(auto &ip_it : _unknown_ipaddr){ //判断是否是第一次查询
            if(ip_it.unknown_ip == next_hop_ip){
                _first_flag = false;
                break;
            }
        }
        if(_first_flag == true)
            _unknown_ipaddr.emplace_back(next_hop_ip); //第一次发送则直接在队列里构造
        _arpframe_send();//再次尝试arp
        _waiting_frame.emplace_back(next_hop_ip,frame); //查不到地址的以太网帧进入缓存
    }
    return;
}

//! \param[in] frame the incoming Ethernet frame
std::optional<InternetDatagram> NetworkInterface::recv_frame(const EthernetFrame &frame) {
    if(!_ethernet_address_equal(frame.header().dst,ETHERNET_BROADCAST)&&
       !_ethernet_address_equal(frame.header().dst,_ethernet_address)){
       return std::nullopt;
    } else if (frame.header().type == EthernetHeader::TYPE_IPv4) {
        InternetDatagram dgram;
        if(dgram.parse(frame.payload()) == ParseResult::NoError)
            return std::optional{dgram};
        else
            return std::nullopt;
    } else if (frame.header().type == EthernetHeader::TYPE_ARP) {
        ARPMessage rec_frame;
            if(rec_frame.parse(frame.payload()) == ParseResult::NoError){
                auto dst_ip = rec_frame.sender_ip_address; 
                _mapping_table[dst_ip].mac = rec_frame.sender_ethernet_address;
                _mapping_table[dst_ip].ttl = 0; //更新映射关系
                for(auto it = _unknown_ipaddr.begin();it!=_unknown_ipaddr.end();++it){ //清除待查询队列中的匹配ip
                    if(it->unknown_ip == dst_ip){
                        _unknown_ipaddr.erase(it);
                        break;
                    }
                }
                if(rec_frame.target_ip_address == _ip_address.ipv4_numeric() && rec_frame.opcode == ARPMessage::OPCODE_REQUEST) { //收到广播后发送arp响应报文
                    ARPMessage arp_reply;
                    arp_reply.opcode = ARPMessage::OPCODE_REPLY;
                    arp_reply.sender_ethernet_address = _ethernet_address;
                    arp_reply.sender_ip_address = _ip_address.ipv4_numeric();
                    arp_reply.target_ethernet_address = rec_frame.sender_ethernet_address;
                    arp_reply.target_ip_address = rec_frame.sender_ip_address;
                    EthernetFrame arp_frame;
                    arp_frame.header().type = EthernetHeader::TYPE_ARP;
                    arp_frame.header().src = _ethernet_address;
                    arp_frame.header().dst = rec_frame.sender_ethernet_address;
                    arp_frame.payload() = std::move(arp_reply.serialize());
                    _frames_out.emplace(std::move(arp_frame));
                }
                for(auto it = _waiting_frame.begin();it!=_waiting_frame.end();){ //查询以太网帧缓冲区，如果查到映射则移出缓冲区到发送队列中
                    if(auto eth_dst=_mapping_table.find(it->ipaddr);eth_dst!=_mapping_table.end()){
                        it->frame.header().dst = eth_dst->second.mac;
                        _frames_out.emplace(it->frame);
                        it = _waiting_frame.erase(it);
                    }
                    else
                        ++it;
                }
            }
            else
                return std::nullopt;
    }
    return std::nullopt;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void NetworkInterface::tick(const size_t ms_since_last_tick) {
    for(auto it = _mapping_table.begin();it!=_mapping_table.end();){ //更新映射存在时间
        it->second.ttl += ms_since_last_tick;
        if(it->second.ttl > expire_time){ //超时抛弃
            it = _mapping_table.erase(it);}
        else
            ++it;
    }
    for(auto it = _unknown_ipaddr.begin();it!=_unknown_ipaddr.end();++it) //更新arp报文计时器
        it->last_arp_time += ms_since_last_tick;
    _arpframe_send();
    return;
}

void NetworkInterface::_arpframe_send(){ //arp报文到时重传或者第一次发送arp查询
    auto it = _unknown_ipaddr.begin();
    while(it != _unknown_ipaddr.end()){
        if(_first_flag){
            _first_flag = false;
        }else if(it->last_arp_time <= arp_again)
            return;
        
        ARPMessage msg;
        msg.opcode = ARPMessage::OPCODE_REQUEST;
        msg.sender_ethernet_address = _ethernet_address;
        msg.sender_ip_address = _ip_address.ipv4_numeric();
        msg.target_ethernet_address = {0,0,0,0,0,0};
        msg.target_ip_address = it->unknown_ip;

        EthernetFrame arp_frame;
        arp_frame.header().type = EthernetHeader::TYPE_ARP;
        arp_frame.header().src = _ethernet_address;
        arp_frame.header().dst = ETHERNET_BROADCAST;
        arp_frame.payload() = std::move(msg.serialize());

        _frames_out.emplace(std::move(arp_frame));
        it->last_arp_time = 0; //重置arp发送计时器
        ++it;
    }
    return;
}

bool NetworkInterface::_ethernet_address_equal(const EthernetAddress& addr1, const EthernetAddress& addr2) { //判断mac地址相等
    for (int i = 0; i < 6; i++)
        if (addr1[i] != addr2[i])
            return false;
    return true;
}