readOutput_model <- function(file_in_name)  {
  # funzione che importa il file prodotto dal modello 
  # INPUT
  #   file_in_name: nome (o path completa) del nome del file
  # OUTPUT
  #   lista_export: una list in cui riporto
  #                 'dataset': data.frame dei dati e relative intestazioni
  #                 'report_elab': informazioni per i dataset usati e per i setting
  #                 'report_error': informazioni su eventuali errori o check condition
  #                 'report_model': informazioni su versione del modello
  
  fid = file(file_in_name)
  r1 = readLines(fid)
  close(fid)
  rm(fid)
  
  
  # estraggo il report per le informazioni sul modello a fine del file
  ini_report_mod = grep('3D-CMCC',r1)
  ini_info_elab = grep('site:',r1)
  
  report_model = NA
  
  if (length(ini_report_mod) > 0) {
    report_model = r1[seq(ini_report_mod,ini_info_elab)]
  }
  rm(ini_report_mod,ini_info_elab)
  
  # estraggo il report per gli errori sul modello a fine del file
  ini_report_error = grep('error',r1)
  ini_info_elab = grep('site:',r1)
  
  report_error = NA
  
  if (length(ini_report_error) > 0) {
    report_error = r1[seq(ini_report_error,ini_info_elab)]
  }
  rm(ini_report_error,ini_info_elab)
  
  # estraggo il report per le informazioni sui setting e file usati dal modello a fine del file
  
  ini_info_elab = grep('site:',r1)
  
  report_elab = NA
  
  if (length(ini_info_elab) > 0) {
    report_elab = r1[seq(ini_info_elab,length(r1))]
  }
  rm(ini_info_elab)
  
  # estraggo i dati e le relative intestazioni
  df = read.csv(file_in_name,comment.char = '#')
  # nel caso di check condition devo fare alcune correzioni ai dati importati
  if ( is.factor(df$YEAR) ) {
    pos_na = which(is.na(df$LAYER) == 1)
    if ( length(pos_na) > 0 ) {
      df = df[-1*pos_na,]
      df$YEAR = as.numeric(as.character(df$YEAR))
    }
    rm(pos_na)
  }
  
  lista_export = list('dataset' = df,
                      'report_elab' = report_elab,
                      'report_error' = report_error,
                      'report_model' = report_model)
  
  return(lista_export)
}
