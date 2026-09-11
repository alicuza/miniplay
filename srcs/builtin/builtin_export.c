#include "env.h"
#include "minishell.h"

#define EQUAL 0

static void	print_val(void *content_void_p)
{
	t_env_content	*content;

	content = (t_env_content *)content_void_p;
	if (content->val == NULL)
		printf("declare -x %s\n", (char *)content->key);
	else
		printf("declare -x %s=\"%s\"\n", (char *)content->key, content->val);
}

int	add_args_to_env(t_ctx *c, t_command_ctx *command_ctx)
{
	char		**tmp;
	uint64_t	i;

	i = 1;
	while (i < command_ctx->argc)
	{
		tmp = ft_split_key_value(command_ctx->argv[i], '=');
		if (tmp == NULL)
			return (exit_mem_issue());
		if (is_valid_var_name(tmp[0]) == false || ft_strncmp(tmp[0], SHLVL, 6)
			== EQUAL)
		{
			free_str_arr(tmp);
			i++;
			continue ;
		}
		if (env_update(&c->env, tmp[0], tmp[1]))
		{
			free(tmp);
			return (EXIT_FAILURE);
		}
		free(tmp);
		i++;
	}
	return (EXIT_SUCCESS);
}

/* TODO nik:
2.9.1.1 Order of Processing: if it is export , then any remaining words,
	that would be recognized as a variable assignment in isolation,
	shall be expanded as a variable assignment (tilde expansion after the first <equals-sign> and after any unquoted <colon>,
	parameter expansion, command substitution, arithmetic expansion,
	and quote removal,
	but no field splitting or pathname expansion); while remaining words that would not be a variable assignment in isolation shall be subject to regular expansion (tilde expansion for only a leading <tilde>,
	parameter expansion, command substitution, arithmetic expansion,
	field splitting, pathname expansion,
	and quote removal). For all other command names,
	words after the word that produced the command name shall be subject only to regular expansion. All fields resulting from the expansion of the word that produced the command name and the subsequent words,
	except for the field containing the command name,
	shall be the arguments for the command.

All values undergo variable expansion,
	and quote removal (see Shell Parameter Expansion).
	Word splitting and filename expansion are not performed.


		If any of the assignments attempts to assign a value to a readonly variable,
			an error occurs,
			and the command exits with a non-zero status. see what are readonly ones

*/
int	builtin_export(t_ctx *c, t_command_ctx *command_ctx)
{
	if (command_ctx->argc == 1)
	{
		ft_lstiter(c->env.vals, &print_val);
		return (EXIT_SUCCESS);
	}
	else
		return (add_args_to_env(c, command_ctx));
}
