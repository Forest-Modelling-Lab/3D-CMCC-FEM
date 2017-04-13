rm(list = ls())

library('RSQLite')

setwd('C:/SviluppoR/')

input_dir = 'C:\\Users\\CMCC\\Desktop\\profound\\'

output_dir_gen = paste0(input_dir,'\\out_R_wrapper_new\\')

dir.create(output_dir_gen,showWarnings = FALSE)

# db_name = 'ProfoundData_download_20170224.sqlite'
db_name ='ProfoundData_download_20170410.sqlite'

# connect to the sqlite file
con <- dbConnect(drv=RSQLite::SQLite(), paste0(input_dir,db_name))

# get a list of all tables
alltables = dbListTables(con)

# write a file with all tables
write.csv(alltables,paste0(output_dir_gen,'alltables.csv'),row.names = FALSE)

# write the METADATA* files ----
output_dir_gen2 = paste0(output_dir_gen,'/meta_data/')
dir.create(output_dir_gen2,showWarnings = FALSE)

for ( t in alltables) {
  pos = grep("METADATA", t)
  if ( length(pos) > 0 ) {
    qrt = dbGetQuery(con,
      paste("SELECT * FROM",t)
          )
    cat(paste("write table: ",t,'\n'))
    write.csv(qrt,paste0(output_dir_gen2,t,'.csv'),row.names = FALSE)
    rm(qrt)
    cat(paste("write table: ",t,' ...OK\n'))
  }
  rm(pos)
}
rm(t)

# write sites information ----
t_1 = c('SITES','OVERVIEW','OVERVIEW_EXTENDED')

for (t in t_1) {
  qrt = dbGetQuery(con, 
                   paste("SELECT * FROM",t)
  )
  cat(paste("write table: ",t,'\n'))
  write.csv(qrt,paste0(output_dir_gen,t,'.csv'),row.names = FALSE)
  
  cat(paste("write table: ",t,' ...OK\n'))
  rm(qrt)
}
rm(t,t_1)
rm(output_dir_gen2)

# # extract all tables for sites
# all_tab_for_site = alltables[grep("_21", alltables)]
# 
# for (t in all_tab_for_site) {
#   # write sites information
#   qrt = dbGetQuery(con, 
#                    paste("SELECT * FROM",t)
#   )
#   cat(paste("write table for site: ",t,'\n'))
#   write.csv(qrt,paste0(output_dir_gen,t,'.csv'),row.names = FALSE)
#   rm(qrt)
#   cat(paste("write table for site: ",t,' ...OK\n'))  
# }

# 
# # create the soil file ----
# 
# # extract lat, lon from the table SITES
# t = 'SITES'
# qrt = dbGetQuery(con, 
#                  paste("SELECT lat,lon FROM",t,' WHERE site_id = ','21')
# )
# 
# soil_lat = qrt$lat
# soil_lon = qrt$lon
# rm(qrt,t)
# 
# # extract percentage of clay,silt, sand and soil depth from table SOIL
# 
# t = paste0('SOIL_','21')
# qrt = dbGetQuery(con, 
#                  paste("SELECT clay_percent,silt_percent,sand_percent,thickness_cm FROM",
#                        t,' WHERE site_id = ','21')
# )
# 
# soil_clay_percent = qrt$clay_percent
# soil_silt_percent = qrt$silt_percent
# soil_sand_percent = qrt$sand_percent
# soil_depth = qrt$thickness_cm
# 
# rm(qrt,t)
# 
# # create the topo file ----
# 
# t = 'SITES'
# qrt = dbGetQuery(con, 
#                  paste("SELECT elevation_masl FROM",t,' WHERE site_id = ','21')
# )
# 
# topo_elev = qrt$elevation_masl
# rm(qrt,t)

# for each site etraxct inpout dataset and create folder structure ----

lista_siti = read.csv(paste0(output_dir_gen,'SITES.csv'))
cyS = lista_siti$site_id[8]

for (cyS in lista_siti$site_id) {
  
  output_dir = paste0(output_dir_gen,factor(lista_siti$site)[which(lista_siti$site_id == cyS)],'/')
  dir.create(output_dir,showWarnings = FALSE)
  
  # create STAND input file ----
  
  stand_tab = alltables[grep(paste0("_",cyS), alltables)]
  stand_tab = stand_tab[grep("STAND", stand_tab)]
  
  for (t in stand_tab) {
    
    if ( grepl("METADATA_STAND",t) == 1) {
      next
    }
    
    cat(paste0('import table: ',t,'\n'))
    
    ck_var =  dbGetQuery(con,
                         paste("SELECT * FROM",t)
    )
    # 
    # write.table(ck_var,paste0(output_dir,'all_',t,'.txt'),row.names = FALSE,sep=',',
    #             quote = FALSE,append = FALSE)
    # 
    # 
    # extract only the 
    Year = ck_var$year
    x = rep(0,length(Year))
    y = rep(0,length(Year))
    Age = ck_var$age
    Species = ck_var$species
    Management = rep('T',length(Year))
    N = ck_var$density_treeha
    Stool = rep(0,length(Year))
    # TEST
    AvDBH = ck_var$dbhArith_cm
    Height = ck_var$heightArith_m
    Wf = rep(0,length(Year))
    Wrc = rep(0,length(Year))
    Wrf = rep(0,length(Year))
    Ws = rep(0,length(Year))
    Wbb = rep(0,length(Year))
    Wres = rep(0,length(Year))
    Lai = rep(0,length(Year))
    
    
    df_stand = data.frame(Year,x,y,Age,Species,Management,N,Stool,AvDBH,Height,Wf,Wrc,Wrf,Ws,Wbb,Wres,Lai)
    
    rm(ck_var)
    
    filename = t
    cat(paste0("write file: ",filename,'.txt\n'))
    write.table(df_stand,paste0(output_dir,filename,'.txt'),row.names = FALSE,sep=',',
                quote = FALSE,append = FALSE)
    cat(paste0("write file: ",filename,'.txt ... OK\n'))
    
    rm(Year,x,y,Age,Species,Management,N,Stool,AvDBH,Height,Wf,Wrc,Wrf,Ws,Wbb,Wres,Lai)
    rm(filename,df_stand)
    
  }
  rm(stand_tab,t)
  
  # create meteo input file ----
  
  meteo_tab = alltables[grep(paste0("_",cyS), alltables)]
  meteo_tab = meteo_tab[grep("CLIMATE", meteo_tab)]
  
  for (t in meteo_tab) {
    
    if ( grepl("METADATA_CLIMATE",t) == 1) {
      next
    }
    
    cat(paste0('import table: ',t,'\n'))
    
    ck_var =  dbGetQuery(con,
                         paste("SELECT * FROM",t)
    )
    
    fd_menu = 'None'
    fc_menu = 'None'
    
    if ( length(grep('forcingDataset',colnames(ck_var))) != 0) {
      
      # extract forcingDataset option
      fd_menu =  dbGetQuery(con,
                            paste("SELECT forcingDataset FROM",t,"GROUP BY","forcingDataset")
      )
      fd_menu = unlist(fd_menu)
      
    }
    
    if ( length(grep('forcingConditions',colnames(ck_var))) != 0) {
      # extract forcingConditions option
      fc_menu =  dbGetQuery(con,
                            paste("SELECT forcingConditions FROM",t,"GROUP BY","forcingConditions")
      )
      fc_menu = unlist(fc_menu)
    }
    
    rm(ck_var)
    
    for ( cy_fd in fd_menu) {
      for ( cy_fc in fc_menu) {
        query_str = paste0("SELECT ",
                           "year,",
                           "mo,",
                           "day,",
                           "rad_Jcm2day,",
                           "tmean_degC,",
                           "tmax_degC,",
                           "tmin_degC,",
                           "relhum_percent,",
                           "p_mm",
                           " FROM ",t)
        if ( !grepl(cy_fd,'None')) {
          query_str = paste0(query_str,
                             " WHERE forcingDataset =\'",cy_fd,"\'")
        } 
        if ( !grepl(cy_fc,'None') ) {
          if ( !grepl(cy_fd,'None') ) {
            query_str = paste0(query_str,
                               " AND forcingConditions  =\'",cy_fc,"\'")
          } else {
            query_str = paste0(query_str,
                               " WHERE forcingConditions  =\'",cy_fc,"\'")
          }
        }
        
        qrt = dbGetQuery(con, query_str )
        
        rm(query_str)
        
        Year = qrt$year
        Month = qrt$mo
        n_days = qrt$day
        
        # convert 1 J/cm2 = 0.01 MJ/m2; 1 MJ/m2 = 100 J/cm2
        # source: http://www.endmemo.com/sconvert/j_cm2mj_m2.php
        Rg_f = qrt$rad_Jcm2day * 0.01
        
        Ta_f = qrt$tmean_degC
        Tmax = qrt$tmax_degC
        Tmin = qrt$tmin_degC
        RH_f = qrt$relhum_percent
        Ts_f = rep(-9999,length(RH_f))
        Precip = qrt$p_mm
        SWC = rep(-9999,length(RH_f))
        LAI = rep(-9999,length(RH_f))
        ET = rep(-9999,length(RH_f))
        WS_f = rep(-9999,length(RH_f))
        
        meteo_all = data.frame(
          Year, Month, n_days, Rg_f, Ta_f, Tmax, Tmin, RH_f, Ts_f, Precip, SWC, LAI, ET, WS_f
        )
        
        meteo_all = meteo_all[order(meteo_all[,1],meteo_all[,2],meteo_all[,3]),]
        
        filename = t
        if ( !grepl(cy_fc,'None') ) {
          filename = paste0(filename,'_fc_',cy_fc)
        }
        if ( !grepl(cy_fd,'None') ) {
          filename = paste0(filename,'_fd_',cy_fd)
        }
        
        cat(paste0("write file: ",filename,'.txt\n'))
        write.table(meteo_all,paste0(output_dir,filename,'.txt'),row.names = FALSE,sep='\t',
                    quote = FALSE,append = FALSE)
        cat(paste0("write file: ",filename,'.txt ... OK\n'))
        
        rm(meteo_all, Year, Month, n_days, Rg_f, Ta_f, Tmax, Tmin, RH_f, Ts_f, Precip, SWC, LAI,ET,WS_f)
        rm(qrt,filename)
        
      }
      
    }
    rm(cy_fd,cy_fc,fd_menu,fc_menu)
  }
  rm(t,meteo_tab)

  # create CO2 file ----

  co2_tab = alltables[grep(paste0("_",cyS), alltables)]
  co2_tab = co2_tab[grep("CO2", co2_tab)]
  
  for (t in co2_tab) {
    
    if ( grepl("METADATA_",t) == 1) {
      next
    }
    
    cat(paste0('import table: ',t,'\n'))
    
    ck_var =  dbGetQuery(con,
                         paste("SELECT * FROM",t)
    )
    
    fc_menu = 'None'
    
    if ( length(grep('forcingConditions',colnames(ck_var))) != 0) {
      # extract forcingConditions option
      fc_menu =  dbGetQuery(con,
                            paste("SELECT forcingConditions FROM",t,"GROUP BY","forcingConditions")
      )
      fc_menu = unlist(fc_menu)
    }
    
    rm(ck_var)
    
    cat(paste0('import table: ',t,'\n'))
    
    for ( cy_fc in fc_menu) {
      
      query_str = paste("SELECT year,co2_ppm FROM",t)
      
      if ( !grepl(cy_fc,'None') ) {
        query_str = paste0(query_str,
                           " WHERE forcingConditions  =\'",cy_fc,"\'")
      }
      
      qrt = dbGetQuery(con, query_str )
      
      year = qrt$year
      CO2_ppm = qrt$co2_ppm
      
      co2_all = data.frame(year,CO2_ppm)
      
      co2_all = co2_all[order(co2_all[,1]),]
      
      cat(paste0("write file: ",t,'_fc_',cy_fc,'.txt\n'))
      
      write.table(co2_all,paste0(output_dir,t,'_fc_',cy_fc,'.txt'),row.names = FALSE,sep='\t',
                  quote = FALSE,append = FALSE)
      
      cat(paste0("write file: ",t,'_fc_',cy_fc,'.txt ... OK\n'))
      
      
      rm(year, CO2_ppm,co2_all)
      rm(qrt,query_str)
    }
    rm(cy_fc,fc_menu)
    
  }
  rm(t,co2_tab)
  
  # check duplicate dataset and move in folder '_identici'
  
  for(cyDes in c('CLIMATE','CO2')) {
    
    # compare all CLIMATE files and delete the similar
    file_all = list.files(path=output_dir,pattern = cyDes)
    
    for ( cy1 in seq(1,length(file_all)) ) {
      
      
      if ( !file.exists(paste0(output_dir,file_all[cy1])) ) {
        next
      }
      
      cat(paste('REFERENCE import file:',file_all[cy1],'\n'))
      
      if ('STAND' == cyDes) {
        df_ref = read.csv(paste0(output_dir,file_all[cy1]),sep = ',')
      } else {
        df_ref = read.csv(paste0(output_dir,file_all[cy1]),sep = '\t')
      }
      
      
      for ( cy2 in seq((cy1 + 1),length(file_all)) ) {
        
        if ( !file.exists(paste0(output_dir,file_all[cy2])) ) {
          next
        }
        
        cat(paste('COMPARE import file:',file_all[cy2],'\n'))
        
        if ( abs(file.info(paste0(output_dir,file_all[cy1]))$size -
            file.info(paste0(output_dir,file_all[cy2]))$size ) > 0) {
          next
        }
        
        if ('STAND' == cyDes) {
          df_cmp = read.csv(paste0(output_dir,file_all[cy2]),sep = ',') 
        } else {
          df_cmp = read.csv(paste0(output_dir,file_all[cy2]),sep = '\t') 
        }
        
        if ( abs (length(df_cmp[,1]) - length(df_ref[,1])) > 0 ){
          rm(df_cmp)
          next
        }
        
        listaV = unique(c(colnames(df_cmp),colnames(df_ref)))
        
        conta_diffe = 0
        
        for( cyv in listaV ) {
          
          M = data.frame('ref' = df_ref[,which(colnames(df_ref) == cyv)],
                'cmp' = df_cmp[,which(colnames(df_cmp) == cyv)])
        
          M_diffe = M[,1]-M[,2]
          M_diffe = M_diffe[-1*which( M_diffe == 0)]
          
          if ( length(M_diffe) > 0 ) {
            conta_diffe = 1
            break
          }
        }
        rm(M_diffe,cyv,listaV)
        rm(df_cmp)
        if ( conta_diffe == 0 ) {
          
          dir.create(paste0(output_dir,'_identici/'),showWarnings = FALSE)
          
          file.copy(paste0(output_dir,file_all[cy2]),
                    paste0(output_dir,'_identici/',file_all[cy2]))
          
          file.remove(paste0(output_dir,file_all[cy2]))
        }
        rm(conta_diffe)
      }
      rm(df_ref)
    }
    rm(cy1,file_all)
  }
  rm(cyDes)
}

# clean up ----
dbDisconnect(con)
rm(con)
rm(lista_siti)

# move climate files in subfolder and rename ----

lista_siti = read.csv(paste0(output_dir_gen,'SITES.csv'))

for ( cy in lista_siti$site ) {
  
  lf = list.files(path=paste0(output_dir_gen,cy,'/'),pattern = 'CLIMATE')
  
  for (cy_f in lf ) {
    
    name = gsub('CLIMATE_ISIMIP','',cy_f)
    name2 = unlist(strsplit(name,'_'))
    
    dir.create(paste0(output_dir_gen,cy,'/',name2[1],'/'),showWarnings = FALSE)
    
    file.copy(paste0(output_dir_gen,cy,'/',cy_f),
              paste0(output_dir_gen,cy,'/',name2[1],'/'))
    
    name3 = c(paste0(name2[1],'_'),paste0(name2[2],'_'))
    
    file.rename(
      paste0(output_dir_gen,cy,'/',name2[1],'/',cy_f),
      paste0(output_dir_gen,cy,'/',name2[1],'/',
             gsub('historical','hist',
             gsub(name3[2],'',gsub(name3[1],'',gsub('CLIMATE_ISIMIP','',cy_f))))))
    
    file.remove(paste0(output_dir_gen,cy,'/',cy_f))
    
    rm(name2,name3,name)
    
  }
  
  rm(lf)

}
rm(cy,lista_siti)

# check begin and end of timeseries ----

lista_siti = read.csv(paste0(output_dir_gen,'SITES.csv'))

list_des_rcp = c('hist','rcp2p6','rcp4p5','rcp6p0','rcp8p5')

cy = lista_siti$site[8]

# for ( cy in lista_siti$site ) {
  # list of files and RCP
  lf = c()
  lf_des = c()
  for (cy_des_rcp in list_des_rcp) {
    lf = c(lf,list.files(path = paste0(output_dir_gen,cy,'/'),pattern = cy_des_rcp))
    lf_des = c(lf_des,
               rep(cy_des_rcp,
                   length(list.files(path = paste0(output_dir_gen,cy,'/'),pattern = cy_des_rcp))))
  }
  rm(cy_des_rcp)
  
  fine_name = c()
  begin_year = c()
  end_year = c()
    
  for (cy2 in lf) {
    
    cat(paste('open file:',cy2,'\n'))
    df = read.csv(paste0(output_dir_gen,cy,'/',cy2),sep='\t')
    fine_name = c(fine_name,cy2)
    
    if ( grepl('^CO2',cy2) ) {
      begin_year = c(begin_year,min(df$year))
      end_year = c(end_year,max(df$year))
    } else {
      begin_year = c(begin_year,min(df$Year))
      end_year = c(end_year,max(df$Year))
    }
    rm(df)
  }
  rm(cy2)
  
  dff = data.frame(fine_name,begin_year,end_year)
  
  for ( cyf in seq(1,length(fine_name)) ) {
    if ( grepl('^CO2',fine_name[cyf]) ) {
      next
    }
    
    file_co2 = fine_name[grep('^CO2',fine_name)]
    file_co2 = file_co2[grep(lf_des[cyf],file_co2)]
    
    cat('\tcreate setting file\n')
          
    # import settings file
    filepath = paste0(output_dir_gen,'Soroe_settings_ISIMIP_Manag-on_CO2-on_test.txt')
    con = file(filepath, "r")
    line = readLines(con)
    close(con)
    
    rm(filepath,con)
    
    line_year_start = line[grep('^YEAR_START ',line)]
    line_year_end = line[grep('^YEAR_END ',line)]
    
    begin_year[cyf] = 1950
    
    line_year_start = gsub(line_year_start,paste0('YEAR_START ',begin_year[cyf]),line_year_start)
    line_year_end = gsub(line_year_end,paste0('YEAR_END ',end_year[cyf]),line_year_end)
    
    line_year_start = paste0(line_year_start,'    //Starting year simulation')
    line_year_end = paste0(line_year_end,'    //Ending year simulation')
    
    line[grep('^YEAR_START ',line)] = line_year_start
    line[grep('^YEAR_END ',line)] = line_year_end
    
    file_setting = paste0('s_M-on_CO2-on_',gsub('.txt','',fine_name[cyf]),'.txt')
    
    filepath = paste0(output_dir_gen,cy,'/',file_setting)

    cat('\tsetting file:',filepath,' CREATED\n')
    
    fileConn<-file(filepath)
    writeLines(line, fileConn)
    close(fileConn)
    
    rm(line,filepath,fileConn)
    
    # create batch file
    str_cmd = '3D-CMCC-Forest-Model '
    str_cmd = paste0(str_cmd,' -i soro/')
    str_cmd = paste0(str_cmd,' -o soro/output_',gsub('.txt','',fine_name[cyf]),'/')
    str_cmd = paste0(str_cmd,' -p parameterization/')
    str_cmd = paste0(str_cmd,' -d Soroe_stand_ISIMIP.txt')
    str_cmd = paste0(str_cmd,' -s Soroe_soil_ISIMIP.txt')
    str_cmd = paste0(str_cmd,' -t Soroe_topo_ISIMIP.txt')
    str_cmd = paste0(str_cmd,' -k ',file_co2)
    str_cmd = paste0(str_cmd,' -c ',file_setting)
    str_cmd = paste0(str_cmd,' -m ',fine_name[cyf])
    
    str_cmd = paste0(str_cmd,'>result_',gsub('.txt','',fine_name[cyf]),'.txt')
    
    filepath = paste0(output_dir_gen,'/launch_',gsub('.txt','',fine_name[cyf]),'.bat')
    
    fileConn<-file(filepath)
    writeLines(str_cmd, fileConn)
    close(fileConn)
    
    file.copy(paste0(output_dir_gen,'/Soroe_stand_ISIMIP.txt'),
              paste0(output_dir_gen,'/',cy,'/Soroe_stand_ISIMIP.txt'))
    file.copy(paste0(output_dir_gen,'/Soroe_soil_ISIMIP.txt'),
              paste0(output_dir_gen,'/',cy,'/Soroe_soil_ISIMIP.txt'))
    file.copy(paste0(output_dir_gen,'/Soroe_topo_ISIMIP.txt'),
              paste0(output_dir_gen,'/',cy,'/Soroe_topo_ISIMIP.txt'))
    
    old_wd = getwd()
    
    setwd(output_dir_gen)
    system(paste0('launch_',gsub('.txt','',fine_name[cyf]),'.bat'))
  
    setwd(old_wd)
    
    rm(filepath,str_cmd,fileConn,old_wd)
    
    file.remove(paste0(output_dir_gen,'/',cy,'/Soroe_stand_ISIMIP.txt'))
    file.remove(paste0(output_dir_gen,'/',cy,'/Soroe_soil_ISIMIP.txt'))
    file.remove(paste0(output_dir_gen,'/',cy,'/Soroe_topo_ISIMIP.txt'))
    
  }
  rm(cyf)
}
rm(cy_des_rcp)
  
rm(cy)


# 
# # create input directory and launch the model ----
# 
# setwd('c:/Users/CMCC/Desktop/profound/out_R_tool/')
# shell(paste0('3D-CMCC-Forest-Model.exe',
#              ' -i input_default/',
#              ' -o output_default2/',
#              ' -p input_default/',
#              ' -d Soroe_stand_ISIMIP.txt',
#              ' -m GCM1_rcp8p5.txt',
#              ' -s Soroe_soil_ISIMIP.txt',
#              ' -t Soroe_topo_ISIMIP.txt',
#              ' -c Soroe_settings_ISIMIP_Manag-on_CO2-on.txt',
#              ' -k CO2_rcp8p5_1950_2099.txt',
#              '>result_default3.txt'))
# 
# system('launch_test.bat')
# 

# create input directory and launch the model
# 
# list_meteo_in = list.files(output_dir,pattern='CLIMATE_')
# list_co2_in = list.files(output_dir,pattern='CO2_')
# cy_meteo = list_meteo_in[1]
# cy_co2 = list_co2_in[1]
# 
# for ( cy_meteo in list_meteo_in ) {
#   for ( cy_co2 in list_co2_in ) {
#     dir_tmp = paste0(output_dir,
#                    gsub('.txt','',cy_meteo),'_c_',gsub('.txt','',cy_co2))
#     dir.create(dir_tmp)
#     dir.create(paste0(dir_tmp,'\\output\\'))
#     # copy default data in input dir
#     file.copy('c:\\Users\\CMCC\\Desktop\\profound\\out_R_tool\\input_default\\Fagussylvatica.txt',
#             dir_tmp)    
#     file.copy('c:\\Users\\CMCC\\Desktop\\profound\\out_R_tool\\input_default\\Soroe_soil_ISIMIP.txt',
#               dir_tmp)    
#     file.copy('c:\\Users\\CMCC\\Desktop\\profound\\out_R_tool\\input_default\\Soroe_settings_ISIMIP_Manag-on_CO2-on.txt',
#               dir_tmp)    
#     file.copy('c:\\Users\\CMCC\\Desktop\\profound\\out_R_tool\\input_default\\Soroe_topo_ISIMIP.txt',
#               dir_tmp)    
#     file.copy('c:\\Users\\CMCC\\Desktop\\profound\\out_R_tool\\input_default\\Soroe_stand_ISIMIP.txt',
#               dir_tmp)    
#     
#     file.copy(paste0(output_dir,cy_meteo),dir_tmp)    
#     file.copy(paste0(output_dir,cy_co2),dir_tmp)  
#     
#     # move the file in input directory
#     str_sys = '3D-CMCC-Forest-Model'
#     str_sys = paste0(str_sys,' -i "',dir_tmp,'\\"')
#     str_sys = paste0(str_sys,' -o "',paste0(dir_tmp,'\\output\\"'))
#     str_sys = paste0(str_sys,' -p "',dir_tmp,'\\"')
#     str_sys = paste0(str_sys,' -d Soroe_stand_ISIMIP.txt')
#     str_sys = paste0(str_sys,' -m ',cy_meteo)
#     str_sys = paste0(str_sys,' -s Soroe_soil_ISIMIP.txt')
#     str_sys = paste0(str_sys,' -t Soroe_topo_ISIMIP.txt')
#     str_sys = paste0(str_sys,' -c Soroe_settings_ISIMIP_Manag-on_CO2-on.txt')
#     str_sys = paste0(str_sys,' -k ',cy_co2)
#     
#     str_sys = paste0(str_sys,' >result_',gsub('.txt','',cy_meteo),'_c_',gsub('.txt','',cy_co2),'.txt')
#     
#     str_sys = gsub('\\\\','/',str_sys)
#     str_sys = gsub('\\','/',str_sys)
#     str_sys = gsub('//','/',str_sys)
#     
#     cat(str_sys)
#     
#     #launch the model
#     cat(paste('\n\n===================\n launch 3D-CMCC with:\n\tmeteo: ',cy_meteo,'\n\tCO2:',cy_co2,'\n'))
#     system(str_sys)
#     cat(paste('\n\n launch 3D-CMCC with:\n\tmeteo: ',cy_meteo,'\n\tCO2:',cy_co2,'\n\tDONE\n===================\n'))
#   }
# }
# 

  

