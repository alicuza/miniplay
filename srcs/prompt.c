/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/25 12:41:14 by sancuta           #+#    #+#             */
/*   Updated: 2026/09/02 12:11:15 by nribakov         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

#define GREEN "\001\033[38;5;40m\002"
#define RESET "\001\033[0m\002"
#define USER "USER"
#define LOGNAME "LOGNAME"
#define ANON "anonymous"

static bool	add_hostname_to_prompt(t_arena *prompt)
{
	int		fd;
	size_t	offset;
	size_t	read_len;
	size_t	size;

	errno = 0;
	fd = open("/proc/sys/kernel/hostname", O_RDONLY);
	if (fd < 0 && errno)
		return (/* error_log("add_hostname_to_prompt", "read", strerror(errno)), */ false);
	size = prompt->cap + prompt->stride - prompt->offset;
	offset = arena_alloc(prompt, size, 1) - 1;
	read_len = read(fd, prompt->buf + offset, size);
	while (read_len == size)
	{
		if (ft_indchr(prompt->buf + offset, '\n') != -1)
			break ;
		size = prompt->cap;
		offset = arena_alloc(prompt, size, 1) - 1;
		read_len = read(fd, prompt->buf + offset, size);
	}
	close(fd);
	prompt->offset = prompt->stride
		+ word_len(prompt->buf + prompt->stride, '\n') + 1;
	return (true);
}

static void	init_prompt(t_arena *prompt)
{
	char	*user;
	size_t	save_offset;

	user = getenv(USER);
	if(!user)
		user = getenv(LOGNAME);
	if (!user)
		user = ANON;
	arena_clear(prompt);
	arena_strlcpy(prompt, GREEN, ft_strlen(GREEN) + 1);
	arena_strlcat(prompt, user, ft_strlen(user) + 1);
	save_offset = arena_save(prompt);
	arena_strlcat(prompt, "@", 2);
	if (!add_hostname_to_prompt(prompt))
		arena_restore(prompt, save_offset);
}

static bool	add_cwd_to_prompt(t_ctx *c)
{
	char	*cwd;
	t_arena	*prompt;
	size_t	size;
	size_t	tmp_offset;

	prompt = &(c->arena[AT_PROMPT]);
	arena_strlcat(prompt, ":", 2);
	size = prompt->cap + prompt->stride - prompt->offset;
	tmp_offset = arena_alloc(prompt, size, 1) - 1; // essentially doubling the capacity
	errno = 0;
	cwd = getcwd(prompt->buf + tmp_offset, size);
	while (!cwd && errno)
	{
		if (errno != ERANGE)
					// TODO stefan: implement error logging for debug
			return (/* error_log("get_prompt", "get_cwd", strerror(errno)), */ false);
		size = prompt->cap;
		init_prompt(prompt);
		arena_strlcat(prompt, ":", 2);
		tmp_offset = arena_alloc(prompt, size, 1) - 1;
		errno = 0;
		cwd = getcwd(prompt->buf + tmp_offset, size);
	}
	prompt->offset = prompt->stride
		+ word_len(prompt->buf + prompt->stride, ' ') + 1;
	return (true);
}

static void	end_prompt(t_arena *prompt, int ret)
{
	arena_strlcat(prompt, "[", 2);
	--(prompt->offset);
	arena_itoa(prompt, ret); // to put the return of the last command
	arena_strlcat(prompt, "]", 2);
	arena_strlcat(prompt, SHELLNAME, ft_strlen(SHELLNAME) + 1);
	arena_strlcat(prompt, "$ ", 3);
	arena_strlcat(prompt, RESET, ft_strlen(RESET) + 1);
}

char	*get_prompt(t_ctx *c, bool with_cwd)
{
	t_arena	*prompt;

	prompt = &(c->arena[AT_PROMPT]);
	init_prompt(prompt);
	if (with_cwd && !add_cwd_to_prompt(c))
		return (get_prompt(c, NO_CWD));
	end_prompt(prompt, c->return_status);
	return ((char *)get_ptr_from_offset(prompt, prompt->stride));
}

/* backup in case of other prompt related problems.
char	*get_prompt(t_ctx *c, bool with_cwd)
{
	(void)c;
	return ("shni $ ");
}
*/
