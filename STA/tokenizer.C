/* Copyright (c) 2005 ACM/SIGDA
   Prepared by Ying (Frank) Liu, frankliu@us.ibm.com
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "tokenizer.h"

Tokenizer::Tokenizer() 
{
  for (int i=0; i<MAX_TOKENS; i++) {
    _buffer[i] = (char*)malloc(MAX_LENGTH*sizeof(double));
  }
  CleanBuffer();
}

Tokenizer::~Tokenizer() 
{
  for (int i=0; i<MAX_TOKENS; i++) {
    free (_buffer[i]);
  }
}

char *Tokenizer::SkipBlanks(char *pos) 
{
  if ( *pos != ' ' && *pos != '\t' ) return (pos);
  while ( *pos == ' ' || *pos == '\t' ) {
    pos++;
  }
  return (pos);
}

int Tokenizer::FindString(char *pos) 
{
  int sz=0;
  
  while ( *pos != ' ' && *pos != '\t' && *pos != '\0' && *pos != '\n' ) {
    pos++;
    sz++;
  }
  return (sz);
}

void Tokenizer::CleanBuffer() 
{
  for (int i=0; i<MAX_TOKENS; i++) {
    memset(_buffer[i],'\0', MAX_LENGTH*sizeof(char));
  }
  memset(_last_str, '\0', (MAX_LENGTH*(MAX_TOKENS+5))*sizeof(char));
  _last_size = 0;
}

/* 
   This is the function actually does the work
   returns the number of tokens in the string. The tokens are in "tokens"
   */
int Tokenizer::Parse(char *str, char ***tokens) 
{
  int counter, size;
  char *cur;
  
  counter = 0;
  size = 1;
  cur = str;
  
  // special treatment for  #, *, %, indicating comments
  if ( *cur == '\0' ) {
	//printf("NULL STRING!\n");
    *tokens = 0;
    return (counter);
  }
  
  // in case we are accidently called twice
  if ( strcmp(cur, _last_str) == 0 ) {
	//printf("Warning: parse called twice!\n");
    return _last_size;
  }
  
  strcpy(_last_str, str); 
  CleanBuffer();
  cur = str;

  if ( *cur == '#' || *cur == '*' || *cur == '%' ) {
    strncpy(_buffer[counter++], cur, size);
    cur++;
  }
  
  // loop through the end
  while ( *cur != '\0' ) {
    if ( *cur == ' ' || *cur == '\t' ) {
      cur = SkipBlanks(cur);
    }

    if ( *cur == '\0' || *cur == '\n' ) break;
    size = FindString(cur);
    strncpy(_buffer[counter++], cur, size);
    cur += size;
  }

  *tokens = &(_buffer[0]);
  _last_size = counter;
  return (counter);
}
