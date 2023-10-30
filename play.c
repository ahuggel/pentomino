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
/* play.c                                       */
/* These are the functions to play the game,    */
/* i.e., search the tree for solutions.         */
/************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "pentomino.h"
#include "data.h"
#include "setup.h"
#include "print.h"
#include "play.h"


/* local variables */

static struct fieldT the_game;


/* local function declarations */

void          go(struct tnode *);



/*****************************************************/
/* set all pieces from the tryfile on the gameboard, */
/* return the first piece which is not used yet      */
/*****************************************************/

struct tnode *setup_gameboard(void)
{
   struct tnode *curr_piece;

   f_clear(&the_game);
   
   for (curr_piece  = gme.first_piece;
        curr_piece != NULL && curr_piece->pos != NULL;
        curr_piece  = curr_piece->next)
   {
      f_set(&the_game, curr_piece->pos);
   }

   if (!gme.f_plausible_fct(the_game, NULL))
   {
      fprintf(stderr, "%s: %s: This try fails the plausibility check\n",
              prg.progname, prg.tryfile);
      exit(1);
   }
       
   return curr_piece;
}


/********************************************************/
/* check all orderings of the pieces on the field       */ 
/* starting with start_piece and the (preset) gameboard */
/********************************************************/

void search_tree(struct tnode *start_piece)
{
   while( start_piece->last_pos != find_pos(start_piece) )
   {
      go(start_piece);
   }
}


/***********************************************************/
/* go does the trial and error job recursively. The number */
/* of branches at each node depends on the field, find_pos */
/* gets them all.                                          */
/***********************************************************/

void go(struct tnode *piece)
{
   f_set(&the_game, piece->pos);
   
   if (piece->next == NULL)
   {

      /* This is a solution */

      display_it();
      
   }
   else
   {
      while( find_pos(piece->next) )
      {
         go(piece->next);
      }
   }
   
   f_rm(&the_game, piece->pos);
}


/**************************************************************/
/* return the next position of a piece which fits in the_game */
/* and seems to make sense                                    */
/**************************************************************/

struct pnode *find_pos(struct tnode *piece)
{
   struct pnode *pos;

   do
   {
      while ( (pos = t_get_pos(piece)) && !f_fits(the_game, pos) );
   }
   while ( pos && !gme.f_plausible_fct(the_game, pos) );
   
   return pos;
}


/********************************************************************/
/* Plausibility check whether it makes sense to set piece in field. */
/* If pos is NULL, the field alone is checked.                      */
/* This is the function for equally sized pieces.                   */
/********************************************************************/

int f_plausible1(struct fieldT field, struct pnode *pos)
{
   int i,j;
   int ok=OK;
   struct fieldT test_field=field;

   f_set(&test_field, pos);

   for (i=0; i<YDIM && ok; i++)
   {     
      for (j=0; j<XDIM && ok; j++)
      {
         ok = (f_fill(&test_field, j, i) % gme.piece_sizes == 0);
      }
   }
   
   f_rm(&test_field, pos);
   
   return ok;
}


/********************************************************************/
/* Plausibility check whether it makes sense to set piece in field. */
/* If pos is NULL, the field alone is checked.                      */
/* This is the function for pieces of different sizes.              */
/********************************************************************/

int f_plausible2(struct fieldT field, struct pnode *pos)
{
   int ok=OK;

   /* check to be implemented */
   
   return ok;
}
