# CS144-Labs

>   Stanford CS144 Lab Assignments ,Fall 2020.
>
>   This is [official website](https://cs144.github.io/)

&nbsp;&nbsp; please switch branches to view the Lab 0 to Lab 7. 

## Lab 0: network warmup

key files: webget.cc byte_stream.hh byte_stream.cc

**result**

```
make check_lab0   
[100%] **Testing Lab 0...**  
Test project /home/comix/CS144/Labs/lab0/sponge/build  
   Start 23: t_byte_stream_construction   
1/9 Test #23: t_byte_stream_construction .......  Passed   0.00 sec   
   Start 24: t_byte_stream_one_write   
2/9 Test #24: t_byte_stream_one_write ..........  Passed   0.00 sec   
   Start 25: t_byte_stream_two_writes   
3/9 Test #25: t_byte_stream_two_writes .........  Passed   0.00 sec     
   Start 26: t_byte_stream_capacity   
4/9 Test #26: t_byte_stream_capacity ...........  Passed   0.24 sec   
   Start 27: t_byte_stream_many_writes   
5/9 Test #27: t_byte_stream_many_writes ........  Passed   0.00 sec   
   Start 28: t_webget   
6/9 Test #28: t_webget .........................  Passed   1.11 sec   
   Start 50: t_address_dt   
7/9 Test #50: t_address_dt .....................  Passed   0.03 sec   
   Start 51: t_parser_dt   
8/9 Test #51: t_parser_dt ......................  Passed   0.01 sec   
   Start 52: t_socket_dt   
9/9 Test #52: t_socket_dt ......................  Passed   0.01 sec    
100% tests passed, 0 tests failed out of 9    
Total Test time (real) =  1.41 sec   
[100%] Built target check_lab0
```