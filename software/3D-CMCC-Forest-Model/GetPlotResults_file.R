
GetPlotResults_file <- function(df_t,color_variable = NULL,
                                var_to_skip = NULL,
                                file_out_pdf) {
  # plot di tutte le variabili prodotte dal modello
  # INPUT
  #   df_t: dataframe con i dati da plottare
  #   color_variable: variabile da usare per i colori
  #   var_to_skip: variabili da non plottare
  # OUTPUT
  #   lista_p: lista con tutti i plot prodotti

  # library(ggplot2)
  library(lubridate)
  # library(cowplot)
  
  # plot output model 
  
  lista_p = list()
  
  for (i in seq(1,length(df_t)) ) {

    if ( length(which(var_to_skip == colnames(df_t[i]))) > 0 ) {
      cat(sprintf('create timeseries plot of variable: %s SKIPPED\n',colnames(df_t[i])))
      next
    } else {
      cat(sprintf('create timeseries plot of variable: %s\n',colnames(df_t[i])))
    }

    df_single_value = data.frame('Date' = ymd(df_t$TIME),
                                 'var_name' = colnames(df_t[i]),
                                 'color_var' = df_t[,color_variable])
    if ( is.factor(df_t[,i]) ) {
      df_single_value$v1 = as.character(df_t[,i])
    } else {
      df_single_value$v1 = df_t[,i]
    }
    
    mpt = ggplot(df_single_value) +
      geom_line(aes(x = Date, y = v1, color = color_var),size = 0.5) +
      ylab(colnames(df_t[i])) +
      theme(legend.position = "top",
            axis.title = element_text(size = 20),
            axis.text = element_text(size = 20),
            plot.title = element_text(20),
            legend.text=element_text(size=20),
            legend.title = element_blank())
    xlab('year')
    lista_p[[length(lista_p)+1]] = mpt
  }
  
  # number of plot for each pdf page
  nr_plot_per_page = 24
  nr_col_per_page = 4
  
  pdf(file_out_pdf,
      onefile = T, width = 30,height = 24)
  
  ref_plot = nr_plot_per_page
  while ( length(lista_p) > 0) {
    cat(paste0('number of variables to plot... ',length(lista_p),'\n'))
    if ( length(lista_p) < ref_plot ) {
      ref_plot = length(lista_p2)
    }
    lista_p2 = lista_p[seq(1,ref_plot)]
    
    mpt = plot_grid(plotlist = lista_p2,ncol = nr_col_per_page,align = 'hv')
    print(mpt)
    
    lista_p = lista_p[-1*seq(1,ref_plot)]
    
  }
  
  dev.off()
  
  cat(paste0(file_out_pdf,' created!\n'))
  
}