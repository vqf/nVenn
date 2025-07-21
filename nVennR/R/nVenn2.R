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
#' @source <https://stackoverflow.com/questions/49471565/transforming-data-to-create-generalized-quasi-proportional-venn-diagrams-using>
"exampledf"


.setSetNames <- function(nVennObj, snames){
  nVennObj$setNames <- snames
  return(nVennObj)
}

.validOpts <- function(){
  result <- c("opacity", 
              "fontSize", 
              "lineWidth",
              "palette",
              "showRegions",
              "showWeights")
  return(result);
}

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
    nVennObj <- .resetvcolors(nVennObj)
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

#' Set graphical parameters of an nVenn object diagram.
#' 
#' The function returns an object whose parameters will change the way the 
#' diagram is plotted. 
#'
#' @param nVennObj nVenn object generated with [nVennDiagram()].
#' @param opacity Opacity of sets, between 0 (completely transparent)
#'                and 1 (completely opaque). Defaults to 0.4.
#' @param fontSize Size of the font for the numbers of elements (weights) in each
#'                 region. It also indirectly controls the font sizes of the 
#'                 region descriptions, if shown (showRegions=T). Defaults to
#'                 10.
#' @param lineWidth Width of the lines defining each set. The value will be 
#'                  rounded to the closest integer. Defaults to 1.
#' @param palette Color palette to use to fill the sets. Integer from 0 to 3. 
#'                Defaults to 0.
#' @param showRegions If true (default), show region descriptions.
#' @param showWeights If true (default), show number of elements in each region.
#' @param plot If true (default), plots the diagram after the operation.
#'
#' @returns nVennObj with graphical options set.
#' @export
#'
#' @examples
#' myv <- nVennDiagram(exampledf)
#' myv <- setVennOpts(myv, showRegions=F, opacity=0.2, lineWidth=2)
setVennOpts <- function(nVennObj, opacity = 0.4, fontSize = 12,
                     lineWidth = 1, palette = 0,
                     showRegions = T, showWeights = T, plot=T){
  if (is.null(nVennObj$opts)){
    nVennObj$opts = .optData()
  }
  defaults <- list(opacity = opacity, fontSize = fontSize,
                   lineWidth = lineWidth, palette = palette,
                   showRegions = showRegions, showWeights = showWeights
  )
  dnames <- names(defaults)
  passed <- names(match.call())
  prevOptions <- names(nVennObj$opts)
  params <- list()
  allopts <- .validOpts()
  for (o in passed){
    if (o %in% allopts){
      params[[o]] <- defaults[[o]]
    }
  }
  nVennObj <- setVennSkin(nVennObj, params, plot = F)
  if (plot){
    plotVenn(nVennObj)
  }
  return(nVennObj)
}

#' Set nVenn diagram appearance
#'
#' @param nVennObj nVenn object generated with [nVennDiagram()].
#' @param params List of graphical options, as in [setVennOpts()], plus two
#' special parameters: `palette` to set a color palette as in [setVennPalette()]
#' and `colors` to change set colors as in [setVennColors()].
#' @param plot If true (default), plots the diagram after the operation.
#' 
#' @details
#' Running this function is equivalent to running [setVennOpts()] repeatedly.
#' The advantage is that [setVennSkin()] can set multiple parameters at the 
#' same time. The same `params` list can be used on multiple nVenn objects
#' as a theme.
#' 
#' The only valid parameters for `params` are those in [setVennOpts()], 
#' `palette` and `colors`.
#' 
#'
#' @returns
#' @export
#'
#' @examples
#' theme <- list(opacity=0.2, lineWidth=2, fontSize=14, showRegions=F,
#'               palette=2, colors=c("black"))
#' myv <- nVennDiagram(exampledf)
#' myv <- setVennSkin(myv, theme)
setVennSkin <- function(nVennObj, params, plot=T){
  valid <- .validOpts()
  for (p in names(params)){
    if (p %in% valid){
      nVennObj$opts[[p]] <- params[[p]]
    }
    else if (p == "palette"){
      nVennObj <- setVennPalette(nVennObj, params[[p]], plot = F)
    }
    else if (p == "colors"){
      nVennObj <- setVennColors(nVennObj, params[[p]], plot = F)
    }
    else{
      warning(paste("Unrecognized parameter \"", p, "\"", sep = ""))
    }
  }
  if (plot){
    plotVenn(nVennObj)
  }
  return(nVennObj)
}

#' Change the color palette for a diagram
#'
#' @param nVennObj nVenn object created with [nVennDiagram()].
#' @param palette Either 0 (default), 1, 2 or 3. Each number defines a 
#' color palette that will be applied to the results
#' @param plot If true (default), plots the diagram after applying the palette.
#'
#' @returns nVenn object with the palette set to the requested value. 
#' @details
#' The color palette for a diagram can also be set with [setVennOpts()], but 
#' with a small difference. If a set color has been set with [setVennColor()],
#' [setVennPalette()] will override and delete that setting, while [setVennOpts()]
#' will not. This way, one can have a color defined for a set and change the 
#' colors of the rest of sets with [setVennOpts()] or reset every color to the 
#' pre-defined palette with [setVennPalette()].
#' 
#' @seealso [setVennOpts()] for an alternative way to change the color palette.
#' 
#' @export
#'
#' @examples
#' myv <- nVennDiagram(list(Set1=c("a", "b", "c"), Set2=c("a", "c", "d")), verbose=F)
#' myv <- setVennPalette(myv, 2)
#' myv <- setVennPalette(myv, 3)
setVennPalette <- function(nVennObj, palette = 0, plot=T){
  nVennObj <- setVennOpts(nVennObj = nVennObj, palette = palette, plot = F)
  nVennObj <- .resetvcolors(nVennObj)
  if (plot){
    plotVenn(nVennObj)
  }
  return(nVennObj)
}

#' Change a set color
#'
#' @param nVennObj nVenn object created with [nVennDiagram()].
#' @param setName Name of the set.
#' @param color New color. It is important to notice that this parameter must be 
#' a valid color in SVG format. The value is not checked, and therefore an 
#' incorrect value may break the plot in [plotVenn()] or lead to unexpected
#' results.
#' @param plot If true (default), plots the diagram after setting the color.
#'
#' @returns nVenn object with the change in color for the set.
#' @export
#'
#' @examples
#' myv <- nVennDiagram(list(Set1=c("a", "b", "c"), Set2=c("a", "c", "d")), verbose=F)
#' myv <- setVennColor(myv, "Set2", "black")
#' myv <- setVennColor(myv, "Set1", "#ffff00")
setVennColor <- function(nVennObj, setName, color, plot=T){
  if (setName %in% nVennObj$setNames){
    nVennObj$colors[[setName]] <- color
  }
  else{
    warning(cat("Set\"", setName, "\" does not exist. Use getVennSetNames() to see",
    " a list of set names", sep = ""))
  }
  if (plot){
    plotVenn(nVennObj)
  }
  return(nVennObj)
}

#' Change set colors
#'
#' @param nVennObj nVenn object created with [nVennDiagram()].
#' @param setName Name of the set.
#' @param colorList Vector or list of colors for the sets (see Details).
#' @param plot If true (default), plots the diagram after setting the colors.
#' 
#' @details
#' If a vector of svg-formatted colors is provided, they will be used in the same
#' order. This is a good way to create and use a custom color palette.
#' If a list is used, the function will call [setVennColor()] with the names
#' in the list.
#' 
#' It is important to notice that each color must be 
#' a valid color in SVG format. The value is not checked, and therefore an 
#' incorrect value may break the plot in [plotVenn()] or lead to unexpected
#' results.
#' 
#' 
#'
#' @returns nVenn object with changed set colors.
#' @export
#'
#' @examples
#' myv <- nVennDiagram(list(Set1=c("a", "b", "c"), Set2=c("a", "c", "d")), verbose=F)
#' mypalette <- c("black", "#ffff00", "red")
#' myv <- setVennColors(myv, mypalette)
setVennColors <- function(nVennObj, colorList, plot=T){
  nVennObj <- .avcolors(nVennObj)
  sn <- names(colorList)
  if (is.null(sn)){
    for (i in 1:length(nVennObj$setNames)){
      if (i <= length(colorList)){
        nm <- nVennObj$setNames[i]
        vl <- colorList[i]
        nVennObj <- setVennColor(nVennObj, nm, vl, plot = F);
      }
    }
  }
  else{
    for (nm in sn){
      nVennObj <- setVennColor(nVennObj, nm, colorList[[nm]], plot = F)
    }
  }
  if (plot){
    plotVenn(nVennObj)
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
plotVenn <- function(nVennObj, outFile='', systemShow = F){
  tfile = outFile
  if (tfile == "") tfile <- tempfile(fileext = ".svg")
  #tfile2 <- tempfile(fileext = ".svg")
  cat(getVennSvg(nVennObj), file=tfile)
  if (requireNamespace("rsvg", quietly = TRUE) && requireNamespace("grImport2", quietly = TRUE)) {
    out <- tryCatch(
      {
        #rsvg::rsvg_svg(svg = tfile, tfile2)
        p <- grImport2::readPicture(rawToChar(rsvg::rsvg_svg(svg = tfile)), warn = F)
        plot.new()
        
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


