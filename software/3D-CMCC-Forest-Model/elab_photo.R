rm(list = ls())

# setwd('/media/carlo/A464024564021AA6/SviluppoR/')
library(ggplot2)
library(cowplot)
library(matrixStats)
library(ggrepel)
library(lubridate)
library(rstudioapi)

setwd(dirname(rstudioapi::getActiveDocumentContext()$path))

source('readOutput_model.R')

# dir_in = '/home/alessio/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/output/'
# dir_out = '/home/alessio/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/output_per_colin/'
# dir_valid_eddy = '/home/carlo/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/eddy_validation/'
dir_valid_eddy ='/home/alessio/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/eddy_validation/'
dir_in_main = '/home/alessio/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/output/'#/media/carlo/8CB8D0E1B8D0CABA/Photo-approach/'#'/media/carlo/8CB8D0E1B8D0CABA/risultati_colin/output/'
dir_out_main = '/media/carlo/8CB8D0E1B8D0CABA/Photo-approach/results_new/'
dir_out_main = '/home/alessio/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/output/result_27_gen_2018/'
dir.create(dir_out_main,showWarnings = F)

list_var = c('gpp','npp','MaxLAI')
# list_var = c("solar_rad","tavg","tmax","tmin","vpd","prcp","tsoil","rh","avg_asw","X.CO2.","gpp","npp","MaxLAI")

# lista dei file LOCAL DAILY e ANNUAL ----

for ( cy_time in c('annual','daily') ) {
  
  lf = list.files(dir_in_main,recursive = T,full.names = T,pattern = cy_time)
  
  if ( length(grep('results',lf)) > 0 ) {
    lf = lf[-1*grep('results',lf)]
  }
  if ( length(grep('.pdf',lf)) > 0 ) {
    lf = lf[-1*grep('.pdf',lf)]
  }
  if ( length(grep('_log_',lf)) > 0 ) {
    lf = lf[-1*grep('_log_',lf)]
  }
  if ( length(grep('Peitz',lf)) > 0 ) {
    lf = lf[-1*grep('Peitz',lf)]
  }
  if ( length(grep('Solling',lf)) > 0 ) {
    lf = lf[-1*grep('Solling',lf)]
  }
  
  lf = lf[grep('LOCAL',lf)]
  
  meth = c()
  meth_unc = c()
  exp = c()
  sito = c()
  esm = c()
  esm_rcp = c()
  co2 = c()
  co2_rcp = c()
  man = c()
  
  for ( cyf in lf ) {
    tmp = gsub(dir_in_main,'',cyf)
    tmp = unlist(strsplit(tmp,'/'))
    
    meth = c(meth, unlist(strsplit(tmp[3],'-'))[2])# metodo di fotosintesi
    # if ( length(grep('less20',cyf)) > 0) {
    #   meth_unc = c(meth_unc, 'less20')#incertezza 
    # } else if ( length(grep('plus20',cyf)) > 0) {
    #   meth_unc = c(meth_unc, 'plus20')#incertezza 
    # } else {
      meth_unc = c(meth_unc, 'opt')#incertezza
    # }
    # 
    exp = c(exp, unlist(strsplit(tmp[3],'-'))[1])
    
    nome_file = basename(cyf)
    
    nome_file = gsub('.txt','',nome_file)
    nome_file = gsub('_d_10000','',nome_file)
    nome_file = unlist(strsplit(nome_file,'_'))
    nome_file = nome_file[-1*seq(1,3)]
    pos_exp = grep('LOCAL',nome_file)
    sito = c(sito, paste(nome_file[1:(pos_exp-1)],collapse = '_'))
    esm = c(esm, nome_file[pos_exp+1])
    pos_exp = grep('CO2',nome_file)[1]
    co2 = c(co2, nome_file[pos_exp+1])
    pos_exp = grep('CO2',nome_file)[2]
    co2_rcp = c(co2_rcp,nome_file[pos_exp+1])
    man = c(man, nome_file[length(nome_file)])
    
    rm(tmp,pos_exp,nome_file)
  }
  rm(cyf)
  
  df_file = data.frame(lf,meth,meth_unc,exp,sito,esm,co2,co2_rcp,man)
  # df_file = data.frame(lf,meth,exp,sito,esm,co2,co2_rcp,man)
  rm(lf,meth,meth_unc,exp,sito,esm,co2,co2_rcp,man)
  
  lista_sito = as.character(sort(unique(df_file$sito)))
  lista_esm = as.character(sort(unique(df_file$esm)))
  lista_co2 = as.character(sort(unique(df_file$co2)))
  lista_co2_rcp = as.character(sort(unique(df_file$co2_rcp)))
  lista_man = as.character(sort(unique(df_file$man)))
  lista_exp = as.character(sort(unique(df_file$exp)))
  lista_meth = as.character(sort(unique(df_file$meth)))
  lista_meth_unc = as.character(sort(unique(df_file$meth_unc)))

  # importo i dati LOCAL ----
  
  dir_out1 = paste0(dir_out_main,'extract_LOCAL_',cy_time,'/')
  dir.create(dir_out1,showWarnings = F)
  
  extract_file = 1
  
  if (extract_file == 1) {
    file_extr = c()
    for (cyf in seq(1,length(df_file[,1])) ) {
      
      cat(sprintf('import file: %d / %d %s\n',cyf,length(df_file[,1]),cy_time))
      # importo le informazioni del report in fond al file
      
      df = read.csv(as.character(df_file$lf[cyf]),comment.char = '#')
      if ( is.factor(df$YEAR) ) {
        pos_na = which(is.na(df$LAYER) == 1)
        if ( length(pos_na) > 0 ) {
          df = df[-1*pos_na,]
          df$YEAR = as.numeric(as.character(df$YEAR))
        }
        rm(pos_na)
      }
      if (cy_time == 'annual') {
        col_to_use = c('YEAR')
      }
      if (cy_time == 'daily') {
        col_to_use = c('YEAR','MONTH','DAY')
      }
      
      for (cy_v in list_var) {
        pos_v = which(colnames(df) == cy_v)
        if ( length(pos_v) > 0 ) {
          col_to_use = c(col_to_use,cy_v)
        }
        rm(pos_v)
      }
      rm(cy_v)
      df_f = df[,col_to_use]
      rm(col_to_use)
      # salvo il file
      write.csv(x = df_f,file = paste0(dir_out1,
                                       paste(as.character(df_file$meth[cyf]),
                                             as.character(df_file$meth_unc[cyf]),
                                             as.character(df_file$exp[cyf]),
                                             as.character(df_file$esm[cyf]),
                                             'CO2',
                                             as.character(df_file$co2[cyf]),
                                             as.character(df_file$co2_rcp[cyf]),
                                             'Man',
                                             as.character(df_file$man[cyf]),
                                             as.character(df_file$sito[cyf]),sep = '_',collapse = '_'
                                       ),'.csv'),row.names = F)
      file_extr = c(file_extr,
                    paste0(dir_out1,
                           paste(as.character(df_file$meth[cyf]),
                                 as.character(df_file$meth_unc[cyf]),
                                 as.character(df_file$exp[cyf]),
                                 as.character(df_file$esm[cyf]),
                                 'CO2',
                                 as.character(df_file$co2[cyf]),
                                 as.character(df_file$co2_rcp[cyf]),
                                 'Man',
                                 as.character(df_file$man[cyf]),
                                 as.character(df_file$sito[cyf]),sep = '_',collapse = '_'
                           ),'.csv'
                    )
      )
      rm(df_f,df)
    }
    rm(cyf)
    
    df_file$file_extr = file_extr
    rm(file_extr)
    write.csv(x = df_file, file = paste0(dir_out_main,'db_file_local_',cy_time,'.csv'),row.names = F)
  }
  
  df_file = read.csv(paste0(dir_out_main,'db_file_local_',cy_time,'.csv'))
  
  # calcolo le medie,max e min per LOCAL (con e senza parametri)----
  
  calc_stat = 1
  if ( calc_stat == 1) {
    lista_meth_unc = 'opt'
    for ( cy_meth_unc in lista_meth_unc ) {
      
      dir_out1 = paste0(dir_out_main,'local_mean_',cy_meth_unc,'_',cy_time,'/')
      dir.create(dir_out1,showWarnings = F)
      dir_out2 = paste0(dir_out_main,'local_max_',cy_meth_unc,'_',cy_time,'/')
      dir.create(dir_out2,showWarnings = F)
      dir_out3 = paste0(dir_out_main,'local_min_',cy_meth_unc,'_',cy_time,'/')
      dir.create(dir_out3,showWarnings = F)
      
      for (cy_meth in lista_meth) {
        for (cy_exp in lista_exp) {
          for ( cy_co2 in lista_co2 ) {
            for ( cy_co2_rcp in lista_co2_rcp ) {
              for ( cy_man in lista_man ) {
                for (cy_sito in lista_sito) {
                  
                  df_file2 = df_file
                  
                  pos_1 = which(df_file2$meth_unc == cy_meth_unc)
                  if ( length(pos_1) == 0 ) next
                  df_file2 = df_file2[pos_1,]
                  
                  pos_1 = which(df_file2$meth == cy_meth)
                  if ( length(pos_1) == 0 ) next
                  df_file2 = df_file2[pos_1,]
                  
                  pos_1 = which(df_file2$exp   == cy_exp)
                  if ( length(pos_1) == 0 ) next
                  df_file2 = df_file2[pos_1,]
                  
                  pos_1 = which(df_file2$co2   == cy_co2)
                  if ( length(pos_1) == 0 ) next
                  df_file2 = df_file2[pos_1,]
                  
                  pos_1 = which(df_file2$co2_rcp  == cy_co2_rcp)
                  if ( length(pos_1) == 0 ) next
                  df_file2 = df_file2[pos_1,]
                  
                  pos_1 = which(df_file2$man   == cy_man)
                  if ( length(pos_1) == 0 ) next
                  df_file2 = df_file2[pos_1,]
                  
                  pos_1 = which(df_file2$sito   == cy_sito)
                  if ( length(pos_1) == 0 ) next
                  df_file2 = df_file2[pos_1,]
                  
                  # calcolo le medie
                  cat(sprintf(paste0('calculate stat: %s\n',
                              '\tmeth_unc: %d / %d\n',
                              '\tmeth: %d / %d\n',
                              '\texp: %d / %d\n',
                              '\tco2: %d / %d\n',
                              '\tco2_rcp: %d / %d\n',
                              '\tman: %d / %d\n',
                              '\tsite: %d / %d\n'),
                              cy_time,
                              grep(cy_meth_unc,lista_meth_unc),length(lista_meth_unc),
                              grep(cy_meth,lista_meth),length(lista_meth),
                              grep(cy_exp,lista_exp), length(lista_exp),
                              grep(cy_co2,lista_co2), length(lista_co2),
                              grep(cy_co2_rcp,lista_co2_rcp), length(lista_co2_rcp),
                              grep(cy_man,lista_man), length(lista_man),
                              grep(cy_sito, lista_sito), length(lista_sito)))
                              
                  
                  if ( exists('df_t') ) {
                    rm(df_t)
                  }
                  for (cy_sub in df_file2$file_extr) {
                    df = read.csv(as.character(cy_sub))
                    if ( exists('df_t') ) {
                      df_t = rbind(df_t,df)
                    } else {
                      df_t = df
                    }
                    rm(df)
                  }
                  rm(cy_sub)
                  
                  # calcolo le statistiche
                  if (cy_time == 'daily') { 
                    unique_year = sort(unique(df_t$YEAR))
                    df_mean = df_t[1,]
                    df_max = df_t[1,]
                    df_min = df_t[1,]
                    for (cy_year in unique_year) {
                      for (cy_month in seq(1,12)) {
                        for (cy_day in seq(1,31) ) {
                          pos_year = which(df_t$YEAR == cy_year &
                                             df_t$MONTH == cy_month &
                                             df_t$DAY == cy_day)
                          if ( length(pos_year) == 0 ) {
                            next
                          }
                          df_mean = rbind(df_mean,
                                          colMeans(df_t[pos_year,],na.rm = T))
                          df_max = rbind(df_max,
                                         colMaxs(as.matrix(df_t[pos_year,]),na.rm = T))
                          df_min = rbind(df_min,
                                         colMins(as.matrix(df_t[pos_year,]),na.rm = T))
                        }
                        rm(cy_day)
                      }
                      rm(cy_month)
                    }
                    rm(cy_year,pos_year)
                    df_mean = df_mean[-1,]
                    df_max = df_max[-1,]
                    df_min = df_min[-1,]
                  }
                  if (cy_time == 'annual') {
                    unique_year = sort(unique(df_t$YEAR))
                    df_mean = df_t[1,]
                    df_max = df_t[1,]
                    df_min = df_t[1,]
                    for (cy_year in unique_year) {
                      pos_year = which(df_t$YEAR == cy_year)
                      df_mean = rbind(df_mean,
                                      colMeans(df_t[pos_year,],na.rm = T))
                      df_max = rbind(df_max,
                                     colMaxs(as.matrix(df_t[pos_year,]),na.rm = T))
                      df_min = rbind(df_min,
                                     colMins(as.matrix(df_t[pos_year,]),na.rm = T))
                    }
                    rm(cy_year,pos_year)
                    df_mean = df_mean[-1,]
                    df_max = df_max[-1,]
                    df_min = df_min[-1,]
                  }
                  # salvo i risultati
                  write.csv(df_mean,paste0(dir_out1,
                                           cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'),
                            row.names = F,quote = F)
                  write.csv(df_max,paste0(dir_out2,
                                          cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'),
                            row.names = F,quote = F)
                  write.csv(df_min,paste0(dir_out3,
                                          cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'),
                            row.names = F,quote = F)
                  rm(unique_year,df_mean,df_max,df_min,df_t)
                }
                rm(cy_sito)
              }
              rm(cy_man)
              
            }
            rm(cy_co2_rcp)
          }
          rm(cy_co2)
        }
        rm(cy_exp)
      }
      rm(cy_meth,df_file2)
      rm(dir_out1,dir_out2,dir_out3)
    }
    rm(cy_meth_unc)
  }
  rm(calc_stat)
  
}
rm(cy_time)

# plot di validazione per i risultati LOCAL ----

dir_out1 = paste0(dir_out_main,'validation/')
dir.create(dir_out1,showWarnings = F) 

for (cys in lista_sito) {
  if ( cys == "Bily_Kriz" ) des_eddy = 'CZ-BK1'
  if ( cys == "Hyytiala" )  des_eddy = 'FI-Hyy'
  if ( cys == "LeBray" )    des_eddy = 'FR-LBr'
  if ( cys == "Soroe" )     des_eddy = 'DK-Sor'
  
  # importo il file giornaliero eddy
  file_eddy = list.files(dir_valid_eddy,pattern = des_eddy)
  df_eddy = read.csv(paste0(dir_valid_eddy,file_eddy))
  
  df_eddy = df_eddy[,c('TIMESTAMP','GPP_DT_CUT_USTAR50','GPP_DT_CUT_05','GPP_DT_CUT_95','NEE_CUT_USTAR50_QC')]
  
  pos_na = which(df_eddy$NEE_CUT_USTAR50_QC < 0.5)
  if ( length(pos_na) > 0 ) df_eddy[pos_na,] = NA
  
  pos_na = which(df_eddy$GPP_DT_CUT_USTAR50 < -9990)
  if ( length(pos_na) > 0 ) df_eddy$GPP_DT_CUT_USTAR50[pos_na] = NA
  
  pos_na = which(df_eddy$GPP_DT_CUT_05 < -9990)
  if ( length(pos_na) > 0 ) df_eddy$GPP_DT_CUT_05[pos_na] = NA
  
  pos_na = which(df_eddy$GPP_DT_CUT_95 < -9990)
  if ( length(pos_na) > 0 ) df_eddy$GPP_DT_CUT_95[pos_na] = NA
  
  rm(file_eddy)
  rm(pos_na)
  
  # separo la data
  df_eddy$TIME = ymd(df_eddy$TIMESTAMP)
  df_eddy$YEAR = year(df_eddy$TIME)
  df_eddy$MONTH = month(df_eddy$TIME)
  df_eddy$DAY = day(df_eddy$TIME)
  
  # importo il file giornaliero mediato per gli ESM
  lf_md = list.files(paste0(dir_out_main,'extract_LOCAL_daily/'),pattern = cys)
  
  for ( cy_mt in c('FvCB','LUE') ) {
    
    lf_md_opt = lf_md[grep('opt',lf_md)]
    lf_md_mth_opt = lf_md_opt[grep(cy_mt,lf_md_opt)]
    
    # lf_md_less = lf_md[grep('less',lf_md)]
    # lf_md_mth_less = lf_md_less[grep(cy_mt,lf_md_less)]
    # 
    # lf_md_plus = lf_md[grep('plus',lf_md)]
    # lf_md_mth_plus = lf_md_plus[grep(cy_mt,lf_md_plus)]
    # 
    df_opt = read.csv(paste0(dir_out_main,'extract_LOCAL_daily/',lf_md_mth_opt))
    # df_less = read.csv(paste0(dir_out_main,'extract_LOCAL_daily/',lf_md_mth_less))
    # df_plus = read.csv(paste0(dir_out_main,'extract_LOCAL_daily/',lf_md_mth_plus))
    
    if (cy_mt == 'FvCB') {
      df_FvCB_opt = df_opt
      # df_FvCB_less = df_less
      # df_FvCB_plus = df_plus
    }
    if (cy_mt == 'LUE') {
      df_Fra_opt = df_opt
      # df_Fra_less = df_less
      # df_Fra_plus = df_plus
    }
    # if (cy_mt == 'LUE-Ver') {
    #   df_Ver_opt = df_opt
    #   df_Ver_less = df_less
    #   df_Ver_plus = df_plus
    # }
    rm(df_opt,lf_md_opt,lf_md_mth_opt)
    # rm(df_less,lf_md_less,lf_md_mth_less)
    # rm(df_plus,lf_md_plus,lf_md_mth_plus)
  }
  rm(cy_mt)
  
  # sincronizzo le serie temporali
  range_year = c(
    min(c(df_eddy$YEAR,df_FvCB_opt$YEAR, df_Fra_opt$YEAR#, df_Ver_opt$YEAR
          #df_eddy$YEAR,df_FvCB_less$YEAR,df_Ver_less$YEAR,df_Fra_less$YEAR,
          #df_eddy$YEAR,df_FvCB_plus$YEAR,df_Ver_plus$YEAR,df_Fra_plus$YEAR
          ),na.rm = T),
    max(c(df_eddy$YEAR,df_FvCB_opt$YEAR, df_Fra_opt$YEAR#, df_Ver_opt$YEAR
          #df_eddy$YEAR,df_FvCB_less$YEAR,df_Ver_less$YEAR,df_Fra_less$YEAR,
          #df_eddy$YEAR,df_FvCB_plus$YEAR,df_Ver_plus$YEAR,df_Fra_plus$YEAR
          ),na.rm = T)
  )
  time_serie = seq( ymd(101+(range_year[1]*10000)),ymd(1231+(range_year[2]*10000)),by = 1)
  time_serie = data.frame('YEAR' = year(time_serie), 'MONTH' = month(time_serie), 'DAY' = day(time_serie))
  
  GPP_DT_CUT_USTAR50 = c()
  GPP_DT_CUT_05 = c()
  GPP_DT_CUT_95 = c()
  
  # sincronizzo eddy
  for ( cyt in seq(1,length(time_serie[,1])) ) {
    pos_time = which(df_eddy$YEAR == time_serie$YEAR[cyt] &
                       df_eddy$MONTH == time_serie$MONTH[cyt] &
                       df_eddy$DAY == time_serie$DAY[cyt] 
    )
    if ( length(pos_time) == 0 ) {
      GPP_DT_CUT_USTAR50 = c(GPP_DT_CUT_USTAR50,NA)
      GPP_DT_CUT_05 = c(GPP_DT_CUT_05,NA)
      GPP_DT_CUT_95 = c(GPP_DT_CUT_95,NA)
    } else {
      GPP_DT_CUT_USTAR50 = c(GPP_DT_CUT_USTAR50,df_eddy$GPP_DT_CUT_USTAR50[pos_time])
      GPP_DT_CUT_95 = c(GPP_DT_CUT_95,df_eddy$GPP_DT_CUT_95[pos_time])
      GPP_DT_CUT_05 = c(GPP_DT_CUT_05,df_eddy$GPP_DT_CUT_05[pos_time])
    }
    rm(pos_time)
  }
  rm(cyt)
  #sincronizzo i dati del modello
  for ( zz in seq(1,2) ) {
    if ( zz == 1 ) df = df_FvCB_opt
    # if ( zz == 2 ) df = df_Ver_opt
    if ( zz == 2 ) df = df_Fra_opt
    # if ( zz == 4 ) df = df_FvCB_less
    # if ( zz == 5 ) df = df_Ver_less
    # if ( zz == 6 ) df = df_Fra_less
    # if ( zz == 7 ) df = df_FvCB_plus
    # if ( zz == 8 ) df = df_Ver_plus
    # if ( zz == 9 ) df = df_Fra_plus
    gpp_tmp = c()
    for ( cyt in seq(1,length(time_serie[,1])) ) {
      pos_time = which(df$YEAR == time_serie$YEAR[cyt] &
                         df$MONTH == time_serie$MONTH[cyt] &
                         df$DAY == time_serie$DAY[cyt] 
      )
      if ( length(pos_time) == 0 ) {
        gpp_tmp = c(gpp_tmp,NA)
      } else {
        gpp_tmp = c(gpp_tmp,df$gpp[pos_time])
      }
      rm(pos_time)
    }
    rm(cyt)
    if ( zz == 1 ) GPP_FvCB_OPT = gpp_tmp
    # if ( zz == 2 ) GPP_Ver_OPT = gpp_tmp
    if ( zz == 2 ) GPP_Fra_OPT = gpp_tmp
    # if ( zz == 4 ) GPP_FvCB_LESS = gpp_tmp
    # if ( zz == 5 ) GPP_Ver_LESS = gpp_tmp
    # if ( zz == 6 ) GPP_Fra_LESS = gpp_tmp
    # if ( zz == 7 ) GPP_FvCB_PLUS = gpp_tmp
    # if ( zz == 8 ) GPP_Ver_PLUS = gpp_tmp
    # if ( zz == 9 ) GPP_Fra_PLUS = gpp_tmp
    rm(gpp_tmp,df)
  }
  rm(zz)
  
  # creo il df per il plot
  df_plot = data.frame(
    'YEAR' = time_serie$YEAR,
    'MONTH' = time_serie$MONTH,
    'DAY' = time_serie$DAY,
    GPP_DT_CUT_USTAR50,GPP_DT_CUT_05,GPP_DT_CUT_95,
    GPP_FvCB_OPT,#,GPP_FvCB_LESS,GPP_FvCB_PLUS,
    # GPP_Ver_OPT,GPP_Ver_LESS,GPP_Ver_PLUS,
    GPP_Fra_OPT#,GPP_Fra_LESS,GPP_Fra_PLUS
  )
  
  rm(GPP_DT_CUT_USTAR50,GPP_DT_CUT_05,GPP_DT_CUT_95)
  rm(GPP_FvCB_OPT)#,GPP_FvCB_LESS,GPP_FvCB_PLUS)
  # rm(GPP_Ver_OPT,GPP_Ver_LESS,GPP_Ver_PLUS)
  rm(GPP_Fra_OPT)#,GPP_Fra_LESS,GPP_Fra_PLUS)
  rm(df_FvCB_opt)#,df_FvCB_plus,df_FvCB_less)
  # rm(df_Ver_opt,df_Ver_plus,df_Ver_less)
  rm(df_Fra_opt)#,df_Fra_plus,df_Fra_less)
  rm(time_serie)
  # elimino le righe che hanno valori NA
  for(cyc in colnames(df_plot)) {
    pos_na = which(is.na(df_plot[,cyc]) == 1)
    if ( length(pos_na) > 0 ) {
      df_plot = df_plot[-1*pos_na,]
    }
    rm(pos_na)
  }
  rm(cyc)
  
  # salvo i dati per le validazioni
  write.csv(x = df_plot,file = paste0(dir_out1,cys,'_daily_validation.txt'),row.names = F)
  
  # calcolo la somma mensile
  df_plot_m = df_plot[1,]
  unique_year = sort(unique(df_plot$YEAR))
  tmp_year = c()
  tmp_month = c()
  for (cyy in unique_year) {
    for (cym in seq(1,12)) {
      pos_my = which(df_plot$YEAR == cyy & df_plot$MONTH == cym)
      if (length(pos_my) == 0) next
      tmp_year = c(tmp_year,cyy)
      tmp_month = c(tmp_month,cym)
      df_plot_m = rbind(df_plot_m,colSums(df_plot[pos_my,],na.rm = T))
    }
    rm(cym,pos_my)
  }
  rm(unique_year)
  df_plot_m = df_plot_m[-1,]
  df_plot_m$YEAR = tmp_year
  df_plot_m$MONTH = tmp_month
  rm(tmp_year,tmp_month)
  
  # calcolo il mese medio
  df_plot_m2 = df_plot_m[1,]
  tmp_FvCB_unc_max = c()
  tmp_FvCB_unc_min = c()
  tmp_Ver_unc_max = c()
  tmp_Ver_unc_min = c()
  tmp_Fra_unc_max = c()
  tmp_Fra_unc_min = c()
  tmp_ec_unc_max = c()
  tmp_ec_unc_min = c()
  for (cym in seq(1,12)) {
    pos_my = which(df_plot_m$MONTH == cym)
    if (length(pos_my) == 0) next
    # tmp_FvCB_unc_max = c(tmp_FvCB_unc_max,colMaxs(as.matrix(df_plot_m$GPP_FvCB_PLUS[pos_my]),na.rm = T))
    # tmp_FvCB_unc_min = c(tmp_FvCB_unc_min,colMins(as.matrix(df_plot_m$GPP_FvCB_LESS[pos_my]),na.rm = T))
    # tmp_Ver_unc_max = c(tmp_Ver_unc_max,colMaxs(as.matrix(df_plot_m$GPP_Ver_PLUS[pos_my]),na.rm = T))
    # tmp_Ver_unc_min = c(tmp_Ver_unc_min,colMins(as.matrix(df_plot_m$GPP_Ver_LESS[pos_my]),na.rm = T))
    # tmp_Fra_unc_max = c(tmp_Fra_unc_max,colMaxs(as.matrix(df_plot_m$GPP_Fra_PLUS[pos_my]),na.rm = T))
    # tmp_Fra_unc_min = c(tmp_Fra_unc_min,colMins(as.matrix(df_plot_m$GPP_Fra_LESS[pos_my]),na.rm = T))
    tmp_ec_unc_max =  c(tmp_ec_unc_max,colMaxs(as.matrix(df_plot_m$GPP_DT_CUT_95[pos_my]),na.rm = T))
    tmp_ec_unc_min =  c(tmp_ec_unc_min,colMins(as.matrix(df_plot_m$GPP_DT_CUT_05[pos_my]),na.rm = T))
    df_plot_m2 = rbind(df_plot_m2,
                       colMeans(df_plot_m[pos_my,],na.rm = T))
  }
  rm(cym,pos_my)
  df_plot_m2 = df_plot_m2[-1,]
  
  # df_plot_m2$GPP_FvCB_PLUS = tmp_FvCB_unc_max
  # df_plot_m2$GPP_FvCB_LESS = tmp_FvCB_unc_min
  # df_plot_m2$GPP_Ver_PLUS = tmp_Ver_unc_max
  # df_plot_m2$GPP_Ver_LESS = tmp_Ver_unc_min
  # df_plot_m2$GPP_Fra_PLUS = tmp_Fra_unc_max
  # df_plot_m2$GPP_Fra_LESS = tmp_Fra_unc_min
  df_plot_m2$GPP_DT_CUT_05 = tmp_ec_unc_min
  df_plot_m2$GPP_DT_CUT_95 = tmp_ec_unc_max
  
  # rm(tmp_FvCB_unc_max,tmp_FvCB_unc_min,tmp_Ver_unc_max,tmp_Ver_unc_min,tmp_Fra_unc_max,tmp_Fra_unc_min)
  rm(tmp_ec_unc_max,tmp_ec_unc_min)
  
  # traccio i plot di validazione
  mp1 = ggplot(df_plot_m2) +
    geom_ribbon(aes(x = MONTH, ymax = GPP_DT_CUT_95, ymin = GPP_DT_CUT_05, fill='EC-Uncertainty'),alpha = 0.4) +
    # geom_ribbon(aes(x = MONTH, ymax = GPP_Ver_PLUS, ymin = GPP_Ver_LESS,fill='Ver-Uncertainty'),alpha = 0.4) +
    # geom_ribbon(aes(x = MONTH, ymax = GPP_Fra_PLUS, ymin = GPP_Fra_LESS,fill='Fra-Uncertainty'),alpha = 0.4) +
    # geom_ribbon(aes(x = MONTH, ymax = GPP_FvCB_PLUS, ymin = GPP_FvCB_LESS,fill='FvCB-Uncertainty'),alpha = 0.4) +
    # geom_line(aes(x = MONTH, y = GPP_Ver_OPT,color = 'LUE-Opt-Ver')) +
    geom_line(aes(x = MONTH, y = GPP_Fra_OPT,color = 'LUE-Opt-Fra')) +
    geom_line(aes(x = MONTH, y = GPP_DT_CUT_USTAR50,color = 'EC')) +
    geom_line(aes(x = MONTH, y = GPP_FvCB_OPT,color = 'FvCB-Optimized')) +
    scale_color_manual(values = c('FvCB-Optimized' = 'red','LUE-Opt-Ver' = 'blue','LUE-Opt-Fra' = 'green','EC' ='black')) +
    scale_fill_manual(values =  c('FvCB-Uncertainty' = 'red','Ver-Uncertainty' = 'blue',
                                  'Fra-Uncertainty' = 'green','EC-Uncertainty' ='black')) +
    ggtitle(cys) +
    scale_x_continuous(breaks = seq(1,12),labels = c('J','F','M','A','M','J','J','A','S','O','N','D')) +
    theme_bw() +
    theme (panel.grid.major = element_blank(), panel.grid.minor = element_blank(),
           legend.title = element_blank(),legend.position = c(0.07,0.85),axis.title.x = element_blank(),
           panel.border = element_rect(linetype = 'solid',
                                       fill = NA, color='black',size=0.5))+
    ylab('GPP (gC m-2 month-1)') +
    guides(color = guide_legend(order=1),
           fill = guide_legend(order=2))
  
  ggsave(
    filename = paste0(dir_out1,cys,'_validation.png'),
    plot = mp1,
    scale=1.8,
    width = 30,
    height = 15,
    units = "cm",
    device = 'png',
    dpi = 300
  )
  rm(df_plot,df_plot_m,df_plot_m2,mp1)
  rm(range_year,cyy,des_eddy)
}
rm(cys)

# plot di regressione tra i dati giornalieri EDDY e modello ----

dir_out1 = paste0(dir_out_main,'validation/')
dir.create(dir_out1,showWarnings = F)

for (cys in lista_sito) {
  
  df_plot = read.csv(paste0(dir_out1,cys,'_daily_validation.txt'))
  # calcolo la regr lineare tra eddy e i risultati del modello
  fitted_y = c()
  vx = c()
  vy = c()
  fitted_des = c()
  site_des = c()
  
  for ( cy_col in c("GPP_FvCB_OPT","GPP_Fra_OPT") ) {#,"GPP_Ver_OPT")) {
    tmp = df_plot[,c('GPP_DT_CUT_USTAR50',cy_col)]
    colnames(tmp) = c('vx','vy')
    pos_na = which(is.na(tmp$vy) == 1)
    if ( length(pos_na) > 0 ) {
      tmp = tmp[-1*pos_na,]
    }
    rm(pos_na)
    pos_na = which(is.na(tmp$vx) == 1)
    if ( length(pos_na) > 0 ) {
      tmp = tmp[-1*pos_na,]
    }
    rm(pos_na)
    
    lin_regr = lm(vy ~ vx, data = tmp)
    stat_rcp = cor.test(tmp$vx,tmp$vy)
    p_val = ''
    if ( stat_rcp$p.value < 0.05 ) p_val = paste0(p_val,'*')
    if ( stat_rcp$p.value < 0.01 ) p_val = paste0(p_val,'*')
    if ( stat_rcp$p.value < 0.001 ) p_val = paste0(p_val,'*')
    #aggiungo le statistiche al nome della legenda
    leg = sprintf('%s (r=%.2f%s; %s = %.2f*EC %+.2f)',gsub('GPP_','',cy_col),stat_rcp$estimate[[1]],p_val,gsub('GPP_','',cy_col),lin_regr$coefficients[[2]]
                      ,lin_regr$coefficients[[1]])
    
    fitted_y = c(fitted_y,lin_regr$fitted.values)
    vx = c(vx,tmp$vx)
    vy = c(vy,tmp$vy)
    fitted_des = c(fitted_des,rep(leg,length(tmp$vx)))
    site_des = c(site_des,rep(cys,length(tmp$vx)))
    
    rm(p_val,lin_regr,stat_rcp,tmp,leg)
  }
  rm(cy_col)
  
  df_plot2 = data.frame(vx,vy,fitted_des,fitted_y,site_des)
  
  mp1 = ggplot(df_plot2) +
    geom_point( aes(x = df_plot2$vx, y = df_plot2$vy,color = fitted_des)) +
    geom_line( aes(x = df_plot2$vx, y = df_plot2$fitted_y,color = fitted_des),size = 2) +
    xlab('GPP_DT_CUT_USTAR50 (gC m-2d-1)') +
    ylab('GPP_md (gC m-2d-1)') +
    ggtitle(cys) +
    theme_bw() +
    theme (panel.grid.major = element_blank(), panel.grid.minor = element_blank(),
           legend.title = element_blank(),legend.position = c(0.15,0.87),
           panel.border = element_rect(linetype = 'solid',
                                       fill = NA, color='black',size=0.5))
  ggsave(
    filename = paste0(dir_out1,cys,'_daily_validation_regr.png'),
    plot = mp1,
    scale=1.8,
    width = 30,
    height = 15,
    units = "cm",
    device = 'png',
    dpi = 300
  )
  
  rm(vx,vy,fitted_des,fitted_y,site_des)
  rm(df_plot,df_plot2,mp1)
}
rm(cys)

# creo la lista dei file da utilizzare per le elaborazioni (FT) ----

for (cy_time in c('annual','daily')) {
  
  lf = list.files(dir_in_main,recursive = T,full.names = T,pattern = cy_time)
  if ( length(grep('result',lf)) > 0 ) {
    lf = lf[-1*grep('result',lf)]
  }
  if ( length(grep('.pdf',lf)) > 0 ) {
    lf = lf[-1*grep('.pdf',lf)]
  }
  if ( length(grep('_log_',lf)) > 0 ) {
    lf = lf[-1*grep('_log_',lf)]
  }
  if ( length(grep('LOCAL',lf)) > 0 ) {
    lf = lf[-1*grep('LOCAL',lf)]
  }
  if ( length(grep('Peitz',lf)) > 0 ) {
    lf = lf[-1*grep('Peitz',lf)]
  }
  if ( length(grep('Solling',lf)) > 0 ) {
    lf = lf[-1*grep('Solling',lf)]
  }
  # separo i campi identificativi dei nomi e percorsi dei files
  # i nomi hanno questa struttura:
  meth = c()
  meth_unc = c()
  exp = c()
  sito = c()
  esm = c()
  esm_rcp = c()
  co2 = c()
  co2_rcp = c()
  man = c()
  
  for ( cyf in lf ) {
    tmp = gsub(dir_in_main,'',cyf)
    tmp = unlist(strsplit(tmp,'/'))
    
    meth = c(meth, unlist(strsplit(tmp[3],'-'))[2])# metodo di fotosintesi
    # if ( length(grep('less20',cyf)) > 0) {
    #   meth_unc = c(meth_unc, 'less20')#incertezza 
    # } else if ( length(grep('plus20',cyf)) > 0) {
    #   meth_unc = c(meth_unc, 'plus20')#incertezza 
    # } else {
      meth_unc = c(meth_unc, 'opt')#incertezza 
    # }
    
    exp = c(exp, unlist(strsplit(tmp[3],'-'))[1])
    
    nome_file = basename(cyf)
    
    nome_file = gsub('.txt','',nome_file)
    nome_file = gsub('_d_10000','',nome_file)
    nome_file = unlist(strsplit(nome_file,'_'))
    nome_file = nome_file[-1*seq(1,3)]
    pos_exp = grep('FT',nome_file)
    sito = c(sito, paste(nome_file[1:(pos_exp-1)],collapse = '_'))
    esm = c(esm, nome_file[pos_exp+1])
    esm_rcp = c(esm_rcp, nome_file[pos_exp+2])
    pos_exp = grep('CO2',nome_file)[1]
    co2 = c(co2, nome_file[pos_exp+1])
    pos_exp = grep('CO2',nome_file)[2]
    co2_rcp = c(co2_rcp,nome_file[pos_exp+1])
    man = c(man, nome_file[length(nome_file)])
    
    rm(tmp,pos_exp,nome_file)
  }
  rm(cyf)
  
  df_file = data.frame(lf,meth,meth_unc,exp,sito,esm,esm_rcp,co2,co2_rcp,man)
  rm(lf,meth,meth_unc,exp,sito,esm,esm_rcp,co2,co2_rcp,man)
  
  lista_sito = as.character(sort(unique(df_file$sito)))
  lista_esm = as.character(sort(unique(df_file$esm)))
  lista_esm_rcp = as.character(sort(unique(df_file$esm_rcp)))
  lista_co2 = as.character(sort(unique(df_file$co2)))
  lista_co2_rcp = as.character(sort(unique(df_file$co2_rcp)))
  lista_man = as.character(sort(unique(df_file$man)))
  lista_exp = as.character(sort(unique(df_file$exp)))
  lista_meth = as.character(sort(unique(df_file$meth)))
  lista_meth_unc = as.character(sort(unique(df_file$meth_unc)))
  
  # importo i dati FT ----
  dir_out1 = paste0(dir_out_main,'extract_',cy_time,'/')
  dir.create(dir_out1,showWarnings = F)
  
  extract_file = 1
  
  if (extract_file == 1) {
    file_extr = c()
    for (cyf in seq(1,length(df_file[,1])) ) {
      
      cat(sprintf('import FT: %d / %d %s\n',cyf,length(df_file[,1]),cy_time ))
      
      df = read.csv(as.character(df_file$lf[cyf]),comment.char = '#')
      
      if ( is.factor(df$YEAR) ) {
        pos_na = which(is.na(df$LAYER) == 1)
        if ( length(pos_na) > 0 ) {
          df = df[-1*pos_na,]
          df$YEAR = as.numeric(as.character(df$YEAR))
        }
        rm(pos_na)
      }
      if (cy_time == 'annual') col_to_use = c('YEAR')
      if (cy_time == 'daily')  col_to_use = c('YEAR','MONTH','DAY')
      
      for (cy_v in list_var) {
        pos_v = which(colnames(df) == cy_v)
        if ( length(pos_v) > 0 ) {
          col_to_use = c(col_to_use,cy_v)
        }
        rm(pos_v)
      }
      rm(cy_v)
      df_f = df[,col_to_use]
      rm(col_to_use)
      
      # salvo il file
      write.csv(x = df_f,file = paste0(dir_out1,
                                       paste(as.character(df_file$meth[cyf]),
                                             as.character(df_file$meth_unc[cyf]),
                                             as.character(df_file$exp[cyf]),
                                             as.character(df_file$esm[cyf]),
                                             as.character(df_file$esm_rcp[cyf]),
                                             'CO2',
                                             as.character(df_file$co2[cyf]),
                                             as.character(df_file$co2_rcp[cyf]),
                                             'Man',
                                             as.character(df_file$man[cyf]),
                                             as.character(df_file$sito[cyf]),sep = '_',collapse = '_'
                                       ),'.csv'
      ),row.names = F)
      file_extr = c(file_extr,
                    paste0(dir_out1,
                           paste(as.character(df_file$meth[cyf]),
                                 as.character(df_file$meth_unc[cyf]),
                                 as.character(df_file$exp[cyf]),
                                 as.character(df_file$esm[cyf]),
                                 as.character(df_file$esm_rcp[cyf]),
                                 'CO2',
                                 as.character(df_file$co2[cyf]),
                                 as.character(df_file$co2_rcp[cyf]),
                                 'Man',
                                 as.character(df_file$man[cyf]),
                                 as.character(df_file$sito[cyf]),sep = '_',collapse = '_'
                           ),'.csv'
                    )
      )
      rm(df_f,df)
    }
    rm(cyf)
    
    df_file$file_extr = file_extr
    rm(file_extr)
    write.csv(x = df_file, file = paste0(dir_out_main,'db_file_',cy_time,'.csv'),row.names = F)
  }
  rm(extract_file)
  
  df_file = read.csv(paste0(dir_out_main,'db_file_',cy_time,'.csv'))
  
  lista_sito = as.character(sort(unique(df_file$sito)))
  lista_esm = as.character(sort(unique(df_file$esm)))
  lista_esm_rcp = as.character(sort(unique(df_file$esm_rcp)))
  lista_co2 = as.character(sort(unique(df_file$co2)))
  lista_co2_rcp = as.character(sort(unique(df_file$co2_rcp)))
  lista_man = as.character(sort(unique(df_file$man)))
  lista_exp = as.character(sort(unique(df_file$exp)))
  lista_meth = as.character(sort(unique(df_file$meth)))
  lista_meth_unc = as.character(sort(unique(df_file$meth_unc)))
  
  # calcolo le medie,max e min tra diversi ESM (con e senza parametri)----
  
  calc_stat = 0
  
  if ( calc_stat == 1) {
    for ( cy_meth_unc in lista_meth_unc ) {
      
      cnt_meth_unc = grep(cy_meth_unc,lista_meth_unc)
      
      dir_out1 = paste0(dir_out_main,'mean_',cy_meth_unc,'_',cy_time,'/')
      dir.create(dir_out1,showWarnings = F)
      dir_out2 = paste0(dir_out_main,'max_',cy_meth_unc,'_',cy_time,'/')
      dir.create(dir_out2,showWarnings = F)
      dir_out3 = paste0(dir_out_main,'min_',cy_meth_unc,'_',cy_time,'/')
      dir.create(dir_out3,showWarnings = F)
      
      dir_out4 = paste0(dir_out_main,'valori_per_medie_',cy_meth_unc,'_',cy_time,'/')
      dir.create(dir_out4,showWarnings = F)
      
      for (cy_meth in lista_meth) {
        cnt_meth = grep(cy_meth,lista_meth)
        for (cy_exp in lista_exp) {
          cnt_exp = grep(cy_exp,lista_exp)
          for ( cy_esm_rcp in lista_esm_rcp ) {
            cnt_esm_rcp = grep(cy_esm_rcp,lista_esm_rcp)
            for ( cy_co2 in lista_co2 ) {
              cnt_co2 = grep(cy_co2,lista_co2)
              for ( cy_co2_rcp in lista_co2_rcp ) {
                cnt_co2_rcp = grep(cy_co2_rcp,lista_co2_rcp)
                for ( cy_man in lista_man ) {
                  cnt_man = grep(cy_man,lista_man)
                  for (cy_sito in lista_sito) {
                    
                    cnt_sito = grep(cy_sito,lista_sito)
                    
                    cat(sprintf('calculate_stat:\n'))
                    cat(sprintf('\t meth_unc: %d / %d\n',cnt_meth_unc,length(lista_meth_unc)))
                    cat(sprintf('\t meth: %d / %d\n',cnt_meth,length(lista_meth)))
                    cat(sprintf('\t exp: %d / %d\n',cnt_exp,length(lista_exp)))
                    cat(sprintf('\t esm_rcp: %d / %d\n',cnt_esm_rcp,length(lista_esm_rcp)))
                    cat(sprintf('\t co2: %d / %d\n',cnt_co2,length(lista_co2)))
                    cat(sprintf('\t co2_rcp: %d / %d\n',cnt_co2_rcp,length(lista_co2_rcp)))
                    cat(sprintf('\t man: %d / %d\n',cnt_man,length(lista_man)))
                    cat(sprintf('\t sito: %d / %d\n',cnt_sito,length(lista_sito)))
                    
                    df_file2 = df_file
                    
                    pos_1 = which(df_file2$meth_unc == cy_meth_unc)
                    if ( length(pos_1) == 0 ) next
                    df_file2 = df_file2[pos_1,]
                    
                    pos_1 = which(df_file2$meth == cy_meth)
                    if ( length(pos_1) == 0 ) next
                    df_file2 = df_file2[pos_1,]
                    
                    pos_1 = which(df_file2$exp   == cy_exp)
                    if ( length(pos_1) == 0 ) next
                    df_file2 = df_file2[pos_1,]
                    
                    pos_1 = which(df_file2$esm_rcp   == cy_esm_rcp)
                    if ( length(pos_1) == 0 ) next
                    df_file2 = df_file2[pos_1,]
                    
                    pos_1 = which(df_file2$co2   == cy_co2)
                    if ( length(pos_1) == 0 ) next
                    df_file2 = df_file2[pos_1,]
                    
                    pos_1 = which(df_file2$co2_rcp   == cy_co2_rcp)
                    if ( length(pos_1) == 0 ) next
                    df_file2 = df_file2[pos_1,]
                    
                    pos_1 = which(df_file2$man   == cy_man)
                    if ( length(pos_1) == 0 ) next
                    df_file2 = df_file2[pos_1,]
                    
                    pos_1 = which(df_file2$sito   == cy_sito)
                    if ( length(pos_1) == 0 ) next
                    df_file2 = df_file2[pos_1,]
                    
                    # calcolo le medie
                    if ( exists('df_t') ) {
                      rm(df_t,df_t2)
                    }
                    for (cy_sub in df_file2$file_extr) {
                      df = read.csv(as.character(cy_sub))
                      df2 = df
                      tmp_esm = basename(cy_sub)
                      tmp_esm = gsub(paste(cy_meth,cy_meth_unc,cy_exp,sep = '_'),'',tmp_esm)
                      tmp_esm = unlist(strsplit(tmp_esm,'_'))[2]
                      df2$esm = tmp_esm
                      rm(tmp_esm)
                      df2$meth_unc = cy_meth_unc
                      df2$meth = cy_meth
                      df2$exp = cy_exp
                      df2$esm_rcp = cy_esm_rcp
                      df2$co2 = cy_co2
                      df2$co2_rcp = cy_co2_rcp
                      df2$man = cy_man
                      if ( exists('df_t') ) {
                        df_t = rbind(df_t,df)
                        df_t2 = rbind(df_t2,df2)
                      } else {
                        df_t = df
                        df_t2 = df2
                      }
                      rm(df)
                    }
                    rm(cy_sub)
                    
                    write.csv(df_t2,paste0(dir_out4,
                       cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,'_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',
                       cy_meth,'.csv'),
                        row.names = F,quote = F)
                    
                    # calcolo le statistiche
                    if (cy_time == 'daily') { 
                      unique_year = sort(unique(df_t$YEAR))
                      df_mean = df_t[1,]
                      df_max = df_t[1,]
                      df_min = df_t[1,]
                      for (cy_year in unique_year) {
                        for (cy_month in seq(1,12)) {
                          for (cy_day in seq(1,31) ) {
                            pos_year = which(df_t$YEAR == cy_year &
                                               df_t$MONTH == cy_month &
                                               df_t$DAY == cy_day)
                            if ( length(pos_year) == 0 ) {
                              next
                            }
                            df_mean = rbind(df_mean,
                                            colMeans(df_t[pos_year,],na.rm = T))
                            df_max = rbind(df_max,
                                           colMaxs(as.matrix(df_t[pos_year,]),na.rm = T))
                            df_min = rbind(df_min,
                                           colMins(as.matrix(df_t[pos_year,]),na.rm = T))
                          }
                          rm(cy_day)
                        }
                        rm(cy_month)
                      }
                      rm(cy_year,pos_year)
                      df_mean = df_mean[-1,]
                      df_max = df_max[-1,]
                      df_min = df_min[-1,]
                    }
                    if (cy_time == 'annual') {
                      unique_year = sort(unique(df_t$YEAR))
                      df_mean = df_t[1,]
                      df_max = df_t[1,]
                      df_min = df_t[1,]
                      for (cy_year in unique_year) {
                        pos_year = which(df_t$YEAR == cy_year)
                        df_mean = rbind(df_mean,
                                        colMeans(df_t[pos_year,],na.rm = T))
                        df_max = rbind(df_max,
                                       colMaxs(as.matrix(df_t[pos_year,]),na.rm = T))
                        df_min = rbind(df_min,
                                       colMins(as.matrix(df_t[pos_year,]),na.rm = T))
                      }
                      rm(cy_year,pos_year)
                      df_mean = df_mean[-1,]
                      df_max = df_max[-1,]
                      df_min = df_min[-1,]
                    }
                    # salvo i risultati
                    write.csv(df_mean,paste0(dir_out1,
                                             cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,'_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'),
                              row.names = F,quote = F)
                    write.csv(df_max,paste0(dir_out2,
                                            cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,'_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'),
                              row.names = F,quote = F)
                    write.csv(df_min,paste0(dir_out3,
                                            cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,'_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'),
                              row.names = F,quote = F)
                    
                    rm(unique_year,df_mean,df_max,df_min,df_t)
                  }
                  rm(cy_sito)
                }
                rm(cy_man)
                
              }
              rm(cy_co2_rcp)
            }
            rm(cy_co2)
          }
          rm(cy_esm_rcp)
        }
        rm(cy_exp)
      }
      rm(cy_meth)
    }
    rm(cy_meth_unc)
    rm(cnt_meth_unc,cnt_meth,cnt_exp,cnt_esm_rcp,cnt_co2,cnt_co2_rcp,cnt_man,cnt_sito)
    rm(dir_out1,dir_out2,dir_out3,dir_out4)
  }
  rm(calc_stat)
}
rm(cy_time)

# plot di seasonal per i risultati FT CU,NF,FF ----

dir_out1 = paste0(dir_out_main,'seasonal_FT/')
dir.create(dir_out1,showWarnings = F) 
cy_sito = lista_sito[1]
cy_exp = lista_exp[1]
cy_co2 = lista_co2[1]
cy_man = lista_man[1]
      
for (cy_sito in lista_sito) {
  for ( cy_exp in lista_exp ) {
    for( cy_co2 in lista_co2 ) {
      for ( cy_man in lista_man ) {
        if ( exists('df_t_mean') ) {
          rm(df_t_mean,df_t_max,df_t_min)
        }
        for (cy_co2_rcp in lista_co2_rcp) {
          for (cy_esm_rcp in lista_esm_rcp) {
            for (cy_meth in lista_meth) {
              if (!file.exists(paste0(dir_out_main,'mean_opt_daily/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,
                                      '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv')) ) {
                next
              }
              
            }
            rm(cy_meth)
          }
          rm(cy_esm_rcp)
        }
        rm(cy_co2_rcp)
      }
      rm(cy_man)
    }
    rm(cy_co2)
  }
  rm(cy_exp)
}
rm(cy_sito)

  # importo il file giornaliero mediato per gli ESM
  lf_md = list.files(paste0(dir_out_main,'mean_opt_daily/'),pattern = cys)
  for ( cy_mt in c('FvCB','LUE') ) {
     
    lf_md_mth_opt = lf_md[grep(cy_mt,lf_md)]
    #
    # lf_md_less = lf_md[grep('less',lf_md)]
    # lf_md_mth_less = lf_md_less[grep(cy_mt,lf_md_less)]
    #
    # lf_md_plus = lf_md[grep('plus',lf_md)]
    # lf_md_mth_plus = lf_md_plus[grep(cy_mt,lf_md_plus)]
    #
    if ( exists('df_t') ) rm(df_t)
    for ( cy_qq in lf_md_mth_opt ) {
      df_opt = read.csv(paste0(dir_out_main,'mean_opt_daily/',cy_qq))
      df_opt$file_name = cy_qq
      df_opt$meth = cy_mt
      if ( exists('df_t') ) {
        df_t = rbind(df_t,df_opt)
      } else {
        df_t = df_opt
      }
      rm(df_opt)
    }
    rm(cy_qq,lf_md_mth_opt)

    # df_less = read.csv(paste0(dir_out_main,'extract_LOCAL_daily/',lf_md_mth_less))
    # df_plus = read.csv(paste0(dir_out_main,'extract_LOCAL_daily/',lf_md_mth_plus))

    if (cy_mt == 'FvCB') {
      df_FvCB_opt = df_t
      # df_FvCB_less = df_less
      # df_FvCB_plus = df_plus
    }
    if (cy_mt == 'LUE') {
      df_Fra_opt = df_t
      # df_Fra_less = df_less
      # df_Fra_plus = df_plus
    }
    rm(df_t)
    # if (cy_mt == 'LUE-Ver') {
    #   df_Ver_opt = df_opt
    #   df_Ver_less = df_less
    #   df_Ver_plus = df_plus
    # }
    # rm(df_opt,lf_md_opt,lf_md_mth_opt)
    # rm(df_less,lf_md_less,lf_md_mth_less)
    # rm(df_plus,lf_md_plus,lf_md_mth_plus)
  }
  rm(cy_mt)

  # sincronizzo le serie temporali
  range_year = c(
    min(c(df_FvCB_opt$YEAR, df_Fra_opt$YEAR#, df_Ver_opt$YEAR
          #df_eddy$YEAR,df_FvCB_less$YEAR,df_Ver_less$YEAR,df_Fra_less$YEAR,
          #df_eddy$YEAR,df_FvCB_plus$YEAR,df_Ver_plus$YEAR,df_Fra_plus$YEAR
    ),na.rm = T),
    max(c(df_FvCB_opt$YEAR, df_Fra_opt$YEAR#, df_Ver_opt$YEAR
          #df_eddy$YEAR,df_FvCB_less$YEAR,df_Ver_less$YEAR,df_Fra_less$YEAR,
          #df_eddy$YEAR,df_FvCB_plus$YEAR,df_Ver_plus$YEAR,df_Fra_plus$YEAR
    ),na.rm = T)
  )
  time_serie = seq( ymd(101+(range_year[1]*10000)),ymd(1231+(range_year[2]*10000)),by = 1)
  time_serie = data.frame('YEAR' = year(time_serie), 'MONTH' = month(time_serie), 'DAY' = day(time_serie))

  #sincronizzo i dati del modello
  for ( zz in seq(1,2) ) {
    if ( zz == 1 ) df = df_FvCB_opt
    # if ( zz == 2 ) df = df_Ver_opt
    if ( zz == 2 ) df = df_Fra_opt
    # if ( zz == 4 ) df = df_FvCB_less
    # if ( zz == 5 ) df = df_Ver_less
    # if ( zz == 6 ) df = df_Fra_less
    # if ( zz == 7 ) df = df_FvCB_plus
    # if ( zz == 8 ) df = df_Ver_plus
    # if ( zz == 9 ) df = df_Fra_plus
    gpp_tmp = c()
    gpp_tmp_max = c()
    gpp_tmp_min = c()
    for ( cyt in seq(1,length(time_serie[,1])) ) {
      pos_time = which(df$YEAR == time_serie$YEAR[cyt] &
                         df$MONTH == time_serie$MONTH[cyt] &
                         df$DAY == time_serie$DAY[cyt]
      )
      if ( length(pos_time) == 0 ) {
        gpp_tmp = c(gpp_tmp,NA)
        gpp_tmp_max = c(gpp_tmp_max,NA)
        gpp_tmp_min = c(gpp_tmp_min,NA)
      } else {
        gpp_tmp = c(gpp_tmp,mean(df$gpp[pos_time]))
        gpp_tmp_max = c(gpp_tmp_max,max(df$gpp[pos_time]))
        gpp_tmp_min = c(gpp_tmp_min,min(df$gpp[pos_time]))
      }
      rm(pos_time)
    }
    rm(cyt)
    if ( zz == 1 ) {
      GPP_FvCB_OPT = gpp_tmp
      GPP_FvCB_LESS = gpp_tmp_min
      GPP_FvCB_PLUS = gpp_tmp_max
    }
    # if ( zz == 2 ) GPP_Ver_OPT = gpp_tmp
    if ( zz == 2 ) {
      GPP_Fra_OPT = gpp_tmp
      GPP_Fra_LESS = gpp_tmp_min
      GPP_Fra_PLUS = gpp_tmp_max
    }
    # if ( zz == 4 ) GPP_FvCB_LESS = gpp_tmp
    # if ( zz == 5 ) GPP_Ver_LESS = gpp_tmp
    # if ( zz == 6 ) GPP_Fra_LESS = gpp_tmp
    # if ( zz == 7 ) GPP_FvCB_PLUS = gpp_tmp
    # if ( zz == 8 ) GPP_Ver_PLUS = gpp_tmp
    # if ( zz == 9 ) GPP_Fra_PLUS = gpp_tmp
    rm(gpp_tmp,gpp_tmp_min,gpp_tmp_max)
  }
  rm(zz)

  # creo il df per il plot
  df_plot = data.frame(
    'YEAR' = time_serie$YEAR,
    'MONTH' = time_serie$MONTH,
    'DAY' = time_serie$DAY,
    GPP_FvCB_OPT,GPP_FvCB_LESS,GPP_FvCB_PLUS,
    # GPP_Ver_OPT,GPP_Ver_LESS,GPP_Ver_PLUS,
    GPP_Fra_OPT,GPP_Fra_LESS,GPP_Fra_PLUS
  )

#   # salvo i valori medi, max e min delle medie dei vari ESM
#   write.csv(df_plot, file = paste0(dir_out1,cys,'_mean_max_min_gpp.csv'))
# }
# 
#   rm(GPP_DT_CUT_USTAR50,GPP_DT_CUT_05,GPP_DT_CUT_95)
#   rm(GPP_FvCB_OPT)#,GPP_FvCB_LESS,GPP_FvCB_PLUS)
#   # rm(GPP_Ver_OPT,GPP_Ver_LESS,GPP_Ver_PLUS)
#   rm(GPP_Fra_OPT)#,GPP_Fra_LESS,GPP_Fra_PLUS)
#   rm(df_FvCB_opt)#,df_FvCB_plus,df_FvCB_less)
#   # rm(df_Ver_opt,df_Ver_plus,df_Ver_less)
#   rm(df_Fra_opt)#,df_Fra_plus,df_Fra_less)
#   rm(time_serie)
#   # elimino le righe che hanno valori NA
#   for(cyc in colnames(df_plot)) {
#     pos_na = which(is.na(df_plot[,cyc]) == 1)
#     if ( length(pos_na) > 0 ) {
#       df_plot = df_plot[-1*pos_na,]
#     }
#     rm(pos_na)
#   }
#   rm(cyc)
#   
#   # salvo i dati per le validazioni
#   write.csv(x = df_plot,file = paste0(dir_out1,cys,'_daily_validation.txt'),row.names = F)
#   
#   # calcolo la somma mensile
#   df_plot_m = df_plot[1,]
#   unique_year = sort(unique(df_plot$YEAR))
#   tmp_year = c()
#   tmp_month = c()
#   for (cyy in unique_year) {
#     for (cym in seq(1,12)) {
#       pos_my = which(df_plot$YEAR == cyy & df_plot$MONTH == cym)
#       if (length(pos_my) == 0) next
#       tmp_year = c(tmp_year,cyy)
#       tmp_month = c(tmp_month,cym)
#       df_plot_m = rbind(df_plot_m,colSums(df_plot[pos_my,],na.rm = T))
#     }
#     rm(cym,pos_my)
#   }
#   rm(unique_year)
#   df_plot_m = df_plot_m[-1,]
#   df_plot_m$YEAR = tmp_year
#   df_plot_m$MONTH = tmp_month
#   rm(tmp_year,tmp_month)
#   
#   # calcolo il mese medio
#   df_plot_m2 = df_plot_m[1,]
#   tmp_FvCB_unc_max = c()
#   tmp_FvCB_unc_min = c()
#   tmp_Ver_unc_max = c()
#   tmp_Ver_unc_min = c()
#   tmp_Fra_unc_max = c()
#   tmp_Fra_unc_min = c()
#   tmp_ec_unc_max = c()
#   tmp_ec_unc_min = c()
#   for (cym in seq(1,12)) {
#     pos_my = which(df_plot_m$MONTH == cym)
#     if (length(pos_my) == 0) next
#     # tmp_FvCB_unc_max = c(tmp_FvCB_unc_max,colMaxs(as.matrix(df_plot_m$GPP_FvCB_PLUS[pos_my]),na.rm = T))
#     # tmp_FvCB_unc_min = c(tmp_FvCB_unc_min,colMins(as.matrix(df_plot_m$GPP_FvCB_LESS[pos_my]),na.rm = T))
#     # tmp_Ver_unc_max = c(tmp_Ver_unc_max,colMaxs(as.matrix(df_plot_m$GPP_Ver_PLUS[pos_my]),na.rm = T))
#     # tmp_Ver_unc_min = c(tmp_Ver_unc_min,colMins(as.matrix(df_plot_m$GPP_Ver_LESS[pos_my]),na.rm = T))
#     # tmp_Fra_unc_max = c(tmp_Fra_unc_max,colMaxs(as.matrix(df_plot_m$GPP_Fra_PLUS[pos_my]),na.rm = T))
#     # tmp_Fra_unc_min = c(tmp_Fra_unc_min,colMins(as.matrix(df_plot_m$GPP_Fra_LESS[pos_my]),na.rm = T))
#     tmp_ec_unc_max =  c(tmp_ec_unc_max,colMaxs(as.matrix(df_plot_m$GPP_DT_CUT_95[pos_my]),na.rm = T))
#     tmp_ec_unc_min =  c(tmp_ec_unc_min,colMins(as.matrix(df_plot_m$GPP_DT_CUT_05[pos_my]),na.rm = T))
#     df_plot_m2 = rbind(df_plot_m2,
#                        colMeans(df_plot_m[pos_my,],na.rm = T))
#   }
#   rm(cym,pos_my)
#   df_plot_m2 = df_plot_m2[-1,]
#   
#   # df_plot_m2$GPP_FvCB_PLUS = tmp_FvCB_unc_max
#   # df_plot_m2$GPP_FvCB_LESS = tmp_FvCB_unc_min
#   # df_plot_m2$GPP_Ver_PLUS = tmp_Ver_unc_max
#   # df_plot_m2$GPP_Ver_LESS = tmp_Ver_unc_min
#   # df_plot_m2$GPP_Fra_PLUS = tmp_Fra_unc_max
#   # df_plot_m2$GPP_Fra_LESS = tmp_Fra_unc_min
#   df_plot_m2$GPP_DT_CUT_05 = tmp_ec_unc_min
#   df_plot_m2$GPP_DT_CUT_95 = tmp_ec_unc_max
#   
#   # rm(tmp_FvCB_unc_max,tmp_FvCB_unc_min,tmp_Ver_unc_max,tmp_Ver_unc_min,tmp_Fra_unc_max,tmp_Fra_unc_min)
#   rm(tmp_ec_unc_max,tmp_ec_unc_min)
#   
#   # traccio i plot di validazione
#   mp1 = ggplot(df_plot_m2) +
#     geom_ribbon(aes(x = MONTH, ymax = GPP_DT_CUT_95, ymin = GPP_DT_CUT_05, fill='EC-Uncertainty'),alpha = 0.4) +
#     # geom_ribbon(aes(x = MONTH, ymax = GPP_Ver_PLUS, ymin = GPP_Ver_LESS,fill='Ver-Uncertainty'),alpha = 0.4) +
#     # geom_ribbon(aes(x = MONTH, ymax = GPP_Fra_PLUS, ymin = GPP_Fra_LESS,fill='Fra-Uncertainty'),alpha = 0.4) +
#     # geom_ribbon(aes(x = MONTH, ymax = GPP_FvCB_PLUS, ymin = GPP_FvCB_LESS,fill='FvCB-Uncertainty'),alpha = 0.4) +
#     # geom_line(aes(x = MONTH, y = GPP_Ver_OPT,color = 'LUE-Opt-Ver')) +
#     geom_line(aes(x = MONTH, y = GPP_Fra_OPT,color = 'LUE-Opt-Fra')) +
#     geom_line(aes(x = MONTH, y = GPP_DT_CUT_USTAR50,color = 'EC')) +
#     geom_line(aes(x = MONTH, y = GPP_FvCB_OPT,color = 'FvCB-Optimized')) +
#     scale_color_manual(values = c('FvCB-Optimized' = 'red','LUE-Opt-Ver' = 'blue','LUE-Opt-Fra' = 'green','EC' ='black')) +
#     scale_fill_manual(values =  c('FvCB-Uncertainty' = 'red','Ver-Uncertainty' = 'blue',
#                                   'Fra-Uncertainty' = 'green','EC-Uncertainty' ='black')) +
#     ggtitle(cys) +
#     scale_x_continuous(breaks = seq(1,12),labels = c('J','F','M','A','M','J','J','A','S','O','N','D')) +
#     theme_bw() +
#     theme (panel.grid.major = element_blank(), panel.grid.minor = element_blank(),
#            legend.title = element_blank(),legend.position = c(0.07,0.85),axis.title.x = element_blank(),
#            panel.border = element_rect(linetype = 'solid',
#                                        fill = NA, color='black',size=0.5))+
#     ylab('GPP (gC m-2 month-1)') +
#     guides(color = guide_legend(order=1),
#            fill = guide_legend(order=2))
#   
#   ggsave(
#     filename = paste0(dir_out1,cys,'_validation.png'),
#     plot = mp1,
#     scale=1.8,
#     width = 30,
#     height = 15,
#     units = "cm",
#     device = 'png',
#     dpi = 300
#   )
#   rm(df_plot,df_plot_m,df_plot_m2,mp1)
#   rm(range_year,cyy,des_eddy)
# }
# rm(cys)

# plot di verifica tra valori del modello e la media calcolata (test NO USARE)----
# 
# lf = list.files(paste0(dir_out_main,'valori_per_medie_opt/'),pattern = 'Soroe_FT_CO2_ON_rcp8p5_ESM_rcp8p5_')
# while ( length(lf) > 0 ) {
#   # costruisco il nome togliendo la desinenza del metodo
#   nome_com = unlist(strsplit(basename(lf[1]),'_'))
#   nome_com = nome_com[seq(1,(length(nome_com)-1))]
#   nome_com = paste(nome_com,collapse = '_')
#   des_cmp = grep(nome_com,lf)
#   # importo i file
#   if (exists('df_t')) rm(df_t)
#   for (cy_des_cmp in des_cmp) {
#     df = read.csv(paste0(dir_out_main,'valori_per_medie_opt/',lf[cy_des_cmp]))
#     df$mth = gsub('_','',gsub('.csv','',gsub(nome_com,'',lf[cy_des_cmp])))
#     if (exists('df_t')) {
#       df_t = rbind(df_t,df)
#     } else {
#       df_t = df
#     }
#     rm(df)
#   }
#   rm(cy_des_cmp)
#   
#   # importo il file con la media
#   lf_medie = list.files(paste0(dir_out_main,'mean_opt/'),pattern = nome_com)
#   if (exists('df_t_medie')) rm(df_t_medie)
#   for (cy_des_cmp in lf_medie) {
#     df = read.csv(paste0(dir_out_main,'mean_opt/',cy_des_cmp))
#     df$mth = gsub('_','',gsub('.csv','',gsub(nome_com,'',cy_des_cmp)))
#     if (exists('df_t_medie')) {
#       df_t_medie = rbind(df_t_medie,df)
#     } else {
#       df_t_medie = df
#     }
#     rm(df)
#   }
#   rm(cy_des_cmp)
#   #faccio i plot di confronto
#   df_t$mth = factor(df_t$mth)
#   df_plot = data.frame(
#     'xv' = df_t$YEAR,
#     'yv' = df_t$gpp,
#     'mth' = df_t$mth,
#     'esm' = df_t$esm,
#     'stat' = 'orig'
#   )
#   df_t$mth = factor(df_t$mth)
#   df_plot = rbind(df_plot,
#     data.frame(
#       'xv' = df_t_medie$YEAR,
#       'yv' = df_t_medie$gpp,
#       'mth' = df_t_medie$mth,
#       'esm' = 'all',
#       'stat' = 'mean')
#   )
#   
#   mp1 = ggplot() +
#     geom_point(data = df_plot[which(df_plot$stat == 'orig'),],aes(x = xv, y = yv, color = mth)) +
#     geom_line(data = df_plot[which(df_plot$stat == 'mean'),],aes(x = xv, y = yv, color = mth),size = 2) +
#     theme(legend.title = element_blank(),axis.title.x = element_blank()) +
#     ggtitle(paste(nome_com, 'OPT')) +
#     scale_color_manual(values = c('FvCB' = 'red','LUE-Fra' = 'blue','LUE-Ver' = 'green')) +
#     ylab('gpp') + ylim(710,4300)
#   
#   mp2 = ggplot() +
#     geom_point(data = df_plot[which(df_plot$stat == 'orig' & df_plot$mth == 'FvCB'),],aes(x = xv, y = yv, color = esm)) +
#     geom_line(data = df_plot[which(df_plot$stat == 'mean' & df_plot$mth == 'FvCB'),],aes(x = xv, y = yv, color = esm),size = 2) +
#     theme(legend.title = element_blank(),axis.title.x = element_blank()) +
#     ggtitle(paste(nome_com, 'OPT')) +
#     # scale_color_manual(values = c('FvCB' = 'red','LUE-Fra' = 'blue','LUE-Ver' = 'green')) +
#     ylab('gpp') + ylim(710,4300)
#   
#   mp3 = ggplot() +
#     geom_point(data = df_plot[which(df_plot$stat == 'orig' & df_plot$mth == 'LUE-Fra'),],aes(x = xv, y = yv, color = esm)) +
#     geom_line(data = df_plot[which(df_plot$stat == 'mean' & df_plot$mth == 'LUE-Fra'),],aes(x = xv, y = yv, color = esm),size = 2) +
#     theme(legend.title = element_blank(),axis.title.x = element_blank()) +
#     ggtitle(paste(nome_com, 'OPT')) +
#     # scale_color_manual(values = c('FvCB' = 'red','LUE-Fra' = 'blue','LUE-Ver' = 'green')) +
#     ylab('gpp') + ylim(710,4300)
#   
#   mp4 = ggplot() +
#     geom_point(data = df_plot[which(df_plot$stat == 'orig' & df_plot$mth == 'LUE-Ver'),],aes(x = xv, y = yv, color = esm)) +
#     geom_line(data = df_plot[which(df_plot$stat == 'mean' & df_plot$mth == 'LUE-Ver'),],aes(x = xv, y = yv, color = esm),size = 2) +
#     theme(legend.title = element_blank(),axis.title.x = element_blank()) +
#     ggtitle(paste(nome_com, 'OPT')) +
#     # scale_color_manual(values = c('FvCB' = 'red','LUE-Fra' = 'blue','LUE-Ver' = 'green')) +
#     ylab('gpp') + ylim(710,4300)
#     
#   plot_grid(mp1,mp2,mp3,mp4,nrow = 2)
#   
#   
#   mp1 = ggplot() +
#     geom_point(data = df_plot[which(df_plot$stat == 'orig'),],aes(x = xv, y = yv, color = mth)) +
#     geom_line(data = df_plot[which(df_plot$stat == 'mean'),],aes(x = xv, y = yv, color = mth),size = 2) +
#     theme(legend.title = element_blank(),axis.title.x = element_blank()) +
#     ggtitle(paste(nome_com, 'OPT')) +
#     scale_color_manual(values = c('FvCB' = 'red','LUE-Fra' = 'blue','LUE-Ver' = 'green')) +
#     ylab('gpp') + ylim(710,4300)
#   
#   mp2 = ggplot() +
#     geom_point(data = df_plot[which(df_plot$stat == 'orig' & df_plot$mth == 'FvCB'),],aes(x = xv, y = yv, color = mth)) +
#     geom_line(data = df_plot[which(df_plot$stat == 'mean' & df_plot$mth == 'FvCB'),],aes(x = xv, y = yv, color = mth),size = 2) +
#     theme(legend.title = element_blank(),axis.title.x = element_blank()) +
#     ggtitle(paste(nome_com, 'OPT')) +
#     scale_color_manual(values = c('FvCB' = 'red','LUE-Fra' = 'blue','LUE-Ver' = 'green')) +
#     ylab('gpp') + ylim(710,4300)
#   
#   mp3 = ggplot() +
#     geom_point(data = df_plot[which(df_plot$stat == 'orig' & df_plot$mth == 'LUE-Fra'),],aes(x = xv, y = yv, color = mth)) +
#     geom_line(data = df_plot[which(df_plot$stat == 'mean' & df_plot$mth == 'LUE-Fra'),],aes(x = xv, y = yv, color = mth),size = 2) +
#     theme(legend.title = element_blank(),axis.title.x = element_blank()) +
#     ggtitle(paste(nome_com, 'OPT')) +
#     scale_color_manual(values = c('FvCB' = 'red','LUE-Fra' = 'blue','LUE-Ver' = 'green')) +
#     ylab('gpp') + ylim(710,4300)
#   
#   mp4 = ggplot() +
#     geom_point(data = df_plot[which(df_plot$stat == 'orig' & df_plot$mth == 'LUE-Ver'),],aes(x = xv, y = yv, color = mth)) +
#     geom_line(data = df_plot[which(df_plot$stat == 'mean' & df_plot$mth == 'LUE-Ver'),],aes(x = xv, y = yv, color = mth),size = 2) +
#     theme(legend.title = element_blank(),axis.title.x = element_blank()) +
#     ggtitle(paste(nome_com, 'OPT')) +
#     scale_color_manual(values = c('FvCB' = 'red','LUE-Fra' = 'blue','LUE-Ver' = 'green')) +
#     ylab('gpp') + ylim(710,4300)
#   
#   plot_grid(mp1,mp2,mp3,mp4,nrow = 2)
#   
# }
# rm(lf)
#     
  
# faccio i plot delle medie, max e min (usando i parametri) per il MaxLAI----

dir_out1 = paste0(dir_out_main,'plot_ts_max_min_mean_MaxLAI/')
dir.create(dir_out1,showWarnings = F)

for ( cy_sito in lista_sito ) {
  for ( cy_exp in lista_exp ) {
    for( cy_co2 in lista_co2 ) {
      for ( cy_man in lista_man ) {
        if ( exists('df_t_mean') ) {
          rm(df_t_mean,df_t_max,df_t_min)
        }
        for (cy_co2_rcp in lista_co2_rcp) {
          for (cy_esm_rcp in lista_esm_rcp) {
            for (cy_meth in lista_meth) {
              if (!file.exists(paste0(dir_out_main,'mean_opt_annual/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,
                                      '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv')) ) {
                next
              }
              df_mean = read.csv(paste0(dir_out_main,'mean_opt_annual/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,
                                        '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'))
              df_mean$rcp = paste0('ESM_',cy_esm_rcp,'_CO2_',cy_co2_rcp)
              df_mean$meth = cy_meth
              
              df_max = read.csv(paste0(dir_out_main,'max_opt_annual/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,
                                       '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'))
              df_max$rcp = paste0('ESM_',cy_esm_rcp,'_CO2_',cy_co2_rcp)
              df_max$meth = cy_meth
              
              df_min = read.csv(paste0(dir_out_main,'min_opt_annual/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,
                                       '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'))
              df_min$rcp = paste0('ESM_',cy_esm_rcp,'_CO2_',cy_co2_rcp)
              df_min$meth = cy_meth
              
              if ( exists('df_t_mean') ) {
                df_t_mean = rbind(df_t_mean,df_mean)
                df_t_max = rbind(df_t_max,df_max)
                df_t_min = rbind(df_t_min,df_min)
              } else {
                df_t_mean = df_mean
                df_t_max = df_max
                df_t_min = df_min
              }
              rm(df_mean,df_max,df_min)
            }
            rm(cy_meth)
            
          }
          rm(cy_esm_rcp)
        }
        rm(cy_co2_rcp)
        if ( exists('df_plot') ) {
          rm(df_plot)
        }
        for (cy_var in c('MaxLAI')) {
          if ( exists('df_plot') ) {
            df_plot = rbind(df_plot,data.frame('year'= df_t_mean$YEAR,
                                               'mean' = df_t_mean[,cy_var],
                                               'max' = df_t_max[,cy_var],
                                               'min' = df_t_min[,cy_var],
                                               'rcp' = df_t_mean$rcp,
                                               'meth' = df_t_mean$meth,
                                               'var' = cy_var))
          } else {
            df_plot = data.frame('year'= df_t_mean$YEAR,
                                 'mean' = df_t_mean[,cy_var],
                                 'max' = df_t_max[,cy_var],
                                 'min' = df_t_min[,cy_var],
                                 'rcp' = df_t_mean$rcp,
                                 'meth' = df_t_mean$meth,
                                 'var' = cy_var) 
          }
        }
        rm(cy_var)
        
        df_plot$rcp = gsub('.txt','',df_plot$rcp)
        
        mp1 = ggplot(df_plot) +
          geom_ribbon(aes(x = year, ymax = max, ymin = min,fill = meth),alpha = 0.4) +
          geom_line(aes(x = year, y = mean,color = meth)) +
          scale_fill_manual(values = c('FvCB' = 'red', 'LUE' = 'blue')) +
          scale_color_manual(values =  c('FvCB' = 'red', 'LUE' = 'blue')) +
          ggtitle(paste(cy_sito,cy_exp,'CO2',cy_co2,'Man',cy_man)) +
          theme(axis.title = element_blank(),legend.title = element_blank(),legend.position = c(0.1,0.1),
                legend.direction = 'horizontal',
                legend.background = element_rect(fill = "transparent", colour = "transparent")) +
          facet_grid(var ~ rcp,scale = 'free_y')
        ggsave(
          filename = paste0(dir_out1,cy_sito,'_',cy_exp,'_','CO2','_',cy_co2,'_','Man','_',cy_man,'_MaxLAI_annual.png'),
          plot = mp1,
          scale=1.8,
          width = 30,
          height = 15,
          units = "cm",
          device = 'png',
          dpi = 300
        )
        rm(mp1,df_plot,df_t_mean,df_t_max,df_t_min)
      }
      rm(cy_man)
    }
    rm(cy_co2)
  }
  rm(cy_exp)
}
rm(cy_sito)

  
# faccio i plot delle medie, max e min (usando i parametri)----

dir_out1 = paste0(dir_out_main,'plot_ts_max_min_mean/')
dir.create(dir_out1,showWarnings = F)

for ( cy_sito in lista_sito ) {
  for ( cy_exp in lista_exp ) {
    for( cy_co2 in lista_co2 ) {
      for ( cy_man in lista_man ) {
        if ( exists('df_t_mean') ) {
          rm(df_t_mean,df_t_max,df_t_min)
        }
        for (cy_co2_rcp in lista_co2_rcp) {
          for (cy_esm_rcp in lista_esm_rcp) {
            for (cy_meth in lista_meth) {
              if (!file.exists(paste0(dir_out_main,'mean_opt_annual/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,
                                      '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv')) ) {
                next
              }
              df_mean = read.csv(paste0(dir_out_main,'mean_opt_annual/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,
                                        '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'))
              df_mean$rcp = paste0('ESM_',cy_esm_rcp,'_CO2_',cy_co2_rcp)
              df_mean$meth = cy_meth
              
              df_max = read.csv(paste0(dir_out_main,'max_opt_annual/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,
                                       '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'))
              df_max$rcp = paste0('ESM_',cy_esm_rcp,'_CO2_',cy_co2_rcp)
              df_max$meth = cy_meth
              
              df_min = read.csv(paste0(dir_out_main,'min_opt_annual/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,
                                       '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'))
              df_min$rcp = paste0('ESM_',cy_esm_rcp,'_CO2_',cy_co2_rcp)
              df_min$meth = cy_meth
              
              if ( exists('df_t_mean') ) {
                df_t_mean = rbind(df_t_mean,df_mean)
                df_t_max = rbind(df_t_max,df_max)
                df_t_min = rbind(df_t_min,df_min)
              } else {
                df_t_mean = df_mean
                df_t_max = df_max
                df_t_min = df_min
              }
              rm(df_mean,df_max,df_min)
            }
            rm(cy_meth)
            
          }
          rm(cy_esm_rcp)
        }
        rm(cy_co2_rcp)
        if ( exists('df_plot') ) {
          rm(df_plot)
        }
        for (cy_var in c('gpp','npp')) {
          if ( exists('df_plot') ) {
            df_plot = rbind(df_plot,data.frame('year'= df_t_mean$YEAR,
                                               'mean' = df_t_mean[,cy_var],
                                               'max' = df_t_max[,cy_var],
                                               'min' = df_t_min[,cy_var],
                                               'rcp' = df_t_mean$rcp,
                                               'meth' = df_t_mean$meth,
                                               'var' = cy_var))
          } else {
            df_plot = data.frame('year'= df_t_mean$YEAR,
                                 'mean' = df_t_mean[,cy_var],
                                 'max' = df_t_max[,cy_var],
                                 'min' = df_t_min[,cy_var],
                                 'rcp' = df_t_mean$rcp,
                                 'meth' = df_t_mean$meth,
                                 'var' = cy_var) 
          }
        }
        rm(cy_var)
        
        df_plot$rcp = gsub('.txt','',df_plot$rcp)
        
        mp1 = ggplot(df_plot) +
          geom_ribbon(aes(x = year, ymax = max, ymin = min,fill = meth),alpha = 0.4) +
          geom_line(aes(x = year, y = mean,color = meth)) +
          scale_fill_manual(values = c('FvCB' = 'red', 'LUE' = 'blue')) +
          scale_color_manual(values =  c('FvCB' = 'red', 'LUE' = 'blue')) +
          ggtitle(paste(cy_sito,cy_exp,'CO2',cy_co2,'Man',cy_man)) +
          theme(axis.title = element_blank(),legend.title = element_blank(),legend.position = c(0.1,0.95),
                legend.direction = 'horizontal',
                legend.background = element_rect(fill = "transparent", colour = "transparent")) +
          facet_grid(var ~ rcp,scale = 'free_y')
        ggsave(
          filename = paste0(dir_out1,cy_sito,'_',cy_exp,'_','CO2','_',cy_co2,'_','Man','_',cy_man,'_annual.png'),
          plot = mp1,
          scale=1.8,
          width = 30,
          height = 15,
          units = "cm",
          device = 'png',
          dpi = 300
        )
        rm(mp1,df_plot,df_t_mean,df_t_max,df_t_min)
      }
      rm(cy_man)
    }
    rm(cy_co2)
  }
  rm(cy_exp)
}
rm(cy_sito)

# plot di regressione ----

dir_out1 = paste0(dir_out_main,'plot_FvCB_vs_LUE_regr/')
dir.create(dir_out1,showWarnings = F)

for ( cy_exp in lista_exp ) {
  
  for ( cy_sito in lista_sito ) {
    
    x_lim_min = c()
    x_lim_max = c()
    y_lim_min = c()
    y_lim_max = c()
    x_lim_min_no_unc = c()
    x_lim_max_no_unc = c()
    y_lim_min_no_unc = c()
    y_lim_max_no_unc = c()
    var_lim = c()
    
    for (cy_lim in seq(1,2)) {
    
      # if ( cy_sito == "Peitz" | cy_sito == "Solling_beech" | cy_sito == "Solling_spruce") {
      #   next
      # }
    
      if ( exists('df_t_mean_t') ) {
        rm(df_t_mean_t)
      }
      
      for ( des_st in c('mean','max','min') ){#},'less20','plus20') ) {
        for (cy_meth in lista_meth) {
          for( cy_co2 in lista_co2 ) {
            for ( cy_man in lista_man ) {
              for (cy_co2_rcp in lista_co2_rcp) {
                for (cy_esm_rcp in lista_esm_rcp) {
                  
                  if (!file.exists(paste0(dir_out_main,des_st,'_opt_annual/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',
                                          cy_co2_rcp,
                                          '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv')) ) {
                    next
                  }
                  
                  df_mean = read.csv(paste0(dir_out_main,des_st,'_opt_annual/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,
                                            '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'))
                  df_mean$co2 = cy_co2
                  df_mean$man = cy_man
                  df_mean$rcp = paste0('ESM_',cy_esm_rcp,'_CO2_',cy_co2_rcp)
                  df_mean$rcp_esm = cy_esm_rcp
                  df_mean$rcp_co2 = cy_co2_rcp
                  df_mean$meth = cy_meth
                  df_mean$meth_unc = des_st
                  df_mean$time = NA
                  
                  pos_nf = which(df_mean$YEAR < 2005)
                  if ( length(pos_nf) > 0 ) {
                    df_mean$time[pos_nf] = 'CU 1996-2005'
                  }
                  rm(pos_nf)
                  
                  pos_nf = which(df_mean$YEAR > 2019 & df_mean$YEAR < 2051)
                  if ( length(pos_nf) > 0 ) {
                    df_mean$time[pos_nf] = 'NF 2030-2050'
                  }
                  rm(pos_nf)
                  
                  pos_ff = which(df_mean$YEAR > 2069)
                  if ( length(pos_ff) > 0 ) {
                    df_mean$time[pos_ff] = 'FF 2070-2099'
                  }
                  rm(pos_ff)
                  
                  if ( exists('df_t_mean_t') ) {
                    df_t_mean_t = rbind(df_t_mean_t,df_mean)
                  } else {
                    df_t_mean_t = df_mean
                  }
                  rm(df_mean)#,df_max,df_min)
                }
                rm(cy_esm_rcp)
              }
              rm(cy_co2_rcp)
            }
            rm(cy_man)
          }
          rm(cy_co2)
          # if (cy_meth == "FvCB") {
          #   if ( zz == 1 ) df_t_mean_FvCB = df_t_mean
          #   if ( zz == 2 ) df_t_mean_FvCB_min = df_t_mean
          #   if ( zz == 3 ) df_t_mean_FvCB_max = df_t_mean
          # } 
          # if (cy_meth == "LUE-Fra" ) {
          #   if ( zz == 1 ) df_t_mean_fra = df_t_mean
          #   if ( zz == 2 ) df_t_mean_fra_min = df_t_mean
          #   if ( zz == 3 ) df_t_mean_fra_max = df_t_mean
          # }
          # if (cy_meth == "LUE-Ver" ) {
          #   if ( zz == 1 ) df_t_mean_ver = df_t_mean
          #   if ( zz == 2 ) df_t_mean_ver_min = df_t_mean
          #   if ( zz == 3 ) df_t_mean_ver_max = df_t_mean
          # }
          # rm(df_t_mean)
        }
        rm(cy_meth)
      }
      rm(des_st)
      
      lista_p = list()
      lista_p_no_unc = list()
      cy_var = 'gpp'
      for (cy_var in c('gpp','npp')) {
        if (cy_var == 'MaxLAI') {
          next
        }
        if ( exists('df_plot') ) {
          rm(df_plot)
        }
        
        df_plot = df_t_mean_t[,c(cy_var,'co2','rcp','rcp_esm','rcp_co2','time','meth','meth_unc')]
        # elimino i casi no CLIMATE CHANGE
        df_plot = df_plot[-1*grep("ESM_rcp0p0_CO2_rcp0p0",df_plot$rcp),]
        
        df_plot$facet = NA
        
        pos_1 = which(df_plot$rcp_esm != "rcp0p0" & df_plot$co2 == "ON")
        if (length(pos_1) > 0 ) df_plot$facet[pos_1] = 'CLIMATE ON, CO2 ON'
        rm(pos_1)
        
        pos_1 = which(df_plot$rcp_esm != "rcp0p0" & df_plot$co2 == "VAR")
        if (length(pos_1) > 0 ) df_plot$facet[pos_1] = 'CLIMATE ON, CO2 OFF'
        rm(pos_1)
        
        pos_1 = which(df_plot$rcp_esm == "rcp0p0" & df_plot$co2 == "ON")
        if (length(pos_1) > 0 ) df_plot$facet[pos_1] = 'CLIMATE OFF, CO2 ON'
        rm(pos_1)
        
        pos_1 = which(df_plot$rcp_esm == "rcp0p0" & df_plot$co2 == "VAR")
        if (length(pos_1) > 0 ) df_plot$facet[pos_1] = 'CLIMATE OFF, CO2 OFF'
        rm(pos_1)
        
        # faccio i plot
      
        lista_p1 = list()
        lista_p1_no_unc = list()
        data1_des = c('CLIMATE ON, CO2 OFF','CLIMATE OFF, CO2 ON','CLIMATE ON, CO2 ON')
        
        for ( zzyy in data1_des ) {
          
          data1 = df_plot[grep(zzyy,df_plot$facet),]
          
          if ( length(data1[,1]) == 0 ) next
          
          uni_rcp = unique(data1$rcp)
          uni_mth = unique(data1$meth)
          uni_mth_unc = unique(data1$meth_unc)
          # sistemo i dati OPT per i plot e calcolo r, slope e pval
          if ( exists('data_plot_ok') ) rm(data_plot_ok)
          
          for ( cy_mth in uni_mth) {
            for ( cy_rcp in uni_rcp) {
              if (cy_mth == 'FvCB') next
              cat(sprintf('regression: %s vs %s (rcp = %s)\n','FvCB',cy_mth,cy_rcp))
              # dati su cui calcolo le regressioni
              pos_rrcp = which(data1$rcp == cy_rcp & data1$meth == cy_mth & data1$meth_unc == 'mean')
              pos_rrcp2 = which(data1$rcp == cy_rcp & data1$meth == 'FvCB' & data1$meth_unc == 'mean')
              # dati per massimo
              pos_rrcp_max = which(data1$rcp == cy_rcp & data1$meth == cy_mth & data1$meth_unc == 'max')
              pos_rrcp2_max = which(data1$rcp == cy_rcp & data1$meth == 'FvCB' & data1$meth_unc == 'max')
              # dati per minimo
              pos_rrcp_min = which(data1$rcp == cy_rcp & data1$meth == cy_mth & data1$meth_unc == 'min')
              pos_rrcp2_min = which(data1$rcp == cy_rcp & data1$meth == 'FvCB' & data1$meth_unc == 'min')
              if ( length(pos_rrcp) > 0 ) {
                data_reg = data.frame('ref' = data1[pos_rrcp2,cy_var], 'cmp' = data1[pos_rrcp,cy_var])
                stat_rcp = cor.test(data_reg$ref,data_reg$cmp)
                lin_regr = lm(cmp ~ ref,data = data_reg)
                
                p_val = ''
                if ( stat_rcp$p.value < 0.05 ) p_val = paste0(p_val,'*')
                if ( stat_rcp$p.value < 0.01 ) p_val = paste0(p_val,'*')
                if ( stat_rcp$p.value < 0.001 ) p_val = paste0(p_val,'*')
                #aggiungo le statistiche al nome della legenda
                cy_rcp2 = gsub('rcp','',cy_rcp)
                cy_rcp2 = sprintf('%s (r=%.2f%s; %s = %.2f*FvCB %+.2f)',cy_rcp2,
                                  stat_rcp$estimate[[1]],p_val,cy_mth,lin_regr$coefficients[[2]],
                                  lin_regr$coefficients[[1]])
                data_reg$leg = cy_rcp2
                data_reg$mth = cy_mth
                data_reg$facet = cy_mth
                data_reg$time = data1$time[pos_rrcp]
                data_reg$mth_unc = data1$mth_unc[pos_rrcp]
                # aggiungo le colonne dei valori max e min
                data_reg$cmp_max = data1[pos_rrcp_max,cy_var]
                data_reg$cmp_min = data1[pos_rrcp_min,cy_var]
                data_reg$ref_max = data1[pos_rrcp2_max,cy_var]
                data_reg$ref_min = data1[pos_rrcp2_min,cy_var]
                if ( exists('data_plot_ok') ) {
                  data_plot_ok = rbind(data_plot_ok,data_reg)
                } else {
                  data_plot_ok = data_reg
                }
                rm(stat_rcp,lin_regr,p_val,cy_rcp2,data_reg)
              }
              rm(pos_rrcp,pos_rrcp2,pos_rrcp_max,pos_rrcp2_max,pos_rrcp_min,pos_rrcp2_min)
            }
            rm(cy_mth)
          }
          rm(cy_rcp,uni_rcp,uni_mth)
          
          if ( !exists('data_plot_ok') ) next
          
          data1_rcp2p6 = data_plot_ok[grep('2p6',data_plot_ok$leg),]
          data1_rcp4p5 = data_plot_ok[grep('4p5',data_plot_ok$leg),]
          data1_rcp6p0 = data_plot_ok[grep('6p0',data_plot_ok$leg),]
          data1_rcp8p5 = data_plot_ok[grep('8p5',data_plot_ok$leg),]
          
          data1_ellisse = data_plot_ok
          pos_na = which(is.na(data1_ellisse$time) == 1)
          if (length(pos_na) > 0) data1_ellisse = data1_ellisse[-1*pos_na,]
          rm(pos_na)
          data1_ellisse$time = factor(data1_ellisse$time)
          data1_ellisse$time = factor(data1_ellisse$time, levels = c("CU 1996-2005", "NF 2030-2050", "FF 2070-2099"))
          
          # plot
          mpX = ggplot() +
            geom_errorbar(data  = data1_rcp8p5,aes(x = ref, color = leg,ymin = cmp_min,ymax = cmp_max),alpha = 0.1) +
            geom_errorbarh(data = data1_rcp8p5,aes(x = ref, y = cmp,color = leg,xmin = ref_min, xmax = ref_max),alpha = 0.1) +
            geom_errorbar(data  = data1_rcp6p0,aes(x = ref, color = leg,ymin = cmp_min,ymax = cmp_max),alpha = 0.1) +
            geom_errorbarh(data = data1_rcp6p0,aes(x = ref, y = cmp,color = leg,xmin = ref_min, xmax = ref_max),alpha = 0.1) +
            geom_errorbar(data  = data1_rcp4p5,aes(x = ref, color = leg,ymin = cmp_min,ymax = cmp_max),alpha = 0.1) +
            geom_errorbarh(data = data1_rcp4p5,aes(x = ref, y = cmp,color = leg,xmin = ref_min, xmax = ref_max),alpha = 0.1) +
            geom_errorbar(data  = data1_rcp2p6,aes(x = ref, color = leg,ymin = cmp_min,ymax = cmp_max),alpha = 0.1) +
            geom_errorbarh(data = data1_rcp2p6,aes(x = ref, y = cmp,color = leg,xmin = ref_min, xmax = ref_max),alpha = 0.1) +
            geom_point(data = data1_rcp8p5,aes(x = ref, y = cmp,color = leg)) +
            geom_point(data = data1_rcp6p0,aes(x = ref, y = cmp,color = leg)) +
            geom_point(data = data1_rcp4p5,aes(x = ref, y = cmp,color = leg)) +
            geom_point(data = data1_rcp2p6,aes(x = ref, y = cmp,color = leg)) +
            stat_ellipse(data = data1_ellisse,aes(x = ref, y = cmp, linetype = time),
                         size = 1) +
            ggtitle(sprintf('%s var: %s site: %s',zzyy,cy_var,cy_sito)) +
            xlab(paste0('FvCB ',cy_var,' (gC m-2 yr-1)')) +
            ylab(paste0('LUE ',cy_var,' (gC m-2 yr-1)')) + theme_bw() +
            scale_linetype_manual(values = c("CU 1996-2005" = 'dotted','NF 2030-2050' = 'dashed','FF 2070-2099' = 'solid')) +
            # scale_colour_brewer(palette='Dark2') +
            guides(color = guide_legend(order=2),linetype = guide_legend(order=1))
          
          mpX_no_unc = ggplot() +
            geom_point(data = data1_rcp8p5,aes(x = ref, y = cmp,color = leg)) +
            geom_point(data = data1_rcp6p0,aes(x = ref, y = cmp,color = leg)) +
            geom_point(data = data1_rcp4p5,aes(x = ref, y = cmp,color = leg)) +
            geom_point(data = data1_rcp2p6,aes(x = ref, y = cmp,color = leg)) +
            stat_ellipse(data = data1_ellisse,aes(x = ref, y = cmp, linetype = time),
                         size = 1) +
            ggtitle(sprintf('%s var: %s site: %s',zzyy,cy_var,cy_sito)) +
            xlab(paste0('FvCB ',cy_var,' (gC m-2 yr-1)')) +
            ylab(paste0('LUE ',cy_var,' (gC m-2 yr-1)')) + theme_bw() +
            scale_linetype_manual(values = c("CU 1996-2005" = 'dotted','NF 2030-2050' = 'dashed','FF 2070-2099' = 'solid')) +
            # scale_colour_brewer(palette='Dark2') +
            guides(color = guide_legend(order=2),linetype = guide_legend(order=1))
          
          if ( cy_lim == 1 ) {
            x_lim_min = c(x_lim_min,ggplot_build(mpX)$layout$panel_ranges[[1]]$x.range[1])
            x_lim_max = c(x_lim_max,ggplot_build(mpX)$layout$panel_ranges[[1]]$x.range[2])
            y_lim_min = c(y_lim_min,ggplot_build(mpX)$layout$panel_ranges[[1]]$y.range[1])
            y_lim_max = c(y_lim_max,ggplot_build(mpX)$layout$panel_ranges[[1]]$y.range[2])
            
            x_lim_min_no_unc = c(x_lim_min_no_unc,ggplot_build(mpX_no_unc)$layout$panel_ranges[[1]]$x.range[1])
            x_lim_max_no_unc = c(x_lim_max_no_unc,ggplot_build(mpX_no_unc)$layout$panel_ranges[[1]]$x.range[2])
            y_lim_min_no_unc = c(y_lim_min_no_unc,ggplot_build(mpX_no_unc)$layout$panel_ranges[[1]]$y.range[1])
            y_lim_max_no_unc = c(y_lim_max_no_unc,ggplot_build(mpX_no_unc)$layout$panel_ranges[[1]]$y.range[2])
            
            var_lim = c(var_lim,cy_var)
          } else {
            axis_lim = c(
              min(c(x_lim_min[var_lim == cy_var],y_lim_min[var_lim == cy_var])),
              max(c(x_lim_max[var_lim == cy_var],y_lim_max[var_lim == cy_var]))
            )
            axis_lim_no_unc = c(
              min(c(x_lim_min_no_unc[var_lim == cy_var],y_lim_min_no_unc[var_lim == cy_var])),
              max(c(x_lim_max_no_unc[var_lim == cy_var],y_lim_max_no_unc[var_lim == cy_var]))
            )
            mpX = mpX + xlim(axis_lim) + ylim(axis_lim)
            mpX_no_unc = mpX_no_unc + xlim(axis_lim_no_unc) + ylim(axis_lim_no_unc)
            
            regr11 = data.frame('x11' = axis_lim,'y11' = axis_lim)
            regr11_no_unc = data.frame('x11' = axis_lim_no_unc,'y11' = axis_lim_no_unc)
            mpX = mpX +
              geom_line(data =regr11, aes(x = x11, y = y11),color = 'black') +
              theme (panel.grid.major = element_blank(), panel.grid.minor = element_blank(),
                     legend.title = element_blank(),
                     panel.border = element_rect(linetype = 'solid',
                                 fill = NA, color='black',size=0.5))
            mpX_no_unc = mpX_no_unc +
              geom_line(data =regr11_no_unc, aes(x = x11, y = y11),color = 'black') +
              theme (panel.grid.major = element_blank(), panel.grid.minor = element_blank(),
                     legend.title = element_blank(),
                     panel.border = element_rect(linetype = 'solid',
                                                 fill = NA, color='black',size=0.5))
            
            rm(regr11,axis_lim)
            rm(regr11_no_unc,axis_lim_no_unc)
          }
  
          lista_p1[[length(lista_p1)+1]] = mpX
          lista_p1_no_unc[[length(lista_p1_no_unc)+1]] = mpX_no_unc
          rm(mpX,data1,mpX_no_unc)
          rm(data1_rcp2p6,data1_rcp4p5,data1_rcp6p0,data1_rcp8p5)
          rm(data1_ellisse)
        }
        rm(zzyy,data1_des)
      
        mpt = plot_grid(plotlist = lista_p1,ncol = 1)
        mpt_no_unc = plot_grid(plotlist = lista_p1_no_unc,ncol = 1)
        
        lista_p[[length(lista_p)+1]] = mpt
        lista_p_no_unc[[length(lista_p_no_unc)+1]] = mpt_no_unc
        
        rm(lista_p1,mpt,mpt_no_unc)
      }
      rm(cy_var)
    
      mpt2 = plot_grid(plotlist = lista_p,ncol = 2)
      mpt2_no_unc = plot_grid(plotlist = lista_p_no_unc,ncol = 2)
      ggsave(
        filename = paste0(dir_out1,cy_sito,'_',cy_exp,'_FvCB_vs_LUE_annual.png'),
        plot = mpt2,
        scale=1.8,
        width = 30,
        height = 15,
        units = "cm",
        device = 'png',
        dpi = 300
      )
      ggsave(
        filename = paste0(dir_out1,cy_sito,'_',cy_exp,'_FvCB_vs_LUE_no_unc_annual.png'),
        plot = mpt2_no_unc,
        scale=1.8,
        width = 30,
        height = 15,
        units = "cm",
        device = 'png',
        dpi = 300
      )
      rm(df_plot,mpt2,lista_p,mpt2_no_unc,lista_p_no_unc)
      rm(df_t_mean_FvCB,df_t_mean_FvCB_min,df_t_mean_FvCB_max)
      rm(df_t_mean_lue,df_t_mean_lue_min,df_t_mean_lue_max)
    }
    rm(cy_lim)
  }
  rm(cy_sito)
  rm(x_lim_min,x_lim_max,y_lim_min,y_lim_max,var_lim)
}
rm(cy_exp)


# faccio i plot delle medie, max e min LUE vs FvCB con barre di errore (parametro)----

# faccio l'analisi fattoriale per i dati prodotti dal modello (NF e FF) ----

dir_out1 = paste0(dir_out_main,'factorial/')
dir.create(dir_out1,showWarnings = F)

# calcolo le medie per NF e FF
if ( exists('mean_ff') ) {
  rm(mean_nf, mean_ff)
}
for ( cyf in seq(1,length(df_file[,1])) ) {
  
  cat(sprintf('factorial analisys: %d/%d\n',cyf,length(df_file[,1])))
      
  df = read.csv(as.character(df_file$file_extr[cyf]))
  if ( !exists('mean_ff') ) {
    mean_nf = df[1,]
    mean_ff = df[1,]
  }
  pos_nf1 = which(df$YEAR == 2020)
  pos_nf2 = which(df$YEAR == 2050)
  if ( (length(pos_nf1)*length(pos_nf2)) == 0 ) {
    df_empty = df[1,]
    for (cy_col in colnames(df_empty)) df_empty[,cy_col] = NA
    rm(cy_col)
    mean_nf = rbind(mean_nf,df_empty)
    rm(df_empty)
  } else {
    mean_nf = rbind(mean_nf,
                    colMeans(as.matrix(df[seq(pos_nf1,pos_nf2),]))
      )
  }
  
  pos_ff = which(df$YEAR > 2070)
  if ( length(pos_ff) == 0 ) {
    df_empty = df[1,]
    for (cy_col in colnames(df_empty)) df_empty[,cy_col] = NA
    rm(cy_col)
    mean_ff = rbind(mean_ff,df_empty)
    rm(df_empty)
  } else {
    mean_ff = rbind(mean_ff,
                    colMeans(as.matrix(df[pos_ff,]))
    )
  }
  rm(pos_nf1,pos_nf2,pos_ff)
}
rm(cyf)

if ( exists('mean_ff') ) {
  mean_nf = mean_nf[-1,]
  mean_ff = mean_ff[-1,]
}

for ( cys in lista_sito ) {
  for (cy_var in list_var) {
    for (cytime in c('NF','FF')) {
      
      pos_sito = which(df_file$sito == cys)
      
      df_file2 = df_file[pos_sito,]
      
      if ( cytime == 'NF' ) df_file2$v1 = mean_nf[pos_sito,cy_var]
      if ( cytime == 'FF' ) df_file2$v1 = mean_ff[pos_sito,cy_var]
      
      df_file2 = droplevels(df_file2)
      
      fit <- aov(v1 ~ meth * meth_unc * esm * esm_rcp * co2 * co2_rcp, data=df_file2) # same thing
      
      table_fit = summary(fit)
      
      table_fit[[1]]$`Sum Sq perc`  = (table_fit[[1]]$`Sum Sq` / sum(table_fit[[1]]$`Sum Sq`))*100
      table_fit[[1]]$`Mean Sq perc` = (table_fit[[1]]$`Mean Sq` / sum(table_fit[[1]]$`Mean Sq`))*100
      
      capture.output(table_nf,file=paste0(dir_out1,cys,"_",cy_var,"_anova_",cytime,".txt"))
      rm(fit,df_file2,table_fit)
    }
    rm(cytime)
  }
  rm(cy_var)
}
rm(cys)

# faccio i plot delle medie LUE vs FvCB con testo dell'anno (TODO)----

dir_out1 = paste0(dir_out_main,'figure3/')
dir.create(dir_out1,showWarnings = F)
cy_sito = lista_sito[3]
cy_exp = lista_exp[1]

for ( cy_sito in lista_sito ) {
  if ( cy_sito == "Peitz" | cy_sito == "Solling_beech" | cy_sito == "Solling_spruce") {
    next
  }
  
  for ( cy_exp in lista_exp ) {
    if ( exists('df_t_mean_lue') ) {
      rm(df_t_mean_lue,df_t_mean_FvCB)
    }
    
    for (cy_meth in lista_meth) {
      if ( exists('df_t_mean') ) {
        rm(df_t_mean)#,df_t_max,df_t_min)
      }
      for( cy_co2 in lista_co2 ) {
        for ( cy_man in lista_man ) {
          for (cy_co2_rcp in lista_co2_rcp) {
            for (cy_esm_rcp in lista_esm_rcp) {
              
              if (!file.exists(paste0(dir_out_main,'mean_rcp/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,
                                      '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv')) ) {
                next
              }
              
              df_mean = read.csv(paste0(dir_out_main,'mean_rcp/',cy_sito,'_',cy_exp,'_CO2_',cy_co2,'_',cy_co2_rcp,
                                        '_ESM_',cy_esm_rcp,'_Man_',cy_man,'_',cy_meth,'.csv'))
              df_mean$rcp = paste0('ESM_',cy_esm_rcp,'_CO2_',cy_co2_rcp)
              df_mean$meth = cy_meth
              df_mean$co2 = cy_co2
              
              if ( exists('df_t_mean') ) {
                df_t_mean = rbind(df_t_mean,df_mean)
              } else {
                df_t_mean = df_mean
              }
              rm(df_mean)#,df_max,df_min)
            }
            rm(cy_esm_rcp)
          }
          rm(cy_co2_rcp)
        }
        rm(cy_man)
      }
      rm(cy_co2)
      if (cy_meth == "FvCB") {
        df_t_mean_FvCB = df_t_mean
      } 
      if (cy_meth == "LUE-Veroustraete" ) {
        df_t_mean_lue = df_t_mean
      }
      rm(df_t_mean)
    }
    rm(cy_meth)
    
    lista_p = list()
    cy_var = list_var[1]
    for (cy_var in list_var) {
      if ( exists('df_plot') ) {
        rm(df_plot)
      }
      
      df_plot = df_t_mean_FvCB[,c(cy_var,'rcp','co2','YEAR')]
      df_plot = cbind(df_t_mean_lue[,cy_var],df_plot)
      colnames(df_plot)[1] = 'LUE'
      colnames(df_plot)[2] = 'FvCB'
      
      df_plot = df_plot[-1*grep("ESM_rcp0p0.txt_CO2_rcp0p0.txt",df_plot$rcp),]
      
      df_plot$rcp = gsub('.txt','',df_plot$rcp)
      
      df_plot$facet = 'CLIMATE ON, CO2 ON'
      
      pos_1 = grep("rcp0p0",df_plot$rcp)
      df_plot$facet[pos_1] = 'CLIMATE OFF, CO2 ON'
      
      pos_1 = grep('VAR',df_plot$co2)
      df_plot$facet[pos_1] = 'CLIMATE ON, CO2 OFF'
      
      mp1 = ggplot(data = df_plot[grep('CLIMATE OFF, CO2 ON',df_plot$facet),],aes(x = LUE, y = FvCB,color = rcp)) +
        geom_point() +
        geom_text_repel(aes(label=YEAR), nudge_x = 0.25, nudge_y = 0.25) +
        ggtitle(sprintf('%s var: %s site: %s','CLIMATE OFF, CO2 ON',cy_var,cy_sito)) +
        xlab(paste0('LUE ',cy_var,' (gC m-2 yr-1)')) +
        ylab(paste0('FvCB ',cy_var,' (gC m-2 yr-1)'))
      
      mp2 = ggplot(data = df_plot[grep('CLIMATE ON, CO2 OFF',df_plot$facet),],aes(x = LUE, y = FvCB,color = rcp)) +
        geom_errorbar(aes(ymin = FvCB_min,ymax = FvCB_max)) + 
        geom_errorbarh(aes(xmin = LUE_min, xmax = LUE_max)) +
        geom_point() +
        ggtitle(sprintf('%s var: %s site: %s','CLIMATE ON, CO2 OFF',cy_var,cy_sito)) +
        xlab(paste0('LUE ',cy_var,' (gC m-2 yr-1)')) +
        ylab(paste0('FvCB ',cy_var,' (gC m-2 yr-1)'))
      
      mp3 = ggplot(data = df_plot[grep('CLIMATE ON, CO2 ON',df_plot$facet),],aes(x = LUE, y = FvCB,color = rcp)) +
        geom_errorbar(aes(ymin = FvCB_min,ymax = FvCB_max)) + 
        geom_errorbarh(aes(xmin = LUE_min, xmax = LUE_max)) +
        geom_point() +
        ggtitle(sprintf('%s var: %s site: %s','CLIMATE ON, CO2 ON',cy_var,cy_sito)) +
        xlab(paste0('LUE ',cy_var,' (gC m-2 yr-1)')) +
        ylab(paste0('FvCB ',cy_var,' (gC m-2 yr-1)'))
      
      x_lim1 = c(ggplot_build(mp1)$layout$panel_ranges[[1]]$x.range[1],
                 ggplot_build(mp1)$layout$panel_ranges[[1]]$x.range[2])
      y_lim1 = c(ggplot_build(mp1)$layout$panel_ranges[[1]]$y.range[1],
                 ggplot_build(mp1)$layout$panel_ranges[[1]]$y.range[2])
      x_lim2 = c(ggplot_build(mp2)$layout$panel_ranges[[1]]$x.range[1],
                 ggplot_build(mp2)$layout$panel_ranges[[1]]$x.range[2])
      y_lim2 = c(ggplot_build(mp2)$layout$panel_ranges[[1]]$y.range[1],
                 ggplot_build(mp2)$layout$panel_ranges[[1]]$y.range[2])
      x_lim3 = c(ggplot_build(mp3)$layout$panel_ranges[[1]]$x.range[1],
                 ggplot_build(mp3)$layout$panel_ranges[[1]]$x.range[2])
      y_lim3 = c(ggplot_build(mp3)$layout$panel_ranges[[1]]$y.range[1],
                 ggplot_build(mp3)$layout$panel_ranges[[1]]$y.range[2])
      
      axis_lim = c(
        min(c(x_lim1[1],y_lim1[1],x_lim2[1],y_lim2[1],x_lim3[1],y_lim3[1])),
        max(c(x_lim1[2],y_lim1[2],x_lim2[2],y_lim2[2],x_lim3[2],y_lim3[2]))
      )
      
      mp1 = mp1 + xlim(axis_lim) + ylim(axis_lim)
      mp2 = mp2 + xlim(axis_lim) + ylim(axis_lim)
      mp3 = mp3 + xlim(axis_lim) + ylim(axis_lim)
      
      regr11 = data.frame('x11' = axis_lim,'y11' = axis_lim)
      mp1 = mp1 +
        geom_line(data =regr11, aes(x = x11, y = y11),color = 'black') +
        theme (legend.title = element_blank())
      mp2 = mp2 +
        geom_line(data =regr11, aes(x = x11, y = y11),color = 'black') +
        theme (legend.title = element_blank())
      mp3 = mp3 +
        geom_line(data =regr11, aes(x = x11, y = y11),color = 'black') +
        theme (legend.title = element_blank())
      
      mpt = plot_grid(mp1,mp2,mp3,ncol = 1)
      
      lista_p[[length(lista_p)+1]] = mpt
    }
    
    mpt2 = plot_grid(plotlist = lista_p,ncol = 2)
    
    ggsave(
      filename = paste0(dir_out1,cy_sito,'_',cy_exp,'_FvCB_vs_LUE.png'),
      plot = mpt2,
      scale=1.8,
      width = 30,
      height = 15,
      units = "cm",
      device = 'png',
      dpi = 300
    )
    rm(mpt,mp1,mp2,mp3,df_plot,regr11,mpt2,lista_p)
    rm(x_lim1,y_lim1,x_lim2,y_lim2,x_lim3,y_lim3)
    rm(pos_1,axis_lim)
    
  }
  rm(cy_exp)
}
rm(cy_sito)

