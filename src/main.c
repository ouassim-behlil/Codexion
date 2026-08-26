/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obehlil <obehlil@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:11:29 by obehlil           #+#    #+#             */
/*   Updated: 2026/08/26 11:11:30 by obehlil          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/codexion.h"

int	main(int argc, const char *argv[])
{
	t_sim	sim;

	if (init_simulation(&sim, argc, argv))
		return (-1);
	start_simulation(&sim);
	destroy_simulation(&sim);
	return (0);
}
