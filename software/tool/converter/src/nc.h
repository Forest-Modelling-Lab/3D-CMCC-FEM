// nc.h
//
// written by A.Ribeca
//
// please note that this file must be included only once!
//

// <modelname>_<obs>_<clim_scenario>_<socio-econ-scenario>_<sens-scenarios>_<variable>_<region>_<timestep>_<start-year>_<end-year>.nc4
int nc_conv(dataset_t* dataset, const char*const folder)
{
#define BUF_SIZE			(256)	// should be enough
#define NC_MISSING_VALUE	(1.e+20f)
	char* sitename;
	char buf[BUF_SIZE];
	int i;
	int ret;
	double* index;
	int site_index;

	const size_t latlon_index[] = { 0 };

	const char modelname[] = "3D-CMCC-CNR-BGC";

	struct {
		char* model_name;
		char* isimip_name;
		double lat;
		double lon;
	} sites [] = {
		{ "Bily_Kriz", "bily_kriz", 49.3, 18.32 }
		, { "Collelongo", "collelongo", 41.85, 13.59 }
		, { "Hyytiala", "hyytiala", 61.84, 24.29 }
		, { "Kroof", "kroof", 48.25, 11.10 }
		, { "LeBray", "le_bray", 44.72, -0.77 }
		, { "Peitz", "peitz", 51.92, 14.35 }
		, { "Solling_beech", "solling_beech", 51.77, 9.57 }
		, { "Solling_spruce", "solling_spruce", 51.76, 9.58 }
		, { "Soroe", "soro", 55.29, 11.38 }
	};

	const char *gcms[] = // is ecm for model
	{
		"hadgem2-es"
		, "ipsl-cm5a-lr"
		, "miroc-esm-chem"
		, "gfdl-esm2m"
		, "noresm1-m"
		, "miroc5"
		, "princeton"
		, "gswp3"
		, "watch"
		, "watch-wfdei.gpcc"
		, "localclim"
	};

	const char socio_econ_scenario[] = "man";

	ret = 0; // default to err
	index = NULL;
	
	assert(dataset);

	index = malloc(dataset->rows_count*sizeof*index);
	if ( ! index )
	{
		puts(err_out_of_memory);
		return 0;
	}

	if ( ANNUAL_DATASET_TYPE == dataset->type )
	{
		for ( i = 0; i < dataset->rows_count; ++i )
		{
			index[i] = dataset->start_year + i;
		}
	}
	// DAILY_DATASET_TYPE
	else
	{
		int day;
		int year;

		day = 0;
		year = dataset->start_year;
		for ( i = 0; i < dataset->rows_count; ++i )
		{
			if ( ++day > (IS_LEAP_YEAR(year) ? 366 : 365) )
			{
				day = 1;
				++year;
			}
			index[i] = day;
		}
	}

	// get sitename
	sitename = dataset->sitename;
	for ( i = 0; i < SIZEOF_ARRAY(sites); ++i )
	{
		if ( ! string_compare_i(sitename, sites[i].model_name) )
		{
			sitename = sites[i].isimip_name;
			site_index = i;
			break;
		}
	}
	assert(site_index < SIZEOF_ARRAY(sites));

	for ( i = 0; i < dataset->columns_count; ++i )
	{
		const char* var;
		int j;
		int nc_id, lon_dim_id, lat_dim_id, time_dim_id;
		int lon_id, lat_id, time_id, var_id;
		int var_dim_ids[3];

		// get var name
		var = (ANNUAL_DATASET_TYPE == dataset->type) ? annual_vars[i] : daily_vars[i];

		// compute filename
		if ( folder )
		{
			sprintf(buf, "%s/%s_%s_%s_%s.nc4", folder
							, modelname, gcms[dataset->esm-1], var
							, (ANNUAL_DATASET_TYPE == dataset->type) ? "annual" : "daily"
			);
		}
		else
		{
			sprintf(buf, "%s%s_%s_%s_%s.nc4", dataset->path ? dataset->path : ""
							, modelname, gcms[dataset->esm-1], var
							, (ANNUAL_DATASET_TYPE == dataset->type) ? "annual" : "daily"
			); 
		}
		
		// convert INVALID_VALUE TO NC_MISSING_VALUE
		for ( j = 0; j < dataset->rows_count; ++j )
		{
			if ( IS_INVALID_VALUE(dataset->vars[i][j]) )
			{
				dataset->vars[i][j] = NC_MISSING_VALUE;
			}
		}

		printf("- creating %s...", buf);

		// create nc file
		if ( (ret = nc_create(buf, NC_CLOBBER, &nc_id)) )
			goto err;

		// define dims
		if ( (ret = nc_def_dim(nc_id, "lat", 1, &lat_dim_id)) )
			goto err;
		if ( (ret = nc_def_dim(nc_id, "lon", 1, &lon_dim_id)) )
			goto err;
		if ( (ret = nc_def_dim(nc_id, "time", dataset->rows_count, &time_dim_id)) )
			goto err;
		
		// define vars
		if ( (ret = nc_def_var(nc_id, "lat", NC_DOUBLE, 1, &lat_dim_id, &lat_id)) )
			goto err;
		if ( (ret = nc_def_var(nc_id, "lon", NC_DOUBLE, 1, &lon_dim_id, &lon_id)) )
			goto err;
		if ( (ret = nc_def_var(nc_id, "time", NC_DOUBLE, 1, &time_dim_id, &time_id)) )
			goto err;

		var_dim_ids[0] = time_dim_id; // time goes first
		var_dim_ids[1] = lat_dim_id;
		var_dim_ids[2] = lon_dim_id;
		if ( (ret = nc_def_var(nc_id, var, NC_DOUBLE, 3, var_dim_ids, &var_id)) )
			goto err;

		// end "define" mode
		if ( (ret = nc_enddef(nc_id)) )
			goto err;

		// put data
		if ( (ret = nc_put_var1_double(nc_id, lat_id, latlon_index, &sites[site_index].lat)) )
			goto err;
		if ( (ret = nc_put_var1_double(nc_id, lon_id, latlon_index, &sites[site_index].lon)) )
			goto err;
		if ( (ret = nc_put_var_double(nc_id, time_id, index)) )
			goto err;
		if ( (ret = nc_put_var_double(nc_id, var_id, dataset->vars[i])) )
			goto err;

		// close file
		if ( (ret = nc_close(nc_id)) )
			goto err;

		puts("ok");
	}
	if ( index ) free(index);
	return 1;

err:
	printf("error - %s\n", nc_strerror(ret));
	if ( index ) free(index);
	return 0;
#undef NC_MISSING_VALUE
#undef BUF_SIZE
}
