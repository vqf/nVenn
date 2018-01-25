# nVenn
This tool creates generalized, proportional Venn and Euler diagrams. The command-line version can be directly compiled in most systems with g++. The graphical version uses OpenGL and at this time is Windows-specific. It can be compiled by adding the -DGRAPHICS option. There is also a web interface and further information [here](http://degradome.uniovi.es/cgi-bin/nVenn/nvenn.cgi).

Binaries for Windows (command-line and graphical) and Debian Linux (command-line) are located in the binaries folder. Each of them has demo file called venn.txt. This file describes the regions of the Venn diagram:
```
Line             | Meaning
-----------------|------------------------------
nVenn v1.2       | Header
4                | Number of sets
8h               | Name of set 1
8m               | Name of set 2
16h              | Name of set 3
16m              | Name of set 4
0                | 0000
80               | 0001 <- Only in set 4
60               | 0010 <- Only in set 3
5                | 0011 <- Only in sets 3 and 4
45               | 0100 <- Only in set 2
78               | 0101 ...
65               | 0110
4                | 0111
57               | 1000
34               | 1001
7                | 1010
8                | 1011
65               | 1100
35               | 1101
65               | 1110
8                | 1111 <- Present in all sets
```
Each region can be represented with a binary chain, where each position represents a set, from left to right. For each set, a 1 means that the region belongs to the set, and a 0 means that the region does not belong to the set. For instance, region 0101 belongs to sets 2 and 4.

As shown in the right column, each number represents the number of elements in a region (80 elements belong only to set 4, 60 
elements belong only to set 3, 5 elements belong only to sets 3 and 4, ...). The order of the regions is given by the chain interpreted as a number (0, 0000; 1, 0001; 2, 0010; 3, 0011;...). 

There is also a Perl script (feed_venn.pl) which reads text files with set elements and outputs the corresponding venn input text. The text files contain a first line with the name of the group and the elements of the group, one per line. You can see examples in the files ex1.txt, ex2.txt and ex3.txt. Run the example with 

```
perl feed_venn.txt ex1 ex2 ex3
```
The output contains an analysis of the regions. For instance, you can see how elements g and h belong to groups 2 and three (011). Then, starting at === nVenn input ===, the output contains the corresponding input for nVenn.

The program has been coded with Code:Blocks. Windows binaries were directly compiled with MinGW:

```
g++.exe -Wall -c -g  main.cpp -o .\.objs\main.o
g++.exe -static -static-libgcc -static-libstdc++ -o .\binaries\Win\nVenn.exe .\.objs\main.o

mingw32-c++.exe -lopengl32 -lglu32 -static-libgcc -static-libstdc++ -DGRAPHICS  -c D:\cpp\venn\1.2\m
ain.cpp -o .\.globjs\main.o
mingw32-c++.exe -o .\binaries\Win\nVennOGL.exe .globjs\main.o  -static -static-libgcc  -lopengl32 -l
glu32 -lgdi32 -luser32 -lkernel32 -mwindows
```
At this time, the OpenGL version only reads a "venn.txt" file in the same folder and outputs a "result.svg" and a "result.ps" in the same folder. We believe that this tool is mainly useful to visualize and tweak the algorithm. Press ESC to start each step of the algorithm and to finish.
