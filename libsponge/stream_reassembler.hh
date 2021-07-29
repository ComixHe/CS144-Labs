#ifndef SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH
#define SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH

#include "byte_stream.hh"
#include <cstdint>
#include <set>

//! \brief A class that assembles a series of excerpts from a byte stream (possibly out of order,
//! possibly overlapping) into an in-order byte stream.
class StreamReassembler {
  private:
  struct seg
  {
        std::string data;
        uint64_t index;
        seg(std::string s, uint64_t x) : data(std::move(s)), index(x) {}
        bool operator<(const seg& b) const { return this->index < b.index; }
  };

    // Your code here -- add private members as necessary.
    ByteStream _output;  //!< The reassembled in-order byte stream
    uint64_t _capacity;    //!< The maximum number of bytes
    uint64_t _bytes_unassembled;
    std::set<seg> _waiting{};
    bool _eof;
    uint64_t _eof_pos;
    void _insert_waiting(const seg &node);
  
  public:
    //! \brief Construct a `StreamReassembler` that will store up to `capacity` bytes.
    //! \note This capacity limits both the bytes that have been reassembled,
    //! and those that have not yet been reassembled.
    StreamReassembler(const uint64_t capacity);

    //! \brief Receive a substring and write any newly contiguous bytes into the stream.
    //!
    //! The StreamReassembler will stay within the memory limits of the `capacity`.
    //! Bytes that would exceed the capacity are silently discarded.
    //!
    //! \param data the substring
    //! \param index indicates the index (place in sequence) of the first byte in `data`
    //! \param eof the last byte of `data` will be the last byte in the entire stream
    void push_substring(const std::string &data, const uint64_t index, const bool eof);

    //! \name Access the reassembled byte stream
    //!@{
    const ByteStream &stream_out() const { return _output; }
    ByteStream &stream_out() { return _output; }
    //!@}

    //! The number of bytes in the substrings stored but not yet reassembled
    //!
    //! \note If the byte at a particular index has been pushed more than once, it
    //! should only be counted once for the purpose of this function.
    uint64_t unassembled_bytes() const;

    uint64_t first_unassembled() const {return _output.bytes_written();}

    uint64_t first_unread() const {return _output.bytes_read();}

    uint64_t first_unaccept() const {return _output.bytes_read() + _capacity;}

    //! \brief Is the internal state empty (other than the output stream)?
    //! \returns `true` if no substrings are waiting to be assembled
    bool empty() const;
};

#endif  // SPONGE_LIBSPONGE_STREAM_REASSEMBLER_HH