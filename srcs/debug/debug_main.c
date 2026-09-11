#include "minishell.h"

void	debug_print_read_line(t_ctx *c)
{
	if (c->dbg.states & DBG_PARSER)
	{
		fprintf(stderr, "\n--- read_line ---\n");
		fprintf(stderr, "%s\n", c->read_line);
	}
	if (c->dbg.arenas & DBG_ARENA_PROMPT)
	{
		fprintf(stderr, "\n--- prompt arena after get_prompt ---\n");
		print_arena(&c->arena[AT_PROMPT]);
	}
}

void	debug_print_after_parse(t_ctx *c, t_parser_state *parse)
{
	if (c->dbg.scope & SCOPE_TOKENS)
		print_tokens(stdout, c);
	if (c->dbg.arenas & DBG_ARENA_TOKENS)
		print_arena(&c->arena[AT_TOKENS]);
	if (c->dbg.scope & SCOPE_STACK)
		print_stack(stdout, c, parse);
	if (c->dbg.arenas & DBG_ARENA_STACK)
		print_arena(&c->arena[AT_STACK]);
	if (c->dbg.scope & SCOPE_COMMAND)
		print_nodes(stdout, c);
	if (c->dbg.arenas & DBG_ARENA_COMMAND)
		print_arena(&c->arena[AT_COMMAND]);
}

static const t_dbg_flag	g_scope_flags[] = {
{"tokens", SCOPE_TOKENS}, {"stack", SCOPE_STACK},
{"command", SCOPE_COMMAND}, {"trace", SCOPE_TRACE}, {NULL, 0}};
static const t_dbg_flag	g_state_flags[] = {
{"lexer", DBG_LEXER}, {"parser", DBG_PARSER}, {"here", DBG_HEREDOC}, {NULL, 0}};
static const t_dbg_flag	g_arena_flags[] = {
{"prompt", DBG_ARENA_PROMPT}, {"string", DBG_ARENA_STRING},
{"tokens", DBG_ARENA_TOKENS}, {"stack", DBG_ARENA_STACK},
{"command", DBG_ARENA_COMMAND}, {NULL, 0}};
static const t_dbg_flag	g_parser_flags[] = {
{"flags", DBG_SHOW_FLAGS}, {"stack", DBG_SHOW_STACK}, {"action", DBG_SHOW_ACTION},
{"nodes", DBG_SHOW_NODES}, {"links", DBG_SHOW_LINKS}, {NULL, 0}};

static const char	*match_prefix(const char *arg, const char *prefix)
{
	size_t	len;

	if (!prefix)
		return (NULL);
	len = ft_strlen(prefix);
	if (!ft_strncmp(arg, prefix, len) && arg[len])
		return (arg + len);
	return (NULL);
}

/* returns the matched category index, or -1 if arg is not a --flag= list */
static int	apply_arg(const t_dbg_cat *cats, const char *arg)
{
	const char	*spec;
	int			i;

	i = 0;
	while (cats[i].prefix)
	{
		spec = match_prefix(arg, cats[i].prefix);
		if (!spec)
			spec = match_prefix(arg, cats[i].alias);
		if (spec)
		{
			parse_flag_list(spec, cats[i].mask, cats[i].flags, cats[i].all);
			return (i);
		}
		++i;
	}
	return (-1);
}

static void	apply_defaults(const t_dbg_cat *cats, const bool *seen)
{
	int	i;

	i = 0;
	while (i < DBG_CAT_COUNT)
	{
		if (!seen[i])
			*cats[i].mask = cats[i].def;
		++i;
	}
}

void	parse_debug_args(int argc, char **argv, t_ctx *c)
{
	const t_dbg_cat	cats[] = {
	{"--scope=", "--tests=", &c->dbg.scope, g_scope_flags,
		SCOPE_ALL, DBG_DEFAULT_SCOPE},
	{"--states=", NULL, &c->dbg.states, g_state_flags,
		DBG_ALL_STATES, DBG_DEFAULT_STATES},
	{"--arenas=", NULL, &c->dbg.arenas, g_arena_flags,
		DBG_ARENA_ALL, DBG_DEFAULT_ARENAS},
	{"--parser=", NULL, &c->dbg.parser, g_parser_flags,
		DBG_SHOW_ALL, DBG_DEFAULT_PARSER},
	{NULL, NULL, NULL, NULL, 0, 0}};
	bool			seen[DBG_CAT_COUNT];
	int				i;
	int				cat;

	ft_memset(seen, 0, sizeof(seen));
	i = 0;
	while (++i < argc)
	{
		if (!ft_strncmp(argv[i], "--no_exec", 10))
			c->dbg.no_exec = true;
		else
		{
			cat = apply_arg(cats, argv[i]);
			if (cat >= 0)
				seen[cat] = true;
		}
	}
	apply_defaults(cats, seen);
}
