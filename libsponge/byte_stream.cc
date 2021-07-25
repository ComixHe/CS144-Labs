#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

ByteStream::ByteStream(const size_t capacity):_capacity(capacity) {}

size_t ByteStream::write(const std::string &data) {
    size_t write_count {0};
    for(auto& c : data)
    {
        if(remaining_capacity() == 0)
            break;
        _stream.push_back(c);
        ++_bytes_written;
        ++write_count;
    }
    return write_count;
}

//! \param[in] len bytes will be copied from the output side of the buffer
std::string ByteStream::peek_output(const size_t len) const {
    const auto peek_length = len > buffer_size() ? buffer_size() : len;
    auto str_b = _stream.cbegin();
    auto str_e {str_b};
    std::advance(str_e,peek_length);
    return std::string(str_b,str_e);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    auto pop_length = len > buffer_size() ? buffer_size() : len;
    _bytes_read += pop_length;
    while(pop_length--)
        _stream.pop_front();
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    auto read_str = peek_output(len);
    pop_output(len);
    return read_str;
}

void ByteStream::end_input() { _end = true; }

bool ByteStream::input_ended() const { return _end; }

size_t ByteStream::buffer_size() const { return _stream.size(); }

bool ByteStream::buffer_empty() const { return _stream.empty(); }

bool ByteStream::eof() const { return _end && buffer_empty(); }

size_t ByteStream::bytes_written() const { return _bytes_written; }

size_t ByteStream::bytes_read() const { return _bytes_read; }

size_t ByteStream::remaining_capacity() const { return _capacity - buffer_size(); }
