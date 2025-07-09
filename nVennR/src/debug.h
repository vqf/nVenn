#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <stdio.h>
#include <fstream>
#include <sstream>


/**< Code enclosed into DEBUGONLY() will only appear if DEBUG is defined.
Only in that case the function tolog(toString(__LINE__) + "\n" + ) will be available and executed.
 */

#define DEBUG
#undef DEBUG

#ifdef DEBUG
#define _L_ "Line " + toString(__LINE__) + " - "
#define DEBUGONLY(a) a
#else
#define DEBUGONLY(a)
#define restart_log()
#define tolog(a)
//#define cout cout << __LINE__ << endl
//#define tolog(toString(__LINE__) + "\n" + a)
#endif // DEBUG




DEBUGONLY(


void restart_log(){
  std::ofstream dlme;
  dlme.open("log.txt", std::ios_base::out);
  dlme.write("", 0);
  dlme.close();
}

void tologa(std::string t){
  std::ofstream f;
  f.open("log.txt", std::ios_base::app);
  f.write(t.c_str(), t.size());
  f.close();
}
)


#endif // DEBUG_H_INCLUDED
