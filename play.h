/************************************************/
/* PENTOMINO          written by Andreas Huggel */
/*                                              */
/* play.h                                       */
/* Header file for play.c                       */
/************************************************/

#ifndef PLAY_H
#define PLAY_H

extern struct tnode *setup_gameboard (void);

extern void          search_tree     (struct tnode *);

extern struct pnode *find_pos        (struct tnode *);

extern int           f_plausible1    (struct fieldT, struct pnode *);

extern int           f_plausible2    (struct fieldT, struct pnode *);

#endif /* ! PLAY_H */
