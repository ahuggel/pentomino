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
/* setup.h                                      */
/* Header file for setup.c                      */
/************************************************/

#ifndef SETUP_H
#define SETUP_H

/* type of the plausicheck function */
typedef int (*PF2)(struct fieldT, struct pnode *);

/* Global game info */
struct ginfo {
   struct tnode *first_piece;         /* pointer to the first piece      */
   struct pnode *before_my_first_pos; /* range of positions of the first */
   struct pnode *after_my_last_pos;   /* piece to work out per process   */
   unsigned int piece_count;          /* number of pieces                */
   int          piece_sizes;          /* -1: not set                     */
                                      /*  0: piece sizes differ          */
                                      /* >0: size, i.e. ones per piece   */
   PF2          f_plausible_fct;      /* The plausibility check function */
   unsigned int counter;              /* The number of solutions         */
};


/* Global game info */
extern struct ginfo gme;


/* public function declarations */


extern void create_pos_lists   (void);

extern void sort_pieces        (struct tnode **);

extern void restrict_pos_lists (void);

extern void check_pieces       (int *, PF2 *);

extern void parse_try          (brickT *);

#endif /* ! SETUP_H */
