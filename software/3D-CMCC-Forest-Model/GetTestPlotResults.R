# mi sembra che per ora i plot per annual siano OK, che ne dici?
# morto?
# io stacco di sentiamo domani!

rm(list = ls())

setwd('/home/alessio-cmcc/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/')

library(ggplot2)
library(cowplot)
library(lubridate)
source("GetPlotResults_file.R")

# directory principale dei file da elaborare
dir_in = '/home/alessio-cmcc/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/output/'

lista_siti = c('Collelongo','Soroe','Hyytiala')
lista_time = c('annual')

# number of plot for each pdf page
nr_plot_per_page = 24
nr_col_per_page = 4
cy_time = lista_time[1]
cy_s = lista_siti[1]
for (cy_time in lista_time) {
  for (cy_s in lista_siti) {
    l_dir = list.dirs(dir_in,recursive = F)
    l_dir = l_dir[grep(cy_s,l_dir)]
    
    lf = list.files(l_dir,pattern = cy_time,recursive = T, full.names = T)
    
    if ( length(grep('.pdf',lf)) > 0 ) {
      lf = lf[-1*grep('.pdf',lf)]
    }
    
    lista_p = GetPlotResults_file(lf,
                                  c('YEAR','LAYER','SPECIES','MANAGEMENT','filename','Date'))
    
    pdf(paste0(l_dir,'/',cy_time,'_',cy_s,'_file_all_new.pdf'),
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
    
    cat(paste0(l_dir,'/',cy_time,'_',cy_s,'_file_all_new.pdf created!\n'))
    
    rm(lista_p2,lista_p,mpt,ref_plot)
  }
}



# 
# f = all_out_files2[1]
# outputCMCC <-read.csv(f,header=T,comment.char = "#")
# 
# pdf(paste0(getwd(),'/',dirname(all_out_files2[1]),'/',cy_time,"_",site,'_file_all.pdf'),
#     onefile = T, width = 30,height = 24)
# 
# ref_plot = nr_plot_per_page
# while ( length(lista_p) > 0) {
#   cat(paste0('number of variables to plot... ',length(lista_p),'\n'))
#   if ( length(lista_p) < ref_plot ) {
#     ref_plot = length(lista_p2)
#   }
#   lista_p2 = lista_p[seq(1,ref_plot)]
#   
#   mpt = plot_grid(plotlist = lista_p2,ncol = nr_col_per_page,align = 'hv')
#   print(mpt)
#   
#   lista_p = lista_p[-1*seq(1,ref_plot)]
#   
# }
# # }
# 
# dev.off()
# rm(lista_p2)
# cat(paste0(getwd(),'/',dirname(all_out_files2[1]),'/',cy_time,"_",site,'_file_test.pdf created!\n'))
# 
