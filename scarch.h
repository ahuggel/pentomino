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
/* scarch.h                                     */
/* Header file for scarch.c                     */
/************************************************/

#ifdef SCARCH

#ifndef SCARCH_H
#define SCARCH_H



/* global counter type */
typedef int cntT;



/* process control data structure */
struct pcntl
{
   int           nproc;   /* process number (0: parent)           */
   pid_t         pid;     /* process identifier                   */
   int           semid;   /* semaphore identifier                 */
   int           shmid;   /* shared memory region identifier      */
   char         *shmaddr; /* shared memory for the global counter */
};



/* public function declarations */

extern void          spawn_processes (struct tnode *);

extern void          my_semop        (int, short);



/* public variables */

/* global process control info */
extern struct pcntl prctl;



/* macros */

/***********************************/
/* acquire and release a semaphore */
/***********************************/

/* Arguments   : int semaphore identifier */
/* Return value: void                     */

#define P(semid)    (my_semop ((semid), -1))
#define V(semid)    (my_semop ((semid),  1))



#endif /* ! SCARCH_H */

#endif /* SCARCH */
