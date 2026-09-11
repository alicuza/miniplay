#include "minishell.h"

#define EQUAL 0


static void	print_val(char *str)
{
	if (!is_empty_str(str))
		printf("%s", str);
}

static int64_t	find_n_flag(t_command_ctx *command_ctx)
{
	uint64_t i;
	uint64_t j;
	int64_t found_at;

	found_at = -1;
	i = 1;
	j = 1;
	while (i < command_ctx->argc && command_ctx->argv[i][0] == '-')
	{
		if(command_ctx->argv[i][j] == 'n')
		{
			while (command_ctx->argv[i][j] == 'n')
			{
				j++;
			}
			if (command_ctx->argv[i][j] == '\0')
				found_at = i;
		}
		else
			return (found_at);
		j = 1;
		i++;
	}
	return (found_at);
}

int	echo(t_ctx *c, t_command_ctx *command_ctx)
{
	uint64_t i;
	int64_t n_flag_found_at;

	(void) c;
	if (command_ctx->argc == 1)
		print_val("\n");
	else
	{
		n_flag_found_at = find_n_flag(command_ctx);
		if (n_flag_found_at != -1)
			i = n_flag_found_at + 1;
		else
			i = 1;
		while (i < command_ctx->argc)
		{
			print_val(command_ctx->argv[i]);
			if (i + 1 < command_ctx->argc)
				printf(" ");
			i++;
		}
		if (n_flag_found_at == -1)
			printf("\n");
	}
	return (0);
}

