#include "stream_reassembler.hh"

StreamReassembler::StreamReassembler(const uint64_t capacity): _output(capacity),_capacity(capacity),
                                                            _bytes_unassembled(0),_eof(false),_eof_pos(0) {}

void StreamReassembler::_insert_waiting(const seg& node){
    if (_waiting.empty()) //重组区为空直接插入
    {
        _waiting.insert(node);
        _bytes_unassembled += node.data.length();
        return;
    }

    seg tmp = node;
    auto it = _waiting.lower_bound(node); //定位node在重组区的位置
    auto var_index = tmp.index, var_length = tmp.data.length();
    if (it != _waiting.begin()) //若node不为首节点则判断是否能向左合并
    {
        --it;   
        if (var_index < it->index + it->data.length() ) //相交或被包含
        {     
            if (var_index + var_length <= it->index + it->data.length())      //被包含不处理
                return;
            tmp.data = it->data + tmp.data.substr(it->index + it->data.length() - var_index); //相交处理
            tmp.index = it->index;
            var_index = tmp.index; var_length = tmp.data.length();
            _bytes_unassembled -= it->data.length();
            it = _waiting.erase(it); //合并后移除原节点
        } 
        else
            ++it;
    }
    while (it != _waiting.end() && var_index + var_length > it->index) //判断是否能向右合并，可能出现连续合并情况
    {
        if (var_index >= it->index && var_index + var_length < it->index + it->data.length()) //被包含则直接丢弃
            return;
        if (var_index + var_length < it->index + it->data.length()) //相交处理
            tmp.data += it->data.substr(var_index + var_length - it->index);
        _bytes_unassembled -= it->data.length();   
        it = _waiting.erase(it);    //合并完成移除节点
    }
    _waiting.insert(tmp);   //合并完成后插入重排区，也有可能没有合并操作发生
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

    if (index + data.length() < first_unassembled || index >= first_unaccept)  //超出范围直接丢弃
        return;
    if (index + data.length() > first_unaccept)
        new_seg.data = new_seg.data.substr(0, first_unaccept - index); //部分超出做截断处理
    if (index <= first_unassembled) //若新字串可以直接写入bytestream
    {   
        _output.write(new_seg.data.substr(first_unassembled - index));
        auto it = _waiting.begin();
        while (it->index <= _output.bytes_written() && it!=_waiting.end())  //检查重排区是否能继续写入
        {
            if (it->index + it->data.length() > new_seg.index + new_seg.data.length()) 
                _output.write(it->data.substr(_output.bytes_written() - it->index));
            _bytes_unassembled -= it->data.length();
            it = _waiting.erase(it);
        }
    } 
    else
        _insert_waiting(new_seg);    //不能直接写入则待重排

    if (eof) 
    {
        _eof = true;
        _eof_pos = index + data.length(); //确定流传输数量，以免还有数据在重排区却以为读取完毕
    }
    if (_eof && _output.bytes_written() == _eof_pos) //已经读取完毕，通知bytestream
        _output.end_input();   
}

uint64_t StreamReassembler::unassembled_bytes() const { return _bytes_unassembled; }

bool StreamReassembler::empty() const { return _bytes_unassembled == 0; }