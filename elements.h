#ifndef ELEMENTS_H_INCLUDED
#define ELEMENTS_H_INCLUDED

#include <unordered_set>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

typedef unsigned int UINT;

typedef enum{unset, t, f} colInfo;

template <typename T> void printVector(std::vector<T> v) {
  for (UINT i = 0; i < v.size(); i++) {
    std::cout << i << " - " << v[i] << std::endl;
  }
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

char getSep(std::string t){
  std::vector<char> p = {',', ';', '\t', ' '};
  char result = p[0];
  UINT n = 0;
  for (UINT i = 0; i < p.size(); i++){
    UINT tn = countChar(t, p[i]);
    if (tn > n){
      result = p[i];
      n = tn;
    }
  }
  return result;
}


typedef struct nvset{
  std::string setName;
  std::unordered_set<std::string> setElements;
} vset;

class nvenn{
  std::vector<std::vector<std::string>> cells;
  std::vector<vset> sets;
  std::stringstream warnings;


  void transpose(){
    std::vector<std::vector<std::string>> tcells;
    for (UINT i = 0; i < cells[0].size(); i++){
      tcells.push_back({});
      tcells[i].clear();
      for (UINT j = 0; j < cells.size(); j++){
        if (i < cells[j].size()){
          tcells[i].push_back(cells[j][i]);
        }
      }
    }
    cells = tcells;
  }

public:
  nvenn(){}
  nvenn(std::string desc, const char sep = 0x00, colInfo byCol = unset){
    addInfo(desc, sep, byCol);
  }
  void addInfo(std::string desc, const char sep = 0x00, colInfo byCol = unset){
    desc = exchangeChar(desc, '\r', 0x00);
    char separator = sep;
    if (sep == 0x00){
      separator = getSep(desc);
    }
    warnings << "Separator: " << (UINT) separator << std::endl;
    std::vector<std::string> lines = split(desc, '\n');
    for (UINT i = 0; i < lines.size(); i++){
      cells.push_back({});
      cells[i].clear();
      std::vector<std::string> cols = split(lines[i], separator);
      for (UINT j = 0; j < cols.size(); j++){
        cells[i].push_back(cols[j]);
      }
    }
    if (byCol == f){
      transpose();
    }
    for (UINT i = 0; i < cells.size(); i++){
      vset st;
      st.setName = cells[i][0];
      for (UINT j = 1; j < cells[i].size(); j++){
        if (cells[i][j] != ""){
          st.setElements.insert(cells[i][j]);
        }
      }
      sets.push_back(st);
    }
  }
  void showSets(){
    for (UINT i = 0; i < sets.size(); i++){
      std::cout << "Set " << sets[i].setName << ": ";
      for (const std::string& el : sets[i].setElements){
        std::cout << el << ", ";
      }
      std::cout << std::endl;
    }
  }
  std::string getInfo(){
    return warnings.str();
  }
  void showCells(){
    for (UINT i = 0; i < cells.size(); i++){
      std::cout << "Row " << i + 1 << std::endl;
      for (UINT j = 0; j < cells[i].size(); j++){
        std::cout << "\tColumn " << j + 1 << ": " << cells[i][j] << std::endl;
      }
    }
  }
};

#endif // ELEMENTS_H_INCLUDED
