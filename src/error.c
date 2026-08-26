/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obehlil <obehlil@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:11:37 by obehlil           #+#    #+#             */
/*   Updated: 2026/08/26 11:11:38 by obehlil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

void	print_error(t_error_code err)
{
	if (err == ERR_OK)
		return ;
	if (err & ERR_INVALID_NUM_PARAMS)
		fprintf(stderr, "ERROR: Invalid Number of parameters\n");
	else if (err & ERR_NULL_PTR)
		fprintf(stderr, "ERROR: BAD Parameter, cannot dereference NULL\n");
	else if (err & ERR_EMPTY_STR)
		fprintf(stderr, "ERROR: Empty string is not accepted as parameter\n");
	else if (err & ERR_INVALID_ARG)
		fprintf(stderr, "ERROR: Invalid number passed as argument\n");
	else if (err & ERR_OVERFLOW)
		fprintf(stderr, "ERROR: Number is larger than the type\n");
	else if (err & ERR_BAD_SCHEDULER)
		fprintf(stderr, "ERROR: scheduler should be fifo or edf\n");
}
