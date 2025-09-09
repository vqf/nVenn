#' @keywords internal
"_PACKAGE"

## usethis namespace: start

#' nVennR
#'
#' An interface to nVenn2 to create n-dimensional, quasi-proportional Venn
#' diagrams.
#'
#' @name nVennR
#' @details
#' The input for nVennR is a list of lists or a text table. The dataset 
#' `exampledf` contains an example of a list of lists, where each internal 
#' list has a name for the set and some set elements. The result will be a 
#' Venn diagram where each region is represented by a circle whose area is 
#' approximately proportional to the number of elements in that region. Each 
#' circle has two labels. The number in a larger font size represents the 
#' number of elements in the region. The smaller numbers in parentheses 
#' show which sets the region belongs to.
#' 
#' A diagram is created 
#' with [nVennDiagram()] and plotted and/or saved with [plotVenn()]. The 
#' appearance of the plot can be changed with [setVennOpts()] and 
#' [setVennColors()]. If a plot is saved, the resulting svg file can be 
#' loaded again with [readVennSVG()].
#' 
#' @author Victor Quesada, Universidad de Oviedo (Spain).
#' @examples
#' library(nVennR)
#' myv <- nVennDiagram(exampledf, verbose=F)
#' myv <- setVennOpts(myv, opacity=0.2, fontSize=14)
#' 

## usethis namespace: end
NULL

