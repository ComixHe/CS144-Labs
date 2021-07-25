# CS144-Labs

>   Stanford CS144 Lab Assignments ,Fall 2020.
>
>   This is [official website](https://cs144.github.io/)

&nbsp;&nbsp; please switch branches to view the Lab 0 to Lab 7. 

## Lab 1:  stitching substrings into a byte stream

key files: stream_reassembler.cc stream_reassembler.hh

**result**

```
make check_lab1
[100%] Testing the stream reassembler...
Test project /home/comix/CS144-Labs/build
      Start 15: t_strm_reassem_single
 1/16 Test #15: t_strm_reassem_single ............   Passed    0.00 sec
      Start 16: t_strm_reassem_seq
 2/16 Test #16: t_strm_reassem_seq ...............   Passed    0.00 sec
      Start 17: t_strm_reassem_dup
 3/16 Test #17: t_strm_reassem_dup ...............   Passed    0.00 sec
      Start 18: t_strm_reassem_holes
 4/16 Test #18: t_strm_reassem_holes .............   Passed    0.00 sec
      Start 19: t_strm_reassem_many
 5/16 Test #19: t_strm_reassem_many ..............   Passed    0.05 sec
      Start 20: t_strm_reassem_overlapping
 6/16 Test #20: t_strm_reassem_overlapping .......   Passed    0.00 sec
      Start 21: t_strm_reassem_win
 7/16 Test #21: t_strm_reassem_win ...............   Passed    0.05 sec
      Start 22: t_strm_reassem_cap
 8/16 Test #22: t_strm_reassem_cap ...............   Passed    0.05 sec
      Start 23: t_byte_stream_construction
 9/16 Test #23: t_byte_stream_construction .......   Passed    0.00 sec
      Start 24: t_byte_stream_one_write
10/16 Test #24: t_byte_stream_one_write ..........   Passed    0.00 sec
      Start 25: t_byte_stream_two_writes
11/16 Test #25: t_byte_stream_two_writes .........   Passed    0.00 sec
      Start 26: t_byte_stream_capacity
12/16 Test #26: t_byte_stream_capacity ...........   Passed    0.26 sec
      Start 27: t_byte_stream_many_writes
13/16 Test #27: t_byte_stream_many_writes ........   Passed    0.00 sec
      Start 50: t_address_dt
14/16 Test #50: t_address_dt .....................   Passed    0.03 sec
      Start 51: t_parser_dt
15/16 Test #51: t_parser_dt ......................   Passed    0.00 sec
      Start 52: t_socket_dt
16/16 Test #52: t_socket_dt ......................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 16

Total Test time (real) =   0.46 sec
[100%] Built target check_lab1
```