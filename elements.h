#ifndef ELEMENTS_H_INCLUDED
#define ELEMENTS_H_INCLUDED

#include <unordered_set>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "strFuncts.h"
//#include "topol.h"


/**< Functions and objects to input sets and interrogate regions */

typedef unsigned int UINT;

/** \brief 1 for true, 2 for false, 0 for autodetect
 */
//typedef enum{unset, t, f} colInfo;


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

typedef struct rgn{
  UINT nreg;
  std::vector<std::string> elems;
} rgn;


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
  std::vector<std::string> vsetNames;
  std::vector<rgn> regions;
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

  std::string cleanName(std::string in){
    std::string result;
    result = exchangeChar(in, ';', '_');
    result = exchangeChar(result, '[', '-');
    result = exchangeChar(result, ']', '-');
    result = exchangeChar(result, '(', '-');
    result = exchangeChar(result, ')', '-');
    result = exchangeChar(result, '"', '_');
    return result;
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
      if (els.size() > 0){
        rgn r;
        r.nreg = i;
        r.elems = els;
        regions.push_back(r);
      }
    }
    upToDate = true;
  }

  /** \brief Heuristically decide whether sets are in rows or columns.
   *
   * \return UINT 1, by column; 2, by row.
   *
   */
  UINT decideByCol(){
    UINT result = 2;
    bool canBeByCol = true;
    bool canBeByRow = true;
    UINT dimy = cells.size();
    UINT dimx = 0;
    for (UINT i = 0; i < cells.size(); i++){
        if (cells[i].size() > dimx){
            dimx = cells[i].size();
        }
    }
    //byrow?
    if (cells[0].size() < dimx || cells[0][cells[0].size()-1] == ""){
        canBeByCol = false;
    }
    //bycol?
    if (tcells[0].size() < dimy || tcells[0][tcells[0].size()-1] == ""){
        canBeByRow = false;
    }
    if (canBeByCol && !canBeByRow){
      result = 1;
    }
    else if (!canBeByCol && canBeByRow){
      result = 2;
    }
    else if (canBeByCol && canBeByRow){
      UINT nrows = cells[0].size();
      UINT ncols = tcells[0].size();
      result = (nrows > ncols) ? 2 : 1;
    }
    return result;
  }

public:
  nvenn(){}

  nvenn(std::string desc, const char sep = 0x00, UINT byCol = 0){
    addInfo(desc, sep, byCol);
  }

  /** \brief Converts an unordered_set of strings into a vector of strings
   *
   * \param us std::unordered_set<std::string>
   * \return std::vector<std::string>
   *
   */
  std::vector<std::string> asVector(std::unordered_set<std::string> us){
        std::vector<std::string> r;
        for (const std::string& el : us){
            r.push_back(el);
        }
        return r;
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
    if (elements.size() > 0){
      setName = cleanName(setName);
      auto p = setNames.insert(setName);
      if (p.second){
        st.setName = setName;
        for (UINT j = 0; j < elements.size(); j++){
          std::string tel = cleanName(elements[j]);
          if (tel != ""){
            st.setElements.insert(tel);
          }
        }
        sets.push_back(st);
        vsetNames.push_back(setName);
      }
      else{
        warnings << "Duplicated set name: " << setName << ". The set has not bee added" << std::endl;
      }
    }
    else{
      warnings << "Empty set: " << setName << std::endl;
    }
  }

  /** \brief Gets the names of the sets as a string vector
   *
   * \return std::vector<std::string> Names of the sets
   *
   */
  std::vector<std::string> names(){
    return vsetNames;
  }

  /** \brief Add sets from a table
   *
   * \param desc std::string Table with sets in rows or columns
   * \param const char sep = 0x00 Character that separates columns in `desc`. If no character is
   * provided, it will be inferred with getSep().
   * \param unset UINT byCol = 0 If 1, sets are in columns. If 2, sets are in rows. If
   * 0, it will be heuristically inferred by decideByCol().
   * \return void
   *
   */
  void addInfo(std::string desc, const char sep = 0x00, UINT byCol = 0){
    upToDate = false;
    desc = exchangeChar(desc, '\r', 0x00);
    char separator = sep;
    if (sep == 0x00){
      separator = getSep(desc);
    }
    warnings << "Separator: " << (UINT) separator << std::endl;
    std::vector<std::string> lines = split(desc, '\n');
    UINT realline = 0;
    for (UINT i = 0; i < lines.size(); i++){
      std::vector<std::string> cols = split(lines[i], separator);
      if (cols.size() > 0){
        cells.push_back({});
        cells[realline].clear();
        for (UINT j = 0; j < cols.size(); j++){
          cells[realline].push_back(cols[j]);
        }
        realline++;
      }
    }
    transpose();
    activeCells = cells;
    if (byCol == 0){
      byCol = decideByCol();
      warnings << "ByRow (0 - unset, 1 - true, 2 - false): " << (UINT) byCol << std::endl;
    }
    if (byCol == 1){
      activeCells = tcells;
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

  /** \brief Get a string with the elements in a region
   *
   * \param n UINT Region. The binary representation of this integer shows the sets the region
   *          belongs to (1s) and does not belong to (0s).
   * \return std::string Elements in the region, separated with a newline
   *
   */
  std::string region(UINT n){
      std::vector<std::string> v = getRegion(n);
      std::string r = join("\n", v);
      return r;
  }

  /** \brief Get elements in a region
   *
   * \param n UINT Region. The binary representation of this integer shows the sets the region
   *          belongs to (1s) and does not belong to (0s).
   * \return std::vector<std::string> Elements belonging to the region.
   *
   */
  std::vector<std::string> getRegion(UINT n){
    UINT tmp = n;
    std::vector<std::string> snames;
    UINT i = 0;
    while (tmp > 0 && i <= sets.size()){
      if ((tmp & 1) > 0 && i < sets.size()){
        snames.push_back(sets[i].setName);
      }
      tmp = tmp >> 1;
      i++;
    }
    return getRegion(snames);
  }


  /** \brief Get elements in a region
   *
   * \param regionDesc std::vector<std::string> Names of the sets the region belongs to.
   * \return std::vector<std::string> Elements belonging to the region.
   *
   */
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
      if (r.size() > 0){
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
    }
    result.insert(result.begin(), r.begin(), r.end());
    return result;
  }

  /** \brief Shows a text representation of the sets in stdout
   *
   * \return void
   *
   */
/*  void showSets(){
    for (UINT i = 0; i < sets.size(); i++){
      std::cout << "Set " << sets[i].setName << ": ";
      for (const std::string& el : sets[i].setElements){
        std::cout << el << ", ";
      }
      std::cout << std::endl;
    }
  }
*/
  /** \brief Gets the elements in each region in a JSON-like format.
   *         The function is not guaranteed to give a valid JSON string.
   *         Semicolons, parentheses and such are eliminated.
   *
   * \return std::string JSON-like string
   *
   */
  std::string asJSON(){
    if (!upToDate){
      update();
    }
    std::vector<std::string> inner;
    for (rgn r : regions){
      inner.push_back("\"" + toString(r.nreg) + "\": " "[" + join(", ", r.elems, "\"", 80) + "]");
    }
    std::string result = "{" + join(", ", inner, "", 80) + "}";
    return result;
  }

  /** \brief Gets a string with an nVenn-formatted description of the
   *         diagram.
   *
   * \return std::string
   *
   */
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
    for (rgn r : regions){
      result << r.nreg << "\t" << r.elems.size() << std::endl;
    }
    return result.str();
  }

  /** \brief Returns a new nvenn object equivalent to the current.
   *
   * \return nvenn
   *
   */
  nvenn clone(){
    nvenn result = nvenn();
    result.cells = cells;
    result.tcells = tcells;
    result.activeCells = activeCells;
    result.sets = sets;
    result.setNames = setNames;
    result.vsetNames = vsetNames;
    result.regions = regions;
    //result.warnings = warnings;
    result.upToDate = false;
    return result;
  }

  std::string getSets(){
      fileText r(";", 80);
      for (UINT i = 0; i < sets.size(); i++){
        r.addLine("S");
        r.addLine(sets[i].setName);
        for (const std::string& el : sets[i].setElements){
          r.addLine(el);
        }
      }
      std::string result = r.getText();
      return result;
  }

  std::string getInfo(){
    return warnings.str();
    warnings.clear();
    warnings.str("");
  }
/*
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
  */
};

#endif // ELEMENTS_H_INCLUDED
