for ISIMIP simulations 2001-2099 in Soroe we used:

-input stand data coming from Andrea Ibrom dataset making an 
 weighted average of 7 dbh-height classes into one single dbh-height weighted average class for 2001
 data are stored in "Stand_data_andreas_Ibrom.xlsx" file 
 FILE NAME: "Soroe_stand_2001_ISIMIP.txt" (unique for all simulations)
 
-input soil data coming from data used in Collalti et al., 2016 (no data are available from ISIMIP package..), each soil file is the same of others,
 except for "sitename" variable that is different and is used ONLY for file signatures in output files
 FILE NAME: "Soroe_soil_x.x_ISIMIP.txt"

-input metdata for all GCMs and all RCPs comes from elaboration of Monia Santini 
 FILE NAME: "GCMx_hist_rcpxpx_2001_2099.txt"

-input model settings is all the same for all simulation and come from Soroe_settings.txt file
 FILE NAME: "Soroe_settings_ISIMIP.txt" (unique for all simulations)

-input topo is one for all simulation
 FILE NAME: "Soroe_topo_ISIMIP.txt" (unique for all simulations)

-parameterization file for Fagussylvatica is stored in parameterization folder 
 FILE NAME: "Fagussylvatica.txt"(unique for all simulations)
 
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