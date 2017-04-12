#' @export

#Function to create parameter file

writeParamFile <- function(paramDir,species,paramValues) {   
  write.table(as.matrix(paramValues), 
              file =file.path(paramDir, paste0(species,".txt")),
              row.names = TRUE, 
              col.names = FALSE, 
              quote = FALSE, 
              sep = "\t")   
}
