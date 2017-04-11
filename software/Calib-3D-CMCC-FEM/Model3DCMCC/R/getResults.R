#' @export

getResults<-function(outDir,nyears,truePath=FALSE){
  results<-list()
  nmonths <- nyears*12
  ndays <- nyears*365
  
  Sys.setlocale("LC_ALL","English")
  
  ifelse (truePath==FALSE,
  yes = newDir <- file.path(outDir,paste0("output_5.3.1_",format(Sys.Date(),"%Y"),"_",toupper(format(Sys.Date(),"%B")),"_",format(Sys.Date(),"%d"))),
  no = newDir <- outDir
  )
  if (dir.exists(file.path(newDir,"annual"))) results[["annual"]]<-read.csv(list.files(file.path(newDir,"annual"),full.names = T)[1],nrows=nyears,header=T)  
  if (dir.exists(file.path(newDir,"monthly")))results[["monthly"]]<-read.csv(list.files(file.path(newDir,"monthly"),full.names = T)[1],nrows=nmonths,header=T)
  if (dir.exists(file.path(newDir,"daily"))) results[["daily"]]<-read.csv(list.files(file.path(newDir,"daily"),full.names = T)[1],nrows=ndays,header=T)
  
  Sys.setlocale()
  
  return(results)     
}
