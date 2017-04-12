#' @export

# Function to call the model

    run3DCMCC <-function (pathExec,mainDir,inputDir, paramDir, outDir,standFile,climatefile, soilfile,topofile, setfile,co2file,
        species,paramValues,nyears,truePath=FALSE) {
     
         writeParamFile(file.path(mainDir,paramDir),species,paramValues)
           
         systemCall  <-paste0(pathExec," ", 
                          "-i"," ", file.path(mainDir,inputDir)," ", 
                           "-p"," ", file.path(mainDir,paramDir)," ", 
                           "-o"," ", outDir," ", 
                           "-d"," ", standFile, " ", 
                           "-m"," ", climatefile," ", 
                           "-s"," ", soilfile," ", 
                           "-t"," ", topofile," ", 
                           "-c"," ", setfile," ", 
                           "-k"," ", co2file)    
          currentwd <- getwd()
          setwd(mainDir)
          system(command=systemCall,show.output.on.console = FALSE)
          setwd(currentwd)
          

     getResults(file.path(mainDir,outDir),nyears,truePath=truePath)

    }

