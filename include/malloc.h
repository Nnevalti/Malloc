#ifndef MALLOC_H
#define MALLOC_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/resource.h>

#define HEAP_SHIFT(start) ((void *)start + sizeof(t_heap))
#define BLOCK_SHIFT(start) ((void *)start + sizeof(t_block))

#define TINY_HEAP_ALLOCATION_SIZE (4 * getpagesize())
#define TINY_BLOCK_SIZE (TINY_HEAP_ALLOCATION_SIZE / 128)
#define SMALL_HEAP_ALLOCATION_SIZE (32 * getpagesize())
#define SMALL_BLOCK_SIZE (SMALL_HEAP_ALLOCATION_SIZE / 128)

typedef enum s_bool
{
    FALSE,
    TRUE
} t_bool;

typedef enum e_heap_type
{
    TINY,
    SMALL,
    LARGE
} t_heap_type;

typedef struct s_block
{
    size_t data_size;
    struct s_block *next;
    struct s_block *prev;
    t_bool free;
} t_block;

typedef struct s_heap
{
    t_heap_type type;
    struct s_heap *next;
    struct s_heap *prev;
    size_t free_size;
    size_t total_size;
    size_t nb_blocks;
} t_heap;

extern t_heap *g_heap;
extern pthread_mutex_t g_mutex;

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void show_alloc_mem(void);
void show_alloc_mem_hex(void);

void *start_malloc(size_t size);
void start_free(void *ptr);
void ft_putstr(char *str);

/**
 * UTILS FUNCTIONS
 */

int ft_strlen(char *str);
void ft_putstr(char *str);
void ft_itoa_base(size_t n, char base, char width);
void ft_bzero(void *s, size_t n);
void ft_memset(void *ptr, int c, size_t size);
void ft_memmove(void *dst, const void *src, size_t len);

#endif