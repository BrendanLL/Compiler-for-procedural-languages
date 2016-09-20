#ifndef __LEXER_H__
#define __LEXER_H__
#include <string>
#include <vector>

namespace ASM{
  // The different states the the MIPS recognizer uses
  // Judicious use of the pimpl idiom or the Bridge pattern
  // would allow us to place this in the implementation file
  // However, that's more complexity than is necessary
  enum State {
      ST_ID,  
      ST_NUM,
      ST_LPAREN,
      ST_RPAREN,
      ST_LBRACE,
      ST_RBRACE,
      ST_RETURN,
      ST_IF,
      ST_ELSE,
      ST_WHILE,
      ST_PRINTLN,
      ST_WAIN,
      ST_BECOMES,
      ST_INT,
      ST_EQ,
      ST_NE,
      ST_LT,
      ST_GT,
      ST_LE,
      ST_GE,
      ST_PLUS,
      ST_MINUS,
      ST_STAR,
      ST_SLASH,
      ST_PCT,
      ST_COMMA,
      ST_SEMI,
      ST_NEW,
      ST_DELETE,
      ST_LBRACK,
      ST_RBRACK,
      ST_AMP,
      ST_NULL,
      ST_WHITESPACE,
      ST_ZERO,
      ST_COMMENT,
      //ID BELOW
      ST_I, //int if
      ST_IN,
      ST_R, //return
      ST_RE,
      ST_RET,
      ST_RETU,
      ST_RETUR,
      ST_E, // ELSE
      ST_EL, 
      ST_ELS,
      ST_W, //WHILE
      ST_WH,
      ST_WHI,
      ST_WHIL,
      ST_P,//PRINTLN
      ST_PR,
      ST_PRI,
      ST_PRIN,
      ST_PRINT,
      ST_PRINTL,
     //WAIN
      ST_WA,
      ST_WAI,
      ST_N, //NEW NULL
      ST_NX,
      ST_NEX,
      ST_NU,
      ST_NUL,
      ST_D,//DELETE
      ST_DE,
      ST_DEL,
      ST_DELE,
      ST_DELET,
      //ERROR BELOW
      ST_ERR, // ERROR TERM
      ST_START, // START TERM
      ST_NOT  // !
    };
  // Forward declare the Token class to reduce compilation dependencies
  class Token;

  // Class representing a MIPS recognizer
  class Lexer {
    // At most 21 states and 256 transitions (max number of characters in ASCII)
    static const int maxStates = 100;
    static const int maxTrans = 256;
    // Transition function
    State delta[maxStates][maxTrans];
    // Private method to set the transitions based upon characters in the
    // given string
    void setTrans(State from, const std::string& chars, State to);
  public:
    Lexer();
    // Output a vector of Tokens representing the Tokens present in the
    // given line
    std::vector<Token*> scan(const std::string& line);
  };
}

#endif