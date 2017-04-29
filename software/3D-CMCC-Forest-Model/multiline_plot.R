# Developed by:
# Alessio Collalti (alessio.collalti@cmcc.it)
# Carlo Trotta (trottacarlo@unitus.it)
# Gaetano Pellicone (gaetano.pellicone@isafom.cnr.it)
# starting date: 27 April 2017


rm(list = ls())
library(ggplot2)
library(grid)
library(gridExtra)
library(cowplot)

# setwd('/home/alessio-cmcc/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/')
getwd()
# source('multiplot.R')

input_dir = ('/home/alessio-cmcc/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/output/output_Rstudio-5.3.2-ISIMIP-Hyytiala-2B/output_5.3.2-ISIMIP_2017_APRIL_30/annual/')
file_name = 'annual_5.3.2-ISIMIP_Hyytiala_GCM2_rcp2p6.txt_(2006-2299)_CO2_ON_CO2_rcp2p6.txt_Man_ON_d_10000_txt.txt'

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

# aggiungo colonne che hanno un solo valore (non per specie)
df_tmp = df

# riorganizzo il df mettendo stessa variabile diversa sp su un unica colonna
df_single_value = data.frame('YEAR' = df_tmp$YEAR) 
df_multi_spp = data.frame('YEAR' = rep(df_tmp$YEAR, length(col_sp)) )

df_tmp = df_tmp[,-1* which(colnames(df_tmp) == 'YEAR')]

while (1) {
    
    nome_var = colnames(df_tmp)[1]
    
    cat(paste0('variable: ',nome_var,'\n'))
    
    col_v = which(colnames(df_tmp) == nome_var)
    
    for (cySP in seq(1,length(col_sp))) {
        if ( length(which(colnames(df_tmp) == paste0(nome_var,'.',cySP))) == 0 ) {
            break
        }
        col_v = c(col_v,which(colnames(df_tmp) == paste0(nome_var,'.',cySP)))
    }
    
    if ( length(col_v) == 1 ) {
        if ( is.factor(df_tmp[,col_v]) ) { 
            df_single_value = cbind(df_single_value,as.character(df_tmp[,col_v]))
        } else {
            df_single_value = cbind(df_single_value,df_tmp[,col_v])
        }
        colnames(df_single_value)[which(colnames(df_single_value) == "df_tmp[, col_v]")] = nome_var
        
    } else {
        
        tmp_v = c()
        
        for (cy_v in seq(1,length(col_v)) ) {
            if ( is.factor(df_tmp[,col_v[cy_v]]) ) {
                tmp_v = c(tmp_v,as.character(df_tmp[,col_v[cy_v]]))
            } else {
                tmp_v = c(tmp_v,df_tmp[,col_v[cy_v]])
            }
        }
        
        df_multi_spp = cbind(df_multi_spp,tmp_v)
        
        colnames(df_multi_spp)[which(colnames(df_multi_spp) == "tmp_v")] = nome_var
        
        rm(cy_v,tmp_v)
    }
    
    df_tmp = df_tmp[,-1*col_v]
    
    rm(col_v,cySP)
    
    if ( is.null(colnames(df_tmp)) ) {
        break
    }
}

if ( length(df_tmp) > 0 ) {
    df_single_value$last_col = df_tmp
    colnames(df_single_value)[which(colnames(df_single_value) == "last_col")] = colnames(df)[length(df)]
}

rm(df_tmp)

pdf(gsub('_txt.txt','_txt.pdf',file_name), onefile = T)
n_col_plot = 5
cy0 = 1
while (1) {
    
    cy0 = cy0 + 1
    cy0
    list_p = list()
    
    for ( cy1 in seq(cy0,cy0 +19) ) {
        if (cy1 > length(df_multi_spp)) {
            break
        }
        df1 = data.frame('YEAR' = as.Date(as.character(df_multi_spp$YEAR),'%Y'), 'v1' = df_multi_spp[,cy1],'SPECIES' = df_multi_spp$SPECIES)
        
        p1 = ggplot(df1, aes(x = YEAR,y = v1, colour = SPECIES)) +
            geom_line() + 
            ylab(colnames(df_multi_spp)[cy1]) +
            xlab('') + 
            theme(legend.title = element_blank(),legend.position="top", legend.direction="horizontal",
                  axis.text=element_text(size=5),axis.title=element_text(size=5),
                  plot.title=element_text(size=6),legend.text=element_text(size=5) 
            )
        
        list_p[[length(list_p)+1]] = p1
        rm(p1,df1)
    }
    
    cy0 = cy1
    rm(cy1)
    
    mp = plot_grid(plotlist = list_p, ncol = n_col_plot)
    
    print(mp)
    
    rm(list_p,mp)
    if (cy0 > length(df_multi_spp)) {
        break
    }
}
rm(cy0)

cy0 = 1
while (1) {
    
    cy0 = cy0 + 1
    cy0
    list_p = list()
    
    # cy1 = cy0 + 1
    # cy1 = 1
    for ( cy1 in seq(cy0,cy0 +19) ) {
        if (cy1 > length(df_single_value)) {
            break
        }
        
        cat(paste0('\n',colnames(df_single_value)[cy1],'\n'))
        
        df1 = data.frame('YEAR' = df_single_value$YEAR, 'v1' = df_single_value[,cy1])
        
        p1 = ggplot(df1, aes(x = YEAR,y = v1)) +
            geom_line() + 
            ylab(colnames(df_single_value)[cy1]) +
            xlab('') + 
            theme(legend.title = element_blank(),legend.position="top", legend.direction="horizontal",
                  axis.text=element_text(size=5),axis.title=element_text(size=5),
                  plot.title=element_text(size=6),legend.text=element_text(size=5) 
            )
        
        list_p[[length(list_p)+1]] = p1
        rm(p1,df1)
    }
    
    cy0 = cy1
    rm(cy1)
    
    mp = plot_grid(plotlist = list_p, ncol = n_col_plot)
    
    print(mp)
    
    rm(list_p,mp)
    if (cy0 > length(df_single_value)) {
        break
    }
}
rm(cy0)

dev.off()

cat(paste0('\n',gsub('_txt.txt','_txt.pdf',file_name),'\n'))
