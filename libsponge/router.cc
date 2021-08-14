#include "router.hh"

#include <iostream>

// Dummy implementation of an IP router

// Given an incoming Internet datagram, the router decides
// (1) which interface to send it out on, and
// (2) what next hop address to send it to.

// For Lab 6, please replace with a real implementation that passes the
// automated checks run by `make check_lab6`.

// You will need to add private members to the class declaration in `router.hh`

//! \param[in] route_prefix The "up-to-32-bit" IPv4 address prefix to match the datagram's destination address against
//! \param[in] prefix_length For this route to be applicable, how many high-order (most-significant) bits of the route_prefix will need to match the corresponding bits of the datagram's destination address?
//! \param[in] next_hop The IP address of the next hop. Will be empty if the network is directly attached to the router (in which case, the next hop address should be the datagram's final destination).
//! \param[in] interface_num The index of the interface to send the datagram out on.
void Router::add_route(const uint32_t route_prefix,
                       const uint8_t prefix_length,
                       const std::optional<Address> next_hop,
                       const size_t interface_num) {
    std::cerr << "DEBUG: adding route " << Address::from_ipv4_numeric(route_prefix).ip() << "/" << int(prefix_length)
         << " => " << (next_hop.has_value() ? next_hop->ip() : "(direct)") << " on interface " << interface_num << "\n";
    _router_table.emplace_back(route_rule{prefix_length,route_prefix,interface_num,next_hop});
}

//! \param[in] dgram The datagram to be routed
void Router::route_one_datagram(InternetDatagram &dgram) {
    if(dgram.header().ttl <= 1) //ttl到期则抛弃
        return;
    --dgram.header().ttl; 
    bool route_rule_exist{false};
    auto dst_ip = dgram.header().dst;
    route_rule match_rule;
    for(auto& it : _router_table){
        if(prefix_equal(dst_ip,it.route_prefix,it.prefix_length)){ //最长前缀匹配
            if(!route_rule_exist || match_rule.prefix_length < it.prefix_length){
                match_rule = it;
                route_rule_exist = true;
            }
        }
    }

    if(!route_rule_exist) //找不到则丢弃
        return;
    if(match_rule.next_hop.has_value())
        _interfaces[match_rule.interface_num].send_datagram(dgram,match_rule.next_hop.value()); //下一跳有值则进行间接交付，转发到对应接口上
    else
        _interfaces[match_rule.interface_num].send_datagram(dgram,Address::from_ipv4_numeric(dgram.header().dst)); //下一跳没有值则直接交付
}

void Router::route() {
    // Go through all the interfaces, and route every incoming datagram to its proper outgoing interface.
    for (auto &interface : _interfaces) {
        auto &queue = interface.datagrams_out();
        while (not queue.empty()) {
            route_one_datagram(queue.front());
            queue.pop();
        }
    }
}

bool Router::prefix_equal(uint32_t pre_a, uint32_t pre_b, uint8_t mask_bit) { //前缀匹配检查
    uint32_t offset = mask_bit==0 ? 0 : (0xffffffff << (32 - mask_bit));
    return (pre_a&offset) == (pre_b&offset);
}