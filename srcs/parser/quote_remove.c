#include "minishell.h"

char	*quote_remove_inplace(char *s)
{
	size_t	r;
	size_t	w;
	ssize_t	end;

	r = 0;
	w = 0;
	while (s[r])
	{
		end = -1;
		if (is_char_in_set(s[r], QUOTE_SET))
			end = ft_indchr(s + r + 1, s[r]);
		if (end < 0)
			s[w++] = s[r++];
		else
		{
			r++;
			while (end-- > 0)
				s[w++] = s[r++];
			r++;
		}
	}
	s[w] = '\0';
	return (s);
}
