// CPSC323 assignment 2
// Yong Kim
//// - add a syntactic analyzer to the lexer which was made in assignment 1

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <array>
#include <iomanip>

#include <stack>

using namespace std;
	// input           l  d  ! sp ( ) [ ] { } ' , . : ; * +  - = / > < % \n Unknown $	
typedef enum Input {
    	LETTER,            		// 0    letter
       	DIGIT,            		// 1    digit
    	EXCLAMATION,        		// 2    !
    	SPACE,            		// 3    sp
    	OPEN_PAREN,        		// 4    (
    	CLOSE_PAREN,        		// 5    )
    	OPEN_BLANK,        		// 6    [
    	CLOSE_BLANK,        		// 7   	]
    	OPEN_CURLY_BLANK,   		// 8    {
    	CLOSE_CURLY_BLANK,  		// 9    }
    	APOS,            		// 10    '
    	COMMA_SIGN,        		// 11    ,
    	PERIOD_SIGN,        		// 12    .
    	COLON,            		// 13    :
    	SEMI_COLON,        		// 14    ;
    	MULTI_SIGN,        		// 15    *
    	PLUS_SIGN,        		// 16    +
    	MINUS_SIGN,        		// 17    -
    	EQUAL_SIGN,        		// 18    =
    	DIVIDE_SIGN,        		// 19    /
    	LEFT_GREATER,       		// 20    >
    	RIGHT_GREATER,      		// 21    <
    	MOD_SIGN,        		// 22    %
    	LINE_END,        		// 23    \n
    	UNKNOWN,        		// 24    ?
    	DOLLAR_SIGN,			// 25	 $

     	// Input number, put a new input above this line
	INPUT_COUNT    			// 26
} Input;

const size_t COL_SIZE = INPUT_COUNT; // for checkking the # of columns

typedef enum State {
    	START,            		// 0    start
	STRING,            		// 1    keyword + identifier
	NUMBER,            		// 2    number
    	END,            		// 3    regular end
    	IN_COMMENT,        		// 4    in comment !
    	END_STRING,        		// 5    end string
    	REAL_NUM,        		// 6    real number
    	END_INTEGER,        		// 7    end_integer
	END_REAL,        		// 8    end real number
    	START_COMMENT,      		// 9    start ! comment
    	END_COMMENT,        		// 10    end ! comment
    	FOUND_SPACE,        		// 11    found sp
    	FOUND_OPEN_PAREN,   		// 12    found (
    	FOUND_CLOSE_PAREN,  		// 13    found )
    	FOUND_OPEN_BLANK,   		// 14    found [
    	FOUND_CLOSE_BLANK,  		// 15    found ]
    	FOUND_OPEN_CUR_BLANK,    	// 16    found {
    	FOUND_CLOSE_CUR_BLANK,    	// 17    found }
    	FOUND_APOS,        		// 18    found '
    	FOUND_COMMA,        		// 19    found ,
    	FOUND_PERIOD,        		// 20    found .
    	FOUND_COLON,        		// 21    found :
    	FOUND_SEMICOLON,    		// 22    found ;
    	FOUND_MULTI,        		// 23    found *
    	FOUND_PLUS,        		// 24    found +
    	FOUND_MINUS,        		// 25    found -
    	FOUND_EQ,        		// 26    found =
    	FOUND_DIVIDE,        		// 27    found /
    	FOUND_LEFT_GREAT,    		// 28    found >
    	FOUND_RIGHT_GREAT,    		// 29    found <
    	FOUND_MOD,        		// 30    found %
    	FOUND_LINE_END,        		// 31    found \n
    	UNDEFINED,        		// 32    unknown
    	START_REAL,			// 33	 start real
    	FOUND_DOLLAR,			// 34	 found $
	
    	// State number, put a new state above this line
    	STATE_COUNT    			// 35
} State;

const size_t ROW_SIZE = STATE_COUNT; // for checking the # of rows


typedef enum Cur_char {
	ID,				// 0 	id
	PLUS,				// 1	+
	MINUS,				// 2	-
	MULTI,				// 3	*
	DIVIDE,				// 4	/
	LEFT_PAREN,			// 5	(
	RIGHT_PAREN,			// 6	)
	S_C,				// 7 	;

	// Cur_char number, put a new char above this line
	TDP_INPUT			// 8	
} Cur_char;

const size_t TDP_NCOL = TDP_INPUT; 	// the # of cols in TDP table

typedef enum Production_rule {
	E,				// 0	E
	Q,				// 1	Q
	T,				// 2	T
	R,				// 3	R
	F,				// 4	F

	// Production_rule number, put a new rule above this line
	TDP_EXP				// 5
} Production_rule;

const size_t TDP_NROW = TDP_EXP;	// the # of rows in TDP table


class TDP {										// Data structure for the Table Driven Parser
	private:
		string tdp_table[TDP_NROW][TDP_NCOL] = {							//	id	+	-	*	/	(	)	;	
				{"TQ"	, "NULL", "NULL", "NULL", "NULL", "TQ"	, "NULL", "NULL"},		// E |  TQ					TQ			
				{"NULL"	, "+TQ"	, "-TQ"	, "NULL", "NULL", "NULL", "e"	, "e"	},		// Q |		+TQ	-TQ				e	e	
				{"FR"	, "NULL", "NULL", "NULL", "NULL", "FR"	, "NULL", "NULL"},		// T |	FR					FR		
				{"NULL"	, "e"	, "e"	, "*FR"	, "/FR"	, "NULL", "e"	, "e"	},		// R |		e	e	*FR	/FR		e	e
				{"i"	, "NULL", "NULL", "NULL", "NULL", "(E)"	, "NULL", "NULL"}		// F |	i					(E)
		};
	public:
	class FSA {										// Data structure for the FSA table
		private:
			// input l  d  ! sp ( ) [ ] { } ' , . : ; * +  - = / > < % \n Unknown $
			int fsa_table[ROW_SIZE][COL_SIZE] ={
				{1, 2, 9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,34},    // 0 start
				{1, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1},    // 1 identifier, keep going
				{32,2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 33,7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},    // 2 integer, keep going
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 3 regular end, will be used later
				{4, 4,10, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},    // 4 in comment, keep going
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 5 end string
				{8, 6, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},    // 6 real, keep going
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},    // 7 end integer
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 8 end real
				{4, 4,10, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},    // 9 start ! comment
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 10 end ! comment
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 11 found sp
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 12 found '('
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 13 found ')'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 14 found '['
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 15 found ']'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 16 found '{'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 17 found '}'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 18 found '\''
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 19 found ','
				{0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 20 found '.'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 21 found ':'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 22 found ';'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 23 found '*'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 24 found '+'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 25 found '-'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 26 found '='
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 27 found '/'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 28 found '>'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 29 found '<'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 30 found '%'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	   // 31 found '\n'
				{32,32,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	   // 32 unknow TODO
				{7, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,32,32},	   // 33 start real w/ '.'
				{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}     // 34 found '$'
			};
    
			std::array <std::string, 20> keywords = { "if", "else", "int", "float", "while", "bool",  "then", "do", "while", "whileend",
				"doend", "for", "and", "or", "function", "main", "char", "string"};       
			std::array <char, 10> operators = {'*', '=', '+', '-', '/', '>', '<', '%'};                        // temporary contatiner
		    	std::queue<char> tmp_lex;
		    	std::queue<char> cur_char;
    
		public:
			int get_input(const char&);                    		// categorize an input char to a number(enum)
		    	int get_state(const int&, const int&);           	// next state = table [current state] [input]
		    	void state_check(const int&);                    	// check the state
		    	void add_lexeme(const int&);                    	// add a lexeme w/o current char
		    	void print();                            		// print result
		    	bool is_keyword(const std::string&);                	// check whether an identifier is a keyword or not
		    	bool is_operator(const char&);                    	// check whether a symbol is a operator or separator
	
		    	struct Token
		     	{
				int category;
				std::string lexeme;
		    	};
			std::vector<Token> lex_storage;
	};

	void parse(const FSA&);							// parser
	int get_cur_exp(const char&);						// convert the EXPRESSION to int for TABLE
	int get_cur_char(const char&); 						// convert CURRENT CHARACTER to int for TABLE
	string get_rule(const int&, const int&);				// get the table result [ pop(exp), cur_char ]
};


int TDP::FSA::get_input (const char& c) {			// transform current character to an input signal
	int c_type;
	if    (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z')) { //  || c == '$') {    // excluded ' || c == '_'  '
        c_type = LETTER;
	}
    	else if    ('0' <= c && c <= '9') {
		c_type = DIGIT;
    	}
    	else if (c == '!') {
	    	c_type = EXCLAMATION; 
     	}
    	else if (c == ' ') {
		c_type= SPACE;
    	}
    	else if (c == '(') {
		c_type = OPEN_PAREN;
    	}
    	else if (c == ')') {
		c_type = CLOSE_PAREN;
    	}
    	else if (c == '[') {
		c_type = OPEN_BLANK;
    	}
    	else if (c == ']') {
		c_type = CLOSE_BLANK;
    	}
    	else if (c == '{') {
		c_type = OPEN_CURLY_BLANK;
    	}
    	else if (c == '}') {
		c_type = CLOSE_CURLY_BLANK;
    	}
    	else if (c == '\'') {
		c_type = APOS;
    	}
    	else if (c == ',') {
		c_type = COMMA_SIGN;
    	}
    	else if (c == '.') {
		c_type = PERIOD_SIGN;
    	}
    	else if (c == ':') {
		c_type = COLON;
    	}
    	else if (c == ';') {
		c_type = SEMI_COLON;
    	}
    	else if (c == '*') {
		c_type = MULTI_SIGN;
    	}
    	else if (c == '+') {
		c_type = PLUS_SIGN;
    	}
    	else if (c == '-') {
		c_type = MINUS_SIGN;
    	}
    	else if (c == '=') {
		c_type = EQUAL_SIGN;
    	}
    	else if (c == '/') {
		c_type = DIVIDE_SIGN;
    	}
    	else if (c == '>') {
		c_type = LEFT_GREATER;
    	}
    	else if (c == '<') {
		c_type = RIGHT_GREATER;
    	}
    	else if (c == '%') {
		c_type = MOD_SIGN;
    	}
    	else if (c == '\n') {
		c_type = LINE_END;
    	}
    	else if (c == '$') {
		c_type = DOLLAR_SIGN;
    	}
    	else {
		//cout << "invalid input!" << std::endl;		// for TEST
		return 9999;
    	}
    	cur_char.push(c);
    	return c_type;
	}

int TDP::FSA::get_state (const int& state, const int& input) {	// state change
	return fsa_table[state][input];
}

void TDP::FSA::state_check(const int& state) {			// based on states, call add_lexeme, back up the current state, or throw out current character (comment)
    	if (state == 3 || state == 5 || (state >= 7 && state <= 31)) {
		add_lexeme(state);
    	}

    	if (state == 0) {        					// BACKUP
		add_lexeme(state);
		return;
    	}
    	else if (state == 4) {
		while (cur_char.size() != 0) {
	     		cur_char.pop();
		}							
		return;
    	}
    	else if (cur_char.size() != 0 ) {
		tmp_lex.push(cur_char.front());
		cur_char.pop();
    	}
    	return;
}

void TDP::FSA::add_lexeme (const int& state) {			// based on state, store the character(s) in lex_storage with category
	std::string lex, temp_lex;
    	Token token, temp_token;

    	while (!tmp_lex.empty()) {
		if (tmp_lex.front() == '\n') {
	    		tmp_lex.pop();
		}
		else {
	    		lex.push_back(tmp_lex.front());
	    		tmp_lex.pop();
		}
    	}
    	if (state == END_STRING) {
		if (is_keyword(lex)) {
	    		token.category = 1;
		} else {
	    		token.category = 2;
		}
    	}
    	else if (state == END_INTEGER) {
    		if (lex.back() == '.') {
    			temp_token.lexeme = '.';
    			temp_token.category = 6;
    			lex.pop_back();
    		}
    		token.category = 3;
    	}
    	else if (state == END_REAL) {
		token.category = 4;
    	}
    	else if (state == IN_COMMENT) {
		return;
    	}
    	else {
		if (is_operator(lex[0])) {
	    		token.category = 5;
		} else {
	    		token.category = 6;
		}
    	}
    	token.lexeme = lex;

    	if (lex == "") {return;}					// if lex is "", which is nothing, add nothing.
    	lex_storage.push_back(token);

    	if (temp_token.lexeme != "") {
    		lex_storage.push_back(temp_token);
    	}
    	return;
}

ofstream fout("output.txt");
void TDP::FSA::print() {						// make the "output.txt" file
    	fout << left << setw(15) << "Token" << "Lexeme" << std::endl;
    	fout << endl;
    	for (int i = 0; i < lex_storage.size(); ++i) {
		if (lex_storage[i].lexeme == "") { ++i; }
		if (lex_storage[i].category == 1) {
	    		fout << left << setw(10) << "Keyword" << setw(5) <<":"<< lex_storage[i].lexeme << std::endl;
		}
		else if (lex_storage[i].category == 2) {
	    		fout << left << setw(10) << "Identifier" << setw(5) << ":" << lex_storage[i].lexeme << std::endl;
		}
		else if (lex_storage[i].category == 3) {
	    		fout << left << setw(10) << "Integer" << setw(5) << ":" << lex_storage[i].lexeme << std::endl;
		}
		else if (lex_storage[i].category == 4) {
	    		fout << left << setw(10) << "Float" << setw(5) <<":" << lex_storage[i].lexeme << std::endl;
		}
		else if (lex_storage[i].category == 5) {
	    		fout << left << setw(10) << "Operator" << setw(5) <<":" << lex_storage[i].lexeme << std::endl;
		}
		else if ( lex_storage[i].category == 6) {
	    		if (lex_storage[i].lexeme[0] == ' ') {
				continue;											// DO NOT PRINT!! TODO
//				fout << left << setw(10) << "Separator" << setw(5) << ":" << "sp(space)" << std::endl;
	    		}
	    		else {
				fout << left << setw(10) << "Separator" << setw(5) << ":" << lex_storage[i].lexeme << std::endl;
	    		}
		}
		else {
	    		fout << left << setw(5) << "???" << std::endl;
		}
    	}

    	fout << flush;
    	fout.close();
    	return;
}

bool TDP::FSA::is_keyword(const std::string& end_string) {		// check whether a string is a keyword or not
      	bool is_keyword = false;
      	for (int i = 0; i < keywords.size(); ++i) {
 	 	if (end_string == keywords[i]) {
 	     		is_keyword = true;
 	 	}
      	}
      	return is_keyword;
}

bool TDP::FSA::is_operator(const char& c) {					// check whether a symbol is an operator or not(separator)
    	bool is_operator = false;
    	for (int i = 0; i < operators.size(); ++i) {
		if (c == operators[i]) {
	    		is_operator = true;
		}
    	}
    	return is_operator;
}

int TDP::get_cur_exp(const char& c) {						// convert the EXPRESSION to int for TABLE
	int c_exp;
	if(c == 'E') {
		c_exp = E;
	} else if (c == 'Q') {
		c_exp = Q;
	} else if (c == 'T') {
		c_exp = T;
	} else if (c == 'R') {
		c_exp = R;
	} else if (c == 'F') {
		c_exp = F;
	}
       return c_exp;
}

int TDP::get_cur_char(const char& c) {						// convert the current character to int for TABLE
	int c_char;
	if(c == '+') {
		c_char = PLUS;
	} else if (c == '-') {
		c_char = MINUS;
	} else if (c == '*') {
		c_char = MULTI;
	} else if (c == '/') {
		c_char = DIVIDE;
	} else if (c == '(') {
		c_char = LEFT_PAREN;
	} else if (c == ')') {
		c_char = RIGHT_PAREN;
	} else if (c == '$') {
		c_char = S_C;
//	} else if (c == ';') {
//		c_char = S_C;
	} else {
		c_char = 9999;
		fout << "Error : " << c << " is not being processed yet." << std::endl;
	}
	return c_char;
}
	
string TDP::get_rule(const int& row, const int& col) {				// get the table result [ pop(exp), cur_char ]
	return tdp_table[row][col];
}

void TDP::parse(const TDP::FSA& fsa) {
	size_t length = fsa.lex_storage.size();								// To get the modified length of test

	std::queue <FSA::Token> c_terminal;								// current EXPRESSION w/o delimters
	std::stack <char> exp;										// process of Production Rules

	for (size_t i = 0; i < length; ++i) {
	
		FSA::Token end_sign;						
		end_sign.lexeme = "$";
		end_sign.category = 0;

		int tdp_r, tdp_c = 0;
		char tmp_exp_top;
		
		if(fsa.lex_storage[i].lexeme[0] != ';' && fsa.lex_storage[i].lexeme[0] != ' ') {	// Make a current EXPRESSION	
			c_terminal.push(fsa.lex_storage[i]);						// if lexeme is not a separator, add to current Expression	
		}
		if(fsa.lex_storage[i].lexeme[0] == ' ') {						// if lexeme is a ' '(space) or '\n'(line feed), skip it
		}
		if(fsa.lex_storage[i].lexeme[0] == ';') {						// if lexeme is ';' 
			exp.push('$');									// add '$' and 'E' to the "exp"stack
			exp.push('E'); 				
			c_terminal.push(end_sign);							// also add '$'(end_sign token) to c_terminal
			string rule;									
			rule.clear();

			while(exp.size() != 0) {							// processing the "exp" stack(=expression)
				tmp_exp_top = exp.top();						// store the top of the stack
				exp.pop();								

				tdp_r = get_cur_exp(tmp_exp_top);					// convert the EXPRESSION to an int to put into TABLE 

				if (c_terminal.front().category == 2) {					// check if the current character is identifier or not
					tdp_c = 0;
				} else {
					tdp_c = get_cur_char(c_terminal.front().lexeme[0]);		// get the current character and convert to an int for TABLE
					if (tdp_c == 9999) {break;}					// ERROR
				}
				
				rule = get_rule(tdp_r, tdp_c);						// use TABLE
				
				if (rule == "NULL") {							// case - Not Accepted
					fout << "Error : On this current lexeme, " << c_terminal.front().lexeme;
					fout << ", the syntax is wrong." << std::endl;
					rule.clear();
					break;
				} else if (rule == "e") {						// case - "epsilon"
					fout << "RULE: <" << tmp_exp_top << "> ---> <" << rule << ">" << std::endl;
					rule.clear();
				} else {								// case - a proper process
					fout << "RULE: <" << tmp_exp_top << "> ---> <" << rule << ">" << std::endl;
					while(!rule.empty()) {
						exp.push(rule.back());
						rule.pop_back();
					}
					rule.clear();
				}

				if (exp.top() == c_terminal.front().lexeme[0] || (exp.top() == 'i' && c_terminal.front().category == 2)) {	// the top of the "exp" matches "current character"
					if (c_terminal.front().lexeme != "$") { 
					fout << "---- Lexeme processed: " << c_terminal.front().lexeme << std::endl; 
					} else {
					fout << "---- Lexeme processed: ;"<< std::endl;
					}	
					exp.pop();
					c_terminal.pop();
				}
			}
		}
		
	}
	fout<<endl;
	fout<<endl;
	fout<<endl;
}
		
		


int main (int argc, char** argv) {
	if (argc != 2) {
		std::cout << "Usage: ./lexer targetFile.txt" << std::endl;
		std::cout << "You will have the  \"output.txt\" file of \"targetFile.txt\"" << std::endl;
		return 0;
	}
	std::fstream _file;
	_file.open(argv[1]);


	if (_file.is_open()) {
		TDP tdp;
		TDP::FSA fsa;		
		char c;
		int state = 0;
		int input;

		while(_file.get(c)) {
			input = fsa.get_input(c);        			//from character to input signal(int)
			if (input != 9999) {
				state = fsa.get_state(state, input);    	//to the next state
				fsa.state_check(state);            		//check the state is final or not, and save the lexeme or not
				if ( state == 0 ) {        			// every ending state need to check current 'char' again, in the textbook "BACKUP"
					state = fsa.get_state(state, input);
					fsa.state_check(state);
				}
			}
		}
		tdp.parse(fsa);						// table driven parsing (fsa)

		fsa.print();                    			//print storage on the "output.txt" 

	} else {
		std::cout << "cannot open file" << std::endl;
	}
	_file.close();
	return 0;
}
