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
/* read.c                                       */
/* Provides a simple file input interface       */
/************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pentomino.h"
#include "data.h"
#include "setup.h"
#include "print.h"
#include "read.h"


/* local function declarations */

static void          add_line    (struct tnode *, char *, int);   
static FILE *        f_r_open    (const char *);


/**************************************/
/* read the inifile                   */
/**************************************/

void read_pieces(struct tnode **first_piece, unsigned int *piece_count)
{
   const size_t  piece_id_len = strlen(PIECE_ID); 

   char          fline[MAXLINE];
   char         *tname = NULL;
   int           tnum = 0;
   int           line_ctr=0;
   struct tnode *curr_piece  = NULL;

   FILE         *fp;

   *first_piece = NULL;

   fp = f_r_open(prg.inifile);
   
   if (prg.verbose)
   {
      fprintf(stdout, "Pieces defined in %s:\n", prg.inifile);
   }
   
   while ((fgets(fline, MAXLINE, fp)) != NULL)
   {
      line_ctr++;
	 
      if (strlen(fline) >= piece_id_len &&
          0 == strncmp(fline, PIECE_ID, piece_id_len))
      {
         if (curr_piece != NULL)
         {
            if (curr_piece->pos_list == NULL)
            {
               fprintf(stderr, "%s: %s, line %d: Next identifier before definition "
                       "of piece %s (No. %d)\n",
                       prg.progname, prg.inifile, line_ctr,
                       curr_piece->name, curr_piece->number);
               exit(1);
            }

            if (prg.verbose)
            {
               fprintf(stdout, "%2i. %s\n", curr_piece->number, curr_piece->name);
#ifdef DEBUG
               print_field(curr_piece->pos_list->field);
#endif
            }
         }
         tname = fline + piece_id_len;
         tname[strlen(tname)-1] = '\0'; 
         tnum++;

         curr_piece = t_alloc(tname, tnum);
         t_add(first_piece, curr_piece);
         (*piece_count)++;	    

      } /* if (strlen(fline) >= ...)) */
	 
      else if (curr_piece != NULL        &&
               (fline[0] == LINE_ID_0 ||
                fline[0] == LINE_ID_1   )  )
      {
         add_line(curr_piece, fline, line_ctr);
      }
   } /* while ((fline = fgets...)) */
   
   if (curr_piece == NULL)
   {
      fprintf(stderr, "%s: %s, line %d: EOF before any piece definition\n",
              prg.progname, prg.inifile, line_ctr);
      exit(1);
   }
   
   if (curr_piece != NULL && curr_piece->pos_list == NULL)
   {
      fprintf(stderr, "%s: %s, line %d: EOF before definition of "
              "piece %s (No. %d)\n", prg.progname, prg.inifile, line_ctr,
              curr_piece->name, curr_piece->number);
      exit(1);
   }

   if (prg.verbose)
   {
      fprintf(stdout, "%2i. %s\n", curr_piece->number, curr_piece->name);
#ifdef DEBUG
      print_field(curr_piece->pos_list->field);
#endif
      fprintf(stdout, "\n");
   }
   
   fclose(fp);

}



/***************************************************/
/* add a new line to the first position of a piece */
/***************************************************/

static void add_line(struct tnode *curr_piece,
		     char *fline,
		     int line_ctr)
{
   static struct tnode *same_piece=NULL;
   static int y_ctr=0;

   int i;
   int invalid_char=0;

   if (curr_piece == NULL)
   {
      fprintf(stderr, "%s: %s, line %d: Pointer to piece in add_line() is NULL",
	      prg.progname, prg.inifile, line_ctr);
      exit(1);
   }
   
   if (curr_piece != same_piece)
   {
      same_piece = curr_piece;
      y_ctr = 0;
      t_add_pos(&curr_piece, p_alloc());
   }
   
   if (++y_ctr > YDIM)
   {
      fprintf(stderr, "%s: %s, line %d: Too many definition lines for "
	      "piece %s (No. %d)\n", prg.progname, prg.inifile, line_ctr,
	      curr_piece->name, curr_piece->number);
      exit(1);
   }

   for (i=0; i < XDIM; i++)
   {
      if (fline[i] == LINE_ID_1)
      {
         f_setxy(&curr_piece->pos_list->field, i, y_ctr-1);
         curr_piece->one_count++;
      }
      else if (fline[i] != LINE_ID_0)
      {
         invalid_char = TRUE;
         break;   /* break at the first invalid character */
      }
   }
   if (!invalid_char && (fline[XDIM] == LINE_ID_0 || fline[XDIM] == LINE_ID_1))
   {
      fprintf(stderr, "%s: %s, line %d: Too long definition line for "
              "piece %s (No. %d)\n", prg.progname, prg.inifile, line_ctr,
              curr_piece->name, curr_piece->number);
      exit(1);
   }

}



/********************************************/
/* reads the tryfile into a character array */                   
/********************************************/

void read_try(void)
{
   
   FILE         *fp;
   int           i,j,l;
   char          fline[MAXLINE];
   brickT       *try;

   fp = f_r_open(prg.tryfile);

   try = b_alloc();
   
   for (i=0; i<YDIM; i++)
   {
      if ((fgets(fline, MAXLINE, fp)) == NULL)
      {
         fprintf(stderr, "%s: %s: EOF before complete field definition\n",
                 prg.progname, prg.tryfile);
         exit(1);
      }
      if ((l = strlen(fline)-1) != XDIM)
      {
         fprintf(stderr, "%s: %s: Line %i is too %s\n",
                 prg.progname, prg.tryfile, i+1, l < XDIM ? "short" : "long");
         exit(1);
      }

      /* copy the line to the brick field */

      for (j=0; j<l; j++)
      {
         b_setxy(try, j, i, fline[j] == LINE_ID_0 ? EMPTY_BRICK
                                                  : (unsigned int)fline[j]);
      }
   }

   fclose(fp);
   
   if (prg.verbose)
   {
      fprintf(stdout, "Field read from %s:\n", prg.tryfile);
      print_bfield(*try);
      fprintf(stdout, "\n");
   }

   parse_try(try);

   b_free(try);
   
}


/********************************************************/
/* Open a regular file for reading, perform some checks */
/********************************************************/

static FILE *f_r_open(const char *filename)
{
   FILE         *fp;
   struct stat   stbuf;

   if ((fp = fopen(filename, F_READ)) == NULL ||
       fstat(fileno(fp), &stbuf)  == -1         )
   {
      fprintf(stderr, "%s: %s: ", prg.progname, filename);
      perror(NULL);
      exit(1);
   }
   if (S_ISDIR(stbuf.st_mode))
   {
      fprintf(stderr, "%s: %s is a directory\n", prg.progname, filename);
      exit(1);
   }

   return fp;
}
