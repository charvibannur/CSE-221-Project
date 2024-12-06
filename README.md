# CSE 221 Project - Benchmarking Project for a Local System

## Overview:
This report uses experimentation to examine how to assess and comprehend a complex operating system. Through benchmarking and performance testing, we want to identify important traits and behaviors of an operating system. The goal is to be able to analyze the behavior of an operating system commonly used today.


To ensure we get accurate results, each team member was responsible for conducting the experiments individually to obtain consistent and repeatable results. This methodology ensures each group member is aware of the concepts and can analyze the behavior accordingly.
To conduct the experiments to measure operating system performance we use C as our language of choice and Apple Clang version 16.0.0 and an arm64-apple-darwin23.5.0. We did not run into any significant issues and spent about 80 hours on the project including drafting the report and understanding experiments.

## Machine Specifications:

The machine we use in this study is a machine with MacOS with an M3 processor, system firmware version of 10151.121.1 with the following specifications:
1. Processor Macbook Air with Apple M3, 8 (4 performance and 4 efficiency)
2. CPU clock speed 4.05 GHz
3. L1 Cache 192+128 KiB per core (Performance Core) and 128+64 KiB per core (Efficiency Core)
4. L2 Cache 16 MB
5. L3 Cache 8 MB (no official data, estimated one)
6. Data Cache 64KB
7. Memory bus bandwidth M3 has a 192-bit memory bus
8. Instruction 64 bit
9. I/O bus T600 I/O bus
10. Disk
    * SDRAM 6,400 MT/s LPDDR5
    * SSD 256GB
    * Disk Speed sequential read speed of 2,280.2 MB/s and a sequential write speed of 2,108.9
12. Network card bandwidth Wi-Fi 6E (802.11ax) and Bluetooth 5.3
13. Operating System MacOS Sonoma version 14.5 (23F79) 10151.121.1

    
We utilize mach_absolute_time() as an accurate measure of time for performing experiments. It is designed for high-resolution timing on macOS, directly returning the time in a hardware-independent format, and is generally unaffected by CPU frequency scaling or background tasks. Unlike some other timing functions, mach_absolute_time() doesn’t require a context switch to the kernel, making it fast and reliable for precise time measurements. Its main limitation is that the raw timing results need to be converted into nanoseconds using a fixed timebase, which could introduce slight imprecision if the timebase were to change during runtime (though this is rare).

## Summary

![image]("https://github.com/user-attachments/assets/c7dba3be-3bf6-401d-bb19-46a96fc304c1")


## Project Members
1. Charvi Bannur
2. Vibha Murthy
3. Harshit Timmanagoudar
