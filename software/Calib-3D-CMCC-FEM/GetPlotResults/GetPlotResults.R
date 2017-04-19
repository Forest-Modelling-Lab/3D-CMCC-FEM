setwd("C:/Users/gaetano/Desktop/ITbon_gpp/")

if(.Platform$OS.type=="unix"){
  systemCall  <- paste0("./model/3D-CMCC-Forest-Model"," ",
                        "-i"," ", "./input/"," ",
                        "-p"," ", "parameters/"," ",
                        "-o"," ", "output/"," ",
                        "-d"," ", "Bonis_stand_plaricio.txt", " ",
                        "-m"," ", "Bonis_meteo_1955_new.txt"," ",
                        "-s"," ", "Bonis_soil_new.txt"," ",
                        "-t"," ", "Bonis_topo.txt"," ",
                        "-c"," ", "Bonis_settings_rep.txt"," ",
                        "-k"," ", "z_CO2_historical_1901_2012.txt")
  system(systemCall)}
if(.Platform$OS.type=="windows"){
  systemCall  <- paste0("./model/3D-CMCC-Forest-Model.exe"," ",
                        "-i"," ", "./input/"," ",
                        "-p"," ", "parameters/"," ",
                        "-o"," ", "output/"," ",
                        "-d"," ", "Bonis_stand_plaricio.txt", " ",
                        "-m"," ", "Bonis_meteo_1955_new.txt"," ",
                        "-s"," ", "Bonis_soil_new.txt"," ",
                        "-t"," ", "Bonis_topo.txt"," ",
                        "-c"," ", "Bonis_settings_rep.txt"," ",
                        "-k"," ", "z_CO2_historical_1901_2012.txt")
  system(systemCall)}

outputCMCC<- list()

annual <- list.files("./output", pattern = "annual",recursive = TRUE, full.names = TRUE)
monthly <- list.files("./output", pattern = "monthly",recursive = TRUE, full.names = TRUE)
daily <- list.files("./output", pattern = "daily",recursive = TRUE, full.names = TRUE)

if (file.exists(annual)) outputCMCC[["annual"]]<-read.csv(annual,header=T,comment.char = "#")
if (file.exists(monthly)) outputCMCC[["monthly"]]<-read.csv(monthly,header=T,comment.char = "#")
if (file.exists(daily)) outputCMCC[["daily"]]<-read.csv(daily,header=T,comment.char = "#")

outputCMCC$annual$Date <- paste0("01/01/",outputCMCC$annual$YEAR)
outputCMCC$annual$Date <- as.Date(outputCMCC$annual$Date, format = "%d/%m/%Y")

outputCMCC$monthly$Date <- paste0("01/",outputCMCC$monthly$MONTH, "/",outputCMCC$monthly$YEAR)
outputCMCC$monthly$Date <- as.Date(outputCMCC$monthly$Date, format = "%d/%m/%Y")

outputCMCC$daily$Date <- paste0(outputCMCC$daily$DAY,"/",outputCMCC$daily$MONTH, "/",outputCMCC$daily$YEAR)
outputCMCC$daily$Date <- as.Date(outputCMCC$daily$Date, format = "%d/%m/%Y")

#output model DAILY
pdf("Daily.pdf",onefile = T,width = 15,height = 12)
par(mfrow=c(4,4))
for (i in 1:length(outputCMCC$daily)) plot(outputCMCC$daily$Date, outputCMCC$daily[,i], main=colnames(outputCMCC$daily[i]))
dev.off()

#output model MONTHLY
pdf("Monthly.pdf",onefile = T,width = 15,height = 12)
par(mfrow=c(4,4))
for (i in 1:length(outputCMCC$monthly)) plot(outputCMCC$monthly$Date,outputCMCC$monthly[,i], main=colnames(outputCMCC$monthly[i]),type = "l")
dev.off()

#output model ANNUAL
pdf("Annual.pdf",onefile = T,width = 15,height = 12)
par(mfrow=c(4,4))
for (i in 1:length(outputCMCC$annual)) plot(outputCMCC$annual$Date,outputCMCC$annual[,i], main=colnames(outputCMCC$annual[i]),type="l")
dev.off()
