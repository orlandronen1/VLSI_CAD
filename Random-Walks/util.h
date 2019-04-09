// $Id: util.h,v 1.1.1.1 2004/07/06 20:40:39 cvsdevel Exp $
//    Frank Liu    draft     11/01/2003

// basic utilities

#ifndef _UTIL_H
#define _UTIL_H

#include <string.h>
#include <stdlib.h>
#include <math.h>

#define SkipBlanks(cptr) while (*(cptr) && *(cptr) <= ' ') (cptr)++;

void Tokenizer(char *buf, DType *dt, char *from, char *to, double &val1, double &val2) {
  int i;
  char tmp[64];

  SkipBlanks(buf);
  for(i=0 ;i<=128&& !((*buf)==' '||(*buf)=='\t'||(*buf)=='\0');i++) {
    tmp[i] = toupper(*buf++);
  }
  tmp[i]='\0';
  if ( strcmp(tmp, "WIRE")==0 ) {
    *dt = WIRE;
  } else if ( strcmp(tmp, "DCAP") == 0 ) {
    *dt = DCAP;
  } else if ( strcmp(tmp, "LOAD") == 0 ) {
    *dt = LOAD;
  } else if ( strcmp(tmp, "SUPP") == 0 ) {
    *dt = SUPP;
  } else if ( strcmp(tmp, "SOLV") == 0 ) {
    *dt = SOLV;
  } else {
    *dt = UNKNOWN;
  }

  SkipBlanks(buf);
  for(i=0 ;i<=128&& !((*buf)==' '||(*buf)=='\t'||(*buf)=='\0'||(*buf)=='\n');i++) {
    tmp[i] = *buf++;
  }
  tmp[i]='\0';
  strcpy(from,tmp);

  if ( *dt == SOLV ) return;

  if ( *dt == WIRE ) {
    SkipBlanks(buf);
    for(i=0 ;i<=128&& !((*buf)==' '||(*buf)=='\t'||(*buf)=='\0');i++) {
      tmp[i] = *buf++;
    }
    tmp[i]='\0';
    strcpy(to, tmp);
  }  else {
    tmp[0] = '\0';
      strncpy(to, tmp, 1);
  }

  SkipBlanks(buf);
  for(i=0 ;i<=128&& !((*buf)==' '||(*buf)=='\t'||(*buf)=='\0');i++) {
    tmp[i] = *buf++;
  }
  tmp[i]='\0';
  val1 = atof(tmp);

  if ( *dt == WIRE ) {
    SkipBlanks(buf);
    for(i=0 ;i<=128&& !((*buf)==' '||(*buf)=='\t'||(*buf)=='\0');i++) {
      tmp[i] = *buf++;
    }
    tmp[i]='\0';
    val2 = atof(tmp);
  } else {
    val2 = 0.0;
  }
}

// returns a double rand number (between 0 and 1)
double dbl_rand() {
  return ( (double)rand()/(double)RAND_MAX );
}

// test if a double is close to zero
bool is_dbl_zero(double a) {
  return ( fabs(a)<1e-6 ? true : false );
}

#endif

// end
