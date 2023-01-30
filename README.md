# Malloc
Malloc is a wrapper around the mmap and munmap system functions. It allows a better management of memory to prevent costly syscalls.
This malloc works as follows :
- It first search for an existing freed block in already allocated heaps
- If no blocks are big enough for the requested size, it searches through the already allocated heaps to find one with enough free space or request a new heap using mmap.
- Then it appends a new block to that heap and return a pointer to the block skipping the internal header/metadata.

# Free
Free as is name suggest it, frees the memory. It does not neccessarly return memory to the system.
Instead free set the free flag of a block to True so that already allocated memory can be reused later. If the previous or next blocks are also free it merges them to prevent fragmentation. If this is the last block of a heap, heap is then returned to the system with munmap unless it is the last LARGE type of heap allocated.

# Realloc
Realloc resizes a previously allocated memory space.
- If a NULL pointer is given it just calls malloc and return a new pointer.
- If the size is not greater than 0 it just frees the pointer and returns NULL.
- If the pointer is not found it returns NULL.
- If the size is equal to the size of the actual block the provided pointer is returned without modification.
- Finally it starts malloc with the new given size, copy the memory from the first block to the new block, frees the old pointer and returns the new one.

# Show allocated memory
This function prints the actual state of the heap and block structure. It prints heaps one by one, it first prints the type of heap (TINY, SMALL, LARGE) and its address, then it list all blocks contained in that heap, displaying the block starting address, ending address and size.
The last line displays the total size of allocated memory.
```
TINY : 0x102A2C000
0x102A2C050 - 0x102A2C250 : 512 octets
SMALL : 0x10BBC4000
0x10BBC4050 - 0x10BBC4450 : 1024 octets
LARGE : 0x102A20000
0x102A20050 - 0x102A28050 : 32768 octets
LARGE : 0x10BAC0000
0x10BAC0050 - 0x10BBC0050 : 1048576 octets
LARGE : 0x10AABC000
0x10AABC050 - 0x10BABC050 : 16777216 octets
LARGE : 0x102AB8000
0x102AB8050 - 0x10AAB8050 : 134217728 octets
Total : 152077824 octets
```


# Show allocated memory hex
As the previous function it prints the type and address of each heap struct. And then it displays lines of the actual value stored in memory in hexadecimal form, first the address then the data. It print 16 bytes for each line.
```
TINY : 0x102A2C000
0x102A2C000 : 00 00 00 00 00 00 00 00 00 40 BC 0B 01 00 00 00
0x102A2C010 : 00 00 00 00 00 00 00 00 B0 FD 00 00 00 00 00 00
0x102A2C020 : 00 00 01 00 00 00 00 00 01 00 00 00 00 00 00 00
0x102A2C030 : 00 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0x102A2C040 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0x102A2C050 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0x102A2C060 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0x102A2C070 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0x102A2C080 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0x102A2C090 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0x102A2C0A0 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0x102A2C0B0 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0x102A2C0C0 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0x102A2C0D0 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
0x102A2C0E0 : 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
```

In the example above my computer is in little endian, the struct I use are :
```cpp
typedef struct s_block {
    size_t data_size;
    struct s_block *next;
    struct s_block *prev;
    t_bool free;
} t_block;

typedef struct s_heap {
    t_heap_type type;
    struct s_heap *next;
    struct s_heap *prev;
    size_t free_size;
    size_t total_size;
    size_t nb_blocks;
} t_heap;
```

```
heap :
0x102A2C000 : 0000000000000000 / 0040BC0B01000000 // TYPE : 0 = TINY / next heap ptr : 0040BC0B01000000 = 0x000000010BBC4000 (see next heap)
0x102A2C010 : 0000000000000000 / B0FD000000000000 // prev heap ptr / free_size: B0FD000000000000 = 64944
0x102A2C020 : 0000010000000000 / 0100000000000000 // total_size: 0000010000000000 = 65536 / nb_blocks: 0100000000000000 = 1
block :
0x102A2C030 : 0002000000000000 / 0000000000000000 // data_size: 0002000000000000 = 512 / next block ptr
0x102A2C040 : 0000000000000000 / 0000000000000000 // prev block ptr / free 0 = FALSE
block data :
0x102A2C050 : 0000000000000000 / 0000000000000000 // 0x102A2C050 is the ptr returned to the user
0x102A2C060 : 0000000000000000 / 0000000000000000
...
SMALL : 0x10BBC4000
0x10BBC4000 : 01 00 00 00 00 00 00 00 00 00 A2 02 01 00 00 00
```

# Mutex
Every functions are protected by a global mutex to prevent data race.
Also in this malloc size are 16 bytes aligned : `size = (size + 15) & ~15;`
