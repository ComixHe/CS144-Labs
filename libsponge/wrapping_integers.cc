#include "wrapping_integers.hh"

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

//! Transform an "absolute" 64-bit sequence number (zero-indexed) into a WrappingInt32
//! \param n The input absolute 64-bit sequence number
//! \param isn The initial sequence number
WrappingInt32 wrap(uint64_t n, WrappingInt32 isn) { return isn + (n % (1UL << 32)); } //除去回环次数即得偏移量

//! Transform a WrappingInt32 into an "absolute" 64-bit sequence number (zero-indexed)
//! \param n The relative sequence number
//! \param isn The initial sequence number
//! \param checkpoint A recent absolute 64-bit sequence number
//! \returns the 64-bit sequence number that wraps to `n` and is closest to `checkpoint`
//!
//! \note Each of the two streams of the TCP connection has its own ISN. One stream
//! runs from the local TCPSender to the remote TCPReceiver and has one ISN,
//! and the other stream runs from the remote TCPSender to the local TCPReceiver and
//! has a different ISN.
uint64_t unwrap(WrappingInt32 n, WrappingInt32 isn, uint64_t checkpoint) { //checkpoint: 本地流中最后一个重组字节的index
    auto distance = n - wrap(checkpoint,isn); //放入同一范围内计算,计算两点之间距离，正右负左
    int64_t location = checkpoint + distance; //计算n真正的位置，n有可能为负（溢出），则distance为负
    return location >= 0 ? location : location + (1UL << 32); //此时locationwei，所以要加上2^32。
} //另一种通过两个点定位n的比这个复杂，不采用