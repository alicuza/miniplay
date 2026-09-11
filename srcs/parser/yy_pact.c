#include "minishell.h"

int32_t	get_yypact(uint64_t idx)
{
	static const int32_t	yypact[] = {
		  8,  -44,   18,   21,   33,  -44,  -44,  -44,   14,   14,
		 30,   14,    8,    8,   17,   16,  -44,   58,  -44,   40,
		 47,  -44,  -44,  -44,  -44,  -44,  -44,  -44,  -44,  -44,
		 24,   33,    7,  -44,    8,    8,    8,   58,  -44,  -44,
		 54,  -44,  -44,   40,  -44,  -44,   17,    8,   17,   33,
		 33,   33,  -44,  -44,  -44,   54,    7,   16,   16,  -44,
		 17
	};
	return (yypact[idx]);
}

int32_t	get_yydefact(uint64_t idx)
{
	static const int32_t	yydefact[] = {
		 45,   42,    0,   44,    3,    1,   43,   24,    0,    0,
		  0,    0,   45,   45,    5,    6,    9,   12,   11,   23,
		 21,   26,   34,   35,   39,   36,   37,   41,   40,   38,
		  0,    0,   44,    2,   45,   45,   45,   13,   32,   30,
		 22,   28,   25,   20,   27,   14,   18,   15,    4,    0,
		  0,    0,   33,   31,   29,   19,   16,    7,    8,   10,
		 17
	};
	return (yydefact[idx]);
}

int32_t	get_yypgoto(uint64_t idx)
{
	static const int32_t	yypgoto[] = {
		-44,  -44,  -44,  -27,  -43,   -7,  -44,  -44,  -44,  -44,
		-44,  -44,  -44,   -9,  -44,  -17,  -44,   13,  -44,  -44,
		-12,   -4
	};
	return (yypgoto[idx]);
}

int32_t	get_yydefgoto(uint64_t idx)
{
	static const int32_t	yydefgoto[] = {
		  0,    2,   13,   14,   15,   16,   17,   30,   47,   18,
		 19,   43,   20,   40,   37,   21,   22,   25,   23,   28,
		  3,    4
	};
	return (yydefgoto[idx]);
}
