#include <stdexcept>

#include "io_utils.h"

struct rule
{
	std::string regex;
	std::string code;

	rule ( ) = default;

	rule (strings_t const & lines)
	{
		regex = lines.at(0);
		if (lines.size() > 1)
		{
			code = concat(lines.begin() + 1, lines.end());
		}
	}
};

void generate_header (std::string const & lexer_name, strings_t const & user_code,
							 strings_t const & types)
{
	std::ofstream out(lexer_name + ".hpp");

	// header
	out << "#pragma once\n";
	out << "#include <iostream>\n";
	out << "#include <tuple>\n";
	out << "\n";
	out << "#include <boost/regex.hpp>\n";
	out << "\n";
	out << concat(user_code.begin(), user_code.end());
	out << "\n";

	out << "namespace detail\n{\n";

	// tuple indices
	out << tab << "template <typename T>\n";
	out << tab << "struct tuple_index;\n";
	for (std::size_t i = 0; i < types.size(); ++i)
	{
		out << "\n";
		out << tab << "template < >\n";
		out << tab << "struct tuple_index<" << types[i] << ">\n";
		out << tab << "{ static const std::size_t value = " << i << "; };\n";
	}
	out << "\n";

	// tuple type
	out << tab << "typedef std::tuple<";
	for (std::size_t i = 0; i < types.size(); ++i)
	{
		if (i > 0)
			out << ", ";
		out << types[i];
	}
	out << "> value_type;\n";

	out << "}\n";
	out << "\n";

	// lexer class
	out << "struct " << lexer_name << "\n";
	out << "{\n";
	out << tab << lexer_name << " (std::istream & is);\n";
	out << "\n";
	out << tab << "template <typename T>\n";
	out << tab << "T & value ( )\n";
	out << tab << "{ return std::get<detail::tuple_index<T>::value>(value_); }\n";
	out << "\n";
	out << tab << "std::string & term ( )\n";
	out << tab << "{ return term_; }\n";
	out << "\n";
	out << tab << "void next ( );\n";
	out << "\n";
	out << "private:\n";
	out << tab << "std::string data_;\n";
	out << tab << "std::string::const_iterator pos_;\n";
	out << "\n";
	out << tab << "std::string term_;\n";
	out << tab << "detail::value_type value_;\n";
	out << "\n";
	out << tab << "std::vector<boost::regex> rules_;\n";
	out << "};\n";
}

void generate_source (std::string const & lexer_name, strings_t const & user_code, std::vector<rule> const & rules)
{
	std::ofstream out(lexer_name + ".cpp");

	// header
	out << "#include \"" << lexer_name << ".hpp\"\n";
	out << "\n";
	out << concat(user_code.begin(), user_code.end());
	out << "\n";

	// lexer constructor
	out << lexer_name << "::" << lexer_name << " (std::istream & is)\n";
	out << "{\n";
	for (auto const & r : rules)
	{
		out << tab << "rules_.emplace_back(boost::regex{R\"(" + r.regex + ")\"});\n";
	}
	out << "\n";
	out << tab << "std::ostringstream oss;\n";
	out << tab << "oss << is.rdbuf();\n";
	out << tab << "data_ = oss.str();\n";
	out << tab << "pos_ = data_.begin();\n";
	out << tab << "next();\n";
	out << "}\n";
	out << "\n";

	// lexer next() method
	out << "void " << lexer_name << "::next ( )\n";
	out << "{\n";
	out << tab << "term().clear();\n";
	out << "\n";
	out << tab << "while (term().empty())\n";
	out << tab << "{\n";

	out << tab << tab << "if (pos_ == data_.cend())\n";
	out << tab << tab << tab << "return;\n";
	out << "\n";

	out << tab << tab << "boost::smatch m;\n";
	out << tab << tab << "std::size_t i;\n";
	out << tab << tab << "for (i = 0; i < rules_.size(); ++i)\n";
	out << tab << tab << "{\n";
	out << tab << tab << tab << "if (boost::regex_search(pos_, data_.cend(), m, rules_[i]) && ";
	out << "m[0].first == pos_)\n";
	out << tab << tab << tab << tab << "break;\n";
	out << tab << tab << "}\n";
	out << "\n";

	out << tab << tab << "if (i == rules_.size())\n";
	out << tab << tab << tab << "throw std::runtime_error(\"unknown token \" + std::string(pos_, m[0].first));\n";
	out << "\n";

	out << tab << tab << "auto begin = m[0].first;\n";
	out << tab << tab << "auto end = m[0].second;\n";
	out << "\n";

	out << tab << tab << "switch (i)\n";
	out << tab << tab << "{\n";
	for (std::size_t i = 0; i < rules.size(); ++i)
	{
		out << tab << tab << "case " << i << ":\n";
		out << tab << tab << tab << "{\n";
		out << tab << tab << tab << "// user code here\n";
		out << rules[i].code;
		out << tab << tab << tab << "}\n";
		out << tab << tab << tab << "break;\n";
	}
	out << tab << tab << "}\n";
	out << "\n";

	out << tab << tab << "pos_ = end;\n";

	out << tab << "}\n";
	out << "}\n";
}

int main (int argc, char ** argv)
try
{
	if (argc != 2)
	{
		std::cerr << "Usage: lexgen <input-file>" << std::endl;
		return 0;
	}

	std::ifstream input(argv[1]);

	std::string lexer_name = getline(input);
	expect_separator(input);

	auto user_header_code = read_until_separator(input);
	auto user_source_code = read_until_separator(input);

	auto types = read_until_separator(input);

	std::vector<rule> rules;

	while (true)
	{
		auto next_rule = read_until_separator(input);
		if (next_rule.empty())
			break;
		rules.emplace_back(next_rule);
	}

	generate_header(lexer_name, user_header_code, types);
	generate_source(lexer_name, user_source_code, rules);
}
catch (std::exception const & e)
{
	std::cerr << "Error: " << e.what() << std::endl;
	return 1;
}
