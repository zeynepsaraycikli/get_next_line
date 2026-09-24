/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zesarayc <zesarayc@student.42istanbul.c    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/15 20:17:05 by zesarayc          #+#    #+#             */
/*   Updated: 2026/09/23 03:31:30 by zesarayc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*fill_stash(int fd, char *stash)
{
	char	*buf;
	char	*tmp;
	int		nb;

	buf = malloc(sizeof(char) * ((size_t)BUFFER_SIZE + 1));
	if (!buf)
		return (NULL);
	nb = 1;
	while (!ft_strchr(stash, '\n') && nb != 0)
	{
		nb = read(fd, buf, BUFFER_SIZE);
		if (nb == -1)
		{
			free(buf);
			free(stash);
			return (NULL);
		}
		buf[nb] = '\0';
		tmp = stash;
		stash = ft_strjoin(tmp, buf);
		if (!stash)
			break ;
	}
	free(buf);
	return (stash);
}

static char	*fetch_line(char *stash)
{
	char	*line;
	size_t	i;

	i = 0;
	if (!stash || !stash[0])
		return (NULL);
	while (stash[i] && stash[i] != '\n')
		i++;
	line = malloc(sizeof(char) * (i + 2));
	if (!line)
		return (NULL);
	i = 0;
	while (stash[i] && stash[i] != '\n')
	{
		line[i] = stash[i];
		i++;
	}
	if (stash[i] == '\n')
		line[i++] = '\n';
	line[i] = '\0';
	return (line);
}

static char	*trim_stash(char *stash)
{
	char	*remainder;
	size_t	i;
	size_t	j;

	i = 0;
	while (stash[i] && stash[i] != '\n')
		i++;
	if (!stash[i] || !stash[i + 1])
	{
		free(stash);
		return (NULL);
	}
	i++;
	remainder = malloc(sizeof(char) * (ft_strlen(stash) - i + 1));
	if (!remainder)
	{
		free(stash);
		return (NULL);
	}
	j = 0;
	while (stash[i])
		remainder[j++] = stash[i++];
	remainder[j] = '\0';
	free(stash);
	return (remainder);
}

char	*get_next_line(int fd)
{
	static char	*stash;
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	stash = fill_stash(fd, stash);
	if (!stash)
		return (NULL);
	line = fetch_line(stash);
	stash = trim_stash(stash);
	return (line);
}
