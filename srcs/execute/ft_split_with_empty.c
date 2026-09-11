/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_with_empty.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nribakov <nribakov@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 14:43:42 by nribakov          #+#    #+#             */
/*   Updated: 2026/09/11 14:11:13 by sancuta          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	ft_number_of_element(char const *s, char c)
{
	size_t	i;
	int		result;

	i = 0;
	result = 1;
	if (s != NULL)
		while (s[i])
		{
			if (s[i] == c)
			{
				result++;
				i++;
			}
			else
			{
				while (s[i] && s[i] != c)
					i++;
			}
		}
	return (result);
}

static char	*get_substring(char const *s, char c, int n)
{
	size_t	i;
	size_t	start;
	int		substr_index;

	i = 0;
	substr_index = 0;
	start = i;
	while (s[i])
	{
		if (s[i] == c)
		{
			if (substr_index == n)
				return (ft_substr(s, start, i - start));
			else
			{
				start = i + 1;
			}
			substr_index++;
		}
		i++;
	}
	if (substr_index == n)
		return (ft_substr(s, start, i - start));
	return (ft_strdup(""));
}

static void	ft_free_everything(char **result, int n)
{
	int	i;

	i = 0;
	while (i < n && result[i])
	{
		free(result[i]);
		i++;
	}
	free(result);
}

char	**ft_split_with_empty(char const *s, char c)
{
	char	**result;
	int		number_of_element;
	int		i;

	number_of_element = ft_number_of_element(s, c);
	result = ft_calloc(sizeof(char *), (number_of_element + 1));
	if (!result)
		return (NULL);
	i = 0;
	while (i < number_of_element)
	{
		result[i] = get_substring(s, c, i);
		if (!result[i])
		{
			ft_free_everything(result, i);
			return (NULL);
		}
		i++;
	}
	result[number_of_element] = NULL;
	return (result);
}

// void	test_number_of_element(void)
// {
// 	printf("\"\": %d \n", ft_number_of_element("", ':'));
// 	printf("\":\": %d \n", ft_number_of_element(":", ':'));
// 	printf("\"::\": %d \n", ft_number_of_element("::", ':'));
// 	printf("\"a\": %d \n", ft_number_of_element("a", ':'));
// 	printf("\":a\": %d \n", ft_number_of_element(":a", ':'));
// 	printf("\"a:\": %d \n", ft_number_of_element("a:", ':'));
// 	printf("\":a:\": %d \n", ft_number_of_element(":a:", ':'));
// 	printf("\"a:b\": %d \n", ft_number_of_element("a:b", ':'));
// 	printf("\":a:b:\": %d \n", ft_number_of_element(":a:b:", ':'));
// 	printf("\"a:b:\": %d \n", ft_number_of_element("a:b:", ':'));
// 	printf("\":a:b\": %d \n", ft_number_of_element(":a:b", ':'));
// 	printf("\"a:b:c\": %d \n\n", ft_number_of_element("a:b:c", ':'));
// }

// int	main(void)
// {
// 	char	**result;
// 	char	*str;
// 	char	c;

// 	// test_number_of_element();
// 	str = ":Hello::Cat:and:Dog:";
// 	c = ':';
// 	result = ft_split(str, c);
// 	if (result != NULL)
// 	{
// 		for (int i = 0; result[i]; i++)
// 			printf("%s\n", result[i]);
// 		ft_free_all(result, ft_number_of_element(str, c));
// 	}
// 	return (0);
// }
