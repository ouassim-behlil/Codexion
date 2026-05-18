#include "include/codexion.h"

static int	compile(t_coder *coder)
{
	long	now;

	coder->attempt_start_ms = get_time_ms();
	request_dongles(coder, coder->sim);
	log_msg(coder, "has taken dongle");
	log_msg(coder, "has taken dongle");
	log_msg(coder, "is compiling");
	now = get_time_ms();
	pthread_mutex_lock(&coder->lock);
	coder->last_compile_start_ms = now;
	pthread_mutex_unlock(&coder->lock);
	sleep_until_ms(now + coder->sim->time_to_compile, coder->sim);
	pthread_mutex_lock(&coder->lock);
	coder->compiles_done += 1;
	pthread_mutex_unlock(&coder->lock);
	release_dongles(coder);
	return (0);
}

static void	debug(t_coder *coder)
{
	log_msg(coder, "is debugging");
	sleep_until_ms(get_time_ms() + coder->sim->time_to_debug, coder->sim);
}

static void	refactor(t_coder *coder)
{
	log_msg(coder, "is refactoring");
	sleep_until_ms(get_time_ms() + coder->sim->time_to_refactor, coder->sim);
	pthread_mutex_lock(&coder->lock);
	coder->compiles_done += 1;
	pthread_mutex_unlock(&coder->lock);
}

void	*coder_routine(void *arg)
{
	t_coder		*coder;
	int			done;

	coder = (t_coder *)arg;
	pthread_mutex_lock(&coder->lock);
	done = coder->compiles_done;
	pthread_mutex_unlock(&coder->lock);
	while (done < coder->sim->compiles_required)
	{
		if (compile(coder) != 0)
		{
			signal_stop(coder->sim);
			return (NULL);
		}
		debug(coder);
		refactor(coder);
		pthread_mutex_lock(&coder->lock);
		done = coder->compiles_done;
		pthread_mutex_unlock(&coder->lock);
	}
	return (NULL);
}
