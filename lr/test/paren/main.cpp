#include <iostream>
#include "parser.hpp"

struct lexer_t
{
	token_t next ( )
	{
		char c;
		while (c = std::cin.get(), std::cin)
			if (!std::isspace(c)) break;
		if (!std::cin)
			return _EOF_;
		if (c == '(') return LPAREN;
		if (c == ')') return RPAREN;
	}

	int value ( )
	{
		return 0;
	}
};

int main ( )
{
	lexer_t l;
	int d = parse(l);
	std::cout << "Max depth: " << d << std::endl;
}
