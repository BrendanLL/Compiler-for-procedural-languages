#include "lexer.h"
#include "kind.h"
using std::string;
using std::vector;

string earses(string in, char a){
  in.erase (in.begin()+in.find(a));
  return in;
}

// Use the annonymous namespace to prevent external linking
namespace {
  // An array represents the Token kind that each state represents
  // The ERR Kind represents an invalid token
  ASM::Kind stateKinds[] = {
   ASM::ID,//    ST_ID,  
   ASM::NUM,//    ST_NUM,
   ASM::LPAREN,//    ST_LPAREN,
   ASM::RPAREN,//    ST_RPAREN,
   ASM::LBRACE,//    ST_LBRACE,
   ASM::RBRACE,//    ST_RBRACE,
   ASM::RETURN,//    ST_RETURN,
   ASM::IF,//    ST_IF,
   ASM::ELSE,//    ST_ELSE,
   ASM::WHILE,//    ST_WHILE,
   ASM::PRINTLN,//    ST_PRINTLN,
   ASM::WAIN,//    ST_WAIN,
   ASM::BECOMES,//    ST_BECOMES,
   ASM::INT,//    ST_INT,
   ASM::EQ,//    ST_EQ,
   ASM::NE,//    ST_NE,
   ASM::LT,//   ST_LT,
   ASM::GT,//    ST_GT,
   ASM::LE,//    ST_LE,
   ASM::GE,//   ST_GE,
   ASM::PLUS,//    ST_PLUS,
   ASM::MINUS,//    ST_MINUS,
   ASM::STAR,//    ST_STAR,
   ASM::SLASH,//    ST_SLASH,
   ASM::PCT,//    ST_PCT,
   ASM::COMMA,//    ST_COMMA,
   ASM::SEMI,//    ST_SEMI,
   ASM::NEW,//    ST_NEW,
   ASM::DELETE,//    ST_DELETE,
   ASM::LBRACK,//    ST_LBRACK,
   ASM::RBRACK,//    ST_RBRACK,
   ASM::AMP,//    ST_AMP,
   ASM::NULL_,//    ST_NULL_,
   ASM::WHITESPACE,//    ST_WHITESPACE,
   ASM::NUM,//    ST_ZERO,
   ASM::WHITESPACE,//    ST_COMMENT,
  //    //ID BELOW
   ASM::ID,//    ST_I, //int if
   ASM::ID,//   ST_IN,
   ASM::ID,//    ST_R, //return
   ASM::ID,//    ST_RE,
   ASM::ID,//    ST_RET,
   ASM::ID,//    ST_RETU,
   ASM::ID,//    ST_RETUR,
   ASM::ID,//    ST_E, // ELSE
   ASM::ID,//    ST_EL, 
   ASM::ID,//    ST_ELS,
   ASM::ID,//    ST_W, //WHILE
   ASM::ID,//    ST_WH,
   ASM::ID,//    ST_WHI,
   ASM::ID,//    ST_WHIL,
   ASM::ID,//    ST_P,//PRINTLN
   ASM::ID,//    ST_PR,
   ASM::ID,//    ST_PRI,
   ASM::ID,//    ST_PRIN,
   ASM::ID,//    ST_PRINT,
   ASM::ID,//    ST_PRINTL,
  //WAIN
   ASM::ID,//    ST_WA,
   ASM::ID,//    ST_WAI,
   ASM::ID,//    ST_N, //NEW NULL
   ASM::ID,// st_NX
   ASM::ID,//    ST_NEX,
   ASM::ID,//    ST_NU,
   ASM::ID,//    ST_NUL,
   ASM::ID,//    ST_D,//DELETE
   ASM::ID,//    ST_DE,
   ASM::ID,//    ST_DEL,
   ASM::ID,//    ST_DELE,
   ASM::ID,//    ST_DELET,
      //ERROR BELOW
   ASM::ERR,//   ST_ERR, // ERROR TERM
   ASM::ERR,//   ST_START, // START TERM
   ASM::ERR,//   ST_NOT  // !
  };
  const string whitespace = "\t\n\r ";
  const string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  const string digits = "0123456789";
  const string idinput = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  const string oneToNine =  "123456789";
}

ASM::Lexer::Lexer(){
  // Set default transitions to the Error state
  for(int i=0; i < maxStates; ++i){
    for(int j=0; j < maxTrans; ++j){
      delta[i][j] = ST_ERR;
    }
  }
  // Change transitions as appropriate for the MIPS recognizer
  setTrans( ST_START,      whitespace,     ST_WHITESPACE );
  setTrans( ST_WHITESPACE, whitespace,     ST_WHITESPACE );
  // SIGNLE TERM
  setTrans( ST_START,             "(",          ST_LPAREN);
  setTrans( ST_START,             ")",          ST_RPAREN);
  setTrans( ST_START,             "{",          ST_LBRACE);
  setTrans( ST_START,             "}",          ST_RBRACE);
  setTrans( ST_START,             "+",          ST_PLUS);
  setTrans( ST_START,             "-",          ST_MINUS);
  setTrans( ST_START,             "*",          ST_STAR);
  setTrans( ST_START,             "/",          ST_SLASH);
  setTrans( ST_SLASH,             "/",          ST_COMMENT);
  setTrans( ST_START,             "%",          ST_PCT);
  setTrans( ST_START,             ",",          ST_COMMA);
  setTrans( ST_START,             ";",          ST_SEMI);
  setTrans( ST_START,             "[",          ST_LBRACK);
  setTrans( ST_START,             "]",          ST_RBRACK);
  setTrans( ST_START,             "&",          ST_AMP);  
  //NUM TERM
  setTrans( ST_START,             "0",          ST_ZERO);
  setTrans( ST_ZERO,          idinput,          ST_NOT);
  setTrans( ST_START,       oneToNine,          ST_NUM);
  setTrans( ST_NUM,            digits,          ST_NUM);
  // = TERM
  setTrans( ST_START,             "=",          ST_BECOMES);
  setTrans( ST_BECOMES,           "=",          ST_EQ);
  setTrans( ST_START,             "<",          ST_LT);
  setTrans( ST_START,             ">",          ST_GT);
  setTrans( ST_LT,                "=",          ST_LE);
  setTrans( ST_GT,                "=",          ST_GE);
  setTrans( ST_START,             "!",          ST_NOT);
  setTrans( ST_NOT,               "=",          ST_NE);
  // id term  str.erase (str.begin()+str.find('e'));
  setTrans( ST_START,             "i",          ST_I); //int
  setTrans( ST_I,                 "n",          ST_IN);
  setTrans( ST_IN,                "t",          ST_INT);
  setTrans( ST_I,                 "f",          ST_IF);//IF
  setTrans( ST_START,             "r",          ST_R); //RETURN
  setTrans( ST_R,                 "e",          ST_RE);
  setTrans( ST_RE,                "t",          ST_RET);
  setTrans( ST_RET,               "u",          ST_RETU);
  setTrans( ST_RETU,              "r",          ST_RETUR);
  setTrans( ST_RETUR,             "n",          ST_RETURN);
  setTrans( ST_START,             "e",          ST_E);//ELSE
  setTrans( ST_E,                 "l",          ST_EL);
  setTrans( ST_EL,                "s",          ST_ELS);
  setTrans( ST_ELS,               "e",          ST_ELSE);
  setTrans( ST_START,             "w",          ST_W);//WHILE
  setTrans( ST_W,                 "h",          ST_WH);
  setTrans( ST_WH,                "i",          ST_WHI);
  setTrans( ST_WHI,               "l",          ST_WHIL);
  setTrans( ST_WHIL,              "e",          ST_WHILE);
  setTrans( ST_START,             "p",          ST_P);//PRINTLN
  setTrans( ST_P,                 "r",          ST_PR);
  setTrans( ST_PR,                "i",          ST_PRI);
  setTrans( ST_PRI,               "n",          ST_PRIN);
  setTrans( ST_PRIN,              "t",          ST_PRINT);
  setTrans( ST_PRINT,             "l",          ST_PRINTL);
  setTrans( ST_PRINTL,            "n",          ST_PRINTLN);
  setTrans( ST_START,             "w",          ST_W);//WAIN
  setTrans( ST_W,                 "a",          ST_WA);
  setTrans( ST_WA,                "i",          ST_WAI);
  setTrans( ST_WAI,               "n",          ST_WAIN);
  setTrans( ST_START,             "n",          ST_N);//NEW
  setTrans( ST_N,                 "e",          ST_NEX);
  setTrans( ST_NEX,               "w",          ST_NEW);
  setTrans( ST_START,             "N",          ST_NX);//NULL
  setTrans( ST_NX,                "U",          ST_NU);
  setTrans( ST_NU,                "L",          ST_NUL);
  setTrans( ST_NUL,               "L",          ST_NULL);
  setTrans( ST_START,             "d",          ST_D);//DELETE
  setTrans( ST_D,                 "e",          ST_DE);
  setTrans( ST_DE,                "l",          ST_DEL);
  setTrans( ST_DEL,               "e",          ST_DELE);
  setTrans( ST_DELE,              "t",          ST_DELET);
  setTrans( ST_DELET,             "e",          ST_DELETE);
// TO NORMAL ID
  setTrans( ST_INT,           idinput,          ST_ID);
  setTrans( ST_IF,            idinput,          ST_ID);
  setTrans( ST_ELSE,          idinput,          ST_ID);
  setTrans( ST_RETURN,        idinput,          ST_ID);
  setTrans( ST_WHILE,         idinput,          ST_ID);
  setTrans( ST_WAIN,          idinput,          ST_ID);
  setTrans( ST_NEW,           idinput,          ST_ID);
  setTrans( ST_NULL,          idinput,          ST_ID);
  setTrans( ST_ID,            idinput,          ST_ID);
  setTrans( ST_I,  earses(earses(idinput,'n'),'f'),        ST_ID);
  setTrans( ST_IN,           earses(idinput,'t'),          ST_ID);  
  setTrans( ST_R,            earses(idinput,'e'),          ST_ID);
  setTrans( ST_RE,           earses(idinput,'t'),          ST_ID);
  setTrans( ST_RET,          earses(idinput,'u'),          ST_ID);
  setTrans( ST_RETU,         earses(idinput,'r'),          ST_ID);
  setTrans( ST_RETUR,        earses(idinput,'n'),          ST_ID);
  setTrans( ST_E,            earses(idinput,'l'),          ST_ID);
  setTrans( ST_EL,           earses(idinput,'s'),          ST_ID);
  setTrans( ST_ELS,          earses(idinput,'e'),          ST_ID);
  setTrans( ST_W,   earses(earses(idinput,'h'),'a'),       ST_ID);
  setTrans( ST_WH,           earses(idinput,'i'),          ST_ID);
  setTrans( ST_WHI,          earses(idinput,'l'),          ST_ID);
  setTrans( ST_WHIL,         earses(idinput,'e'),          ST_ID);
  setTrans( ST_P,            earses(idinput,'r'),          ST_ID);
  setTrans( ST_PR,           earses(idinput,'i'),          ST_ID);
  setTrans( ST_PRI,          earses(idinput,'n'),          ST_ID);
  setTrans( ST_PRIN,         earses(idinput,'t'),          ST_ID);
  setTrans( ST_PRINT,        earses(idinput,'l'),          ST_ID);
  setTrans( ST_PRINTL,       earses(idinput,'n'),          ST_ID);
  setTrans( ST_WA,           earses(idinput,'i'),          ST_ID);
  setTrans( ST_WAI,          earses(idinput,'n'),          ST_ID);
  setTrans( ST_N,            earses(idinput,'e'),          ST_ID);
  setTrans( ST_NEX,          earses(idinput,'w'),          ST_ID);
  setTrans( ST_NX,           earses(idinput,'U'),          ST_ID);
  setTrans( ST_NU,           earses(idinput,'L'),          ST_ID);
  setTrans( ST_NUL,          earses(idinput,'L'),          ST_ID);
  setTrans( ST_D,            earses(idinput,'e'),          ST_ID);
  setTrans( ST_DE,           earses(idinput,'l'),          ST_ID);
  setTrans( ST_DEL,          earses(idinput,'e'),          ST_ID);
  setTrans( ST_DELE,         earses(idinput,'t'),          ST_ID);
  setTrans( ST_DELET,        earses(idinput,'e'),          ST_ID);
  setTrans( ST_START,"ABCDEFGHIJKLMOPQRSTUVWXYZabcfghjklmoqstuvxyz",ST_ID);

  // A comment can only ever lead to the comment state
  for(int j=0; j < maxTrans; ++j) delta[ST_COMMENT][j] = ST_COMMENT;
}

// Set the transitions from one state to another state based upon characters in the
// given string
void ASM::Lexer::setTrans(ASM::State from, const string& chars, ASM::State to){
  for(string::const_iterator it = chars.begin(); it != chars.end(); ++it)
    delta[from][static_cast<unsigned int>(*it)] = to;
}

// Scan a line of input (as a string) and return a vector
// of Tokens representing the MIPS instruction in that line
vector<ASM::Token*> ASM::Lexer::scan(const string& line){
  // Return vector
  vector<Token*> ret;
  if(line.size() == 0) return ret;
  // Always begin at the start state
  State currState = ST_START;
  // startIter represents the beginning of the next Token
  // that is to be recognized. Initially, this is the beginning
  // of the line.
  // Use a const_iterator since we cannot change the input line
  string::const_iterator startIter = line.begin();
  // Loop over the the line
  for(string::const_iterator it = line.begin();;){
    // Assume the next state is the error state
    State nextState = ST_ERR;
    // If we aren't done then get the transition from the current
    // state to the next state based upon the current character of
    // input
    if(it != line.end())
      nextState = delta[currState][static_cast<unsigned int>(*it)];
    // If there is no valid transition then we have reach then end of a
    // Token and can add a new Token to the return vector
    if(ST_ERR == nextState){
      // Get the kind corresponding to the current state
      Kind currKind = stateKinds[currState];
      // If we are in an Error state then we have reached an invalid
      // Token - so we throw and error and delete the Tokens parsed
      // thus far
      if(ERR == currKind){
        vector<Token*>::iterator vit;
        for(vit = ret.begin(); vit != ret.end(); ++vit)
          delete *vit;
        throw "ERROR in lexing after reading " + string(line.begin(),it);
      }
      // If we are not in Whitespace then we push back a new token
      // based upon the kind of the State we end in
      // Whitespace is ignored for practical purposes
      if(WHITESPACE != currKind)
        ret.push_back(Token::makeToken(currKind,string(startIter,it)));
      // Start of next Token begins here
      startIter = it;
      // Revert to start state to begin recognizing next token
      currState = ST_START;
      if(it == line.end()) break;
    } else {
      // Otherwise we proceed to the next state and increment the iterator
      currState = nextState;
      ++it;
    }
  }
  return ret;
}
