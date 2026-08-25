#include "../include/codexion.h"

static void	push_request(t_coder *coder, t_dongle *dongle)
{
	t_request		req;
	t_policy		scheduler;
	long			key;

	key = get_time_ms();
	// printf("coder %d made a request at %ld for dongle %d\n", coder->id, key - coder->sim->start_ms, dongle->id);
	req.coder_id = coder->id;
	scheduler = coder->sim->scheduler;
	if (scheduler == POLICY_EDF)
	{
		pthread_mutex_lock(&coder->lock);
		key = coder->last_compile_start_ms + coder->sim->time_to_burnout;
		pthread_mutex_unlock(&coder->lock);
	}
	req.key = key;
	heap_insert(&dongle->heap, req);
}

static void	request_dongle(t_coder *c, t_dongle *d, t_sim *sim)
{
	t_timespec		timeout;

	pthread_mutex_lock(&d->lock);
	push_request(c, d);
	if (sim->scheduler == POLICY_EDF)
	{
		pthread_mutex_unlock(&d->lock);
		usleep(50);
		pthread_mutex_lock(&d->lock);
	}
	while (
		d->held == 1
		|| heap_peek(&d->heap) != c->id
		|| get_time_ms() < d->not_available_until_ms
	)
	{
		if (is_stopped(sim) == 1)
		{
			pthread_mutex_unlock(&d->lock);
			return ;
		}
		if (heap_peek(&d->heap) != c->id)
			pthread_cond_wait(&d->cv, &d->lock);
		else
		{
			timeout = ms_to_timespec(d->not_available_until_ms);
			pthread_cond_timedwait(&d->cv, &d->lock, &timeout);
		}
	}
	d->held = 1;
	pthread_mutex_unlock(&d->lock);
}

int	request_dongles(t_coder *c, t_sim *sim)
{
	t_dongle	*first;
	t_dongle	*last;

	first = sim->dongles[c->left_dongle_id - 1];
	last = sim->dongles[c->right_dongle_id - 1];
	if (c->id % 2 != 0)
	{
		first = sim->dongles[c->right_dongle_id - 1];
		last = sim->dongles[c->left_dongle_id - 1];
	}

	request_dongle(c, first, sim);
	if (last == first)
		return (-1);
	request_dongle(c, last, sim);
	return (0);
}
