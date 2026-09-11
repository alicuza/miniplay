#include "minishell.h"
#define EQUAL 0

char* env_get(t_env *env, char *key)
{
	t_list *vals;
	size_t key_size;

	vals = env->vals;
	key_size = ft_strlen(key) + 1;
	if (vals)
		while (vals)
		{
			if (ft_strncmp(((t_env_content*) vals->content)->key, key, key_size) == EQUAL)
			{
				if(((t_env_content*) vals->content)->val != NULL)
						return (ft_strdup(((t_env_content*) vals->content)->val));
				return (NULL);
			}
			vals = vals->next;
		}
	return (NULL);
}
