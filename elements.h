#ifndef ELEMENTS_H_INCLUDED
#define ELEMENTS_H_INCLUDED

#include <unordered_set>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "strFuncts.h"
#include "topol.h"

typedef unsigned int UINT;

typedef enum{unset, t, f} colInfo;


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
  std::vector<std::vector<std::string>> tcells;
  std::vector<std::vector<std::string>> activeCells;
  std::vector<vset> sets;
  std::vector<std::vector<std::string>> regions;
  std::stringstream warnings;
  bool upToDate = false;


  void transpose(){
    for (UINT i = 0; i < cells[0].size(); i++){
      upToDate = false;
      tcells.push_back({});
      tcells[i].clear();
      for (UINT j = 0; j < cells.size(); j++){
        if (i < cells[j].size()){
          tcells[i].push_back(cells[j][i]);
        }
      }
    }
  }

  std::unordered_set<std::string> intersection(std::unordered_set<std::string> s1, std::unordered_set<std::string> s2){
    std::unordered_set<std::string> result;
    for (std::string o : s1){
      if (s2.count(o) > 0){
        result.insert(o);
      }
    }
    return result;
  }

  std::unordered_set<std::string> setDiff(std::unordered_set<std::string> s1, std::unordered_set<std::string> s2){
    std::unordered_set<std::string> result;
    for (std::string o : s1){
      if (s2.count(o) == 0){
        result.insert(o);
      }
    }
    return result;
  }

  void update(){
    regions.clear();
    UINT nreg = 1 << sets.size();
    for (UINT i = 0; i < nreg; i++){
      std::vector<std::string> els = getRegion(i);
      regions.push_back(els);
    }
    upToDate = true;
  }

  colInfo decideByCol(){
    colInfo result = f;
    bool canBeByCol = true;
    bool canBeByRow = true;
    //byrow?
    for (UINT i = 0; i < cells[0].size(); i++){
      if (cells[0][i] == ""){
        canBeByRow = false;
      }
    }
    //bycol?
    for (UINT i = 0; i < cells.size(); i++){
      if (cells[i][0] == ""){
        canBeByRow = false;
      }
    }
    if (canBeByCol && !canBeByRow){
      result = t;
    }
    else if (!canBeByCol && canBeByRow){
      result = f;
    }
    else if (canBeByCol && canBeByRow){
      UINT nrows = cells[0].size();
      UINT ncols = cells.size();
      result = (nrows > ncols) ? t : f;
    }
    return result;
  }

public:
  nvenn(){}

  nvenn(std::string desc, const char sep = 0x00, colInfo byCol = unset){
    addInfo(desc, sep, byCol);
  }
  void addSet(std::string setName, std::vector<std::string> elements){
    upToDate = false;
    vset st;
    st.setName = setName;
    for (UINT j = 0; j < elements.size(); j++){
      if (elements[j] != ""){
        st.setElements.insert(elements[j]);
      }
    }
    sets.push_back(st);
  }
  void addInfo(std::string desc, const char sep = 0x00, colInfo byCol = unset){
    upToDate = false;
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
    if (byCol == unset){
      byCol = decideByCol();
    }
    transpose();
    activeCells = tcells;
    if (byCol == f){
      activeCells = cells;
    }
    for (UINT i = 0; i < activeCells.size(); i++){
      std::string sn = activeCells[i][0];
      std::vector<std::string> els;
      for (UINT j = 1; j < activeCells[i].size(); j++){
        if (activeCells[i][j] != ""){
          els.push_back(activeCells[i][j]);
        }
      }
      addSet(sn, els);
    }
  }

  std::vector<std::string> getRegion(UINT n){
    UINT tmp = n;
    std::vector<std::string> snames;
    UINT i = 0;
    while (tmp > 0 && i < sets.size()){
      if ((tmp & 1) > 0){
        snames.push_back(sets[i].setName);
      }
      tmp = tmp >> 1;
      i++;
    }
    return getRegion(snames);
  }

  std::vector<std::string> getRegion(std::vector<std::string> regionDesc){
    std::vector<std::string> result;
    std::unordered_set<std::string> r;
    for (vset v : sets){
      bool included = false;
      for (std::string setName : regionDesc){
        if (v.setName == setName){
          included = true;
        }
      }
      if (included){
        for (std::string el : v.setElements){
          r.insert(el);
        }
      }
    }
    for (vset v : sets){
      bool included = false;
      for (std::string setName : regionDesc){
        if (v.setName == setName){
          included = true;
        }
      }
      if (included){
        r = intersection(r, v.setElements);
        //result.insert(result.begin(), r.begin(), r.end());
        //printVector(result); exit(0);
      }
      else{
        r = setDiff(r, v.setElements);
      }
    }
    result.insert(result.begin(), r.begin(), r.end());
    return result;
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

  std::string getCode(){
    std::stringstream result;
    result << "nVenn_2.0" << std::endl;
    result << sets.size() << std::endl;
    for (vset s : sets){
      result << s.setName << std::endl;
    }
    if (!upToDate){
      update();
    }
    for (std::vector<std::string> r : regions){
      result << r.size() << std::endl;
    }
    return result.str();
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
  void showTCells(){
    for (UINT i = 0; i < tcells.size(); i++){
      std::cout << "Row " << i + 1 << std::endl;
      for (UINT j = 0; j < tcells[i].size(); j++){
        std::cout << "\tColumn " << j + 1 << ": " << tcells[i][j] << std::endl;
      }
    }
  }
};

#endif // ELEMENTS_H_INCLUDED
