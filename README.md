# CS144-Labs

>   Stanford CS144 Lab Assignments ,Fall 2020.
>
>   This is [official website](https://cs144.github.io/)

&nbsp;&nbsp; please switch branches to view the Lab 0 to Lab 7. 

## Lab 2: The TCP Receiver

key files: wrapping_integers.cc wrapping_integers.hh tcp_reciver.cc tcp_receiver.hh

**result**

```
make check_lab2
[100%] Testing the TCP receiver...
Test project /home/comix/CS144-Labs/build
      Start  1: t_wrapping_ints_cmp
 1/25 Test  #1: t_wrapping_ints_cmp ..............   Passed    0.00 sec
      Start  2: t_wrapping_ints_unwrap
 2/25 Test  #2: t_wrapping_ints_unwrap ...........   Passed    0.00 sec
      Start  3: t_wrapping_ints_wrap
 3/25 Test  #3: t_wrapping_ints_wrap .............   Passed    0.00 sec
      Start  4: t_recv_connect
 4/25 Test  #4: t_recv_connect ...................   Passed    0.00 sec
      Start  5: t_recv_transmit
 5/25 Test  #5: t_recv_transmit ..................   Passed    0.03 sec
      Start  6: t_recv_window
 6/25 Test  #6: t_recv_window ....................   Passed    0.00 sec
      Start  7: t_recv_reorder
 7/25 Test  #7: t_recv_reorder ...................   Passed    0.00 sec
      Start  8: t_recv_close
 8/25 Test  #8: t_recv_close .....................   Passed    0.00 sec
      Start 15: t_strm_reassem_cap
 9/25 Test #15: t_strm_reassem_cap ...............   Passed    0.05 sec
      Start 16: t_strm_reassem_single
10/25 Test #16: t_strm_reassem_single ............   Passed    0.00 sec
      Start 17: t_strm_reassem_seq
11/25 Test #17: t_strm_reassem_seq ...............   Passed    0.00 sec
      Start 18: t_strm_reassem_dup
12/25 Test #18: t_strm_reassem_dup ...............   Passed    0.00 sec
      Start 19: t_strm_reassem_holes
13/25 Test #19: t_strm_reassem_holes .............   Passed    0.00 sec
      Start 20: t_strm_reassem_many
14/25 Test #20: t_strm_reassem_many ..............   Passed    0.05 sec
      Start 21: t_strm_reassem_overlapping
15/25 Test #21: t_strm_reassem_overlapping .......   Passed    0.00 sec
      Start 22: t_strm_reassem_win
16/25 Test #22: t_strm_reassem_win ...............   Passed    0.05 sec
      Start 23: t_byte_stream_construction
17/25 Test #23: t_byte_stream_construction .......   Passed    0.00 sec
      Start 24: t_byte_stream_one_write
18/25 Test #24: t_byte_stream_one_write ..........   Passed    0.00 sec
      Start 25: t_byte_stream_two_writes
19/25 Test #25: t_byte_stream_two_writes .........   Passed    0.00 sec
      Start 26: t_byte_stream_capacity
20/25 Test #26: t_byte_stream_capacity ...........   Passed    0.27 sec
      Start 27: t_byte_stream_many_writes
21/25 Test #27: t_byte_stream_many_writes ........   Passed    0.00 sec
      Start 28: t_webget
22/25 Test #28: t_webget .........................   Passed    1.13 sec
      Start 50: t_address_dt
23/25 Test #50: t_address_dt .....................   Passed    0.03 sec
      Start 51: t_parser_dt
24/25 Test #51: t_parser_dt ......................   Passed    0.00 sec
      Start 52: t_socket_dt
25/25 Test #52: t_socket_dt ......................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 25

Total Test time (real) =   1.65 sec
[100%] Built target check_lab2
```