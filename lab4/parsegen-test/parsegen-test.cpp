#include <parser.hpp>

int main ( )
try
{
	parser p(std::cin);

	std::cout << p.parse() << std::endl;
}
catch (std::exception const & e)
{
	std::cout << e.what() << std::endl;
}
