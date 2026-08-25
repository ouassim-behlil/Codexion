#include "../include/codexion.h"
#define BASE_DEC 10

static t_error_code	parse_long(const char *s, long *out)
{
	long	n;
	int		digit;

	if (!s || !out)
		return (ERR_NULL_PTR);
	if (!(*s))
		return (ERR_EMPTY_STR);
	n = 0;
	while (*s >= '0' && *s <= '9')
	{
		digit = *s - '0';
		if (n > (LONG_MAX - digit) / BASE_DEC)
			return (ERR_OVERFLOW);
		n = n * BASE_DEC + digit;
		s++;
	}
	if (*s)
		return (ERR_INVALID_ARG);
	*out = n;
	return (ERR_OK);
}

static t_error_code	parse_int(const char *s, int *out)
{
	int		n;
	int		digit;

	if (!s || !out)
		return (ERR_NULL_PTR);
	if (!(*s))
		return (ERR_EMPTY_STR);
	n = 0;
	while (*s >= '0' && *s <= '9')
	{
		digit = *s - '0';
		if (n > (INT_MAX - digit) / BASE_DEC)
			return (ERR_OVERFLOW);
		n = n * BASE_DEC + digit;
		s++;
	}
	if (*s)
		return (ERR_INVALID_ARG);
	*out = n;
	return (ERR_OK);
}

static t_error_code	parse_scheduler(const char *s, t_policy *policy)
{
	if (!s)
		return (ERR_NULL_PTR);
	if (strcmp(s, "fifo") == 0)
	{
		*policy = POLICY_FIFO;
		return (ERR_OK);
	}
	if (strcmp(s, "edf") == 0)
	{
		*policy = POLICY_EDF;
		return (ERR_OK);
	}
	return (ERR_BAD_SCHEDULER);
}

int	parse_args(int ac, const char **av, t_sim *sim)
{
	t_error_code	err;

	if (ac != 9)
	{
		print_error(ERR_INVALID_NUM_PARAMS);
		return (-1);
	}
	err = ERR_OK;
	err |= parse_int(av[1], &sim->n_coders);
	err |= parse_long(av[2], &sim->time_to_burnout);
	err |= parse_long(av[3], &sim->time_to_compile);
	err |= parse_long(av[4], &sim->time_to_debug);
	err |= parse_long(av[5], &sim->time_to_refactor);
	err |= parse_int(av[6], &sim->compiles_required);
	err |= parse_long(av[7], &sim->dongle_cooldown);
	err |= parse_scheduler(av[8], &sim->scheduler);
	if (err != ERR_OK)
	{
		print_error(err);
		return (-1);
	}
	return (0);
}
