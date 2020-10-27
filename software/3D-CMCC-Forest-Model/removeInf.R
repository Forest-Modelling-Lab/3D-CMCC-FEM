# rm(list = ls())
# source('fluxnetTo3DCMCC.R')
# file_name_model = 'c:/Users/CMCC/Desktop/da_fluxnet_release_2015/FLX_CZ-BK1_FLUXNET2015_FULLSET_2004-2008_2-3_TIER_1/out2/FLX_CZ-BK1_FLUXNET2015_FULLSET_HH_2004-2008_2-3.csv'
# file_name_fluxnet = 'c:/Users/CMCC/Desktop/da_fluxnet_release_2015/FLX_CZ-BK1_FLUXNET2015_FULLSET_2004-2008_2-3_TIER_1/FLX_CZ-BK1_FLUXNET2015_FULLSET_HH_2004-2008_2-3.csv'
# fluxnetTo3DCMCC(file_name_fluxnet,file_name_model)
removeInf <- function (df) {
  # function to remove -Inf and Inf values from a dataframe
  
  for (cy_c in colnames(df) ) {
    pos_inf = which(df[,cy_c] == Inf)
    if ( length(pos_inf) > 0 ) {
      df[pos_inf,cy_c] = NA
    }
    rm(pos_inf)
    pos_inf = which(df[,cy_c] == -Inf)
    if ( length(pos_inf) > 0 ) {
      df[pos_inf,cy_c] = NA
    }
    rm(pos_inf)
  }
  return(df)
}