#ifndef LEX_H_
#define LEX_H_

#define LEN_PATH 4097
#define NUM_KEYWORDS 12

extern char sourcePath[LEN_PATH];


//Token-related data types
typedef enum TOKEN_TYPE {OPEN_BRACE, CLOSED_BRACE, OPEN_PAREN, CLOSED_PAREN, SEMICOLON,
                         INT_KEYW, RET_KEYW, INT_LITERAL, IDENTIFIER,
                         NEGATION, BITWISE_COMP, LOGIC_NEG} TOKEN_TYPE;

//Tokenlist node, contains token data and pointer to next token (if available)
typedef struct token_t {
  char *value;
  TOKEN_TYPE type;
  struct token_t *next;
} token_t;

//Tokenlist type
typedef struct tokenlist_t {
  token_t *head;
  token_t *tail;
  int numTokens;
} tokenlist_t;


//Regex functions
void initRegexp();
token_t *createToken(char *value, TOKEN_TYPE type);
void freeRegs();


//Token functions
tokenlist_t *initTokenlist();
token_t *popToken(tokenlist_t *tokens);
void appendToken(tokenlist_t *tokens, token_t *token);
void printTokens(tokenlist_t *tokens);
void freeTokens(tokenlist_t *tokens);

tokenlist_t *lex();

//Other functions
void printSubstr(char *line, int start, int end);
#endif // LEX_H_
