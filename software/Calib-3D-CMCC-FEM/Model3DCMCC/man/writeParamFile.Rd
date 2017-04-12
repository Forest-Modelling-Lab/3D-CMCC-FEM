\name{writeParamFile}
\alias{writeParamFile}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
Function to write parameter files for 3D-CMCC
}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
writeParamFile(paramDir, species, paramValues)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{paramDir}{
Directory where to write the new parameter file
}
  \item{species}{
species considered for the simulation (no multiple species supported at this moment)
}
  \item{paramValues}{
named vector, including the parameter names written in capital letters, and corresponding parameter values. A full set of parameters should be provided.
}
}
\details{
%%  ~~ If necessary, more details than the description above ~~
}
\value{
%%  ~Describe the value returned
%%  If it is a LIST, use
%%  \item{comp1 }{Description of 'comp1'}
%%  \item{comp2 }{Description of 'comp2'}
%% ...
}
\references{
%% ~put references to the literature/web site here ~
}
\author{
Bagnara M., Pellicone G.
}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
%% ~~objects to See Also as \code{\link{help}}, ~~~
}
\examples{
#writeParamFile(paramDir="~/Desktop/TG18//3D-CMCC/Asimulazioni_def//parameters/collelongo_fagus//",species="Fagussylvatica",paramValues=param)

}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
