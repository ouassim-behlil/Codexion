#include "../include/codexion.h"

static void	swap(t_request *req1, t_request *req2)
{
	t_request		temp_req;

	if (req1 == req2)
	{
		return ;
	}
	temp_req = *req1;
	*req1 = *req2;
	*req2 = temp_req;
}

void	heap_insert(t_heap *heap, t_request req)
{
	int			idx;
	int			parent_idx;
	t_request	*requests;

	requests = heap->requests;
	requests[heap->size] = req;
	idx = heap->size;
	parent_idx = (idx - 1) / 2;
	while (idx > 0 && requests[idx].key < requests[parent_idx].key)
	{
		swap(&requests[idx], &requests[parent_idx]);
		idx = parent_idx;
		parent_idx = (idx - 1) / 2;
	}
	heap->size ++;
}

static void	heapify_up(t_heap *heap, int idx)
{
	int			parent_idx;
	t_request	*requests;

	requests = heap->requests;
	parent_idx = (idx - 1) / 2;
	while (idx > 0 && requests[idx].key < requests[parent_idx].key)
	{
		swap(&requests[idx], &requests[parent_idx]);
		idx = parent_idx;
		parent_idx = (idx - 1) / 2;
	}
}

static void	heapify_down_from(t_heap *heap, int idx)
{
	int			left;
	int			right;
	int			smallest;
	t_request	*requests;

	requests = heap->requests;
	while (idx < heap->size)
	{
		smallest = idx;
		left = idx * 2 + 1;
		right = idx * 2 + 2;
		if (left < heap->size && requests[smallest].key > requests[left].key)
			smallest = left;
		if (right < heap->size && requests[smallest].key > requests[right].key)
			smallest = right;
		if (smallest == idx)
			return ;
		swap(&requests[smallest], &requests[idx]);
		idx = smallest;
	}
}

void	heap_remove_by_id(t_heap *heap, int coder_id)
{
	int			idx;
	t_request	*requests;

	requests = heap->requests;
	idx = 0;
	while (idx < heap->size && requests[idx].coder_id != coder_id)
		idx ++;
	if (idx == heap->size)
		return ;
	requests[idx] = requests[heap->size - 1];
	heap->size --;
	if (idx == heap->size)
		return ;
	heapify_up(heap, idx);
	heapify_down_from(heap, idx);
}


// heap_peek returns the coder id at the root of the heap
int	heap_peek(t_heap *heap)
{
	if (heap->size == 0)
		return (-1);
	return (heap->requests[0].coder_id);
}
