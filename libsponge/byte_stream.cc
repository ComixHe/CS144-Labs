#include "byte_stream.hh"
#include <iostream>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

ByteStream::ByteStream(const size_t capacity):_capacity(capacity) {}

size_t ByteStream::write(const std::string &data) {
    size_t write_count = data.size();
    if(write_count > _capacity - _stream.size())
        write_count = _capacity - _stream.size();
    _bytes_written += write_count;
    _stream.append(BufferList{data.substr(0,write_count)});
    return write_count;
}

//! \param[in] len bytes will be copied from the output side of the buffer
std::string ByteStream::peek_output(const size_t len) const {
    const auto peek_length = len > buffer_size() ? buffer_size() : len;
    return _stream.concatenate().substr(0,peek_length);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    auto pop_length = len > buffer_size() ? buffer_size() : len;
    _bytes_read += pop_length;
    _stream.remove_prefix(pop_length);
    return;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    auto read_str = peek_output(len);
    pop_output(len);
    return read_str;
}

void ByteStream::end_input() { _input_end = true; }

bool ByteStream::input_ended() const { return _input_end; }

size_t ByteStream::buffer_size() const { return _stream.size(); }

bool ByteStream::buffer_empty() const { return _stream.size() == 0; }

bool ByteStream::eof() const { return input_ended() && buffer_empty(); }

size_t ByteStream::bytes_written() const { return _bytes_written; }

size_t ByteStream::bytes_read() const { return _bytes_read; }

size_t ByteStream::remaining_capacity() const { return _capacity - buffer_size(); }