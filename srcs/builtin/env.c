#include "minishell.h"

static void	print_val(void *content_void_p)
{
	t_env_content *content;

	content = (t_env_content *)content_void_p;
	if (content->val != NULL)
		printf("%s=%s\n", (char *)content->key, content->val);
}

int	env(t_ctx *c, t_command_ctx *command_ctx)
{
#ifdef DEBUG
	fprintf(stderr, "\nExecuting env:\n");
#endif


	(void)command_ctx;
	t_list *env = c->env.vals;
	if (env)
	{
		ft_lstiter(env, &print_val);
		return (EXIT_SUCCESS);
	}
	else
	{
		ft_putstr_fd("env: enviroment is empty\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
}
