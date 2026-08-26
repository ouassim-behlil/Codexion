/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obehlil <obehlil@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:10:51 by obehlil           #+#    #+#             */
/*   Updated: 2026/08/26 11:11:18 by obehlil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
# include <limits.h>
# include <time.h>

typedef enum s_error_code
{
	ERR_OK = 0,
	ERR_INVALID_ARG = 1 << 0,
	ERR_OVERFLOW = 1 << 1,
	ERR_INVALID_NUM_PARAMS = 1 << 2,
	ERR_NULL_PTR = 1 << 3,
	ERR_EMPTY_STR = 1 << 4,
	ERR_BAD_SCHEDULER = 1 << 5,
}	t_error_code;

typedef enum e_policy
{
	POLICY_FIFO = 0,
	POLICY_EDF = 1
}	t_policy;

typedef struct s_sim	t_sim;
typedef struct s_coder	t_coder;
typedef struct s_dongle	t_dongle;
typedef struct timeval	t_time_val;
typedef struct timespec	t_timespec;

typedef struct s_request
{
	int			coder_id;
	long		key;
}	t_request;

typedef struct s_heap
{
	int			cap;
	int			size;
	t_request	requests[2];
}	t_heap;

struct s_dongle
{
	int					id;
	int					held;
	long				not_available_until_ms;
	t_heap				heap;
	pthread_mutex_t		lock;
	pthread_cond_t		cv;
};

struct s_coder
{
	int					id;
	int					left_dongle_id;
	int					right_dongle_id;
	long				last_compile_start_ms;
	int					compiles_done;
	long				attempt_start_ms;
	pthread_mutex_t		lock;
	pthread_t			thread;
	t_sim				*sim;
};

struct s_sim
{
	int					n_coders;
	long				time_to_burnout;
	long				time_to_compile;
	long				time_to_debug;
	long				time_to_refactor;
	int					compiles_required;
	long				dongle_cooldown;
	t_policy			scheduler;
	long				start_ms;
	int					stop;
	int					n_dongles;
	t_coder				**coders;
	t_dongle			**dongles;
	pthread_mutex_t		log_lock;
	pthread_t			monitor;
};

// parser.c
int			parse_args(int ac, const char **av, t_sim *sim);

// error.c
void		print_error(t_error_code err);

// time.c
long		get_time_ms(void);
void		sleep_until_ms(long until_ms, t_sim *sim);
t_timespec	ms_to_timespec(long ms);
long		elapsed_ms(long since_ms);

// coder.c
int			init_coders(t_sim *sim);
void		destroy_coders(t_sim *sim);

// dongle.c
int			init_dongles(t_sim *sim);
void		destroy_dongles(t_sim *sim);

// simulation.c
int			init_simulation(t_sim *sim, int ac, const char **av);
void		destroy_simulation(t_sim *sim);
void		start_simulation(t_sim *sim);

// monitor.c
int			is_stopped(t_sim *sim);
void		signal_stop(t_sim *sim);
void		*monitor_routine(void *arg);

// request.c
int			request_dongles(t_coder *c, t_sim *sim);

// release.c
void		release_dongles(t_coder *coder);

// coder_routine.c
void		*coder_routine(void *arg);

// log.c
void		log_msg(t_coder *coder, const char *msg);

// heap.c
void		heap_insert(t_heap *heap, t_request req);
int			heap_peek(t_heap *heap);
void		heap_remove_by_id(t_heap *heap, int coder_id);

#endif