/* netcdf_stuff.h */
#ifndef NETCDF_STUFF_H
#define NETCDF_STUFF_H

/* */
#include "types.h"

/* */
int netcdf_create_from_yos(const YOS *const yos, const int yos_count);
const char *netcdf_err(void);
const char *netcdf_get_version(void);

/* */
#endif /* NETCDF_STUFF_H */
