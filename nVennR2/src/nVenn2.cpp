
#include <Rcpp.h>
#include <filesystem>
#include "topol.h"
using namespace Rcpp;



void nvSimulate(borderLine& bl, bool verbose){
  UINT step = 1;
  UINT maxStep = 8;
  bool goon = bl.setStep(step);
  while (step < maxStep && goon){
    bl.setCycle(step);
    if (bl.err()){
      Rcout << bl.errorMsg() << std::endl;
      step = maxStep;
    }
    if (bl.isStepFinished(step)){
      if (verbose){
        Rcout << "Step " << step << " finished." << std::endl;
      }
      step++;
      goon = bl.setStep(step);
    }
  }
}

SEXP toRObject(std::string desc, float opacity = 0.4, 
               UINT fontSize = 12, UINT lineWidth = 1, 
               UINT palette = 0,
               bool showRegions = true,
               bool showWeights = true){
  Function asNamespace("asNamespace");
  Environment nv_env = asNamespace("nVennR2");
  Function g = nv_env[".setAsObject"];
  Function h = nv_env[".optData"];
  SEXP opts = h(opacity, fontSize, lineWidth, palette, showRegions, showWeights);
  SEXP r = g(desc, opts);
  return(r);
}

//' Creates nVenn plot
//'
//' @param desc Description of sets, either as a file path, a list of lists, text or a 
//' previously created nVenn object (see Details). 
//' @param plot If true (default), the resulting diagram is plotted. If false, 
//' only the object is returned.
//' @param outFile If it contains a valid file path and `plot` is also true, 
//' the svg code of the plot will 
//' be saved in that path.
//' @param systemShow If true, and `plot` is true, the function will attempt to 
//' open the resulting 
//' svg figure in the default editor. Defaults to false. 
//' @param verbose If true, shows messages as the nVenn plot is created.
//' @param byCol If the input is a text, this parameter indicates whether 
//' each set is a column (1) or a row (2). Defaults to 0, which means that 
//' the package will try to guess which possibility makes more sense.
//' @return nVenn object. As a side effect, shows the nVenn plot.
//' @details A list of lists contains inner lists with a name, which will be 
//' the corresponding set name. A dataframe can be used in the same way.
//' 
//' The input can also be a text containing a table, possibly with missing 
//' values. If a text is provided, the package will try to guess if each set
//' is encoded in columns or rows (use `byCol` to force) and which
//' character separates fields (usually tab, space or comma). If the text
//' describes a valid text file path, the contents of the file will be used.
//' @examples
//' myv <- nVennDiagram(list(Set1=c("a", "b", "c"), Set2=c("a", "c", "d")), verbose=F)
// [[Rcpp::export]]
SEXP nVennDiagram(SEXP desc, bool plot = true, std::string outFile="", bool systemShow=false,
                    bool verbose = true, unsigned int byCol = 0){
  List sv = desc;
  bool correct = true;
  borderLine bl;
  std::string result;
  Function asNamespace("asNamespace");
  Environment nv_env = asNamespace("nVennR2");
  if (sv.containsElementNamed("desc")){
    bl.restoreBl(as<std::string>(sv["desc"]));
  }
  else{
    std::string dsc;
    if (sv.size() > 0){
      StringVector s1 = as<StringVector>(sv[0]);
      if (sv.size() == 1 && s1.size() == 1){ //Text
        dsc = as<std::string>(s1[0]);
        if (std::filesystem::exists(dsc)){
          dsc = getFileText(dsc);
        }
      }
      else{
        Function f = nv_env[".lol2string"];
        dsc = as<std::string>(f(desc));
      }
    }
    bl = borderLine(dsc, byCol);
    if (bl.err()){
      Rcout << bl.errorMsg() << std::endl;
      Rcout << "If you are trying to use a file, please make sure that the path exists" << std::endl;
      correct = false;
      return R_NilValue;
    }
  }
  if (correct){
    nvSimulate(bl, verbose);
    if (bl.err()){
      Rcout << bl.errorMsg() << std::endl;
      return R_NilValue;
    }
    else{
      result = bl.saveBl();
    }
  }
  SEXP r = toRObject(result);
  Function n = nv_env[".setSetNames"];
  std::vector<std::string> sn = bl.getSetNames();
  r = n(r, sn);
  if (plot){
    Function f = nv_env["plotVenn"];
    f(r, outFile, systemShow);
  }
  return r;
}

//' Creates nVenn plot
//'
//' @param svgFile File name of an SVG or HTML file created by nVenn. 
//' @param plot If true (default), the resulting diagram is plotted. If false, 
//' only the object is returned.
//' @param outFile If it contains a valid file path and `plot` is also true, 
//' the svg code of the plot will 
//' be saved in that path.
//' @param systemShow If true, and `plot` is true, the function will attempt to 
//' open the resulting 
//' svg figure in the default editor. Defaults to false. 
//' @returns nVenn object.
//' @details In principle, this function should work with any SVG or HTML file
//' created by nVenn, with either nVennR2, a web interface or nVennPy.
// [[Rcpp::export]]
SEXP readVennSVG(std::string svgFile, bool plot = true, std::string outFile="", bool systemShow=false){
  Function asNamespace("asNamespace");
  Environment nv_env = asNamespace("nVennR2");
  borderLine bl;
  bl.restoreFromFile(svgFile);
  std::string result = bl.saveBl();
  SEXP r = toRObject(result);
  Function n = nv_env[".setSetNames"];
  std::vector<std::string> sn = bl.getSetNames();
  r = n(r, sn);
  if (plot){
    Function f = nv_env["plotVenn"];
    f(r, outFile, systemShow);
  }
  return r;
}


//' Gets the names of the sets
//'
//' @param nVennObj nVennR2 object generated with [nVennDiagram()].
//' @return List of set names.
//' @examples
//' myv <- nVennDiagram(list(Set1=c("a", "b", "c"), Set2=c("a", "c", "d")), verbose=F)
//' getVennSetNames(myv)
// [[Rcpp::export]]
List getVennSetNames(List nVennObj){
  borderLine bl;
  bl.restoreBl(as<std::string>(nVennObj["desc"]));
  std::vector<std::string> r = bl.getSetNames();
  List result;
  for (unsigned int i = 0; i < r.size(); i++){
    result.push_back(r[i]);
  }
  return result;
}

//' Gets a list of the elements in a region of the diagram
//'
//' @param nVennObj nVennR2 object generated with [nVennDiagram()].
//' @param n Region, either as an integer or as a vector of set names. See Details
//' @return List of set names.
//' @details An integer expresses a region by considering its binary representation
//' in reverse. A 1 in a position means "belongs to" and a 0 or empty position
//' means "does not belong to". Thus, the binary representation of 19  (10011),
//' read from right to left, means "region that belongs to
//' sets 1, 2 and 5 and does not belong to any other set".
//' 
//' A vector of set names expresses a region by giving the sets the region
//' belongs to. It is understood that the region does not belong to any other
//' set.
//' @examples
//' myv <- nVennDiagram(list(Set1=c("a", "b", "c"), Set2=c("a", "c", "d")), verbose=FALSE)
//' #Both commands are equivalent:
//' getVennRegion(myv, 3)
//' getVennRegion(myv, c("Set1", "Set2"))
// [[Rcpp::export]]
List getVennRegion(List nVennObj, SEXP n) {
  borderLine bl;
  bl.restoreBl(as<std::string>(nVennObj["desc"]));
  List result;
  if (TYPEOF(n) == STRSXP){
    StringVector sv = as<StringVector>(n);
    std::vector<std::string> v(sv.size()); 
    for (unsigned int i = 0; i < sv.size(); i++){
      v[i] = as<std::string>(sv(i));
    }
    result = bl.getVennRegionVectorL(v);
  }
  else if ((TYPEOF(n) == INTSXP) || (TYPEOF(n) == REALSXP)){
    IntegerVector iv = as<IntegerVector>(n);
    unsigned int nreg = bl.nregions();
    unsigned int v = iv(0);
    if (v <= nreg){
      std::vector<std::string> rv = bl.getVennRegionVector(v);
      if (rv.size() > 0){
        for (unsigned int i = 0; i < rv.size(); i++){
          result.push_back(rv[i]);
        }
      }
    }
  }
  return result;
}


//' Lists the elements in every region of the diagram
//'
//' @param nVennObj nVennR2 object generated with [nVennDiagram()].
//' @param showEmpty If true, lists every region, even if empty. 
//' If false (default), only lists regions containing elements.
//' @return Nothing. The result is printed.
//' @examples
//' myv <- nVennDiagram(list(Set1=c("a", "b", "c"), Set2=c("a", "c", "d")), verbose=F)
//' listVennRegions(myv)
// [[Rcpp::export]]
void listVennRegions(List nVennObj, bool showEmpty = false) {
  borderLine bl;
  bl.restoreBl(as<std::string>(nVennObj["desc"]));
  std::vector<std::string> sets = bl.getSetNames();
  unsigned int tnreg = 1 << sets.size();
  for (unsigned int v = 0; v < tnreg; v++){
    std::vector<std::string> rv = bl.getVennRegionVector(v);
    if (rv.size() > 0 || showEmpty){
      std::string rdesc = bl.regionDescription(v);
      Rcout << "Region " << v << " " << rdesc << ":" << std::endl;
      
      for (unsigned int i = 0; i < rv.size(); i++){
        Rcout << "\t" << rv[i] << std::endl;
      }
    }
  }
  return;
}

//' Get the svg code of an nVenn diagram
//'
//' @param nVennObj nVennR2 object generated with [nVennDiagram()].
//' @return String with svg code.
//' @details
//' The code returned by the funciton can be saved to a file and then 
//' edited with vectorial image 
//' software. This can be done directly with [nVennDiagram()] or
//' [plotVenn()] by using the `outFile` param.
//' @examples
//' myv <- nVennDiagram(list(Set1=c("a", "b", "c"), Set2=c("a", "c", "d")), verbose=F)
//' getVennSvg(myv)
// [[Rcpp::export]]
String getVennSvg(List nVennObj) {
  borderLine bl;
  bl.restoreBl(as<std::string>(nVennObj["desc"]));
  List opts = as<List>(nVennObj["opts"]);
  float opacity = as<float>(opts["opacity"]);
  unsigned int fontSize = as<unsigned int>(opts["fontSize"]);
  unsigned int lineWidth = as<unsigned int>(opts["lineWidth"]);
  unsigned int palette = as<unsigned int>(opts["palette"]);
  bool showRegions = as<bool>(opts["showRegions"]);
  bool showWeights = as<bool>(opts["showWeights"]);
  bl.setSVGOpacity(opacity);
  bl.setSVGLineWidth(lineWidth);
  bl.showCircleNumbers(showWeights);
  bl.showRegionNumbers(showRegions);
  bl.setFontSize(fontSize);
  bl.loadPalette(palette);
  
  if (nVennObj.containsElementNamed("colors")){
    nVennObj["setNames"] = bl.getSetNames();
    List snames = as<List>(nVennObj["setNames"]);
    List colors = as<List>(nVennObj["colors"]);
    
    for (UINT i = 0; i < snames.length(); i++){
      std::string sn = as<std::string>(snames[i]);
      if (colors.containsElementNamed(sn.c_str())){
        if (as<std::string>(colors[sn]) != "_"){
          bl.setVennColor(i, colors[sn]);
        }
      }
    }
  }
  
  return bl.toSVG().getText();
}


//' Rotate a diagram
//'
//' @param nVennObj nVennR2 object generated with [nVennDiagram()].
//' @param angle Angle of rotation in degrees. Positive values rotate
//' counterclockwise.
//' @param plot If true (default), prints the diagram after the rotation.
//' @return nVenn object. 
// [[Rcpp::export]]
SEXP rotateVenn(List nVennObj, float angle, bool plot = true){
  Function asNamespace("asNamespace");
  Environment nv_env = asNamespace("nVennR2");
  borderLine bl;
  bl.restoreBl(as<std::string>(nVennObj["desc"]));
  List skin;
  if (nVennObj.containsElementNamed("opts")){
    skin = nVennObj["opts"];
  }
  if (nVennObj.containsElementNamed("colors")){
    skin["colors"] = nVennObj["colors"];
  }
  float ang = 3.141592 * angle / 180;
  bl.rotateScene(ang);
  std::string result = bl.saveBl();
  SEXP r = toRObject(result);
  Function n = nv_env[".setSetNames"];
  std::vector<std::string> sn = bl.getSetNames();
  r = n(r, sn);
  Function setSkin = nv_env["setVennSkin"];
  r = setSkin(r, skin);
  if (plot){
    Function p = nv_env["plotVenn"];
    p(r);
  }
  return r;
}



