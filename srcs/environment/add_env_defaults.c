#include "env.h"
#include "minishell.h"

static int	add_pwd(t_env *env)
{
	char	*pwd;
	char	*key;
	char	*value;

	pwd = env_get(env, PWD);
	if (is_empty_str(pwd))
	{
		key = ft_strdup(PWD);
		value = getcwd(NULL, 0);
		if (key == NULL || value == NULL)
		{
			free(key);
			free(value);
			return (exit_mem_issue());
		}
		return (env_add(env, key, value));
	}
	free(pwd);
	return (EXIT_SUCCESS);
}

static int	add_path(t_env *env)
{
	char	*path;
	char	*key;
	char	*value;

	path = env_get(env, PATH);
	if (is_empty_str(path))
	{
		key = ft_strdup(PATH);
		value = ft_strdup(DEFAULT_PATH);
		if (key == NULL || value == NULL)
		{
			free(key);
			free(value);
			return (exit_mem_issue());
		}
		return (env_add(env, key, value));
	}
	free(path);
	return (EXIT_SUCCESS);
}

static int	increase_shlvl(t_env *env, char *shlvl)
{
	int	tmp;
	char	*key;
	char	*value;

	tmp = ft_atoi(shlvl);
	tmp += 1;
	key = ft_strdup(SHLVL);
	value = ft_itoa(tmp);
	if (key == NULL || value == NULL)
	{
		free(key);
		free(value);
		return (exit_mem_issue());
	}
	return (env_update(env, key, value));
}

static int	add_shlvl(t_env *env)
{
	char	*shlvl;
	char	*key;
	char	*value;
	int		status;

	shlvl = env_get(env, SHLVL);
	if (is_empty_str(shlvl))
	{
		key = ft_strdup(SHLVL);
		value = ft_strdup(DEFAULT_SHLVL);
		if (key == NULL || value == NULL)
		{
			free(key);
			free(value);
			return (exit_mem_issue());
		}
		return (env_add(env, key, value));
	}
	else
	{
		status = increase_shlvl(env, shlvl);
		free(shlvl);
		return (status);
	}
}

int	add_env_defaults(t_env *env)
{
	if (add_pwd(env) != EXIT_FAILURE && add_path(env) != EXIT_FAILURE
		&& add_shlvl(env) != EXIT_FAILURE)
		return (EXIT_SUCCESS);
	return (EXIT_FAILURE);
}
