#include "tcp_sender.hh"
#include "tcp_config.hh"
#include <random>

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

/** tcpreciver仅仅只对数据进行处理，表明当前状态，超时重传等具体操作在lab4中实现。
 * 在本实现中的重传类似于回退N步而不是选择重传，所以windos_size为1代表接收方接收窗口为0，
 * 为0时代表发送方重发队列长度大于接收方接收窗口，故无法发送新数据。
 **/
//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _initial_retransmission_timeout{retx_timeout}
    , _retransmission_timeout{_initial_retransmission_timeout}
    , _isn(fixed_isn.value_or(WrappingInt32{std::random_device()()}))
    , _stream(capacity) {}

void TCPSender::fill_window() { //策略是一次性尽可能发送更多数据
    if(!_sent_syn){
        TCPSegment new_seg;
        new_seg.header().syn = true;
        process_segment(new_seg);
        _sent_syn = true;
        return;
    }
    size_t real_windows_size = _zero_flag ? 1 : _window_size; //零检测文档要求，且不发会造成双方互相等报文的情况
    auto free_space = real_windows_size - (_next_seqno - _send_base);
    while(free_space != 0 && !_sent_fin){
        auto real_size = std::min(free_space,TCPConfig::MAX_PAYLOAD_SIZE);
        TCPSegment output_seg;
        auto content = _stream.read(real_size);
        free_space -= content.length();
        output_seg.payload() = std::move(Buffer(std::move(content)));
        if(_stream.eof() && output_seg.length_in_sequence_space() < real_windows_size){
            output_seg.header().fin = true; //上层写入完毕（流eof）且当前接收窗口没满则附带fin（或单独发送空载fin）
            _sent_fin = true;
        }
        if(output_seg.length_in_sequence_space() == 0)
            break;
        process_segment(output_seg);
    }
    return;
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
//处理接收方的ack，处理窗口大小和接收确认，判断重传等
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    auto rec_ack = unwrap(ackno,_isn,_send_base);
    if(rec_ack > _next_seqno) //超出边界丢弃
        return;
    _window_size = window_size;
    if(_window_size == 0)
        _zero_flag = true;
    else
        _zero_flag = false;
    if(rec_ack <= _send_base) //已经接收过了，但可能是对方发送的零探测窗口，故更新窗口大小在它之前
        return;
    _send_base = rec_ack;

    while(!_segments_outstanding.empty()){//确认接收
        auto& seg = _segments_outstanding.front();
        if(unwrap(seg.header().seqno,_isn,_send_base) + seg.length_in_sequence_space() <= rec_ack){
            _bytes_in_flight -= seg.length_in_sequence_space();
            _segments_outstanding.pop();
        }
        else
            break;
    }
    fill_window(); //返回ack

    _retransmission_timeout = _initial_retransmission_timeout;
    _consecutive_retransmissions = 0;

    if(!_segments_outstanding.empty()){
        _running_timer = true;
        _time_elapsed = 0;
    }
    return;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick){ //回退N步
    if(!_running_timer)
        return;
    _time_elapsed += ms_since_last_tick;
    if(_time_elapsed >= _retransmission_timeout && !_segments_outstanding.empty()) {
        _segments_out.emplace(_segments_outstanding.front()); //进入传送队列
        ++_consecutive_retransmissions;
        if(!_zero_flag) //等于0时重传并不是网络拥塞引起，多次syn不用增长rto
            _retransmission_timeout *= 2;
        _running_timer = true; //开始重传
        _time_elapsed = 0;
    }
    if(_segments_outstanding.empty())
        _running_timer = false;
    return;
}

void TCPSender::send_empty_segment() { //不用进入重传队列
    TCPSegment empty_seg;
    empty_seg.header().seqno = wrap(_next_seqno,_isn);
    empty_seg.payload() = {};
    _segments_out.emplace(std::move(empty_seg));
    return;
}

void TCPSender::process_segment(TCPSegment &seg) {
    seg.header().seqno = wrap(_next_seqno,_isn);
    _next_seqno += seg.length_in_sequence_space();
    _bytes_in_flight += seg.length_in_sequence_space();
    _segments_out.emplace(seg);
    _segments_outstanding.emplace(std::move(seg));
    if(!_running_timer) { //上一个计时器结束后，重新启动计时器，没结束则不执行
        _running_timer = true;
        _time_elapsed = 0;
    }
    return;
}