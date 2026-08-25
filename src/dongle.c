#include "../include/codexion.h"

static void	destroy_dongle(t_dongle *dongle)
{
	if (!dongle)
		return ;
	pthread_cond_destroy(&(dongle->cv));
	pthread_mutex_destroy(&(dongle->lock));
	free(dongle);
}

static void	fill_dongle(t_dongle *dongle, int id)
{
	pthread_cond_init(&dongle->cv, NULL);
	pthread_mutex_init(&dongle->lock, NULL);
	dongle->held = 0;
	dongle->id = id;
	dongle->not_available_until_ms = 0;
	dongle->heap.size = 0;
	dongle->heap.cap = 2;
}

int	init_dongles(t_sim *sim)
{
	int			i;
	t_dongle	*dongle;

	sim->dongles = malloc(sim->n_dongles * sizeof(t_dongle *));
	if (!sim->dongles)
		return (-1);
	i = 0;
	while (i < sim->n_dongles)
	{
		dongle = malloc(sizeof(t_dongle));
		if (!dongle)
		{
			while (i--)
				destroy_dongle(sim->dongles[i]);
			free(sim->dongles);
			return (-1);
		}
		fill_dongle(dongle, i + 1);
		sim->dongles[i] = dongle;
		i++;
	}
	return (0);
}

void	destroy_dongles(t_sim *sim)
{
	int			i;

	if (!sim)
		return ;
	i = 0;
	while (i < sim->n_dongles)
	{
		destroy_dongle(sim->dongles[i]);
		i++;
	}
	free(sim->dongles);
}
