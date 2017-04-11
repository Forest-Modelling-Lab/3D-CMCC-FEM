\name{run3DCMCC}
\alias{run3DCMCC}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{Function to run 3D-CMCC in R
}
\description{
%%  ~~ A concise (1-5 lines) description of what the function does. ~~
}
\usage{
run3DCMCC(pathExec, inputDir, paramDir, outDir, standFile, climatefile, soilfile, topofile, setfile, co2file, species, paramValues, nyears,truePath=TRUE)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{pathExec}{
path to the executable for 3D-CMCC
}
  \item{mainDir}{
Main directory where all the needed subdirectories are located. Must be an absolute path.
}
  \item{inputDir}{
Directory where the input files are located. Path relative to mainDir
}
  \item{paramDir}{
Directory where to write the new parameter file. Path relative to mainDir
}
  \item{outDir}{
Directory where the results from the 3D-CMCC run are located. Path relative to mainDir
}
  \item{standFile}{
name or location of the input Stand file. Path relative to mainDir
}
  \item{climatefile}{
name of the input climate file.
}
  \item{soilfile}{
name of the input soil file
}
  \item{topofile}{
name of the input topographic file
}
  \item{setfile}{
name of the input settings file
}
  \item{co2file}{
name of the input CO2 file
}
  \item{species}{
species considered for the simulation (no multiple species supported at this moment)
}
  \item{paramValues}{
named vector, including the parameter names written in capital letters, and corresponding parameter values. A full set of parameters should be provided.
}
  \item{nyears}{
simulated number of years. The function internally calculated the corrisponding number of months and days
}
  \item{truePath}{
is outDir the path to the true directory with results? If TRUE, the function will look in the specified outDir for results. If FALSE (default), it will look in a subdirectory specified as file.path(outDir,paste0("output_",format(Sys.Date(),"%Y"),"_",toupper(format(Sys.Date(),"%B")),"_",format(Sys.Date(),"%d")))
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
# run3DCMCC(pathExec="D:/bash_model/software/3D-CMCC-Forest-Model/bin/3D-CMCC-Forest-Model.exe",
#               mainDir="~/Desktop/TG18//3D-CMCC/Asimulazioni_def",
#               inputDir="input", 
#               paramDir="parameters/collelongo_fagus", 
#               outDir="output/collelongo_fagus",
#               standFile="stand/collelongo_fagus/stand.txt",
#               climatefile="Collelongo_clima_1997_2014.txt", soilfile="Collelongo_soil.txt",
#               topofile="Collelongo_topo.txt", setfile="Collelongo_settings.txt",co2file="CO2_historical_1901_2012.txt",
#               species="Fagussylvatica",paramValues=param, nyears=6)
}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ ~kwd1 }
\keyword{ ~kwd2 }% __ONLY ONE__ keyword per line
