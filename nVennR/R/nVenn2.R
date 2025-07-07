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


.lol2string <- function(t){
  result <- ""
  for (r in names(t)){
    result <- paste(c(result, paste(c(r, t[[r]]), collapse = "\t")) , collapse = "\n")
  }
  return(result);
}

.setAsObject <- function(t){
  class(t) <- c(class(t), "nVennObj")
  return(t)
}

#' Title Plot the current Venn diagram. 
#'
#' @param systemShow If true, it also displays the result in the system-defined editor.
#'
#' @export
#'
plotSVG <- function(nVennObj, systemShow = F){
  tfile <- tempfile(fileext = ".svg")
  tfile2 <- tempfile(fileext = ".svg")
  cat(getVennSvg(nVennObj), file=tfile)
  if (requireNamespace("rsvg", quietly = TRUE) && requireNamespace("grImport2", quietly = TRUE)) {
    out <- tryCatch(
      {
        rsvg::rsvg_svg(svg = tfile, tfile2)
        p <- grImport2::readPicture(tfile2, warn = F)
        grImport2::grid.picture(p)
      },
      error=function(cond){
        message(paste("rsvg or grImport2 reported an error: ", cond))
        message("The figure cannot be rendered in the plot window. Please, use the arguments outFile and/or systemShow.")
      }
    )
  } else {
    if (systemShow == FALSE && outFile == ''){
      message("The figure cannot be rendered in the plot window. Please, use the arguments outFile and/or systemShow.")
    }
  }
  if (systemShow){
    utils::browseURL(tfile)
  }
}


