plot_multi_lm <- function(dt,yvar_name = NULL,xvar_name = NULL) {
  # plot delle regressioni lineari tra pi? variabili
  # INPUT
  #   df: dataframe che contiene le seguenti colonne:
  #     [,1]: valori per asse X (reference)
  #     [,2]: valori per l'asse Y (da confrontare)
  #     [,3]: valori per la legenda
  # yvar_name: nome per l'asse Y
  # OUTPUT
  #   plot1: lista con il plot delle regressioni e la tabella dei valori
  
  library(gridExtra)
  library(ggplot2)
  library(cowplot)
  
  # elenco delle variabili da confrontare
  if (is.null(yvar_name)) {
    yvar_name = sort(unique(as.character(dt[,3])))
  } 
  
  if (is.null(xvar_name)) {
    xvar_name = 'Reference'
  } 
  
  colnames(dt)[1] = 'ref'
  colnames(dt)[2] = 'cmp'
  colnames(dt)[3] = 'cmp_name'
  
  # regressione lineare per stessa variabile ma diverse combinazioni
  if( exists('df_lm_t')) {
    rm(df_lm_t)
    rm(df_lm_t_table)
  }
  
  yvar_name2 = sort(unique(as.character(dt[,3])))
  
  for (cy_var in yvar_name2) {
    
    # estraggo le righe
    pos_row = which(as.character(dt[,3]) == cy_var)
    df_lm = dt[pos_row,]
    
    # sincronizzo togliendo i NA
    pos_na = which(is.na(df_lm$ref) == 1)
    if ( length(pos_na) > 0 ) {
      df_lm = df_lm[-1*pos_na,]
    }
    rm(pos_na)
    
    if ( length(df_lm[,1]) == 0 ) {
      next
    }
    pos_na = which(is.na(df_lm$cmp) == 1)
    if ( length(pos_na) > 0 ) {
      df_lm = df_lm[-1*pos_na,]
    }
    rm(pos_na)
    
    if ( length(df_lm[,1]) == 0 ) {
      next
    }
    
    linear_md = lm(cmp ~ ref, data = df_lm)
    df_lm$fitted = linear_md$fitted.values
    slope = coef(linear_md)[2]
    intercp = coef(linear_md)[1]
    r_square = cor(df_lm$fitted,df_lm$cmp)

    if( exists('df_lm_t')) {
      
      df_lm_t = rbind(df_lm_t,df_lm)
      df_lm_t_table = rbind(df_lm_t_table,
                            data.frame(
                              'dataset' = cy_var,
                              'Nr' = sprintf('%d', length(df_lm$cmp)),
                              'slope' = sprintf('%.2f',slope),
                              'intercept' = sprintf('%.2f',intercp),
                              'r' = sprintf('%.2f', r_square),
                              'mean Y' = sprintf('%.2f', mean(df_lm$cmp)),
                              'sd Y' = sprintf('%.2f', sd(df_lm$cmp)),
                              'mean X' = sprintf('%.2f', mean(df_lm$ref)),
                              'sd X' = sprintf('%.2f', sd(df_lm$ref)),
                              'RMSE' = sprintf('%.2f', sqrt(mean((df_lm$ref - df_lm$cmp)^2)))
                            )
      )
    } else {
      
      df_lm_t = df_lm
      df_lm_t_table = data.frame(
          'dataset' = cy_var,
          'Nr' = sprintf('%d', length(df_lm$cmp)),
          'slope' = sprintf('%.2f',slope),
          'intercept' = sprintf('%.2f',intercp),
          'r' = sprintf('%.2f', r_square),
          'mean Y' = sprintf('%.2f', mean(df_lm$cmp)),
          'sd Y' = sprintf('%.2f', sd(df_lm$cmp)),
          'mean X' = sprintf('%.2f', mean(df_lm$ref)),
          'sd X' = sprintf('%.2f', sd(df_lm$ref)),
          'RMSE' = sprintf('%.2f', sqrt(mean((df_lm$ref - df_lm$cmp)^2)))
      )
    }
  }
  
  ck_empty = 0
  
  if( exists('df_lm_t') == 0) {
    df_lm_t = data.frame(
      'ref' = NA,
      'cmp' = NA,
      'cmp_name' = 'empty',
      'fitted' = NA
    )
    df_lm_t_table = data.frame(
      'dataset' = cy_var,
      'Nr' = NA,
      'slope' = NA,
      'intercept' = NA,
      'r' = NA,
      'mean Y' = NA,
      'sd Y' = NA,
      'mean X' = NA,
      'sd X' = NA,
      'RMSE' = NA
    )
    ck_empty = 1
  } 
  
  if (ck_empty == 1) {
    mp1 = ggplot() +
      geom_text(data = df_lm_t,aes(x = ref, y = cmp,label = 'dataset empty')) +
      theme(axis.text = element_blank())
  } else {
    mp1 = ggplot() +
      # geom_line(data = df_lm_t,aes(x = X, y = fitted2,color = color_var)) +
      geom_point(data = df_lm_t,aes(x = ref, y = cmp,color = cmp_name)) +
      geom_line(data = df_lm_t,aes(x = ref, y = fitted,color = cmp_name)) +
      theme(legend.position = 'top',legend.title = element_blank())
  }
  
  if ( length(which(xvar_name == 'None')) > 0 ) {
    mp1 = mp1 + theme(axis.title.x = element_blank())
  } else {
    mp1 = mp1 + xlab(xvar_name)
  }
  if ( length(which(yvar_name == 'None')) > 0 ) {
    mp1 = mp1 + theme(axis.title.y = element_blank())
  } else {
    mp1 = mp1 + ylab(yvar_name)
  } 
  
  mp2 = tableGrob(df_lm_t_table)
  plot_lm_t = list(mp1,mp2)
    
  return(plot_lm_t)
  
}

