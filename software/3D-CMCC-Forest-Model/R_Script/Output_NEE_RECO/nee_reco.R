# Script to create ISIMIP Output Files starting from 3D-CMCC-FEM Output
# -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-


# Developed by:
#----------------------------------------------------------------------------------------------------------------------------------  
#   Carlo Trotta
#     (Euro-Mediterranean Center on Climate Changes (CMCC))
#     (Department for innovation in biological, agro-food and forest systems (DIBAF) - University of Tuscia)
#     (trottacarlo@unitus.it)

#   Alessio Collalti
#     (Euro-Mediterranean Center on Climate Changes (CMCC))
#     (Institute for Agriculture and Forestry Systems in the Mediterranean of the National Research Council of Italy (CNR-ISAFOM))
#     (alessio.collalti@cmcc.it)

#   Corrado Biondo
#     (Euro-Mediterranean Center on Climate Changes (CMCC))
#     (Department for innovation in biological, agro-food and forest systems (DIBAF) - University of Tuscia)
#     (corrado.biondo@cmcc.it)
#----------------------------------------------------------------------------------------------------------------------------------


#-----------------------------------------------------------------------------------------------------------------------------
# DESCRIPTION ----
#--------------------------------------------------------------------------
# This script converts 3D-CMCC-FEM model annual, monthly and daily output files in ISIMIP output files.

# The 3D-CMCC-FEM files are imported using the following functions:

#     - "list_file.R"           (this function creates a dataframe containing info about the 3D-CMCC-FEM output files path)
#     - "read3DCMCC_output.R"   (this function reads the 3D-CMCC-FEM output files using paths imported by "list_file.R")

# These functions must be contained in the same folder and have to be run.

# This script also needs some csv files as imput to change 3D-CMCC FEM names into ISIMIP ones:

#     - annual_list.csv
#     - daily_list.csv
#     - monthly_list.csv

#     - specie.csv

#     - esm_ISIMIP.csv

#     - clim_scenario.csv
#     - socio-econ-scenario.csv
#     - sens-scenario.csv
#     - gcm.csv
#     - region.csv

#     - climate_scenario.csv
#     - soc-scenario.csv
#     - co2sens-scenario.csv

# Short Overviw of the Operations:
#--------------------------------------------------------------------------
#   - calculating of multiple variables as sum of model variables
#   - matching of 3D-CMCC and ISIMIP name of variables
#   - calculating of monthly LAI_PROJ from daily data
#   - converting of unit of variables
#   - generating of ISIMIP output files in ".txt". The ISIMIP output files contain an YEAR and a VALUE columns without headers
#-----------------------------------------------------------------------------------------------------------------------------

# IMPORTANT ----
#----------------------------------------------------------------------------------------------
# BEFORE STARTING OPERATIONS, CONTROL "setwd" AND "dir_in" IN THIS SCRIPT AND IN "list_file.R".
#----------------------------------------------------------------------------------------------


# -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-



# Preliminary operations ----
#----------------------------

cat(paste0('----------- Preliminary Operations ----------- ', '\n'))

rm(list = ls())

setwd('/home/corrado/Desktop/ISIMIP_nee_reco/')
source('list_file.R')
source('read3DCMCC_output.R')

# Directories containing 3D-CMCC-FEM (dir_in) and ISIMIP Output (dir_out) files
# ***********************************************************
dir_in = '/home/corrado/Desktop/ISIMIP_nee_reco/ISIMIP_OUTPUT_26_ott_2017/'

dir_out = '/home/corrado/Desktop/ISIMIP_nee_reco/Out/'
dir.create(dir_out,showWarnings = F)

# List of variables
# ******************
model_name = '3d-cmcc'

time_list  =  c('daily')

# List of variables to export for ISIMIP

cat(paste0('reading list of variables: ', '\n'))

# annual_list = c(
#   'nee'
# )

daily_list = c(
  'nee',
  'hr'
)

# monthly_list = c(
#   'LAI_PROJ'
# )

cat(paste0('reading list of variables...Ok! ', '\n'))

# Starting Operations  ----
# ***************************

cat(paste0('---------------------------- Starting Operations ---------------------------- ', '\n'))

# For cycle in "annual" and "daily"
for ( cy_t in time_list ) {
  
# Create lf_list dataframe containing path, file name and info about exp, site, esm, rcp, co2, man 
lf_list = list_file(dir_in,cy_t)

 # For cycle in list of saved paths
 for ( cy_p in seq(1,length(lf_list$path_tot)) ) {

   cat(paste0('import file: ',as.character(lf_list$path_tot[cy_p],'\n')))
   
  # Import 3D-CMCC output file in dataframe "Data" as character
  Data = read3DCMCC_output(as.character(lf_list$path_tot[cy_p]))
  
  # Check on ck values imported in the dataframe (if ck = 0, there is an error)
  if ( Data[[2]] == 0 ) {
    next
  }
  
  # Convert data.frame characters into a numeric one
  Data = Data[[1]]
  
  cat(paste0('import file: ',as.character(lf_list$path_tot[cy_p]),' ... OK\n'))

# Calculate multiple variables ----
# **********************************
  
  # cat(paste0('Calculate multiple variables: ', '\n'))
  
  # if (cy_t == 'annual') {
  # # Carbon Mass in Veg Biomass
  # Data$CMVB = Data$STEM_C + Data$MAX_LEAF_C + Data$MAX_FROOT_C + Data$CROOT_C + Data$BRANCH_C + Data$FRUIT_C
  # # Carbon Mass in Wood
  # Data$CMW  = Data$STEM_C + Data$BRANCH_C   + Data$CROOT_C
  # # Carbon Mass in Roots
  # Data$CMR  = Data$MAX_FROOT_C + Data$CROOT_C
  # 
  # cat('\nadd variables: annual\n')
  # }
  
  # if (cy_t == 'daily') {
  #   # Net Prim Prod Aboveground Biomass
  #   Data$NPPAB = Data$dWS  + Data$dWL + Data$dWBB + Data$dFRUIT
  #   # Net Prim Prod Belowground Biomass
  #   Data$NPPBB = Data$dWFR + Data$dWCR
  #   # Root Autotr Respir
  #   Data$RAR   = Data$FRAR + Data$CRAR
  #   
  #   cat('\nadd variables: daily\n')
  # }
  # 
  # cat(paste0('Calculate multiple variables...Ok! ', '\n'))
  
# Import tables from csv files to match 3D-CMCC and ISIMIP names for ISIMIP output to generate ----
# **************************************************************************************************
  
  cat(paste0('Import tables from csv files: ', '\n'))
  
  if ( lf_list$exp[cy_p] == '2A' || lf_list$exp[cy_p] == 'LOCAL' || lf_list$exp[cy_p] == 'FT' ) { 
    
    cat(paste0('experiment: ',lf_list$exp[cy_p],'\n',
               'clim_scenario.csv\n',
               'socio-econ-scenario.csv\n',
               'sens-scenario.csv\n',
               'gcm.csv\n',
               'region.csv\n'))
    
    df_db_rcp = read.csv('clim_scenario.csv')
    df_db_se  = read.csv('socio-econ-scenario.csv')
    df_db_co2 = read.csv('sens-scenario.csv')
    df_db_esm = read.csv('gcm.csv')
    df_db_site = read.csv('region.csv')
    
  } else {
    
    cat(paste0('experiment: ',lf_list$exp[cy_p],'\n',
        'climate_scenario.csv\n',
        'soc-scenario.csv\n',
        'co2sens-scenario.csv\n',
        'gcm.csv\n',
        'region.csv\n'))

    df_db_rcp = read.csv('climate_scenario.csv')
    df_db_se  = read.csv('soc-scenario.csv')
    df_db_co2 = read.csv('co2sens-scenario.csv')
    df_db_esm = read.csv('gcm.csv')
    df_db_site = read.csv('region.csv')
    
  }
  
  cat(paste0('Import tables from csv files...Ok! ', '\n'))
  
# Match 3D-CMCC and ISIMIP name of variables ----
# *******************************************
  cat('find corrispondence\n')

  # gcm
  gcm = as.character(lf_list$esm[cy_p])

  gcm = gsub('ESM','',gcm)

  if ( any(df_db_esm$esm_id == gcm) == 0 ) {
    warning('ESM corrispondence not found\n')
  }
  gcm = as.character(df_db_esm$esm_name[df_db_esm$esm_id == gcm])
  rm(df_db_esm)

  # clim_scenario (rcp)
  rcp = lf_list$rcp[cy_p]
  rcp = as.character(rcp)

  if ( any(df_db_rcp$model_name == rcp) == 0 ) {
    warning('rcp corrispondence not found\n')
  }

  rcp = as.character(df_db_rcp$ISIMIP_name[df_db_rcp$model_name == rcp])
  rm(df_db_rcp)

  # socio_econ_scenario (Management VAR or OFF)
  socio_econ_scen = lf_list$man[cy_p]

  if ( any(df_db_se$model_name == socio_econ_scen) == 0 ) {
    warning('socio_econ_scen corrispondence not found\n')
  }
  
  socio_econ_scen = as.character(df_db_se$ISIMIP_name[df_db_se$model_name == socio_econ_scen])
  rm(df_db_se)

  # sens_scenarios (CO2 ON or VAR)
  sens_scen = lf_list$co2[cy_p]
  sens_scen = as.character(sens_scen)

  if ( any(df_db_co2$model_name == sens_scen) == 0 ) {
    warning('sens_scen corrispondence not found\n')
  }
  
  sens_scen = df_db_co2$ISIMIP_name[df_db_co2$model_name == sens_scen]
  rm(df_db_co2)

  # region (site)
  region = lf_list$site[cy_p]
  region = as.character(region)

  if ( any(df_db_site$model_name == region) == 0 ) {
    warning('region corrispondence not found\n')
  }
  
  region = df_db_site$ISIMIP_name[df_db_site$model_name == region]
  rm(df_db_site)
  
  # timestep (annual or daily)
  timestep = cy_t

  # start_year
  start_year = min(Data$YEAR)
  # end_year
  end_year = max(Data$YEAR)
  
  cat('find corrispondence...Ok!\n')
  
  # Determine the specie (match with csv file)
  cat('find corrispondence SPECIE\n')
  specie = as.character(Data[1,grep('SPECIES',colnames(Data))])
  
  df_db_spe = read.csv('specie.csv')

  if ( any(df_db_spe$model_name == specie) == 0 ) {
    warning('specie corrispondence not found\n')
  }
  
  specie = df_db_spe$ISIMIP_name[df_db_spe$model_name == specie]
  specie = as.character(specie)
  rm(df_db_spe)
  
  cat('find corrispondence SPECIE...Ok!\n')
  
# Select temporary list and import table lists from csv (match) ----
# **************************************************************
  
  cat(paste0('Select temporary list: ', '\n'))
  
  # if (cy_t == 'annual' ) {
  # 
  #   actual_list = annual_list
  #   # Import csv containing 3D-CMCC and ISIMIP name to match
  #   df_db_list  = read.csv('annual_list.csv')
  # }

  if (cy_t == 'daily' ) {

    actual_list = daily_list
    df_db_list  = read.csv('daily_list.csv')
  }

  # if (cy_t == 'monthly' ) {
  # 
  #   actual_list = monthly_list
  #   df_db_list  = read.csv('monthly_list.csv')
  # }

  cat(paste0('Select temporary list...Ok! ', '\n'))
  
# Calculate monthly LAI_PROJ ----
# ***************************
  
  # cat(paste0('Calculate monthly LAI_PROJ: ', '\n'))
  # 
  # if (timestep == 'daily') {
  #   
  # min_year_calc = min(Data$YEAR)
  # max_year_calc = max(Data$YEAR)
  # 
  # output_val = c()
  # year_val   = c()
  # year_vector = c()
  # 
  # for ( i in seq(min_year_calc,max_year_calc) ) {
  # 
  #   for ( m in seq(1:12) ) {
  # 
  #     pos_month  = which(Data$MONTH == m & Data$YEAR == i)
  # 
  #     output_val = c(output_val, mean(Data$LAI_PROJ[pos_month]))
  #     year_val   = c(year_val, i)
  #   }
  #   year_vector  = c(year_vector, year_val)
  #   year_val     = c()
  # }

  # Dataframe to put in write.csv
  # LAI_PROJ_MONTH = data.frame(year_vector, output_val)
  # names(LAI_PROJ_MONTH) <- NULL
  # }
  # 
  # cat(paste0('Calculate monthly LAI_PROJ...Ok! ', '\n'))
  
# For cycle in variable annual and daily list ----
# *****************************************************
  for ( cy_v in actual_list ) {
    
    cat(paste0('\n',cy_v,'\n'))
    df_tmp = data.frame(Data$YEAR, Data[,(colnames(Data) == cy_v)])
    
    
# Unit conversion ----
# *********************************************************************************************
    
    # cat(paste0('Unit conversion: ', '\n'))
    #   
    # # tC/ha --> KgC/m2 (*1000/10000)
    # if ( cy_v == 'CMVB' || cy_v == 'MAX_LEAF_C' || cy_v == 'CMW' || cy_v == 'CMR') {
    #   
    #   df_tmp = data.frame(df_tmp[,1], df_tmp[,2] / 10)
    #   
    # }
    # 
    # # gC/m2/day --> KgC/m2/s
    # if ( cy_v == 'GPP' || cy_v == 'NPP' || cy_v == 'RA' || cy_v == 'RAR' ) {
    #   
    #   df_tmp = data.frame(df_tmp[,1], df_tmp[,2] / (1000 * 86400))
    #   
    # }
    # 
    # # tC/ha --> KgC/m2/s (*1000/10000/86400)
    # if ( cy_v == 'dWL' || cy_v == 'dWFR' || cy_v == 'NPPAB' || cy_v == 'NPPBB' ) {
    #   
    #   df_tmp = data.frame(df_tmp[,1], df_tmp[,2] / (10 * 86400))
    #   
    # }
    # 
    # # °C --> °K (+273.13)
    # if ( cy_v == 'Tsoil' ) {
    #   
    #   df_tmp = data.frame(df_tmp[,1], df_tmp[,2] + 273.13)
    #   
    # }
    # 
    # # mm/mq/day --> Kg/mq/s (/86400)
    # if ( cy_v == 'et' || cy_v == 'C_INT' || cy_v == 'soil_evapo' || cy_v == 'C_TRA' ) {
    #   
    #   df_tmp = data.frame(df_tmp[,1], df_tmp[,2] / 86400)
    #   
    # }
    # 
    # cat(paste0('Unit conversion...Ok!: ', '\n'))
    
# Change to ISIMIP name ----
# **********************
    
    cat(paste0('Change to ISIMIP name: ', '\n'))
    
    cy_v2 = df_db_list$ISIMIP_name[df_db_list$model_name == cy_v]
    cy_v2 = as.character(cy_v2)
    
    # Name of columns in ISIMIP file
    #colnames(df_tmp) = c('YEAR', cy_v2)
    names(df_tmp) <- NULL
    
    # Different names to append to ISIMIP basic name of variables
    str2 = paste0(specie, '_total')
    
    # if (cy_v2 == 'cleaf' || cy_v2 == 'cwood' ||  cy_v2 =='croot' || cy_v2 == 'npp_landleaf' || cy_v2 == 'npp_landroot' || cy_v2 == 'npp_abovegroundwood' || cy_v2 == 'npp_belowgroundwood' ) {
    # str2 = paste0(specie)
    # }
    # if ( cy_v2 == 'evap' ) {
    # str2 = paste0('total')
    # }
    # if ( cy_v2 == 'esoil' || cy_v2 == 'soilmoist' || cy_v2 == 'tsl' ) {
    #   str2 = paste0('')
    # }

    cat(paste0('Change to ISIMIP name...Ok! ', '\n'))
    
# Save ISIMIP output ----
# **********************
    
    cat(paste0('Save ISIMIP output: ', '\n'))
    
    # _______________________
    # if ( cy_v2 == 'esoil' || cy_v2 == 'soilmoist' || cy_v2 == 'tsl' ) {
    #   
    #   cat(paste0('write file: ',dir_out, model_name, '_', gcm, '_', rcp, '_', socio_econ_scen, '_', sens_scen, '_', 
    #              cy_v2, '_', region, '_', timestep, '_', start_year, '_', end_year,'.txt'
    #              ,'\n'))
    #   
    #   
    #   write.table(
    #     df_tmp,
    #     paste0(dir_out, model_name, '_', gcm, '_', rcp, '_', socio_econ_scen, '_', sens_scen, '_', 
    #            cy_v2, '_', region, '_', timestep, '_', start_year, '_', end_year,'.txt'
    #     ),
    #     quote = FALSE,
    #     sep = " ",
    #     row.names = FALSE
    #   )
    #   
    #   
    # } else {
    
    # _______________________
    cat(paste0('write file: ',dir_out, model_name, '_', gcm, '_', rcp, '_', socio_econ_scen, '_', sens_scen, '_', 
                              cy_v2, '_', str2, '_', region, '_', timestep, '_', start_year, '_', end_year,'.txt'
    ,'\n'))
    
    
    write.table(
      df_tmp,
      paste0(dir_out, model_name, '_', gcm, '_', rcp, '_', socio_econ_scen, '_', sens_scen, '_', 
             cy_v2, '_', str2, '_', region, '_', timestep, '_', start_year, '_', end_year,'.txt'
             ),
      quote = FALSE,
      sep = " ",
      row.names = FALSE
    )
    # }
    # if ( timestep == 'daily') {
    #   
    #   # Save file containing LAI_PROJ monthly values calculated ad mean from daily ones
    #   write.table(
    #     LAI_PROJ_MONTH,
    #     paste0(dir_out, model_name, '_', gcm, '_', rcp, '_', socio_econ_scen, '_', sens_scen, '_', 
    #            'lai', '_', specie, '_total', '_', region, '_', 'monthly', '_', start_year, '_', end_year,'.txt'
    #     ),
    #     quote = FALSE,
    #     sep = " ",
    #     row.names = FALSE
    #   )
    #   
    #   
    # }
    
    cat(paste0('write file: ',dir_out, model_name, '_', gcm, '_', rcp, '_', socio_econ_scen, '_', sens_scen, '_', 
               cy_v2, '_', str2, '_', region, '_', timestep, '_', start_year, '_', end_year,'.txt'
               ,' ... OK\n'))

    cat(paste0('Save ISIMIP output...Ok! ', '\n'))
    
  }
  rm(cy_v)
  cat('ciao\n')
 }
 rm(cy_p)
 
}
rm(cy_t)

# Delete elements ----
# ****************

cat(paste0('Delete elements: ', '\n'))

# Delete directories
rm (dir_in, dir_out)

# Delete counter and other variables
rm(str2, cy_v2, i, m, pos_month)

# Delete lists
rm(actual_list, annual_list, daily_list, time_list)

# Delete variables
rm(model_name, gcm, rcp, region, sens_scen, socio_econ_scen, specie, output_val)

# Delete time variables
rm(start_year, end_year, min_year_calc, max_year_calc, year_val, timestep)

# Delete dataframes
rm(df_db_list, df_tmp, LAI_PROJ_MONTH)

cat(paste0('Delete elements...Ok! ', '\n'))
cat(paste0('---------------------------- End of Operations ---------------------------- ', '\n'))
