# You can learn more about package authoring with RStudio at:
#
#   http://r-pkgs.had.co.nz/
#
# Some useful keyboard shortcuts for package authoring:
#
#   Build and Reload Package:  'Ctrl + Shift + B'
#   Check Package:             'Ctrl + Shift + E'
#   Test Package:              'Ctrl + Shift + T'

#' @useDynLib nVennR
#' @importFrom Rcpp sourceCpp
NULL

#' Example data frame.
#'
#' A dataset containing programming preferences from 18 employees. This data set was
#' provided by user Krantz to inquire about nVennR
#' 
#' @format A data frame with 18 rows and 3 variables:
#' \describe{
#'   \item{Employee}{Employee ID}
#'   \item{SAS}{Employee uses SAS}
#'   \item{Python}{Employee uses Python}
#'   \item{R}{Employee uses R}
#' }
#' @source \url{https://stackoverflow.com/questions/49471565/transforming-data-to-create-generalized-quasi-proportional-venn-diagrams-using}
"exampledf"


lol2string <- function(t){
  result <- ""
  for (r in names(t)){
    result <- paste(c(result, paste(c(r, t[[r]]), collapse = "\t")) , collapse = "\n")
  }
  return(result);
}

nVenn <- function(t){
  result <- ""
  if (inherits(t, "list")){
    for (r in names(t)){
      result <- paste(c(result, paste(c(r, t[[r]]), collapse = "\t")) , collapse = "\n")
    }
  }
  else if (inherits(t, "character")){
    result <- t
  }
  else{
    print("Cannot convert object for use in nVenn")
  }
  nVennR::nVennCpp(result)
}

