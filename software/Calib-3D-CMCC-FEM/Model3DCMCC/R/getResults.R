#' @export

getResults<-function(outDir){
  #nmonths <- nyears*12
  #ndays <- nyears*365
  
  #Sys.setlocale("LC_ALL","English")
  
  #ifelse (truePath==FALSE,
  #yes = newDir <- file.path(outDir, paste0("output_5.3.1_",format(Sys.Date(),"%Y"),"_",toupper(format(Sys.Date(),"%B")),"_",format(Sys.Date(),"%d"))),
  #no = newDir <- outDir
  #)
  errorMessage <- "Error in running 3D-CMCC-CNR-FEM. Please check the log file in the main directory."
  if (length(list.files(outDir))==0 ) {
    cat("\n")
    cat(message(errorMessage))
    results <- errorMessage} else {
  annual <- list.files(outDir, pattern = "annual",recursive = TRUE, full.names = TRUE)
  monthly <- list.files(outDir, pattern = "monthly",recursive = TRUE, full.names = TRUE)
  daily <- list.files(outDir, pattern = "daily",recursive = TRUE, full.names = TRUE)
  
  if ((file.exists(annual) & "#error:" %in% readLines(annual)) | 
      (file.exists(monthly) & "#error:" %in% readLines(monthly)) | 
      (file.exists(daily) & "#error:" %in% readLines(daily))){
    cat("\n")
    cat(message(errorMessage))
    results <- errorMessage} else {
  results<-list()
  if (file.exists(annual)) results[["annual"]]<-read.csv(annual,header=T,comment.char = "#")
  if (file.exists(monthly)) results[["monthly"]]<-read.csv(monthly,header=T,comment.char = "#")
  if (file.exists(daily)) results[["daily"]]<-read.csv(daily,header=T,comment.char = "#")
    }
  
  #sys.setlocale()
    }
  return(results)     
}

#annual <- list.files("D:/RR/Github/TG18/3D-CMCC/Simul_Model/sensitivity/ITbon_gpp/output/", pattern = "annual",recursive = TRUE, full.names = TRUE)
#monthly <- list.files("D:/RR/Github/TG18/3D-CMCC/Simul_Model/sensitivity/ITbon_gpp/output/", pattern = "monthly",recursive = TRUE, full.names = TRUE)
#daily <- list.files("D:/RR/Github/TG18/3D-CMCC/Simul_Model/sensitivity/ITbon_gpp/output/", pattern = "daily",recursive = TRUE, full.names = TRUE)

