#include "tcp_receiver.hh"

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

void TCPReceiver::segment_received(const TCPSegment &seg) {
    auto header = seg.header();
    if(header.syn) //开始同步，设定isn
        _isn = WrappingInt32(header.seqno); 
    if(!_isn.has_value())
        return;
    auto checkpoint = _reassembler.stream_out().bytes_written();
    auto abs_seqno = unwrap(header.seqno,_isn.value(),checkpoint); //获取接收seg的流中绝对序列号
    _reassembler.push_substring(seg.payload().copy(),header.syn?0:abs_seqno - 1,header.fin);
    //流中绝对索引值需要排除syn。
}

std::optional<WrappingInt32> TCPReceiver::ackno() const {
    if(!_isn.has_value())
        return std::nullopt; //没有开始则返回空
    if(_reassembler.stream_out().input_ended()) //结束则偏移两字节，ackno代表已确认接收数，故不能简单判断是否接收fin
        return WrappingInt32(wrap(_reassembler.stream_out().bytes_written() + 2,_isn.value()));
    return WrappingInt32(wrap(_reassembler.stream_out().bytes_written() + 1,_isn.value())); //没结束则偏移一字节
}

size_t TCPReceiver::window_size() const { return _reassembler.first_unaccept() - _reassembler.first_unassembled(); } //接收窗口大小