#include "malloc.h"

void search_ptr(t_heap **heap, t_block **block, void *ptr)
{
	*heap = g_heap;
	while (*heap)
	{
		*block = (t_block *)HEAP_SHIFT(*heap);
		while (*block)
		{
			if (ptr == (void *)*block + sizeof(t_block))
				return;
			*block = (*block)->next;
		}
		*heap = (*heap)->next;
	}
	*heap = NULL;
	*block = NULL;
}

t_block *merge_block(t_block *block)
{
	t_block *merged_block = NULL;

	if (block->next && block->next->free)
	{
		block->data_size += block->next->data_size + sizeof(t_block);
		if (block->next->next)
			block->next->next->prev = block;
		block->next = block->next->next;
		merged_block = block;
	}
	if (block->prev && block->prev->free)
	{
		block->prev->next = block->next;
		if (block->next)
			block->next->prev = block->prev;
		block->prev->data_size += block->data_size + sizeof(t_block);
		merged_block = block->prev;
	}
	return (merged_block);
}

void remove_block_if_last(t_heap *heap, t_block *block)
{
	if (!block->next)
	{
		if (block->prev)
			block->prev->next = NULL;
		heap->free_size += block->data_size + sizeof(t_block);
		heap->nb_blocks--;
	}
}

t_bool is_last_preallocated_of_type(t_heap *heap)
{
	t_heap *tmp = g_heap;

	while (tmp)
	{
		if (tmp->type == heap->type && tmp != heap)
			return (FALSE);
		tmp = tmp->next;
	}
	return (TRUE);
}

void delete_heap_if_empty(t_heap *heap)
{
	if (heap->nb_blocks == 0)
	{
		if (heap->prev)
			heap->prev->next = heap->next;
		if (heap->next)
			heap->next->prev = heap->prev;
		if (is_last_preallocated_of_type(heap) && heap->type != LARGE)
			return;
		if (heap == g_heap)
			g_heap = heap->next;
		munmap(heap, heap->total_size);
	}
}

void start_free(void *ptr)
{
	t_heap *heap = NULL;
	t_block *block = NULL;
	t_block *merged_block = NULL;

	if (!ptr)
		return;
	search_ptr(&heap, &block, ptr);
	if (!heap || !block)
		return;
	block->free = TRUE;
	if ((merged_block = merge_block(block)))
		block = merged_block;
	remove_block_if_last(heap, block);
	delete_heap_if_empty(heap);
}

void free(void *ptr)
{
	pthread_mutex_lock(&g_mutex);
	start_free(ptr);
	pthread_mutex_unlock(&g_mutex);
}