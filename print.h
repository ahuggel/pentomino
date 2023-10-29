/************************************************/
/* PENTOMINO          written by Andreas Huggel */
/*                                              */
/* print.h                                      */
/* Header file for print.c                      */
/************************************************/

#ifndef PRINT_H
#define PRINT_H

#define INTERVAL        20
#define MAX_SOLUTIONS  100

#define FIELD_SET      '1'
#define FIELD_NOT_SET  '0'


/* public function declarations */

extern void display_it   (void);

extern void list_pieces  (void);

extern void print_field  (struct fieldT);
                                  
extern void prt_time     (FILE *);

extern void print_bfield (brickT);

#endif /* ! PRINT_H */
