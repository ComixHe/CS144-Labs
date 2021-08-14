# CS144-Labs

>   Stanford CS144 Lab Assignments ,Fall 2020.
>
>   This is [official website](https://cs144.github.io/)

&nbsp;&nbsp; please switch branches to view the Lab 0 to Lab 7. 

## Lab 5: down the stack(the network interface)

key files: network_interface.hh network_interface.cc webget.cc

**result**

```
make check_lab5                    
--   NOTE: You can choose a build type by calling cmake with one of:
--     -DCMAKE_BUILD_TYPE=Release   -- full optimizations
--     -DCMAKE_BUILD_TYPE=Debug     -- better debugging experience in gdb
--     -DCMAKE_BUILD_TYPE=RelASan   -- full optimizations plus address and undefined-behavior sanitizers
--     -DCMAKE_BUILD_TYPE=DebugASan -- debug plus sanitizers
-- Configuring done
-- Generating done
-- Build files have been written to: /home/comix/CS144/Labs/lab/sponge/build
--   NOTE: You can choose a build type by calling cmake with one of:
--     -DCMAKE_BUILD_TYPE=Release   -- full optimizations
--     -DCMAKE_BUILD_TYPE=Debug     -- better debugging experience in gdb
--     -DCMAKE_BUILD_TYPE=RelASan   -- full optimizations plus address and undefined-behavior sanitizers
--     -DCMAKE_BUILD_TYPE=DebugASan -- debug plus sanitizers
-- Configuring done
-- Generating done
-- Build files have been written to: /home/comix/CS144/Labs/lab/sponge/build
[100%] Testing Lab 5...
Test project /home/comix/CS144/Labs/lab/sponge/build
    Start 31: t_webget
1/2 Test #31: t_webget .........................   Passed    1.60 sec
    Start 32: arp_network_interface
2/2 Test #32: arp_network_interface ............   Passed    0.00 sec

100% tests passed, 0 tests failed out of 2

Total Test time (real) =   1.61 sec
[100%] Built target check_lab5
```

