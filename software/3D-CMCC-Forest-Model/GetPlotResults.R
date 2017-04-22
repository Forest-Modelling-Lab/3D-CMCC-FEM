Sys.setlocale(category = "LC_TIME", locale="en_GB.UTF-8") 

version="5.3.2-ISIMIP"

single_simulation="on"
# single_simulation<-readline(prompt="Single smulation on or off?")
# print(single_simulation)

#  output folder name
output_folder="output_Rstudio"

# single simulation
site="Soroe"
esm="2"
rcp="0p0"
man="off"
co2="off"
protocol="2B"

# multiple simulations
site_list<-c("Soroe","Hyytiala")
esm_list <-c("1","2","3","4","5")
rcp_list <-c("2p6","4p5","6p0","8p5")
man_list <-c("on", "off")
co2_list <-c("on", "off")

#  date
today <- Sys.Date()
day=format(today, format="%d")
month=format(today, format="%B")
year=format(today, format="%Y")
month=toupper(month)

setwd("/home/alessio-cmcc/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model")
if (single_simulation == "on"){
  #  create arguments
  systemCall  <- paste0("Debug/3D_CMCC_Forest_Model", " ",
                        "-i"," ", "input/", site, " ",
                        "-p"," ", "input/parameterization", " ",
                        "-o"," ", "output/",output_folder,"-", version,"-",protocol,"-", site, " ",
                        "-d"," ", "PAPER/", site,"_stand_ISIMIP.txt", " ",
                        "-m"," ", "PAPER/", protocol, "/GCM", esm,"/", "GCM", esm,"_", "rcp", rcp, ".txt", " ",
                        "-s"," ", "PAPER/", site,"_soil_ISIMIP.txt", " ",
                        "-t"," ", "PAPER/", site,"_topo_ISIMIP.txt", " ",
                        "-c"," ", "PAPER/", protocol, "/", site,"_settings_ISIMIP_Manag-", man, "_CO2-", co2,".txt", " ",
                        "-k"," ", "PAPER/", protocol,"/CO2/CO2_", "rcp",rcp, "_1950_2099.txt")
  # launch execution
  system(systemCall)
  outputCMCC<- list()
  
  # tree
  annual <-  list.files(paste0("./output/",output_folder,"-", version,"-",protocol,"-", site), pattern = "annual",recursive = TRUE, full.names = TRUE)
  monthly <- list.files(paste0("./output/",output_folder,"-", version,"-",protocol,"-", site), pattern = "monthly",recursive = TRUE, full.names = TRUE)
  daily <-   list.files(paste0("./output/",output_folder,"-", version,"-",protocol,"-", site), pattern = "daily",recursive = TRUE, full.names = TRUE)

  # tree
  if (file.exists(annual)) outputCMCC[["annual"]]<-read.csv(annual,header=T,comment.char = "#")
  if (file.exists(monthly)) outputCMCC[["monthly"]]<-read.csv(monthly,header=T,comment.char = "#")
  if (file.exists(daily)) outputCMCC[["daily"]]<-read.csv(daily,header=T,comment.char = "#")

  # tree
  outputCMCC$annual$Date <- paste0("01/01/",outputCMCC$annual$YEAR)
  outputCMCC$annual$Date <- as.Date(outputCMCC$annual$Date, format = "%d/%m/%Y")
  
  outputCMCC$monthly$Date <- paste0("01/",outputCMCC$monthly$MONTH, "/",outputCMCC$monthly$YEAR)
  outputCMCC$monthly$Date <- as.Date(outputCMCC$monthly$Date, format = "%d/%m/%Y")
  
  outputCMCC$daily$Date <- paste0(outputCMCC$daily$DAY,"/",outputCMCC$daily$MONTH, "/",outputCMCC$daily$YEAR)
  outputCMCC$daily$Date <- as.Date(outputCMCC$daily$Date, format = "%d/%m/%Y")
  
  #****************************************************************************************************************************************************************************
  
  #output model TREE DAILY
  pdf(paste0("./output/",output_folder, "-",version,"-",protocol,"-", site,"/output_", version,"_", year,"_", month,"_", day,"/daily/Daily.pdf"), onefile = T, width = 30,height = 24)
  par(mfrow=c(5,5))
  for (i in 5:length(outputCMCC$daily)) plot(outputCMCC$daily$Date, outputCMCC$daily[,i], main=colnames(outputCMCC$daily[i]), col="black", xlab = "year", ylab= "unit", type = "l")
  dev.off()
  
  #output model TREE MONTHLY
  pdf(paste0("./output/",output_folder, "-",version,"-",protocol,"-", site,"/output_", version, "_", year,"_", month,"_", day,"/monthly/Monthly.pdf"), onefile = T, width = 30,height = 24)
  par(mfrow=c(5,5))
  for (i in 4:length(outputCMCC$monthly)) plot(outputCMCC$monthly$Date,outputCMCC$monthly[,i], main=colnames(outputCMCC$monthly[i]), col="black", xlab = "year", ylab= "unit", type = "l")
  dev.off()
  
  #output model TREE ANNUAL
  pdf(paste0("./output/",output_folder, "-", version,"-",protocol,"-", site,"/output_", version, "_", year,"_", month,"_", day,"/annual/Annual.pdf"), onefile = T, width = 20,height = 16)
  par(mfrow=c(5,5))
  for (i in 3:length(outputCMCC$annual)) plot(outputCMCC$annual$Date,outputCMCC$annual[,i], main=colnames(outputCMCC$annual[i]), col="black", xlab = "year", ylab= "unit", type = "l")
  dev.off()

  #****************************************************************************************************************************************************************************
  
  # create annual GPP plot
  dev.new()
  plot(outputCMCC$annual$Date,outputCMCC$annual[,"GPP"], main = colnames(outputCMCC$annual[8]), col="red", xlab = "year", type = "l")

  # create annual RA plot
  dev.new()
  plot(outputCMCC$annual$Date,outputCMCC$annual[,"RA"], main = colnames(outputCMCC$annual[11]), col="red", xlab = "year", type = "l")
 
  # create annual NPP plot
  dev.new()
  plot(outputCMCC$annual$Date,outputCMCC$annual[,"NPP"], main = colnames(outputCMCC$annual[12]), col="red", xlab = "year", type = "l")

  # create annual CUE plot
  dev.new()
  plot(outputCMCC$annual$Date,outputCMCC$annual[,"CUE"], main = colnames(outputCMCC$annual[13]), col="red", xlab = "year", type = "l")

  # create annual LAI plot
  dev.new()
  plot(outputCMCC$annual$Date,outputCMCC$annual[,"PeakLAI"], main = colnames(outputCMCC$annual[15]), xlab = "year", type = "l")
  
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
            #  create arguments
            systemCall  <- paste0("Debug/3D_CMCC_Forest_Model", " ",
                                  "-i"," ", "input/", site_list[e], " ",
                                  "-p"," ", "input/parameterization", " ",
                                  "-o"," ", "output/",output_folder, "-", version, "-", site_list[e], " ",
                                  "-d"," ", "PAPER/", site_list[e],"_stand_ISIMIP.txt", " ",
                                  "-m"," ", "PAPER/", "GCM", esm_list[f],"/", "GCM", esm_list[f],"_", "rcp", rcp_list[g], ".txt", " ",
                                  "-s"," ", "PAPER/", site_list[e],"_soil_ISIMIP.txt", " ",
                                  "-t"," ", "PAPER/", site_list[e],"_topo_ISIMIP.txt", " ",
                                  "-c"," ", "PAPER/", site_list[e],"_settings_ISIMIP_Manag-", man_list[h], "_CO2-", co2_list[i],".txt", " ",
                                  "-k"," ", "PAPER/CO2/CO2_", "rcp", rcp_list[g], "_1950_2099.txt")
            # launch execution
            system(systemCall)
            
            outputCMCC$annual$Date <- paste0("01/01/",outputCMCC$annual$YEAR)
            outputCMCC$annual$Date <- as.Date(outputCMCC$annual$Date, format = "%d/%m/%Y")
            
            outputCMCC$monthly$Date <- paste0("01/",outputCMCC$monthly$MONTH, "/",outputCMCC$monthly$YEAR)
            outputCMCC$monthly$Date <- as.Date(outputCMCC$monthly$Date, format = "%d/%m/%Y")
            
            outputCMCC$daily$Date <- paste0(outputCMCC$daily$DAY,"/",outputCMCC$daily$MONTH, "/",outputCMCC$daily$YEAR)
            outputCMCC$daily$Date <- as.Date(outputCMCC$daily$Date, format = "%d/%m/%Y")
            
            #output model DAILY
            pdf(paste0("./output/",output_folder, "-", version, "-", site,"/output_", version,"_", year,"_", month,"_", day,"/daily/Daily.pdf"), onefile = T,width = 15,height = 12)
            par(mfrow=c(4,4))
            for (i in 5:length(outputCMCC$daily)) plot(outputCMCC$daily$Date, outputCMCC$daily[,i], main=colnames(outputCMCC$daily[i]))
            dev.off()
            
            #output model MONTHLY
            pdf(paste0("./output/",output_folder, "-", version, "-", site,"/output_", version,"_", year,"_", month,"_", day,"/monthly/Monthly.pdf"), onefile = T,width = 15,height = 12)
            par(mfrow=c(4,4))
            for (i in 4:length(outputCMCC$monthly)) plot(outputCMCC$monthly$Date,outputCMCC$monthly[,i], main=colnames(outputCMCC$monthly[i]),type = "l")
            dev.off()
            
            #output model ANNUAL
            pdf(paste0("./output/",output_folder, "-", version, "-", site,"/output_", version,"_", year,"_", month,"_", day,"/annual/Annual.pdf"), onefile = T,width = 15,height = 12)
            par(mfrow=c(4,4))
            for (i in 3:length(outputCMCC$annual)) plot(outputCMCC$annual$Date,outputCMCC$annual[,i], main=colnames(outputCMCC$annual[i]),type="l")
          }
          # here analyze at co2 level
        }
        # here analyze at man level
      }
      # here analyze at rcp level
    }
    # here analyze at esm level
  }
  # here analyze at site level
}



