flux_validation <- function(ls_file_md,
         nome_sito,
         file_ec,
         soglia_qc = 0.6,
         var_md = c('nee','reco','gpp'),
         var_eddy = c('NEE_CUT_USTAR50','RECO_DT_CUT_USTAR50','GPP_DT_CUT_USTAR50'),
         var_eddy_qc = c('NEE_CUT_USTAR50_QC','NEE_CUT_USTAR50_QC','NEE_CUT_USTAR50_QC'),
         var_eddy_unc_max = c('NEE_CUT_95','RECO_DT_CUT_95','GPP_DT_CUT_95'),
         var_eddy_unc_min = c('NEE_CUT_05','RECO_DT_CUT_05','GPP_DT_CUT_05')
         ) {
  # validazione dei dati di flusso del modello
  # INPUT
  #   df_mod: dataframe con i dati del modello
  #   nome_sito: nome del sito (usato nel titolo)
  #   df_ec: dataframe dei dati fluxnet
  #   soglia_qc: soglia al di sotto della quale i dati sono scartati
  #   var_eddy: lista delle variabili da fluxnet per la validazione
  #   var_eddy_qc: lista delle variabili QC da fluxnet per la validazione
  #   var_eddy_unc_max: lista delle variabili da fluxnet per l'incertezza MASSIMA
  #   var_eddy_unc_min: lista delle variabili da fluxnet per l'incertezza MINIMA
  # OUTPUT
  #   pdf con i plot di validazione

  # setwd('C:/SviluppoR/')
  library(lubridate)
  library(ggplot2)
  library(cowplot)
  library(matrixStats)
  library(scales)
  # source('read3DCMCC_output.R')
  source('removeInf.R')
  source('colSums2.R')
# ciclo per le variabili da validare ----
  lista_p = list()  
for (cyv in seq(1,length(var_md)) ) {
  if ( exists('df_plot_yy_tot') ) {
    rm(df_plot_tot,df_plot_yy_tot,df_plot_mm_tot,
    df_season_tot,df_season_mm_tot)
  }
  y_lab = var_md[cyv]
  y_lab_ec = var_eddy[cyv]
  
  for (file_md in ls_file_md) {
    
    # importo il file EC ----
    
    cat(sprintf('import file: %s\n',file_ec))
    
    df_ec = read.csv(file_ec)
    
    cat(sprintf('import file: %s OK\n',file_ec))
    
    # sostituisco i -9999 con NA
    for(cy_ck in colnames(df_ec) ) {
      pos_menonove = which(df_ec[,cy_ck] < -9990)
      if ( length(pos_menonove) > 0 ) {
        df_ec[pos_menonove,cy_ck] = NA
      }
      rm(pos_menonove)
    }
    rm(cy_ck)
    
    # per le variabili da plottare applico i qc
    for (cy_ck in seq(1,length(var_eddy)) ) {
      pos_qc = which(df_ec[,var_eddy_qc[cy_ck]] < soglia_qc)
      if ( length(pos_qc) > 0 ) {
        df_ec[pos_qc,var_eddy[cy_ck]] = NA
      }
      rm(pos_qc)
    }
    rm(cy_ck)
    
    # aggiungo la colonna TIME ed YEAR
    df_ec$TIME = ymd(df_ec$TIMESTAMP)
    df_ec$YEAR = year(df_ec$TIME)
    
    # importo il file MD ----
    cat(sprintf('import file: %s\n',file_md))
    df_mod2 = read.csv(file_md,comment.char = '#')
    cat(sprintf('import file: %s OK\n',file_md))
    df_mod2$filename = basename(file_md)
    filename2 = c()
    for ( cy_fn in df_mod2$filename ) {
      filename2 = c(filename2,
                    unlist(strsplit(cy_fn,'_'))[1]
      )
    }
    df_mod2$filename2 = filename2
    des_file = filename2[1]
    rm(filename2,cy_fn)
    
    # sincronizzo le serie temporali ----
    min_year = max(c(min(df_mod2$YEAR),min(df_ec$YEAR)))
    max_year = min(c(max(df_mod2$YEAR),max(df_ec$YEAR)))
    
    pos_year = which(df_mod2$YEAR < min_year)
    if ( length(pos_year) > 0) {
      df_mod2 = df_mod2[-1*pos_year,]
    }
    rm(pos_year)
    #
    pos_year = which(df_mod2$YEAR > max_year)
    if ( length(pos_year) > 0) {
      df_mod2 = df_mod2[-1*pos_year,]
    }
    rm(pos_year)
    #
    pos_year = which(df_ec$YEAR < min_year)
    if ( length(pos_year) > 0) {
      df_ec = df_ec[-1*pos_year,]
    }
    rm(pos_year)
    #
    pos_year = which(df_ec$YEAR > max_year)
    if ( length(pos_year) > 0) {
      df_ec = df_ec[-1*pos_year,]
    }
    rm(pos_year)
    rm(min_year,max_year)
    TIME = df_ec$TIME
    
    # creo i df dei dati
    df_plot = data.frame(
      'TIME' = TIME,
      'YEAR' = year(TIME),
      'MD' = df_mod2[,var_md[cyv]],
      'EC' = df_ec[,var_eddy[cyv]],
      'EC_qc' = df_ec[,var_eddy_qc[cyv]],
      'EC_max' = df_ec[,var_eddy_unc_max[cyv]],
      'EC_min' = df_ec[,var_eddy_unc_min[cyv]]
    )
    
    # applico la soglia dei dati eddy
    TIME2 = df_plot$TIME
    pos_qc = which(df_plot$EC_qc < soglia_qc)
    if ( length(pos_qc) > 0 ) {
      df_plot[pos_qc,] = NA
    }
    df_plot$TIME = TIME2
    rm(TIME2,pos_qc)
      
    # sincronizzo in base ai NA
    TIME2 = df_plot$TIME
    for (cy_ck in seq(1,length(df_plot)) ) {
      pos_na = which(is.na(df_plot[,cy_ck]) == 1)
      if ( length(pos_na) > 0 ) {
        df_plot[pos_na,] = NA
      }
      rm(pos_na)
    }
    df_plot$TIME = TIME2
    rm(cy_ck,TIME2)
    
    y_lab = var_md[cyv]
    
    # calcolo le somme annuali e mensili e i valori max e min (df_plot_yy,df_plot_mm)----
    # per ragioni di calcolo estraggo la colonna TIME e calcolo i valori annuali e mensili
    df_tmp = df_plot
    TIME2 = df_tmp$TIME
  
    df_tmp = df_tmp[,-1*which(colnames(df_tmp) == 'TIME')]
  
    unique_year = sort(unique(year(TIME2)))
    df_plot_yy = df_tmp[1,]
    df_plot_mm = df_tmp[1,]
    month = c()
    year_mon = c()
    for (cy_year in unique_year) {
      for (cy_mon in seq(1,12)) {
        pos_year = which(year(TIME2) == cy_year & month(TIME2) == cy_mon)
        if ( length(pos_year) > 0 ) {
          
          df_plot_mm = rbind(df_plot_mm,colSums2(df_tmp[pos_year,]))
          
          month = c(month,cy_mon)
          year_mon = c(year_mon,cy_year)
        }
        rm(pos_year)
      }
      rm(cy_mon)
      
      pos_year = which(year(TIME2) == cy_year) 
      df_plot_yy = rbind(df_plot_yy,colSums2(df_tmp[pos_year,]))
      
      rm(pos_year)
    }
    rm(cy_year)
    
    df_plot_yy = df_plot_yy[-1,]
    df_plot_mm = df_plot_mm[-1,]
    df_plot_yy$YEAR = unique_year
    df_plot_mm$YEAR = year_mon
    df_plot_mm$MONTH = month
    
    rm(TIME2,df_tmp,month,year_mon,unique_year)
    
    # sincronizzo in base ai NA
    TIME2 = df_plot_yy$YEAR
    for (cy_ck in seq(1,length(df_plot_yy)) ) {
      pos_na = which(is.na(df_plot_yy[,cy_ck]) == 1)
      if ( length(pos_na) > 0 ) {
        df_plot_yy[pos_na,] = NA
      }
      rm(pos_na)
    }
    rm(cy_ck)
    df_plot_yy$YEAR = TIME2
    rm(TIME2)
    
    # sincronizzo in base ai NA
    TIME2 = df_plot_mm$YEAR
    TIME3 = df_plot_mm$MONTH
    for (cy_ck in seq(1,length(df_plot_mm)) ) {
      pos_na = which(is.na(df_plot_mm[,cy_ck]) == 1)
      if ( length(pos_na) > 0 ) {
        df_plot_mm[pos_na,] = NA
      }
      rm(pos_na)
    }
    df_plot_mm$YEAR = TIME2
    df_plot_mm$MONTH = TIME3
    rm(TIME2,TIME3,cy_ck)
    
    # calcolo i valori per il plot seasonal daily (df_season) ----
    df_tmp = df_plot
    df_tmp = df_tmp[,-1*grep('TIME',colnames(df_tmp))]
    df_season = df_tmp[1,]
    doy = c()
    max_ec = c()
    min_ec = c()
    max_md = c()
    min_md = c()
    for (cy_mon in seq(1,12)) {
      for (cy_day in seq(1,31)) {
        pos_ym = which(month(df_plot$TIME) == cy_mon &
                         day(df_plot$TIME) == cy_day)
        if ( length(pos_ym) == 0 ) {
          next
        }
        df_season = rbind(df_season,
                          colMeans(df_tmp[pos_ym,],na.rm = T))
        doy = c(doy,length(doy)+1)
        max_ec = c(max_ec,max(df_tmp$EC[pos_ym],na.rm = T))
        min_ec = c(min_ec,min(df_tmp$EC[pos_ym],na.rm = T))
        max_md = c(max_md,max(df_tmp$MD[pos_ym],na.rm = T))
        min_md = c(min_md,min(df_tmp$MD[pos_ym],na.rm = T))
      }
      rm(cy_day,pos_ym)
    }
    rm(cy_mon)
    df_season = df_season[-1,]
    df_season = data.frame('TIME' = doy,
                           'EC' = df_season$EC,
                           'MD' = df_season$MD,
                           'EC_max' = max_ec,
                           'EC_min' = min_ec,
                           'MD_max' = max_md,
                           'MD_min' = min_md
    )
    rm(df_tmp,doy,min_md,max_md,min_ec,max_ec)
    
    df_season = removeInf(df_season)
    
    # calcolo i valori per il plot seasonal monthly (df_season_mm) ----
    df_tmp = df_plot_mm
    df_season_mm = df_tmp[1,]
    max_ec = c()
    min_ec = c()
    max_md = c()
    min_md = c()
    
    for (cy_mon in seq(1,12)) {
      pos_ym = which(df_tmp$MONTH == cy_mon)
      if ( length(pos_ym) > 0 ) {
        df_season_mm = rbind(df_season_mm,
                             colMeans(df_tmp[pos_ym,],na.rm = T))
        
        max_ec = c(max_ec,max(df_tmp$EC[pos_ym],na.rm = T))
        min_ec = c(min_ec,min(df_tmp$EC[pos_ym],na.rm = T))
        max_md = c(max_md,max(df_tmp$MD[pos_ym],na.rm = T))
        min_md = c(min_md,min(df_tmp$MD[pos_ym],na.rm = T))
      }
    }
    rm(cy_mon,pos_ym)
    df_season_mm = df_season_mm[-1,]
    df_season_mm$EC_max = max_ec
    df_season_mm$EC_min = min_ec
    df_season_mm$MD_max = max_md
    df_season_mm$MD_min = min_md
    rm(df_tmp,min_md,max_md,min_ec,max_ec)
    
    df_season_mm = removeInf(df_season_mm)
    
    # calcolo l'anno medio eddy e calcolo le differenze tra questi valori e tutti gli altri (df_plot_yy,df_plot_bar) ----
    df_plot_yy$anom_ec = df_plot_yy$EC - mean(df_plot_yy$EC,na.rm = T)
    df_plot_yy$anom_md = df_plot_yy$MD - mean(df_plot_yy$MD,na.rm = T)
    
    df_plot_bar = data.frame('TIME' = c(df_plot_yy$YEAR,df_plot_yy$YEAR),
                             'anom' = c(df_plot_yy$anom_ec,df_plot_yy$anom_md),
                             'des_file' = c(rep(y_lab_ec,length(df_plot_yy$anom_ec)),
                                        rep(des_file,length(df_plot_yy$anom_md))),
                             'col_var' = y_lab
    )
    
    df_plot$des_file = des_file[1]
    df_season$des_file = des_file[1]
    df_plot_mm$des_file = des_file[1]
    df_plot_yy$des_file = des_file[1]
    df_season_mm$des_file = des_file[1]
    
    # salvo tutti i risultati ----
    if ( exists('df_plot_yy_tot') ) {
      
      df_plot_tot = rbind(df_plot_tot,df_plot)
      df_season_tot = rbind(df_season_tot,df_season)
      df_plot_bar_tot = rbind(df_plot_bar_tot,df_plot_bar)
      df_plot_mm_tot = rbind(df_plot_mm_tot,df_plot_mm)
      df_plot_yy_tot = rbind(df_plot_yy_tot,df_plot_yy)
      df_season_mm_tot = rbind(df_season_mm_tot,df_season_mm)
  
    } else {
      
      df_plot_tot = df_plot
      df_plot_bar_tot = df_plot_bar
      df_season_tot = df_season
      df_plot_mm_tot = df_plot_mm
      df_plot_yy_tot = df_plot_yy
      df_season_mm_tot = df_season_mm
      
    }
    rm(des_file,df_plot,df_plot_bar,df_season,df_plot_mm,df_plot_yy,df_season_mm)
    }
  rm(file_md)
 
  # plot seasonal (anno medio) ----
  if (exists('df_season_tot1')) {
    rm(df_season_tot1)
  }
  for ( cy_des in unique(df_season_tot$des_file) ) {
    pos11 = which(df_season_tot$des_file == cy_des)
    ddf_tmp2 = df_season_tot[pos11,c('TIME','MD','MD_max','MD_min','des_file')]
    colnames(ddf_tmp2) = c('TIME','v1','v1_max','v1_min','des2')
    if ( exists('df_season_tot1') ) {
      df_season_tot1 = rbind(df_season_tot1,ddf_tmp2)
      rm(ddf_tmp2)
    } else {
      df_season_tot1 = df_season_tot[pos11,c('TIME','EC','EC_max','EC_min','des_file')]
      df_season_tot1$des_file = y_lab_ec
      colnames(df_season_tot1) = c('TIME','v1','v1_max','v1_min','des2')
      df_season_tot1 = rbind(df_season_tot1,ddf_tmp2)
      rm(ddf_tmp2)
    }
    rm(pos11)
  }
  
  anno_medio = ggplot(df_season_tot1) +
    geom_ribbon(aes(x = TIME,ymax = v1_max, ymin = v1_min,fill = des2),alpha = 0.4) +
    geom_line(aes(x = TIME,y = v1,color = des2)) +
    theme(legend.title = element_blank(),axis.title.x = element_blank(),
          legend.position="top") +
    ggtitle(paste(nome_sito,'Daily')) +
    ylab(y_lab)
  rm(df_season_tot1)
  # plot seasonal (mese medio) ----
  
  for ( cy_des in unique(df_season_mm_tot$des_file) ) {
    pos11 = which(df_season_mm_tot$des_file == cy_des)
    ddf_tmp2 = df_season_mm_tot[pos11,c('MONTH','MD','MD_max','MD_min','des_file')]
    colnames(ddf_tmp2) = c('MONTH','v1','v1_max','v1_min','des2')
    if ( exists('df_season_mm_tot1') ) {
      df_season_mm_tot1 = rbind(df_season_mm_tot1,ddf_tmp2)
      rm(ddf_tmp2)
    } else {
      df_season_mm_tot1 = df_season_mm_tot[pos11,c('MONTH','EC','EC_max','EC_min','des_file')]
      df_season_mm_tot1$des_file = y_lab_ec = var_eddy[cyv]
      colnames(df_season_mm_tot1) = c('MONTH','v1','v1_max','v1_min','des2')
      df_season_mm_tot1 = rbind(df_season_mm_tot1,ddf_tmp2)
      rm(ddf_tmp2)
    }
    rm(pos11)
  }
  
  mese_medio = ggplot(df_season_mm_tot1) +
    geom_ribbon(aes(x = MONTH,ymax = v1_max, ymin = v1_min,fill = des2),alpha = 0.4) +
    geom_line(aes(x = MONTH,y = v1,color = des2)) +
    scale_x_continuous(breaks = seq(1,12),labels=c("J","F","M","A","M","J","J","A","S",'O','N','D')) +
    theme(legend.title = element_blank(),axis.title.x = element_blank(),
          legend.position="top") +
    ggtitle(paste(nome_sito,'Monthly')) +
    ylab(y_lab)
  rm(df_season_mm_tot1)
  
  # plot anomalie annuali ----
  
  anom = ggplot(df_plot_bar_tot) +
    geom_bar( aes(x=TIME, y=anom, fill = des_file),stat="identity",
              position=position_dodge()) +
    geom_hline(yintercept = 0) +
    theme(legend.title = element_blank(),axis.title.x = element_blank(),
          legend.position="top") +
    ggtitle(paste(nome_sito,'Annual')) +
    ylab(y_lab)
  rm(df_plot_bar_tot)
  
  # plot dellq serie temporali daily ----
  if (exists('df_plot_tot1')) {
    rm(df_plot_tot1)
  }
  for (cy_des in unique(df_plot_tot$des_file)) {
    pos11 = which(df_plot_tot$des_file == cy_des)
    dd_tmp = df_plot_tot[pos11,c('TIME','MD')]
    dd_tmp$v1_max = NA
    dd_tmp$v1_min = NA
    dd_tmp$des2 = cy_des
    colnames(dd_tmp) = c('TIME','v1','v1_max','v1_min','des2')
    if ( exists('df_plot_tot1') ) {
      df_plot_tot1 = rbind(df_plot_tot1,dd_tmp)
    } else {
      df_plot_tot1 = df_plot_tot[pos11,c('TIME','EC','EC_max','EC_min')]
      df_plot_tot1$des2 = y_lab_ec
      colnames(df_plot_tot1) = c('TIME','v1','v1_max','v1_min','des2')
      df_plot_tot1 = rbind(df_plot_tot1,dd_tmp)
    }
    rm(dd_tmp,pos11)
  }
  
  serie_time = ggplot(df_plot_tot1) +
    geom_ribbon(aes(x = TIME,ymax = v1_max, ymin = v1_min,fill = des2),alpha = 0.4) +
    geom_point(aes(x = TIME,y = v1,color = des2)) +
    theme(legend.title = element_blank(),axis.title.x = element_blank(),
          legend.position="none") +
    # ggtitle(paste(as.character(lista_siti$nome_fluxnet[cy_s]),'Daily')) +
    ylab(y_lab)

  # plot della serie temporale MONTHLY ----
  if (exists('df_plot_mm_tot1')) {
    rm(df_plot_mm_tot1)
  }
  df_plot_mm_tot$TIME = ymd((df_plot_mm_tot$YEAR*10000) + (df_plot_mm_tot$MONTH*100) +1)
  
  for (cy_des in unique(df_plot_mm_tot$des_file)) {
    pos11 = which(df_plot_mm_tot$des_file == cy_des)
    dd_tmp = df_plot_mm_tot[pos11,c('TIME','MD')]
    dd_tmp$v1_max = NA
    dd_tmp$v1_min = NA
    dd_tmp$des2 = cy_des
    colnames(dd_tmp) = c('TIME','v1','v1_max','v1_min','des2')
    if ( exists('df_plot_mm_tot1') ) {
      df_plot_mm_tot1 = rbind(df_plot_mm_tot1,dd_tmp)
    } else {
      df_plot_mm_tot1 = df_plot_mm_tot[pos11,c('TIME','EC','EC_max','EC_min')]
      df_plot_mm_tot1$des2 = y_lab_ec
      colnames(df_plot_mm_tot1) = c('TIME','v1','v1_max','v1_min','des2')
      df_plot_mm_tot1 = rbind(df_plot_mm_tot1,dd_tmp)
    }
    rm(dd_tmp,pos11)
  }
  
  serie_time_mm = ggplot(df_plot_mm_tot1) +
    geom_ribbon(aes(x = TIME,ymax = v1_max, ymin = v1_min,fill = des2),alpha = 0.4) +
    geom_point(aes(x = TIME,y = v1,color = des2)) +
    theme(legend.title = element_blank(),axis.title.x = element_blank(),
          legend.position="none") +
    scale_x_date(labels = date_format("%m-%Y")) +
    # ggtitle(paste(as.character(lista_siti$nome_fluxnet[cy_s]),'Annual')) +
    ylab(y_lab)
  
  # plot della serie temporale ANNUAL ----
  if (exists('df_plot_yy_tot1')) {
    rm(df_plot_yy_tot1)
  }
  for (cy_des in unique(df_plot_yy_tot$des_file)) {
    pos11 = which(df_plot_yy_tot$des_file == cy_des)
    dd_tmp = df_plot_yy_tot[pos11,c('YEAR','MD')]
    dd_tmp$v1_max = NA
    dd_tmp$v1_min = NA
    dd_tmp$des2 = cy_des
    colnames(dd_tmp) = c('YEAR','v1','v1_max','v1_min','des2')
    if ( exists('df_plot_yy_tot1') ) {
      df_plot_yy_tot1 = rbind(df_plot_yy_tot1,dd_tmp)
    } else {
      df_plot_yy_tot1 = df_plot_yy_tot[pos11,c('YEAR','EC','EC_max','EC_min')]
      df_plot_yy_tot1$des2 = y_lab_ec
      colnames(df_plot_yy_tot1) = c('YEAR','v1','v1_max','v1_min','des2')
      df_plot_yy_tot1 = rbind(df_plot_yy_tot1,dd_tmp)
    }
    rm(dd_tmp,pos11)
  }
  serie_time_yy = ggplot(df_plot_yy_tot1) +
    geom_ribbon(aes(x = YEAR,ymax = v1_max, ymin = v1_min,fill = des2),alpha = 0.4) +
    geom_point(aes(x = YEAR,y = v1,color = des2)) +
    theme(legend.title = element_blank(),axis.title.x = element_blank(),
          legend.position="none") +
    # ggtitle(paste(as.character(lista_siti$nome_fluxnet[cy_s]),'Annual')) +
    ylab(y_lab)
    
  # plot di validazione regressioni DAILY ----
  if ( exists('df_plot_tot1_lm') ) {
    rm(df_plot_tot1_lm)
  }
  for (cy_des in unique(df_plot_tot1$des2)) {
    if ( cy_des == y_lab_ec ) {
      next
    }
    ec = df_plot_tot1$v1[which(df_plot_tot1$des2 == y_lab_ec)]
    md= df_plot_tot1$v1[which(df_plot_tot1$des2 == cy_des)]
    if ( exists('df_plot_tot1_lm') ) {
      df_plot_tot1_lm = rbind(df_plot_tot1_lm, data.frame('EC' = ec, 'MD' = md,'des2' = cy_des))
    } else {
      df_plot_tot1_lm = data.frame('EC' = ec, 'MD' = md,'des2' = cy_des)
    }
  }
  rm(ec,md,cy_des)
  
  colnames(df_plot_tot1_lm)[colnames(df_plot_tot1_lm) == 'EC'] = y_lab_ec
  lista_p_lr_dd = plot_multi_lm(df_plot_tot1_lm,yvar_name = paste(y_lab,'Simulated'),xvar_name = paste(y_lab_ec,'Measured'))
  
  # plot di validazione regressioni MONTHLY ----
  if (exists('df_plot_mm_tot1_lm')) {
    rm(df_plot_mm_tot1_lm)
  }
  for (cy_des in unique(df_plot_mm_tot1$des2)) {
    if ( cy_des == y_lab_ec ) {
      next
    }
    ec = df_plot_mm_tot1$v1[which(df_plot_mm_tot1$des2 == y_lab_ec)]
    md= df_plot_mm_tot1$v1[which(df_plot_mm_tot1$des2 == cy_des)]
    if ( exists('df_plot_mm_tot1_lm') ) {
      df_plot_mm_tot1_lm = rbind(df_plot_mm_tot1_lm,data.frame('EC' = ec, 'MD' = md,'des2' = cy_des))
    } else {
      df_plot_mm_tot1_lm = data.frame('EC' = ec, 'MD' = md,'des2' = cy_des)
    }
  }
  rm(ec,md,cy_des)
  colnames(df_plot_mm_tot1_lm)[colnames(df_plot_mm_tot1_lm) == 'EC'] = y_lab_ec
  lista_p_lr_mm = plot_multi_lm(df_plot_mm_tot1_lm,yvar_name = paste(y_lab,'Simulated'),xvar_name = paste(y_lab_ec,'Measured'))
  
  # plot di validazione regressioni ANNUAL ----
  if ( exists('df_plot_yy_tot1_lm') ) {
    rm(df_plot_yy_tot1_lm)
  }
  for (cy_des in unique(df_plot_yy_tot1$des2)) {
    if ( cy_des == y_lab_ec ) {
      next
    }
    ec = df_plot_yy_tot1$v1[which(df_plot_yy_tot1$des2 == y_lab_ec)]
    md= df_plot_yy_tot1$v1[which(df_plot_yy_tot1$des2 == cy_des)]
    if ( exists('df_plot_yy_tot1_lm') ) {
      df_plot_yy_tot1_lm = rbind(df_plot_yy_tot1_lm,data.frame('EC' = ec, 'MD' = md,'des2' = cy_des))
    } else {
      df_plot_yy_tot1_lm = data.frame('EC' = ec, 'MD' = md,'des2' = cy_des)
    }
  }
  rm(ec,md,cy_des)
  
  colnames(df_plot_yy_tot1_lm)[colnames(df_plot_yy_tot1_lm) == 'EC'] = y_lab_ec
  
  lista_p_lr_yy = plot_multi_lm(df_plot_yy_tot1_lm,yvar_name = paste(y_lab,'Simulated'),xvar_name = paste(y_lab_ec,'Measured'))
  
  # plot dei risultati ----
  cat(sprintf('create plot of flux validation, %s\n',y_lab_ec))
  mpt = plot_grid(anno_medio,mese_medio,anom,serie_time,serie_time_mm,serie_time_yy)
  mpt_ln = plot_grid(lista_p_lr_dd[[1]],lista_p_lr_dd[[2]],
                     lista_p_lr_mm[[1]],lista_p_lr_mm[[2]],
                     lista_p_lr_yy[[1]],lista_p_lr_yy[[2]],
                     ncol = 2)
  lista_p[[length(lista_p)+1]] = mpt
  lista_p[[length(lista_p)+1]] = mpt_ln
  cat(sprintf('create plot of flux validation, %s ...OK\n',y_lab_ec))
  rm(anno_medio,mese_medio,anom,serie_time,serie_time_mm,serie_time_yy)
  rm(lista_p_lr_dd,lista_p_lr_mm,lista_p_lr_yy)
  
}
rm(cyv)
return(lista_p)  
  }#end function
#   #     df_plot_mm$LM = model_lm_mm$fitted.values
#   #     text_df = data.frame('pos_x' = min(df_plot_mm$EC),'pos_y' = max(df_plot_mm$LM)*0.99,
#   #                          'testo' = 
#   #                            sprintf("MD = %.2f EC %+.2f ; r2 = %.2f",
#   #                                    coef(model_lm_mm)[2],
#   #                                    coef(model_lm_mm)[1],summary.lm(model_lm_mm)$r.squared)
#   #     )
#   #     p1_mm = 
#   #       ggplot() +
#   #       geom_point(data = df_plot_mm,aes(x = EC,y = MD),size=0.5) +
#   #       ggtitle(sprintf('MD: mean = %.2f gC m-2 month-1 (sd: %.2f) (n = %.0f%%)\nEC: mean = %.2f gC m-2 d-1 (sd: %.2f) (n = %.0f%%)',
#   #                       mean(df_plot_mm$MD),sd(df_plot_mm$MD),100*(length(df_plot_mm$MD)/nr_dati_mm),
#   #                       mean(df_plot_mm$EC),sd(df_plot_mm$EC),100*(length(df_plot_mm$EC)/nr_dati_mm)),
#   #               subtitle = sprintf('RMSE: %.2f; %s',sqrt(mean((df_plot_mm$EC - df_plot_mm$MD)^2)),text_df$testo)
#   #       )+
#   #       geom_line(data = df_plot_mm,aes(x = EC,y = LM),color='red',size = 1) +
#   #       # geom_text(data = text_df,aes(x = pos_x, y = pos_y, label = testo),hjust=-0.01) +
#   #       xlab('Measured') +
#   #       ylab('Simulated') +
#   #       theme(plot.title = element_text(size = 10,hjust = 0.5),
#   #             plot.subtitle = element_text(size=9,hjust=0.5),
#   #             axis.title = element_text(size = 9),
#   #             axis.text = element_text(size=9),
#   #             legend.title = element_blank(),legend.position="none",
#   #             legend.direction="horizontal")
#   #     
#   #     # regressione lineare annual
#   #     model_lm_yy <- lm(MD ~ EC, data = df_plot_yy)
#   #     
#   #     df_plot_yy$LM = model_lm_yy$fitted.values
#   #     
#   #     text_df = data.frame('pos_x' = min(df_plot_yy$EC),'pos_y' = max(df_plot_yy$LM)*1.04,
#   #                          'testo' = 
#   #                            sprintf("MD = %.2f EC %+.2f ; r2 = %.2f",
#   #                                    coef(model_lm_yy)[2],
#   #                                    coef(model_lm_yy)[1],summary.lm(model_lm_yy)$r.squared)
#   #     )
#   #     p1_yy = ggplot(df_plot_yy) +
#   #       geom_point(aes(x = EC,y = MD),size=0.5) +
#   #       ggtitle(sprintf('MD: mean = %.2f gC m-2 year-1 (sd: %.2f) (n = %.0f%%)\nEC: mean = %.2f gC m-2 d-1 (sd: %.2f) (n = %.0f%%)',
#   #                       mean(df_plot_yy$MD),sd(df_plot_yy$MD),100*(length(df_plot_yy$MD)/nr_dati_yy),
#   #                       mean(df_plot_yy$EC),sd(df_plot_yy$EC),100*(length(df_plot_yy$EC)/nr_dati_yy)
#   #       ),subtitle = sprintf('RMSE: %.2f; %s',
#   #                            sqrt(mean((df_plot_yy$EC - df_plot_yy$MD)^2)),text_df$testo))+
#   #       geom_line(aes(x = EC,y = LM),color='red',size = 1) +
#   #       # geom_text(data = text_df,aes(x = pos_x, y = pos_y, label = testo),hjust=-0.01) +
#   #       xlab('Measured') +
#   #       ylab('Simulated') +
#   #       theme(plot.title = element_text(size = 10,hjust = 0.5),
#   #             plot.subtitle = element_text(size=9,hjust=0.5),
#   #             axis.title = element_text(size = 9),
#   #             axis.text = element_text(size=9),
#   #             legend.title = element_blank(),legend.position="none",
#   #             legend.direction="horizontal")
#   #     
#   #     mp_t = plot_grid(plotlist = list(anno_medio,mese_medio,anom,
#   #                                      serie_time,serie_time_mm,serie_time_yy,
#   #                                      p1,p1_mm,p1_yy),
#   #                      ncol = 3,align = 'v',labels = 'AUTO')
#   #     
#   #     lista_plot[[length(lista_plot)+1]] = mp_t
#   #     
#   #     rm(df_plot_yy,df_plot,y_lab,p1,serie_time,p1_yy,serie_time_yy,mp_t)
#   #     rm(model_lm,model_lm_yy,text_df,df_plot_bar)
#   #   }
#   #   rm(cyv,TIME,df_ec,df_mod2)
#   #   
#   # # for (file_md in ls_file_md) {
#   # #   cat(sprintf('import file: %s\n',file_md))
#   # #   df_mod2 = read.csv(file_md,comment.char = '#')
#   # #   cat(sprintf('import file: %s OK\n',file_md))
#   # #   df_mod2$filename = basename(file_md)
#   # #   filename2 = c()
#   # #   for ( cy_fn in df_mod2$filename ) {
#   # #     filename2 = c(filename2,
#   # #                   unlist(strsplit(cy_fn,'_'))[1]
#   # #     )
#   # #   }
#   # #   df_mod2$filename2 = filename2
#   # #   rm(filename2,cy_fn)
#   #   
#   #   if ( exists('df_mod_t') ) {
#   #     df_mod_t = rbind(df_mod_t,df_mod2)
#   #     df_ec = rbind(df_ec,df_ec1)
#   #   } else {
#   #     df_mod_t = df_mod2
#   #     df_ec = df_ec1
#   #   }
#   #   rm(df_mod2)
#   # }
#   # rm(file_md)
#   # 
#   # df_mod2 = df_mod_t
#   # 
#   # # validazione 
#   # lista_plot = list()
#   # 
#   # for (cyv in seq(1,length(var_md)) ) {
#   #   
#   #   # creo i df dei dati
#   #   df_plot = data.frame(
#   #     'TIME' = TIME,
#   #     'YEAR' = year(TIME),
#   #     'MD' = df_mod2[,var_md[cyv]],
#   #     'EC' = df_ec[,var_eddy[cyv]],
#   #     'EC_qc' = df_ec[,var_eddy_qc[cyv]],
#   #     'EC_max' = df_ec[,var_eddy_unc_max[cyv]],
#   #     'EC_min' = df_ec[,var_eddy_unc_min[cyv]],
#   #     'col_var' = df_mod2$filename2
#   #   )
#   #   
#   #   y_lab = var_md[cyv]
#   #   
#   #   # applico la soglia dei dati eddy
#   #   TIME2 = df_plot$TIME
#   #   pos_qc = which(df_plot$EC_qc < soglia_qc)
#   #   if ( length(pos_qc) > 0 ) {
#   #     df_plot[pos_qc,] = NA
#   #   }
#   #   df_plot$TIME = TIME2
#   #   rm(TIME2,pos_qc)
#   #   
#   #   # sincronizzo in base ai NA
#   #   TIME2 = df_plot$TIME
#   #   for (cy_ck in seq(1,length(df_plot)) ) {
#   #     pos_na = which(is.na(df_plot[,cy_ck]) == 1)
#   #     if ( length(pos_na) > 0 ) {
#   #       df_plot[pos_na,] = NA
#   #     }
#   #     rm(pos_na)
#   #   }
#   #   df_plot$TIME = TIME2
#   #   rm(cy_ck,TIME2)
#   #   
#   #   # per ragioni di calcolo estraggo la colonna TIME e calcolo i valori annuali e mensili
#   #   TIME2 = df_plot$TIME
#   #   col_var = df_plot$col_var
#   #   df_tmp = df_plot
#   #   df_tmp = df_tmp[,-1*which(colnames(df_tmp) == 'TIME')]
#   #   df_tmp = df_tmp[,-1*which(colnames(df_tmp) == 'col_var')]
#   #   # calcolo le somme annuali e i valori max e min
#   #   unique_year = sort(unique(year(TIME2)))
#   #   df_plot_yy = df_tmp[1,]
#   #   df_plot_mm = df_tmp[1,]
#   #   month = c()
#   #   year_mon = c()
#   #   for (cy_year in unique_year) {
#   #     for (cy_mon in seq(1,12)) {
#   #       pos_year = which(year(TIME2) == cy_year & month(TIME2) == cy_mon)
#   #       if ( length(pos_year) > 0 ) {
#   #         
#   #         df_plot_mm = rbind(df_plot_mm,colSums2(df_tmp[pos_year,]))
#   #         
#   #         month = c(month,cy_mon)
#   #         year_mon = c(year_mon,cy_year)
#   #       }
#   #       rm(pos_year)
#   #     }
#   #     rm(cy_mon)
#   #     
#   #     pos_year = which(year(TIME2) == cy_year) 
#   #     df_plot_yy = rbind(df_plot_yy,colSums2(df_tmp[pos_year,]))
#   #     
#   #     rm(pos_year)
#   #   }
#   #   rm(cy_year)
#   #   
#   #   df_plot_yy = df_plot_yy[-1,]
#   #   df_plot_mm = df_plot_mm[-1,]
#   #   df_plot_yy$YEAR = unique_year
#   #   df_plot_mm$YEAR = year_mon
#   #   df_plot_mm$MONTH = month
#   #   rm(TIME2,df_tmp,month,year_mon,unique_year)
#   #   
#   #   # sincronizzo in base ai NA
#   #   TIME2 = df_plot_yy$YEAR
#   #   for (cy_ck in seq(1,length(df_plot_yy)) ) {
#   #     pos_na = which(is.na(df_plot_yy[,cy_ck]) == 1)
#   #     if ( length(pos_na) > 0 ) {
#   #       df_plot_yy[pos_na,] = NA
#   #     }
#   #     rm(pos_na)
#   #   }
#   #   rm(cy_ck)
#   #   df_plot_yy$YEAR = TIME2
#   #   rm(TIME2)
#   #   
#   #   # sincronizzo in base ai NA
#   #   TIME2 = df_plot_mm$YEAR
#   #   TIME3 = df_plot_mm$MONTH
#   #   for (cy_ck in seq(1,length(df_plot_mm)) ) {
#   #     pos_na = which(is.na(df_plot_mm[,cy_ck]) == 1)
#   #     if ( length(pos_na) > 0 ) {
#   #       df_plot_mm[pos_na,] = NA
#   #     }
#   #     rm(pos_na)
#   #   }
#   #   df_plot_mm$YEAR = TIME2
#   #   df_plot_mm$MONTH = TIME3
#   #   rm(TIME2,TIME3,cy_ck)
#   #   
#   #   # calcolo i valori per il plot seasonal daily
#   #   df_tmp = df_plot
#   #   df_tmp = df_tmp[,-1*grep('TIME',colnames(df_tmp))]
#   #   df_tmp = df_tmp[,-1*grep('col_var',colnames(df_tmp))]
#   #   df_season = df_tmp[1,]
#   #   doy = c()
#   #   max_ec = c()
#   #   min_ec = c()
#   #   max_md = c()
#   #   min_md = c()
#   #   for (cy_mon in seq(1,12)) {
#   #     for (cy_day in seq(1,31)) {
#   #       pos_ym = which(month(df_plot$TIME) == cy_mon &
#   #                        day(df_plot$TIME) == cy_day)
#   #       if ( length(pos_ym) == 0 ) {
#   #         next
#   #       }
#   #       df_season = rbind(df_season,
#   #                         colMeans(df_tmp[pos_ym,],na.rm = T))
#   #       doy = c(doy,length(doy)+1)
#   #       max_ec = c(max_ec,max(df_tmp$EC[pos_ym],na.rm = T))
#   #       min_ec = c(min_ec,min(df_tmp$EC[pos_ym],na.rm = T))
#   #       max_md = c(max_md,max(df_tmp$MD[pos_ym],na.rm = T))
#   #       min_md = c(min_md,min(df_tmp$MD[pos_ym],na.rm = T))
#   #     }
#   #     rm(cy_day,pos_ym)
#   #   }
#   #   rm(cy_mon)
#   #   df_season = df_season[-1,]
#   #   df_season = data.frame('TIME' = doy,
#   #                          'EC' = df_season$EC,
#   #                          'MD' = df_season$MD,
#   #                          'EC_max' = max_ec,
#   #                          'EC_min' = min_ec,
#   #                          'MD_max' = max_md,
#   #                          'MD_min' = min_md
#   #   )
#   #   rm(df_tmp,doy,min_md,max_md,min_ec,max_ec)
#   #   
#   #   df_season = removeInf(df_season)
#   #   
#   #   # calcolo i valori per il plot seasonal monthly
#   #   df_tmp = df_plot_mm
#   #   df_season_mm = df_tmp[1,]
#   #   max_ec = c()
#   #   min_ec = c()
#   #   max_md = c()
#   #   min_md = c()
#   #   for (cy_mon in seq(1,12)) {
#   #     pos_ym = which(df_tmp$MONTH == cy_mon)
#   #     if ( length(pos_ym) > 0 ) {
#   #       df_season_mm = rbind(df_season_mm,
#   #                            colMeans(df_tmp[pos_ym,],na.rm = T))
#   #       
#   #       max_ec = c(max_ec,max(df_tmp$EC[pos_ym],na.rm = T))
#   #       min_ec = c(min_ec,min(df_tmp$EC[pos_ym],na.rm = T))
#   #       max_md = c(max_md,max(df_tmp$MD[pos_ym],na.rm = T))
#   #       min_md = c(min_md,min(df_tmp$MD[pos_ym],na.rm = T))
#   #     }
#   #   }
#   #   rm(cy_mon,pos_ym)
#   #   df_season_mm = df_season_mm[-1,]
#   #   df_season_mm$EC_max = max_ec
#   #   df_season_mm$EC_min = min_ec
#   #   df_season_mm$MD_max = max_md
#   #   df_season_mm$MD_min = min_md
#   #   rm(df_tmp,min_md,max_md,min_ec,max_ec)
#   #   
#   #   df_season_mm = removeInf(df_season_mm)
#   #   
#   #   # calcolo l'anno medio eddy e calcolo le differenze tra questi valori e tutti gli altri
#   #   df_plot_yy$anom_ec = df_plot_yy$EC - mean(df_plot_yy$EC,na.rm = T)
#   #   df_plot_yy$anom_md = df_plot_yy$MD - mean(df_plot_yy$MD,na.rm = T)
#   #   df_plot_bar = data.frame('TIME' = c(df_plot_yy$YEAR,df_plot_yy$YEAR),
#   #                            'anom' = c(df_plot_yy$anom_ec,df_plot_yy$anom_md),
#   #                            'tipo' = c(rep('EC',length(df_plot_yy$anom_ec)),
#   #                                       rep('MD',length(df_plot_yy$anom_md)))
#   #   )
#   #   
#   #   # # salvo i dataset delle serie temporali(anno medio)
#   #   # write.csv(df_season,paste0(dir_out,
#   #   #        as.character(lista_siti$nome_fluxnet[cy_s]),"_",
#   #   #        var_eddy[var_md == y_lab],"_valid_season_DD.csv"),quote = F,row.names = F)
#   #   # write.csv(df_season_mm,paste0(dir_out,
#   #   #        as.character(lista_siti$nome_fluxnet[cy_s]),"_",
#   #   #        var_eddy[var_md == y_lab],"_valid_season_MM.csv"),quote = F,row.names = F)
#   #   # write.csv(df_plot,paste0(dir_out,
#   #   #        as.character(lista_siti$nome_fluxnet[cy_s]),"_",
#   #   #        var_eddy[var_md == y_lab],"_valid_DD.csv"),quote = F,row.names = F)
#   #   # write.csv(df_plot_mm,paste0(dir_out,
#   #   #         as.character(lista_siti$nome_fluxnet[cy_s]),"_",
#   #   #         var_eddy[var_md == y_lab],"_valid_MM.csv"),quote = F,row.names = F)
#   #   # write.csv(df_plot_yy,paste0(dir_out,
#   #   #         as.character(lista_siti$nome_fluxnet[cy_s]),"_",
#   #   #         var_eddy[var_md == y_lab],"_valid_YY.csv"),quote = F,row.names = F)
#   #   # 
#   #   # plot di validazione serie temporali
#   #   
#   #   # plot seasonal (anno medio)
#   #   anno_medio = ggplot(df_season) +
#   #     geom_ribbon(aes(x = TIME,ymax = EC_max, ymin = EC_min,fill = 'EC'),alpha = 0.4) +
#   #     geom_ribbon(aes(x = TIME,ymax = MD_max, ymin = MD_min,fill = 'MD'),alpha = 0.4) +
#   #     geom_line(aes(x = TIME,y = EC,color = 'EC')) +
#   #     geom_line(aes(x = TIME,y = MD,color = 'MD')) +
#   #     scale_color_manual(labels = c('EC' = var_eddy[cyv],'MD' = as.character(df_mod2$filename2[1])), values = c('red','blue')) +
#   #     scale_fill_manual(labels = c('EC' = var_eddy[cyv],'MD' = as.character(df_mod2$filename2[1])), values = c('red', 'blue')) +
#   #     theme(legend.title = element_blank(),axis.title.x = element_blank(),
#   #           legend.position="top") +
#   #     ggtitle(paste(nome_sito,'Daily')) +
#   #     ylab(y_lab)
#   #   
#   #   # plot seasonal (mese medio)
#   #   mese_medio = ggplot(df_season_mm) +
#   #     geom_ribbon(aes(x = MONTH,ymax = EC_max, ymin = EC_min,fill = 'EC'),alpha = 0.4) +
#   #     geom_ribbon(aes(x = MONTH,ymax = MD_max, ymin = MD_min,fill = 'MD'),alpha = 0.4) +
#   #     geom_line(aes(x = MONTH,y = EC,color = 'EC')) +
#   #     geom_line(aes(x = MONTH,y = MD,color = 'MD')) +
#   #     scale_color_manual(labels = c('EC' = var_eddy[cyv],'MD' = as.character(df_mod2$filename2[1])), values = c('red','blue')) +
#   #     scale_fill_manual(labels = c('EC' = var_eddy[cyv],'MD' = as.character(df_mod2$filename2[1])), values = c('red', 'blue')) +
#   #     scale_x_continuous(breaks = seq(1,12),labels=c("J","F","M","A","M","J","J","A","S",'O','N','D')) +
#   #     theme(legend.title = element_blank(),axis.title.x = element_blank(),
#   #           legend.position="top") +
#   #     ggtitle(paste(nome_sito,'Monthly')) +
#   #     ylab(y_lab)
#   #   
#   #   # plot anomalie annuali 
#   #   anom = ggplot(df_plot_bar) +
#   #     geom_bar( aes(x=TIME, y=anom, fill = tipo),stat="identity",
#   #               position=position_dodge()) +
#   #     scale_color_manual(labels = c('EC' = var_eddy[cyv],'MD' = as.character(df_mod2$filename2[1])), values = c('red','blue')) +
#   #     scale_fill_manual(labels = c('EC' = var_eddy[cyv],'MD' = as.character(df_mod2$filename2[1])), values = c('red', 'blue')) +
#   #     theme(legend.title = element_blank(),axis.title.x = element_blank(),
#   #           legend.position="top") +
#   #     ggtitle(paste(nome_sito,'Annual')) +
#   #     ylab(y_lab)
#   #   
#   #   # plot delle serie temporali
#   #   serie_time = ggplot(df_plot) +
#   #     geom_ribbon(aes(x = TIME,ymax = EC_max, ymin = EC_min),alpha = 0.4) +
#   #     geom_point(aes(x = TIME,y = EC,color = 'EC')) +
#   #     geom_point(aes(x = TIME,y = MD,color = 'MD'),shape = 1) +
#   #     scale_color_manual(values = c('EC' = 'red', 'MD' = 'blue')) +
#   #     theme(legend.title = element_blank(),axis.title.x = element_blank(),
#   #           legend.position="none") +
#   #     # ggtitle(paste(as.character(lista_siti$nome_fluxnet[cy_s]),'Daily')) +
#   #     ylab(y_lab)
#   #   
#   #   df_plot_mm$TIME = ymd((df_plot_mm$YEAR*10000) + (df_plot_mm$MONTH*100) +1)
#   #   
#   #   serie_time_mm = ggplot(df_plot_mm) +
#   #     geom_ribbon(aes(x = TIME,ymax = EC_max, ymin = EC_min),alpha = 0.4) +
#   #     geom_point(aes(x = TIME,y = EC,color = 'EC')) +
#   #     geom_point(aes(x = TIME,y = MD,color = 'MD')) +
#   #     scale_color_manual(values = c('EC' = 'red', 'MD' = 'blue')) +
#   #     theme(legend.title = element_blank(),axis.title.x = element_blank(),
#   #           legend.position="none") +
#   #     scale_x_date(labels = date_format("%m-%Y")) +
#   #     # ggtitle(paste(as.character(lista_siti$nome_fluxnet[cy_s]),'Annual')) +
#   #     ylab(y_lab)
#   #   
#   #   serie_time_yy = ggplot(df_plot_yy) +
#   #     geom_ribbon(aes(x = YEAR,ymax = EC_max, ymin = EC_min),alpha = 0.4) +
#   #     geom_point(aes(x = YEAR,y = EC,color = 'EC')) +
#   #     geom_point(aes(x = YEAR,y = MD,color = 'MD')) +
#   #     scale_color_manual(values = c('EC' = 'red', 'MD' = 'blue')) +
#   #     theme(legend.title = element_blank(),axis.title.x = element_blank(),
#   #           legend.position="none") +
#   #     # ggtitle(paste(as.character(lista_siti$nome_fluxnet[cy_s]),'Annual')) +
#   #     ylab(y_lab)
#   #   
#   #   # plot di validazione regressioni
#   #   nr_dati = length(df_plot[,1])
#   #   nr_dati_mm = length(df_plot_mm[,1])
#   #   nr_dati_yy = length(df_plot_yy[,1])
#   #   
#   #   # sincronizzo in base ai NA
#   #   for (cy_ck in seq(1,length(df_plot_yy)) ) {
#   #     pos_na = which(is.na(df_plot_yy[,cy_ck]) == 1)
#   #     if ( length(pos_na) > 0 ) {
#   #       df_plot_yy = df_plot_yy[-1*pos_na,]
#   #     }
#   #     rm(pos_na)
#   #   }
#   #   rm(cy_ck)
#   #   # sincronizzo in base ai NA
#   #   for (cy_ck in seq(1,length(df_plot_mm)) ) {
#   #     pos_na = which(is.na(df_plot_mm[,cy_ck]) == 1)
#   #     if ( length(pos_na) > 0 ) {
#   #       df_plot_mm = df_plot_mm[-1*pos_na,]
#   #     }
#   #     rm(pos_na)
#   #   }
#   #   rm(cy_ck)
#   #   
#   #   # sincronizzo in base ai NA
#   #   for (cy_ck in seq(1,length(df_plot)) ) {
#   #     pos_na = which(is.na(df_plot[,cy_ck]) == 1)
#   #     if ( length(pos_na) > 0 ) {
#   #       df_plot = df_plot[-1*pos_na,]
#   #     }
#   #     rm(pos_na)
#   #   }
#   #   rm(cy_ck)
#   #   
#   #   # regressione lineare daily
#   #   model_lm <- lm(MD ~ EC, data = df_plot)
#   #   
#   #   df_plot$LM = model_lm$fitted.values
#   #   
#   #   text_df = data.frame('pos_x' = min(df_plot$EC),'pos_y' = max(df_plot$LM)*0.99,
#   #                        'testo' = 
#   #                          sprintf("MD = %.2f EC %+.2f ; r2 = %.2f",
#   #                                  coef(model_lm)[2],
#   #                                  coef(model_lm)[1],summary.lm(model_lm)$r.squared)
#   #   )
#   #   p1 = 
#   #     ggplot() +
#   #     geom_point(data = df_plot,aes(x = EC,y = MD),size=0.5) +
#   #     ggtitle(sprintf('MD: mean = %.2f gC m-2 day-1 (sd: %.2f) (n = %.0f%%)\nEC: mean = %.2f gC m-2 d-1 (sd: %.2f) (n = %.0f%%)',
#   #                     mean(df_plot$MD),sd(df_plot$MD),100*(length(df_plot$MD)/nr_dati),
#   #                     mean(df_plot$EC),sd(df_plot$EC),100*(length(df_plot$EC)/nr_dati)),
#   #             subtitle = sprintf('RMSE: %.2f; %s',sqrt(mean((df_plot$EC - df_plot$MD)^2)),text_df$testo)
#   #     )+
#   #     geom_line(data = df_plot,aes(x = EC,y = LM),color='red',size = 1) +
#   #     # geom_text(data = text_df,aes(x = pos_x, y = pos_y, label = testo),hjust=-0.01) +
#   #     xlab('Measured') +
#   #     ylab('Simulated') +
#   #     theme(plot.title = element_text(size = 10,hjust = 0.5),
#   #           plot.subtitle = element_text(size=9,hjust=0.5),
#   #           axis.title = element_text(size = 9),
#   #           axis.text = element_text(size=9),
#   #           legend.title = element_blank(),legend.position="none",
#   #           legend.direction="horizontal")
#   #   
#   #   # regressione lineare monthly
#   #   model_lm_mm <- lm(MD ~ EC, data = df_plot_mm)
#   #   
#   #   df_plot_mm$LM = model_lm_mm$fitted.values
#   #   text_df = data.frame('pos_x' = min(df_plot_mm$EC),'pos_y' = max(df_plot_mm$LM)*0.99,
#   #                        'testo' = 
#   #                          sprintf("MD = %.2f EC %+.2f ; r2 = %.2f",
#   #                                  coef(model_lm_mm)[2],
#   #                                  coef(model_lm_mm)[1],summary.lm(model_lm_mm)$r.squared)
#   #   )
#   #   p1_mm = 
#   #     ggplot() +
#   #     geom_point(data = df_plot_mm,aes(x = EC,y = MD),size=0.5) +
#   #     ggtitle(sprintf('MD: mean = %.2f gC m-2 month-1 (sd: %.2f) (n = %.0f%%)\nEC: mean = %.2f gC m-2 d-1 (sd: %.2f) (n = %.0f%%)',
#   #                     mean(df_plot_mm$MD),sd(df_plot_mm$MD),100*(length(df_plot_mm$MD)/nr_dati_mm),
#   #                     mean(df_plot_mm$EC),sd(df_plot_mm$EC),100*(length(df_plot_mm$EC)/nr_dati_mm)),
#   #             subtitle = sprintf('RMSE: %.2f; %s',sqrt(mean((df_plot_mm$EC - df_plot_mm$MD)^2)),text_df$testo)
#   #     )+
#   #     geom_line(data = df_plot_mm,aes(x = EC,y = LM),color='red',size = 1) +
#   #     # geom_text(data = text_df,aes(x = pos_x, y = pos_y, label = testo),hjust=-0.01) +
#   #     xlab('Measured') +
#   #     ylab('Simulated') +
#   #     theme(plot.title = element_text(size = 10,hjust = 0.5),
#   #           plot.subtitle = element_text(size=9,hjust=0.5),
#   #           axis.title = element_text(size = 9),
#   #           axis.text = element_text(size=9),
#   #           legend.title = element_blank(),legend.position="none",
#   #           legend.direction="horizontal")
#   #   
#   #   # regressione lineare annual
#   #   model_lm_yy <- lm(MD ~ EC, data = df_plot_yy)
#   #   
#   #   df_plot_yy$LM = model_lm_yy$fitted.values
#   #   
#   #   text_df = data.frame('pos_x' = min(df_plot_yy$EC),'pos_y' = max(df_plot_yy$LM)*1.04,
#   #                        'testo' = 
#   #                          sprintf("MD = %.2f EC %+.2f ; r2 = %.2f",
#   #                                  coef(model_lm_yy)[2],
#   #                                  coef(model_lm_yy)[1],summary.lm(model_lm_yy)$r.squared)
#   #   )
#   #   p1_yy = ggplot(df_plot_yy) +
#   #     geom_point(aes(x = EC,y = MD),size=0.5) +
#   #     ggtitle(sprintf('MD: mean = %.2f gC m-2 year-1 (sd: %.2f) (n = %.0f%%)\nEC: mean = %.2f gC m-2 d-1 (sd: %.2f) (n = %.0f%%)',
#   #                     mean(df_plot_yy$MD),sd(df_plot_yy$MD),100*(length(df_plot_yy$MD)/nr_dati_yy),
#   #                     mean(df_plot_yy$EC),sd(df_plot_yy$EC),100*(length(df_plot_yy$EC)/nr_dati_yy)
#   #     ),subtitle = sprintf('RMSE: %.2f; %s',
#   #                          sqrt(mean((df_plot_yy$EC - df_plot_yy$MD)^2)),text_df$testo))+
#   #     geom_line(aes(x = EC,y = LM),color='red',size = 1) +
#   #     # geom_text(data = text_df,aes(x = pos_x, y = pos_y, label = testo),hjust=-0.01) +
#   #     xlab('Measured') +
#   #     ylab('Simulated') +
#   #     theme(plot.title = element_text(size = 10,hjust = 0.5),
#   #           plot.subtitle = element_text(size=9,hjust=0.5),
#   #           axis.title = element_text(size = 9),
#   #           axis.text = element_text(size=9),
#   #           legend.title = element_blank(),legend.position="none",
#   #           legend.direction="horizontal")
#   #   
#   #   mp_t = plot_grid(plotlist = list(anno_medio,mese_medio,anom,
#   #                                    serie_time,serie_time_mm,serie_time_yy,
#   #                                    p1,p1_mm,p1_yy),
#   #                    ncol = 3,align = 'v',labels = 'AUTO')
#   #   
#   #   lista_plot[[length(lista_plot)+1]] = mp_t
#   #   
#   #   rm(df_plot_yy,df_plot,y_lab,p1,serie_time,p1_yy,serie_time_yy,mp_t)
#   #   rm(model_lm,model_lm_yy,text_df,df_plot_bar)
#   # }
#   # rm(cyv,TIME,df_ec,df_mod2)
#   # 
#   # return(lista_plot)
#     
# }
# 
# #   }
# #   rm(cy_s,var_eddy_unc_max,var_eddy_unc_min)
# #   rm(var_eddy,var_eddy_qc)
# #   
# # }
# # rm(cy_ec_mth,var_md,soglia_qc)
