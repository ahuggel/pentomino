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
/* print.c                                      */
/* Provides functions for simple tty output.    */
/************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pentomino.h"
#include "data.h"
#include "setup.h"
#ifdef SCARCH
#include <unistd.h>
#include "scarch.h"
#endif
#include "print.h"

/* Background color codes lookup table */
char* colors[] = {
    "\033[41m",
    "\033[42m",
    "\033[43m",
    "\033[44m",
    "\033[45m",
    "\033[46m",
    "\033[47m",
    "\033[100m",
    "\033[101m",
    "\033[103m",
    "\033[105m",
    "\033[106m",
    "\033[104m",
    "\033[102m"
};

#define NUM_COLORS 14
#define EMPTY_BRICK_COLOR "\033[40m"

/******************************/
/* we found a solution, so... */
/******************************/

/* this is just a first and very quick try */
void display_it(void)
{
   brickT *disp;
   char brick='a';
   
   struct tnode *piece;
   struct pnode *pos;
   int i,j;

#ifdef SCARCH
   cntT *all_counter;
   all_counter = (cntT *)prctl.shmaddr;
   P (prctl.semid);
   (*all_counter)++; /* increase the global counter */
#endif /* SCARCH */

   gme.counter++; /* increase the per process counter */

   if (!prg.quiet)
   {
      /* now print the solution as a brick field */

      disp = b_alloc();

      /* fill the array with all pieces */
   
      for (piece=gme.first_piece; piece != NULL; piece=piece->next)
      {
         pos = piece->pos;
      
         if (pos != NULL)
         {
            for (i=0; i<YDIM; i++)
            {     
               for (j=0; j<XDIM; j++)
               {
                  if (f_testxy (pos->field, j, i))
                  {
                     b_setxy (disp, j, i, brick);
                  }
               }
            }
            brick++;
         }
      }

      if (prg.verbose)
      {
         /* print general info */

#ifdef SCARCH
         fprintf (stdout, "%d. Solution (%d. for pid %d):\n",
                  *all_counter, gme.counter, (int) prctl.pid);
#else /* ! SCARCH */
         fprintf (stdout, "%d. Solution:\n", gme.counter);
#endif /* ! SCARCH */
      
         prt_time (stdout);
         fprintf (stdout, "\n");

         /* print the brick field */

         print_bfield (*disp);
         fprintf (stdout, "\n\n");
      }
      else
      {
         /* print only a simple brick field */
      
         print_bfield (*disp);
         fprintf (stdout, "\n");
      }

      b_free (disp);
   }

#ifdef SCARCH
   if (!prg.quiet)
   {
      fflush(stdout);
   }
   V (prctl.semid);
#endif /* SCARCH */

}


/****************************/
/* print the list of pieces */
/****************************/

void list_pieces(void)
{
   struct tnode *curr_piece;
   
   for (curr_piece  = gme.first_piece;
        curr_piece != NULL;
        curr_piece  = curr_piece->next)
   {
      fprintf(stdout,"%s\t%2i\t: %2i Ones, %3i Pos.\n",
              curr_piece->name, curr_piece->number,
              curr_piece->one_count, curr_piece->pos_count);
   }
   fprintf(stdout,"\n");
}


/***************************/
/* print a field to stdout */
/***************************/

void print_field(struct fieldT field)
{
   int i,j;

   for (i=0; i<YDIM; i++)
   {     
      for (j=0; j<XDIM; j++)
      {
         fprintf(stdout, "%c", (f_testxy(field, j, i) ? FIELD_SET
                                                      : FIELD_NOT_SET));
      }
      fprintf(stdout, "%c",'\n');
   }
}


/*********************************/
/* print a brick field to stdout */
/*********************************/

void print_bfield(brickT bfield)
{
   int i,j;
   char a;
   char *c;

   if (prg.color) {
      for (i=0; i<YDIM; i++)
      {     
         for (j=0; j<XDIM; j++)
         {
            a = b_getxy(bfield, j, i);
            c = a == EMPTY_BRICK ? EMPTY_BRICK_COLOR : colors[abs(a - 'a') % NUM_COLORS];
            fprintf(stdout, "%s  ", c);
         }
         fprintf(stdout, "\033[0m\n");
      }
   } else {
      for (i=0; i<YDIM; i++)
      {     
         for (j=0; j<XDIM; j++)
         {
            fprintf(stdout, "%c", b_getxy(bfield, j, i));
         }
         fprintf(stdout, "%c",'\n');
      }
   }
}



/**************************/
/* prints the system time */
/**************************/

void prt_time(FILE *out_stream)
{
   time_t time_now;
   
   time_now = time(NULL);

   if (time_now != -1 && out_stream != NULL)
   {
      fprintf(out_stream, "%s", ctime(&time_now));
   }
}   
