setMethod("swap.prefix", signature(x="memuse"),
  function(x)
  {
    if (x@unit.prefix == "IEC")
      new.prefix <- "SI"
    else if (x@unit.prefix == "SI")
      new.prefix <- "IEC"
    else
      mu_error()
    
    power <- get.power(x)
    
    if (new.prefix == "IEC")
      x@size <- x@size * (1e3/1024)^power
    else
      x@size <- x@size * (1024/1e3)^power
    
    x@unit.prefix <- new.prefix
    
    x <- check.unit(x)
    
    return( x )
  }
)



setMethod("swap.names", signature(x="memuse"),
  function(x)
  {
    if (x@unit.names == "short")
      new.names <- "long"
    else if (x@unit.names == "long")
      new.names <- "short"
    else
      mu_error()
    
    new.unit <- which(.units[[x@unit.names]][[x@unit.prefix]][["print"]] == x@unit)
    
    x@unit <- .units[[new.names]][[x@unit.prefix]][["print"]][new.unit]
    
    x@unit.names <- new.names
    
    return( x )
  }
)



setMethod("swap.unit", signature(x="memuse"),
  function(x, unit)#, precedence=.PRECEDENCE)
  {
    unit <- tolower(unit)
    
    if (unit==x@unit)
      return(x)
    
    if (unit == "best")
      x <- best.unit(x)
    else if (unit == tolower(x@unit))
      return( check.mu(x) )
    else {
      flag <- FALSE
      
      for (names in c("short", "long")){
        for (prefix in c("IEC", "SI")){
          if ( unit %in% .units[[names]][[prefix]][["check"]] ){
            flag <- TRUE
            unit.names <- names
            unit.prefix <- prefix
            break
          }
          if (flag)
            break
        }
      }
      
      if (flag){
        x <- convert.to.bytes(x)
        
        x@unit.names <- unit.names
        x@unit.prefix <- unit.prefix
        
        if (unit.prefix == "IEC")
          f <- 1024
        else
          f <- 1e3
        
        units <- .units[[x@unit.names]][[x@unit.prefix]][["check"]]
        i <- which(units == unit)
        
        x@size <- x@size/(f^(i-1))
        
        new.unit <- .units[[unit.names]][[unit.prefix]][["print"]][i]
        
        x@unit <- new.unit
      }
      else
        stop("invalid argument 'unit'.  See help('memuse')")
    }
    
    return( x )
  }
)


