#include <vector>
#include <string>
#include <stdexcept>

enum token_t
{
	CONST = 0,
	DIV = 1,
	FUNC = 2,
	LPAREN = 3,
	MULT = 4,
	N = 5,
	RPAREN = 6,
	SUB = 7,
	SUM = 8,
	_EOF_ = 9,
	_TOKEN_COUNT_
};

enum non_terminal_t
{
	M = 0,
	MM = 1,
	S = 2,
	SS = 3,
	T = 4,
	_START_ = 5,
	_NON_TERMINAL_COUNT_
};

std::size_t const state_count = 31;

/*
States:

0:
	S = . M 
	S = . M SUM S 
	S = M SUM . S 
	S = . M SUB SS 
	M = . T 
	M = . T MULT M 
	M = . T DIV MM 
	T = . LPAREN S RPAREN 
	T = . N 
	T = . CONST 
	T = . FUNC LPAREN S RPAREN 

1:
	S = . M 
	S = . M SUM S 
	S = . M SUB SS 
	SS = M SUM . S 
	M = . T 
	M = . T MULT M 
	M = . T DIV MM 
	T = . LPAREN S RPAREN 
	T = . N 
	T = . CONST 
	T = . FUNC LPAREN S RPAREN 

2:
	S = . M 
	S = . M SUM S 
	S = . M SUB SS 
	M = . T 
	M = . T MULT M 
	M = . T DIV MM 
	T = . LPAREN S RPAREN 
	T = LPAREN . S RPAREN 
	T = . N 
	T = . CONST 
	T = . FUNC LPAREN S RPAREN 

3:
	S = . M 
	S = . M SUM S 
	S = . M SUB SS 
	M = . T 
	M = . T MULT M 
	M = . T DIV MM 
	T = . LPAREN S RPAREN 
	T = . N 
	T = . CONST 
	T = . FUNC LPAREN S RPAREN 
	T = FUNC LPAREN . S RPAREN 

4:
	S = . M 
	S = . M SUM S 
	S = . M SUB SS 
	M = . T 
	M = . T MULT M 
	M = . T DIV MM 
	T = . LPAREN S RPAREN 
	T = . N 
	T = . CONST 
	T = . FUNC LPAREN S RPAREN 
	_START_ = . S 

5:
	S = M . 
	S = M . SUM S 
	S = M . SUB SS 

6:
	S = M SUM S . 

7:
	S = M SUB . SS 
	SS = . M 
	SS = . M SUB SS 
	SS = . M SUM S 
	M = . T 
	M = . T MULT M 
	M = . T DIV MM 
	T = . LPAREN S RPAREN 
	T = . N 
	T = . CONST 
	T = . FUNC LPAREN S RPAREN 

8:
	S = M SUB SS . 

9:
	SS = . M 
	SS = . M SUB SS 
	SS = M SUB . SS 
	SS = . M SUM S 
	M = . T 
	M = . T MULT M 
	M = . T DIV MM 
	T = . LPAREN S RPAREN 
	T = . N 
	T = . CONST 
	T = . FUNC LPAREN S RPAREN 

10:
	SS = M . 
	SS = M . SUB SS 
	SS = M . SUM S 

11:
	SS = M SUB SS . 

12:
	SS = M SUM S . 

13:
	M = . T 
	M = . T MULT M 
	M = T MULT . M 
	M = . T DIV MM 
	T = . LPAREN S RPAREN 
	T = . N 
	T = . CONST 
	T = . FUNC LPAREN S RPAREN 

14:
	M = . T 
	M = . T MULT M 
	M = . T DIV MM 
	MM = T MULT . M 
	T = . LPAREN S RPAREN 
	T = . N 
	T = . CONST 
	T = . FUNC LPAREN S RPAREN 

15:
	M = T . 
	M = T . MULT M 
	M = T . DIV MM 

16:
	M = T MULT M . 

17:
	M = T DIV . MM 
	MM = . T 
	MM = . T DIV MM 
	MM = . T MULT M 
	T = . LPAREN S RPAREN 
	T = . N 
	T = . CONST 
	T = . FUNC LPAREN S RPAREN 

18:
	M = T DIV MM . 

19:
	MM = . T 
	MM = . T DIV MM 
	MM = T DIV . MM 
	MM = . T MULT M 
	T = . LPAREN S RPAREN 
	T = . N 
	T = . CONST 
	T = . FUNC LPAREN S RPAREN 

20:
	MM = T . 
	MM = T . DIV MM 
	MM = T . MULT M 

21:
	MM = T DIV MM . 

22:
	MM = T MULT M . 

23:
	T = LPAREN S . RPAREN 

24:
	T = LPAREN S RPAREN . 

25:
	T = N . 

26:
	T = CONST . 

27:
	T = FUNC . LPAREN S RPAREN 

28:
	T = FUNC LPAREN S . RPAREN 

29:
	T = FUNC LPAREN S RPAREN . 

30:
	_START_ = S . 

*/

int operation[state_count][_TOKEN_COUNT_] =
{
	{2,0,2,2,0,2,0,0,0,0,},
	{2,0,2,2,0,2,0,0,0,0,},
	{2,0,2,2,0,2,0,0,0,0,},
	{2,0,2,2,0,2,0,0,0,0,},
	{2,0,2,2,0,2,0,0,0,0,},
	{0,0,0,0,0,0,3,2,2,3,},
	{0,0,0,0,0,0,3,0,0,3,},
	{2,0,2,2,0,2,0,0,0,0,},
	{0,0,0,0,0,0,3,0,0,3,},
	{2,0,2,2,0,2,0,0,0,0,},
	{0,0,0,0,0,0,3,2,2,3,},
	{0,0,0,0,0,0,3,0,0,3,},
	{0,0,0,0,0,0,3,0,0,3,},
	{2,0,2,2,0,2,0,0,0,0,},
	{2,0,2,2,0,2,0,0,0,0,},
	{0,2,0,0,2,0,3,3,3,3,},
	{0,0,0,0,0,0,3,3,3,3,},
	{2,0,2,2,0,2,0,0,0,0,},
	{0,0,0,0,0,0,3,3,3,3,},
	{2,0,2,2,0,2,0,0,0,0,},
	{0,2,0,0,2,0,3,3,3,3,},
	{0,0,0,0,0,0,3,3,3,3,},
	{0,0,0,0,0,0,3,3,3,3,},
	{0,0,0,0,0,0,2,0,0,0,},
	{0,3,0,0,3,0,3,3,3,3,},
	{0,3,0,0,3,0,3,3,3,3,},
	{0,3,0,0,3,0,3,3,3,3,},
	{0,0,0,2,0,0,0,0,0,0,},
	{0,0,0,0,0,0,2,0,0,0,},
	{0,3,0,0,3,0,3,3,3,3,},
	{0,0,0,0,0,0,0,0,0,1,},
};

int transition[state_count][_TOKEN_COUNT_] =
{
	{26,-1,27,2,-1,25,-1,-1,-1,-1,},
	{26,-1,27,2,-1,25,-1,-1,-1,-1,},
	{26,-1,27,2,-1,25,-1,-1,-1,-1,},
	{26,-1,27,2,-1,25,-1,-1,-1,-1,},
	{26,-1,27,2,-1,25,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,5,7,0,5,},
	{-1,-1,-1,-1,-1,-1,6,-1,-1,6,},
	{26,-1,27,2,-1,25,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,8,-1,-1,8,},
	{26,-1,27,2,-1,25,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,10,9,1,10,},
	{-1,-1,-1,-1,-1,-1,11,-1,-1,11,},
	{-1,-1,-1,-1,-1,-1,12,-1,-1,12,},
	{26,-1,27,2,-1,25,-1,-1,-1,-1,},
	{26,-1,27,2,-1,25,-1,-1,-1,-1,},
	{-1,17,-1,-1,13,-1,15,15,15,15,},
	{-1,-1,-1,-1,-1,-1,16,16,16,16,},
	{26,-1,27,2,-1,25,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,18,18,18,18,},
	{26,-1,27,2,-1,25,-1,-1,-1,-1,},
	{-1,19,-1,-1,14,-1,20,20,20,20,},
	{-1,-1,-1,-1,-1,-1,21,21,21,21,},
	{-1,-1,-1,-1,-1,-1,22,22,22,22,},
	{-1,-1,-1,-1,-1,-1,24,-1,-1,-1,},
	{-1,24,-1,-1,24,-1,24,24,24,24,},
	{-1,25,-1,-1,25,-1,25,25,25,25,},
	{-1,26,-1,-1,26,-1,26,26,26,26,},
	{-1,-1,-1,3,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,29,-1,-1,-1,},
	{-1,29,-1,-1,29,-1,29,29,29,29,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,30,},
};

int reduce[state_count][_TOKEN_COUNT_] =
{
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,2,-1,-1,2,},
	{-1,-1,-1,-1,-1,-1,2,-1,-1,2,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,2,-1,-1,2,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,3,-1,-1,3,},
	{-1,-1,-1,-1,-1,-1,3,-1,-1,3,},
	{-1,-1,-1,-1,-1,-1,3,-1,-1,3,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,0,0,0,0,},
	{-1,-1,-1,-1,-1,-1,0,0,0,0,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,0,0,0,0,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,1,1,1,1,},
	{-1,-1,-1,-1,-1,-1,1,1,1,1,},
	{-1,-1,-1,-1,-1,-1,1,1,1,1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,4,-1,-1,4,-1,4,4,4,4,},
	{-1,4,-1,-1,4,-1,4,4,4,4,},
	{-1,4,-1,-1,4,-1,4,4,4,4,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,4,-1,-1,4,-1,4,4,4,4,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
};

int reduce_size[state_count][_TOKEN_COUNT_] =
{
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,1,-1,-1,1,},
	{-1,-1,-1,-1,-1,-1,3,-1,-1,3,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,3,-1,-1,3,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,1,-1,-1,1,},
	{-1,-1,-1,-1,-1,-1,3,-1,-1,3,},
	{-1,-1,-1,-1,-1,-1,3,-1,-1,3,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,1,1,1,1,},
	{-1,-1,-1,-1,-1,-1,3,3,3,3,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,3,3,3,3,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,1,1,1,1,},
	{-1,-1,-1,-1,-1,-1,3,3,3,3,},
	{-1,-1,-1,-1,-1,-1,3,3,3,3,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,3,-1,-1,3,-1,3,3,3,3,},
	{-1,1,-1,-1,1,-1,1,1,1,1,},
	{-1,1,-1,-1,1,-1,1,1,1,1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
	{-1,4,-1,-1,4,-1,4,4,4,4,},
	{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,},
};

typedef double (*action_t)(double const *);

double action_5_6 (double const * attr)
{
 return attr[0];
}

double action_5_9 (double const * attr)
{
 return attr[0];
}

double action_6_6 (double const * attr)
{
 return attr[0] + attr[2];
}

double action_6_9 (double const * attr)
{
 return attr[0] + attr[2];
}

double action_8_6 (double const * attr)
{
 return attr[0] - attr[2];
}

double action_8_9 (double const * attr)
{
 return attr[0] - attr[2];
}

double action_10_6 (double const * attr)
{
 return attr[0];
}

double action_10_9 (double const * attr)
{
 return attr[0];
}

double action_11_6 (double const * attr)
{
 return attr[0] + attr[2];
}

double action_11_9 (double const * attr)
{
 return attr[0] + attr[2];
}

double action_12_6 (double const * attr)
{
 return attr[0] - attr[2];
}

double action_12_9 (double const * attr)
{
 return attr[0] - attr[2];
}

double action_15_6 (double const * attr)
{
 return attr[0];
}

double action_15_7 (double const * attr)
{
 return attr[0];
}

double action_15_8 (double const * attr)
{
 return attr[0];
}

double action_15_9 (double const * attr)
{
 return attr[0];
}

double action_16_6 (double const * attr)
{
 return attr[0] * attr[2];
}

double action_16_7 (double const * attr)
{
 return attr[0] * attr[2];
}

double action_16_8 (double const * attr)
{
 return attr[0] * attr[2];
}

double action_16_9 (double const * attr)
{
 return attr[0] * attr[2];
}

double action_18_6 (double const * attr)
{
 return attr[0] / attr[2];
}

double action_18_7 (double const * attr)
{
 return attr[0] / attr[2];
}

double action_18_8 (double const * attr)
{
 return attr[0] / attr[2];
}

double action_18_9 (double const * attr)
{
 return attr[0] / attr[2];
}

double action_20_6 (double const * attr)
{
 return attr[0];
}

double action_20_7 (double const * attr)
{
 return attr[0];
}

double action_20_8 (double const * attr)
{
 return attr[0];
}

double action_20_9 (double const * attr)
{
 return attr[0];
}

double action_21_6 (double const * attr)
{
 return attr[0] * attr[2];
}

double action_21_7 (double const * attr)
{
 return attr[0] * attr[2];
}

double action_21_8 (double const * attr)
{
 return attr[0] * attr[2];
}

double action_21_9 (double const * attr)
{
 return attr[0] * attr[2];
}

double action_22_6 (double const * attr)
{
 return attr[0] / attr[2];
}

double action_22_7 (double const * attr)
{
 return attr[0] / attr[2];
}

double action_22_8 (double const * attr)
{
 return attr[0] / attr[2];
}

double action_22_9 (double const * attr)
{
 return attr[0] / attr[2];
}

double action_24_1 (double const * attr)
{
 return attr[1];
}

double action_24_4 (double const * attr)
{
 return attr[1];
}

double action_24_6 (double const * attr)
{
 return attr[1];
}

double action_24_7 (double const * attr)
{
 return attr[1];
}

double action_24_8 (double const * attr)
{
 return attr[1];
}

double action_24_9 (double const * attr)
{
 return attr[1];
}

double action_25_1 (double const * attr)
{
 return attr[0];
}

double action_25_4 (double const * attr)
{
 return attr[0];
}

double action_25_6 (double const * attr)
{
 return attr[0];
}

double action_25_7 (double const * attr)
{
 return attr[0];
}

double action_25_8 (double const * attr)
{
 return attr[0];
}

double action_25_9 (double const * attr)
{
 return attr[0];
}

double action_26_1 (double const * attr)
{
 return attr[0];
}

double action_26_4 (double const * attr)
{
 return attr[0];
}

double action_26_6 (double const * attr)
{
 return attr[0];
}

double action_26_7 (double const * attr)
{
 return attr[0];
}

double action_26_8 (double const * attr)
{
 return attr[0];
}

double action_26_9 (double const * attr)
{
 return attr[0];
}

double action_29_1 (double const * attr)
{
 return (* func[static_cast<int>(attr[0])])(attr[2]);
}

double action_29_4 (double const * attr)
{
 return (* func[static_cast<int>(attr[0])])(attr[2]);
}

double action_29_6 (double const * attr)
{
 return (* func[static_cast<int>(attr[0])])(attr[2]);
}

double action_29_7 (double const * attr)
{
 return (* func[static_cast<int>(attr[0])])(attr[2]);
}

double action_29_8 (double const * attr)
{
 return (* func[static_cast<int>(attr[0])])(attr[2]);
}

double action_29_9 (double const * attr)
{
 return (* func[static_cast<int>(attr[0])])(attr[2]);
}

action_t reduce_action[state_count][_TOKEN_COUNT_] =
{
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&action_5_6,nullptr,nullptr,&action_5_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&action_6_6,nullptr,nullptr,&action_6_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&action_8_6,nullptr,nullptr,&action_8_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&action_10_6,nullptr,nullptr,&action_10_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&action_11_6,nullptr,nullptr,&action_11_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&action_12_6,nullptr,nullptr,&action_12_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&action_15_6,&action_15_7,&action_15_8,&action_15_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&action_16_6,&action_16_7,&action_16_8,&action_16_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&action_18_6,&action_18_7,&action_18_8,&action_18_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&action_20_6,&action_20_7,&action_20_8,&action_20_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&action_21_6,&action_21_7,&action_21_8,&action_21_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,&action_22_6,&action_22_7,&action_22_8,&action_22_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,&action_24_1,nullptr,nullptr,&action_24_4,nullptr,&action_24_6,&action_24_7,&action_24_8,&action_24_9,},
	{nullptr,&action_25_1,nullptr,nullptr,&action_25_4,nullptr,&action_25_6,&action_25_7,&action_25_8,&action_25_9,},
	{nullptr,&action_26_1,nullptr,nullptr,&action_26_4,nullptr,&action_26_6,&action_26_7,&action_26_8,&action_26_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
	{nullptr,&action_29_1,nullptr,nullptr,&action_29_4,nullptr,&action_29_6,&action_29_7,&action_29_8,&action_29_9,},
	{nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,},
};

int lhs_goto[state_count][_NON_TERMINAL_COUNT_] =
{
	{5,-1,6,-1,15,-1,},
	{5,-1,12,-1,15,-1,},
	{5,-1,23,-1,15,-1,},
	{5,-1,28,-1,15,-1,},
	{5,-1,30,-1,15,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{10,-1,-1,8,15,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{10,-1,-1,11,15,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{16,-1,-1,-1,15,-1,},
	{22,-1,-1,-1,15,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,18,-1,-1,20,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,21,-1,-1,20,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
	{-1,-1,-1,-1,-1,-1,},
};

template <typename Lexer>
double parse (Lexer & lexer)
{
	token_t token = lexer.next();
	std::vector<int> stack = {4};
	std::vector<double> value_stack;
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
			double const * attr = value_stack.data() + value_stack.size() - size;
			stack.push_back(lhs_goto[stack.back()][reduce[state][token]]);
			double value = (reduce_action[state][token])(attr);
			value_stack.resize(value_stack.size() - size);
			value_stack.push_back(value);
			break;
		}
		}
	}
}

