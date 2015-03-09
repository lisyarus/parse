#include <map>
#include <set>
#include <stdexcept>

#include "io_utils.h"

enum rule_element_type_t { RE_TERM, RE_NTERM };

struct rule_element_t
{
	std::string name;
	rule_element_type_t type;
};

struct rule_t
{
	std::vector<rule_element_t> rule;

	std::map<std::size_t, std::string> action;
};

std::string to_string (rule_t const & r)
{
	std::string res;
	for (auto const & e : r.rule)
		res += " " + e.name;
	return res;
}

typedef std::map<std::string, std::vector<rule_t>> rules_map_t;

typedef rules_map_t rules_t;

bool merge_into (std::set<std::string> & dest, std::set<std::string> const & src)
{
	bool changed = false;
	for (auto it = src.begin(); it != src.end(); ++it)
	{
		changed |= dest.insert(*it).second;
	}
	return changed;
}

bool intersect (std::set<std::string> const & s1, std::set<std::string> const & s2)
{
	for (auto const & x : s1)
	{
		if (s2.count(x) > 0)
			return true;
	}
	return false;
}

struct first_t
{
	first_t (rules_t const & rules)
	{
		build(rules);
	}

	std::set<std::string> operator() (rule_element_t const & e)
	{
		switch (e.type)
		{
		case RE_TERM:
			return {e.name};
		case RE_NTERM:
			return first_[e.name];
		}

		return {};
	}

	template <typename Iterator>
	std::set<std::string> operator() (Iterator from, Iterator to)
	{
		std::set<std::string> f;

		bool empty = true;

		for (; from != to; ++from)
		{
			if (from->type == RE_TERM)
			{
				f.insert(from->name);
				empty = false;
				break;
			}
			else
			{
				merge_into(f, first_[from->name]);
				if ((f.count("") > 0))
				{
					f.erase("");
				}
				else
				{
					empty = false;
					break;
				}
			}
		}

		if (empty)
		{
			f.insert("");
		}

		return f;
	}

private:
	std::map<std::string, std::set<std::string>> first_;

	void build (rules_t const & rules)
	{
		bool changed = true;
		while (changed)
		{
			changed = false;

			for (auto const & rs : rules)
			{
				std::string const & n = rs.first;
				for (auto const & r : rs.second)
				{
					if (r.rule.empty())
					{
						changed |= first_[n].insert("").second;
					}
					else
					{
						auto it = r.rule.begin();

						while (it != r.rule.end())
						{
							changed |= merge_into(first_[n], (*this)(*it));

							if (it->type == RE_TERM)
							{
								break;
							}
							else
							{
								if (first_[it->name].count("") > 0)
									++it;
								else
									break;
							}
						}

						if (it == r.rule.end())
						{
							changed |= first_[n].insert("").second;
						}
					}
				}
			}
		}
	}
};

struct follow_t
{
	follow_t (rules_t const & rules, std::string const & s, first_t & first)
	{
		build(rules, s, first);
	}

	std::set<std::string> operator() (std::string const & n)
	{
		return follow_[n];
	}

private:
	std::map<std::string, std::set<std::string>> follow_;

	void build (rules_t const & rules, std::string const & s, first_t & first)
	{
		follow_[s].insert("\0");

		bool changed = true;
		while (changed)
		{
			changed = false;

			for (auto const & rs : rules)
			{
				for (auto const & r : rs.second)
				{
					if (!r.rule.empty())
					{
						auto it = --r.rule.end();

						if (r.rule.size() > 1)
						do
						{
							--it;

							if (it->type == RE_NTERM)
							{
								auto f = first(*std::next(it));
								f.erase("");

								changed |= merge_into(follow_[it->name], f);
							}

						} while (it != r.rule.begin());

						it = r.rule.end();

						do
						{
							--it;

							if (it->type == RE_NTERM)
							{
								changed |= merge_into(follow_[it->name], follow_[rs.first]);
							}

							if (it->type == RE_TERM || first(*it).count("") == 0)
								break;
						} while (it != r.rule.begin());
					}
				}
			}
		}
	}
};

void generate_header (std::string const & parser_name, std::string const & lexer_name,
							 strings_t const & user_header_code,
							 std::map<std::string, std::string> const & non_terminals,
							 std::map<std::string, std::set<std::string>> const & parents,
							 std::string const & start, rules_t const & rules)
{
	auto get_type = [&](std::string const & name) -> std::string
	{
		auto it = non_terminals.find(name);
		if (it != non_terminals.end())
			return it->second;
		else
			return "void";
	};

	auto void_type = [&](std::string const & name)
	{
		return get_type(name) == "void";
	};

	std::ofstream out(parser_name + ".hpp");

	out << "#pragma once\n";

	out << "\n";
	out << concat(user_header_code.begin(), user_header_code.end());
	out << "\n";

	out << "struct " << parser_name << "\n";
	out << "{\n";
	out << tab << parser_name << " (std::istream & is)\n";
	out << tab << tab << ": lexer_(is)\n";
	out << tab << "{ }\n\n";

	out << tab << get_type(start) << " parse ( )\n";
	out << tab << "{ return parse_" << start << "(";
	for (auto const & p : parents.at(start))
	{
		bool first = true;
		if (!void_type(p))
		{
			if (!first) out << ", ";
			first = false;

			out << get_type(p) << "()";
		}
	}
	out << "); }\n\n";;

	for (auto const & n : rules)
	{
		out << tab << get_type(n.first) << " ";
		out << "parse_" << n.first << " (";
		bool first = true;
		for (auto const & p : parents.at(n.first))
		{
			if (!void_type(p))
			{
				if (!first) out << ", ";
				first = false;

				out << get_type(p) << " x" << p;
			}
		}
		out << ");\n";
	}

	out << "\nprivate:\n";
	out << tab << lexer_name << " lexer_;\n";
	out << "};\n\n";
}

void generate_source (std::string const & parser_name,
							 strings_t const & user_source_code,
							 rules_t const & rules,
							 std::map<std::string, std::string> const & non_terminals,
							 std::map<std::string, std::string> const & terminals,
							 std::map<std::string, std::set<std::string>> const & parents,
							 first_t & first,
							 follow_t & follow)
{
	auto get_type = [&](std::string const & name) -> std::string
	{
		auto it = terminals.find(name);
		if (it != terminals.end())
			return it->second;

		it = non_terminals.find(name);
		if (it != non_terminals.end())
			return it->second;

		return "void";
	};

	auto void_type = [&](std::string const & name)
	{
		return get_type(name) == "void";
	};

	auto get_action = [](rule_t const & r, std::size_t i) -> std::string
	{
		auto it = r.action.find(i);
		if (it != r.action.end())
			return it->second;
		return "";
	};

	std::ofstream out(parser_name + ".cpp");

	out << "#include \"" << parser_name << ".hpp\"\n\n";

	out << "\n";
	out << concat(user_source_code.begin(), user_source_code.end()) << "\n";
	out << "\n";

	for (auto const & n : rules)
	{
		out << get_type(n.first) << " ";
		out << parser_name << "::parse_" << n.first << " (";

		bool first_parent = true;
		for (auto const & p : parents.at(n.first))
		{
			if (!void_type(p))
			{
				if (!first_parent) out << ", ";
				first_parent = false;

				out << get_type(p) << " x" << p;
			}
		}
		out << ")\n";

		out << "{\n";
		if (!void_type(n.first))
			out << tab << get_type(n.first) << " x0;\n";

		for (rule_t const & r : n.second)
		{
			out << tab << "if (";

			auto f = r.rule.empty()
				? follow(n.first)
				: first(r.rule.begin(), r.rule.end());

			for (auto it = f.begin(); it != f.end(); ++it)
			{
				if (it != f.begin())
					out << tab << tab;
				out << "lexer_.term() == \"" << *it << "\"";
				if (std::next(it) != f.end())
					out << " ||";
			}
			out << ")\n" << tab << "{\n";

			if (r.action.count(0) > 0)
			{
				out << tab << tab << "// user code here\n";
				out << get_action(r, 0);
			}

			std::size_t i = 1;
			for (auto it = r.rule.begin(); it != r.rule.end(); ++it, ++i)
			{
				if (!void_type(it->name))
				{
					out << tab << tab << get_type(it->name) << " x" << i << " = ";
				}
				else
				{
					out << tab << tab;
				}

				switch (it->type)
				{
				case RE_TERM:
				{
					out << "lexer_.value<" << get_type(it->name) << ">();\n";
					out << tab << tab << "if (lexer_.term() != \"" << it->name << "\")\n";
					out << tab << tab << tab << "throw std::runtime_error(\"unexpected token \" + lexer_.term());\n";
				}
					break;
				case RE_NTERM:
				{
					out << "parse_" << it->name << "(";

					bool first_parent = true;
					for (auto const & p : parents.at(it->name))
					{
						if (!void_type(p))
						{
							if (!first_parent) out << ", ";
							first_parent = false;

							if (p == n.first)
								out << "x0";
							else
								out << get_type(p) << "()";
						}
					}
					out << ");\n";
				}
					break;
				}

				if (it->type == RE_TERM)
					out << tab << tab << "lexer_.next();\n";

				if (r.action.count(i) > 0)
				{
					out << tab << tab << "// user code here\n";
					out << get_action(r, i);
				}
			}

			out << tab << "}\n";
		}

		out << tab << "return x0;\n";
		out << "}\n\n";
	}
}

std::string to_string (std::set<std::string> const & s)
{
	std::string res;
	for (auto it = s.begin(); it != s.end(); ++it)
	{
		if (it != s.begin())
			res += " ";
		if (it->empty())
			res += "\"\"";
		else
			res += *it;
	}
	return res;
}

int main (int argc, char ** argv)
try
{
	if (argc != 2)
	{
		std::cerr << "Usage: parsegen <input-file>" << std::endl;
		return 0;
	}

	std::ifstream input(argv[1]);

	std::string parser_name = getline(input);
	std::string lexer_name = getline(input);
	expect_separator(input);

	strings_t user_header_code = read_until_separator(input);
	strings_t user_source_code = read_until_separator(input);

	std::map<std::string, std::string> terminals = get_tokens_with_types(input);

	std::map<std::string, std::string> non_terminals = get_tokens_with_types(input);

	rules_t rules;

	bool first_non_terminal = true;

	std::string start;

	while (input)
	{
		strings_t single_rules = read_until_separator(input);
		if (single_rules.empty())
			break;

		std::string non_terminal = single_rules.front();

		if (first_non_terminal)
		{
			start = non_terminal;
			first_non_terminal = false;
		}

		std::vector<std::pair<std::string, strings_t>> splitted_rules;

		for (std::size_t i = 1; i < single_rules.size(); ++i)
		{
			if (single_rules[i].front() == '%')
			{
				splitted_rules.emplace_back();
				splitted_rules.back().first.assign(single_rules[i].begin() + 1, single_rules[i].end());
			}
			else
			{
				splitted_rules.back().second.push_back(single_rules[i]);
			}
		}

		for (auto const & p : splitted_rules)
		{
			rules[non_terminal].emplace_back();

			std::istringstream iss(p.first);
			for (std::string name; iss >> name;)
			{
				rule_element_type_t type;
				if (terminals.count(name))
				{
					type = RE_TERM;
				}
				else
				{
					type = RE_NTERM;
				}
				rules[non_terminal].back().rule.push_back({name, type});
			}

			std::size_t index = rules[non_terminal].back().rule.size();

			for (auto const & ln : p.second)
			{
				if (ln.front() == '$')
				{
					std::istringstream iss(std::string(ln.begin() + 1, ln.end()));
					iss >> index;
				}
				else
				{
					rules[non_terminal].back().action[index] += ln + "\n";
				}
			}
		}
	}

	std::map<std::string, std::set<std::string>> parents;

	for (auto const & n : rules)
	{
		parents[n.first];
		for (auto const & r : n.second)
		{
			for (auto const & e : r.rule)
			{
				if (e.type == RE_NTERM)
				{
					parents[e.name].insert(n.first);
				}
			}
		}
	}

	first_t first(rules);
	follow_t follow(rules, start, first);

	// check that grammar is indeed LL(1)
	for (auto const & n : rules)
	{
		for (auto rit1 = n.second.begin(); rit1 != n.second.end(); ++rit1)
		{
			auto f1 = rit1->rule.empty()
				? follow(n.first)
				: first(rit1->rule.begin(), rit1->rule.end());

			for (auto rit2 = n.second.begin(); rit2 != n.second.end(); ++rit2)
			{
				if (rit2 == rit1) continue;

				auto f2 = rit2->rule.empty()
					? follow(n.first)
					: first(rit2->rule.begin(), rit2->rule.end());

				if (intersect(f1, f2))
					throw std::runtime_error("ambiguous rules [" + n.first + " ->" + to_string(*rit1) +
													 "] with set {" + to_string(f1) + "} and [" + n.first + " ->" + to_string(*rit2) +
													 "] with set {" + to_string(f2) + "}");
			}
		}
	}

	generate_header(parser_name, lexer_name, user_header_code, non_terminals, parents, start, rules);
	generate_source(parser_name, user_source_code, rules, non_terminals, terminals, parents, first, follow);
}
catch (std::exception const & e)
{
	std::cout << "Error: " << e.what() << std::endl;
   return 1;
}
