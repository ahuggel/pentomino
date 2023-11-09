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
/* setup.c                                      */
/* All functions to setup the data structures.  */
/************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "pentomino.h"
#include "data.h"
#include "play.h"
#ifdef DEBUG
#include "print.h"
#endif
#include "setup.h"


/* local function declarations */

static struct pnode *rotate      (struct pnode *);
static struct pnode *form_exists (struct pnode *, struct pnode *);


/********************************************/
/* create the position lists for all pieces */
/********************************************/

void create_pos_lists(void)
{
   struct tnode *curr_piece = NULL;
   struct pnode *rot_list   = NULL;
   struct pnode *curr_rot   = NULL;
   struct pnode *currx_pos  = NULL;
   struct pnode *curry_pos  = NULL;
   struct pnode *tmpx_pos   = NULL;
   struct pnode *tmpy_pos   = NULL;
   
   /* Ueber alle Teilchen ... */   
   for (curr_piece = gme.first_piece;
        curr_piece != NULL;
        curr_piece = curr_piece->next)
   {
      /* Schiebe die allererste Position (aus inifile) in die linke obere Ecke */
      p_shift_lu(curr_piece->pos_list);
      
      /* Erstell die Liste aller Drehungen und Spiegelungen */
      rot_list = rotate(curr_piece->pos_list);

      /* Ueber alle Drehungen und Spiegelungen ... */
      for (curr_rot = rot_list; curr_rot != NULL; curr_rot = curr_rot->next)
      {
         /* Schiebe die Position auf dem ganzen Feld herum ...*/
         currx_pos  = p_identity(curr_rot);
         while (currx_pos != NULL)
         {
            tmpx_pos   = currx_pos;
            currx_pos  = p_right(tmpx_pos);

            curry_pos  = p_identity(tmpx_pos);
            while (curry_pos != NULL)
            {
               tmpy_pos   = curry_pos;
               curry_pos  = p_down(tmpy_pos);
               
               /* Uebernimm die Position in die Liste falls der Check OK ist */
               /* und die Position nicht die allererste ist                  */
               if ( gme.f_plausible_fct(tmpy_pos->field, NULL) &&
        	   !f_cmp(tmpy_pos->field, rot_list->field)      )
               {
        	  t_add_pos(&curr_piece, tmpy_pos);
               }
               else
               {
        	  p_free(&tmpy_pos);
               }
            }

            p_free(&tmpx_pos);
            
         }
      }   /* for (curr_rot = ... ) */

      p_flush(&rot_list);
      
   }   /* for (curr_piece = ... ) */

}



/*************************************************************/
/* rotate and mirror a position, return ptr to complete list */
/*************************************************************/

static struct pnode *rotate(struct pnode *first_pos)
{
   struct pnode *first_rot    = NULL;
   struct pnode *mirrored_pos = NULL; 
   struct pnode *turned_pos   = NULL;
   int i,j;

/* Use each rotation once per mirage. All these functions */   
/* allocate memory for new positions.                     */
   
   for (i=0; *p_mirror_fct[i] != NULL; i++)
   {
      mirrored_pos = (*p_mirror_fct[i])(first_pos);

      for (j=0; *p_turn_fct[j] != NULL; j++)
      {
         turned_pos = (*p_turn_fct[j])(mirrored_pos);

         if (turned_pos != NULL)
         {
            if (form_exists(turned_pos, first_rot))
            {
               p_free(&turned_pos);
            }
            else
            {
               p_add(&first_rot, turned_pos);
            }
         }
      }
      
      p_free(&mirrored_pos);

   }
   
   return first_rot;
}



/******************************************************************/
/* check if pos exists in the list plist, return a pointer to pos */ 
/* in plist if it does or NULL otherwise.                         */
/******************************************************************/

static struct pnode *form_exists(struct pnode *pos, struct pnode *plist)
{
   struct pnode *curr_pos = NULL;
   
   for (curr_pos = plist;
        curr_pos != NULL && !f_cmp(curr_pos->field, pos->field);
        curr_pos = curr_pos->next);
        
   return curr_pos;
}


/**********************************************************************/
/* sort the pieces list by pieces with pos already set, LINE_ID_1s,   */
/* the number of positions and the order of the pieces in the inifile */ 
/**********************************************************************/

void sort_pieces(struct tnode **first_piece)
{
   struct tnode **base;  /* pointer to the basetable */
   int i;

   /* allocate the temporary basetable of pointers */

   if ((base = (struct tnode **)malloc((size_t)(gme.piece_count *
                                     sizeof(struct tnode *)))) == NULL)
   {
      fprintf(stderr, "%s: Can't allocate memory for sort-basetable\n",
              prg.progname);
      exit(1);
   }

   /* initialise the table with the pointers to our pieces */

   base[0] = *first_piece;
   
   for (i=0; i<gme.piece_count-1; i++)
   {
      if (base[i])
      {
         base[i+1] = base[i]->next;
      }
      else
      {
         fprintf(stderr, "%s: Piece list is shorter than computed\n",
                 prg.progname);
         exit(1);
      }         
   }
   if (base[i]->next != NULL)
   {
      fprintf(stderr, "%s: Piece list is longer than computed\n",
              prg.progname);
      exit(1);
   }

   /* sort the basetable */
   
   qsort((void *)base,
         (size_t)gme.piece_count,
         sizeof(struct tnode *),
         t_cmp);

   /* shuffle the piece list according to the sorted basetable */

   *first_piece = base[0];
   
   for (i=0; i<gme.piece_count-1; i++)
   {
      base[i]->next = base[i+1]; 
   }
   base[i]->next = NULL;

   /* free the temporary sort basetable */
   
   free(base);

}


/*************************************************************/
/* restrict the position list(s) to omit symmetric solutions */
/*************************************************************/

void restrict_pos_lists(void)
{
   /* Ist noch ueberhaupt nicht allgemein gehalten */

   int i,j;
   int free_it;
   struct tnode *piece;
   struct pnode **pos;

   piece = gme.first_piece;  /* This must be piece 'Kreuz' */

   /* Loesche alle Positionen des Teilchens 'Kreuz' wo der */
   /* Schwerpunkt des Teilchens ausserhalb des oberen      */
   /* linken Viertel des Feldes liegt.                     */
   
   pos = &piece->pos_list;

   while (*pos != NULL) 
   {
      free_it = FALSE;
      
      if (!gme.f_plausible_fct((*pos)->field, *pos)) {
         free_it = TRUE;
      }
      for (i=4; i<YDIM && !free_it; i++)
      {     
         for (j=0; j<XDIM && !free_it; j++)
         {
            if (f_testxy((*pos)->field, j, i))
            {
               free_it = TRUE;
            }
         }
      }
      for (i=0; i<4 && !free_it; i++)
      {     
         for (j=6; j<XDIM && !free_it; j++)
         {
            if (f_testxy((*pos)->field, j, i))
            {
               free_it = TRUE;
            }
         }
      }
      
      if (free_it)
      {
         p_free(pos);
         piece->pos_count -= 1; /* this should be t_rm_pos() or t_free_pos() */
      }
      else
      {
         pos = &(*pos)->next;
      }
   }
}


/*****************************************************************/
/* check piece sizes and field, setup the plausibility check fct */
/*****************************************************************/

void check_pieces(int *piece_sizes, PF2 *f_plausible_fct)
{
   struct tnode *curr_piece;
   struct tnode *next_piece;
   int total_ones  = 0;
   int equal_sizes = TRUE;
   
   curr_piece  = gme.first_piece;

   while(curr_piece != NULL)
   {
      /* sum up all ones to compare it with the field size later */
      
      total_ones += curr_piece->one_count;

      /* figure out if all pieces are of the same size */
         
      if ((next_piece = curr_piece->next) != NULL)
      {
         equal_sizes &= curr_piece->one_count == next_piece->one_count;
      }
      
      curr_piece = next_piece;
   }
   
   /* check whether all pieces fit exactly into the field */
   
   if (XDIM * YDIM != total_ones)
   {
      fprintf(stderr,"%s: %s: The field size is %s than all pieces together\n",
              prg.progname,
              prg.inifile,
              XDIM * YDIM > total_ones ? "larger" : "smaller");
      exit(1);
   }
   
   /* set the piece sizes variable and the plausibility check function */

   *piece_sizes     = equal_sizes ? gme.first_piece->one_count : 0;
   *f_plausible_fct = equal_sizes ? f_plausible1 : f_plausible2;

}


/************************************************************************/
/* parse the brick-field: Try to identify each single-character-pattern */
/* with a position of a piece and set it at the piece                   */
/************************************************************************/

void parse_try(brickT *bfield)
{   
   int           i,j;
   int          *multi_use;
   int           one_cnt;
   char          a, brick;
   struct fieldT tryfield;
   struct tnode *trypiece;
   struct tnode *curr_piece;
   struct pnode *trypos;
   struct pnode *temp_pos;

   if (prg.verbose)
   {
      fprintf(stdout, "Pieces used in %s:\n", prg.tryfile);
   }

   /* allocate the array to check for multiple piece definitions in tryfile */

   if (0 == (multi_use = (int *)calloc(gme.piece_count, sizeof(int))))
   {
      fprintf(stderr, "%s: Can't allocate memory for multi_use array\n",
              prg.progname);
      exit(1);
   }

   /* the pseudo position we need for the search in the pos list */
   
   temp_pos = p_alloc();

   /* filter all the pieces from the tryfile */

   do /* while (one_cnt != 0) */
   {
      one_cnt = 0;
      brick = '\0';
      f_clear(&tryfield);
      
      /* the character pattern for each distinct character      */
      /* defines a piece and its position. First, get a pattern */
      /* into the tryfield variable                             */
      
      for (i=0; i<YDIM; i++)
      {
         for (j=0; j<XDIM; j++)
         {
            switch (a = b_getxy(*bfield, j, i)) {
            case EMPTY_BRICK:
               break;
            default:
               if (brick == '\0')
               {
                  brick = a;
               }
               if (brick == a)
               {
                  b_setxy(bfield, j, i, EMPTY_BRICK);
                  f_setxy(&tryfield, j, i);
                  one_cnt++;
               }
            } /* switch... */
         } /* for (j=0... */
      } /* for (i=0... */

      /* try to find the pattern in the piece list: scan all     */
      /* positions of the pieces with a matching number of ones. */
      /* we need a pseudo pos for form_exists().                 */
      
      f_copy(&temp_pos->field, tryfield);
      
      trypiece = NULL;
      trypos   = NULL;
      for (curr_piece = gme.first_piece;
           trypiece == NULL && curr_piece != NULL;
           curr_piece = curr_piece->next          )
      {
         if (curr_piece->one_count == one_cnt)
         {
            if ((trypos = form_exists(temp_pos, curr_piece->pos_list)))
            {
               trypiece = curr_piece;
            }
         }
      }
      
      /* if we found a matching position, check whether this  */
      /* piece has not been identified yet and set the pos    */
      /* pointer at the respective piece to point there       */

      if (trypiece)
      {
         if (multi_use[trypiece->number-1])
         {
            fprintf(stderr,
                    "%s: %s: Piece %s identified a second time (%c pattern)\n",
                    prg.progname, prg.tryfile, trypiece->name, brick);
#ifdef DEBUG
            print_field(tryfield);
#endif
            exit(1);            
         }
         
         multi_use[trypiece->number-1]++;

         trypiece->pos = trypos;
         
         if (prg.verbose)
         {
            fprintf(stdout, "- %c pattern identified as piece %s\n",
                    brick, trypiece->name);
#ifdef DEBUG         
            print_field(trypiece->pos->field); 
#endif
         }
      }
      else if (one_cnt != 0)
      {
         fprintf(stderr, "%s: %s: No matching piece found for the %c pattern\n",
                 prg.progname, prg.tryfile, brick);
#ifdef DEBUG
         print_field(tryfield);
#endif
         exit(1);
      }

   } while (one_cnt != 0);

   p_free(&temp_pos);
   free(multi_use);
   
   if (prg.verbose)
   {
      fprintf(stdout, "\n");
   }
   
}
