
/* nc.c */
#ifdef NC_USE
#include "netcdf.h"
#endif

const char *netcdf_get_version(void) {
#ifdef NC_USE
        return nc_inq_libvers();
#else
	return "NA";//return nc_inq_libvers();   //if ncdf lib is not used
#endif
}

