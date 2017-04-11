#' @export
#' 
#### FUNCTION FOR READING PARAMETERS_FILE 
readParam <- function (paramFile) {
  param_table <- read.table(paramFile,
                            header= T,
                            comment.char = "/")
  return(param_table)
}


