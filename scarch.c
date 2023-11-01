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
/* scarch.c                                     */
/* These are the functions for the scalable     */
/* architecture support.                        */
/************************************************/

#ifdef SCARCH

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "pentomino.h"
#include "data.h"
#include "setup.h"
#include "play.h"
#include "scarch.h"


/* Global process control data */

struct pcntl prctl;  


/* local function declarations */

static void cleanup (int);



/*******************************************************/
/* initialise control structures to manage multiple    */
/* processes, determine the positions of the start     */
/* piece for each process, start all processes         */
/*******************************************************/
      
void spawn_processes (struct tnode *start_piece)
{
   int i;                       /* counter */
   int cnt;                     /* number of positions */
   int scale;                   /* number of processes */
   int nproc;                   /* counter for processes */
   int npos;                    /* counter for positions */

   pid_t pid;                   /* process id */
   key_t key;                   /* ipc key */
   
   struct pnode *pos;           /* pointer to a position */
   struct pnode *my_last_pos;   /* ptr to the last pos of a process */

   /* some initialisations */

   prctl.pid = getpid ();       /* parent's process id */

   key = ftok (prg.progname, 'A'); /* key for interprocess communication */
   
   for (i=0; i < 5; i++)        /* signal handling, not very sophisticated yet */
   {
      signal (i, cleanup);
   }
   
   /* get and initialise a semaphore for the output control */
   
   prctl.semid = semget (key, 1, IPC_CREAT | 0660);
   if (prctl.semid == -1)
   {
      perror (prg.progname);
      exit (1);
   }
   
   V (prctl.semid);

#ifdef DEBUG   
   printf ("Semaphore %d created and initialised\n", prctl.semid);
#endif

   /* get, attach and initialise shared memory for a global counter */
   
   prctl.shmid = shmget (key, sizeof(cntT), IPC_CREAT | 0660);
   if (prctl.shmid == -1)
   {
      perror (prg.progname);
      cleanup (0);
      exit (1);
   }
   
   prctl.shmaddr = shmat (prctl.shmid, 0, 0);
   if (prctl.shmaddr == (char *) -1)
   {
      perror (prg.progname);
      cleanup (0);
      exit (1);
   }

   P (prctl.semid);
   *(cntT *) prctl.shmaddr = 0;
   V (prctl.semid);
   
   /* count the positions for start piece in the_game */   
   /* (we know that start_piece->pos == NULL)         */

   cnt = 0;
   
   while (NULL != (pos = find_pos (start_piece)))
   {
      cnt++;
   }
   
   scale = min (cnt, prg.scale);
      
   /* start all processes (pos is NULL) */

   my_last_pos = pos;
   pos = find_pos (start_piece);  /* pos is always 'one ahead' */
   
   for (nproc = 0; nproc < scale; nproc++)
   {
      /* set last pos which we do not have to work out */       

      gme.before_my_first_pos = my_last_pos;

      /* get number of positions for the process */

      npos  = cnt / (scale - nproc);
      cnt  -= npos;

      /* determine the last position to work out for the process */

      while (--npos > 0)
      {
         pos = find_pos (start_piece);            
      }

      my_last_pos = pos;
      pos = find_pos (start_piece);       
      gme.after_my_last_pos = pos;

      /* prevent the same output being flushed from multiple processes */

      fflush(NULL);

      /* start a child process */

      pid = fork ();

      switch (pid)
      {
      case -1:      /* fork error */
         
         perror (prg.progname);
         cleanup (0);
         exit (1);
         break;

      case 0:       /* child process */

         prctl.pid = getpid ();
         
         start_piece->pos      = gme.before_my_first_pos;
         start_piece->last_pos = gme.after_my_last_pos;

         search_tree (start_piece);

         exit (0);
         break;
         
      default:      /* parent process */

         if (prg.verbose)
         {
            fprintf (stdout, "%2d. process (pid %d) started\n",
                    nproc+1, (int) pid);

#ifdef DEBUG
            if (gme.before_my_first_pos == NULL)
               fprintf (stdout, "before_my_first_pos is NULL\n");
            else
            {
               print_field ((gme.before_my_first_pos)->field);
               fprintf (stdout, "\n");
            }
            if (gme.after_my_last_pos == NULL)
               fprintf (stdout, "after_my_last_pos is NULL\n");
            else
            {
               print_field ((gme.after_my_last_pos)->field);
               fprintf (stdout, "\n");
            }
#endif /* DEBUG */
            
         }
         break;
      }
      
   }  /* for (nproc = 0; ... ) */

   /* wait for all children to finish */
   
   while (wait (NULL) >= 0);

   cleanup (0);

}



/***************************************/
/* perform simple semaphore operations */
/***************************************/

void my_semop (int semid, short op)
{
   struct sembuf sb;

#ifdef DEBUG
   static short my_semval = 0;
   static char getstr[] = "P (acquire sem)";
   static char relstr[] = "V (release sem)";
   static char unkstr[] = "unknown semop  ";
   char *cmd;
   
   my_semval += op;

   switch (op)
   {
   case -1:
      cmd = getstr;
      break;
   case 1:
      cmd = relstr;
      break;
   default:
      cmd = unkstr;
      break;
   }
   
   fprintf (stdout, "pid %d: %s semval = %d\n",
            (int) prctl.pid, cmd, (int) my_semval); 
#endif
   
   sb.sem_num = 0;
   sb.sem_op  = op;
   sb.sem_flg = 0;

   if ((semop (semid, &sb, 1)) == -1)
   {
      perror (prg.progname);   
      cleanup (0);
      exit (1);
   }
}



/******************************/
/* cleanup - very raw version */
/******************************/

static void cleanup (int sig)
{
   if (prg.verbose && sig > 0)
   {
      fprintf (stdout, "pid: %d: Signal %d caught. Cleaning up and exiting...\n",
               prctl.pid, sig);
   }
#ifdef DEBUG
   fprintf (stdout, "Removing semaphore %d and shared memory segment %d\n",
            prctl.semid, prctl.shmid);
#endif
   semctl (prctl.semid, 0, IPC_RMID, NULL);
   shmdt  (prctl.shmaddr);
   shmctl (prctl.shmid, IPC_RMID, NULL);
   
   if (sig > 0)
   {
      exit (sig);
   }
}



#endif /* SCARCH */
