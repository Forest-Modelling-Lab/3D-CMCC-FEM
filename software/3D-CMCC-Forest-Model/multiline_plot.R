rm(list = ls())
library(ggplot2)
library(grid)
library(gridExtra)
library(cowplot)

setwd('C:/SviluppoR/')

# source('multiplot.R')

input_dir = ('C:/Users/CMCC/Desktop/profound/')
file_name = 'annual_5.3.2-ISIMIP_Hyytiala_GCM3_rcp2p6.txt_(2006-2299)_CO2_ON_CO2_rcp2p6.txt_Man_ON_d_10000_txt.txt'

df = read.csv(paste0(input_dir,file_name),comment.char = '#')

col_sp = grep(paste0('^','SPECIES'),colnames(df))

lista_to_change = c("Y...","X....","y...",'CC.Proj','BGB.AGB',"DELTA_TREE_VOL.perc.")
lista_to_change2 = c("Y(perc)","X(perc)",'y(perc)','CC-Proj','ratio(BGB_AGB)',"DELTA_TREE_VOL(perc)")

for ( cy1 in seq(1,length(lista_to_change)) ) {
  
  colnames(df)[which(colnames(df) == lista_to_change[cy1])] = lista_to_change2[cy1]
  
  for ( cy2 in seq(1,length(col_sp)) ) {
    if ( length(which(colnames(df) == paste0(lista_to_change[cy1],'.',cy2))) == 0 ) {
      break
    }
    colnames(df)[which(colnames(df) == paste0(lista_to_change[cy1],'.',cy2))] = paste0(lista_to_change2[cy1],'.',cy2)
  }
  rm(cy2)
}
rm(cy1)

# riorganizzo il df mettendo stessa variabile diversa sp su un unica colonna
df2 = data.frame('YEAR' = rep(df$YEAR,rep(length(col_sp))))

lista_var = c(
  'SPECIES','HEIGHT','DBH','AGE','GPP','GR','MR','RA','NPP','CUE','Y(perc)','PeakLAI','CC-Proj',
  'DBHDC','HD','HDMAX','HDMIN','Ntree','VEG_D','FIRST_VEG_DAY','CTRANSP','CINT','CLE','WUE',
  'EFF_LIVE_TOTAL_WOOD_FRAC','MIN_RESERVE_C','RESERVE_C','STEM_C','STEM_LIVE_WOOD_C','STEM_DEAD_WOOD_C',
  'MAX_LEAF_C','MAX_FROOT_C','CROOT_C','CROOT_LIVE_WOOD_C','CROOT_DEAD_WOOD_C','BRANCH_C','BRANCH_LIVE_WOOD_C',
  'BRANCH_DEAD_WOOD_C','TREE_CAI','TREE_MAI','VOLUME','DELTA_TREE_VOL(perc)','DELTA_AGB','DELTA_BGB','AGB','BGB',
  'ratio(BGB_AGB)','DELTA_TREE_AGB','DELTA_TREE_BGB','STEM_AUT_RESP','LEAF_AUT_RESP','FROOT_AUT_RESP','CROOT_AUT_RESP',
  'BRANCH_AUT_RESP'
)

for (cy in seq(1,length(lista_var))) {
  
  cat(paste0('variable: ',cy,' ',lista_var[cy],'\n'))
  
  col_v = which(colnames(df) == lista_var[cy])
  
  for (cySP in seq(1,length(col_sp))) {
    if ( length(which(colnames(df) == paste0(lista_var[cy],'.',cySP))) == 0 ) {
      break
    }
    col_v = c(col_v,which(colnames(df) == paste0(lista_var[cy],'.',cySP)))
  }
  
  tmp_v = c()
  
  for (cy_v in seq(1,length(col_v)) ) {
    if ( is.factor(df[,col_v[cy_v]]) ) {
      tmp_v = c(tmp_v,as.character(df[,col_sp[cy_v]]))
    } else {
      tmp_v = c(tmp_v,df[,col_v[cy_v]])
    }
      
  }
  
  df2 = cbind(df2,tmp_v)
  
  rm(tmp_v,col_v,cySP)
}
rm(cy)

colnames(df2) = c('YEAR',lista_var)

# par(mfrow=c(4,4))
# pdf(gsub('_txt.txt','_txt.pdf',file_name), onefile = T, width = 30,height = 24)
# par(mfrow=c(4,4))

pdf(gsub('_txt.txt','_txt.pdf',file_name), onefile = T)

for (cy in seq(3,length(df2),by=6)) {
  
    list_p = list()
    cy1 = cy
    if ( cy1 < length(df2) ) {
      p1 = ggplot(df2, aes(x = YEAR,y = df2[,cy1], colour = SPECIES)) +
        geom_line() +ylab(colnames(df2)[cy1]) + 
        theme(legend.title = element_blank(),legend.position="top", legend.direction="horizontal")
      list_p[[length(list_p)+1]] = p1
      rm(p1)
    }
    
    
    cy2 = cy1 + 1
    if ( cy2 < length(df2) ) {
      p2 = ggplot(df2, aes(x = YEAR,y = df2[,cy2], colour = SPECIES)) +
        geom_line() +ylab(colnames(df2)[cy2]) + 
        theme(legend.title = element_blank(),legend.position="top", legend.direction="horizontal")
      list_p[[length(list_p)+1]] = p2
      rm(p2)
    }
    
    cy3 = cy2 + 1
    if ( cy3 < length(df2) ) {
      p3 = ggplot(df2, aes(x = YEAR,y = df2[,cy3], colour = SPECIES)) +
        geom_line() +ylab(colnames(df2)[cy3]) +
        theme(legend.title = element_blank(),legend.position="top", legend.direction="horizontal")
      list_p[[length(list_p)+1]] = p3
      rm(p3)
    }
    
    cy4 = cy3 + 1
    if ( cy4 < length(df2) ) {
      p4 = ggplot(df2, aes(x = YEAR,y = df2[,cy4], colour = SPECIES)) +
        geom_line() +ylab(colnames(df2)[cy4]) + 
        theme(legend.title = element_blank(),legend.position="top", legend.direction="horizontal")
      list_p[[length(list_p)+1]] = p4
      rm(p4)
    }
    
    cy5 = cy4 + 1
    if ( cy5 < length(df2) ) {
      p5 = ggplot(df2, aes(x = YEAR,y = df2[,cy5], colour = SPECIES)) +
        geom_line() +ylab(colnames(df2)[cy5]) + 
        theme(legend.title = element_blank(),legend.position="top", legend.direction="horizontal")
      list_p[[length(list_p)+1]] = p5
      rm(p5)
    }
    
    cy6 = cy5 + 1
    if ( cy6 < length(df2) ) {
      p6 = ggplot(df2, aes(x = YEAR,y = df2[,cy6], colour = SPECIES)) +
        geom_line() +ylab(colnames(df2)[cy6]) + 
        theme(legend.title = element_blank(),legend.position="top", legend.direction="horizontal")
      list_p[[length(list_p)+1]] = p6
      rm(p6)
    }
    
    mp = plot_grid(plotlist = list_p, ncol = 2)
    
    print(mp)
    
    rm(list_p,mp,cy1,cy2,cy3,cy4,cy5,cy6)
}

dev.off()

rm(cy)
cat(paste0('\n',gsub('_txt.txt','_txt.pdf',file_name),'\n'))
