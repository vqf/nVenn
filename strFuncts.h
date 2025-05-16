#ifndef STRFUNCTS_H_INCLUDED
#define STRFUNCTS_H_INCLUDED

#include <cstdarg>

typedef unsigned int UINT;


template <typename T> void printVector(std::vector<T> v) {
  for (UINT i = 0; i < v.size(); i++) {
    std::cout << i << " - " << v[i] << std::endl;
  }
}

/** \brief Save replacement for sprintf
 *
 * \param const char * const zcFormat
 * \param ...
 * \return const std::string
 *
 */
const std::string vformat(const char * const zcFormat, ...) {

  // initialize use of the variable argument array
  va_list vaArgs;
  va_start(vaArgs, zcFormat);

  // reliably acquire the size
  // from a copy of the variable argument array
  // and a functionally reliable call to mock the formatting
  va_list vaArgsCopy;
  va_copy(vaArgsCopy, vaArgs);
  const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
  va_end(vaArgsCopy);

  // return a formatted string without risking memory mismanagement
  // and without assuming any compiler or platform specific behavior
  std::vector<char> zc(iLen + 1);
  std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
  va_end(vaArgs);
  return std::string(zc.data(), iLen);
}



std::vector<std::string> split(std::string s, const char d) {
  std::vector<std::string> result;
  result.clear();
  UINT cpos = 0;
  UINT nxt = s.find(d) + 1;
  while (nxt > cpos && nxt <= s.size()) {
    std::string r = s.substr(cpos, nxt - cpos - 1);
    result.push_back(r);
    cpos = nxt;
    nxt = s.find(d, cpos) + 1;
  }
  if (cpos < s.size()){
    std::string r = s.substr(cpos, s.size() - nxt);
    result.push_back(r);
  }
  return result;
}

std::string cleanString(std::string input){
  std::string result = "";
  for (std::basic_string<char>::const_iterator it = input.cbegin();
       it != input.cend(); it++) {
    UINT c = *it;
    if ((c > 0x28) && (c != 0x3B) &&
        (c != 0x40) && (c != 0x60)) {
      result += *it;
    }
    else{
      result += "_";
    }
  }
  return result;
}

std::string exchangeChar(std::string input, const char from, const char to){
  std::string result = "";
  for (std::basic_string<char>::const_iterator it = input.cbegin();
       it != input.cend(); it++) {
    UINT c = *it;
    if (c == from) {
      if (to != 0x00){
        result += to;
      }
    }
    else{
      result += *it;
    }
  }
  return result;
}

/** \brief Eliminates characters that cannot belong to a number.
 *         In this version, eliminates any letter, except for e and E.
 * \param input std::string
 * \return std::string
 *
 */
std::string purgeLetters(std::string input) {
  std::string result = "";
  for (std::basic_string<char>::const_iterator it = input.cbegin();
       it != input.cend(); it++) {
    UINT c = *it;
    if ((c > 43 && c < 65) || c == 101 || c == 69) {
      result += *it;
    }
  }
  return result;
}

std::string getFile(std::string prompt, std::string errorPrompt)
{
    std::string fname;
    std::cout << prompt << std::endl;
    std::cin >> fname;
    std::ifstream isfile;
    isfile.open(fname.c_str());
    if (!isfile.is_open())
    {
        isfile.close();
        std::cout << errorPrompt << std::endl;
        fname = getFile(prompt, errorPrompt);
    }
    isfile.close();
    return fname;
}

class splitString{
  UINT counter;
  std::vector<std::string> v;
public:
  splitString(std::string input = "", const char sep = ';'){
    v = split(input, sep);
    counter = 0;
  }
  std::string next(){
    std::string result = "";
    if (finished()){
      return result;
    }
    result = v[counter];
    counter++;
    return result;
  }
  bool finished(){
    bool result = false;
    if (counter >= v.size()){
      result = true;
    }
    return result;
  }
};

template<typename T>
std::string join(std::string interm, std::vector<T> arr) {
  if (arr.size() < 1){
    return "";
  }
  std::stringstream r;
  for (UINT i = 0; i < (arr.size() - 1); i++){
    r << arr[i] << interm;
  }
  r << arr[arr.size() - 1];
  return r.str();
}

UINT countChar(std::string t, const char c){
  UINT result = 0;
  for (std::basic_string<char>::const_iterator it = t.cbegin();
       it != t.cend(); it++) {
    UINT tc = *it;
    if (tc == c){
      result++;
    }
  }
  return result;
}


#endif // STRFUNCTS_H_INCLUDED
