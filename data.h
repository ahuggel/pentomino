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
/* data.h                                       */
/* Header file for data.c                       */
/************************************************/

#ifndef DATA_H
#define DATA_H

/* Dimensionen des Spielfelds */
#define XDIM       10
#define YDIM        6

/* Eine Spielfeld Zeile */
typedef unsigned long lineT;

/* Ein Spielfeld */
struct fieldT {
   lineT line[YDIM];
};

/* brick field type: a character array */
typedef char brickT[YDIM][XDIM+1];

#define EMPTY_BRICK '.'

/* Eine Position */ 
struct pnode {
   struct pnode *next;
   struct fieldT field;
};

/* Ein Teilchen */
struct tnode {
   struct tnode *next;
   char *name;
   int number;
   int pos_count;
   int one_count;
   struct pnode *pos;       /* ptr to current pos in pos_list */
   struct pnode *last_pos;  /* ptr to last pos to work out    */
   struct pnode *pos_list;  /* list of all positions          */
};



/* public function declarations */


extern struct tnode *t_alloc      (char *, int);
                                  
extern void          t_add        (struct tnode **, struct tnode *);
                                  
extern void          t_add_pos    (struct tnode **, struct pnode *);
                                  
extern int           t_cmp        (const void *, const void *);

                                  
extern struct pnode *p_alloc      (void);
                                  
extern void          p_add        (struct pnode **, struct pnode *);
                                  
extern void          p_free       (struct pnode **);
                                  
extern void          p_flush      (struct pnode **);
                                  
extern int           p_up         (struct pnode *);
                                  
extern int           p_left       (struct pnode *);
                                  
extern struct pnode *p_down       (struct pnode *);
                                  
extern struct pnode *p_right      (struct pnode *);
                                  
extern void          p_shift_lu   (struct pnode *);
                                  
extern struct pnode *p_identity   (struct pnode *);
                                  
extern struct pnode *p_turn_90    (struct pnode *);
                                  
extern struct pnode *p_turn_180   (struct pnode *);
                                  
extern struct pnode *p_turn_270   (struct pnode *);
                                  
extern struct pnode *p_mirror_x   (struct pnode *);
                                  

extern int           f_cmp        (struct fieldT, struct fieldT);
                                  
extern void          f_copy       (struct fieldT *, struct fieldT);
                                  
extern void          f_clear      (struct fieldT *);

extern void          f_set        (struct fieldT *, struct pnode *);
                                  
extern void          f_rm         (struct fieldT *, struct pnode *);
                                  
extern int           f_fits       (struct fieldT, struct pnode *);
                                  
extern int           f_fill       (struct fieldT *, int, int);


extern brickT       *b_alloc      (void);

extern void          b_free       (brickT *);

extern void          b_setxy      (brickT *, int, int, unsigned int);

extern char          b_getxy      (brickT, int, int);


/* public variables */

typedef struct pnode *(*PF1)(struct pnode *);

extern PF1 p_mirror_fct[];    /* Functions to mirror a position  */

extern PF1 p_turn_fct[];      /* Functions to rotate a position  */

/* macros */

/***************************************/
/* get the next position for the piece */
/***************************************/

/* Arguments   : struct tnode *        */
/* Return value: struct pnode *        */

#define t_get_pos(piece) (((piece)->pos==NULL) ? ((piece)->pos=(piece)->pos_list) \
                                               : ((piece)->pos=(piece)->pos->next) )



/***********************************************************/
/* test position xy in a field (upper left corner is (0,0) */
/***********************************************************/

/* Arguments   : struct fieldT field, int x, int y         */
/* Return value: int                                       */

#define f_testxy(field, x, y)  (((field).line[(y)] >> (XDIM-1-(x))) & 1)



/**********************************************************/
/* set position xy in a field (upper left corner is (0,0) */
/**********************************************************/

/* Arguments   : struct fieldT *field, int x, int y       */
/* Return value: void                                     */

#define f_setxy(field, x, y)  ((field)->line[(y)] |= (1 << (XDIM-1-(x))))

#endif /* ! DATA_H */
