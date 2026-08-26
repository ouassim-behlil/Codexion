/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obehlil <obehlil@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:11:43 by obehlil           #+#    #+#             */
/*   Updated: 2026/08/26 11:11:43 by obehlil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	log_msg(t_coder *coder, const char *msg)
{
	long	rel_time;

	if (!msg)
		return ;
	rel_time = get_time_ms() - coder->sim->start_ms;
	pthread_mutex_lock(&coder->sim->log_lock);
	if (coder->sim->stop == 1)
	{
		pthread_mutex_unlock(&coder->sim->log_lock);
		return ;
	}
	fprintf(stdout, "%ld %d %s\n", rel_time, coder->id, msg);
	pthread_mutex_unlock(&coder->sim->log_lock);
}
