#include <lexer.hpp>

int main ( )
{
	lexer l(std::cin);

	while (!l.term().empty())
	{
		std::cout << l.term() << std::endl;
		if (l.term() == "Num")
			std::cout << "    " << l.value<int>() << std::endl;
		if (l.term() == "Op")
			std::cout << "    " << l.value<std::string>() << std::endl;
		l.next();
	}
}
