# Developed by:
# Alessio Collalti (alessio.collalti@cmcc.it)
# Carlo Trotta (trottacarlo@unitus.it)
# Gaetano Pellicone (gaetano.pellicone@isafom.cnr.it)
# starting date: 18 April 2017
library(ggplot2)
library(cowplot)

model<-"3D-CMCC-FEM"
version="5.3.2-ISIMIP"

print("*******************************************************")
print(paste0("* ", model, " version: ", version, " R script *"))
print("*******************************************************")

Sys.setlocale(category = "LC_TIME", locale="en_GB.UTF-8") 

#  date
today <- Sys.Date()
day=format(today, format="%d")
month=format(today, format="%B")
year=format(today, format="%Y")
month=toupper(month)

# workig directory
# setwd('/home/alessio-cmcc/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/')
getwd()

#  output folder name
output_folder="output_Rstudio"

# multiple simulations
site_list<-c("Soroe")#,"Soroe","Hyytiala","All")
esm_list <-c("2")# ("1","2","3","4","5")
rcp_list <-c("2p6")# ("0p0","2p6","4p5","6p0","8p5")
man_list <-c("off")# ("on",'off')
co2_list <-c("on")# , "off")
protocol_list<-c("2B")# ("2A","2B") 

## a way to time an R expression: system.time is preferred
start.time <- Sys.time()

for (protocol in protocol_list) {
    for (site in site_list) {
        for ( esm in esm_list) {
            for (rcp in rcp_list) {
                for (man in man_list) {
                    for (co2 in co2_list) {
                        #  create arguments
                        dir.create(paste0("./output/",output_folder,"-", version, "-", site,"-",protocol),showWarnings = FALSE)
                        
                        cat(paste0("\nstart", model," ",version," ","protocol: ",protocol, " site: ", site, 
                                   " ESM: ", esm," RCP: ", rcp," Manag-", man, " CO2-", co2,'\n'))
                        
                        systemCall  <- paste0("Debug/3D_CMCC_Forest_Model", " ",
                                              "-i"," ", "input/", site, " ",
                                              "-p"," ", "input/parameterization", " ",
                                              "-o"," ", "output/",output_folder,"-", version, "-", site,"-",protocol," ",
                                              "-d"," ", "PAPER/", site,"_stand_ISIMIP.txt", " ",
                                              "-m"," ", "PAPER/", protocol, "/GCM", esm,"/", "GCM", esm,"_", "rcp", rcp, ".txt", " ",
                                              "-s"," ", "PAPER/", site,"_soil_ISIMIP.txt", " ",
                                              "-t"," ", "PAPER/", site,"_topo_ISIMIP.txt", " ",
                                              "-c"," ", "PAPER/", protocol, "/", site,"_settings_ISIMIP_Manag-", man, "_CO2-", co2,".txt", " ",
                                              "-k"," ", "PAPER/", protocol,"/CO2/CO2_", "rcp",rcp, ".txt"
                        )
                        # launch execution
                        system(systemCall)
                        outputCMCC<- list()
                        
                        cat(paste0("start 3D-CMCC ",
                                   "protocol: ",protocol, " site: ", site, 
                                   " ESM: ", esm," RCP: ", rcp," Manag-", man, " CO2-", co2,' ... COMPLETE!\n'))
                        
                        # read start and end_year from setting file
                        fid = file(paste0(getwd(),"/input/",site,"/PAPER/", protocol, "/", site,"_settings_ISIMIP_Manag-", 
                                          man, "_CO2-", co2,".txt"),'r')
                        settings = readLines(fid)
                        close(fid)
                        rm(fid)
                        
                        start_year = unlist(strsplit(settings[grep('^YEAR_START ',settings)],' '))[2]
                        end_year = unlist(strsplit(settings[grep('^YEAR_END ',settings)],' '))[2]
                        
                        rm(settings)
                        
                        all_out_files = list.files(paste0("output/",output_folder,"-", version, "-", site,"-",protocol,"/"), 
                                                   pattern = paste0(version,'_',site,'_GCM',esm,'_rcp',rcp,
                                                                    '.txt_'),
                                                   recursive = TRUE, full.names = TRUE)
                        
                        list_time = c('annual','monthly','daily')
                        
                        for (cy_time in list_time) {
                            for (f in all_out_files) {
                                
                                file_name = unlist(strsplit(f,'/'))
                                file_name = file_name[length(file_name)]
                                
                                # plot only the last results
                                ck_f = 0
                                if ( paste0(cy_time,'_',version,'_',site,'_GCM',esm,'_rcp',rcp,'.txt_(',start_year,'-',end_year,')_CO2_',toupper(co2),'_CO2_rcp',rcp,'.txt_Man_',toupper(man),'_d_10000_txt.txt') == file_name ) {
                                    ck_f = 1
                                }
                                
                                if (ck_f == 0) {
                                    next
                                }
                                
                                cat(paste0('\nImport file: ',f,'\n'))
                                
                                # read file
                                outputCMCC <-read.csv(f,header=T,comment.char = "#")
                                
                                if ( grepl('^annual_',file_name) ) {
                                    outputCMCC$Date <- paste0("01/01/",outputCMCC$YEAR)
                                    outputCMCC$Date <- as.Date(outputCMCC$Date, format = "%d/%m/%Y")
                                    start_col = 3
                                }
                                if ( grepl('^monthly_',file_name) ) {
                                    outputCMCC$Date <- paste0("01/",outputCMCC$MONTH, "/",outputCMCC$YEAR)
                                    outputCMCC$Date <- as.Date(outputCMCC$Date, format = "%d/%m/%Y")
                                    start_col = 4
                                }
                                if ( grepl('^daily_',file_name) ) {
                                    outputCMCC$Date <- paste0(outputCMCC$DAY,"/",outputCMCC$MONTH, "/",outputCMCC$YEAR)
                                    outputCMCC$Date <- as.Date(outputCMCC$Date, format = "%d/%m/%Y")
                                    start_col = 3
                                }
                                
                                # plot output model 
                                pdf(gsub('_txt.txt','_txt.pdf',f), onefile = T, width = 30,height = 24)
                                par(mfrow=c(4,4))
                                for (i in seq(start_col,length(outputCMCC)) ) {
                                    plot(outputCMCC$Date, outputCMCC[,i], main=colnames(outputCMCC[i]), 
                                         col="black", xlab = "year", ylab= "unit", type = "l", col.lab="red", cex.lab=2, cex.axis=2, cex.main=2, pch =30)
                                } 
                                dev.off()
                                cat(paste0('\n',gsub('_txt.txt','_txt.pdf',f),'\n'))
                                
                                rm(file_name,outputCMCC,start_col)
                            }
                            rm(f)
                        }
                        rm(all_out_files,end_year,start_year,list_time,cy_time)
                    }
                    rm(co2)
                }
                rm(man)
            }
            rm(rcp)
        }
        rm(esm)
    }
    rm(site)
}
rm(protocol)
end.time <- Sys.time()
print(end.time - start.time)


#     # # create annual GPP plot
#     # dev.new()
#     # plot(outputCMCC$annual$Date,outputCMCC$annual[,"GPP"], main = colnames(outputCMCC$annual[8]), col="red", xlab = "year", type = "l")
#     # 
#     # # create annual RA plot
#     # dev.new()
#     # plot(outputCMCC$annual$Date,outputCMCC$annual[,"RA"], main = colnames(outputCMCC$annual[11]), col="red", xlab = "year", type = "l")
#     # 
#     # # create annual NPP plot
#     # dev.new()
#     # plot(outputCMCC$annual$Date,outputCMCC$annual[,"NPP"], main = colnames(outputCMCC$annual[12]), col="red", xlab = "year", type = "l")
#     # 
#     # # create annual CUE plot
#     # dev.new()
#     # plot(outputCMCC$annual$Date,outputCMCC$annual[,"CUE"], main = colnames(outputCMCC$annual[13]), col="red", xlab = "year", type = "l")
#     # 
#     # # create annual LAI plot
#     # dev.new()
#     # plot(outputCMCC$annual$Date,outputCMCC$annual[,"PeakLAI"], main = colnames(outputCMCC$annual[15]), xlab = "year", type = "l")
# )
# pt = c()
# 
# p1 = ggplot(data=outputCMCC_tot)
# g1 = ggdraw()
# for (ip in outputCMCC_tot) {
#   p1 = p1 + geom_line(aes(x=Date, y=ip, group = esm, colour = esm)) + ylab(colnames(ip))
#   g1 = g1 + draw_plot(
#     ggplot(data=outputCMCC_tot,aes(x=Date, y=ip, group = esm, colour = esm))
#     + ylab(colnames(ip)) + geom_line(),0,0,)
#   pt = c(pt,p1)
# }
# ggdraw() +
#   
# plot_grid(pt)
# plot_grid(p1)
#   }
# }

# 
# 
# }
# 
# 
#     annual <- list.files(paste0("./output/",output_folder,"-", version, "-", site,"-",protocol), pattern = ".txt",recursive = TRUE, full.names = TRUE)
#     
#     monthly <- list.files(paste0("./output/",output_folder,"-", version, "-", site,"-",protocol), pattern = "monthly",recursive = TRUE, full.names = TRUE)
#     daily <- list.files(paste0("./output/",output_folder,"-", version, "-", site,"-",protocol), pattern = "daily",recursive = TRUE, full.names = TRUE)
#     
#     
#     # tree
#     if (file.exists(annual)) outputCMCC[["annual"]]<-read.csv(annual,header=T,comment.char = "#")
#     if (file.exists(monthly)) outputCMCC[["monthly"]]<-read.csv(monthly,header=T,comment.char = "#")
#     if (file.exists(daily)) outputCMCC[["daily"]]<-read.csv(daily,header=T,comment.char = "#")
#     
#     # tree
#     outputCMCC$annual$Date <- paste0("01/01/",outputCMCC$annual$YEAR)
#     outputCMCC$annual$Date <- as.Date(outputCMCC$annual$Date, format = "%d/%m/%Y")
#     
#     outputCMCC$monthly$Date <- paste0("01/",outputCMCC$monthly$MONTH, "/",outputCMCC$monthly$YEAR)
#     outputCMCC$monthly$Date <- as.Date(outputCMCC$monthly$Date, format = "%d/%m/%Y")
#     
#     outputCMCC$daily$Date <- paste0(outputCMCC$daily$DAY,"/",outputCMCC$daily$MONTH, "/",outputCMCC$daily$YEAR)
#     outputCMCC$daily$Date <- as.Date(outputCMCC$daily$Date, format = "%d/%m/%Y")
#     
#     #****************************************************************************************************************************************************************************
#     
#     #output model TREE DAILY
#     pdf(paste0("./output/",output_folder, "-", version, "-", site,"-",protocol,"/output_", version,"_", year,"_", month,"_", day,"/daily/Daily_",site,"-GCM",esm,"-RCP-",rcp,"-Man-",man,"-Co2-",co2,".pdf"), onefile = T, width = 30,height = 24)
#     par(mfrow=c(5,5))
#     for (i in 5:length(outputCMCC$daily)) plot(outputCMCC$daily$Date, outputCMCC$daily[,i], main=colnames(outputCMCC$daily[i]), 
#                                                col="black", xlab = "year", ylab= "unit", type = "l", col.lab="red", cex.lab=2, cex.axis=1.5, cex.main=2, pch =30)
#     dev.off()
#     
#     #output model TREE MONTHLY
#     pdf(paste0("./output/",output_folder, "-", version, "-", site,"-",protocol,"/output_", version, "_", year,"_", month,"_", day,"/monthly/Monthly_",site,"-GCM",esm,"-RCP-",rcp,"-Man-",man,"-Co2-",co2,".pdf"), onefile = T, width = 30,height = 24)
#     par(mfrow=c(5,5))
#     for (i in 4:length(outputCMCC$monthly)) plot(outputCMCC$monthly$Date,outputCMCC$monthly[,i], main=colnames(outputCMCC$monthly[i]),
#                                                  col="black", xlab = "year", ylab= "unit", type = "l", col.lab="red", cex.lab=1.5, cex.axis=1.5, cex.main=2, pch =30)
#     dev.off()
#     
#     #output model TREE ANNUAL
#     pdf(paste0("./output/",output_folder, "-", version, "-", site,"-",protocol,"/output_", version, "_", year,"_", month,"_", day,"/annual/Annual_",site,"-GCM",esm,"-RCP-",rcp,"-Man-",man,"-Co2-",co2,".pdf"), onefile = T, width = 20,height = 16)
#     par(mfrow=c(5,5))
#     for (i in 3:length(outputCMCC$annual)) plot(outputCMCC$annual$Date,outputCMCC$annual[,i], main=colnames(outputCMCC$annual[i]), 
#                                                 col="black", xlab = "year", ylab= "unit", type = "l", col.lab="red", cex.lab=1.5, cex.axis=1.5, cex.main=2, pch =30)
#     dev.off()
#     
#     #****************************************************************************************************************************************************************************
#     
#     # # create annual GPP plot
#     # dev.new()
#     # plot(outputCMCC$annual$Date,outputCMCC$annual[,"GPP"], main = colnames(outputCMCC$annual[8]), col="red", xlab = "year", type = "l")
#     # 
#     # # create annual RA plot
#     # dev.new()
#     # plot(outputCMCC$annual$Date,outputCMCC$annual[,"RA"], main = colnames(outputCMCC$annual[11]), col="red", xlab = "year", type = "l")
#     # 
#     # # create annual NPP plot
#     # dev.new()
#     # plot(outputCMCC$annual$Date,outputCMCC$annual[,"NPP"], main = colnames(outputCMCC$annual[12]), col="red", xlab = "year", type = "l")
#     # 
#     # # create annual CUE plot
#     # dev.new()
#     # plot(outputCMCC$annual$Date,outputCMCC$annual[,"CUE"], main = colnames(outputCMCC$annual[13]), col="red", xlab = "year", type = "l")
#     # 
#     # # create annual LAI plot
#     # dev.new()
#     # plot(outputCMCC$annual$Date,outputCMCC$annual[,"PeakLAI"], main = colnames(outputCMCC$annual[15]), xlab = "year", type = "l")
#     
# }else{
#     
#     # multiple simulations
#     site_list<-c("Soroe","Hyytiala","All")
#     esm_list <-c("1","2","3","4","5","All")
#     rcp_list <-c("2p6","4p5","6p0","8p5","All")
#     man_list <-c("on", "off","All")
#     co2_list <-c("on", "off","All")
#     protocol_list<-c("2A","2B","All") 
#     
#     match<-"yes"
#     cat("Available site(s):\n")
#     for (i in 1: length(site_counter<-table(site_list))) cat(paste0('[',i,'] ',site_list[i],'\n'))
#     input_site<-readline(prompt="Which site?")
#     
#     while (match == "yes"){
#         for (i in 1: length(site_counter<-table(site_list)))  
#         {
#             if (input_site == site_list[i]) 
#             {
#                 match<-"yes" 
#                 print("ok site!")
#             }
#             else 
#             {
#                 match<-"no"
#                 print("try again!")
#             }
#         }
#     }
#     
#     #  count number of elements
#     if (site == "All")site_counter<-table(site_list)  else site_counter=1
#     if (esm == "All")esm_counter<-table(esm_list)  else esm_counter=1
#     if (rcp == "All")rcp_counter<-table(rcp_list)  else rcp_counter=1
#     if (man == "All")man_counter<-table(man_list)  else man_counter=1
#     if (co2 == "All")co2_counter<-table(co2_list)  else co2_counter=1
#     if (protocol == "All")protocol_counter<-table(protocol_list)  else protocol_counter=1
#     
#     for (e in 1: length(site_counter))
#     {
#         print(site_list[e])
#         for(f in 1: length(esm_counter))
#         {
#             print(esm_list[f])
#             for(g in 1: length(rcp_counter))
#             {
#                 print(rcp_list[g])
#                 for (h in 1: length(man_counter))
#                 {
#                     print(man_list[h])
#                     for (i in 1: length(co2_counter))
#                     {
#                         print(co2_list[i])
#                         
#                         #  create arguments
#                         systemCall  <- paste0("Debug/3D_CMCC_Forest_Model", " ",
#                                               "-i"," ", "input/", site_list[e], " ",
#                                               "-p"," ", "input/parameterization", " ",
#                                               "-o"," ", "output/",output_folder, "-", version, "-", site_list[e],"-",protocol," ",
#                                               "-d"," ", "PAPER/", site_list[e],"_stand_ISIMIP.txt", " ",
#                                               "-m"," ", "PAPER/", "GCM", esm_list[f],"/", "GCM", esm_list[f],"_", "rcp", rcp_list[g], ".txt", " ",
#                                               "-s"," ", "PAPER/", site_list[e],"_soil_ISIMIP.txt", " ",
#                                               "-t"," ", "PAPER/", site_list[e],"_topo_ISIMIP.txt", " ",
#                                               "-c"," ", "PAPER/", site_list[e],"_settings_ISIMIP_Manag-", man_list[h], "_CO2-", co2_list[i],".txt", " ",
#                                               "-k"," ", "PAPER/CO2/CO2_", "rcp", rcp_list[g], "_1950_2099.txt")
#                         
#                         # launch execution
#                         system(systemCall)
#                         
#                         # tree
#                         annual <- list.files(paste0("./output/",output_folder,"-", version, "-", site,"-",protocol), pattern = "annual",recursive = TRUE, full.names = TRUE)
#                         monthly <- list.files(paste0("./output/",output_folder,"-", version, "-", site,"-",protocol), pattern = "monthly",recursive = TRUE, full.names = TRUE)
#                         daily <- list.files(paste0("./output/",output_folder,"-", version, "-", site,"-",protocol), pattern = "daily",recursive = TRUE, full.names = TRUE)
#                         
#                         outputCMCC$annual$Date <- paste0("01/01/",outputCMCC$annual$YEAR)
#                         outputCMCC$annual$Date <- as.Date(outputCMCC$annual$Date, format = "%d/%m/%Y")
#                         
#                         outputCMCC$monthly$Date <- paste0("01/",outputCMCC$monthly$MONTH, "/",outputCMCC$monthly$YEAR)
#                         outputCMCC$monthly$Date <- as.Date(outputCMCC$monthly$Date, format = "%d/%m/%Y")
#                         
#                         outputCMCC$daily$Date <- paste0(outputCMCC$daily$DAY,"/",outputCMCC$daily$MONTH, "/",outputCMCC$daily$YEAR)
#                         outputCMCC$daily$Date <- as.Date(outputCMCC$daily$Date, format = "%d/%m/%Y")
#                         
#                         #output model DAILY
#                         pdf(paste0("./output/",output_folder, "-", version, "-", site,"-",protocol,"/output_", version,"_", year,"_", month,"_", day,"/daily/Daily.pdf"), onefile = T,width = 15,height = 12)
#                         par(mfrow=c(4,4))
#                         for (i in 5:length(outputCMCC$daily)) plot(outputCMCC$daily$Date, outputCMCC$daily[,i], main=colnames(outputCMCC$daily[i]))
#                         dev.off()
#                         
#                         #output model MONTHLY
#                         pdf(paste0("./output/",output_folder, "-", version, "-", site,"-",protocol,"/output_", version,"_", year,"_", month,"_", day,"/monthly/Monthly.pdf"), onefile = T,width = 15,height = 12)
#                         par(mfrow=c(4,4))
#                         for (i in 4:length(outputCMCC$monthly)) plot(outputCMCC$monthly$Date,outputCMCC$monthly[,i], main=colnames(outputCMCC$monthly[i]),type = "l")
#                         dev.off()
#                         
#                         #output model ANNUAL
#                         pdf(paste0("./output/",output_folder, "-", version, "-", site,"-",protocol,"/output_", version,"_", year,"_", month,"_", day,"/annual/Annual.pdf"), onefile = T,width = 15,height = 12)
#                         par(mfrow=c(4,4))
#                         for (i in 3:length(outputCMCC$annual)) plot(outputCMCC$annual$Date,outputCMCC$annual[,i], main=colnames(outputCMCC$annual[i]),type="l")
#                     }
#                     # here analyze at co2 level
#                 }
#                 # here analyze at man level
#             }
#             # here analyze at rcp level
#         }
#         # here analyze at esm level
#     }
#     # here analyze at site level
# }

