#include "../include/codexion.h"

int	is_stopped(t_sim *sim)
{
	pthread_mutex_lock(&sim->log_lock);
	if (sim->stop == 1)
	{
		pthread_mutex_unlock(&sim->log_lock);
		return (1);
	}
	pthread_mutex_unlock(&sim->log_lock);
	return (0);
}

static int	check_burnout(t_coder *coder)
{
	t_sim	*sim;

	sim = coder->sim;
	pthread_mutex_lock(&coder->lock);
	if (
		coder->compiles_done < sim->compiles_required
		&& elapsed_ms(coder->last_compile_start_ms) >= sim->time_to_burnout
	)
	{
		pthread_mutex_unlock(&coder->lock);
		return (-1);
	}
	pthread_mutex_unlock(&coder->lock);
	return (0);
}

void	signal_stop(t_sim *sim)
{
	int		i;

	i = 0;
	
	while (i < sim->n_dongles)
	{
		pthread_mutex_lock(&sim->dongles[i]->lock);
		pthread_cond_broadcast(&sim->dongles[i]->cv);
		pthread_mutex_unlock(&sim->dongles[i]->lock);
		i++;
	}
}

static int	check_coders(t_sim *sim, int *n)
{
	int			i;
	t_coder		*coder;

	i = 0;
	while (i < sim->n_coders)
	{
		coder = sim->coders[i];
		if (check_burnout(coder) != 0)
		{
			log_msg(coder, "burned out");
			pthread_mutex_lock(&sim->log_lock);
			sim->stop = 1;
			pthread_mutex_unlock(&sim->log_lock);
			signal_stop(sim);
			return (-1);
		}
		pthread_mutex_lock(&coder->lock);
		if (coder->compiles_done < sim->compiles_required)
			*n = 1;
		pthread_mutex_unlock(&coder->lock);
		i++;
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_sim		*sim;
	int			n;

	sim = (t_sim *)arg;
	while (1)
	{
		if (is_stopped(sim) != 0)
			return (NULL);
		n = 0;
		if (check_coders(sim, &n) != 0)
		{
			return (NULL);
		}
		if (n == 0)
		{
			return (NULL);
		}
		usleep(500);
	}
}
