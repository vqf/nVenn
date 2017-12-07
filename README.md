# nVenn
This tool creates generalized, proportional Venn and Euler diagrams. The command-line version can be directly compiled in most systems with g++. The graphical version uses OpenGL and at this time is Windows-specific. It can be compiled by adding the -DGRAPHICS option. There is also a web interface [here](http://degradome.uniovi.es/cgi-bin/nVenn/nvenn.cgi).

Binaries for Windows (command-line and graphical) and Debian Linux (command-line) are located in the binaries folder. Each of them has demo file called venn.txt. This file describes the regions of the Venn diagram:
```
nVenn v1.2       | Header
4                | Number of sets
8h               | Names of sets
8m               |
16h              |
16m              |
0                | 0000
80               | 0001 <- Only in set 1
60               | 0010 <- Only in set 2
5                | 0011 <- Only in sets 1 and 2
45               | 0100 <- Only in set 3
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
The program has been coded with Code:Blocks. Windows binaries were directly compiled with MinGW:

```
g++.exe -Wall -c -g  main.cpp -o .\.objs\main.o
g++.exe -static -static-libgcc -static-libstdc++ -o .\binaries\Win\nVenn.exe .\.objs\main.o

mingw32-c++.exe -lopengl32 -lglu32 -static-libgcc -static-libstdc++ -DGRAPHICS  -c D:\cpp\venn\1.2\m
ain.cpp -o .\.globjs\main.o
mingw32-c++.exe -o .\binaries\Win\nVennOGL.exe .globjs\main.o  -static -static-libgcc  -lopengl32 -l
glu32 -lgdi32 -luser32 -lkernel32 -mwindows
```
At this time, the OpenGL version only reads a "venn.txt" file in the same folder and outputs a "result.svg" and a "result.ps" in the same folder. We believe that this tool is great for tweaking the algorithm, but it has no further use.
