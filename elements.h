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


/**< Functions and objects to input sets and interrogate regions */

typedef unsigned int UINT;

/** \brief t for true, f for false, unset for autodetect
 */
typedef enum{unset, t, f} colInfo;


/** \brief Heuristically predict which separator is being used in a table
 *
 * \param t std::string Input table
 * \return char Which of the most common characters is being used as a column separator
 *
 */
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


/** \brief A set with a name
 */
typedef struct nvset{
  std::string setName;
  std::unordered_set<std::string> setElements;
} vset;

/** \brief Input sets and interrogate regions
 */
class nvenn{
  std::vector<std::vector<std::string>> cells;
  std::vector<std::vector<std::string>> tcells;
  std::vector<std::vector<std::string>> activeCells;
  std::vector<vset> sets;
  std::unordered_set<std::string> setNames;
  std::vector<std::vector<std::string>> regions;
  std::stringstream warnings;
  bool upToDate = false;


  /** \brief Generate tcells from cells
   *
   * \return void
   *
   */
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

  /** \brief Intersection of two sets
   *
   * \param s1 std::unordered_set<std::string>
   * \param s2 std::unordered_set<std::string>
   * \return std::unordered_set<std::string>
   *
   */
  std::unordered_set<std::string> intersection(std::unordered_set<std::string> s1, std::unordered_set<std::string> s2){
    std::unordered_set<std::string> result;
    for (std::string o : s1){
      if (s2.count(o) > 0){
        result.insert(o);
      }
    }
    return result;
  }

  /** \brief Which elements belong to the first set and not to the second
   *
   * \param s1 std::unordered_set<std::string>
   * \param s2 std::unordered_set<std::string>
   * \return std::unordered_set<std::string>
   *
   */
  std::unordered_set<std::string> setDiff(std::unordered_set<std::string> s1, std::unordered_set<std::string> s2){
    std::unordered_set<std::string> result;
    for (std::string o : s1){
      if (s2.count(o) == 0){
        result.insert(o);
      }
    }
    return result;
  }

  /** \brief Lazily create nVenn code
   *
   * \return void
   *
   */
  void update(){
    regions.clear();
    UINT nreg = 1 << sets.size();
    for (UINT i = 0; i < nreg; i++){
      std::vector<std::string> els = getRegion(i);
      regions.push_back(els);
    }
    upToDate = true;
  }

  /** \brief Heuristically decide whether sets are in rows or columns
   *
   * \return colInfo
   *
   */
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
    for (UINT i = 0; i < tcells[0].size(); i++){
      if (tcells[0][i] == ""){
        canBeByCol = false;
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
      UINT ncols = tcells[0].size();
      result = (nrows > ncols) ? f : t;
    }
    return result;
  }

public:
  nvenn(){}

  nvenn(std::string desc, const char sep = 0x00, colInfo byCol = unset){
    addInfo(desc, sep, byCol);
  }

  /** \brief Add set with name to object
   *
   * \param setName std::string
   * \param elements std::vector<std::string>
   * \return void
   *
   */
  void addSet(std::string setName, std::vector<std::string> elements){
    upToDate = false;
    vset st;
    auto p = setNames.insert(setName);
    if (p.second){
      st.setName = setName;
      for (UINT j = 0; j < elements.size(); j++){
        if (elements[j] != ""){
          st.setElements.insert(elements[j]);
        }
      }
      sets.push_back(st);
    }
    else{
      warnings << "Duplicated set name: " << setName << ". The set has not bee added" << std::endl;
    }
  }

  /** \brief Add sets from a table
   *
   * \param desc std::string Table with sets in rows or columns
   * \param const char sep = 0x00 Character that separates columns in @desc. If no character is
   * provided, it will be inferred with @getSep.
   * \param unset colInfo byCol = unset If t, sets are in columns. If f, sets are in rows. If
   * unset, it will be heuristically inferred by @decideByCol.
   * \return void
   *
   */
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
    transpose();
    activeCells = tcells;
    if (byCol == unset){
      byCol = decideByCol();
      warnings << "Bycol: " << (UINT) byCol << std::endl;
    }
    if (byCol == t){
      activeCells = cells;
    }
    for (UINT i = 0; i < activeCells.size(); i++){
      std::string sn = activeCells[i][0];
      std::vector<std::string> els;
      for (UINT j = 1; j < activeCells[i].size(); j++){
        if (activeCells[i][j] != ""){
          els.push_back(activeCells[i][j]);
          //std::cout << activeCells[i][j] << std::endl;
        }
        //std::cout << "Ended " << i << std::endl;
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
    warnings.clear();
    warnings.str("");
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
