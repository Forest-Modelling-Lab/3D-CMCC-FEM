# Developed by:
# Alessio Collalti (alessio.collalti@cmcc.it)
# Carlo Trotta (trottacarlo@unitus.it)
# starting date: 18 April 2017
rm(list = ls())

# working directory
# setwd('/home/alessio-cmcc/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/')
setwd(getwd())
#setwd('/home/alessio-cmcc/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/')

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

model<-"3D-CMCC-FEM"
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
build_list<-c('Debug')#, 'Release')
site_list<-c("Bily_Kriz")
esm_list <-c("3")# ("1","2","3","4","5", "All")
rcp_list <-c("All")# ("0p0","2p6","4p5","6p0","8p5","All")
man_list <-c("off")# ("on",'off', "All")
co2_list <-c("on")# , "on",off", "All")
protocol_list<-c("FT")# ("2A","2B", "All") 
local_list<-c('on')
climate_off_list<-c('off') # for climate off and co2 on

time_list = c('annual')

#  output folder name
output_folder = paste0("Test_output_Rstudio_ct_photosynthesis_TEST_", protocol_list,'_',co2_list)

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

## a way to time an R expression: system.time is preferred
run_model = 1
if ( run_model == 1 ) {
  start.time <- Sys.time()
  
  error_list = c()
  
  for (site in site_list) {
    for (man in man_list) {
      for (co2 in co2_list) {
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
            paste0( protocol, "/", site,"_settings_ISIMIP_Manag-on_CO2-", co2,".txt"),
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
            dir.create(paste0(getwd(),"/output/",output_folder,"-", version, "-", site),showWarnings = FALSE)
            dir.create(paste0(getwd(),"/output/",output_folder,"-", version, "-", site,"/",protocol),showWarnings = FALSE)
            
            cat(paste0("\nstart", model," ",version," ","protocol: ",protocol, " site: ", site,'\n'))
            
            systemCall  <- paste0(build_list,'/3D_CMCC_Forest_Model', " ",
                                  "-i"," ", "input/", site, " ",
                                  "-p"," ", "input/parameterization", " ",
                                  "-o"," ", "output/",output_folder,"-", version, "-", site,"/",protocol," ",
                                  "-d"," ", "ISIMIP/", site,"_stand_ISIMIP.txt", " ",
                                  "-m"," ", "ISIMIP/", protocol,"/",protocol, "_hist.txt", " ",
                                  "-s"," ", "ISIMIP/", site,"_soil_ISIMIP.txt", " ",
                                  "-t"," ", "ISIMIP/", site,"_topo_ISIMIP.txt", " ",
                                  "-c"," ", "ISIMIP/", protocol, "/", site,"_settings_ISIMIP_Manag-on_CO2-", co2,".txt", " ",
                                  "-k"," ", "ISIMIP/", "/CO2/CO2_hist.txt",
                                  ">output/",output_folder,"-", version, "-", site,"/",protocol,"_log_",site,"_LOCAL_Manag-on_CO2-", co2,".txt"
            )
            # launch execution
            system(systemCall)
            outputCMCC<- list()
            cat(paste0("start 3D-CMCC ",
                       "protocol: ",protocol, " site: ", site, '... COMPLETE!\n'))
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
                paste0( protocol, "/", site,"_settings_ISIMIP_Manag-", man, "_CO2-", co2,".txt"),
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
                dir.create(paste0(getwd(),"/output/",output_folder,"-", version, "-", site),showWarnings = FALSE)
                dir.create(paste0(getwd(),"/output/",output_folder,"-", version, "-", site,"/",protocol),showWarnings = FALSE)
                
                cat(paste0("start 3D-CMCC ",
                           "protocol: ",protocol, " site: ", site, 
                           " ESM: ", esm," RCP: ", rcp," CO2 RCP: ", rcp," Manag-", man, " CO2-", co2,'\n'))
                
                systemCall  <- paste0(build_list,'/3D_CMCC_Forest_Model', " ",
                                      "-i"," ", "input/", site, " ",
                                      "-p"," ", "input/parameterization", " ",
                                      "-o"," ", "output/",output_folder,"-", version, "-", site,"/",protocol," ",
                                      "-d"," ", "ISIMIP/", site,"_stand_ISIMIP.txt", " ",
                                      "-m"," ", "ISIMIP/", protocol, "/ESM", esm,"/", protocol,"_","ESM", esm,"_", "rcp", rcp, ".txt", " ",
                                      "-s"," ", "ISIMIP/", site,"_soil_ISIMIP.txt", " ",
                                      "-t"," ", "ISIMIP/", site,"_topo_ISIMIP.txt", " ",
                                      "-c"," ", "ISIMIP/", protocol, "/", site,"_settings_ISIMIP_Manag-", man, "_CO2-", co2,".txt", " ",
                                      "-k"," ", "ISIMIP/", "/CO2/CO2_", "rcp",rcp, ".txt",
                                      ">output/",output_folder,"-", version, "-", site,"/",protocol,"_log_",site,"_",protocol,
                                      "_ESM_", esm,"_RCP_", rcp,"_CO2_RCP_", rcp,"_Manag-", man, "_CO2-", co2,".txt"
                )
                
                # launch execution
                system(systemCall)
                outputCMCC<- list()
                
                cat(paste0("start 3D-CMCC ",
                           "protocol: ",protocol, " site: ", site, 
                           " ESM: ", esm," RCP: ", rcp," CO2 RCP: ", rcp," Manag-", man, " CO2-", co2,' ... COMPLETE!\n'))
                
                # THIS RUN FIXING CLIMATE TO CURRENT AND CHANGES CO2 CONCENTRATIONS
                if ( rcp == '0p0' && co2 == 'on' && climate_off_list == 'on' )
                 {
                   for (rcp2 in rcp_list)
                   {
                     cat(paste0("start 3D-CMCC ",
                                "protocol: ",protocol, " site: ", site, 
                                " ESM: ", esm," RCP: ", rcp," CO2 RCP: ", rcp2," Manag-", man, " CO2-", co2,'\n'))
                     
                     systemCall  <- paste0(build_list,'/3D_CMCC_Forest_Model', " ",
                                           "-i"," ", "input/", site, " ",
                                           "-p"," ", "input/parameterization", " ",
                                           "-o"," ", "output/",output_folder,"-", version, "-", site,"/",protocol," ",
                                           "-d"," ", "ISIMIP/", site,"_stand_ISIMIP.txt", " ",
                                           "-m"," ", "ISIMIP/", protocol, "/ESM", esm,"/", protocol,"_","ESM", esm,"_", "rcp", rcp, ".txt", " ",
                                           "-s"," ", "ISIMIP/", site,"_soil_ISIMIP.txt", " ",
                                           "-t"," ", "ISIMIP/", site,"_topo_ISIMIP.txt", " ",
                                           "-c"," ", "ISIMIP/", protocol, "/", site,"_settings_ISIMIP_Manag-", man, "_CO2-", co2,".txt", " ",
                                           "-k"," ", "ISIMIP/", "/CO2/CO2_", "rcp",rcp2, ".txt",
                                           ">output/",output_folder,"-", version, "-", site,"/",protocol,"_log_",site,"_",protocol,
                                           "_ESM_", esm,"_RCP_",rcp,"_CO2_RCP_",rcp2,"_Manag-", man, "_CO2-", co2,".txt"
                     )
                     
                     # launch execution
                     system(systemCall)
                     outputCMCC<- list()
                     
                     cat(paste0("start 3D-CMCC ",
                                "protocol: ",protocol, " site: ", site, 
                                " ESM: ", esm," RCP: ", rcp," CO2 RCP: ", rcp2," Manag-", man, " CO2-", co2,' ... COMPLETE!\n'))
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
          all_out_files = list.files(paste0("output/",output_folder,"-", version, "-", site,"/"),
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

          
          
          if ( sum(grepl('^Bench_',basename(all_out_files))) == 0  ) {
            pos = 1
            if( file.exists(paste0(getwd(),'/',dirname(all_out_files[pos]),'/',basename(all_out_files[pos]))) ) {
              file.rename(
                paste0(getwd(),'/',dirname(all_out_files[pos]),'/',basename(all_out_files[pos])),
                paste0(getwd(),'/',dirname(all_out_files[pos]),'/','Bench_',basename(all_out_files[pos]))
              )
              all_out_files2 = c(all_out_files2,paste0(dirname(all_out_files[pos]),'/','Bench_',basename(all_out_files[pos])))
            }
          } else {
            pos = grep('^Bench_',basename(all_out_files))
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
      rm(co2)
    }
    rm(man)
  }
  rm(site)
  
  if ( length(error_list) > 0 ) {
    for(tt in error_list) {
      cat(tt)
    }
    rm(tt)
  }
  
  end.time <- Sys.time()
  print(end.time - start.time)
}


# create the comparison plots ----

print("\n\nCOMPARSION PLOTS START....\n\n")

for (site in site_list) {
  for(cy_time in time_list) {
    
    dir_gen = paste0(getwd(),"/output/",output_folder,"-", version, "-", site)
    
    lf = list.files(paste0(getwd(),"/output/",output_folder,"-", version, "-", site),recursive=T,full.names = T,pattern = cy_time)
    
    if ( length(lf) == 0 ) {
      next
    }
    
    if ( length(grep('.pdf',lf)) > 0 ) {
      lf = lf[-1*grep('.pdf',lf)]
    }
    
    if ( length(lf) == 0 ) {
      next
    }
    if ( exists('df_t') ) {
      rm(df_t)
    }
    df_t = import_all_out(paste0(getwd(),"/output/",output_folder,"-", version, "-", site),'annual')
    file_name2 = c()
    for (cy_r in as.character(df_t$filename)) {
      file_name2 = c(file_name2,unlist(strsplit(cy_r,'_'))[1])
    }
    df_t$filename2 = file_name2
    rm(file_name2,cy_r)
    
    GetPlotResults_file(df_t,color_variable = 'filename2',
                        var_to_skip = c('YEAR','MONTH','DAY','LAYER','SPECIES',
                                        'MANAGEMENT','filename',
                                        'filename2','Date','TIME'),
                        paste0(getwd(),"/output/",output_folder,"-", version, "-", site,'/',cy_time,'_',site,'_file_all.pdf'))
    
    rm(df_t)
    
    
  }
  rm(cy_time)
}
rm(site)
rm(lf)

print("\n\nCOMPARSION PLOTS COMPLETE\n\n")


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

