
setwd("/home/alessio-cmcc/git/3D-CMCC-LAND/software")
systemCall  <- paste0("3D-CMCC-Forest-Model/3D_CMCC_Forest_Model"," ",
                      "-i"," ", "3D-CMCC-Forest-Model/input/Soroe"," ",
                      "-p"," ", "3D-CMCC-Forest-Model/input/parameterization"," ",
                      "-o"," ", "3D-CMCC-Forest-Model/output/prova_Rstudio"," ",
                      "-d"," ", "PAPER/Soroe_stand_ISIMIP.txt", " ",
                      "-m"," ", "PAPER/GCM1/GCM1_rcp8p5.txt"," ",
                      "-s"," ", "PAPER/Soroe_soil_ISIMIP.txt"," ",
                      "-t"," ", "PAPER/Soroe_topo_ISIMIP.txt"," ",
                      "-c"," ", "PAPER/Soroe_settings_ISIMIP_Manag-off_CO2-on.txt"," ",
                      "-k"," ", "PAPER/CO2/CO2_rcp8p5_1950_2099.txt")
system(systemCall)

outputCMCC<- list()

annual <- list.files("./3D-CMCC-Forest-Model/output/prova_Rstudio/", pattern = "annual",recursive = TRUE, full.names = TRUE)
monthly <- list.files("./3D-CMCC-Forest-Model/output/prova_Rstudio/", pattern = "monthly",recursive = TRUE, full.names = TRUE)
daily <- list.files("./3D-CMCC-Forest-Model/output/prova_Rstudio/", pattern = "daily",recursive = TRUE, full.names = TRUE)

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
# pdf("./3D-CMCC-Forest-Model/output/prova_Rstudio/output_5.3.2-ISIMIP_2017_APRIL_20/daily/Daily.pdf",onefile = T,width = 15,height = 12)
# par(mfrow=c(4,4))
# for (i in 1:length(outputCMCC$daily)) plot(outputCMCC$daily$Date, outputCMCC$daily[,i], main=colnames(outputCMCC$daily[i]))
# dev.off()

#output model MONTHLY
pdf("./3D-CMCC-Forest-Model/output/prova_Rstudio/output_5.3.2-ISIMIP_2017_APRIL_20/monthly/Monthly.pdf",onefile = T,width = 15,height = 12)
par(mfrow=c(4,4))
for (i in 1:length(outputCMCC$monthly)) plot(outputCMCC$monthly$Date,outputCMCC$monthly[,i], main=colnames(outputCMCC$monthly[i]),type = "l")
dev.off()

#output model ANNUAL
pdf("./3D-CMCC-Forest-Model/output/prova_Rstudio/output_5.3.2-ISIMIP_2017_APRIL_20/annual/Annual.pdf",onefile = T,width = 15,height = 12)
par(mfrow=c(4,4))
for (i in 1:length(outputCMCC$annual)) plot(outputCMCC$annual$Date,outputCMCC$annual[,i], main=colnames(outputCMCC$annual[i]),type="l")
dev.off()

# create annual GPP plot
dev.new()
plot(outputCMCC$annual$Date,outputCMCC$annual[,8], main = colnames(outputCMCC$annual[8]))
# plot(outputCMCC$annual$Date,outputCMCC$annual[,8], main=colnames(outputCMCC$annual[8]),type = "l")
# # create annual RA plot
dev.new()
plot(outputCMCC$annual$Date,outputCMCC$annual[,11], main = colnames(outputCMCC$annual[11]))
# plot(outputCMCC$annual$Date,outputCMCC$annual[,11], main=colnames(outputCMCC$annual[11]),type = "l")
# # create annual NPP plot
dev.new()
plot(outputCMCC$annual$Date,outputCMCC$annual[,12], main = colnames(outputCMCC$annual[12]))
# plot(outputCMCC$annual$Date,outputCMCC$annual[,12], main=colnames(outputCMCC$annual[12]),type = "l")
# # create annual LAI plot
dev.new()
plot(outputCMCC$annual$Date,outputCMCC$annual[,15], main = colnames(outputCMCC$annual[15]))
# plot(outputCMCC$annual$Date,outputCMCC$annual[,15], main=colnames(outputCMCC$annual[15]),type = "l")
