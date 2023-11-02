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
/* pentomino.c                                  */
/* The main function parses the command line    */
/* and controls the program flow.               */
/************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "data.h"
#include "setup.h"
#include "read.h"
#include "print.h"
#include "play.h"
#ifdef SCARCH
#include "scarch.h"
#endif
#include "pentomino.h"

struct pinfo prg;                     /* Global program information */
struct ginfo gme;                     /* Global game information */

int main(int argc, char *argv[])
{
   int c, errflg = 0;
   extern char *optarg;                /* for getopt */
   extern int  optind, opterr, optopt; /* also for getopt */

   struct tnode *start_piece;
   
   /* Initialisations */
   
   prg.progname = argv[0];
   prg.inifile  = NULL;
   prg.tryfile  = NULL;
   prg.color    = FALSE;
   prg.verbose  = FALSE;
   prg.sort     = TRUE;
#ifdef SCARCH
   prg.scale    = 1;
#endif
   
   gme.first_piece         = NULL;
   gme.before_my_first_pos = NULL;
   gme.after_my_last_pos   = NULL;
   gme.f_plausible_fct     = NULL;
   gme.piece_count         = 0;
   gme.piece_sizes         = -1;
   gme.counter             = 0;
   
   /* handle the command line arguments */
   
   optarg = NULL;
   
   while (!errflg && (c = getopt(argc, argv, ARGS)) != -1)
   {
      switch (c)
      {
      case 'c':
         prg.color = TRUE;
         break;
      case 'n':
         prg.sort = FALSE;
         break;
#ifdef SCARCH
      case 'p':
         prg.scale = abs (atoi (optarg));
         break;
#endif
      case 's':
         prg.tryfile = optarg;
         break;
      case 'v':
         prg.verbose = TRUE;
         break;
      case ':':
         fprintf(stderr, "%s: Option -%c requires an argument\n",
                 prg.progname, optopt);
         errflg++;
         break;
      case '?':
         fprintf(stderr, "%s: Unrecognized option: -%c\n",
                 prg.progname, optopt);
         errflg++;
         break;
      default :
         errflg++;
      }
   }
   
#ifdef SCARCH
   if (prg.scale == 0)
   {
      fprintf(stderr, "%s: Scale must be greater than zero\n",
              prg.progname);
      errflg++;
   }
#endif
   if (prg.sort == FALSE && prg.tryfile != NULL)
   {
      fprintf(stderr, "%s: Only one of -n and -s is allowed\n",
              prg.progname);
      errflg++;
   }
   if (!errflg && optind != argc - 1)
   {
      fprintf(stderr, "%s: No inifile specified\n", prg.progname);
      errflg++;
   }
   if (errflg)
   {
      fprintf(stderr, USAGE_STR, prg.progname);
      exit(1);
   }

   prg.inifile = argv[optind];
      
   read_pieces(&gme.first_piece,
               &gme.piece_count );     /* read the inifile, count pieces */

   check_pieces(&gme.piece_sizes,
                &gme.f_plausible_fct); /* check and setup the plausi fct */
   
   create_pos_lists();                 /* pos lists for all pieces */

   if (prg.tryfile)
   {
      read_try();                      /* read the tryfile */
   }

   /* if we're trying to find a solution for a tryfile, then we can't restrict */
   /* the position lists, as the position of any piece in the file may be one  */
   /* one that would get removed.                                              */
   if (!prg.tryfile && gme.piece_sizes == 5) /* 5 not general enough yet */
   {
      restrict_pos_lists();            /* due to field symmetries */
   }
   
   if (prg.sort)
   {
      sort_pieces(&gme.first_piece);
   }
   
   if (prg.verbose)
   {
      fprintf(stdout, "%s order:\n", prg.sort ? "Sorted" : "Unsorted");
      list_pieces();                       /* print the pieces list */
   }
   
   if (prg.verbose)
   {
      fprintf(stdout, "Starting the search...\n");
      prt_time(stdout);
   }

   start_piece = setup_gameboard();       /* set all pieces from tryfile */

#ifdef SCARCH
   spawn_processes(start_piece);
#else
   search_tree(start_piece);              /* trial and error... */
   fprintf (stdout, "%d solutions found\n", gme.counter);
#endif
   if (prg.verbose)
   {
      prt_time (stdout);
   }

   return 0;
}



