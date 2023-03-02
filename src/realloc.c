#include "malloc.h"

/**
 * @brief Search for a block in the heap corresponding to the pointer
 * @param ptr The pointer to search
 * @return void
 **/
t_block *search_ptr_realloc(void *ptr)
{
	t_heap *heap = g_heap;
	t_block *block = NULL;

	while (heap)
	{
		block = (t_block *)HEAP_SHIFT(heap);
		while (block)
		{
			if (ptr == (void *)block + sizeof(t_block))
				return (block);
			block = block->next;
		}
		heap = heap->next;
	}
	return (NULL);
}

/**
 * @brief Start realloc process
 * @param ptr The pointer to the block
 * @param size The new size of the block
 * @return void
 **/
void *start_realloc(void *ptr, size_t size)
{
	t_block *block;
	void *new_ptr;

	if (!ptr)
		return (start_malloc(size));
	if (!size)
	{
		start_free(ptr);
		return (NULL);
	}
	block = search_ptr_realloc(ptr);
	if (!block)
		return (NULL);
	if (size == block->data_size)
		return (ptr);
	new_ptr = start_malloc(size);
	ft_memmove(new_ptr, ptr, block->data_size > size ? size : block->data_size);
	start_free(ptr);
	return (new_ptr);
}

/**
 * @brief Reallocate a block
 * @param ptr The pointer to the block
 * @param size The new size of the block
 * @return void
 * @note This function is thread-safe
 **/
void *realloc(void *ptr, size_t size)
{
	void *res;

	pthread_mutex_lock(&g_mutex);
	res = start_realloc(ptr, size);
	pthread_mutex_unlock(&g_mutex);
	return (res);
}