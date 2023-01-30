#include "malloc.h"

void ft_memmove(void *dst, const void *src, size_t len)
{
	unsigned char *d;
	unsigned char *s;

	d = (unsigned char *)dst;
	s = (unsigned char *)src;
	if (d < s)
	{
		while (len--)
			*d++ = *s++;
	}
	else
	{
		d += len;
		s += len;
		while (len--)
			*--d = *--s;
	}
}

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

void *realloc(void *ptr, size_t size)
{
	void *res;

	pthread_mutex_lock(&g_mutex);
	res = start_realloc(ptr, size);
	pthread_mutex_unlock(&g_mutex);
	return (res);
}