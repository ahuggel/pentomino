/*
   PENTOMINO

   Copyright (C) 1995-2023 Andreas Huggel <ahuggel@gmx.net>

   Permission is hereby granted, free of charge, to any person obtaining a copy of this software
   and associated documentation files (the "Software"), to deal in the Software without 
   restriction, including without limitation the rights to use, copy, modify, merge, publish, 
   distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the 
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in all copies or 
   substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
   BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
   DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/************************************************/
/* pentomino.h                                  */
/* Header file for pentomino.c                  */
/************************************************/

#ifndef PENTOMINO_H
#define PENTOMINO_H

#ifndef TRUE
#define TRUE  1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef OK
#define OK    1
#endif
#ifndef NOTOK
#define NOTOK 0
#endif

#ifndef min
#define min(a, b)  ((a) > (b) ? (b) : (a))
#endif
#ifndef max
#define max(a, b)  ((a) > (b) ? (a) : (b))
#endif


/* Command line argument string for getopt */
/* c        : print colored output         */
/* n        : nosort                       */
/* s tryfile: read tryfile and solve it    */
/* p scale  : spawn scale processes        */
/* v        : verbose                      */
#ifdef SCARCH
#define ARGS      ":cnp:s:v"
#define USAGE_STR "Usage: %s [-cv] [-n|-s tryfile] [-p scale] inifile\n"
#else
#define ARGS      ":cns:v"
#define USAGE_STR "Usage: %s [-cv] [-n|-s tryfile] inifile\n"
#endif

/* Global program info */
struct pinfo {
   char *progname; 
   char *inifile;  
   char *tryfile;  
   int  color;
   int  verbose;
   int  sort;
#ifdef SCARCH
   int  scale;
#endif
};


/* Global program information */
extern struct pinfo prg;

#endif /* ! PENTOMINO_H */
