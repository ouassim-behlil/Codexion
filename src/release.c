#include "../include/codexion.h"

static void	release_dongle(t_dongle *dongle, t_sim *sim, t_coder *coder)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->not_available_until_ms = get_time_ms() + sim->dongle_cooldown;
	dongle->held = 0;
	heap_remove_by_id(&dongle->heap, coder->id);
	pthread_cond_broadcast(&dongle->cv);
	pthread_mutex_unlock(&dongle->lock);
}

void	release_dongles(t_coder *coder)
{
	t_dongle	*left;
	t_dongle	*right;

	left = coder->sim->dongles[coder->left_dongle_id - 1];
	right = coder->sim->dongles[coder->right_dongle_id - 1];
	release_dongle(left, coder->sim, coder);
	release_dongle(right, coder->sim, coder);
}


