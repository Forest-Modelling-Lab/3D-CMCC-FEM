library(truncnorm)
library(ineq)
library(hydroGOF)
running.string <- "./3D_CMCC_Forest_Model -i /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/input/DEHainich -o /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/output_6.1/debug_output_6.1/debug_output -b /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/output_6.1/daily_output_6.1/daily_output -f /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/output_6.1/monthly_output_6.1/monthly_output -e /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/output_6.1/annual_output_6.1/annual_output -n /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/output_6.1/soil_output_6.1/soil_output -d /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/input/DEHainich/Hainich_input_1ha_2000.txt -m /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/input/DEHainich/111_111_2000.txt -s /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/input/DEHainich/Hainich_site.txt -c /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/input/DEHainich/Hainich_settings.txt"
running.simulation <- "./3D_CMCC_Forest_Model -i /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/input/DEHainich -o /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/output_6.1/debug_output_6.1/debug_output -b /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/output_6.1/daily_output_6.1/daily_output -f /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/output_6.1/monthly_output_6.1/monthly_output -e /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/output_6.1/annual_output_6.1/annual_output -n /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/output_6.1/soil_output_6.1/soil_output -d /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/input/DEHainich/Hainich_input_1ha_2000.txt -m input/DEHainich/111_111_2000.txt,input/DEHainich/111_111_2001.txt,input/DEHainich/111_111_2002.txt,input/DEHainich/111_111_2003.txt,input/DEHainich/111_111_2004.txt,input/DEHainich/111_111_2005.txt,input/DEHainich/111_111_2006.txt,input/DEHainich/111_111_2007.txt -s /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/input/DEHainich/Hainich_site.txt -c /Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/input/DEHainich/Hainich_settings.txt"
site <- "DEhainich"
species <- "fagussylvatica"

setwd("/Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/src")
target.dir <- paste("/Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/input/", site, sep = "")
eclipseoutput.dir <- "~/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/output_6.1/daily_output_6.1"
observed <--(read.table(file=paste("/Users/sergiomarconi/Documents/MarconietAl2016/",site,"Train.csv",sep=""), header=TRUE))

randomGuess <-function(species, site, running.string)
{
  curr.dir <- getwd()
  target.file <- data.frame(read.table(file=paste(target.dir,"/", species, "Optim.txt",sep =""), header=FALSE))
  
  params.list <- c("ALPHA","K","ALBEDO","GAMMA_LIGHT","LAIGCX","LAIMAXINTCPTN","MAXINTCPTN","SLA","SLA_RATIO",
                   "FRACBB0","FRACBB1","TBB","RHOMIN","RHOMAX","TRHO","COEFFCOND","BLCOND","MAXCOND","MAXAGE","RAGE","NAGE",
                   "MAXAGE_S","RAGE_S","NAGE_S","GROWTHTMAX","GROWTHTOPT","GROWTHSTART","MINDAYLENGTH","SWPOPEN",
                   "SWPCLOSE","SWCONST","SWPOWER","OMEGA_CTEM","S0CTEM","R0CTEM","F0CTEM","RES0CTEM","EPSILON_CTEM","KAPPA","MIN_R0CTEM",
                   "MAX_S0CTEM","YEARS_FOR_CONVERSION","FINE_ROOT_LEAF","STEM_LEAF","COARSE_ROOT_STEM",
                   "LIVE_TOTAL_WOOD","CN_LEAVES","CN_LITTER","CN_FINE_ROOTS","CN_LIVE_WOODS","CN_DEAD_WOODS","BUD_BURST","LEAF_FALL_FRAC_GROWING",
                   "LEAF_LIFE_SPAN","LEAVES_FINERTTOVER","COARSERTTOVER","SAPWOODTTOVER","BRANCHTTOVER","LIVE_WOOD_TURNOVER","WSX1000","THINPOWER",
                   "MR","MS","DBHDCMAX","DBHDCMIN","SAP_A","SAP_B","SAP_LEAF","SAP_WRES","HMAX","DMAX","HPOWER","RPOWER","CHPOWER",
                   "STEMCONST_P","STEMPOWER_P","CRA","CRB","CRC","HDMAX","HDMIN","DENMAX","MINPAREST","MINRAIN","ADULT_AGE",
                   "MAXSEED","MASTSEED","WEIGHTSEED","SEXAGE","GERMCAPACITY","MINTEMP","ESTMAX","FRACFRUIT","ROTATION","MINAGEMANAG","MINDBHMANAG",
                   "AV_SHOOT","VERY_LABILE","LABILE","RESISTANT")
  nparams <- length(params.list)
  row.numpars <- rep(0,nparams)
  for(i in 1: nparams){	
    parm <- params.list[i]
    row.numpars[i] <- which(target.file[,1]==parm, arr.ind=TRUE)
  }
  for(runs in 1:100000)
  {
    new.fill <- read.delim(paste("/Users/sergiomarconi/git/3D-CMCC-FEM/parameters/",species, "Prior.txt",sep = ""), header=FALSE)
    new.values<- rep(0,nparams)
    for(i in 1: nparams){	
      if(new.fill[i,2] < 0){
        if(new.fill[i,2] < -6000){new.values[i] <- -9999 }
        else{
        new.values[i] <-rtruncnorm(n=1, a=0, b=(-(new.fill[i,2]+2*new.fill[i,3])), mean=-new.fill[i,2], sd=sqrt(-new.fill[i,3]))
        new.values[i] <- -new.values[i]}
      }else{
        new.values[i] <-rtruncnorm(n=1, a=0, b=(new.fill[i,2]+2*new.fill[i,3]), mean=new.fill[i,2], sd=sqrt(new.fill[i,3]))
      }
    }
    
    target.file[row.numpars,2] <- new.values
    write.table(x=target.file, file=paste("/Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/input/", 
                                          site,"/", species,"Optim.txt", sep = ""), quote=FALSE, row.names=FALSE, col.names = FALSE)
    setwd(curr.dir)
    system(running.string)
    predicted <- read.table(file=paste(eclipseoutput.dir, "/daily_output_",site,"_DNDC.txt", sep=""), header=TRUE)
    if(is.na.data.frame(mean(predicted$NEE)))
    {
      out.val <- c(cor(observed, predicted$NEE),"NA", "NA")
    }else{
      out.val <- c(cor(observed, predicted$NEE),NSE.data.frame(predicted$NEE, observed),
                   rmse.data.frame(predicted$NEE, observed),mae.data.frame(predicted$NEE, observed))
    }
    #names(out.val) <- out.head
    #out.df <-data.frame(out.val, header = out.head)
    #out.val
    FF <- as.matrix(t(c(new.values, out.val)))
    write.table(FF, file = paste("/Users/sergiomarconi/Documents/MarconietAl2016/SensitivityTables/", site, "_OptimNEE2000.csv", sep = ""),  sep = ",",
                col.names = FALSE, append=TRUE, row.names = FALSE)
  }
}


sensitivityAnalysis <- function(site)
{
  params.set <- (read.table(file=paste("/Users/sergiomarconi/Documents/MarconietAl2016/SensitivityTables/", site, "_OptimNEE2000.csv", 
                                       sep = ""), header=TRUE, sep = ","))
  mefR <- abs(params.set['r']) * params.set['mef']
  #params.set['r']
  #params.set['mef']
  #which(mefR == max(mefR))
  besttry <- params.set[which(mefR == max(mefR)),] 
  return(besttry)
}

nsecalc <- function(guess){
  
  curr.dir <- getwd()
  target.file <- data.frame(read.table(file=paste(target.dir,"/", species, "Optim.txt",sep =""), header=FALSE))
  
  params.list <- c("ALPHA","K","ALBEDO","GAMMA_LIGHT","LAIGCX","LAIMAXINTCPTN","MAXINTCPTN","SLA","SLA_RATIO",
                   "FRACBB0","FRACBB1","TBB","RHOMIN","RHOMAX","TRHO","COEFFCOND","BLCOND","MAXCOND","MAXAGE","RAGE","NAGE",
                   "MAXAGE_S","RAGE_S","NAGE_S","GROWTHTMAX","GROWTHTOPT","GROWTHSTART","MINDAYLENGTH","SWPOPEN",
                   "SWPCLOSE","SWCONST","SWPOWER","OMEGA_CTEM","S0CTEM","R0CTEM","F0CTEM","RES0CTEM","EPSILON_CTEM","KAPPA","MIN_R0CTEM",
                   "MAX_S0CTEM","YEARS_FOR_CONVERSION","FINE_ROOT_LEAF","STEM_LEAF","COARSE_ROOT_STEM",
                   "LIVE_TOTAL_WOOD","CN_LEAVES","CN_LITTER","CN_FINE_ROOTS","CN_LIVE_WOODS","CN_DEAD_WOODS","BUD_BURST","LEAF_FALL_FRAC_GROWING",
                   "LEAF_LIFE_SPAN","LEAVES_FINERTTOVER","COARSERTTOVER","SAPWOODTTOVER","BRANCHTTOVER","LIVE_WOOD_TURNOVER","WSX1000","THINPOWER",
                   "MR","MS","DBHDCMAX","DBHDCMIN","SAP_A","SAP_B","SAP_LEAF","SAP_WRES","HMAX","DMAX","HPOWER","RPOWER","CHPOWER",
                   "STEMCONST_P","STEMPOWER_P","CRA","CRB","CRC","HDMAX","HDMIN","DENMAX","MINPAREST","MINRAIN","ADULT_AGE",
                   "MAXSEED","MASTSEED","WEIGHTSEED","SEXAGE","GERMCAPACITY","MINTEMP","ESTMAX","FRACFRUIT","ROTATION","MINAGEMANAG","MINDBHMANAG",
                   "AV_SHOOT","VERY_LABILE","LABILE","RESISTANT")
  nparams <- length(params.list)
  row.numpars <- rep(0,nparams)
  for(i in 1: nparams){	
    parm <- params.list[i]
    row.numpars[i] <- which(target.file[,1]==parm, arr.ind=TRUE)
  }
  new.values <- exp(guess);
  trial.values["SWPOPEN"] <- -trial.values["SWPOPEN"]
  trial.values["SWPCLOSE"] <- -trial.values["SWPCLOSE"] 
  #new.values <- (guess);
  target.file[row.numpars,2] <- as.numeric(new.values)
  write.table(x=target.file, file=paste("/Users/sergiomarconi/git/3D-CMCC-FEM/software/3D-CMCC-Forest-Model/input/", 
                                        site,"/", species,"Optim.txt", sep = ""), quote=FALSE, row.names=FALSE, col.names = FALSE)
  setwd(curr.dir)
  system(running.string)
  predicted <- read.table(file=paste(eclipseoutput.dir, "/daily_output_",site,"_DNDC.txt", sep=""), header=TRUE)
  if(is.na.data.frame(mean(predicted$NEE)))
  {
    out.val <- 100000
  }else{
    out.val <- -NSE.data.frame(predicted$NEE, observed)
  }
  return(out.val)
}

