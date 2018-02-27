# Developed by:
# Alessio Collalti (alessio.collalti@cmcc.it)
# Carlo Trotta (trottacarlo@unitus.it)
rm(list = ls())

# working directory
# setwd('/home/alessio-cmcc/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/')
# setwd(getwd())
#setwd('/home/alessio-cmcc/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/')
library(rstudioapi)

dir_in = dirname(rstudioapi::getActiveDocumentContext()$path)
setwd(dir_in)

library(ggplot2)
library(cowplot)
library(lubridate)
library(gridExtra)
library(matrixStats)
library(scales)
source("GetPlotResults_file.R")
source('import_all_out.R')
source('plot_ts.R')
source('plot_multi_lm.R')
source('removeInf.R')
source('colSums2.R')
source('flux_validation.R')
source('output_validation.R')

dir_in_ec = paste0(getwd(),'/eddy_validation/')
soglia_qc = 0.6
# file delle corrispondenze tra i nomi delle variabili del modello
# e quelle del db profound
file_db_corrisp = paste0(getwd(),'/stand_mod_vs_stand_profound.txt')
# file di stand di validazione
# file_stand = 'c:/Users/CMCC/Desktop/profound/extract2/STAND.csv'
file_stand = paste0(getwd(),'/STAND.csv')

model<-"3D-CMCC-BGC"
version="5.4"

print("*******************************************************")
print(paste0("* ", model, " version: ", version, " R script *"))
print("*******************************************************")

# Sys.setlocale(category = "LC_TIME", locale="en_GB.UTF-8") 
# 
# #  date
# today <- Sys.Date()
# day=format(today, format="%d")
# month=format(today, format="%B")
# year=format(today, format="%Y")
# month=toupper(month)
# 
# # working directory
# # setwd('/home/alessio-cmcc/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/')
# # setwd(getwd())
# setwd('/home/alessio/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/')

# number of plot for each pdf page
nr_plot_per_page = 24
nr_col_per_page  = 4

# list to rename output files
time_list_output = c('annual','monthly','daily')

# single or multiple simulations
build_list<-c('Debug')        #, 'Release')
site_list<-c("All")
esm_list <-c("All")             # ("1","2","3","4","5", "All")
rcp_list <-c("All")           # ("0p0","2p6","4p5","6p0","8p5","All")
man_list <-c("All")           # ("on",off","All")
co2_list <-c("All")            # ("on",off","All")
protocol_list<-c("LOCAL")        # ("2A","2B","All") 
local_list<-c('on')          # ("on","off") to include local simulation of not under FT protocol
climate_off_list<-c('off')    # ("on",off","All") for climate off and co2 on
photosynthesis_list<-c('FvCB') #("FvCB",'LUE') for photosyhtesis approach
#lue_list<-c("Fra")            #('FvCB','LUE') for lue approach
time_list = c('annual')
turnover_list = seq(0.7,1,by = 0.1)

#  output folder name
output_folder = paste0("TURNOVER_TEST_",co2_list)

if ( length(which(site_list == 'All')) > 0 ) {
  site_list = c("Soroe","Hyytiala","Bily_Kriz","LeBray","Solling_beech","Peitz","Solling_spruce")
}
if ( length(which(esm_list == 'All')) > 0 ) {
  esm_list = c("1","2","3","4","5","6","7","8","9","10")
}
if ( length(which(rcp_list == 'All')) > 0 ) {
  rcp_list = c("0p0","2p6","4p5","6p0","8p5")
}
if ( length(which(man_list == 'All')) > 0 ) {
  man_list = c("on",'off')
}
if ( length(which(co2_list == 'All')) > 0 ) {
  co2_list = c("on",'off')
}
if ( length(which(protocol_list == 'All')) > 0 ) {
  protocol_list = c('FT','2A','2B',"2BLBC","2Bpico","2BLBCpico")#'LOCAL',
}
if ( length(which(time_list == 'All')) > 0 ) {
  time_list = c('annual','monthly','daily')
}
if ( length(which(photosynthesis_list == 'All')) > 0 ) {
  photosynthesis_list = c('FvCB','LUE')
}
# if ( length(which(lue_list == 'All')) > 0 ) {
#   lue_list = c('Fra','Ver')
# }

# modify the parameterization
lista_file_param = c('Fagussylvatica','Castaneasativa','Larixdecidua','Piceaabies',
  'Pinuslaricio','Pinuspinaster','Pinussylvestris','Quercuscerris',
  'Quercusilex','Quercusrobur')
for ( cy_turnover_list in turnover_list ) {
  for (cy_lista_file_param in lista_file_param) {
    fid = file(paste0(getwd(),"/input/parameterization/",cy_lista_file_param,'.txt'))
    r_or = readLines(fid)
    close(fid)
    r_or = gsub('\t',' ',r_or)
    pos_or = grep('LIVE_WOOD_TURNOVER ',r_or)
    line_to_change = unlist(strsplit(r_or[pos_or],' '))
    pos_no_empty = which(nchar(line_to_change) > 0)
    # change original value
    line_to_change[pos_no_empty[2]] = cy_turnover_list
    # recreate parameter file
    r_or[pos_or] = paste(line_to_change,collapse = ' ')
    
    dir.create(paste0(getwd(),"/input/parameterization_LIVE_WOOD_TURNOVER_",cy_turnover_list,'/'),showWarnings = F)
    
    fid = file(paste0(getwd(),"/input/parameterization_LIVE_WOOD_TURNOVER_",cy_turnover_list,'/',
                      cy_lista_file_param,'.txt'))
    writeLines(text = r_or,con = fid)
    close(fid)
    
    # cat(sprintf('write file: %s\n',paste0(getwd(),"/input/parameterization_LIVE_WOOD_TURNOVER_",
    #       cy_turnover_list,'/',cy_lista_file_param,'.txt')))
    
    rm(fid,r_or,pos_or,line_to_change,pos_no_empty)
  }
}

## a way to time an R expression: system.time is preferred
run_model = 1

if ( run_model == 1 ) {
  start.time <- Sys.time()
  
  error_list = c()
  for (site in site_list) {
    for (man in man_list) {
      for (co2 in co2_list) {
        for (photosynthesis in photosynthesis_list) {
          for ( cy_turnover_list in turnover_list ) {
            
            dir.create(paste0(getwd(),"/output/"),showWarnings = FALSE)
            output_folder2 = paste0(output_folder,"_LIVE_WOOD_TURNOVER_",cy_turnover_list)
            
            protocol = 'LOCAL'
            # LOCAL simulation run in all cases
            
            if (local_list == 'on' || protocol_list == 'LOCAL')
            {
              # check if ALL input files exists
              files_to_check_id = 0
              files_to_check = c(
                paste0(site,"_stand_ISIMIP.txt"),
                paste0( site,"_stand_ISIMIP.txt"),
                paste0( protocol,"/",protocol, "_hist.txt"),
                paste0( site,"_soil_ISIMIP.txt"),
                paste0( site,"_topo_ISIMIP.txt"),
                paste0( protocol, "/PHOTOSYNTHESIS/", site,"_settings_ISIMIP_",photosynthesis,"_Manag-on_CO2-", co2,".txt"),
                paste0( "/CO2/CO2_hist.txt")
              )
              for ( zz in files_to_check ) {
                file_name_tmp = paste0(getwd(),'/input/',site,"/ISIMIP/",zz)
                list_ck = list.files(path = dirname(file_name_tmp),pattern = basename(file_name_tmp))
                
                if ( length(list_ck) == 0 ) {
                  str = sprintf('LOCAL experiment; missing file : %s\n',
                                file_name_tmp)
                  
                  error_list = c(error_list,str)
                  files_to_check_id = 1
                  rm(str)
                  break
                }
                rm(file_name_tmp)
              }
              
              if ( files_to_check_id == 0 ) {
                
                dir.create(paste0(getwd(),"/output/"),showWarnings = FALSE)
                dir.create(paste0(getwd(),"/output/",output_folder2,"-", version, "-", site),showWarnings = FALSE)
                dir.create(paste0(getwd(),"/output/",output_folder2,"-", version, "-", site,"/",protocol),showWarnings = FALSE)
                
                cat(paste0("\nstart ", model," ",version," ","protocol: ",protocol, " site: ", site,
                           " LIVE_WOOD_TURNOVER ",cy_turnover_list,'\n'))
                
                systemCall  <- paste0(build_list,'/3D_CMCC_Forest_Model', " ",
                                      "-i"," ", "input/", site, " ",
                                      "-p"," ", "input/parameterization_LIVE_WOOD_TURNOVER_",cy_turnover_list," ",
                                      "-o"," ", "output/",output_folder2,"-", version, "-", site,"/",protocol,"-",photosynthesis," ",
                                      "-d"," ", "ISIMIP/", site,"_stand_ISIMIP.txt", " ",
                                      "-m"," ", "ISIMIP/", protocol,"/",protocol, "_hist.txt", " ",
                                      "-s"," ", "ISIMIP/", site,"_soil_ISIMIP.txt", " ",
                                      "-t"," ", "ISIMIP/", site,"_topo_ISIMIP.txt", " ",
                                      "-c"," ", "ISIMIP/", protocol, "/PHOTOSYNTHESIS/", site,"_settings_ISIMIP_",photosynthesis,"_Manag-on_CO2-on.txt", " ",
                                      "-k"," ", "ISIMIP/", "/CO2/CO2_hist.txt",
                                      ">output/",output_folder2,"-", version, "-", site,"/",protocol,
                                      "_log_",site,"_LOCAL_",photosynthesis,"_Manag-on_CO2-", co2,
                                      "_LIVE_WOOD_TURNOVER_",cy_turnover_list,".txt"
                )
                # launch execution
                system(systemCall)
                outputCMCC<- list()
                cat(paste0("\n", model," ",version," ","protocol: ",protocol, " site: ", site,
                           " LIVE_WOOD_TURNOVER ",cy_turnover_list,'... COMPLETE!\n'))
              }
            }
            
            for (protocol in protocol_list) {
              if (protocol == 'LOCAL') {
                next
              }
              for ( esm in esm_list) {
                for (rcp in rcp_list) {
                  
                  files_to_check_id2 = 0
                  files_to_check = c(
                    paste0( site,"_stand_ISIMIP.txt"),
                    paste0( protocol, "/ESM", esm,"/", protocol,"_","ESM", esm,"_", "rcp", rcp, ".txt"),
                    paste0( site,"_soil_ISIMIP.txt"),
                    paste0( site,"_topo_ISIMIP.txt"),
                    paste0( protocol, "/PHOTOSYNTHESIS/", site,"_settings_ISIMIP_",photosynthesis,"_Manag-", man, "_CO2-", co2,".txt"),
                    paste0("/CO2/CO2_", "rcp",rcp, ".txt")
                  )
                  for ( zz in files_to_check ) {
                    file_name_tmp = paste0(getwd(),'/input/',site,"/ISIMIP/",zz)
                    list_ck = list.files(path = dirname(file_name_tmp),pattern = basename(file_name_tmp))
                    
                    if ( length(list_ck) == 0 ) {
                      str = sprintf('%s experiment; missing file : %s\n',
                                    protocol,
                                    paste0(getwd(),'/input/',site,"/ISIMIP/",zz))
                      
                      error_list = c(error_list,str)
                      files_to_check_id2 = 1
                      rm(str)
                      break
                    }
                    rm(file_name_tmp)
                  }
                  
                  if ( files_to_check_id2 == 0 ) {
                    
                    dir.create(paste0(getwd(),"/output/"),showWarnings = FALSE)
                    dir.create(paste0(getwd(),"/output/",output_folder2,"-", version, "-", site),showWarnings = FALSE)
                    dir.create(paste0(getwd(),"/output/",output_folder2,"-", version, "-", site,"/",protocol),showWarnings = FALSE)
                    
                    cat(paste0("start 3D-CMCC ",
                               "protocol: ",protocol, " site: ", site, 
                               " ESM: ", esm," RCP: ", rcp," CO2 RCP: ", rcp,'_',photosynthesis_list,
                               "_Manag-", man, " CO2-", co2,
                               "_LIVE_WOOD_TURNOVER_",cy_turnover_list,'\n'))
                    
                    systemCall  <- paste0(build_list,'/3D_CMCC_Forest_Model', " ",
                                          "-i"," ", "input/", site, " ",
                                          "-p"," ", "input/parameterization_LIVE_WOOD_TURNOVER_",cy_turnover_list," ",
                                          "-o"," ", "output/",output_folder2,"-", version, "-", site,"/",protocol,"-",photosynthesis," ",
                                          "-d"," ", "ISIMIP/", site,"_stand_ISIMIP.txt", " ",
                                          "-m"," ", "ISIMIP/", protocol, "/ESM", esm,"/", protocol,"_","ESM", esm,"_", "rcp", rcp, ".txt", " ",
                                          "-s"," ", "ISIMIP/", site,"_soil_ISIMIP.txt", " ",
                                          "-t"," ", "ISIMIP/", site,"_topo_ISIMIP.txt", " ",
                                          "-c"," ", "ISIMIP/", protocol, "/PHOTOSYNTHESIS/", site,"_settings_ISIMIP_",photosynthesis,"_Manag-", man, "_CO2-", co2,".txt", " ",
                                          "-k"," ", "ISIMIP/", "/CO2/CO2_", "rcp",rcp, ".txt",
                                          ">output/",output_folder2,"-", version, "-", site,"/",protocol,
                                          "_log_",site,"_",protocol, "_ESM_", esm,"_RCP_", rcp,"_CO2_RCP_",
                                          rcp,'_',photosynthesis,"_Manag-", man, "_CO2-", co2,
                                          "_LIVE_WOOD_TURNOVER_",cy_turnover_list,".txt"
                    )
                    
                    # launch execution
                    system(systemCall)
                    outputCMCC<- list()
                    
                    cat(paste0("start 3D-CMCC ",
                               "protocol: ",protocol, " site: ", site, 
                               " ESM: ", esm," RCP: ", rcp," CO2 RCP: ", rcp,'_',photosynthesis_list,
                               "_Manag-", man, " CO2-", co2,
                               "_LIVE_WOOD_TURNOVER_",cy_turnover_list,' ... COMPLETE!\n'))
                    
                    # THIS RUN FIXING CLIMATE TO CURRENT AND CHANGES CO2 CONCENTRATIONS
                    if ( rcp == '0p0' && co2 == 'on' && climate_off_list == 'on' )
                    {
                      for (rcp2 in rcp_list)
                      {
                        cat(paste0("start 3D-CMCC ",
                                   "protocol: ",protocol, " site: ", site, 
                                   " ESM: ", esm," RCP: ", rcp," CO2 RCP: ", rcp2," Manag-", man, " CO2-", co2,
                                   "_LIVE_WOOD_TURNOVER_",cy_turnover_list,'\n'))
                        
                        systemCall  <- paste0(build_list,'/3D_CMCC_Forest_Model', " ",
                                              "-i"," ", "input/", site, " ",
                                              "-p"," ", "input/parameterization_LIVE_WOOD_TURNOVER_",cy_turnover_list,
                                              "-o"," ", "output/",output_folder2,"-", version, "-", site,"/",protocol,"-",photosynthesis," ",
                                              "-d"," ", "ISIMIP/", site,"_stand_ISIMIP.txt", " ",
                                              "-m"," ", "ISIMIP/", protocol, "/ESM", esm,"/", protocol,"_","ESM", esm,"_", "rcp", rcp, ".txt", " ",
                                              "-s"," ", "ISIMIP/", site,"_soil_ISIMIP.txt", " ",
                                              "-t"," ", "ISIMIP/", site,"_topo_ISIMIP.txt", " ",
                                              "-c"," ", "ISIMIP/", protocol, "/PHOTOSYNTHESIS/", site,"_settings_ISIMIP_",photosynthesis,"_Manag-", man, "_CO2-", co2,".txt", " ",
                                              "-k"," ", "ISIMIP/", "/CO2/CO2_", "rcp",rcp2, ".txt",
                                              ">output/",output_folder2,"-", version, "-", site,"/",protocol,"_log_",site,"_",protocol,
                                              "_ESM_", esm,"_RCP_",rcp,"_CO2_RCP_",rcp2,'_',photosynthesis,"_Manag-",
                                              man, "_CO2-", co2,"_LIVE_WOOD_TURNOVER_",cy_turnover_list,".txt"
                        )
                        
                        # launch execution
                        system(systemCall)
                        outputCMCC<- list()
                        
                        cat(paste0("start 3D-CMCC ",
                                   "protocol: ",protocol, " site: ", site, 
                                   " ESM: ", esm," RCP: ", rcp," CO2 RCP: ", rcp2," Manag-", man, " CO2-", co2,
                                   "_LIVE_WOOD_TURNOVER_",cy_turnover_list,' ... COMPLETE!\n'))
                        
                      }
                    }
                  }
                }
                rm(rcp)
              }
              rm(esm)
            }
            rm(protocol)
            
            for (cy_time in time_list_output) {
              
              # list of the files to plot
              all_out_files = list.files(paste0("output/",output_folder2,"-", version, "-", site,"/"),
                                         recursive = TRUE, full.names = TRUE)
              
              if ( length(all_out_files) == 0) {
                next
              }
              
              all_out_files = all_out_files[grep(cy_time,all_out_files)]
              
              # exclude all PDF files
              if ( length( grep('.pdf',all_out_files) ) > 0 ) {
                all_out_files = all_out_files[-1*grep('.pdf',all_out_files)]
              }
              
              all_out_files2 = c()
              
              if ( length(all_out_files) == 0) {
                next
              }
              
              # local simulation are the first in the file list
              if ( length(grep('_hist',all_out_files)) > 0 ) {
                files_hist = all_out_files[grep('_hist',all_out_files)]
                all_out_files = all_out_files[-1*grep('_hist',all_out_files)]
                all_out_files = c(files_hist,all_out_files)
                rm(files_hist)
              }
              
              if ( sum(grepl('^Benchmark',basename(all_out_files))) == 0  ) {
                pos = 1
                if( file.exists(paste0(getwd(),'/',dirname(all_out_files[pos]),'/',basename(all_out_files[pos]))) ) {
                  file.rename(
                    paste0(getwd(),'/',dirname(all_out_files[pos]),'/',basename(all_out_files[pos])),
                    paste0(getwd(),'/',dirname(all_out_files[pos]),'/','Benchmark_',basename(all_out_files[pos]))
                  )
                  all_out_files2 = c(all_out_files2,paste0(dirname(all_out_files[pos]),'/','Benchmark_',basename(all_out_files[pos])))
                }
              } else {
                pos = grep('^Benchmark',basename(all_out_files))
                all_out_files2 = c(all_out_files2,all_out_files[pos])
              }
              all_out_files = all_out_files[-1*pos]
              rm(pos)
              
              if ( length(all_out_files) == 0) {
                next
              }
              
              cnt = 0
              while (length(all_out_files) > 0) {
                cnt = cnt + 1
                pos = grep(paste0('^',cnt,'_'),basename(all_out_files))
                if ( length(pos) == 0 ) {
                  pos = 1
                  file.rename(
                    paste0(getwd(),'/',dirname(all_out_files[pos]),'/',basename(all_out_files[pos])),
                    paste0(getwd(),'/',dirname(all_out_files[pos]),'/',cnt,'_',basename(all_out_files[pos]))
                  )
                  all_out_files2 = c(all_out_files2,paste0(dirname(all_out_files[pos]),'/',cnt,'_',basename(all_out_files[pos])))
                } else {
                  all_out_files2 = c(all_out_files2,all_out_files[pos])
                }
                all_out_files = all_out_files[-1*pos]
                rm(pos)
              }
              rm(cnt,all_out_files,all_out_files2)
            }
            rm(cy_time)
          }
          rm(photosynthesis)
        }
        rm(co2)
      }
      rm(man)
    }
    rm(site)
  }
  if ( length(error_list) > 0 ) {
    for(tt in error_list) {
      cat(tt)
    }
    rm(tt)
  }
  
  end.time <- Sys.time()
  print(end.time - start.time)
}

# calculate mean multiple ESM ----

dir_out = paste0(getwd(),"/output/result/")
dir.create(dir_out,showWarnings = F)
# 
# cy_exp = c('LOCAL',protocol_list)[1]
# site = site_list[1]
# cy_time = time_list[1]
for (cy_exp in c('LOCAL',protocol_list)) {
  for (site in site_list) {
    for(cy_time in time_list) {
      
      lf = list.files(paste0(getwd(),"/output/"),recursive=T,full.names = T,pattern = cy_time)
      if ( length(lf) == 0 ) next
      
      if ( length(grep('.pdf',lf)) > 0 ) lf = lf[-1*grep('.pdf',lf)]
      if ( length(lf) == 0 ) next
      
      if ( length(grep('_log_',lf)) > 0 ) lf = lf[-1*grep('_log_',lf)]
      if ( length(lf) == 0 ) next
      
      if ( length(grep('result',lf)) > 0 ) lf = lf[-1*grep('result',lf)]
      if ( length(lf) == 0 ) next
      
      lf = lf[grep(site,lf)]
      if ( length(lf) == 0 ) next
      
      lf = lf[grep(cy_exp,lf)]
      if ( length(lf) == 0 ) next
      
      if ( exists('df_t') ) rm(df_t)
      
      for ( cy_turnover_list in turnover_list ) {
        
        lf2 = lf[grep(paste0('LIVE_WOOD_TURNOVER_',cy_turnover_list,'-'),lf)]
        
        if ( length(lf2) == 0 ) next
        cy_lf2 = lf2[1]
        for ( cy_lf2 in lf2 ) {
          
          df = read.csv(cy_lf2,comment.char = '#')
          
          if ( is.factor(df$YEAR) ) {
            pos_na = which(is.na(df$LAYER) == 1)
            if (length(pos_na) > 0) df = df[-1*pos_na,]
            df$YEAR = as.numeric(as.character(df$YEAR))
          }
          # add TIME values
          if (cy_time == 'annual') {
            df$DAY = 1
            df$MONTH = 1
          }
          
          df$TIME = ymd(df$DAY + (df$MONTH * 100) + (df$YEAR*10000))
          
          df$turnover = cy_turnover_list
          df$site = site
          df$timescale = cy_time
          # split the name
          file_name = gsub(paste0(getwd(),"/output/"),'',cy_lf2)
          file_name = gsub('.txt','',file_name)
          file_name = unlist(strsplit(file_name,'/'))
          df$file_name = cy_lf2
          df$file_name2 = basename(cy_lf2)
          df$ph_mt = file_name[3]
          
          file_name = unlist(strsplit(file_name[6],'_'))
          
          df$des_file = file_name[1]
          pos_1 = grep('ESM',file_name)
          if ( length(pos_1) == 0 ) {
            pos_1 = grep('hist',file_name)[1]
            df$esm = 'LOCAL'
            df$esm_rcp = file_name[pos_1]
            df$exp = 'LOCAL'
          } else {
            df$esm = file_name[pos_1]
            df$esm_rcp = file_name[pos_1+1]
            df$exp = file_name[pos_1-1]
          }
          
          pos_1 = which(file_name == 'CO2')
          df$co2 = file_name[pos_1[1]+1]
          df$co2_rcp = file_name[pos_1[2]+1]
          
          pos_1 = which(file_name == 'Man')
          df$man = file_name[pos_1+1]
          rm(pos_1,file_name)
          
          # colonna per le medie tra ESM
          df$mean_esm = paste0('t_',unique(df$turnover),'_',
                               'pth_',unique(df$ph_mt),'_',
                               unique(df$esm_rcp),'_',#'esmrcp_',
                               'co2_',unique(df$co2),'_',
                               'co2rcp_',unique(df$co2_rcp),'_',
                               'man_',unique(df$man)
          )
          # colonna per le medie tra ESM
          df$mean_esm2 = paste0(unique(df$des_file),'_',
                                unique(df$esm),'_',
                                't_',unique(df$turnover),'_',
                                'pth_',unique(df$ph_mt),'_',
                                unique(df$esm_rcp),'_',#'esmrcp_',
                                'co2_',unique(df$co2),'_',
                                'co2rcp_',unique(df$co2_rcp),'_',
                                'man_',unique(df$man)
          )
          if ( exists('df_t') ) {
            df_t = rbind(df_t,df)
          } else {
            df_t = df
          }
          rm(df)
        }
        rm(cy_lf2)
      }
      rm(cy_turnover_list,lf2)
      
      df_t$mean_esm = gsub('co2rcp_','',df_t$mean_esm)
      df_t$mean_esm = gsub('pth_','',df_t$mean_esm)
      df_t$mean_esm = gsub('rcp','',df_t$mean_esm)
      df_t$mean_esm = gsub('Benchmark','B',df_t$mean_esm)
      
      df_t$mean_esm2 = gsub('co2rcp_','',df_t$mean_esm2)
      df_t$mean_esm2 = gsub('pth_','',df_t$mean_esm2)
      df_t$mean_esm2 = gsub('rcp','',df_t$mean_esm2)
      df_t$mean_esm2 = gsub('Benchmark','B',df_t$mean_esm2)
      
      # calculate the mean
      if ( exists('df_mean') ) rm(df_mean)
      
      for (cy_col in colnames(df_t) ) {
        cat(sprintf('%s\n',cy_col))
        v1 = c()
        for ( cy_time2 in unique(df_t$TIME) ) {
          for ( cy_mean_esm in unique(df_t$mean_esm) ) {
            pos_1 = which(df_t$mean_esm == cy_mean_esm &
                            df_t$TIME == cy_time2)
            if ( length(pos_1) == 0 ) next
            
            if ( is.numeric(df_t[,cy_col]) ) {
              v1 = c(v1,mean(df_t[pos_1,cy_col],na.rm = T))
            } else {
              v1 = c(v1,df_t[pos_1[1],cy_col])
            }
          }
          rm(cy_mean_esm)
        }
        rm(cy_time2)
        if ( exists('df_mean') ) {
          df_mean[,cy_col] = v1
        } else {
          df_mean = data.frame(v1)
          colnames(df_mean) = cy_col
        }
        rm(v1)
      }
      rm(cy_col)
      
      df_mean$TIME = ymd(df_mean$DAY + (df_mean$MONTH * 100) + (df_mean$YEAR*10000))
      
      #save the mean
      write.csv(df_mean, paste0(dir_out,'mean_esm_',site,'_',cy_time,'_',cy_exp,'.csv'),row.names = F)
      rm(df_mean,df_t)
    }
  }
  rm(site,cy_time)
}
rm(cy_exp)

# plot mean of multiple ESM ----

print("\n\nCOMPARSION PLOTS\n\n")
dir_out = paste0(getwd(),"/output/result/")
dir.create(dir_out,showWarnings = F)

for (cy_exp in c('LOCAL',protocol_list)) {
  for (site in site_list) {
    for(cy_time in time_list) {
      if ( !file.exists(paste0(dir_out,'mean_esm_',site,'_',cy_time,'_',cy_exp,'.csv')) ) next
      
      df = read.csv(paste0(dir_out,'mean_esm_',site,'_',cy_time,'_',cy_exp,'.csv'))
      df$TIME = ymd(df$DAY + (df$MONTH * 100) + (df$YEAR * 10000))
      
      var_to_skip = c('YEAR','MONTH','DAY','TIME','v1',
                      "turnover","site","timescale","file_name","file_name2","ph_mt","des_file",
                      "esm","esm_rcp","exp","co2","co2_rcp","mean_esm","mean_esm2","v1")
      
      df$v1 = df[,1]
      
      rcp_list = unique(df$co2_rcp)
      
      for ( cy_rcp_list in rcp_list ) {
        df2 = df[df$co2_rcp == cy_rcp_list,]
        df2$mean_esm = gsub(cy_rcp_list,'',df2$mean_esm)
        lista_plot = list()
        for (cy_var in colnames(df) ) {
          if (length(which(var_to_skip == cy_var)) > 0) next
          
          cat(sprintf('create plot for variable: %s (%d / %d)\n',cy_var,(length(lista_plot)),
                      (length(colnames(df))-length(var_to_skip))))
          
          df2$v1 = df2[,cy_var]
            
          mp1 = ggplot(df2) +
            geom_line(aes(x = TIME, y = v1, color = mean_esm)) +
            ggtitle(paste(site,cy_time,cy_rcp_list)) + 
            ylab(cy_var) + 
            theme(legend.position = 'top',legend.title = element_blank(),axis.title.x = element_blank()) +
            guides(col = guide_legend(ncol = 1))
        
          lista_plot[[length(lista_plot)+1]] = mp1
        }
        rm(cy_var)
        
        pdf(paste0(dir_out,'mean_esm_',site,'_',cy_time,'_',cy_exp,'_',cy_rcp_list,'.pdf'),
            onefile = T, width = 20,height = 15)
        nr_plot = 12
        while (length(lista_plot) > 0 ) {
          if ( length(lista_plot) < nr_plot ) nr_plot = length(lista_plot)
          print(plot_grid(plotlist = lista_plot[1:nr_plot],ncol = 4))
          lista_plot = lista_plot[-1*(1:nr_plot)]
          cat(sprintf('%d\n',length(lista_plot)))
        }
        dev.off()
        rm(df2)
      }
      
      rm(df,nr_plot,rcp_list,var_to_skip,mp1)
    }
    rm(cy_time)
  }
  rm(site)
}
rm(cy_exp)
print("\n\nCOMPARSION PLOTS COMPLETE\n\n")
# 
# # create the comparison plots ----
# 
# print("\n\nCOMPARSION PLOTS START....\n\n")
# 
# 
# site = site_list[1]
# cy_time = time_list[1]
#     
# for (site in site_list[1]) {
#   for(cy_time in time_list[1]) {
#     
#     lf = list.files(paste0(getwd(),"/output/"),recursive=T,full.names = T,pattern = cy_time)
#     if ( length(lf) == 0 ) next
#     
#     if ( length(grep('.pdf',lf)) > 0 ) lf = lf[-1*grep('.pdf',lf)]
#     if ( length(lf) == 0 ) next
#     
#     # if ( length(grep('LOCAL',lf)) > 0 ) lf = lf[-1*grep('LOCAL',lf)]
#     # if ( length(lf) == 0 ) next
#     # 
#     lf = lf[grep(site,lf)]
#     if ( length(lf) == 0 ) next
#     
#     if ( exists('df_t') ) rm(df_t)
#     cy_turnover_list = turnover_list[1]
#     
#     for ( cy_turnover_list in turnover_list ) {
#       
#       lf2 = lf[grep(paste0('LIVE_WOOD_TURNOVER_',cy_turnover_list,'-'),lf)]
#       
#       if ( length(lf2) == 0 ) next
#       cy_lf2 = lf2[1]
#       for ( cy_lf2 in lf2 ) {
#         
#         df = read.csv(cy_lf2,comment.char = '#')
#         
#         if ( is.factor(df$YEAR) ) {
#           pos_na = which(is.na(df$LAYER) == 1)
#           if (length(pos_na) > 0) df = df[-1*pos_na,]
#           df$YEAR = as.numeric(as.character(df$YEAR))
#         }
#         # add TIME values
#         if (cy_time == 'annual') {
#           df$DAY = 1
#           df$MONTH = 1
#         }
#         
#         df$TIME = ymd(df$DAY + (df$MONTH * 100) + (df$YEAR*10000))
#         
#         df$turnover = cy_turnover_list
#         df$site = site
#         df$timescale = cy_time
#         # split the name
#         file_name = gsub(paste0(getwd(),"/output/"),'',cy_lf2)
#         file_name = gsub('.txt','',file_name)
#         file_name = unlist(strsplit(file_name,'/'))
#         df$file_name = cy_lf2
#         df$file_name2 = basename(cy_lf2)
#         df$ph_mt = file_name[3]
#         
#         file_name = unlist(strsplit(file_name[6],'_'))
#         
#         df$des_file = file_name[1]
#         pos_1 = grep('ESM',file_name)
#         if ( length(pos_1) == 0 ) {
#           pos_1 = grep('hist',file_name)[1]
#           df$esm = 'LOCAL'
#           df$esm_rcp = file_name[pos_1]
#           df$exp = 'LOCAL'
#         } else {
#           df$esm = file_name[pos_1]
#           df$esm_rcp = file_name[pos_1+1]
#           df$exp = file_name[pos_1-1]
#         }
# 
#         pos_1 = which(file_name == 'CO2')
#         df$co2 = file_name[pos_1[1]+1]
#         df$co2_rcp = file_name[pos_1[2]+1]
#         
#         pos_1 = which(file_name == 'Man')
#         df$man = file_name[pos_1+1]
#         rm(pos_1,file_name)
#         
#         # colonna per le medie tra ESM
#         df$mean_esm = paste0('t_',unique(df$turnover),'_',
#                              'pth_',unique(df$ph_mt),'_',
#                              unique(df$esm_rcp),'_',#'esmrcp_',
#                              'co2_',unique(df$co2),'_',
#                              'co2rcp_',unique(df$co2_rcp),'_',
#                              'man_',unique(df$man)
#         )
#         # colonna per le medie tra ESM
#         df$mean_esm2 = paste0(unique(df$des_file),'_',
#                               unique(df$esm),'_',
#                               't_',unique(df$turnover),'_',
#                              'pth_',unique(df$ph_mt),'_',
#                              unique(df$esm_rcp),'_',#'esmrcp_',
#                              'co2_',unique(df$co2),'_',
#                              'co2rcp_',unique(df$co2_rcp),'_',
#                              'man_',unique(df$man)
#         )
#         if ( exists('df_t') ) {
#           df_t = rbind(df_t,df)
#         } else {
#           df_t = df
#         }
#         rm(df)
#       }
#       rm(cy_lf2)
#     }
#     rm(cy_turnover_list,lf2)
#     
#     df_t$mean_esm = gsub('co2rcp_','',df_t$mean_esm)
#     df_t$mean_esm = gsub('exp_','',df_t$mean_esm)
#     df_t$mean_esm = gsub('pth_','',df_t$mean_esm)
#     df_t$mean_esm = gsub('rcp','',df_t$mean_esm)
#     df_t$mean_esm = gsub('Benchmark','B',df_t$mean_esm)
#     
#     df_t$mean_esm2 = gsub('co2rcp_','',df_t$mean_esm2)
#     df_t$mean_esm2 = gsub('exp_','',df_t$mean_esm2)
#     df_t$mean_esm2 = gsub('pth_','',df_t$mean_esm2)
#     df_t$mean_esm2 = gsub('rcp','',df_t$mean_esm2)
#     df_t$mean_esm2 = gsub('Benchmark','B',df_t$mean_esm2)
#     
#     # se ho stesso ESM con tutte le altre caratteristiche faccio le medie
#     if ( exists('df_mean') ) rm(df_mean)
#   
#     for (cy_col in colnames(df_t) ) {
#       cat(sprintf('%s\n',cy_col))
#       v1 = c()
#       for ( cy_time2 in unique(df_t$TIME) ) {
#         for ( cy_mean_esm in unique(df_t$mean_esm) ) {
#           pos_1 = which(df_t$mean_esm == cy_mean_esm &
#                            df_t$TIME == cy_time2)
#           if ( length(pos_1) == 0 ) next
#           
#           if ( is.numeric(df_t[,cy_col]) ) {
#             v1 = c(v1,mean(df_t[pos_1,cy_col],na.rm = T))
#           } else {
#             v1 = c(v1,df_t[pos_1[1],cy_col])
#           }
#         }
#         rm(cy_mean_esm)
#       }
#       rm(cy_time2)
#       if ( exists('df_mean') ) {
#         df_mean[,cy_col] = v1
#       } else {
#         df_mean = data.frame(v1)
#         colnames(df_mean) = cy_col
#       }
#       rm(v1)
#     }
#     rm(cy_col)
#     df_mean$TIME = ymd(df_mean$DAY + (df_mean$MONTH * 100) + (df_mean$YEAR*10000))
#     mp1 = ggplot() +
#       # geom_point(data = df_t,aes(x = TIME, y = gpp, color = mean_esm2)) +
#       geom_line(data = df_mean,aes(x = TIME, y = gpp, color = mean_esm)) +
#       theme(legend.title = element_blank()) +
#       guides(col = guide_legend(ncol = 1))
#     ggsave(plot = mp1, filename = dir)
#     rm(df_t)
#   }
#   rm(lf)
#   rm(cy_time)
# }
# rm(site)
# 
# print("\n\nCOMPARSION PLOTS COMPLETE\n\n")


# validazione degli stand ----

print("\n\nCOMPARSION PLOTS STAND START....\n\n")
# db con i nomi e gli ID dei siti
df_siti = read.csv('sites_isimip.csv')
lista_time = c('annual')

for (cy_time in lista_time) {
  for (cy_s in site_list) {
    dir_in_gen = paste0(getwd(),"/output/",output_folder,"-", version, "-", cy_s,'/')
    dir_in = list.dirs(dir_in_gen,recursive = F)
    dir_in = dir_in[grep(cy_s,dir_in)]
    dir_out = dir_in_gen
    # import all dataset output in site folder ----
    if (exists('df_t')) {
      rm(df_t)
    }
    df_t = import_all_out(dir_in,cy_time)
    
    filename2 = c()
    for ( cy_fn in df_t$filename ) {
      filename2 = c(filename2,
                    unlist(strsplit(cy_fn,'_'))[1]
      )
    }
    df_t$filename2 = filename2
    rm(filename2,cy_fn)
    
    site_id = df_siti$site_id[df_siti$model_name == cy_s]
    site_sp = as.character(df_siti$species[df_siti$model_name == cy_s])
    
    # # plot all variables in df_t ----
    
    # validazione delle variabili stand e flussi solo sui LOCAL----
    
    df_t = df_t[grep('LOCAL',df_t$filename),]
    
    for (cy_valid in c('STAND') ) {#'FLUX_DT','FLUX_NT',
      if (cy_valid == 'STAND') {
        # importo il file con le corrispondenze delle variabili tra modello e profound
        cat(sprintf('import file: %s\n',file_db_corrisp))
        db_var_corr = read.csv(file_db_corrisp,comment.char = '#')
        cat(sprintf('import file: %s OK\n',file_db_corrisp))
        
        # importo il file stand da profound e uso solo alcune righe
        cat(sprintf('import file: %s\n',file_stand))
        db_valid = read.csv(file_stand,comment.char = '#')
        cat(sprintf('import file: %s OK\n',file_stand))
        # uso solo le righe della specie e del sito
        db_valid = db_valid[db_valid$site_id == site_id,]
        db_valid = db_valid[as.character(db_valid$species_id) == site_sp,]
        
        # corrispondenze tra nome del modello e nome del db
        db_var_corr = data.frame(
          'model.variable' = db_var_corr$model.variable,
          'profound.variable' = db_var_corr$profound.variable
        )
      }
      if (cy_valid == 'FLUX_DT') {
        
        # importo il file stand da profound e uso solo alcune righe
        cat(sprintf('import file: %s\n',paste0('FLUX_',site_id,'_',cy_time,'.csv')))
        db_valid = read.csv(paste0(file_flux,'FLUX_',site_id,'_',cy_time,'.csv'),comment.char = '#')
        cat(sprintf('import file: %s OK\n',paste0('FLUX_',site_id,'_',cy_time,'.csv')))
        
        # corrispondenze tra nome del modello e nome del db
        db_var_corr = data.frame(
          'model.variable' = c("YEAR","gpp","reco","nee"),
          'profound.variable' = c('year','gppDtCutRef_gCm2d1','recoDtCutRef_gCm2d1',
                                  'neeCutRef_gCm2d1')
        )
      }
      if (cy_valid == 'FLUX_NT') {
        
        # importo il file stand da profound e uso solo alcune righe
        cat(sprintf('import file: %s\n',paste0('FLUX_',site_id,'_',cy_time,'.csv')))
        db_valid = read.csv(paste0(file_flux,'FLUX_',site_id,'_',cy_time,'.csv'),comment.char = '#')
        cat(sprintf('import file: %s OK\n',paste0('FLUX_',site_id,'_',cy_time,'.csv')))
        
        # corrispondenze tra nome del modello e nome del db
        db_var_corr = data.frame(
          'model.variable' = c("YEAR","gpp","reco","nee"),
          'profound.variable' = c('year','gppNtCutRef_gCm2d1','recoNtCutRef_gCm2d1',
                                  'neeCutRef_gCm2d1')
        )
      }
      # aggiungo la colonna TIME
      db_valid$TIME = ymd((db_valid$year*10000) + (0101))
      
      db_valid$filename2 = 'Meas'
      
      # sincronizzo le serie temporali usando solo i file LOCAL
      pos_year = c()
      for (cy_year_st in db_valid$year) {
        pos_tmp = which(df_t$YEAR == cy_year_st)
        if ( length(pos_tmp) > 0 ) {
          pos_year = c(pos_year,pos_tmp)
        }
        rm(pos_tmp)
      } 
      rm(cy_year_st)
      pos_year2 = c()
      for (cy_year_st in df_t$YEAR) {
        pos_tmp = which(db_valid$year == cy_year_st)
        if ( length(pos_tmp) > 0 ) {
          pos_year2 = c(pos_year2,pos_tmp)
        }
        rm(pos_tmp)
      } 
      rm(cy_year_st)
      
      if ( (length(pos_year) * length(pos_year2))  > 0 ) {
        
        df_t2 = df_t[pos_year,]
        db_valid2 = db_valid[pos_year2,]
        
        # faccio i plot per le singole variabili
        list_plot_stand_valid = list()
        
        for ( cy_v in seq(1,length(db_var_corr$model.variable)) ) {
          if ( as.character(db_var_corr$model.variable[cy_v]) == 'YEAR' ) {
            next
          }
          if ( as.character(db_var_corr$model.variable[cy_v]) == 'SPECIES' ) {
            next
          }
          
          cat(sprintf('\t create plot for variable: %s\n',as.character(db_var_corr$model.variable[cy_v])))
          
          if ( sum(grepl(as.character(db_var_corr$model.variable[cy_v]),
                         colnames(df_t2)) ) == 0
          ) {
            cat(sprintf('\t\t variable: %s MISSING\n\n',as.character(db_var_corr$model.variable[cy_v])))
            next
          }
          
          df_plot_tmp = data.frame(
            'TIME' = df_t2$TIME,
            'v1' = df_t2[,as.character(db_var_corr$model.variable[cy_v])],
            'legend_name' = df_t2$filename2
          )
          
          df_plot_tmp = rbind(df_plot_tmp,
                              data.frame(
                                'TIME' = db_valid2$TIME,
                                'v1' = db_valid2[,as.character(db_var_corr$profound.variable[cy_v])],
                                'legend_name' = 'Meas'
                              ))
          
          plot1 = plot_ts(df_plot_tmp,
                          yvar_name = as.character(db_var_corr$model.variable[cy_v]),
                          xvar_name = 'YEAR',ribbon = 'N')
          
          rm(df_plot_tmp)
          for (cy_vv in unique(df_t2$filename2)) {
            pos_v = which(df_t2$filename2 == cy_vv)
            if ( exists('df_plot_tmp') ) {
              df_plot_tmp = rbind(df_plot_tmp,data.frame(
                'v1' = df_t2[pos_v,as.character(db_var_corr$model.variable[cy_v])],
                'v2' = db_valid2[,as.character(db_var_corr$profound.variable[cy_v])],
                'legend_name' = cy_vv
              ))
            } else {
              df_plot_tmp = data.frame(
                'v1' = df_t2[pos_v,as.character(db_var_corr$model.variable[cy_v])],
                'v2' = db_valid2[,as.character(db_var_corr$profound.variable[cy_v])],
                'legend_name' = cy_vv
              )
            }
            rm(pos_v)
          }
          rm(cy_vv)
          
          plot_lr = plot_multi_lm(df_plot_tmp,
                                  yvar_name = as.character(db_var_corr$model.variable[cy_v]),
                                  xvar_name = as.character(db_var_corr$profound.variable[cy_v]))
          
          # plot_lr[[1]] = plot_lr[[1]] + theme(legend.position = 'top')
          plot1 = plot1 + theme(legend.position = 'top')
          
          cat(sprintf('\t create plot for variable: %s COMPLETE\n',as.character(db_var_corr$model.variable[cy_v])))
          
          list_plot_stand_valid[[length(list_plot_stand_valid)+1]] = 
            plot_grid(plot1,plot_lr[[1]],plot_lr[[2]],ncol = 3,rel_widths = c(1,1,2))
          
          rm(plot_lr,plot1)
          rm(df_plot_tmp)
        }
        rm(cy_v)
        
        if ( length(list_plot_stand_valid) > 0 ) {
          
          pdf(paste0(dir_out,'/',cy_valid,'_validation_',cy_s,'.pdf'),
              onefile = T, width = 20,height = 15)
          nr_plot_per_page = 5
          while (length(list_plot_stand_valid) > 0) {
            if ( length(list_plot_stand_valid) < 5 ) {
              nr_plot_per_page = length(list_plot_stand_valid)
            }
            print(
              plot_grid(plotlist = list_plot_stand_valid[1:nr_plot_per_page],nrow = 5))
            list_plot_stand_valid = list_plot_stand_valid[-1*(1:nr_plot_per_page)]
          }
          
          dev.off()
          
          cat(paste0(dir_out,'/',cy_valid,'_validation_',cy_s,'.pdf created!\n'))
          
          rm(nr_plot_per_page)
        }
        
        rm(list_plot_stand_valid)
        rm(df_t2,db_valid2,db_var_corr)
      }
      rm(pos_year,pos_year2)
    }
    rm(cy_valid)
  }
}
rm(dir_in_gen)

cat(sprintf("\n\nCOMPARSION PLOTS STAND COMPLETE\n\n"))

# validazione dei flussi ----

cat(sprintf("\n\nFLUX VALIDATION PLOTS START.....\n\n"))

df_siti = read.csv('sites_isimip.csv')

for (cy_s in site_list) {
  dir_in_gen = paste0(getwd(),"/output/",output_folder,"-", version, "-", cy_s,'/')
  site_code = as.character(df_siti$fluxnet_code[df_siti$model_name == cy_s])
  
  # importo i dati fluxnet DD ----
  lista_files = list.files(dir_in_ec,pattern = paste0('_FULLSET_','DD','_'),
                           recursive = T,full.names = T)
  lista_files = lista_files[grep(paste0('FLX_',as.character(site_code)),lista_files)]
  
  if (length(lista_files) == 0 ) {
    stop(sprintf('file: %s\n NOT FOUND',
                 paste0('FLX_',site_code)))
  }
  file_ec = lista_files
  
  dir_in = list.dirs(dir_in_gen,recursive = F)
  dir_in = dir_in[grep(cy_s,dir_in)]
  dir_in = dir_in[grep('LOCAL',dir_in)]
  
  ls_file_md = list.files(dir_in,pattern = 'daily',recursive = T,full.names = T)
  
  lista_p = flux_validation(ls_file_md,
                            cy_s,
                            file_ec)
  
  lista_p2 = flux_validation(ls_file_md,
                             cy_s,
                             file_ec,
                             var_md = c('reco','gpp'),
                             var_eddy = c('RECO_NT_CUT_USTAR50','GPP_NT_CUT_USTAR50'),
                             var_eddy_qc = c('NEE_CUT_USTAR50_QC','NEE_CUT_USTAR50_QC'),
                             var_eddy_unc_max = c('RECO_NT_CUT_95','GPP_NT_CUT_95'),
                             var_eddy_unc_min = c('RECO_NT_CUT_05','GPP_NT_CUT_05'))
  
  lista_p3 = flux_validation(ls_file_md,
                             cy_s,
                             file_ec,
                             var_md = c('le','le'),
                             var_eddy = c('LE_F_MDS','LE_CORR'),
                             var_eddy_qc = c('LE_F_MDS_QC','LE_F_MDS_QC'),
                             var_eddy_unc_max = c('LE_CORR_75','LE_CORR_75'),
                             var_eddy_unc_min = c('LE_CORR_25','LE_CORR_25'))
  rm(file_mod,ls_file_md)
  
  pdf(paste0(dir_in_gen,'validation_flux_',cy_s,'.pdf'),
      onefile = T, width = 20,height = 15)
  
  print(plot_grid(lista_p[[1]]))
  print(plot_grid(lista_p[[2]]))
  
  print(plot_grid(lista_p2[[1]]))
  print(plot_grid(lista_p2[[2]]))
  print(plot_grid(lista_p2[[3]]))
  print(plot_grid(lista_p2[[4]]))
  
  print(plot_grid(lista_p3[[1]]))
  print(plot_grid(lista_p3[[2]]))
  print(plot_grid(lista_p3[[3]]))
  print(plot_grid(lista_p3[[4]]))
  
  dev.off()
  rm(list_p,lista_p2,lista_p3)
  
  cat(sprintf('\ncreate file: %s\n',paste0(dir_in_gen,'validation_flux_',cy_s,'.pdf')))
  
}
rm(cy_s)

cat(sprintf("\n\nFLUX VALIDATION PLOTS COMPLETE\n\n"))

# validazione di tutti gli output ----
# 
# cat(sprintf("\n\nALL SEASONAL PLOTS START...\n\n"))
# for (cy_s in site_list) {
#   dir_in_gen = paste0(getwd(),"/output/",output_folder,"-", version, "-", cy_s,'/')
#   
#   dir_in = list.dirs(dir_in_gen,recursive = F)
#   dir_in = dir_in[grep(cy_s,dir_in)]
#   dir_in = dir_in[grep('LOCAL',dir_in)]
#   
#   
#   ls_file_md = list.files(dir_in,pattern = 'daily',recursive = T,full.names = T)
#   
#   list_p = list()
#   
#   for (file_mod in ls_file_md) {
#     
#     list_p[[length(list_p)+1]] = output_validation(file_mod,
#                                                  cy_s)
#                                                  
#   }
#   rm(file_mod,ls_file_md)
#   
#   pdf(paste0(dir_in_gen,'SEASONAL_output_',cy_s,'.pdf'),
#       onefile = T, width = 15,height = 12)
#   
#   for ( cy_p in seq(1,length(list_p)) ) {
#     for (cy_p2 in seq(1,length(list_p[[cy_p]]))) {
#       mpt = plot_grid(plotlist = list_p[[cy_p]][cy_p2])
#       print(mpt)
#       rm(mpt)
#     }
#   }
#   rm(cy_p,cy_p2)
#   dev.off()
#   rm(list_p)
#   
# }
# rm(cy_s)
# 
# cat(sprintf("\n\nALL SEASONAL PLOTS COMPLETE\n\n"))


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
#     pdf(paste0("./output/",output_folder, "-", version, "-", site,"-",protocol,"/output_", version,"_", year,"_", month,"_", day,"/daily/Daily_",site,"-ESM",esm,"-RCP-",rcp,"-Man-",man,"-Co2-",co2,".pdf"), onefile = T, width = 30,height = 24)
#     par(mfrow=c(5,5))
#     for (i in 5:length(outputCMCC$daily)) plot(outputCMCC$daily$Date, outputCMCC$daily[,i], main=colnames(outputCMCC$daily[i]), 
#                                                col="black", xlab = "year", ylab= "unit", type = "l", col.lab="red", cex.lab=2, cex.axis=1.5, cex.main=2, pch =30)
#     dev.off()
#     
#     #output model TREE MONTHLY
#     pdf(paste0("./output/",output_folder, "-", version, "-", site,"-",protocol,"/output_", version, "_", year,"_", month,"_", day,"/monthly/Monthly_",site,"-ESM",esm,"-RCP-",rcp,"-Man-",man,"-Co2-",co2,".pdf"), onefile = T, width = 30,height = 24)
#     par(mfrow=c(5,5))
#     for (i in 4:length(outputCMCC$monthly)) plot(outputCMCC$monthly$Date,outputCMCC$monthly[,i], main=colnames(outputCMCC$monthly[i]),
#                                                  col="black", xlab = "year", ylab= "unit", type = "l", col.lab="red", cex.lab=1.5, cex.axis=1.5, cex.main=2, pch =30)
#     dev.off()
#     
#     #output model TREE ANNUAL
#     pdf(paste0("./output/",output_folder, "-", version, "-", site,"-",protocol,"/output_", version, "_", year,"_", month,"_", day,"/annual/Annual_",site,"-ESM",esm,"-RCP-",rcp,"-Man-",man,"-Co2-",co2,".pdf"), onefile = T, width = 20,height = 16)
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
#                                               "-m"," ", "PAPER/", "ESM", esm_list[f],"/", "ESM", esm_list[f],"_", "rcp", rcp_list[g], ".txt", " ",
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

