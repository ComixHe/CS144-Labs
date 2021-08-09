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
    if(_window_size == 0 || _sent_fin || (_sent_syn && _stream.buffer_empty() && !_stream.input_ended()))
        return; //分别代表优先重传，发送结束，等待本地流继续写入
    if(_sent_syn == false) {
        //发送syn（握手不携带其他数据）
        _sent_syn = true;
        TCPSegment new_seg;
        new_seg.header().syn = true;
        new_seg.header().seqno = wrap(_next_seqno,_isn);
        ++_next_seqno;
        --_window_size;
        _segments_out.push(new_seg);
        _segments_outstanding.push(new_seg);
    } else if(_stream.eof()) {
        //数据发送完毕，主动关闭连接，发送fin
        TCPSegment new_seg;
        _sent_fin = true;
        new_seg.header().fin = true;
        new_seg.header().seqno = wrap(_next_seqno,_isn);
        ++_next_seqno;
        --_window_size;
        _segments_out.push(new_seg);
        _segments_outstanding.push(new_seg);
    } else {
        //发送一般数据
        while(!_stream.buffer_empty() && _window_size > 0){
            TCPSegment new_seg;
            new_seg.header().seqno = wrap(_next_seqno,_isn);
            auto max_length =  _window_size > TCPConfig::MAX_PAYLOAD_SIZE ? TCPConfig::MAX_PAYLOAD_SIZE : _window_size;
            new_seg.payload() = _stream.read(max_length); //最大化读入
            _window_size -= new_seg.length_in_sequence_space();
            if(_window_size > 0 && _stream.eof()){ //若条件允许，在一次调用中全部读完，则设置fin（fin可携带其他数据）
                _sent_fin = true; //此时，fin算一字节，报文段真实长度需要动态计算。
                --_window_size; 
                new_seg.header().fin = true;
            }
            _next_seqno += new_seg.length_in_sequence_space();
            _segments_out.push(new_seg);
            _segments_outstanding.push(new_seg);
            if(_sent_fin)
                break;
        }
    }

    if(!_running_timer) { //上一个计时器结束后，重新启动计时器，没结束则不执行
        _running_timer = true;
        _time_elapsed = 0;
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
//处理接收方的ack，处理窗口大小和接收确认，判断重传等
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    auto rec_ack = unwrap(ackno,_isn,_send_base);
    if(!_ack_valid(rec_ack))
        return;
    //处理握手syn的ack
    if(_send_base == 0 && rec_ack == 1) { //双端都以自己接到的第一个ack进行处理
        _send_base = 1;
        _time_elapsed = 0;
        _segments_outstanding.pop();
        _retransmission_timeout = _initial_retransmission_timeout;
        _consecutive_retransmissions = 0;
    }else if(_sent_fin && _segments_outstanding.size() == 1 && rec_ack == _next_seqno) { //处理fin的ack
        _send_base += _segments_outstanding.front().length_in_sequence_space();
        _segments_outstanding.pop();
    }else if(!_segments_outstanding.empty() && rec_ack >= _send_base+_segments_outstanding.front().length_in_sequence_space()) {
        //处理普通未确认数据的ack,并且尝试多次确认，if条件中判断收到的ack是否合理
        auto outstanding_seqno = unwrap(_segments_outstanding.front().header().seqno,_isn,_send_base);
        auto outstanding_length = _segments_outstanding.front().length_in_sequence_space();
        while(rec_ack >= outstanding_seqno + outstanding_length){
            _send_base += outstanding_length;
            _segments_outstanding.pop();
            if(_segments_outstanding.empty())
                break;
            outstanding_seqno = unwrap(_segments_outstanding.front().header().seqno,_isn,_send_base);
            outstanding_length = _segments_outstanding.front().length_in_sequence_space();
        }
        _retransmission_timeout = _initial_retransmission_timeout; //全部确认或确认一部分，重设定时器相关状态
        _time_elapsed = 0;
        _consecutive_retransmissions = 0;
    }

    if(bytes_in_flight() == 0)
        _running_timer = false; //全部确认，关闭定时器
    else if(bytes_in_flight() >= window_size) {
        _window_size = 0; //暂停新数据发送，不执行fill_window，优先重传
        return;
    }
    if(window_size == 0) { //当接收端窗口大小为0时，发送方启动零窗口探测以此保证不会因为接收端ack回执丢失带来的死锁
        _window_size = 1;
        _zero_flag = true;//故此时接收端窗口大小看作1
    } else {
        _window_size = window_size;
        _zero_flag = false;
    }
    fill_window();
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick){ //回退N步
    if(!_running_timer)
        return;
    _time_elapsed += ms_since_last_tick;
    if(_time_elapsed >= _retransmission_timeout && !_segments_outstanding.empty()) {
        _segments_out.push(_segments_outstanding.front()); //进入传送队列
        ++_consecutive_retransmissions;
        if(!_zero_flag) //等于0时重传并不是网络拥塞引起，多次syn不用增长rto
            _retransmission_timeout *= 2;
        _time_elapsed = 0;
    }
    return;
}

void TCPSender::send_empty_segment() {
    TCPSegment empty_seg;
    empty_seg.header().seqno = wrap(_next_seqno,_isn);
    empty_seg.payload() = {};
    _segments_out.push(empty_seg);
}

bool TCPSender::_ack_valid(uint64_t abs_ackno) const{ //lab4新加测试，需要验证ackno是否违规
    return abs_ackno <= _next_seqno &&
           abs_ackno >= unwrap(_segments_outstanding.front().header().seqno, _isn, _next_seqno);
}