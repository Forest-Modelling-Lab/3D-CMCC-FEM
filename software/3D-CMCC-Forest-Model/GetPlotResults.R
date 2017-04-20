version="5.3.2-ISIMIP"


# single_simulation<-readline(prompt="Single smulation on or off?")
# print(single_simulation)

#single simulation
site="Soroe"
esm="1"
rcp="8p5"
man="off"
co2="on"

#multiple simulations
site_list<-c("Soroe", "Hyytiala")
esm_list<-c("1","2","3","4","5")
rcp_list<-c("2p6", "4p5", "6p0", "8p5")
man_list<-c("on", "off")
co2_list<-c("on", "off")

setwd("/home/alessio-cmcc/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model")
if (single_simulation == "on"){
    "ciao"
    systemCall  <- paste0("Debug/3D_CMCC_Forest_Model", " ",
                          "-i"," ", "input/", paste0(site), " ",
                          "-p"," ", "input/parameterization", " ",
                          "-o"," ", "output/prova_Rstudio-", paste0(site), " ",
                          "-d"," ", "PAPER/", paste0(site),"_stand_ISIMIP.txt", " ",
                          "-m"," ", "PAPER/", "GCM", paste0(esm),"/", "GCM", paste0(esm),"_", "rcp", paste0(rcp), ".txt", " ",
                          "-s"," ", "PAPER/", paste0(site),"_soil_ISIMIP.txt", " ",
                          "-t"," ", "PAPER/", paste0(site),"_topo_ISIMIP.txt", " ",
                          "-c"," ", "PAPER/", paste0(site),"_settings_ISIMIP_Manag-", paste0(man), "_CO2-", paste0(co2),".txt", " ",
                          "-k"," ", "PAPER/CO2/CO2_", "rcp",paste0(rcp), "_1950_2099.txt")
    # launch execution
    system(systemCall)
    
}else{
    for (e in 1: length(site_list))
    {
        for(f in 1: length(esm_list))
        {
            for(g in 1: length(rcp_list))
            {
                for (h in 1: length(man_list))
                {
                    for (i in 1: length(co2_list))
                    {
                        systemCall  <- paste0("Debug/3D_CMCC_Forest_Model", " ",
                                              "-i"," ", "input/", paste0(site_list[i]), " ",
                                              "-p"," ", "input/parameterization", " ",
                                              "-o"," ", "output/prova_Rstudio-", paste0(site_list[i]), " ",
                                              "-d"," ", "PAPER/", paste0(site_list[i]),"_stand_ISIMIP.txt", " ",
                                              "-m"," ", "PAPER/", "GCM", paste0(esm_list[f]),"/", "GCM", paste0(esm_list[f]),"_", "rcp", paste0(rcp_list[g]), ".txt", " ",
                                              "-s"," ", "PAPER/", paste0(site_list[i]),"_soil_ISIMIP.txt", " ",
                                              "-t"," ", "PAPER/", paste0(site_list[i]),"_topo_ISIMIP.txt", " ",
                                              "-c"," ", "PAPER/", paste0(site_list[i]),"_settings_ISIMIP_Manag-", paste0(man_list[h]), "_CO2-", paste0(co2_list[i]),".txt", " ",
                                              "-k"," ", "PAPER/CO2/CO2_", "rcp", paste0(rcp_list[g]), "_1950_2099.txt")
                        # launch execution
                        system(systemCall)
                    }
                }
            }
        }
    }
}


outputCMCC<- list()


annual <- list.files(paste0("./output/prova_Rstudio-", site), pattern = "annual",recursive = TRUE, full.names = TRUE)
monthly <- list.files(paste0("./output/prova_Rstudio-", site), pattern = "monthly",recursive = TRUE, full.names = TRUE)
daily <- list.files(paste0("./output/prova_Rstudio-", site), pattern = "daily",recursive = TRUE, full.names = TRUE)

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
pdf(paste0("./output/prova_Rstudio-", site,"/output_", paste0(version), "_2017_APRIL_20/daily/Daily.pdf"), onefile = T,width = 15,height = 12)
par(mfrow=c(4,4))
for (i in 5:length(outputCMCC$daily)) plot(outputCMCC$daily$Date, outputCMCC$daily[,i], main=colnames(outputCMCC$daily[i]))
dev.off()

#output model MONTHLY
pdf(paste0("./output/prova_Rstudio-", site,"/output_", paste0(version), "_2017_APRIL_20/monthly/Monthly.pdf"), onefile = T,width = 15,height = 12)
par(mfrow=c(4,4))
for (i in 4:length(outputCMCC$monthly)) plot(outputCMCC$monthly$Date,outputCMCC$monthly[,i], main=colnames(outputCMCC$monthly[i]),type = "l")
dev.off()

#output model ANNUAL
pdf(paste0("./output/prova_Rstudio-", site, "/output_", paste0(version), "_2017_APRIL_20/annual/Annual.pdf"), onefile = T,width = 15,height = 12)
par(mfrow=c(4,4))
for (i in 3:length(outputCMCC$annual)) plot(outputCMCC$annual$Date,outputCMCC$annual[,i], main=colnames(outputCMCC$annual[i]),type="l")
dev.off()

# create annual Height plot
# dev.new()
# plot(outputCMCC$annual$Date,outputCMCC$annual[,3], main = colnames(outputCMCC$annual[3]),type = "l")

# create annual DBH plot
# dev.new()
# plot(outputCMCC$annual$Date,outputCMCC$annual[,4], main = colnames(outputCMCC$annual[4]),type = "l")

# create annual GPP plot
dev.new()
plot(outputCMCC$annual$Date,outputCMCC$annual[,"GPP"], main = colnames(outputCMCC$annual[8]),type = "l")

# # create annual RA plot
dev.new()
plot(outputCMCC$annual$Date,outputCMCC$annual[,"RA"], main = colnames(outputCMCC$annual[11]),type = "l")

# # create annual NPP plot
dev.new()
plot(outputCMCC$annual$Date,outputCMCC$annual[,"NPP"], main = colnames(outputCMCC$annual[12]),type = "l")

# # create annual CUE plot
dev.new()
plot(outputCMCC$annual$Date,outputCMCC$annual[,"CUE"], main = colnames(outputCMCC$annual[13]),type = "l")

# # create annual LAI plot
dev.new()
plot(outputCMCC$annual$Date,outputCMCC$annual[,"PeakLAI"], main = colnames(outputCMCC$annual[15]),type = "l")

