#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <stdio.h>
#include <fstream>
#include "debug.h"

/**< Code enclosed into DEBUGONLY() will only appear if DEBUG is defined.
Only in that case the function tolog(toString(__LINE__) + "\n" + ) will be available and executed.
 */

#define DEBUG
//#undef DEBUG

#ifdef DEBUG
#define _L_ "Line " + toString(__LINE__) + " - "
#define DEBUGONLY(a) a
#else
#define DEBUGONLY(a)
#define tolog(toString(__LINE__) + "\n" + a)
#endif // DEBUG


using namespace std;


DEBUGONLY(


void tolog(string t){
  ofstream f;
  f.open("log.txt", std::ios_base::app);
  f.write(t.c_str(), t.size());
  f.close();
}
)
template<typename T>
string toString(T input)
{
    ostringstream result;
    result << input;
    return result.str();
}


#endif // DEBUG_H_INCLUDED
