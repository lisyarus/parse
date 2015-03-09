#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <map>

template <typename T>
bool merge (std::set<T> & s, std::set<T> x)
{
	auto size = s.size();
	s.insert(x.begin(), x.end());
	return s.size() > size;
}

typedef std::vector<std::string> rule_t;

struct rule_info_t
{
	std::string left;
	rule_t right;
	std::string action;
};

static std::string const predefined_start = "_START_";
static std::string const predefined_eof = "_EOF_";

std::vector<rule_info_t> patch_rules (std::vector<rule_info_t> rules, std::string const & start)
{
	rules.push_back({predefined_start, {start}, "value = attr[0];"});
	return rules;
}

std::set<std::string> patch_non_terminals (std::set<std::string> non_terminals)
{
	non_terminals.insert(predefined_start);
	return non_terminals;
}

struct context_t
{
	std::string const start;
	std::set<std::string> const non_terminals;
	std::vector<rule_info_t> const rules;

	context_t (std::string start, std::set<std::string> non_terminals, std::vector<rule_info_t> rules)
		: start(predefined_start)
		, non_terminals(patch_non_terminals(std::move(non_terminals)))
		, rules(patch_rules(std::move(rules), start))
	{
		build_first();
		build_follow();
	}

	std::set<std::string> first (std::string const & x);
	std::set<std::string> follow (std::string const & x);

private:
	std::map<std::string, std::set<std::string>> first_;
	std::map<std::string, std::set<std::string>> follow_;

	void build_first ( );
	void build_follow ( );
};

void context_t::build_first ( )
{
	while (true)
	{
		bool changed = false;

		for (auto const & p : rules)
		{
			std::string const & n = p.left;

			rule_t const & r = p.right;

			auto it = r.begin();

			while (it != r.end())
			{
				changed |= merge(first_[n], first(*it));

				if (first(*it).count("") > 0)
					++it;
				else
					break;
			}

			if (it == r.end())
				changed |= first_[n].insert("").second;
		}

		if (!changed) break;
	}
}

void context_t::build_follow ( )
{
	follow_[start] = {predefined_eof};

	while (true)
	{
		bool changed = false;

		for (auto const & p : rules)
		{
			rule_t const & r = p.right;

			if (r.empty()) continue;

			auto it = --r.end();

			if (r.size() > 1)
			do
			{
				--it;

				if (non_terminals.count(*it) > 0)
				{
					auto f = first(*std::next(it));
					f.erase("");

					changed |= merge(follow_[*it], f);
				}

			} while (it != r.begin());

			it = r.end();

			do
			{
				--it;

				if (non_terminals.count(*it) > 0)
				{
					changed |= merge(follow_[*it], follow_[p.left]);
				}

				if (first(*it).count("") == 0)
					break;

			} while (it != r.begin());
		}

		if (!changed) break;
	}
}

std::set<std::string> context_t::first (std::string const & x)
{
	if (non_terminals.count(x) == 0)
		return {x};
	return first_[x];
}

std::set<std::string> context_t::follow (std::string const & x)
{
	return follow_[x];
}

struct item_t
{
	rule_info_t const * rule;
	std::size_t pos;
};

bool operator == (item_t const & i1, item_t const & i2)
{
	return i1.rule == i2.rule && i1.pos == i2.pos;
}

bool operator < (item_t const & i1, item_t const & i2)
{
	return i1.rule < i2.rule || (i1.rule == i2.rule && i1.pos < i2.pos);
}

typedef std::set<item_t> items_t;

std::ostream & operator << (std::ostream & os, item_t const & item)
{
	os << item.rule->left << " = ";
	for (std::size_t i = 0; i < item.pos; ++i)
		os << item.rule->right[i] << " ";
	os << ". ";
	for (std::size_t i = item.pos; i < item.rule->right.size(); ++i)
		os << item.rule->right[i] << " ";
	return os;
}

items_t closure (context_t const & ctx, items_t items)
{
	while (true)
	{
		items_t new_items;

		for (item_t const & i : items)
		{
			if (i.pos == i.rule->right.size()) continue;

			for (rule_info_t const & r : ctx.rules)
			{
				if (r.left == i.rule->right[i.pos])
					new_items.insert(item_t{std::addressof(r), 0});
			}
		}

		if (!merge(items, new_items))
			break;
	}

	return items;
}

std::set<std::string> terminals (context_t const & ctx)
{
	std::set<std::string> result;
	for (rule_info_t const & r : ctx.rules)
	{
		result.insert(r.right.begin(), r.right.end());
	}

	for (auto const & n : ctx.non_terminals)
		result.erase(n);

	return result;
}

enum operation_type_t
{
	ERROR,
	SUCCEED,
	SHIFT,
	REDUCE,
};

struct operation_t
{
	operation_type_t type;
	item_t item;
	items_t items;
};

typedef std::map<items_t, std::map<std::string, operation_t>> fsm_t;

bool add_reduce (context_t & ctx, fsm_t & fsm, items_t const & i)
{
	bool changed = false;

	for (item_t const & item : i)
	{
		if (item.pos < item.rule->right.size()) continue;

		if (item.rule->left == predefined_start)
		{
			changed |= fsm[i].insert(std::make_pair(predefined_eof, operation_t{SUCCEED, item, i})).second;
		}
		else
		{
			auto follow = ctx.follow(item.rule->left);
			for (std::string const & t : follow)
			{
				changed |= fsm[i].insert(std::make_pair(t, operation_t{REDUCE, item, i})).second;
			}
		}
	}

	return changed;
}

items_t shift (context_t const & ctx, items_t const & i, std::string const & t)
{
	items_t result;

	for (item_t const & item : i)
	{
		if (item.pos < item.rule->right.size() && item.rule->right[item.pos] == t)
			result.insert(item_t{item.rule, item.pos + 1});
	}

	return closure(ctx, result);
}

items_t starting_state (context_t & ctx)
{
	return closure(ctx, { {std::addressof(ctx.rules.back()), 0} });
}

fsm_t build_fsm (context_t & ctx)
{
	fsm_t fsm;

	std::set<items_t> states;
	std::set<items_t> new_states = { starting_state(ctx) };

	std::set<std::string> symbols = terminals(ctx);
	merge(symbols, ctx.non_terminals);

	while (true)
	{
		states = std::move(new_states);

		bool changed = false;

		for (items_t const & i : states)
		{
			changed |= add_reduce(ctx, fsm, i);

			for (auto const & t : symbols)
			{
				auto shifted = shift(ctx, i, t);

				if (!shifted.empty())
				{
					new_states.insert(shifted);

					changed |= fsm[i].insert(std::make_pair(t, operation_t{SHIFT, {nullptr, 0}, shifted})).second;
				}
			}
		}

		if (new_states.empty()) break;

		if (!changed) break;
	}

	return fsm;
}

int main ( )
{
	std::string attribute_type;

	std::cin >> attribute_type;
	{
		std::string line;
		std::getline(std::cin, line);
	}

	std::string start;
	std::set<std::string> non_terminals;
	std::vector<rule_info_t> rules;

	while (std::cin)
	{
		std::string line;
		std::getline(std::cin, line);

		if (!std::cin) break;

		std::istringstream iss(line);
		std::string nt;
		iss >> nt;

		if (non_terminals.empty())
			start = nt;

		non_terminals.insert(nt);

		std::string delim;
		iss >> delim;

		rule_info_t r;
		r.left = nt;

		while (iss)
		{
			std::string t;
			iss >> t;
			if (!iss) break;
			if (t == "->") break;
			r.right.push_back(t);
		}

		std::getline(iss, r.action);

		rules.push_back(r);
	}

	context_t ctx(start, non_terminals, rules);

	fsm_t fsm = build_fsm(ctx);

	auto terminals = ::terminals(ctx);

	std::map<items_t, std::size_t> state_index;
	{
		std::size_t max = 0;
		for (auto const & p : fsm)
			state_index[p.first] = max++;
	}

	std::map<std::string, std::size_t> terminal_index;
	{
		std::size_t max = 0;
		for (auto const & t : terminals)
			terminal_index[t] = max++;
		terminal_index[predefined_eof] = max++;
	}

	std::map<std::string, std::size_t> non_terminal_index;
	{
		std::size_t max = 0;
		for (auto const & n : ctx.non_terminals)
			non_terminal_index[n] = max++;
	}

	std::cout << "#include <vector>\n";
	std::cout << "#include <string>\n";
	std::cout << "#include <stdexcept>\n";
	std::cout << "\n";

	std::cout << "enum token_t\n{\n";
	for (auto const & p : terminal_index)
		std::cout << "\t" << p.first << " = " << p.second << ",\n";
	std::cout << "\t_TOKEN_COUNT_\n};\n\n";

	std::cout << "enum non_terminal_t\n{\n";
	for (auto const & p : non_terminal_index)
		std::cout << "\t" << p.first << " = " << p.second << ",\n";
	std::cout << "\t_NON_TERMINAL_COUNT_\n};\n\n";

	std::cout << "std::size_t const state_count = " << state_index.size() << ";\n\n";

	std::cout << "/*\nStates:\n\n";
	for (auto const & p : state_index)
	{
		std::cout << p.second << ":\n";
		for (auto const & i : p.first)
			std::cout << "\t" << i << "\n";
		std::cout << "\n";
	}
	std::cout << "*/\n\n";

	std::cout << "int operation[state_count][_TOKEN_COUNT_] =\n{\n";
	for (auto const & p : state_index)
	{
		std::cout << "\t{";
		for (auto const & t : terminal_index)
		{
			std::cout << fsm[p.first][t.first].type << ",";
		}
		std::cout << "},\n";
	}
	std::cout << "};\n\n";

	std::cout << "int transition[state_count][_TOKEN_COUNT_] =\n{\n";
	for (auto const & p : state_index)
	{
		std::cout << "\t{";
		for (auto const & t : terminal_index)
		{
			auto const & st = fsm[p.first][t.first].items;
			std::cout << (state_index.count(st) > 0 ? std::to_string(state_index[st]) : "-1") << ",";
		}
		std::cout << "},\n";
	}
	std::cout << "};\n\n";

	std::cout << "int reduce[state_count][_TOKEN_COUNT_] =\n{\n";
	for (auto const & p : state_index)
	{
		std::cout << "\t{";
		for (auto const & t : terminal_index)
		{
			auto const & st = fsm[p.first][t.first];
			if (st.type == REDUCE)
				std::cout << non_terminal_index[st.item.rule->left];
			else
				std::cout << "-1";
			std::cout << ",";
		}
		std::cout << "},\n";
	}
	std::cout << "};\n\n";

	std::cout << "int reduce_size[state_count][_TOKEN_COUNT_] =\n{\n";
	for (auto const & p : state_index)
	{
		std::cout << "\t{";
		for (auto const & t : terminal_index)
		{
			auto const & st = fsm[p.first][t.first];
			if (st.type == REDUCE)
				std::cout << st.item.rule->right.size();
			else
				std::cout << "-1";
			std::cout << ",";
		}
		std::cout << "},\n";
	}
	std::cout << "};\n\n";

	std::cout << "typedef " << attribute_type << " (*action_t)(" << attribute_type << " const *);\n\n";

	for (auto const & p : state_index)
	{
		for (auto const & t : terminal_index)
		{
			auto const & st = fsm[p.first][t.first];
			if (st.type == REDUCE)
			{
				std::cout << attribute_type << " action_" << p.second << "_" << t.second << " (" << attribute_type << " const * attr)\n{\n";
				std::cout << st.item.rule->action;
				std::cout << "\n}\n\n";
			}
		}
	}

	std::cout << "action_t reduce_action[state_count][_TOKEN_COUNT_] =\n{\n";
	for (auto const & p : state_index)
	{
		std::cout << "\t{";
		for (auto const & t : terminal_index)
		{
			auto const & st = fsm[p.first][t.first];
			if (st.type == REDUCE)
				std::cout << "&action_" << p.second << "_" << t.second;
			else
				std::cout << "nullptr";
			std::cout << ",";
		}
		std::cout << "},\n";
	}
	std::cout << "};\n\n";

	std::cout << "int lhs_goto[state_count][_NON_TERMINAL_COUNT_] =\n{\n";
	for (auto const & p : state_index)
	{
		std::cout << "\t{";
		for (auto const & t : non_terminal_index)
		{
			auto const & st = fsm[p.first][t.first].items;
			std::cout << (state_index.count(st) > 0 ? std::to_string(state_index[st]) : "-1") << ",";
		}
		std::cout << "},\n";
	}
	std::cout << "};\n\n";

	std::cout << "template <typename Lexer>\n" << attribute_type << " parse (Lexer & lexer)\n{\n";
	std::cout << "\ttoken_t token = lexer.next();\n";
	std::cout << "\tstd::vector<int> stack = {" << state_index[starting_state(ctx)] << "};\n";
	std::cout << "\tstd::vector<" << attribute_type << "> value_stack;\n";
	std::cout << "\twhile (true)\n\t{\n";
	std::cout << "\t\tint state = stack.back();\n";
	std::cout << "\t\tint op = operation[state][token];\n";
	std::cout << "\t\tswitch (op)\n\t\t{\n";
	std::cout << "\t\tcase " << ERROR << ": throw std::runtime_error(\"Unexpected token \" + std::to_string(token) + \" at state \" + std::to_string(state));\n";
	std::cout << "\t\tcase " << SUCCEED << ": return value_stack.back();\n";
	std::cout << "\t\tcase " << SHIFT << ":\n";
	std::cout << "\t\t\tstack.push_back(transition[state][token]);\n";
	std::cout << "\t\t\tvalue_stack.push_back(lexer.value());\n";
	std::cout << "\t\t\ttoken = lexer.next();\n";
	std::cout << "\t\t\tbreak;\n";
	std::cout << "\t\tcase " << REDUCE << ":\n";
	std::cout << "\t\t{\n";
	std::cout << "\t\t\tint size = reduce_size[state][token];\n";
	std::cout << "\t\t\tstack.resize(stack.size() - size);\n";
	std::cout << "\t\t\t" << attribute_type << " const * attr = value_stack.data() + value_stack.size() - size;\n";
	std::cout << "\t\t\tstack.push_back(lhs_goto[stack.back()][reduce[state][token]]);\n";
	std::cout << "\t\t\t" << attribute_type << " value = (reduce_action[state][token])(attr);\n";
	std::cout << "\t\t\tvalue_stack.resize(value_stack.size() - size);\n";
	std::cout << "\t\t\tvalue_stack.push_back(value);\n";
	std::cout << "\t\t\tbreak;\n";
	std::cout << "\t\t}\n";
	std::cout << "\t\t}\n";
	std::cout << "\t}\n";
	std::cout << "}\n\n";

/*
	std::vector<std::string> tokens = { "(", "(", ")", ")", "(", ")", predefined_eof };
	std::size_t i = 0;

	std::vector<items_t> stack;

	stack.push_back(starting_state(ctx));

	while (true)
	{
		items_t const & state = stack.back();

		std::cout << "Current state:\n";
		for (auto const & i : state)
			std::cout << "    " << i << "\n";

		std::string const & token = tokens[i];
		std::cout << "\nNext token: " << token << "\n";

		operation_t op = fsm[state][token];

		switch (op.type)
		{
		case SUCCEED:
			std::cout << "Accepted\n"; return 0;
		case SHIFT:
			std::cout << "Shift\n\n";
			stack.push_back(op.items);
			++i;
			break;
		case REDUCE:
			std::cout << "Reduce by " << op.item << "\n\n";
			stack.resize(stack.size() - op.item.rule->right.size());
			stack.push_back(fsm[stack.back()][op.item.rule->left].items);
			break;
		case ERROR:
			std::cout << "Error\n"; return 1;
		}
	}
*/
}
