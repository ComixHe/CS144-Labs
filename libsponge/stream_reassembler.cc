#include "stream_reassembler.hh"

StreamReassembler::StreamReassembler(const uint64_t capacity): _output(capacity),_capacity(capacity),
                                                            _bytes_unassembled(0),_eof(false),_eof_pos(0) {}

void StreamReassembler::_insert_waiting(const seg& node){
    if (_waiting.empty()) 
    {
        _waiting.insert(node);
        _bytes_unassembled += node.data.length();
        return;
    }

    seg tmp = node;
    auto it = _waiting.lower_bound(node); 
    auto var_index = tmp.index, var_length = tmp.data.length();
    if (it != _waiting.begin()) 
    {
        --it;   
        if (var_index < it->index + it->data.length() ) 
        {     
            if (var_index + var_length <= it->index + it->data.length())      
                return;
            tmp.data = it->data + tmp.data.substr(it->index + it->data.length() - var_index);
            tmp.index = it->index;
            var_index = tmp.index; var_length = tmp.data.length();
            _bytes_unassembled -= it->data.length();
            it = _waiting.erase(it);
        } 
        else
            ++it;
    }
    while (it != _waiting.end() && var_index + var_length > it->index) 
    {
        if (var_index >= it->index && var_index + var_length < it->index + it->data.length()) 
            return;
        if (var_index + var_length < it->index + it->data.length())
            tmp.data += it->data.substr(var_index + var_length - it->index);
        _bytes_unassembled -= it->data.length();   
        it = _waiting.erase(it);
    }
    _waiting.insert(tmp);      
    _bytes_unassembled += tmp.data.length();
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const std::string &data, const uint64_t index, const bool eof) {
    seg new_seg{data, index};
    uint64_t first_unread = _output.bytes_read();
    uint64_t first_unassembled = _output.bytes_written();
    uint64_t first_unaccept = first_unread + _capacity;

    if (index + data.length() < first_unassembled || index >= first_unaccept)  
        return;
    if (index + data.length() > first_unaccept)
        new_seg.data = new_seg.data.substr(0, first_unaccept - index);
    if (index <= first_unassembled) 
    {   
        _output.write(new_seg.data.substr(first_unassembled - index));
        auto it = _waiting.begin();
        while (it->index <= _output.bytes_written() && it!=_waiting.end()) 
        {
            if (it->index + it->data.length() > new_seg.index + new_seg.data.length()) 
                _output.write(it->data.substr(_output.bytes_written() - it->index));
            _bytes_unassembled -= it->data.length();
            it = _waiting.erase(it);
        }
    } 
    else
        _insert_waiting(new_seg);    

    if (eof) 
    {
        _eof = true;
        _eof_pos = index + data.length(); 
    }
    if (_eof && _output.bytes_written() == _eof_pos)
        _output.end_input();   
}

uint64_t StreamReassembler::unassembled_bytes() const { return _bytes_unassembled; }

bool StreamReassembler::empty() const { return _bytes_unassembled == 0; }