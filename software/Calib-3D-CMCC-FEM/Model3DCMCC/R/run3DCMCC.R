#' @export

# Function to call the model

    run3DCMCC <-function (pathExec,mainDir,inputDir, paramDir, outDir,standFile,climatefile, soilfile,topofile, setfile,co2file,
        species,paramValues) {
         writeParamFile(file.path(mainDir,paramDir),species,paramValues)
           
         systemCall  <-paste0(#pathExec," ", 
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
          system2(command=pathExec,args=systemCall,stdout = file.path(mainDir,"./logFile.txt"))
          setwd(currentwd)
        assign("systemCall", systemCall, envir=.GlobalEnv)

     getResults(file.path(mainDir,outDir))

    }

