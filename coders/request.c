#include "include/codexion.h"

static void	push_request(t_coder *coder, t_dongle *dongle)
{
	t_request		req;
	t_policy		scheduler;
	long			key;

	key = get_time_ms();
	req.coder_id = coder->id;
	scheduler = coder->sim->scheduler;
	if (scheduler == POLICY_EDF)
	{
		key = coder->last_compile_start_ms + coder->sim->dongle_cooldown;
	}
	req.key = key;
	heap_insert(&dongle->heap, req);
}

static void	request_dongle(t_coder *c, t_dongle *d, t_sim *sim)
{
	t_timespec		timeout;

	pthread_mutex_lock(&d->lock);
	push_request(c, d);
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

void	request_dongles(t_coder *c, t_sim *sim)
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
	request_dongle(c, last, sim);
}
