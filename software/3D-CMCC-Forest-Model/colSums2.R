colSums2 <- function(df) {
  # calcolo la somma di ogni colonna del df
  # INPUT
  # df: data.frame  dei valori da sommare
  # OUTPUT
  # df_sum: dataframe con le somme dei valori di ogni colonna
  # ESEMPIO:
  # df = data.frame(
  #   'col1' = c(1,2,3,4),
  #   'col2' = c(NA,3,2,1),
  #   'col3' = c(NA,NA,NA,NA)
  # )
  df_sum = df[1,]

  for (cy_c in seq(1,length(df)) ) {
    if ( sum(is.na(df[,cy_c])) == length(df[,cy_c]) ) {
      df_sum[1,cy_c] = NA
    } else {
      df_sum[1,cy_c] = sum(df[,cy_c],na.rm = T)
    }
  }
  
  return(df_sum)
}