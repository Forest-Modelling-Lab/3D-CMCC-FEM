plot_ts <- function(dt,yvar_name = NULL,xvar_name = NULL,ribbon = 'N') {
  # plot di una o più valori
  # INPUT
  #   df: dataframe che contiene le seguenti colonne:
  #     [,1]: valori per asse X
  #     [,2]: valori per l'asse Y
  #     [,3]: valori per la legenda
  #   se ribbon == 'Y'
  #     [,4]: valori massimi per il ribbon
  #     [,5]: valori minimi per il ribbon
  #   yvar_name: nome per l'asse Y
  # OUTPUT
  #   plot1: ggplot di df
  
  if (is.null(yvar_name)) {
    yvar_name = colnames(dt)[2]
  }
  
  if (is.null(xvar_name)) {
    xvar_name = colnames(dt)[1]
  } 

  colnames(dt)[1] = 'var_x'
  colnames(dt)[2] = 'var_y'
  colnames(dt)[3] = 'var_color'
  
  plot1 = ggplot(dt)
  
  if (ribbon == 'Y') {
    colnames(dt)[4] = 'max_y'
    colnames(dt)[5] = 'min_y'
    plot1 = plot1 +
      geom_ribbon(data = dt, aes(x = var_x,ymax = max_y,ymin = min_y, 
                      color = var_color,fill = var_color),alpha = 0.4)
  }
  
  plot1 = plot1 +
    geom_line(aes(x = var_x, y = var_y, color = var_color)) +
    theme(legend.title = element_blank())
  
  if ( (yvar_name == 'None') ) {
    plot1 = plot1 + theme(axis.title.y = element_blank())
  } else {
    plot1 = plot1 + ylab(yvar_name)    
  }
  if ((xvar_name == 'None')) {
    plot1 = plot1 + theme(axis.title.x = element_blank())
  } else {
    plot1 = plot1 + xlab(xvar_name)
  }

  return(plot1)
}