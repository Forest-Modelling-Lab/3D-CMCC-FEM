# Farquhar Equation Sensitivity
# Code from 3D-CMCC model, photosynthesis2.c

# Starting R Script ----

rm(list = ls())

library(ggplot2)
library(ggrepel)
library(rstudioapi)

dir_in = dirname(rstudioapi::getActiveDocumentContext()$path)
setwd(dir_in)

# Create directory output to save the plot
dir_in = paste0(gsub('tables','output',dir_in),'/')
dir.create(dir_in,showWarnings = F)
dir_out = paste0(dir_in,'/Farquhar_Plot/')
dir.create(dir_out,showWarnings = F)

# Costant variables

Kc25           = 404    #(ubar) michaelis-menten const carboxylase, 25 deg C  Badger and Collatz value
q10Kc          = 2.1    #(DIM) Q_10 for Kc Badger and Collatz and Collatz et al., (2001)
Ko25           = 248    #(mbar) michaelis-menten const oxygenase, 25 deg C Badger and Collatz value
q10Ko          = 1.2    #(DIM) inhibition constant for O2 Collatz et al., (1991)
act25          = 3.6    #(umol/mgRubisco/min) Rubisco activity at 25 C Badger and Collatz value
q10act         = 2.4    #(DIM) Q_10 for Rubisco activity Badger and Collatz value Collatz et al., (1991)
pabsII_frac    = 0.85   #(DIM) fraction of PAR effectively absorbed by photosytem II (leaf absorptance); 0.8 for Bonan et al., 2011 */
fnr            = 7.16   #(DIM) g Rubisco/gN Rubisco weight proportion of rubisco relative to its N content Kuehn and McFadden (1969)
theta          = 0.7    #(DIM) curvature of the light-response curve of electron transport (DePury and Farquhar, 1997, Bonan et al., 2011)

beta           = 2.1    #ratio between Vcmax and Jmax see dePury and Farquhar 1997; for fagus see Liozon et al., (2000) and Castanea */
test_Vcmax     = 55     #(umol/m2/sec) Vcmax for fagus see Deckmyn et al., 2004 GCB */
test_Jmax      = 100    #(umol/m2/sec) Jmax for fagus see Deckmyn et al., 2004 GCB */

ppe            = 2.6    #(mol e- /mol photons) photons absorbed by PSII per e- transported (quantum yield of electron transport)
O2CONC         = 20.9   #% of [O2] see Verustraete 1994, 2002

#co2Conc        = 400   #Co2 concentration

tday           = 25     #daily temperature (°C)
air_pressure   = 100854 
N_RUBISCO      = 0.16
cond_corr      = 0.52
leafN          = c(1.5, 1.4, 2.9)
leaf_day_mresp = c(0.08, 0.07, 0.15)

ass_type_list = c('tot','sunl','shl')

# Calculating other variables

if (exists('df_tot')) rm(df_tot)

for ( co2Conc in seq(400,1500,by=100) ) {
  
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
    
    # correct kinetic constants for temperature, and do unit conversions
    # Ko (Pa) michaelis-menten constant for oxygenase reaction
    Ko          = Ko25 * (q10Ko ^ ((tday - 25) / 10))
    Ko          = Ko * 100 #mbar --> Pa
    
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
    
    # calculate gamma (Pa) CO2 compensation point due to photorespiration, in the absence of maint (or dark?) respiration
    # BIOME-BGC method 
    
    gamma       = 0.5 * 0.21 * Kc * O2 / Ko
    
    # calculate Vcmax (umol CO2/m2/s) max rate of carboxylation from leaf nitrogen data and Rubisco activity
    
    #  kg Nleaf   kg NRub    kg Rub      umol            umol
    #  -------- X -------  X ------- X ---------   =   --
    #    m2      kg Nleaf   kg NRub   kg RUB * s       m2 * s
    
    #  (lnc)    X  (flnr)  X  (fnr)  X   (act)     =    (Vmax)
    Vcmax       = leafN[j] * N_RUBISCO * fnr * act
    
    # calculate Jmax = f(Vmax) ((umol/m2/s) max rate electron transport)
    Jmax        = beta * Vcmax
    
    # compute pabsII ((molPAR/m2/sec) PAR effectively absorbed by the phosystemII)
    pabsII = par_abs * pabsII_frac / ppe
    
    # calculate J = f(Jmax, ppfd)
    var_a       = theta
    var_b       = -Jmax - pabsII
    var_c       =  Jmax * pabsII
    
    # compute (umol RuBP/m2/s) rate of RuBP (ribulose-1,5-bisphosphate) regeneration
    J           = ( -var_b - sqrt ( var_b * var_b - 4 * var_a * var_c ) ) / ( 2 * var_a )
    
    # solve for Av and Aj using the quadratic equation, substitution for Ci
    #from A = g(Ca-Ci) into the equations from Farquhar and von Caemmerer:
    
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
    temp_Av     = ( -var_b + sqrt( det ) ) / ( 2 * var_a )
    
    # quadratic solution for Aj
    var_a       = -4.5 / cond_corr
    var_b       = 4.5 * Ca + 10.5 * gamma + J / cond_corr - 4.5 * Rd / cond_corr
    var_c       = J * ( gamma - Ca ) + Rd * ( 4.5 * Ca + 10.5 * gamma )

    det         = var_b * var_b - 4 * var_a * var_c
    
    # compute photosynthesis when (umol CO2/m2/s) RuBP (ribulose-1,5-bisphosphate) regeneration limited assimilation
    #(net photosynthesis rate when RuBP (ribulose-1,5-bisphosphate)-regeneration is limiting)
    temp_Aj     = ( -var_b + sqrt( det ) ) / ( 2 * var_a )

    # compute (umol CO2/m2/s) final assimilation rate; estimate A as the minimum of (Av,Aj)
    temp_A      = c()
    for (cy_minimo in temp_Aj ) {
      if (cy_minimo < temp_Av) {
        temp_A  = c(temp_A,cy_minimo)
      } else {
        temp_A  = c(temp_A,temp_Av)
      }
    }
    rm(cy_minimo)
    
    df_A  = data.frame('PAR' = par_abs, 'Ass' = temp_A, 'Ass_name'  = 'A', 'Ass_type'  = ass_type_list[j],
                      'CO2' = co2Conc)
    df_Aj = data.frame('PAR' = par_abs, 'Ass' = temp_Aj, 'Ass_name' = 'Aj', 'Ass_type' = ass_type_list[j],
                       'CO2' = co2Conc)
    df_Av = data.frame('PAR' = par_abs, 'Ass' = temp_Av, 'Ass_name' = 'Av', 'Ass_type' = ass_type_list[j],
                       'CO2' = co2Conc)
    
    if (exists('df_tot')) {
      df_tot = rbind(df_tot,df_A,df_Av,df_Aj)
    } else {
      df_tot = rbind(df_A,df_Av,df_Aj)
    }
    
  } #ending for cycle (j)
  
}# ending for cycle (co2Conc)

# save the output
if ( file.exists( paste0(dir_out,'Farquhar_Sensitivity.csv') )) {
  df_tot2 = read.csv(file = paste0(dir_out,'Farquhar_Sensitivity.csv'))
  df_tot = rbind(df_tot2,df_tot)
  rm(df_tot2)
}

write.csv(df_tot, file = paste0(dir_out,'Farquhar_Sensitivity.csv'),row.names = F)

cat(sprintf('%s\n',paste0(dir_out,'Farquhar_Sensitivity.csv')))

df_tot = read.csv(file = paste0(dir_out,'Farquhar_Sensitivity.csv'))

# Plotting results
mp2 =
ggplot() +
  geom_line(data  = df_tot, aes(x = PAR, y = Ass, color= Ass_name,linetype = Ass_type)) +
  geom_line(data  = df_tot[which(as.character(df_tot$Ass_name) == 'A'),],
            aes(x = PAR, y = Ass, color= Ass_name,linetype = Ass_type))                +
  facet_wrap(~ CO2)                            
ggsave(paste(dir_out,'Farquhar_Sensitivity.png'),plot = mp2,width = 8, height = 5)

