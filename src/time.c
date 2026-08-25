#include "../include/codexion.h"

long	get_time_ms(void)
{
	t_time_val		tv;

	gettimeofday(&tv, NULL);
	return ((long)(tv.tv_sec * 1000 + tv.tv_usec / 1000));
}

void	sleep_until_ms(long until_ms, t_sim *sim)
{
	if (!sim)
		return ;
	while (get_time_ms() < until_ms)
	{
		if (is_stopped(sim) == 1)
			return ;
		usleep(50);
	}
}

t_timespec	ms_to_timespec(long ms)
{
	t_timespec		ts;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	return (ts);
}

long	elapsed_ms(long since_ms)
{
	return (get_time_ms() - since_ms);
}
