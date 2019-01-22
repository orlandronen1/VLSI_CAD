/* Copyright (c) 2005 ACM/SIGDA
   Prepared by Ying (Frank) Liu, frankliu@us.ibm.com
*/

/*
  Simple tokenizer: parse a text line, tokenize the line into different 
  fields separated by either space or tab. Any '#', '*' and '%' at the 
  beginning of the line will be separated so that it can be treated as a
  comment
*/

#ifndef TOKENIZER_H
#define TOKENIZER_H

const int     MAX_TOKENS = 32;  // max number of tokens 
const int     MAX_LENGTH = 128; // max length each token

class Tokenizer {
 private:
  int           _last_size;
  char          _last_str[MAX_LENGTH*(MAX_TOKENS+5)];
  char         *_buffer[MAX_TOKENS];
  
  char *SkipBlanks(char *pos);
  int  FindString(char *str);
  void CleanBuffer();

 public:
  Tokenizer();
  ~Tokenizer();
  int Parse(char *str, char ***tokens);
};

#endif
