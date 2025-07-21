\name{nVennR-package}
\alias{nVennR-package}
\alias{nVennR}
\docType{package}
\title{
   Interface for the nVenn2 algorithm to create quasi-proportional Venn 
   diagrams with an arbitrary number of sets.
}
\description{
  This algorithm is faster
 than the first version of nVenn and attempts to simplify the resulting
 diagram. Starting conditions are pseudo-random, which means that the 
 same input can be run several times with different results.
}
\details{
  The input for nVennR is a list of lists or a text table. The dataset 
  `exampledf` contains an example of a list of lists, where each internal 
  list has a name for the set and some set elements. The result will be a 
  Venn diagram where each region is represented by a circle whose area is 
  approximately proportional to the number of elements in that region. Each 
  circle has two labels. The number in a larger font size represents the 
  number of elements in the region. The smaller numbers in parentheses 
  show which sets the region belongs to.
  
  A diagram is created 
  with [nVennDiagram()] and plotted with [plotVenn()]. The appearance of the plot
  can be changed with [setVennOpts()] and [setVennColors()].
}
\author{
Victor Quesada, Universidad de Oviedo (Spain).

Maintainer: Victor Quesada <quesadavictor@uniovi.es>
}
\references{
  This optional section can contain literature or other references for
  background information.
}
\keyword{ package }
\seealso{
  \link{nVennDiagram} to create diagram, \link{plotVenn} to plot diagram, 
  \link{setVennOpts} to customize the plot, \link{setVennColor},
  \link{setVennColors} and \link{setVennPalette} to change plot colors, 
  \link{setVennSkin} to set all graphical properties.
}
\examples{
     library(nVennR)
     myv <- nVennDiagram(exampledf)
     myv <- setVennOpts(myv, opacity=0.2, fontSize=14)
}
