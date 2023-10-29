/************************************************/
/* PENTOMINO          written by Andreas Huggel */
/*                                              */
/* data.c                                       */
/* This module contains the definitions and     */
/* functions to manipulate the major data       */
/* structures: fields, positions and pieces.    */
/************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include "pentomino.h"
#include "data.h"



/**********************************************/
/* initialise memory for a new piece, name it */
/**********************************************/

struct tnode *t_alloc(char *tname, int tnum)
{
   struct tnode *piece = NULL;
   
   if (NULL == (piece = (struct tnode *)calloc(1, sizeof(struct tnode))) ||
       NULL == (piece->name = (char *)malloc(strlen(tname)+1))             )
   {
      fprintf(stderr, "%s: Can't allocate memory for piece %s (No. %d)\n",
	      prg.progname, tname, tnum);
      exit(1);
   }

   strcpy(piece->name, tname);
   piece->number = tnum;

   return piece;
}


/******************************************************/
/* add curr_piece to the list starting at first_piece */
/******************************************************/

void t_add(struct tnode **first_piece, struct tnode *new_piece)
{
   struct tnode *piece = NULL;
   
   if (NULL == *first_piece)
   {
      *first_piece = new_piece;
   }
   else
   {
      for (piece = *first_piece; piece->next != NULL; piece = piece->next);
      piece->next = new_piece;
   }
}


/**************************************************/
/* add a position to the position list of a piece */
/**************************************************/

void t_add_pos(struct tnode **piece, struct pnode *pos)
{
   p_add(&(*piece)->pos_list, pos);

   (*piece)->pos_count++;
}


/******************************************************/
/* compare two pieces by pos, one_count, pos_count    */  
/* and finally the piece number, which is unique      */
/* (used as qsort compare function)                   */  
/******************************************************/

int t_cmp(const void *pp_a, const void *pp_b)
{
   int cmp;

   /* first, pieces with pos already set come first */

   cmp =   ((*(struct tnode **)pp_a)->pos ? 0 : 1)
         - ((*(struct tnode **)pp_b)->pos ? 0 : 1);

   /* second, compare the number of ones (desc) */

   if (cmp == 0)
   {
      cmp =   (*(struct tnode **)pp_b)->one_count
            - (*(struct tnode **)pp_a)->one_count;
   }
   
   /* third, compare the number of positions (asc) */

   if (cmp == 0)
   {
      cmp =   (*(struct tnode **)pp_a)->pos_count
            - (*(struct tnode **)pp_b)->pos_count;
   }

   /* fourth, compare the (unique) piece number (asc) */
   
   if (cmp == 0)
   {
      cmp =   (*(struct tnode **)pp_a)->number
            - (*(struct tnode **)pp_b)->number;
   }
   
   return cmp;
}



/****************************************/
/* initialise memory for a new position */
/****************************************/

struct pnode *p_alloc(void)
{
   struct pnode *new_pos = NULL;
   
   if (NULL == (new_pos = (struct pnode *)malloc(sizeof(struct pnode))))
   {
      fprintf(stderr, "%s: Can't allocate memory for struct pnode\n", prg.progname);
      exit(1);
   }
   
   new_pos->next = NULL;
   
   f_clear(&new_pos->field);

   return new_pos;
}



/******************************************************/
/* add new position to the list starting at first_pos */
/******************************************************/

void p_add(struct pnode **first_pos, struct pnode *new_pos)
{
   struct pnode *pos = NULL;
   
   if (NULL == *first_pos)
   {
      *first_pos = new_pos;
   }
   else
   {
      for (pos = *first_pos; pos->next != NULL; pos = pos->next);
      pos->next = new_pos;
   }
}
   


/*******************************************/
/* free memory of a position out of a list */
/*******************************************/

void p_free(struct pnode **pos)
{
   struct pnode *next_pos;

   if (*pos != NULL)
   {
      next_pos = (*pos)->next;
      free(*pos);
      *pos = next_pos;
   }
}



/**********************************/
/* free memory of a position list */
/**********************************/

void p_flush(struct pnode **pos_list)
{
   if ((*pos_list)->next != NULL)
   {
      p_flush(&(*pos_list)->next);
   }
   p_free(pos_list);
}



/***************************************************************************/
/* shift a position one step up in its field, return 1 if possible, 0 else */
/***************************************************************************/
   
int p_up(struct pnode *pos)
{
   int ok=NOTOK;
   int i;
   
   if (pos->field.line[0] == 0)
   {
      ok = OK;

      for (i=0; i<YDIM-1; i++)
      {
	 pos->field.line[i] = pos->field.line[i+1];
      }
      pos->field.line[YDIM-1] = 0;
   }
   return ok;
}



/******************************************************************************/
/* shift a position one step to the left in its field, return 1 if OK, 0 else */
/******************************************************************************/
   
int p_left(struct pnode *pos)
{
   int ok=OK;
   int i;
   
   for (i=0; i<YDIM; i++)
   {
      if (f_testxy(pos->field, 0, i))
      {
	 ok = NOTOK;
	 break;
      }
   }
   if (ok)
   {
      for (i=0; i<YDIM; i++)
      {
	 pos->field.line[i] <<= 1;
      }
   }
   return ok;
}



/******************************************************************************/
/* create a new position one step down relative to pos, return NULL if not OK */
/******************************************************************************/
   
struct pnode *p_down(struct pnode *pos)
{
   struct pnode *new_pos = NULL;
   int i;

   if (pos->field.line[YDIM-1] == 0)
   {
      new_pos = p_alloc(); /* field is empty */
      
      for (i=YDIM-1; i>0; i--)
      {
	 new_pos->field.line[i] = pos->field.line[i-1];
      }
   }
   return new_pos;
}



/******************************************************************************/
/* create a new pos one step to the right rel. to pos, return NULL if not OK  */
/******************************************************************************/
   
struct pnode *p_right(struct pnode *pos)
{
   struct pnode *new_pos=NULL;
   int ok=OK;
   int i;

   for (i=0; i<YDIM; i++)
   {
      if (f_testxy(pos->field, XDIM-1, i))
      {
	 ok = NOTOK;
	 break;
      }
   }
   if (ok)
   {
      new_pos = p_alloc();
      
      for (i=0; i<YDIM; i++)
      {
	 new_pos->field.line[i] = (pos->field.line[i] >> 1);
      }
   }
   return new_pos;
}



/**********************************************************/
/* shift a position to the left upper corner of its field */ 
/**********************************************************/

void p_shift_lu(struct pnode *pos)
{
   while (p_left(pos));
   while (p_up(pos));
}



/*********************************************************/
/* the identity function allocates memory and copies pos */
/*********************************************************/
   
struct pnode *p_identity(struct pnode *pos)
{
   struct pnode *new_pos=NULL;
   
   new_pos = p_alloc();
   
   f_copy(&new_pos->field, pos->field);

   return new_pos;
}



/***********************************************************************/
/* create a new pos. by rotating the current by 90 degrees (clockwise) */
/***********************************************************************/
   
struct pnode *p_turn_90(struct pnode *pos)
{
   struct pnode *new_pos=NULL;
   int ok=OK;
   int i,j;
   int dim;
   lineT mask;
   
/* check dimensions of the position */
   if (XDIM > YDIM)
   {
      mask = ~(~0 << (XDIM-YDIM));
      for (i=0; i<YDIM; i++)
      {
	 if (pos->field.line[i] & mask)
	 {
	    ok = NOTOK;
	    break;
	 }
      }
   }
   else
   {
      for (i=XDIM; i<YDIM; i++)
      {
	 if (pos->field.line[i])
	 {
	    ok = NOTOK;
	    break;
	 }
      }
   }
   
/* rotate and move */
   if (ok)
   {
      new_pos = p_alloc();

      dim = XDIM > YDIM ? YDIM : XDIM;
      
      for (i=0; i<dim; i++)
      {
	 for (j=0; j<dim; j++) 
	 {
	    if (f_testxy(pos->field, j, i))
	    {
	       f_setxy(&new_pos->field, i, dim-j-1);
	    }	
	 }    
      }
/* move the position to the upper edge of its field */
      p_shift_lu(new_pos);
   }
   
   return new_pos;
}


   
/************************************************************/
/* create a new pos. by rotating the current by 180 degrees */
/************************************************************/
   
struct pnode *p_turn_180(struct pnode *pos)
{
   struct pnode *new_pos=NULL;
   int i,j;

   new_pos = p_alloc();

   for (i=0; i<YDIM; i++)
   {
      for (j=0; j<XDIM; j++) 
      {
	 if (f_testxy(pos->field, j, i))
	 {
	    f_setxy(&new_pos->field, XDIM-j-1, YDIM-i-1);
	 }	
      }    
   }
      
/* move the new position to the left upper edge of its field */
   p_shift_lu(new_pos);
   
   return new_pos;
}



/************************************************************/
/* create a new pos. by rotating the current by 270 degrees */
/************************************************************/
   
struct pnode *p_turn_270(struct pnode *pos)
{
   return p_turn_90(p_turn_180(pos));
}



/************************************************************/
/* create a new pos. by mirroring the current on the X-axis */
/************************************************************/
   
struct pnode *p_mirror_x(struct pnode *pos)
{
   struct pnode *new_pos=NULL;
   int i;
   
   new_pos = p_alloc();

   for (i=0; i<YDIM; i++)
   {
      new_pos->field.line[YDIM-1-i] = pos->field.line[i];
   }
   p_shift_lu(new_pos);
   
   return new_pos;
}



/*********************************************************/
/* arrays of all mirror functions and all turn functions */
/*********************************************************/

PF1 p_mirror_fct[] = { p_identity, p_mirror_x, NULL };

PF1 p_turn_fct[]   = { p_identity, p_turn_90, p_turn_180, p_turn_270, NULL };



/*****************************************************/
/* compare two fields, return TRUE if they are equal */ 
/*****************************************************/

int f_cmp(struct fieldT field1, struct fieldT field2)
{
   int i;
   
   for(i=0; i<YDIM && field1.line[i] == field2.line[i]; i++);

   return i == YDIM;
}



/****************/
/* copy a field */
/****************/

void f_copy(struct fieldT *to_field, struct fieldT from_field)
{
   *to_field = from_field;
}


/*****************/
/* clear a field */
/*****************/

void f_clear(struct fieldT *field)
{
   memset(field, 0x0, sizeof(struct fieldT));
}


/*****************************************************************/
/* set position piece->pos in the field. No checks are performed */
/*****************************************************************/

void f_set(struct fieldT *field, struct pnode *pos)
{
   int i;
   
   if (pos != NULL)
   {
      for (i=0; i<YDIM; i++)
      {
	 field->line[i] |= pos->field.line[i];
      }
   }
}


/**********************************************************************/
/* remove position piece->pos from the field. No checks are performed */
/**********************************************************************/

void f_rm(struct fieldT *field, struct pnode *pos)
{
   int i;
   
   if (pos != NULL)
   {
      for (i=0; i<YDIM; i++)
      {
	 field->line[i] &= ~pos->field.line[i];
      }
   }
}


/**************************************************/
/* check if position piece->pos fits in the field */
/**************************************************/

int f_fits(struct fieldT field, struct pnode *pos)
{
   register int i=0;

   if (pos != NULL)
   {
      for (i=0; i<YDIM && !(field.line[i] & pos->field.line[i]); i++);
   }
   
   return i == YDIM;
}


/****************************************************************/
/* recursively fill field starting at (x,y) and counting pixels */ 
/****************************************************************/

int f_fill(struct fieldT *field, int x, int y)
{
   int n=0;

   if (!f_testxy(*field, x, y))
   {
      n++;
      f_setxy(field, x, y);

      if (y+1<YDIM && !f_testxy(*field, x, y+1))
      {
	 n += f_fill(field, x, y+1);
      }
      if (x+1<XDIM && !f_testxy(*field, x+1, y))
      {
	 n += f_fill(field, x+1, y);
      }
      if (y-1>=0 && !f_testxy(*field, x, y-1))
      {
	 n += f_fill(field, x, y-1);
      }
      if (x-1>=0 && !f_testxy(*field, x-1, y))
      {
	 n += f_fill(field, x-1, y);
      }
   }

   return n;
}



/********************************************************************/
/* allocate & initialise memory for a brick field, return a pointer */
/********************************************************************/

brickT *b_alloc(void)
{
   int i,j;
   brickT *bfield;
   
   if (NULL == (bfield = (brickT *)malloc(sizeof(brickT))))
   {
      fprintf(stderr, "%s: Can't allocate memory for brickT\n", prg.progname);
      exit(1);
   }
   
   for (i=0; i<YDIM; i++)
   {
      for (j=0; j<XDIM; j++)
      {
         b_setxy(bfield, j, i, EMPTY_BRICK);
      }
   }
   
   return bfield;
}



/********************************/
/* free memory of a brick field */
/********************************/

void b_free(brickT *bfield)
{
   free(bfield);
}



/****************************************************************/
/* set position xy in a brick field (upper left corner is (0,0) */
/****************************************************************/

void b_setxy(brickT *bfield, int x, int y, unsigned int a)
{
   if (x < XDIM && y < YDIM && x >= 0 && y >= 0)
   {
      (*bfield)[y][x] = (char)a;
   }
   else
   {
      fprintf(stderr, "%s: b_setxy: Position (%i,%i) is outside the brick field.\n",
	      prg.progname, x, y);
      exit(1);
   }
}


/*****************************************************************************/
/* get character at position xy in a brick field (upper left corner is (0,0) */ 
/*****************************************************************************/

char b_getxy(brickT bfield, int x, int y)
{
   char a;
   
   if (x < XDIM && y < YDIM && x >= 0 && y >= 0)
   {
      a = bfield[y][x];
   }
   else
   {
      fprintf(stderr, "%s: b_getxy: Position (%i,%i) is outside the brick field.\n",
	      prg.progname, x, y);
      exit(1);
   }
   
   return a;
}
