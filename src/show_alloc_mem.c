#include "malloc.h"

void print_heap_type(t_heap *heap)
{
	if (heap->type == TINY)
		ft_putstr("TINY");
	else if (heap->type == SMALL)
		ft_putstr("SMALL");
	else
		ft_putstr("LARGE");
}

void print_heap_metadata(t_heap *heap)
{
	print_heap_type(heap);
	ft_putstr(" : ");
	ft_putstr("0x");
	ft_itoa_base((size_t)heap, 16, 9);
	ft_putstr("\n");
}

size_t print_heap_blocks(t_block *block)
{
	char *start_address;
	char *end_address;
	size_t total_size;

	total_size = 0;
	start_address = NULL;
	end_address = NULL;
	while (block)
	{
		start_address = BLOCK_SHIFT(block);
		end_address = start_address + block->data_size;
		if (!block->free)
		{
			ft_putstr("0x");
			ft_itoa_base((size_t)start_address, 16, 9);
			ft_putstr(" - ");
			ft_putstr("0x");
			ft_itoa_base((size_t)end_address, 16, 9);
			ft_putstr(" : ");
			ft_itoa_base(block->data_size, 10, 0);
			ft_putstr(" octets\n");
			total_size += block->data_size;
		}
		block = block->next;
	}
	return (total_size);
}

void print_heap_hex_line(u_int8_t *start)
{
	u_int8_t i;

	i = 0;
	ft_putstr("0x");
	ft_itoa_base((size_t)start, 16, 0);
	ft_putstr(" :");
	while (i < 16)
	{
		ft_putstr(" ");
		ft_itoa_base((u_int8_t)start[i], 16, 2);
		i++;
	}
	ft_putstr("\n");
}

static void print_allocated_memory_hex(void)
{
	t_heap *heap;
	size_t i;
	u_int8_t *address;

	heap = g_heap;
	while (heap)
	{
		i = 0;
		print_heap_metadata(heap);
		address = (u_int8_t *)heap;
		while (i < heap->total_size)
		{
			print_heap_hex_line(address + i);
			i += 16;
		}
		heap = heap->next;
	}
}

static void print_allocated_memory(void)
{
	t_heap *heap;
	size_t total_size;

	total_size = 0;
	heap = g_heap;
	while (heap)
	{
		print_heap_metadata(heap);
		if (heap->nb_blocks > 0)
			total_size += print_heap_blocks((t_block *)HEAP_SHIFT(heap));
		heap = heap->next;
	}
	ft_putstr("Total : ");
	ft_itoa_base(total_size, 10, 0);
	ft_putstr(" octets\n");
}

void show_alloc_mem(void)
{
	pthread_mutex_lock(&g_mutex);
	print_allocated_memory();
	pthread_mutex_unlock(&g_mutex);
}

void show_alloc_mem_hex(void)
{
	pthread_mutex_lock(&g_mutex);
	print_allocated_memory_hex();
	pthread_mutex_unlock(&g_mutex);
}