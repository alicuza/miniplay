#include "minishell.h"

size_t	ft_safe_strlen(const char *s)
{
  if (s == NULL)
    return (0);
  else
    return (ft_strlen(s));
}

char *join_key_value(t_env_content *content)
{
    int key_val_len;
    char *join;

    key_val_len = ft_safe_strlen(content->key) + ft_safe_strlen(content->val) + 1;
    join = malloc(sizeof(char) * (key_val_len + 1));
    if(!join)
      return NULL;
    ft_strlcpy(join, content->key, key_val_len + 1);
    ft_strlcat(join, "=", key_val_len + 1);
    ft_strlcat(join, content->val, key_val_len + 1);
  return join;
}

char **env_to_envp(t_env *env)
{
  char **envp;
  t_list *env_vals;
  int env_len;
  int i;

  env_vals = env->vals;
  env_len = ft_lstsize(env_vals);
  envp = malloc(sizeof(char *) * (env_len + 1));
  if(envp == NULL)
    return NULL;
  i = 0;
  while(i < env_len)
  {
    envp[i] = join_key_value((t_env_content*) env_vals->content);
    if(envp[i] == NULL)
      while(i <= 0)
      {
        free(envp[i]);
        i--;
        return NULL;
      }
	env_vals = env_vals->next;
    i++;
  }
  envp[env_len] = NULL;
  return envp;
}
