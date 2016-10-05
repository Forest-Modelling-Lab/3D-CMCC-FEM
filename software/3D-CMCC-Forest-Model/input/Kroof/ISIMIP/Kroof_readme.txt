for ISIMIP simulations 19xx-2099 in Kroof we used:

-input stand data coming from Database dataset making an 
 weighted average of all single dbh-height trees for 1997
 data are stored in 3D-CMCC git Project "Kroof_stand.xlsx" file:
 FILE NAME: "Kroof_stand_1997_ISIMIP.txt" (unique for all simulations starting from 1997)


/* modified below this was for soroe use it as a template for Kroof */
 
-input soil data coming from database data SOIL.csv and psot processed in Soil_TEXTURE.xlsx file 
 soil texture is a weighted average of texutres for each soil horizon
 (soil depth is that used in Collalti et al., 2016, coming from Dalsgaard et al., 2011 for unconsistency of those in database)
 each soil data are the same of all soil files,
 except for "sitename" variable that is different and is used ONLY for file signatures in output files
 FILE NAME: "Soroe_soil_x.x_ISIMIP.txt"

-input metdata for all GCMs and all RCPs comes from elaboration of Monia Santini 
 FILE NAME: "GCMx_hist_rcpxpx_2001_2099.txt"

-input model settings is all the same for all simulation but is different for management on and off 
 FILE NAME: "Soroe_settings_ISIMIP.txt" (unique for all simulations with no management)
 FILE NAME: "Soroe_settings_ISIMIP_Manag_on.txt" (unique for all simulations with management)

-input topo is one for all simulation
 FILE NAME: "Soroe_topo_ISIMIP.txt" (unique for all simulations)

-parameterization file for Fagussylvatica is stored in parameterization folder 
 FILE NAME: "Fagussylvatica.txt"(unique for all simulations)
 
-input CO2 file:
 *for simulations when CO2 is fixed (CO2_fixed "on") we use the same co2 input file for all GCMs and all rcps :"CO2_hist_rcps_1950_2000.txt"
 these simulation use a transient CO2 concentration up to year 2000, after that CO2 is equal for all years (file has been made getting the 
 transient CO2 files and changing values after year 2000 using the value of year 2000
 *for simulations when CO2 in NOT fixed (CO2_fixed "off") we use different CO2 file based ONLY on rcps used
 NOTE: reference value for F_CO2 = 1 is:  CO2CONC "368.865"  //CO2 concentration refers to 2000 as ISIMIP PROTOCOL
 *for simulation with Historical climate dataset (experiments 1a) over 2005 data (to 2012) has been filled with values coming from CO2_rcps files
 
 CLIMATE (1a) experiment: in this case model initialization refers to stand data of 1996 from Andrea Ibrom file, to reason of model behaviour variable
 "THINNING" in Fagussylvatica.txt has been setted to 10 (instead 15) for consistency with ohter simulations where thinning happens in 2005 (otherwise should
 be happened in 2010 in this case)
 
 and example of launch:
 
-i input/DKSoroe
-p input/parameterization
-o output/debug_output/debug_output
-b output/daily_output/daily_output
-e output/annual_output/annual_output
-f output/monthly_output/monthly_output
-d ISIMIP/Soroe_stand_2001_ISIMIP.txt
-m ISIMIP/GCM1/GCM1_hist_rcp6p0_2001_2099.txt
-s ISIMIP/GCM1/Soroe_soil_6.0_prova_isimip.txt
-t ISIMIP/Soroe_topo_ISIMIP.txt
-c ISIMIP/Soroe_settings_ISIMIP.txt
-n output/soil_output/soil_output
-k ISIMIP/CO2/CO2_rcp6p6_1950_2099.txt