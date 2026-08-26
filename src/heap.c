/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obehlil <obehlil@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:11:46 by obehlil           #+#    #+#             */
/*   Updated: 2026/08/26 11:45:11 by obehlil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	heap_insert(t_heap *heap, t_request req)
{
	int			idx;
	int			parent_idx;
	t_request	*requests;
	t_request	temp;

	requests = heap->requests;
	requests[heap->size] = req;
	idx = heap->size;
	parent_idx = (idx - 1) / 2;
	while (idx > 0 && requests[idx].key < requests[parent_idx].key)
	{
		temp = requests[idx];
		requests[idx] = requests[parent_idx];
		requests[parent_idx] = temp;
		idx = parent_idx;
		parent_idx = (idx - 1) / 2;
	}
	heap->size ++;
}

static void	heapify_up(t_heap *heap, int idx)
{
	int			parent_idx;
	t_request	*requests;
	t_request	temp;

	requests = heap->requests;
	parent_idx = (idx - 1) / 2;
	while (idx > 0 && requests[idx].key < requests[parent_idx].key)
	{
		temp = requests[idx];
		requests[idx] = requests[parent_idx];
		requests[parent_idx] = temp;
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
	t_request	temp;

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
		temp = requests[smallest];
		requests[smallest] = requests[idx];
		requests[idx] = temp;
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

int	heap_peek(t_heap *heap)
{
	if (heap->size == 0)
		return (-1);
	return (heap->requests[0].coder_id);
}
