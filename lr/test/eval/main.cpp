#include <iostream>
#include <sstream>
#include <lexer/lexer.hpp>

typedef double (*func_t)(double);

func_t func[] = { &sin, &cos, &exp, &log };

#include "parser.hpp"

#include <boost/lexical_cast.hpp>

struct lexer_t
	: lexer::lexer_t
{
	lexer_t ( )
	{
		add_rule("sin", [&](lexer::token_t){ tokens.push_back({FUNC, 0}); });
		add_rule("cos", [&](lexer::token_t){ tokens.push_back({FUNC, 1}); });
		add_rule("exp", [&](lexer::token_t){ tokens.push_back({FUNC, 2}); });
		add_rule("ln", [&](lexer::token_t){ tokens.push_back({FUNC, 3}); });
		add_rule("pi", [&](lexer::token_t){ tokens.push_back({CONST, M_PI}); });
		add_rule("e", [&](lexer::token_t){ tokens.push_back({CONST, M_E}); });
		add_rule("\\+", [&](lexer::token_t){ tokens.push_back({SUM, 0}); });
		add_rule("\\-", [&](lexer::token_t){ tokens.push_back({SUB, 0}); });
		add_rule("\\*", [&](lexer::token_t){ tokens.push_back({MULT, 0}); });
		add_rule("\\/", [&](lexer::token_t){ tokens.push_back({DIV, 0}); });
		add_rule("\\(", [&](lexer::token_t){ tokens.push_back({LPAREN, 0}); });
		add_rule("\\)", [&](lexer::token_t){ tokens.push_back({RPAREN, 0}); });
		add_rule("\\d+(\\.\\d+)?", [&](lexer::token_t t){
			std::string str(t.begin, t.end);
			tokens.push_back({N, boost::lexical_cast<double>(str)});
		});
		add_rule(" ", [](lexer::token_t){});

		i = -1;
	}

	std::vector<std::pair<token_t, double>> tokens;
	int i;

	token_t next ( ) { ++i; return tokens[i].first; }
	double value ( ) { return tokens[i].second; }
};

int main ( )
{
	std::string line;
	std::getline(std::cin, line);
	std::istringstream iss(line);

	lexer_t l;
	l.process(iss);
	l.tokens.push_back({_EOF_, 0});
	auto r = parse(l);
	std::cout << "Result: " << r << std::endl;
}
