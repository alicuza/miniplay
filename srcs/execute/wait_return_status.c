#include "minishell.h"

static void	soft_exit_on_issue(char *error_prefix)
{
	perror(error_prefix);
}
void	wait_return_status(t_ctx *c)
{
	int		wstatus;
	pid_t	wpid;
	pid_t	pid_to_wait;

	pid_to_wait = c->pid_to_wait;
	while (1)
	{
		wpid = waitpid(-1, &wstatus, 0);
		if (wpid == -1)
		{
			if (errno == EINTR)
				continue ;
			if (errno != ECHILD)
				return (soft_exit_on_issue("waitpid"));
			break ;
		}
		if (wpid == pid_to_wait)
		{
			if (WIFSIGNALED(wstatus))
				c->return_status = 128 + WTERMSIG(wstatus);
			else if (WIFEXITED(wstatus))
				c->return_status = WEXITSTATUS(wstatus);
		}
#ifdef DEBUG
		fprintf(stderr, "\nreaped pid=%jd\n", (intmax_t)wpid);
		if (WIFEXITED(wstatus))
			printf("exited, status=%d\n", WEXITSTATUS(wstatus));
		else if (WIFSIGNALED(wstatus))
			printf("signaled, sig=%d\n", WTERMSIG(wstatus));
#endif
	}
	c->pid_to_wait = -1;
	g_signal = 0;
}
