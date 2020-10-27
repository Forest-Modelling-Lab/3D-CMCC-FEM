
# importo il file della c02
co2 = read.table('/home/alessio/git/3D-CMCC-LAND/software/3D-CMCC-Forest-Model/input/Soroe/ISIMIP/CO2/CO2_rcp8p5.txt',
                 sep = '\t',header = T)
co2 = co2$CO2_ppm
vpd      = 633 #Pascal;
tday     = 15;
O2CONC   = 0.209
air_pressure = 100845

# constants for Wang et al., 2016 Nature Plants CO2 modifiers computation 
#note: constants not described into Wang et al., 2016 are taken from the BIOME-BGC 

#Michealis Menten kinetic
beta   = 240.;   # (units ??) definition ? 
ni     = 0.89;   # (units ??) vicosity of water relative to its value at 25 °C 
ci     = 0.103;   # carbon cost unit for the maintenance of electron transport capacity 
Kc25   = 404;    # (ubar) michaelis-menten const carboxylase, 25 deg C 
q10Kc  = 2.1;    # (DIM) Q_10 for Kc 
Ko25   = 248.0;  # (mbar) michaelis-menten const oxygenase, 25 deg C 
q10Ko  = 1.2;    # (DIM) Q_10 for Ko 

#Arrehnius kinetic
A1     = 2.419 * (10^13);   # (ppmv) Arrhenius constant for KmCO2 tø dependence in ppm for t>=15
A2     = 1.976 * (10^22);   # (ppmv) Arrhenius constant for KmCO2 tø dependence in ppm for t<15 C 
Ea1    = 59400.0;              # (J mol-1) Activation energy for CO2 fixation (KmCO2 temp dependence) 
Ea2    = 109600.0;	           # (J mol-1) Activation energy for CO2 fixation for t<15 C 
Rgas   = 8.3144                # gas constant
AK02   = 8240;                 # Arrhenius constant
EaKO2  = 13913.5;              # (J mol-1) Activation energy for O2 inhibition 

#convert Tair C° -> K
tairK  = tday + 273.13;


# variables for Wang et al., 2016 Nature Plants CO2 modifiers computation 
# double Kc;           # (Pa) effective Michaelis-Menten coefficienct for Rubisco */
# double gamma;        # (Pa) Photorespiratory compensation point */
# double O2;           # (Pa) atmospheric [O2] */
# double Ko;           # (Pa) michaelis-menten constant for oxygenase reaction */
# double m0;
# double vpd;          # (Pa) vpd */
# double co2;          #*(ppmv) co2 atmospheric concentration */


#/*****************************WANG'S VERSION ***********************************/
#/* CO2 MODIFIER AND ACCLIMATION FOR ASSIMILATION */
#/* For reference see Wang et al., 2016 Nature Plants */


#/* calculate atmospheric O2 in Pa, assumes 20.9% O2 by volume */
O2  = O2CONC * air_pressure;

# Michaelis Menten correct kinetic constants for temperature, and do unit conversions */
Ko  = Ko25 * ( q10Ko ^ ( tday - 25. ) / 10. );
Ko  = Ko * 100.; 


  
if ( tday > 15. )
{
  # compute effective Michaelis-Menten coefficienct for Rubisco (tday in °C) */
  Kc  = Kc25  * ( q10Kc ^ ( tday - 25. ) / 10. );
}else
{
  # compute effective Michaelis-Menten coefficienct for Rubisco (tday in °C) */
  Kc  = Kc25  *  ( 1.8 * q10Kc ^  ( tday - 15. ) / 10.) / q10Kc;
}

#/* convert from ubar --> Pa */
Kc = Kc * 0.1;   


#/* calculate gamma (Pa) CO2 compensation point due to photorespiration, in the absence of maint resp, assumes Vomax/Vcmax = 0.21; Badger & Andrews (1974) */
gamma = 0.5 * 0.21 * Kc * O2 / Ko;


m0 = (co2 - gamma) / ((co2 + (2 * gamma) + (3 * gamma) * sqrt((1.6 * vpd * ni )/(beta * (Kc + gamma)))));

plot(co2, m0)

#/* compute FCO2 modifier to apply to intrinsic quantum yield (gC mol-1) and PPFD (mol m-2 sec-1) absorbed */
F_CO2_WANG = m0 * sqrt(1 - (((ci * 4) / m0) ^ (2 / 3)));

plot(co2, F_CO2_WANG)

#/**********************************************************************************/
