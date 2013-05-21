setMethod("howmany", signature(x="memuse"),
  function(x, nrow, ncol, out.type="full", ..., type="double", intsize=4)
  {
    # Manage input arguments
    out.type <- match.arg(arg=tolower(out.type), choices=c("full", "approximate"))
    type <- match.arg(arg=tolower(type), choices=c("double", "integer"))
    
    if (type == "double")
      bytes <- 8
    else if (type == "integer")
      bytes <- intsize
    
    # Get the size
    size <- convert_to_bytes(x)@size
    
    if (!missing(nrow)){
      if (!is.int(nrow))
        stop("argument 'nrow' must be an integer")
      else if (!missing(ncol))
        stop("you should supply at most one of 'nrow' and 'ncol'.  Perhaps you meant to use howbig()?")
      else
        ncol <- floor(size/(nrow*bytes))
    }
    else if (!missing(ncol)){
      if (!is.int(ncol))
        stop("argument 'ncol' must be an integer")
      nrow <- floor(size/(ncol*bytes))
    }
    else
      nrow <- ncol <- floor(sqrt(size/bytes))
    
    # Return
    ret <- c(nrow, ncol)
    
    if (out.type == "approximate")
      ret <- approx_size(ret)
    
    return( ret )
  }
)

