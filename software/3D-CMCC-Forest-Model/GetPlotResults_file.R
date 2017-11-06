
GetPlotResults_file <- function(all_out_files2,lista_v) {
  # funzione per creare i plot di tutte le variabili prodotte dal modello
  # i colori definiscono i file
  # n.b.: i nomi sulla legenda usano la prima parte del file
  #       (splittando con separatore = '_')
  # INPUT
  #   all_out_files2: lista delle path complete dei nomi dei file
  #   lista_v: lista delle variabili che NON deve plottare
  # OUTPUT
  #   out_pdf: nome del pdf (path completa) dove salvo i plot
  
  library(ggplot2)
  library(lubridate)
  library(cowplot)
  #  = 'C:/Users/CMCC/Desktop/get_plot_results/output/Test_output_Rstudio-5.3.3-ISIMIP-Soroe-FT'
  # # list of the files to plot
  # all_out_files2 = list.files(dir_in, recursive = TRUE, full.names = TRUE)
  # cy_time = 'annual'
  # all_out_files2 = all_out_files2[grep(cy_time,all_out_files2)]
  # # exclude all PDF files
  # if ( length( grep('.pdf',all_out_files2) ) > 0 ) {
  #   all_out_files2 = all_out_files2[-1*grep('.pdf',all_out_files2)]
  # }
  
  for (f in all_out_files2) {
    
    file_name = unlist(strsplit(f,'/'))
    file_name = file_name[length(file_name)]
    
    cat(paste0('\nImport file: ',f,'\n'))
    
    # read file
    outputCMCC <-read.csv(f,header=T,comment.char = "#")
    
    if ( grepl('annual_',file_name) ) {
      time = c('annual')
      outputCMCC$Date <- paste0("01/01/",outputCMCC$YEAR)
      outputCMCC$Date <- as.Date(outputCMCC$Date, format = "%d/%m/%Y")
      # start_col = 6# ecco l'errore, lui skippa le prime sei colonne.....
      
    }
    if ( grepl('monthly_',file_name) ) {
      time = c('monthly')
      outputCMCC$Date <- paste0("01/",outputCMCC$MONTH, "/",outputCMCC$YEAR)
      outputCMCC$Date <- as.Date(outputCMCC$Date, format = "%d/%m/%Y")
      # start_col = 6
    }
    if ( grepl('daily_',file_name) ) {
      time = c('daily')
      outputCMCC$Date <- paste0(outputCMCC$DAY,"/",outputCMCC$MONTH, "/",outputCMCC$YEAR)
      outputCMCC$Date <- as.Date(outputCMCC$Date, format = "%d/%m/%Y")
      # start_col = 6
    }
    
    cat(paste0('\nImport_', time, '_file: ',f,' OK\n'))
    
    outputCMCC$filename = unlist(strsplit(file_name,'_'))[1]
    
    if ( exists('df_t') ) {
      df_t = rbind(df_t,outputCMCC)
    } else {
      df_t = outputCMCC
    }
    rm(outputCMCC,file_name)
  }
  rm(f)
  
  # plot output model 
  # pdf(out_pdf,
  #     onefile = T, width = 30,height = 24)
  # 
  lista_p = list()

  for (i in seq(1,length(df_t)) ) {
      # cat(colnames(df_t[i]))
      
    if ( length(which(lista_v == colnames(df_t[i]))) > 0 ) {
        # cat(' SKIPPED!!!!\n')
      next
    }
    # cat('\n')
    
    df_single_value = data.frame('Date' = ymd(df_t$Date),
                                 'var_name' = colnames(df_t[i]),
                                 'file_name' = df_t$filename)
    if ( is.factor(df_t[,i]) ) {
      df_single_value$v1 = as.character(df_t[,i])
    } else {
      df_single_value$v1 = df_t[,i]
    }

    mpt = ggplot(df_single_value) +
      geom_line(aes(x = Date, y = v1, color = file_name),size = 1.5) +
      ylab(colnames(df_t[i])) +
      theme(legend.position = "top",
            axis.title = element_text(size = 20),
            axis.text = element_text(size = 20),
            plot.title = element_text(20),
            legend.text=element_text(size=20),
            legend.title = element_blank())
    xlab('year')
    lista_p[[length(lista_p)+1.5]] = mpt
  }
  return(lista_p)
  
  #   if ( length(lista_p) == 12 ) {
  #     mpt = plot_grid(plotlist = lista_p,ncol = 3,align = 'h')
  #     print(mpt)
  #     lista_p = list()
  #   }
  # }
  # dev.off()
  # 
  # cat(paste0(out_pdf,' created!\n'))
  
}
