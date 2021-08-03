# CS144-Labs

>   Stanford CS144 Lab Assignments ,Fall 2020.
>
>   This is [official website](https://cs144.github.io/)

&nbsp;&nbsp; please switch branches to view the Lab 0 to Lab 7. 

## Lab 2: The TCP Sender

key files: tcp_sender.cc tcp_sender.hh

**result**

```
make check_lab3
[100%] Testing the TCP sender...
Test project /home/comix/CS144-Labs/build
      Start  1: t_wrapping_ints_cmp
 1/31 Test  #1: t_wrapping_ints_cmp ..............   Passed    0.00 sec
      Start  2: t_wrapping_ints_unwrap
 2/31 Test  #2: t_wrapping_ints_unwrap ...........   Passed    0.00 sec
      Start  3: t_wrapping_ints_wrap
 3/31 Test  #3: t_wrapping_ints_wrap .............   Passed    0.00 sec
      Start  4: t_recv_connect
 4/31 Test  #4: t_recv_connect ...................   Passed    0.00 sec
      Start  5: t_recv_transmit
 5/31 Test  #5: t_recv_transmit ..................   Passed    0.04 sec
      Start  6: t_recv_window
 6/31 Test  #6: t_recv_window ....................   Passed    0.00 sec
      Start  7: t_recv_reorder
 7/31 Test  #7: t_recv_reorder ...................   Passed    0.00 sec
      Start  8: t_recv_close
 8/31 Test  #8: t_recv_close .....................   Passed    0.00 sec
      Start  9: t_send_connect
 9/31 Test  #9: t_send_connect ...................   Passed    0.00 sec
      Start 10: t_send_transmit
10/31 Test #10: t_send_transmit ..................   Passed    0.02 sec
      Start 11: t_send_retx
11/31 Test #11: t_send_retx ......................   Passed    0.00 sec
      Start 12: t_send_window
12/31 Test #12: t_send_window ....................   Passed    0.01 sec
      Start 13: t_send_ack
13/31 Test #13: t_send_ack .......................   Passed    0.00 sec
      Start 14: t_send_close
14/31 Test #14: t_send_close .....................   Passed    0.00 sec
      Start 15: t_strm_reassem_cap
15/31 Test #15: t_strm_reassem_cap ...............   Passed    0.05 sec
      Start 16: t_strm_reassem_single
16/31 Test #16: t_strm_reassem_single ............   Passed    0.00 sec
      Start 17: t_strm_reassem_seq
17/31 Test #17: t_strm_reassem_seq ...............   Passed    0.00 sec
      Start 18: t_strm_reassem_dup
18/31 Test #18: t_strm_reassem_dup ...............   Passed    0.00 sec
      Start 19: t_strm_reassem_holes
19/31 Test #19: t_strm_reassem_holes .............   Passed    0.00 sec
      Start 20: t_strm_reassem_many
20/31 Test #20: t_strm_reassem_many ..............   Passed    0.05 sec
      Start 21: t_strm_reassem_overlapping
21/31 Test #21: t_strm_reassem_overlapping .......   Passed    0.00 sec
      Start 22: t_strm_reassem_win
22/31 Test #22: t_strm_reassem_win ...............   Passed    0.05 sec
      Start 23: t_byte_stream_construction
23/31 Test #23: t_byte_stream_construction .......   Passed    0.00 sec
      Start 24: t_byte_stream_one_write
24/31 Test #24: t_byte_stream_one_write ..........   Passed    0.00 sec
      Start 25: t_byte_stream_two_writes
25/31 Test #25: t_byte_stream_two_writes .........   Passed    0.00 sec
      Start 26: t_byte_stream_capacity
26/31 Test #26: t_byte_stream_capacity ...........   Passed    0.26 sec
      Start 27: t_byte_stream_many_writes
27/31 Test #27: t_byte_stream_many_writes ........   Passed    0.00 sec
      Start 28: t_webget
28/31 Test #28: t_webget .........................   Passed    1.24 sec
      Start 50: t_address_dt
29/31 Test #50: t_address_dt .....................   Passed    0.02 sec
      Start 51: t_parser_dt
30/31 Test #51: t_parser_dt ......................   Passed    0.00 sec
      Start 52: t_socket_dt
31/31 Test #52: t_socket_dt ......................   Passed    0.00 sec

100% tests passed, 0 tests failed out of 31

Total Test time (real) =   1.80 sec
[100%] Built target check_lab3
```