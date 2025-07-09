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

#' Set the appearance of an nVenn object diagram.
#' 
#' The function returns an object whose parameters will change the way the 
#' diagram is plotted. To see the changes, it is necessary to run \link{plotSVG}
#' afterwards.
#'
#' @param nVennObj nVenn object.
#' @param opacity Opacity of sets, between 0 (completely transparent)
#'                and 1 (completely opaque). Defaults to 0.4.
#' @param fontSize Size of the font for the numbers of elements (weights) in each
#'                 region. It also indirectly controls the font sizes of the 
#'                 region descriptions, if shown (showRegions=T). Defaults to
#'                 10.
#' @param lineWidth Width of the lines defining each set. The value will be 
#'                  rounded to the closest integer. Defaults to 1.
#' @param palette Color palette to use to fill the sets. Integer from 1 to 4. 
#'                Defaults to 1.
#' @param showRegions If true (default), show region descriptions.
#' @param showWeights If true (default), show number of elements in each region.
#'
#' @returns nVennObj with graphical options set.
#' @seealso \link{plotSVG} for plotting the resulting diagram.
#' @export
#'
#' @examples
#' myv <- nVennDiagram(exampledf)
#' myv <- setVennOpts(myv, showRegions=F, opacity=0.2, lineWidth=2)
#' plotSVG(myv)
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

#' Plot nVenn diagram
#' 
#' The nVenn diagram is generated in the plot window by default. If this is not
#' possible, parameters outFile and systemShow may be used to export the SVG 
#' figure. 
#'
#' @param nVennObj nVenn object.
#' @param outFile Path to export the SVG figure. If empty, the figure is not
#'                exported.
#' @param systemShow If true, and the system has a default SVG-editing program,
#'                   opens the figure in the default editor
#'
#' @export
#'
plotSVG <- function(nVennObj, outFile='', systemShow = F){
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


