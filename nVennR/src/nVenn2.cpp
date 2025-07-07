
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


// [[Rcpp::export]]
SEXP nVennR(SEXP desc, bool verbose = true, unsigned int byCol = 0){
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
      Function f("plotSVG");
      f(result);
    }
  }
  Function g = nv_env[".setAsObject"];
  SEXP r = g(result);
  return r;
}

// [[Rcpp::export]]
List getVennSetNames(std::string nvObject){
  borderLine bl;
  bl.restoreBl(nvObject);
  std::vector<std::string> r = bl.getSetNames();
  List result;
  for (unsigned int i = 0; i < r.size(); i++){
    result.push_back(r[i]);
  }
  return result;
}

// [[Rcpp::export]]
List getVennRegion(std::string nvObject, SEXP n) {
  borderLine bl;
  bl.restoreBl(nvObject);
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
String getVennSvg(std::string nvObject) {
  borderLine bl;
  bl.restoreBl(nvObject);
  return bl.toSVG().getText();
}

// [[Rcpp::export]]
std::string rotateVenn(std::string nvObject, float angle){
  borderLine bl;
  bl.restoreBl(nvObject);
  bl.rotateScene(angle);
  std::string result = bl.saveBl();
  return result;
}
