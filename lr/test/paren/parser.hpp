#include <vector>
#include <string>
#include <stdexcept>

enum token_t
{
	LPAREN = 0,
	RPAREN = 1,
	_EOF_ = 2,
	_TOKEN_COUNT_
};

enum non_terminal_t
{
	S = 0,
	_START_ = 1,
	_NON_TERMINAL_COUNT_
};

std::size_t const state_count = 6;

/*
States:

0:
	S = . LPAREN S RPAREN S 
	S = LPAREN . S RPAREN S 
	S = . 

1:
	S = . LPAREN S RPAREN S 
	S = LPAREN S RPAREN . S 
	S = . 

2:
	S = . LPAREN S RPAREN S 
	S = . 
	_START_ = . S 

3:
	S = LPAREN S . RPAREN S 

4:
	S = LPAREN S RPAREN S . 

5:
	_START_ = S . 

*/

int operation[state_count][_TOKEN_COUNT_] =
{
	{2,3,3,},
	{2,3,3,},
	{2,3,3,},
	{0,2,0,},
	{0,3,3,},
	{0,0,1,},
};

int transition[state_count][_TOKEN_COUNT_] =
{
	{0,0,0,},
	{0,1,1,},
	{0,2,2,},
	{-1,1,-1,},
	{-1,4,4,},
	{-1,-1,5,},
};

int reduce[state_count][_TOKEN_COUNT_] =
{
	{-1,0,0,},
	{-1,0,0,},
	{-1,0,0,},
	{-1,-1,-1,},
	{-1,0,0,},
	{-1,-1,-1,},
};

int reduce_size[state_count][_TOKEN_COUNT_] =
{
	{-1,0,0,},
	{-1,0,0,},
	{-1,0,0,},
	{-1,-1,-1,},
	{-1,4,4,},
	{-1,-1,-1,},
};

typedef int (*action_t)(int const *);

int action_0_1 (int const * attr)
{
 return 0;
}

int action_0_2 (int const * attr)
{
 return 0;
}

int action_1_1 (int const * attr)
{
 return 0;
}

int action_1_2 (int const * attr)
{
 return 0;
}

int action_2_1 (int const * attr)
{
 return 0;
}

int action_2_2 (int const * attr)
{
 return 0;
}

int action_4_1 (int const * attr)
{
 return std::max(attr[1] + 1, attr[3]);
}

int action_4_2 (int const * attr)
{
 return std::max(attr[1] + 1, attr[3]);
}

action_t reduce_action[state_count][_TOKEN_COUNT_] =
{
	{nullptr,&action_0_1,&action_0_2,},
	{nullptr,&action_1_1,&action_1_2,},
	{nullptr,&action_2_1,&action_2_2,},
	{nullptr,nullptr,nullptr,},
	{nullptr,&action_4_1,&action_4_2,},
	{nullptr,nullptr,nullptr,},
};

int lhs_goto[state_count][_NON_TERMINAL_COUNT_] =
{
	{3,-1,},
	{4,-1,},
	{5,-1,},
	{-1,-1,},
	{-1,-1,},
	{-1,-1,},
};

template <typename Lexer>
int parse (Lexer & lexer)
{
	token_t token = lexer.next();
	std::vector<int> stack = {2};
	std::vector<int> value_stack;
	while (true)
	{
		int state = stack.back();
		int op = operation[state][token];
		switch (op)
		{
		case 0: throw std::runtime_error("Unexpected token " + std::to_string(token) + " at state " + std::to_string(state));
		case 1: return value_stack.back();
		case 2:
			stack.push_back(transition[state][token]);
			value_stack.push_back(lexer.value());
			token = lexer.next();
			break;
		case 3:
		{
			int size = reduce_size[state][token];
			stack.resize(stack.size() - size);
			int const * attr = value_stack.data() + value_stack.size() - size;
			stack.push_back(lhs_goto[stack.back()][reduce[state][token]]);
			int value = (reduce_action[state][token])(attr);
			value_stack.resize(value_stack.size() - size);
			value_stack.push_back(value);
			break;
		}
		}
	}
}

