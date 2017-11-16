import_all_out <- function (dir_in,time_scale) {
  # importo tutti i file di output del modello presenti in dir_in
  # INPUT
  #   dir_in: directory dove sono i file da importare
  #   time_scale: 'annual' o 'monthly' o 'daily'
  # OUTPUT
  #   db_t: dataframe con i dati prodotti dal modello
  #         aggiungo la colonna filename
  #         aggiungo la colonna TIME con la data
  
  library(lubridate)
  
  lf = c()
  for ( dir_in1 in dir_in ) {
    lf = c(lf,list.files(dir_in1, pattern = time_scale,recursive = T, full.names = T))
  }
  
  # lf = list.files(paste0(dir_in,cy_s),pattern = cy_time,recursive = T, full.names = T)
  
  if ( length(grep('.pdf',lf)) > 0 ) {
    lf = lf[-1*grep('.pdf',lf)]
  }
  
  cat(sprintf('\n\n=====\nDirectory: %s\n\t %d file found\n\n',dir_in,length(lf)))
  
  cnt = 0

  for (f in lf) {
    cnt = cnt + 1
    if ( any(grepl('/',f)) ) {
      file_name = unlist(strsplit(f,'/'))
    } else {
      file_name = unlist(strsplit(f,'\\'))
    }
    
    file_name = file_name[length(file_name)]
    
    cat(sprintf('\nImport file: %d / %d\n\t%s\n',cnt,length(lf),file_name))
      
    # read file
    outputCMCC <-read.csv(f,header=T,comment.char = "#")
    
    cat(sprintf('\nImport file: %d / %d\n\t%s OK\n',cnt,length(lf),file_name))
    
    cat(paste0('\tinsert TIME column\n'))
    
    if ( grepl('annual_',file_name) ) {
      outputCMCC$TIME <- ymd(
        (outputCMCC$YEAR*10000) +
        ( 100 * rep(1,length(outputCMCC$YEAR))) +
        ( 1 * rep(1,length(outputCMCC$YEAR)))
      )
    }
    if ( grepl('monthly_',file_name) ) {
      outputCMCC$TIME <- ymd(
        (outputCMCC$YEAR*10000) +
          ( 100 * outputCMCC$MONTH) +
          ( 1 * rep(1,length(outputCMCC$YEAR)))
      )
    }
    if ( grepl('daily_',file_name) ) {
      outputCMCC$TIME <- ymd(
        (outputCMCC$YEAR*10000) +
        ( 100 * outputCMCC$MONTH) +
        ( 1 * outputCMCC$DAY)
      )
    }
    
    cat(paste0('\tinsert TIME column OK\n'))
    
    cat(paste0('\tinsert filename column\n'))
    
    outputCMCC$filename = file_name
    
    cat(paste0('\tinsert filename column OK\n'))
    
    cat(paste0('\nupdate data.frame: df_t\n'))
    if ( exists('df_t') ) {
      df_t = rbind(df_t,outputCMCC)
    } else {
      df_t = outputCMCC
    }
    rm(outputCMCC,file_name)
    cat(paste0('\nupdate data.frame: df_t OK\n'))
  }
  rm(f)
  return(df_t)
}
