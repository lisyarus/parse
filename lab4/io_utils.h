#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

std::string getline (std::istream & is)
{
	std::string line;
	std::getline(is, line);
	if (!is)
		throw std::runtime_error("unexpected end");
	return line;
}

std::string const separator = "%%";

void expect_separator (std::istream & is)
{
	if (getline(is) != separator)
		throw std::runtime_error("separator (%%) expected");
}

typedef std::vector<std::string> strings_t;

strings_t read_until_separator (std::istream & is)
{
	strings_t result;
	while (true)
	{
		std::string line = getline(is);
		if (line == separator)
			break;
		else
			result.push_back(line);
	}
	return result;
}

template <typename Iterator>
std::string concat (Iterator begin, Iterator end, std::string const & delimiter = "\n")
{
	return std::accumulate(begin, end, std::string(),
		[&](std::string const & lhs, std::string const & rhs)
	{
		return lhs + rhs + delimiter;
	});
}

std::string const & tab = "  ";

void skip_ws (std::istream & is)
{
	while (std::isspace(is.peek()))
		is.get();
}

std::map<std::string, std::string> get_tokens_with_types (std::istream & is)
{
	std::map<std::string, std::string> result;

	for (auto const & line : read_until_separator(is))
	{
		std::istringstream iss(line);
		std::string name, type;
		iss >> name;
		skip_ws(iss);
		type = getline(iss);
		result[name] = type;
	}

	return result;
}
