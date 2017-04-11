\name{getResults}
\alias{getResults}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{  Function to import 3D-CMCC results in R
}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
getResults(outDir, nyears, truePath=FALSE)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{outDir}{
Directory where the results from the 3D-CMCC run are located
}
  \item{nyears}{
simulated number of years. The function internally calculated the corrisponding number of months and days
}
  \item{truePath}{
is outDir the path to the true directory with results? If TRUE, the function will look in the specified outDir for results. If FALSE (default),
it will look in a subdirectory specified as file.path(outDir,paste0("output_",format(Sys.Date(),"%Y"),"_",toupper(format(Sys.Date(),"%B")),"_",format(Sys.Date(),"%d")))
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
#getResults(outDir="~/Desktop/TG18//3D-CMCC//Asimulazioni_def/output/collelongo_fagus",nyears=6)
#getResults(outDir="~/Desktop/TG18//3D-CMCC//Asimulazioni_def/output/collelongo_fagus/output_2016_DECEMBER_28",nyears=6,truePath=TRUE)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
