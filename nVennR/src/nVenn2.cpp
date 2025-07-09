
#include <Rcpp.h>
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
  Environment nv_env = asNamespace("nVennR");
  Function g = nv_env[".setAsObject"];
  Function h = nv_env[".optData"];
  SEXP opts = h(opacity, fontSize, lineWidth, palette, showRegions, showWeights);
  SEXP r = g(desc, opts);
  return(r);
}

//' Creates nVenn plot
//'
//' @param desc Description of sets, either as a list of lists or as text.
//' @param v2 verbose If true, shows messages as the nVenn plot is created.
//' @return nVenn object. As a side effect, shows the nVenn plot.
// [[Rcpp::export]]
SEXP nVennDiagram(SEXP desc, bool verbose = true, unsigned int byCol = 0){
  List sv = desc;
  std::string dsc;
  borderLine bl;
  Function asNamespace("asNamespace");
  Environment nv_env = asNamespace("nVennR");
  std::string result;
  if (sv.size() > 0){
    StringVector s1 = as<StringVector>(sv[0]);
    if (sv.size() == 1 && s1.size() == 1){ //Text
      dsc = as<std::string>(s1[0]);
    }
    else{
      Function f = nv_env[".lol2string"];
      dsc = as<std::string>(f(desc));
    }
  }
  bl = borderLine(dsc, byCol);
  if (bl.err()){
    Rcout << "Input error\n";
  }
  else{
    nvSimulate(bl, verbose);
    if (bl.err()){
      Rcout << bl.errorMsg() << std::endl;
    }
    else{
      result = bl.saveBl();
    }
  }
  SEXP r = toRObject(result);
  Function f("plotSVG");
  f(r);
  return r;
}

// [[Rcpp::export]]
List getVennSetNames(List nvObject){
  borderLine bl;
  bl.restoreBl(as<std::string>(nvObject["desc"]));
  std::vector<std::string> r = bl.getSetNames();
  List result;
  for (unsigned int i = 0; i < r.size(); i++){
    result.push_back(r[i]);
  }
  return result;
}

// [[Rcpp::export]]
List getVennRegion(List nvObject, SEXP n) {
  borderLine bl;
  bl.restoreBl(as<std::string>(nvObject["desc"]));
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


// [[Rcpp::export]]
String getVennSvg(List nvObject) {
  borderLine bl;
  bl.restoreBl(as<std::string>(nvObject["desc"]));
  List opts = as<List>(nvObject["opts"]);
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
  
  if (nvObject.containsElementNamed("colors")){
    nvObject["setNames"] = bl.getSetNames();
    List snames = as<List>(nvObject["setNames"]);
    List colors = as<List>(nvObject["colors"]);
    
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

// [[Rcpp::export]]
SEXP rotateVenn(List nvObject, float angle){
  borderLine bl;
  bl.restoreBl(as<std::string>(nvObject["desc"]));
  List opts = nvObject["opts"];
  float ang = 3.141592 * angle / 180;
  bl.rotateScene(ang);
  std::string result = bl.saveBl();
  SEXP r = toRObject(result, opts["opacity"],
                     opts["fontSize"], opts["lineWidth"],
                     opts["showRegions"], opts["showWeights"]);
  return r;
}



