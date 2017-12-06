# nVenn
This tool creates generalized, proportional Venn and Euler diagrams. The command-line version can be directly compiled in most systems with g++. The graphical version uses OpenGL and at this time is Windows-specific. It can be compiled by adding the -DGRAPHICS option. There is also a web interface at http://degradome.uniovi.es/cgi-bin/nVenn/nvenn.cgi

Binaries for Windows (command-line and graphical) and Debian Linux (command-line) are located in the binaries folder. Each of them has demo file called venn.txt. This file describes the regions of the Venn diagram:

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
8                | 1111

