![image](https://raw.githubusercontent.com/ahuggel/pentomino/main/Stuff/pentomino.jpg)

# Pentomino - A simple Puzzle

How many different solutions does this puzzle have?

This little puzzle resurfaced one day during the pandemic when we couldn't go anywhere and were looking for board games to play. I call it Pentomino and have had it for a very long time. It has always fascinated me because it is so difficult, yet whenever you finally find a solution, it seems to be a new one. Thus the question.

A long time ago, in 1995, I decided to write a C-program to find the answer, and after cracking my head for quite a while, this pentomino program eventually printed out every possible solution.

Back in 1995, the task took my home PC a solid 25 minutes to complete. Today, on a regular laptop, it needs less than 0.3 seconds! (Thinkpad E14 with an AMD Ryzen 7, quiet mode)

This repository has the old source code for the pentomino program, with some recent modifications made after unearthing it, most notably the change to using just one 8-byte unsigned long integer for a board and the colorful terminal output. It compiles fine in the Windows Subsystem for Linux running Debian on my laptop.

## Building and running the program

The ```pentomino``` program is a Unix/Linux command-line program. You need a C compiler like ```gcc```, the standard C library and header files, and the ```make``` utility to build it:

```
$ make
gcc -O3  -Wall  -DSCARCH   -c -o pentomino.o pentomino.c
gcc -O3  -Wall  -DSCARCH   -c -o print.o print.c
gcc -O3  -Wall  -DSCARCH   -c -o play.o play.c
gcc -O3  -Wall  -DSCARCH   -c -o data.o data.c
gcc -O3  -Wall  -DSCARCH   -c -o setup.o setup.c
gcc -O3  -Wall  -DSCARCH   -c -o read.o read.c
gcc -O3  -Wall  -DSCARCH   -c -o scarch.o scarch.c
gcc   pentomino.o print.o play.o data.o setup.o read.o scarch.o   -o pentomino
$ ./pentomino
./pentomino: No inifile specified
Usage: ./pentomino [-cv] [-n|-s tryfile] [-p scale] inifile
```

The mandatory _inifile_ parameter is the name of a file with the definitions of all the pieces. Use ```pentomino.ini```. The other parameters are optional:

| Option | Description | 
|  --- | --- |
| **-c** | Print colored output instead of letters |
| **-n** | Don't sort the pieces (experimental) |
| **-p** _scale_ | Spawn _scale_ processes |
| **-q** | Be more quiet |
| **-s** _tryfile_ | Read _tryfile_ and solve it (see ```Stuff/*.try```) |
| **-v** | Be more verbose |

The following command finds all solutions and writes them to the terminal in color:

```
$ ./pentomino -cv pentomino.ini
```
![image](https://raw.githubusercontent.com/ahuggel/pentomino/main/Stuff/solutions.png)

## Algorithm and design

The program uses a somewhat optimized (see below) trial-and-error approach, which is implemented as a straightforward recursive algorithm, to find all solutions. It consists of a game board and a list of pieces. Each piece has a list of all the positions where it can be placed on the board, and a pointer to its current position in that list. A game board requires just one 8-byte unsigned long integer, and individual bits are set to mark basic squares as occupied. The `go()` function sets a piece on the board and eventually removes it again. It is called 1.23 million times while the program finds all solutions, which may explain why it can be hard to find a solution manually. If you think that's a big number though, read on.

```c
/********************************************************/
/* check all combinations of the pieces on the board    */ 
/* starting with start_piece and the (preset) gameboard */
/********************************************************/
void search_tree(struct tnode *start_piece)
{
   while (start_piece->last_pos != find_pos(start_piece))
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
      while (find_pos(piece->next))
      {
         go(piece->next);
      }
   }
   
   f_rm(&the_game, piece->pos);
}

/***********************************************************/
/* find the next position of a piece, which fits and seems */
/* to make sense, set it at the piece and return it        */
/***********************************************************/
struct pnode *find_pos(struct tnode *piece)
{
   struct pnode *pos;

   do
   {
      while ((pos = t_next_pos(piece)) && !f_fits(the_game, pos));
   }
   while (pos && !gme.f_plausible_fct(the_game, pos));
   
   return pos;
}
```

## Optimizations

- The positions of the first piece, the cross, are limited to positions in the upper left quadrant of the 10x6 game board to eliminate mirrored and rotated solutions.
- The list of pieces is sorted by the number of positions a piece can be placed on the board. The idea is to use the hardest to place pieces first. If I leave the cross in the first position and invert the positions of all other pieces (```pentomino -p7 -q -n pentomino.ini```), then it takes about 20 seconds to find all solutions, compared to less than 0.3 seconds if the positions are sorted (same command, without the ```-n``` option).
- A plausibility check to determine if a game board still makes sense after adding a piece; it simply checks if the size of every not yet occupied contiguous region on the game board is a multiple of five basic squares. This one is _the_ game changer: without this optimization, the program takes almost 13 minutes to find all solutions and the `go()` function is called a mind-boggling 11.2 _billion_ times (11.2 x 10<sup>9</sup>). With this plausibility check, it is 3,000 times faster, and `go()` is called "only" 1.23 million times. Instead, the recursive fill function used for the check (see below) is then called 183 million times and accounts for 56% of the total time spent (gprof) and 73% of all instructions executed (cachegrind).
- Lastly, the program can split the workload and start multiple processes that work in parallel. It does that by distributing the positions of the first piece to different processes. As the cross has only seven positions in the upper left quadrant of the game board, the number of worker processes is limited to a maximum of seven - not enough to make use of all cores and CPUs of modern computers. A single process requires just over 1 second to find all solutions, with seven worker processes, it takes less than 0.3 seconds.

```c
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

      if (y < (YDIM)-1 && !f_testxy(*field, x, y+1))
      {
         n += f_fill(field, x, y+1);
      }
      if (x < (XDIM)-1 && !f_testxy(*field, x+1, y))
      {
         n += f_fill(field, x+1, y);
      }
      if (y > 0 && !f_testxy(*field, x, y-1))
      {
         n += f_fill(field, x, y-1);
      }
      if (x > 0 && !f_testxy(*field, x-1, y))
      {
         n += f_fill(field, x-1, y);
      }
   }

   return n;
}
```

## Limitations

The pentomino program only works for the standard 10x6 board size. Support for different game board sizes, even holes in the board and different board shapes could be added relatively easily. Support for pieces of different sizes exists but has never really been tested.

## License

Copyright (C) Andreas Huggel <ahuggel@gmx.net>

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
