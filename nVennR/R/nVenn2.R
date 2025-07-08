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
  return(result)
}

.setAsObject <- function(t, options=NULL){
  if (is.null(options)){
    options <- .optData()
  }
  obj <- list()
  obj$desc <- t
  obj$opts <- options
  class(obj) <- c(class(obj), "nVennObj")
  return(obj)
}

.resetvcolors <- function(nVennObj){
  clrs <- list()
  i <- 0
  for (sn in nVennObj$setNames){
    i <- i + 1
    clrs[[sn]] <- "_"
  }
  nVennObj$colors <- clrs
  return(nVennObj)
}

.avcolors <- function(nVennObj){
  if (!("colors" %in% nVennObj)){
    #nVennObj$setNames <- unlist(getVennSetNames(nVennObj))
    nVennObj$colors <- .resetvcolors(nVennObj)
  }
  return(nVennObj)
}

.optData <- function(opacity = 0.4, fontSize = 12,
                     lineWidth = 1, palette = 0,
                     showRegions = T, showWeights = T){
  result <- list()
  result$opacity <- opacity
  result$fontSize <- fontSize
  result$lineWidth <- lineWidth
  result$palette <- palette;
  result$showRegions <- showRegions
  result$showWeights <- showWeights
  class(result) <- c(class(result), "vennOpts")
  return(result)
}

setVennOpts <- function(nVennObj, opacity = 0.4, fontSize = 12,
                     lineWidth = 1, palette = 0,
                     showRegions = T, showWeights = T){
  if (is.null(nVennObj$opts)){
    nVennObj$opts = .optData()
  }
  nVennObj$opts$opacity <- opacity
  nVennObj$opts$fontSize <- fontSize
  nVennObj$opts$lineWidth <- lineWidth
  nVennObj$opts$palette <- palette
  nVennObj$opts$showRegions <- showRegions
  nVennObj$opts$showWeights <- showWeights
  return(nVennObj)
}

setVennPalette <- function(nVennObj, palette = 0){
  nVennObj <- setVennOpts(nVennObj = nVennObj, palette = palette)
  nVennObj <- .resetvcolors(nVennObj)
  return(nVennObj)
}

setVennColor <- function(nVennObj, setName, color){
  #nVennObj <- .avcolors(nVennObj)
  nVennObj$setNames <- unlist(getVennSetNames(nVennObj))
  if (setName %in% nVennObj$setNames){
    nVennObj$colors[[setName]] <- color
  }
  else{
    warning(cat("Set\"", setName, "\" does not exist. Use getVennSetNames() to see",
    " a list of set names", sep = ""))
  }
  return(nVennObj)
}

setVennColors <- function(nVennObj, colorList){
  nVennObj <- .avcolors(nVennObj)
  sn <- names(colorList)
  if (is.null(sn)){
    for (i in 1:length(nVennObj$setNames)){
      if (i <= length(colorList)){
        nm <- nVennObj$setNames[i]
        vl <- colorList[i]
        nVennObj <- setVennColor(nVennObj, nm, vl);
      }
    }
  }
  else{
    for (nm in sn){
      nVennObj <- setVennColor(nVennObj, nm, colorList[[nm]])
    }
  }
  return(nVennObj)
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


