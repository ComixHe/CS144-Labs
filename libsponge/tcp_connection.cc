#include "tcp_connection.hh"

#include <iostream>

// For Lab 4, please replace with a real implementation that passes the
// automated checks run by `make check`.

size_t TCPConnection::remaining_outbound_capacity() const { return _sender.stream_in().remaining_capacity(); }

size_t TCPConnection::bytes_in_flight() const { return _sender.bytes_in_flight(); }

size_t TCPConnection::unassembled_bytes() const { return _receiver.unassembled_bytes(); }

size_t TCPConnection::time_since_last_segment_received() const { return _time_since_last_segment_reveived; }

bool TCPConnection::active() const { return _active; }

void TCPConnection::segment_received(const TCPSegment &seg) {
    if(!_active) //连接中断
        return;
    _time_since_last_segment_reveived = 0;
    if(!_receiver.ackno().has_value() && _sender.next_seqno_absolute() == 0) { //接收第一个syn
        if(!seg.header().syn)
            return;
        _receiver.segment_received(seg);
        connect(); //建立连接，发送syn/ack
        return;
    }
    if(_sender.next_seqno_absolute() > 0 && 
    _sender.bytes_in_flight() == _sender.next_seqno_absolute() &&
    !_receiver.ackno().has_value()) { //收到第三个syn，以上三个条件分别是：接收了第一个syn,本地目前为止没有发送数据，本地没有
        if(seg.payload().size()) //收到空报文
            return;
        if(!seg.header().ack) { //没有ack则发送空报文并更新ackno和win
            if(seg.header().syn) {
                _receiver.segment_received(seg);
                _sender.send_empty_segment();
            }
            return;
        }
        if(seg.header().rst) { //握手时重置
            _receiver.stream_out().set_error();
            _sender.stream_in().set_error();
            _active = false;
            return;
        }
    }
    //到此接收第三次syn和之后的常规报文
    _receiver.segment_received(seg); 
    _sender.ack_received(seg.header().ackno,seg.header().win);
    if(_sender.stream_in().buffer_empty() && seg.length_in_sequence_space()) //当前没有数据供发送，则发送空报文
        _sender.send_empty_segment();
    if(seg.header().rst) { //收到rst报文开始重置
        _sender.send_empty_segment();
        unexpected_shutdown();
        return;
    }
    sent_segments();
}

size_t TCPConnection::write(const std::string &data) {
    if(!data.size())
        return 0;
    auto write_size = _sender.stream_in().write(data);
    _sender.fill_window();
    sent_segments();
    return write_size;
}

//! \param[in] ms_since_last_tick number of milliseconds since the last call to this method
void TCPConnection::tick(const size_t ms_since_last_tick) {
    if(!_active)
        return;
    _time_since_last_segment_reveived += ms_since_last_tick;
    _sender.tick(ms_since_last_tick);
    if(_sender.consecutive_retransmissions() > TCPConfig::MAX_RETX_ATTEMPTS) //重传达到上限
        unexpected_shutdown(); 
    sent_segments();
}

void TCPConnection::end_input_stream() {
    _sender.stream_in().end_input(); //发送方数据写入完毕，只需发送已写入数据
    _sender.fill_window();
    sent_segments();
}

void TCPConnection::connect() { //双端建立连接
    _sender.fill_window(); //fill_window检测到未开始同步会发送syn报文
    sent_segments(); 
}

void TCPConnection::sent_segments() { //对发送报文进行再处理
    while(!_sender.segments_out().empty()){
        auto& seg = _sender.segments_out().front();
        if(_receiver.ackno().has_value()){
            seg.header().ack = true;
            seg.header().ackno = _receiver.ackno().value();
            seg.header().win = _receiver.window_size();
        }
        _segments_out.emplace(std::move(seg));
        _sender.segments_out().pop();
    }
    try_normal_shutdown(); //每次发完数据尝试关闭。
} 

void TCPConnection::unexpected_shutdown() {  //rst重置
    _receiver.stream_out().set_error();      //1.超过重传时间
    _sender.stream_in().set_error();         //2.连接突然中断
    _active = false;
    auto& seg = _sender.segments_out().front();
    seg.header().ack=true;
    if(_receiver.ackno().has_value())
        seg.header().ackno = _receiver.ackno().value();
    seg.header().win = _receiver.window_size();
    seg.header().rst = true;
    _segments_out.emplace(std::move(seg));
    _sender.segments_out().pop();
}

void TCPConnection::try_normal_shutdown(){ //采用文档中的方案A
    if(_receiver.stream_out().input_ended()){ //检测对端是否发送fin来表明数据发送完毕
        if(!_sender.stream_in().eof()) //sender数据发送完毕且不存在待重传数据
            _linger_after_streams_finish = false;
        else if(_sender.bytes_in_flight() == 0){ //没有发送完毕但不存在待重传数据
            if(!_linger_after_streams_finish || time_since_last_segment_received() >= 10*_cfg.rt_timeout)
                _active = false; //只有流在不存在后续待传输数据或者计时器超时时间超过了十倍的tcpconfig中设置的超时时间
                                  //才正常关闭连接
        }
    }
}

TCPConnection::~TCPConnection() {
    try {
        if (active()) {
            std::cerr << "Warning: Unclean shutdown of TCPConnection\n";
            _sender.send_empty_segment();
            unexpected_shutdown(); //发送rst
        }
    } catch (const std::exception &e) {
        std::cerr << "Exception destructing TCP FSM: " << e.what() << std::endl;
    }
}
