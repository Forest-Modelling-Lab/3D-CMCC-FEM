/* nc.c */
#include "netcdf.h"

const char *netcdf_get_version(void) {
        return nc_inq_libvers();
	//return "NA";//return nc_inq_libvers();   //if ncdf lib is not used
}
