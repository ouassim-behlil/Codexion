#include "../include/codexion.h"

static void	destroy_coder(t_coder *coder)
{
	if (!coder)
		return ;
	pthread_mutex_destroy(&coder->lock);
	free(coder);
}

static void	fill_coder(t_coder *coder, int id, t_sim *sim)
{
	coder->id = id;
	coder->compiles_done = 0;
	coder->left_dongle_id = id;
	coder->right_dongle_id = (id % sim->n_dongles) + 1;
	coder->sim = sim;
	coder->last_compile_start_ms = -1;
	coder->attempt_start_ms = 0;
	pthread_mutex_init(&coder->lock, NULL);
}

int	init_coders(t_sim *sim)
{
	int			i;
	t_coder		*coder;

	sim->coders = malloc(sim->n_coders * sizeof(t_coder *));
	if (!(sim->coders))
		return (-1);
	i = 0;
	while (i < sim->n_coders)
	{
		coder = malloc(sizeof(t_coder));
		if (!coder)
		{
			while (i--)
				destroy_coder(sim->coders[i]);
			free(sim->coders);
			return (-1);
		}
		fill_coder(coder, i + 1, sim);
		sim->coders[i] = coder;
		i++;
	}
	return (0);
}

void	destroy_coders(t_sim *sim)
{
	int			i;

	if (!sim)
		return ;
	i = 0;
	while (i < sim->n_coders)
	{
		destroy_coder(sim->coders[i]);
		i++;
	}
	free(sim->coders);
}
