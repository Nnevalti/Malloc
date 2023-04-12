#include "malloc.h"

t_heap *g_heap = NULL;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief Get the system limit of memory
 * @return The system limit of memory
 **/
static rlim_t get_system_limit(void)
{
    struct rlimit rpl;

    if (getrlimit(RLIMIT_DATA, &rpl) < 0)
        return (-1);
    return (rpl.rlim_max);
}

/**
 * @brief Allocate memory using mmap
 * @param size The size to allocate
 * @return The pointer to the allocated memory
 **/
void *mmap_malloc(size_t size)
{
    void *ptr;

    if (size > get_system_limit())
        return (NULL);
    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    if (ptr == MAP_FAILED)
    {
        return NULL;
    }
    return ptr;
}

/**
 * @brief Get the heap type based on the block size
 * @param size The size of the block
 * @return The heap type (TINY, SMALL or LARGE)
 **/
t_heap_type get_heap_type(size_t size)
{
    if (size <= (size_t)TINY_BLOCK_SIZE)
    {
        return (TINY);
    }
    else if (size <= (size_t)SMALL_BLOCK_SIZE)
    {
        return (SMALL);
    }
    return (LARGE);
}

/**
 * @brief Initialize a block
 * @param t_block A pointer to the block to initialize
 * @param size The size of the block
 * @return void
 **/
void init_block(t_block *block, size_t size)
{
    block->data_size = size;
    block->free = FALSE;
    block->next = NULL;
    block->prev = NULL;
}

/**
 * @brief Divide a block into two blocks (optimization to prevent unused memory)
 * @param block The block to divide
 * @param size The size of the first block
 * @param heap The heap containing the block
 * @return void
 **/
void divide_block(t_block *block, size_t size, t_heap *heap)
{
    t_block *new_block;

    new_block = (t_block *)(BLOCK_SHIFT(block) + size);
    init_block(new_block, block->next - new_block);
    new_block->free = TRUE;
    new_block->prev = block;
    new_block->next = block->next;
    block->next = new_block;
    block->data_size = size;
    block->free = FALSE;
    heap->nb_blocks++;
}

/**
 * @brief Searches for a free block in allocated heaps
 * @param size The size of the block to find
 * @param type The type of the heap
 * @return A pointer to the block if found, NULL otherwise
 **/
t_block *find_block(size_t size, t_heap_type type)
{
    t_heap *heap = g_heap;
    t_block *block;

    while (heap)
    {
        block = HEAP_SHIFT(heap);
        while (heap->type == type && block)
        {
            if (block->free && block->data_size >= size + sizeof(t_block))
            {
                divide_block(block, size, heap);
                return block;
            }
            block = block->next;
        }
        heap = heap->next;
    }
    return NULL;
}

/**
 * @brief Get the allocation size based on the heap type
 * @param size The size of the block
 * @param type The type of the heap
 * @return The allocation size
 **/
size_t get_allocation_size(size_t size, t_heap_type type)
{
    if (type == TINY)
    {
        return (TINY_HEAP_ALLOCATION_SIZE);
    }
    else if (type == SMALL)
    {
        return (SMALL_HEAP_ALLOCATION_SIZE);
    }
    return (sizeof(t_heap) + sizeof(t_block) + size);
}

/**
 * @brief Find a heap with enough free space to allocate a block
 * @param size The size of the block
 * @param type The type of the heap
 * @return A pointer to the heap if found, NULL otherwise
 **/
t_heap *find_big_enough_heap(size_t size, t_heap_type type)
{
    t_heap *heap = g_heap;
    while (heap)
    {
        if (heap->type == type && heap->free_size >= size)
        {
            return heap;
        }
        heap = heap->next;
    }
    return NULL;
}

/**
 * @brief Create a new heap
 * @param size The size of the heap
 * @param type The type of the heap
 * @return A pointer to the newly created heap
 **/
t_heap *create_new_heap(size_t size, t_heap_type type)
{
    t_heap *heap;
    size_t allocation_size;

    allocation_size = get_allocation_size(size, type);
    heap = mmap_malloc(allocation_size);
    if (heap == NULL)
    {
        return NULL;
    }
    ft_bzero(heap, sizeof(heap));
    heap->type = type;
    heap->free_size = allocation_size - sizeof(t_heap);
    heap->total_size = allocation_size;
    heap->next = g_heap;
    if (heap->next)
    {
        heap->next->prev = heap;
    }
    g_heap = heap;
    return heap;
}

/**
 * @brief Find or create a heap to allocate a block
 * @param size The size of the block
 * @param type The type of the heap
 * @return A pointer to the heap
 **/
t_heap *find_or_create_heap(size_t size, t_heap_type type)
{
    t_heap *heap;

    heap = find_big_enough_heap(size + sizeof(t_block), type);
    if (heap == NULL)
    {
        heap = create_new_heap(size, type);
    }
    return heap;
}

/**
 * @brief Get the last block of a heap
 * @param heap The heap we want to get the last block from
 * @return A pointer to the last block
 **/
t_block *get_last_block(t_heap *heap)
{
    t_block *block;
    block = HEAP_SHIFT(heap);
    while (block->next)
    {
        block = block->next;
    }
    return block;
}

/**
 * @brief Create a new block in the provided heap
 * @param size The size of the block
 * @param heap The heap
 * @return A pointer to the newly created block
 **/
t_block *create_block(size_t size, t_heap *heap)
{
    t_block *last_block;
    t_block *new_block;

    last_block = NULL;
    new_block = HEAP_SHIFT(heap);
    if (heap->nb_blocks)
    {
        last_block = get_last_block(heap);
        new_block = BLOCK_SHIFT(last_block) + last_block->data_size;
    }
    init_block(new_block, size);
    if (heap->nb_blocks)
    {
        new_block->prev = last_block;
        last_block->next = new_block;
    }
    heap->nb_blocks++;
    heap->free_size -= new_block->data_size + sizeof(t_block);
    return new_block;
}

/**
 * @brief Start the malloc process
 * @param size The size of the block to allocate
 * @return A pointer to the newly allocated block or NULL if an error occured
 **/
void *start_malloc(size_t size)
{
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

/**
 * @brief The malloc function
 * @param size The size of the block to allocate
 * @return A pointer to the newly allocated block
 * @note This function is thread safe
 **/
void *malloc(size_t size)
{
    void *ptr;

    pthread_mutex_lock(&g_mutex);
    ptr = start_malloc(size);
    if (ptr)
    {
        ft_memset(ptr, 0, size);
    }
    pthread_mutex_unlock(&g_mutex);
    return ptr;
}
