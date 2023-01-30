#include "malloc.h"

t_heap *g_heap = NULL;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

static rlim_t	get_system_limit(void)
{
	struct rlimit rpl;

	if (getrlimit(RLIMIT_DATA, &rpl) < 0)
		return (-1);
	return (rpl.rlim_max);
}

// function to refacto use of mmap
void *mmap_malloc(size_t size) {
    void *ptr;

    if (size > get_system_limit())
        return (NULL);
    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (ptr == MAP_FAILED) {
        return NULL;
    }
    return ptr;
}

// start malloc using mmap and not sbrk, and using allocation size define in ft_malloc.h

t_heap_type get_heap_type(size_t size) {
    if (size <= (size_t)TINY_BLOCK_SIZE) {
        return(TINY);
    }
    else if (size <= (size_t)SMALL_BLOCK_SIZE) {
        return (SMALL);
    }
    return (LARGE);
}

void init_block(t_block *block, size_t size) {
    block->data_size = size;
    block->free = FALSE;
    block->next = NULL;
    block->prev = NULL;
}

// divide the block to optimize space
void divide_block(t_block *block, size_t size, t_heap *heap) {
    t_block *new_block;

    new_block = (t_block *) (BLOCK_SHIFT(block) + size);
    init_block(new_block, block->next - new_block - sizeof(t_block));
    new_block->free = TRUE;
    new_block->prev = block;
    new_block->next = block->next;
    block->next = new_block;
    block->data_size = size;
    block->free = FALSE;
    heap->nb_blocks++;
}

t_block *find_block(size_t size, t_heap_type type) {
    t_heap *heap = g_heap;
    t_block *block;

    while (heap) {
        block = HEAP_SHIFT(heap);
        while (heap->type == type && block) {
            if (block->free && block->data_size >= size + sizeof(t_block)) {
                divide_block(block, size, heap);
                return block;
            }
            block = block->next;
        }
        heap = heap->next;
    }
    return NULL;
}

size_t get_allocation_size(size_t size, t_heap_type type) {
    if (type == TINY) {
        return (TINY_HEAP_ALLOCATION_SIZE);
    } else if (type == SMALL) {
        return (SMALL_HEAP_ALLOCATION_SIZE);
    }
    return (sizeof(t_heap) + sizeof(t_block) + size);
}

t_heap *find_big_enough_heap(size_t size, t_heap_type type) {
    t_heap *heap = g_heap;
    while (heap) {
        if (heap->type == type && heap->free_size >= size) {
            return heap;
        }
        heap = heap->next;
    }
    return NULL;
}

void ft_bzero(void *s, size_t n) {
    unsigned char *ptr = s;
    while (n--) {
        *ptr++ = 0;
    }
}

t_heap *create_new_heap(size_t size, t_heap_type type) {
    t_heap *heap;
    size_t allocation_size;

    allocation_size = get_allocation_size(size, type);
    heap = mmap_malloc(allocation_size);
    if (heap == NULL) {
        return NULL;
    }
    ft_bzero(heap, allocation_size);
    heap->type = type;
    heap->next = NULL;
    heap->prev = NULL;
    heap->free_size = allocation_size - sizeof(t_heap);
    heap->total_size = allocation_size;
    heap->nb_blocks = 0;
    if (g_heap == NULL) {
        g_heap = heap;
    } else {
        heap->prev = g_heap->prev;
        heap->next = g_heap;
        g_heap->prev = heap;
        g_heap = heap;
    }
    return heap;
}

t_heap *find_or_create_heap(size_t size, t_heap_type type) {
    t_heap *heap;

    heap = find_big_enough_heap(size + sizeof(t_block), type);
    if (heap == NULL) {
        heap = create_new_heap(size, type);
    }
    return heap;
}

t_block *get_last_block(t_heap *heap) {
    t_block *block;
    block = HEAP_SHIFT(heap);
    while (block->next) {
        block = block->next;
    }
    return block;
}

t_block *create_block(size_t size, t_heap *heap) {
    t_block *last_block;
    t_block *new_block;

    last_block = NULL;
    new_block = HEAP_SHIFT(heap);
    if (heap->nb_blocks) {
        last_block = get_last_block(heap);
        new_block = BLOCK_SHIFT(last_block) + last_block->data_size;
    }
    init_block(new_block, size);
    if (heap->nb_blocks) {
        new_block->prev = last_block;
        last_block->next = new_block;
    }
    heap->nb_blocks++;
    heap->free_size -= new_block->data_size + sizeof(t_block);
    return new_block;
}

void *start_malloc(size_t size) {
    t_block *ptr;
    t_heap *heap;
    t_heap_type type;

    if (!size)
        return NULL;
    size = (size + 15) & ~15; // 16 bits alignement
    type = get_heap_type(size);
    if ((ptr = find_block(size, type)))
        return BLOCK_SHIFT(ptr);
    if (!(heap = find_or_create_heap(size, type)))
        return NULL;
    ptr = create_block(size, heap);
    return BLOCK_SHIFT(ptr);
}

void ft_memset(void *ptr, int c, size_t size) {
    char *p = ptr;
    while (size--) {
        *p++ = c;
    }
}

void *malloc(size_t size) {
    void *ptr;

    pthread_mutex_lock(&g_mutex);
    ptr = start_malloc(size);
    if (ptr) {
        ft_memset(ptr, 0, size);
    }
    pthread_mutex_unlock(&g_mutex);
    return ptr;
}
