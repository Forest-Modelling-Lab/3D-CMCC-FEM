# Farquhar Equation Sensitivity
# Code from 3D-CMCC model, photosynthesis2.c

# Starting R Script ----

rm(list = ls())

library(ggplot2)
library(ggrepel)

# Create directory output to save the plot
dir_in = paste0(gsub('tables','output',getwd()),'/')
dir.create(dir_in,showWarnings = F)
dir_out = paste0(dir_in,'/Farquhar_Plot/')
dir.create(dir_out,showWarnings = F)

# Costant variables

Kc25           = 404.9  #(ubar) michaelis-menten const carboxylase, 25 deg C  Badger and Collatz value
q10Kc          = 2.1    #(DIM) Q_10 for Kc Badger and Collatz and Collatz et al., (2001)
Ko25           = 248    #(mbar) michaelis-menten const oxygenase, 25 deg C Badger and Collatz value
q10Ko          = 1.2    #(DIM) inhibition constant for O2 Collatz et al., (1991)
act25          = 3.6    #(umol/mgRubisco/min) Rubisco activity at 25 C Badger and Collatz value
q10act         = 2.4    #(DIM) Q_10 for Rubisco activity Badger and Collatz value Collatz et al., (1991)
phiII          = 0.85   #(DIM) fraction of PAR effectively absorbed by photosytem II (leaf absorptance); 0.8 for Bonan et al., 2011 */
fnr            = 7.16   #(DIM) g Rubisco/gN Rubisco weight proportion of rubisco relative to its N content Kuehn and McFadden (1969)
thetaII        = 0.7    #(DIM) curvature of the light-response curve of electron transport (DePury and Farquhar, 1997, Bonan et al., 2011)
Rgas           = 8.3144 #Gas constant

beta           = 2.1    #ratio between Vcmax and Jmax see dePury and Farquhar 1997; for fagus see Liozon et al., (2000) and Castanea */
test_Vcmax     = 55     #(umol/m2/sec) Vcmax for fagus see Deckmyn et al., 2004 GCB */
test_Jmax      = 100    #(umol/m2/sec) Jmax for fagus see Deckmyn et al., 2004 GCB */

ppe            = 2.6    #(mol e- /mol photons) photons absorbed by PSII per e- transported (quantum yield of electron transport)
O2CONC         = 20.9   #(ratio) % of [O2]


#fixed variables that however change into 3D-CMCC-FEM
co2Conc        = 400    #(ppmv) Co2 concentration

tday           = 25     #(°C) daily temperature 
air_pressure   = 100854 #(Pa) air pressure
N_RUBISCO      = 0.16   #(ratio) Fraction of Rubisco per Leaf Nitrogen ground area
cond_corr      = 0.52   #(umol/m2/s/Pa)
leafN          = c(1.5, 1.4, 2.9) #Lean nitrogen content per ground area (sun, shadedd, total leaves)
leaf_day_mresp = c(0.08, 0.07, 0.15) #Daytime leaf maint respiration per ground area (sun, shadedd, total leaves)

# Calculating other variables

if (exists('df_tot')) rm(df_tot)

par_abs = seq(0,1500,by = 50)

for (j in seq(1:3)) {
    
    # calculate Rd (umol/m2/s) day leaf m. resp, proj. area basis
    Rd          = leaf_day_mresp[j]
    
    # convert atmospheric CO2 from ppmV --> Pa;
    # Ca (Pa) atmospheric [CO2] pressure
    Ca          = co2Conc * air_pressure / 1000000
    
    # calculate atmospheric O2 in Pa, assumes 20.9% O2 by volume
    # O2 (Pa) intercellular O2 partial pressure, taken to be 0·21 (mol mol-1) see Medlyn et al., 1999
    O2          = (O2CONC / 100) * air_pressure
    
    ########################################################################################
    
    # calculate:
    # Kc (michaelis-menten constant for carboxylase reaction)
    # act (Rubisco activity scaled by temperature and [O2] and [CO2])
    
    #Michaelis Menten approach
    if (tday > 15) {
        
        Kc        = Kc25  * (q10Kc  ^ ((tday - 25) / 10))
        act       = act25 * (q10act ^ ((tday - 25) / 10))
        
    } else {
        
        Kc        = Kc25  * ((1.8 * q10Kc)  ^ ((tday - 15) / 10)) / q10Kc
        act       = act25 * ((1.8 * q10act) ^ ((tday - 15) / 10 ))/ q10act
        
    }
    
    # unit conversion
    Kc          = Kc    * 0.1        # convert Kc ubar --> Pa
    act         = act   * 1000 / 60  # convert rubisco activity units from umol/mgRubisco/min -> umol/gRubisco/s
    
    # correct kinetic constants for temperature, and do unit conversions
    # Ko (Pa) michaelis-menten constant for oxygenase reaction
    Ko          = Ko25 * (q10Ko ^ ((tday - 25) / 10))
    Ko          = Ko * 100 #mbar --> Pa
    
    ########################################################################################
    
    # see also Bernacchi et al., 2001
    # it assumes Vomax/Vcmax = 0.21; Badger & Andrews (1974), Medlyn et al., (2002) */
    # 0.5 because with 1 mol of oxygenations assumed to release 0.5 molCO2 by glycine decarboxilation (Farquhar and Busch 2017) */
    # 
    #       
    #              Kc * Vomax * O
    #   gamma* = -----------------
    #             (2 * K0 * Vcmax)
    
    #test (units???)
    #gamma = (36.9 + 1.88*(tday-25) + 0.036*(tday-25)*(tday-25))/10
    #gamma = 36.9 + 1.88*(tday-25) + 0.036*(tday-25)*(tday-25)
    
    gamma       = 0.5 * 0.21 * Kc * O2 / Ko
    
    # calculate Vcmax (umol CO2/m2/s) max rate of carboxylation from leaf nitrogen data and Rubisco activity
    
    #  kg Nleaf   kg NRub    kg Rub      umol            umol
    #  -------- X -------  X ------- X ---------   =   --
    #    m2      kg Nleaf   kg NRub   kg RUB * s       m2 * s
    
    #  (lnc)    X  (flnr)  X  (fnr)  X   (act)     =    (Vmax)
    Vcmax       = leafN[j] * N_RUBISCO * fnr * act
    
    # calculate Jmax = f(Vmax) ((umol/m2/s) max rate electron transport)
    Jmax        = beta * Vcmax
    
    # irradiance dependence of electron transport (the "non-rectangular hyperbola")
    # from the equation of de Pury and Farquhar (1997) Plant Cell and Env. and Bernacchi et al., (2003) Plant Cell and Env. */
    # 
    #     theta J^2 - (pabsII + Jmax) J + pabsII Jmax = 0
    
    # compute pabsII ((molPAR/m2/sec) PAR effectively absorbed by the phosystemII)
    pabsII = par_abs * phiII / ppe
    
    # calculate J = f(Jmax, ppfd)
    var_a       = thetaII
    var_b       = -Jmax - pabsII
    var_c       =  Jmax * pabsII
    
    # compute (umol RuBP/m2/s) rate of RuBP (ribulose-1,5-bisphosphate) regeneration
    J           = ( -var_b - sqrt ( var_b * var_b - 4 * var_a * var_c ) ) / ( 2 * var_a )
    
    # solve for Av and Aj using the quadratic equation, substitution for Ci
    #from A = g(Ca-Ci) into the equations from Farquhar von Caemmerer and Berry (1980)
    #and Bernacchi et al., (2001:
    
    #       Vmax (Ci - gamma)
    #Av =  ---------------------   -   Rd
    #       Ci + Kc (1 + O2/Ko)
    
    
    #        J (Ci - gamma)
    #Aj  =  -------------------    -   Rd
    #       4.5 Ci + 10.5 gamma
    
    # quadratic solution for Av
    var_a       =  -1 / cond_corr
    var_b       =  Ca + ( Vcmax - Rd ) / cond_corr + Kc * ( 1 + O2 / Ko )
    var_c       =  Vcmax * ( gamma - Ca ) + Rd * ( Ca + Kc * ( 1 + O2 / Ko ) )
    det         =  var_b * var_b - 4 * var_a * var_c
    
    # compute photosynthesis when Av (or Vc) (umol CO2/m2/s) carboxylation rate for limited assimilation 
    #(net photosynthesis rate when Rubisco activity is limiting)
    temp_Av  = ( -var_b + sqrt( det ) ) / ( 2 * var_a )
    
    # quadratic solution for Aj
    var_a       = -4.5 / cond_corr
    var_b       = 4.5 * Ca + 10.5 * gamma + J / cond_corr - 4.5 * Rd / cond_corr
    var_c       = J * ( gamma - Ca ) + Rd * ( 4.5 * Ca + 10.5 * gamma )
    
    det         = var_b * var_b - 4 * var_a * var_c
    
    # compute photosynthesis when (umol CO2/m2/s) RuBP (ribulose-1,5-bisphosphate) regeneration limited assimilation
    #(net photosynthesis rate when RuBP (ribulose-1,5-bisphosphate)-regeneration is limiting)
    temp_Aj  = ( -var_b + sqrt( det ) ) / ( 2 * var_a )
    
    # compute (umol CO2/m2/s) final assimilation rate; estimate A as the minimum of (Av,Aj)
    temp_A   = c()
    for (cy_minimo in temp_Aj ) {
        if (cy_minimo < temp_Av) {
            temp_A = c(temp_A,cy_minimo)
        } else {
            temp_A = c(temp_A,temp_Av)
        }
    }
    rm(cy_minimo)
    
    if (exists('df_tot')) {
        df_tot = cbind(df_tot,data.frame(temp_A,temp_Av,temp_Aj))
    } else {
        df_tot = data.frame(temp_A,temp_Av,temp_Aj)
    }
    
} #ending for cycle (j)


df_tot            = cbind(par_abs,df_tot)
colnames(df_tot)  = c('PAR','A(sunl)','Av(sunl)','Aj(sunl)','A(shl)','Av(shl)','Aj(shl)','A','Av','Aj')

# organizzo la tabella dei dati per il plot
if (exists('df_plot')) rm(df_plot)

for (cy_col in c('A(sunl)','Av(sunl)','Aj(sunl)','A(shl)','Av(shl)','Aj(shl)','A','Av','Aj')) {
    
    df_temp = df_tot[,c('PAR',cy_col)]
    df_temp$variabile = cy_col
    colnames(df_temp) = c('PAR','v1','v1_name')
    
    if (exists('df_plot')) {
        
        df_plot = rbind(df_plot,df_temp)
        
    } else {
        
        df_plot = df_temp
    }
    rm(df_temp)
}
rm(cy_col)

df_plot$type                                  = 'NA'
pos_nl = grep('shl',df_plot$v1_name)

if ( length(pos_nl) > 0) df_plot$type[pos_nl] = 'SHL'

pos_nl = grep('sunl',df_plot$v1_name)

if ( length(pos_nl) > 0) df_plot$type[pos_nl] = 'SUNL'
rm(pos_nl)

# Plotting results
# df_plot = data.frame ('PAR'=par_abs,'A'=A,'Av'=Av,'Aj'=Aj)
pos_label_av = which(df_plot$v1_name == 'Av')
pos_label_av_sunl = which(df_plot$v1_name == 'Av(sunl)')
pos_label_av_shl = which(df_plot$v1_name == 'Av(shl)')

pos_label_a = which(df_plot$v1_name == 'A')
pos_label_a_sunl = which(df_plot$v1_name == 'A(sunl)')
pos_label_a_shl = which(df_plot$v1_name == 'A(shl)')

pos_label_aj = which(df_plot$v1_name == 'Aj')
pos_label_aj_sunl = which(df_plot$v1_name == 'Aj(sunl)')
pos_label_aj_shl = which(df_plot$v1_name == 'Aj(shl)')


df_label_av = df_plot[c(pos_label_av[length(pos_label_av)],pos_label_av_sunl[length(pos_label_av_sunl)],pos_label_av_shl[length(pos_label_av_shl)],
                        pos_label_aj[length(pos_label_aj)],pos_label_aj_sunl[length(pos_label_aj_sunl)],pos_label_aj_shl[length(pos_label_aj_shl)],
                        pos_label_a[length(pos_label_a)],pos_label_a_sunl[length(pos_label_a_sunl)],pos_label_a_shl[length(pos_label_a_shl)]),]
df_label_av$etichetta = paste0('Ca =',co2Conc,'ppm')

df_plot$v1_name = factor(df_plot$v1_name)
df_plot$v1_name <- factor(df_plot$v1_name, levels = c("Aj", "Aj(sunl)", "Aj(shl)",
                                                      "Av", "Av(sunl)", "Av(shl)",
                                                      "A" , "A(sunl)" , "A(shl)"
))
mp2 =
    ggplot()                                                                                   +
    geom_line(data = df_plot,aes(x=PAR,y = v1,color=v1_name))                                  +
    geom_point(data = df_plot,aes(x=PAR,y = v1,color=v1_name,shape = type))                    +
    geom_text_repel(data = df_label_av, aes(x = PAR, y = v1, label = etichetta),size = 2,
                    box.padding = unit(0.35, "lines"),point.padding = unit(0.5, "lines"))      +
    scale_color_manual(values = c('Av' = 'blue' ,'Av(sunl)'  = 'blue' ,'Av(shl)'  = 'blue' ,
                                  'Aj' = 'green','Aj(sunl)'  = 'green','Aj(shl)'  = 'green',
                                  'A'  = 'red'  ,'A(sunl)'   = 'red'  ,'A(shl)'   = 'red'))    +
    scale_shape_manual(values = c('SUNL' = 1,
                                  'SHL' = 0 ,
                                  'NA' = 2 ))                                                  +  
    xlab('PAR photon flux (umol/m2/s)')                                                        +
    ylab('netCO2 flux (umolCO2/m2/s)')                                                         +
    theme(axis.title.x = element_text(size = 8), axis.title.y = element_text(size = 8), 
          legend.text=element_text(size=6),legend.title=element_blank(),
          panel.grid.minor = element_line(colour="white", size=0.5)) +
    scale_x_continuous(minor_breaks = seq(0 , 1500, 100), breaks = seq(0, 1500, 100))
ggsave(paste(dir_out,'.png',sep = '_',collapse = ''),plot = mp2,width = 8, height = 5)



