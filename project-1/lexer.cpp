// CPSC323 assignment 1
// Yong Kim
//// - build a lexer

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <queue>
#include <array>
#include <iomanip>

using namespace std;
	// input           l  d  ! sp ( ) [ ] { } ' , . : ; * +  - = / > < % \n Unknown
typedef enum Input {
    LETTER,            		// 0    letter
    DIGIT,            		// 1    digit
    EXCLAMATION,        	// 2    !
    SPACE,            		// 3    sp
    OPEN_PAREN,        		// 4    (
    CLOSE_PAREN,        	// 5    )
    OPEN_BLANK,        		// 6    [
    CLOSE_BLANK,        	// 7    ]
    OPEN_CURLY_BLANK,   	// 8    {
    CLOSE_CURLY_BLANK,  	// 9    }
    APOS,            		// 10    '
    COMMA_SIGN,        		// 11    ,
    PERIOD_SIGN,        	// 12    .
    COLON,            		// 13    :
    SEMI_COLON,        		// 14    ;
    MULTI_SIGN,        		// 15    *
    PLUS_SIGN,        		// 16    +
    MINUS_SIGN,        		// 17    -
    EQUAL_SIGN,        		// 18    =
    DIVIDE_SIGN,        	// 19    /
    LEFT_GREATER,       	// 20    >
    RIGHT_GREATER,      	// 21    <
    MOD_SIGN,        		// 22    %
    LINE_END,        		// 23    \n
    UNKNOWN,        		// 24    ?

    // Input number, put a new input above this line
    INPUT_COUNT    		// 25
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
    END_INTEGER,        	// 7    end_integer
    END_REAL,        		// 8    end real number
    START_COMMENT,      	// 9    start ! comment
    END_COMMENT,        	// 10    end ! comment
    FOUND_SPACE,        	// 11    found sp
    FOUND_OPEN_PAREN,   	// 12    found (
    FOUND_CLOSE_PAREN,  	// 13    found )
    FOUND_OPEN_BLANK,   	// 14    found [
    FOUND_CLOSE_BLANK,  	// 15    found ]
    FOUND_OPEN_CUR_BLANK,    	// 16    found {
    FOUND_CLOSE_CUR_BLANK,    	// 17    found }
    FOUND_APOS,        		// 18    found '
    FOUND_COMMA,        	// 19    found ,
    FOUND_PERIOD,        	// 20    found .
    FOUND_COLON,        	// 21    found :
    FOUND_SEMICOLON,    	// 22    found ;
    FOUND_MULTI,        	// 23    found *
    FOUND_PLUS,        		// 24    found +
    FOUND_MINUS,        	// 25    found -
    FOUND_EQ,        		// 26    found =
    FOUND_DIVIDE,        	// 27    found /
    FOUND_LEFT_GREAT,    	// 28    found >
    FOUND_RIGHT_GREAT,    	// 29    found <
    FOUND_MOD,        		// 30    found %
    FOUND_LINE_END,        	// 31    found \n
    UNDEFINED,        		// 32    unknown
    START_REAL,			// 33	 start real

    // State number, put a new state above this line
    STATE_COUNT    		// 33
} State;
const size_t ROW_SIZE = STATE_COUNT; // for checking the # of rows

// Data structure for the FSA table
class FSA {
private:
    // input l  d  ! sp ( ) [ ] { } ' , . : ; * +  - = / > < % \n Unknown

    int fsa_table[ROW_SIZE][COL_SIZE] ={
        {1, 2, 9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},    // 0 start
        {1, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5},    // 1 identifier, keep going
        {32,2, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 33,7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7},    // 2 integer, keep going
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 3 regular end, will be used later
        {4, 4,10, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},    // 4 in comment, keep going
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 5 end string
        {8, 6, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8},    // 6 real, keep going
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 7 end integer
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 8 end real
        {4, 4,10, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4},    // 9 start ! comment
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 10 end ! comment
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 11 found sp
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 12 found '('
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 13 found ')'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 14 found '['
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 15 found ']'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 16 found '{'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 17 found '}'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 18 found '\''
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 19 found ','
        {0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 20 found '.'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 21 found ':'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 22 found ';'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 23 found '*'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 24 found '+'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 25 found '-'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 26 found '='
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 27 found '/'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 28 found '>'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},   	// 29 found '<'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},    // 30 found '%'
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	// 31 found '\n'
        {32,32,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	// 32 unknow TODO
        {7, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,32}	// 33 start real w/ '.'
    };

    std::array <std::string, 20> keywords = { "if", "else", "int", "float", "while", "bool",  "then", "do", "while", "whileend",
        "doend", "for", "and", "or", "function", "main", "char", "string"};       
    std::array <char, 10> operators = {'*', '=', '+', '-', '/', '>', '<', '%'};                        // temporary contatiner
    std::map<char, int> input_map;
    std::queue<char> tmp_lex;
    std::queue<char> cur_char;
    struct Token
    {
        int category;
        std::string lexeme;
    };
    std::vector<Token> lex_storage;

public:
    int get_input(const char&);                    	// categorize an input char to a number(enum)
    int get_state(const int&, const int&);           	// next state = table [current state] [input]
    void state_check(const int&);                    	// check the state
    void add_lexeme(const int&);                    	// add a lexeme w/o current char
    void print();                            		// print result
    bool is_keyword(const std::string&);                // check whether an identifier is a keyword or not
    bool is_operator(const char&);                    	// check whether a symbol is a operator or separator
};


int FSA::get_input (const char& c) {			// transform current character to an input signal
    if    (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '$') {    // excluded ' || c == '_'  '
        input_map[c] = LETTER;
    }
    else if    ('0' <= c && c <= '9') {
        input_map[c] = DIGIT;
    }
    else if (c == '!') {
        input_map[c] = EXCLAMATION;
    }
    else if (c == ' ') {
        input_map[c] = SPACE;
    }
    else if (c == '(') {
        input_map[c] = OPEN_PAREN;
    }
    else if (c == ')') {
        input_map[c] = CLOSE_PAREN;
    }
    else if (c == '[') {
        input_map[c] = OPEN_BLANK;
    }
    else if (c == ']') {
        input_map[c] = CLOSE_BLANK;
    }
    else if (c == '{') {
        input_map[c] = OPEN_CURLY_BLANK;
    }
    else if (c == '}') {
        input_map[c] = CLOSE_CURLY_BLANK;
    }
    else if (c == '\'') {
        input_map[c] = APOS;
    }
    else if (c == ',') {
        input_map[c] = COMMA_SIGN;
    }
    else if (c == '.') {
        input_map[c] = PERIOD_SIGN;
    }
    else if (c == ':') {
        input_map[c] = COLON;
    }
    else if (c == ';') {
        input_map[c] = SEMI_COLON;
    }
    else if (c == '*') {
        input_map[c] = MULTI_SIGN;
    }
    else if (c == '+') {
        input_map[c] = PLUS_SIGN;
    }
    else if (c == '-') {
        input_map[c] = MINUS_SIGN;
    }
    else if (c == '=') {
        input_map[c] = EQUAL_SIGN;
    }
    else if (c == '/') {
        input_map[c] = DIVIDE_SIGN;
    }
    else if (c == '>') {
        input_map[c] = LEFT_GREATER;
    }
    else if (c == '<') {
        input_map[c] = RIGHT_GREATER;
    }
    else if (c == '%') {
        input_map[c] = MOD_SIGN;
    }
    else if (c == '\n') {
        input_map[c] = LINE_END;
    }
    else {
        //cout << "invalid input!" << std::endl;		// for TEST
        return 9999;
    }
    cur_char.push(c);
    return input_map[c];
}

int FSA::get_state (const int& state, const int& input) {	// state change
    return fsa_table[state][input];
}

void FSA::state_check(const int& state) {			// based on states, call add_lexeme, back up the current state, or throw out current character (comment)
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


void FSA::add_lexeme (const int& state) {			// based on state, store the character(s) in lex_storage with category

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
void FSA::print() {						// make the "output.txt" file
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
                fout << left << setw(10) << "Separator" << setw(5) << ":" << "sp(space)" << std::endl;
            }
            else {
                fout << left << setw(10) << "Separator" << setw(5) << ":" << lex_storage[i].lexeme << std::endl;
            }
        }
        else {
            fout << left << setw(5) << "???" << std::endl;
        }
    }

//    for (i = 0; i < lex_storage.size(); ++i) {		// for TEST
//	    std::cout << "index[" << i << "] : " << lex_storage[i].lexeme << std::endl;
//	    }

    fout << flush;
    fout.close();
    return;
}

bool FSA::is_keyword(const std::string& end_string) {		// check whether a string is a keyword or not
    bool is_keyword = false;
    for (int i = 0; i < keywords.size(); ++i) {
        if (end_string == keywords[i]) {
            is_keyword = true;
        }
    }
    return is_keyword;
}

bool FSA::is_operator(const char& c) {				// check whether a symbol is an operator or not(separator)
    bool is_operator = false;
    for (int i = 0; i < operators.size(); ++i) {
        if (c == operators[i]) {
            is_operator = true;
        }
    }
    return is_operator;
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

        FSA fsa;
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

        fsa.print();                    			//print storage on the "output.txt"

    } else {
        std::cout << "cannot open file" << std::endl;
    }


    _file.close();
    return 0;
}
