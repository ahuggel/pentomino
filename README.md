![image](https://raw.githubusercontent.com/ahuggel/pentomino/main/Stuff/pentomino.jpg)

# Pentomino - A simple Puzzle

How many different solutions does this puzzle have?

This little puzzle resurfaced one day during the pandemic when we couldn't go anywhere and were looking for board games to play. I call it Pentomino and have had it for a very long time. It has always fascinated me because it is so difficult, yet whenever you finally find a solution, it seems to be a new one. Thus the question.

A long time ago, in 1995, I decided to write a C-program to find the answer, and after cracking my head for quite a while, this pentomino program eventually printed out every possible solution.

Back in 1995, the task took my home PC a solid 25 minutes to complete. Today, on a regular laptop, it needs only about 0.3 seconds! (Thinkpad E14 with an AMD Ryzen 7, quiet mode)

So, how many different solutions do you think there are?

This repository has the old source code for the pentomino program, with some recent modifications made after unearthing it, most notably the change to using just one eight byte unsigned integer for a board and the colorful terminal output. It compiles fine in the Windows Subsystem for Linux running Debian on my laptop.

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

The program uses a somewhat optimized (see below) trial-and-error approach, which is implemented as a straighforward recursive algorithm, to find all solutions. It consists of a game board and a list of pieces. Each piece has a list of all the positions where it can be placed on the board, and a pointer to its current position in that list. A game board requires just one eight byte unsigned integer, and individual bits are set to mark basic squares as occupied. The go() function is called 1,277,173 times while the program finds all solutions.

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
```

## Optimizations

- The list of pieces is sorted by the number of positions a piece can be placed on the board. The idea is to use the hardest to place pieces first. 
- The positions of the first piece, the cross, are limited to positions in the upper left quadrant of the 10x6 game board to eliminate mirrored and rotated solutions.
- A plausibility check to determine if a game board still makes sense after adding a piece; it simply checks if the size of every not yet occupied contiguous region on the game board is a multiple of five basic squares.
- The program can split the workload and start multiple processes that work in parallel.

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
