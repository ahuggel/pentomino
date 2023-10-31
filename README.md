![image](https://raw.githubusercontent.com/ahuggel/pentomino/main/Stuff/pentomino.jpg)

# Pentomino - A simple Puzzle

How many different solutions does this puzzle have?

This little puzzle resurfaced one day during the pandemic when we couldn't go anywhere and were looking for board games to play. I call it Pentomino and have had it for a very long time. It has always fascinated me because it is so difficult, yet whenever you finally find a solution, it seems to be a new one. Thus the question.

A long time ago, in 1995, while I worked on my first job, I decided to write a C-program to find the answer, and after cracking my head over this challenge for quite a while, my pentomino program eventually printed out every possible solution.

Back in 1995, the task took my home PC a solid 25min to complete. Today, on my laptop, it needs less than 2s!

So, how many different solutions do you think there are?

This repository has my old source code for the pentomino program, with just a few modifications that I made after unearthing it. It compiles fine in the Windows Subsystem for Linux running Debian on my laptop.

## Building

The pentomino program is a Unix/Linux command line program. You need a C compiler (```gcc```), the standard C library and header files and the ```make``` utility to build it:

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
| **-s** _tryfile_ | Read _tryfile_ and solve it (see ```Stuff/*.try```) |
| **-v** | Be more verbose |

The following command finds and prints all solutions in color:

```
$ ./pentomino -cv pentomino.ini
```
![image](https://raw.githubusercontent.com/ahuggel/pentomino/main/Stuff/solutions.png)

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
