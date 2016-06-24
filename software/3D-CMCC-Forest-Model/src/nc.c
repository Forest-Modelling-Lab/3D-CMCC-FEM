/* nc.c */
#include "netcdf.h"

const char *netcdf_get_version(void) {
	return nc_inq_libvers();
}
