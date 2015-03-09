#include <lexer/lexer.hpp>

int main ( )
{
	lexer::lexer_t l;
	l.add_rule("\\d+", [](lexer::token_t t){ std::cout << "Number: " << std::string(t.begin, t.end) << std::endl; });
	l.add_rule("\\s+", [](lexer::token_t){ std::cout << "Skipped space\n"; });
	l.add_rule("\\.", [&](lexer::token_t){ l.finish(); std::cout << "Finished\n"; });

	l.process(std::cin);
}
