Cache/Memory Controller
This project is part of the ECE M116C Computer Architecture Systems course at UCLA. The goal is to design a cache/memory controller 
that efficiently manages data storage and retrieval within a computer system.

Overview
In this project, we are tasked with designing a memory controller that interfaces with both a processor and memory to perform load 
(LW) and store (SW) operations. This involves managing a two-level cache hierarchy and the main memory, ensuring data is stored and 
retrieved effectively.

Project Description
The memory controller takes a 32-bit address as input. Depending on the operation (LW or SW), it either loads data from memory or 
stores data at the specified address. Initial skeleton code (memory_driver.cpp) is provided, which handles variable initialization, 
cache object creation, main memory array management, trace reading, and passing information to the memory hierarchy. This skeleton 
can be modified as needed.

Memory Hierarchy
L1 Cache: A direct-mapped cache with 16 lines/sets and 1 block per line. Each block is 4 bytes. The implementation requires correct 
calculation of index, block offset, and tag.

L2 Cache: An 8-way set-associative cache with 16 sets and 1 block per line (4 bytes per block). It necessitates the computation of 
tag, index, and block offset, similar to L1.

Main Memory: Consists of 4096 lines, each 4 bytes in size.

Cache Policies
The L1 cache operates without a replacement policy due to its direct-mapped nature.
The L2 cache uses a Least Recently Used (LRU) replacement policy, suitable for its 8-way set-associative configuration.
The cache design follows an exclusive/write-no-allocate/write-through policy, with all valid bits initially set to zero.
Objectives
Design and implement a memory controller that efficiently manages a two-level cache hierarchy and main memory for LW and SW operations.
Apply computer architecture principles to calculate index, block offset, and tag for cache operations.
Implement and manage cache policies, including an LRU replacement policy for the L2 cache.

Contributions
This assignment is designed to challenge your understanding of computer architecture and cache memory systems. Contributions to enhance the 
provided code or improve the efficiency of the memory controller are welcome.
