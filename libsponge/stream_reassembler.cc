#include "stream_reassembler.hh"
#include <iostream>

StreamReassembler::StreamReassembler(const uint64_t capacity): _output(capacity),_capacity(capacity){}

void StreamReassembler::_insert_waiting(seg& new_seg){
    if (_waiting.empty()) //为空直接插入
    {
        _bytes_unassembled += new_seg.data.length();
        _waiting.emplace(std::move(new_seg));
        return;
    }

    seg tmp{std::move(new_seg)};
    auto it = _waiting.lower_bound(tmp); 
    auto var_index = tmp.begin_index, var_length = tmp.data.length();
    if (it != _waiting.begin()) //尝试向左合并
    {
        --it;
        if (var_index <= it->begin_index + it->data.length() ) 
        {     
            if (var_index + var_length <= it->begin_index + it->data.length())//被左边的报文段完全包含     
                return;
            tmp.data = it->data + tmp.data.substr(it->begin_index + it->data.length() - var_index);//删除重叠部分
            tmp.begin_index = it->begin_index;
            var_index = tmp.begin_index;
            var_length = tmp.data.length();
            _bytes_unassembled -= it->data.length();
            it = _waiting.erase(it);
        } 
        else
            ++it;
    }
    while (it != _waiting.end() && var_index + var_length >= it->begin_index)//尝试向右连续合并
    {
        if (var_index >= it->begin_index && var_index + var_length <= it->begin_index + it->data.length())  //被右边的报文完全包含
            return;
        if (it->begin_index <= var_index + var_length && it->begin_index + it->data.length() > var_index + var_length)
            tmp.data += it->data.substr(var_index + var_length - it->begin_index);
        _bytes_unassembled -= it->data.length();   
        it = _waiting.erase(it);
    }
    _bytes_unassembled += tmp.data.length();
    _waiting.emplace(std::move(tmp));
    return;
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const std::string &data, const uint64_t index, const bool eof) {
    seg new_seg{data,index};
    auto _first_unread = _output.bytes_read();
    auto _first_unassembled = _first_unread + _output.buffer_size();
    auto _first_unacceptable = _first_unread + _capacity;
    if(data.empty())
        goto END;
    if(index >= _first_unacceptable || index + data.length() <= _first_unassembled) //完全超出边界故丢弃
        return;
    if (index + data.length() > _first_unacceptable)//减去超出部分
        new_seg.data = std::move(new_seg.data.substr(0, _first_unacceptable - index));
    if (index <= _first_unassembled) 
    {   
        auto tmp_string{std::move(new_seg.data.substr(_first_unassembled - index))};
        _output.write(tmp_string); //减去多余部分后写入
        _first_unassembled += tmp_string.length();
        auto it = _waiting.begin();
        while (it->begin_index <= _first_unassembled && it!=_waiting.end())  //尝试把重组部分写入输出流
        {
            if(it->begin_index + it->data.length() >= _first_unassembled){
                tmp_string = std::move(it->data.substr(_first_unassembled - it->begin_index));
                _output.write(tmp_string);
                _first_unassembled += tmp_string.length(); 
            }
            _bytes_unassembled -= it->data.length();
            it = _waiting.erase(it);
        }
    } 
    else
        _insert_waiting(new_seg);//等待重组
END:
    if (eof){
        _eof_flag = true;
        _eof_pos = index + data.length();
    }
    if (_eof_flag && _output.bytes_written() == _eof_pos)//收到关闭信号且最后一个字节已写入（可能有丢弃的所以没有全写入）
        _output.end_input();
    return;
}

uint64_t StreamReassembler::unassembled_bytes() const { return _bytes_unassembled; }

bool StreamReassembler::empty() const { return _bytes_unassembled == 0; }