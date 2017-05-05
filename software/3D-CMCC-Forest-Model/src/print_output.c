/* cumulative_balance.c */
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "print_output.h"
#include "common.h"
#include "settings.h"
#include "soil_settings.h"
#include "logger.h"
#include "g-function.h"
#include "nc.h"

extern settings_t* g_settings;
extern logger_t* g_daily_log;
extern logger_t* g_monthly_log;
extern logger_t* g_annual_log;
extern logger_t* g_daily_soil_log;
extern logger_t* g_monthly_soil_log;
extern logger_t* g_annual_soil_log;
extern char *g_sz_dataset_file;
extern char *g_sz_soil_file;
extern char *g_sz_input_met_file;
extern char *g_sz_settings_file;
extern char *g_sz_topo_file;
extern char *g_sz_co2_conc_file;

extern const char sz_launched[];

extern int MonthLength [];
extern int MonthLength_Leap [];

static const char sz_management[] = "TCN";

static const char* get_filename(const char *const s)
{
	const char *p;
	const char *p2;

	p = NULL;

	if ( s ) {
		p = strrchr(s, '/');
		if ( p ) ++p;
		p2 = strrchr(s, '\\');
		if ( p2 ) ++p2;
		if ( p2 > p ) p = p2;
		if ( ! p ) p = s;
	}

	return p;
}

void print_model_paths(logger_t *const _log)
{
	//assert(_log);

	logger(_log, "\n\n#site: %s\n", g_settings->sitename);
	if ( g_sz_dataset_file )
		logger(_log, "#--input files--\n");
	logger(_log, "#input file = %s\n", get_filename(g_sz_dataset_file));
	logger(_log, "#soil file = %s\n", get_filename(g_sz_soil_file));
	logger(_log, "#topo file = %s\n", get_filename(g_sz_topo_file));
	logger(_log, "#met file = %s\n", get_filename(g_sz_input_met_file));
	logger(_log, "#settings file = %s\n", get_filename(g_sz_settings_file));
	if ( g_settings->CO2_trans )
		logger(_log, "#CO2 file = %s\n", get_filename(g_sz_co2_conc_file));
}

void print_model_settings(logger_t*const log)
{
	//assert(log);

	logger(log, "#--model settings--\n");
	logger(log, "#CO2_mod = %s\n", g_settings->CO2_mod ? "on" : "off");
	logger(log, "#CO2 trans = %s\n", (CO2_TRANS_VAR == g_settings->CO2_trans) ? "var" : (CO2_TRANS_ON == g_settings->CO2_trans) ? "on" : "off");
	if ( CO2_TRANS_OFF == g_settings->CO2_trans )
	{
		logger(log, "#fixed co2 concentration = %g ppmv\n", g_settings->co2Conc);
	}
	else if ( CO2_TRANS_VAR == g_settings->CO2_trans )
	{
		logger(log, "#year %d at which co2 concentration is fixed at value = %g ppmv\n", g_settings->year_start_co2_fixed, g_settings->co2Conc);
	}
	logger(log, "#Resp accl = %s\n", g_settings->Resp_accl ? "on" : "off");
	logger(log, "#regeneration = %s\n", g_settings->regeneration ? "on" : "off");
	logger(log, "#Management = %s\n", g_settings->management ? "on" : "off");
	if ( g_settings->management )
	{
		logger(log, "#Year Start Management = %d\n", g_settings->year_start_management);
	}
	if ( g_settings->year_restart != -1 )
	{
		logger(log, "#Year restart = %d\n", g_settings->year_restart);
	}
	else
	{
		logger(log, "#Year restart = off\n");
	}
}

void EOD_print_output_cell_level(cell_t *const c, const int day, const int month, const int year, const int years_of_simulation )
{
	int layer;
	int height;
	int dbh;
	int age;
	int species;

	static int years_counter;

	species_t *s;

	/* return if daily logging is off*/
	if ( ! g_daily_log ) return;

	/* heading */
	if ( !day && !month && !year )
	{
		logger(g_daily_log, "YEAR,MONTH,DAY");

		for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
		{
			for ( height = c->heights_count - 1; height >= 0 ; --height )
			{
				if( layer == c->heights[height].height_z )
				{
					/* heading for layers */
					logger(g_daily_log, ",LAYER");

					/* heading for heights value */
					logger(g_daily_log,",HEIGHT");

					for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
					{
						logger(g_daily_log,",DBH");

						for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
						{
							/* heading for ages */
							logger(g_daily_log,",AGE");

							for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
							{
								/* heading for species name */
								logger(g_daily_log,",SPECIES");

								/* heading for management */
								logger(g_daily_log, ",MANAGEMENT");

								logger(g_daily_log,
										",GPP"
										",RG"
										",RM"
										",RA"
										",NPP"
										",CUE"
										",LAI_PROJ"
										",PEAK-LAI_PROJ"
										",CC_P"
										",CC_E"
										",DBHDC"
										",CROWN_AREA_PROJ"
										",CROWN_AREA_EXP"
										",APAR"
										",Ntree"
										",VEG_D"
										",C_INT"
										",C_WAT"
										",C_EVA"
										",C_TRA"
										",C_ET"
										",C_LE"
										",WUE"
										",WRes"
										",WS"
										",WSsap"
										",WSL"
										",WSD"
										",WL"
										",WFR"
										",WCR"
										",WCRsap"
										",WCRL"
										",WCRD"
										",WBB"
										",WBBsap"
										",WBBL"
										",WBBD"
										",WFru"
										",dWRes"
										",dWS"
										",dWL"
										",dWFR"
										",dWCR"
										",dWBB"
										",dFRUIT"
										",SAR"
										",LAR"
										",FRAR"
										",CRAR"
										",BBAR"
										",FCO2"
										",FCO2_TR"
										",FAGE"
										",FT"
										",FVPD"
										",FN"
										",FSW"
										",LITR_C"
										",CWD_C"
										",SOIL_C"
								);

							}
							if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) {
								logger(g_daily_log,",*");
							}
						}
						if ( c->heights[height].dbhs[dbh].ages_count > 1 ) {
							logger(g_daily_log, ",**");
						}
					}
					if ( c->heights[height].dbhs_count > 1 ) {
						logger(g_daily_log,",***");
					}
				}
				if ( c->tree_layers[layer].layer_n_height_class > 1 ) {
					logger(g_daily_log,",****");
				}
			}
			if ( c->tree_layers_count > 1 ) {
				logger(g_daily_log,",*****");
			}
			/*
							if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) logger(g_daily_log,",*");
						}
						if ( c->heights[height].dbhs[dbh].ages_count > 1 ) logger(g_daily_log,",**");
					}
					if ( c->heights[height].dbhs_count > 1 ) logger(g_daily_log,",***");
				}
				if ( c->tree_layers[layer].layer_n_height_class > 1 ) logger(g_daily_log,",****");
			}
			if ( c->tree_layers_count > 1 ) logger(g_daily_log,",*****");
			 */
		}
		/************************************************************************/
		/* heading variables only at cell level */
		logger(g_daily_log,",gpp,npp,ar,et,le,soil_evapo,snow_pack,asw,iWue,litrC,cwdC,soilC,litrN,soilN\n");
	}
	/*****************************************************************************************************/

	/* values */
	logger(g_daily_log, "%d,%d,%d", c->years[year].year, month + 1, day + 1);

	/* print class level LAI values */
	for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
	{
		for ( height = c->heights_count - 1; height >= 0 ; --height )
		{
			if( layer == c->heights[height].height_z )
			{
				/* print layer */
				logger(g_daily_log,",%d", layer);

				/* print height */
				logger(g_daily_log,",%g", c->heights[height].value);

				for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
				{
					/* print dbh */
					logger(g_daily_log,",%g", c->heights[height].dbhs[dbh].value);

					for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
					{
						/* print age */
						logger(g_daily_log,",%d", c->heights[height].dbhs[dbh].ages[age].value);

						for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
						{
							s  = &c->heights[height].dbhs[dbh].ages[age].species[species];

							/* print species name */
							logger(g_daily_log,",%.3s", c->heights[height].dbhs[dbh].ages[age].species[species].name);

							/* print management */
							logger(g_daily_log,",%c", sz_management[c->heights[height].dbhs[dbh].ages[age].species[species].management]);

							/* print variables at layer-class level */
							logger(g_daily_log,",%6.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,"
									"%d,%d,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f"
									",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f"
									",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f"
									",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f",
									s->value[GPP],
									s->value[TOTAL_GROWTH_RESP],
									s->value[TOTAL_MAINT_RESP],
									s->value[TOTAL_AUT_RESP],
									s->value[NPP],
									s->value[CUE],
									s->value[LAI_PROJ],
									s->value[PEAK_LAI_PROJ],
									s->value[DAILY_CANOPY_COVER_PROJ],
									s->value[DAILY_CANOPY_COVER_EXP],
									s->value[DBHDC_EFF],
									s->value[CROWN_AREA_PROJ],
									s->value[CROWN_AREA_EXP],
									s->value[APAR],
									s->counter[N_TREE],
									s->counter[VEG_DAYS],
									s->value[CANOPY_INT],
									s->value[CANOPY_WATER],
									s->value[CANOPY_EVAPO],
									s->value[CANOPY_TRANSP],
									s->value[CANOPY_EVAPO_TRANSP],
									s->value[CANOPY_LATENT_HEAT],
									s->value[WUE],
									s->value[RESERVE_C],
									s->value[STEM_C],
									s->value[STEM_SAPWOOD_C],
									s->value[STEM_LIVE_WOOD_C],
									s->value[STEM_DEAD_WOOD_C],
									s->value[LEAF_C],
									s->value[FROOT_C],
									s->value[CROOT_C],
									s->value[CROOT_SAPWOOD_C],
									s->value[CROOT_LIVE_WOOD_C],
									s->value[CROOT_DEAD_WOOD_C],
									s->value[BRANCH_C],
									s->value[BRANCH_SAPWOOD_C],
									s->value[BRANCH_LIVE_WOOD_C],
									s->value[BRANCH_DEAD_WOOD_C],
									s->value[FRUIT_C],
									s->value[C_TO_RESERVE],
									s->value[C_TO_STEM],
									s->value[C_TO_LEAF],
									s->value[C_TO_FROOT],
									s->value[C_TO_CROOT],
									s->value[C_TO_BRANCH],
									s->value[C_TO_FRUIT],
									s->value[STEM_AUT_RESP],
									s->value[LEAF_AUT_RESP],
									s->value[FROOT_AUT_RESP],
									s->value[CROOT_AUT_RESP],
									s->value[BRANCH_AUT_RESP],
									s->value[F_CO2],
									s->value[F_CO2_TR],
									s->value[F_AGE],
									s->value[F_T],
									s->value[F_VPD],
									s->value[F_NUTR],
									s->value[F_SW],
									s->value[LITR_C],
									s->value[CWD_C],
									s->value[SOIL_C]
							);
						}

						if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) {
							logger(g_daily_log,",*");
						}
					}
					if ( c->heights[height].dbhs[dbh].ages_count > 1 ) {
						logger(g_daily_log,",**");
					}
				}
				if ( c->heights[height].dbhs_count > 1 ) {
					logger(g_daily_log,",***");
				}
			}
			if ( c->tree_layers[layer].layer_n_height_class > 1 ) {
				logger(g_daily_log,",****");
			}
		}
		if ( c->tree_layers_count > 1 ) {
			logger(g_daily_log,",*****");
		}
	}
	/************************************************************************/
	/* printing variables only at cell level */

	logger(g_daily_log, ",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f\n",
			c->daily_gpp,
			c->daily_npp,
			c->daily_aut_resp,
			c->daily_et,
			c->daily_lh_flux,
			c->daily_soil_evapo,
			c->snow_pack,
			c->asw,
			c->daily_iwue,
			c->litrC,
			c->cwdC,
			c->soilC,
			c->litrN,
			c->soilN
	);
	/************************************************************************/

	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365 ) )
	{
		++years_counter;

		if ( years_counter ==  years_of_simulation)
		{
			logger(g_daily_log, sz_launched, netcdf_get_version(), datetime_current());
			print_model_paths(g_daily_log);
			//const char* p;
			//p = file_get_name_only(g_daily_log->filename);
			logger(g_daily_log, "#output file = %s\n", g_daily_log->filename);
			print_model_settings(g_daily_log);
		}
	}
}

void EOM_print_output_cell_level(cell_t *const c, const int month, const int year, const int years_of_simulation )
{
	int layer;
	int height;
	int dbh;
	int age;
	int species;

	static int years_counter;

	species_t *s;

	/* return if monthly logging is off*/
	if ( ! g_monthly_log ) return;

	/* heading */
	if ( !month && !year )
	{
		logger(g_monthly_log, "YEAR,MONTH");

		for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
		{
			for ( height = c->heights_count - 1; height >= 0 ; --height )
			{
				if( layer == c->heights[height].height_z )
				{
					/* heading for layers */
					logger(g_monthly_log,",LAYER");

					/* heading for heights value */
					logger(g_monthly_log,",HEIGHT");

					for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
					{
						/* heading for dbhs value */
						logger(g_monthly_log, ",DBH");

						for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
						{
							/* heading for ages */
							logger(g_monthly_log,",AGE");

							for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
							{
								/* heading for species name */
								logger(g_monthly_log,",SPECIES");

								/* heading for management */
								logger(g_monthly_log,",MANAGEMENT");

								logger(g_monthly_log,
										",GPP"
										",RA"
										",NPP"
										",CUE"
										",CTRANSP"
										",CET"
										",CLE"
										",CC"
										",DBHDC"
										",HD_EFF"
										",HDMAX"
										",HDMIN"
										",N_TREE"
										",WUE"
										",WRes"
										",WS"
										",WSL"
										",WSD"
										",PWL"
										",PWFR"
										",WCR"
										",WCRL"
										",WCRD"
										",WBB"
										",WBBL"
										",WBBD"
								);

							}
							if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) {
								logger(g_monthly_log,",*");
							}
						}
						if ( c->heights[height].dbhs[dbh].ages_count > 1 ) {
							logger(g_monthly_log,",**");
						}
					}
					if ( c->heights[height].dbhs_count > 1 ) {
						logger(g_monthly_log,",***");
					}
				}
				if ( c->tree_layers[layer].layer_n_height_class > 1 ) {
					logger(g_monthly_log,",****");
				}
			}
			if ( c->tree_layers_count > 1 ) {
				logger(g_monthly_log,",*****");
			}
		}
		/************************************************************************/
		/* heading variables at cell level only if there's more than one layer */

		if( c->heights_count > 1 )
		{
			logger(g_monthly_log,",***,gpp,npp,ar");
		}
		/* heading variables at cell level also if there's more than one layer */
		else
		{
			logger(g_monthly_log,",*****");
		}
		/* heading variables only at cell level */
		logger(g_monthly_log,",et,le,asw,iWue\n");
	}

	/************************************************************************/
	/* values */
	logger ( g_monthly_log, "%d,%d", c->years[year].year, month +1 );

	/* print class level values */
	for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
	{
		for ( height = c->heights_count - 1; height >= 0 ; --height )
		{
			if( layer == c->heights[height].height_z )
			{
				/* print layer */
				logger(g_monthly_log,",%d", layer);

				/* print height */
				logger(g_monthly_log,",%g", c->heights[height].value);

				for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
				{
					/* print age */
					logger(g_monthly_log,",%g", c->heights[height].dbhs[dbh].value);

					for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
					{
						/* print age */
						logger(g_monthly_log,",%d", c->heights[height].dbhs[dbh].ages[age].value);

						for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
						{
							s  = &c->heights[height].dbhs[dbh].ages[age].species[species];

							/* print species name */
							logger(g_monthly_log,",%.3s", c->heights[height].dbhs[dbh].ages[age].species[species].name);

							/* print management */
							logger(g_monthly_log,",%c", sz_management[c->heights[height].dbhs[dbh].ages[age].species[species].management]);

							/* print variables at layer-class level */
							logger(g_monthly_log,",%6.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%d,%3.4f,%3.4f,%3.4f,%3.4f"
									",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f",
									s->value[MONTHLY_GPP],
									s->value[MONTHLY_TOTAL_AUT_RESP],
									s->value[MONTHLY_NPP],
									s->value[MONTHLY_CUE],
									s->value[MONTHLY_CANOPY_TRANSP],
									s->value[MONTHLY_CANOPY_EVAPO_TRANSP],
									s->value[MONTHLY_CANOPY_LATENT_HEAT],
									s->value[CANOPY_COVER_PROJ],
									s->value[DBHDC_EFF],
									s->value[HD_EFF],
									s->value[HD_MAX],
									s->value[HD_MIN],
									s->counter[N_TREE],
									s->value[MONTHLY_WUE],
									s->value[RESERVE_C],
									s->value[STEM_C],
									s->value[STEM_LIVE_WOOD_C],
									s->value[STEM_DEAD_WOOD_C],
									s->value[MAX_LEAF_C],
									s->value[MAX_FROOT_C],
									s->value[CROOT_C],
									s->value[CROOT_LIVE_WOOD_C],
									s->value[CROOT_DEAD_WOOD_C],
									s->value[BRANCH_C],
									s->value[BRANCH_LIVE_WOOD_C],
									s->value[BRANCH_DEAD_WOOD_C]);
						}
						if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) {
							logger(g_monthly_log,",*");
						}
					}
					if ( c->heights[height].dbhs[dbh].ages_count > 1 ) {
						logger(g_monthly_log,",**");
					}
				}
				if ( c->heights[height].dbhs_count > 1 ) {
					logger(g_monthly_log,",***");
				}
			}
			if ( c->tree_layers[layer].layer_n_height_class > 1 ) {
				logger(g_monthly_log,",****");
			}
		}
		if ( c->tree_layers_count > 1 ) {
			logger(g_monthly_log,",*****");
		}
	}
	/************************************************************************/
	/* printing variables at cell level only if there's more than one layer */

	if( c->heights_count > 1 )
	{
		logger(g_monthly_log, ",***,%3.4f,%3.4f,%3.4f",
				c->monthly_gpp,
				c->monthly_npp,
				c->monthly_aut_resp);

	}
	/* printing variables at cell level also if there's more than one layer */
	else
	{
		logger(g_monthly_log,",*****");
	}
	/* printing variables only at cell level */
	logger(g_monthly_log, ",%3.2f,%3.2f,%3.2f,%3.2f\n",
			c->monthly_et,
			c->monthly_lh_flux,
			c->asw,
			c->monthly_iwue);

	/************************************************************************/


	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[11]) : (MonthLength[11] )) == c->doy )
	{
		++years_counter;

		if ( years_counter ==  years_of_simulation)
		{
			logger(g_monthly_log, sz_launched, netcdf_get_version(), datetime_current());
			print_model_paths(g_monthly_log);
			//const char* p;
			//p = file_get_name_only(g_monthly_log->filename);
			logger(g_monthly_log, "#output file = %s\n", g_monthly_log->filename);
			print_model_settings(g_monthly_log);
		}
	}
}

void EOY_print_output_cell_level(cell_t *const c, const int year, const int years_of_simulation )
{
	int layer;
	int height;
	int dbh;
	int age;
	int species;

	static int years_counter;

	species_t *s;

	/* return if annual logging is off*/
	if ( ! g_annual_log ) return;

	/* test */
	//note it can be used only if no other classes are added!!
	/*
	if ( !year)
	{
		c->initial_tree_layers_count = c->tree_layers_count;
		c->initial_heights_count = c->heights_count;

		for ( height = c->heights_count - 1; height >= 0 ; --height )
		{
			c->heights[height].initial_dbhs_count = c->heights[height].dbhs_count;
			for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
			{
				c->heights[height].dbhs[dbh].initial_ages_count = c->heights[height].dbhs[dbh].ages_count;

				for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
				{
					c->heights[height].dbhs[dbh].ages[age].initial_species_count = c->heights[height].dbhs[dbh].ages[age].species_count;
				}
			}
		}
	}
	 */

	/* heading */
	if ( !year )
	{
		logger(g_annual_log, "%s", "YEAR");

		for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
		{
			for ( height = c->heights_count - 1; height >= 0 ; --height )
			{
				if( layer == c->heights[height].height_z )
				{
					/* heading for layers */
					logger(g_annual_log,",LAYER");

					/* heading for heights value */
					logger(g_annual_log,",HEIGHT");

					for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
					{
						/* heading for dbhs value */
						logger(g_annual_log, ",DBH");

						for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
						{
							/* heading for ages */
							logger(g_annual_log,",AGE");

							for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
							{
								/* heading for species name */
								logger(g_annual_log,",SPECIES");

								/* heading for management name */
								logger(g_annual_log,",MANAGEMENT");

								logger(g_annual_log,
										",GPP"
										",GR"
										",MR"
										",RA"
										",NPP"
										",CUE"
										",Y(perc)"
										",PeakLAI"
										",MaxLAI"
										",CC-Proj"
										",DBHDC"
										",CROWN_DIAMETER"
										",CROWN_AREA_PROJ"
										",APAR"
										",HD"
										",HDMAX"
										",HDMIN"
										",Ntree"
										",VEG_D"
										",FIRST_VEG_DAY"
										",CTRANSP"
										",CINT"
										",CLE"
										",WUE"
										",EFF_LIVE_TOTAL_FRAC"
										",MIN_RESERVE_C"
										",RESERVE_C"
										",STEM_C"
										",STEM_LIVE_C"
										",STEM_DEAD_C"
										",MAX_LEAF_C"
										",MAX_FROOT_C"
										",CROOT_C"
										",CROOT_LIVE_C"
										",CROOT_DEAD_C"
										",BRANCH_C"
										",BRANCH_LIVE_C"
										",BRANCH_DEAD_C"
										",TREE_CAI"
										",TREE_MAI"
										",VOLUME"
										",DELTA_TREE_VOL(perc)"
										",DELTA_AGB"
										",DELTA_BGB"
										",AGB"
										",BGB"
										",BGB:AGB"
										",DELTA_TREE_AGB"
										",DELTA_TREE_BGB"
										",STEM_RA"
										",LEAF_RA"
										",FROOT_RA"
										",CROOT_RA"
										",BRANCH_RA");
							}
							if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) {
								logger(g_annual_log,",*");
							}
						}
						if ( c->heights[height].dbhs[dbh].ages_count > 1 ) {
							logger(g_annual_log,",**");
						}
					}
					if ( c->heights[height].dbhs_count > 1 ) {
						logger(g_annual_log,",***");
					}
				}
				if ( c->tree_layers[layer].layer_n_height_class > 1 ) {
					logger(g_annual_log,",****");
				}
			}
			if ( c->tree_layers_count > 1 ) {
				logger(g_annual_log,",*****");
			}
		}
		/************************************************************************/
		/* heading variables only at cell level */
		logger(g_annual_log,",gpp,npp,ar,y(%%),et,le,soil-evapo,asw,iWue,vol,cum_vol");
		logger(g_annual_log,",CO2\n");

	}

	/*****************************************************************************************************/


	/* values */
	logger(g_annual_log, "%d", c->years[year].year);

	/* print class level values */
	if ( c->heights_count )
	{
		for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
		{
			qsort(c->heights, c->heights_count, sizeof(height_t), sort_by_heights_desc);

			for ( height = 0; height < c->heights_count; ++height )
			{
				if( layer == c->heights[height].height_z )
				{
					/* print layer */
					logger(g_annual_log,",%d", layer);

					/* print height */
					logger(g_annual_log,",%g", c->heights[height].value);

					for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
					{
						/* print dbh */
						logger(g_annual_log,",%g", c->heights[height].dbhs[dbh].value);

						// ALESSIOR TO ALESSIOC
						// SHOULD THIS BE CHANGED TO
						// start from c->heights[height].dbhs[dbh].ages_count-1 ?
						for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
						{
							/* print age */
							logger(g_annual_log,",%d", c->heights[height].dbhs[dbh].ages[age].value);

							for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
							{
								s  = &c->heights[height].dbhs[dbh].ages[age].species[species];

								/* print species name */
								logger(g_annual_log,",%.3s", c->heights[height].dbhs[dbh].ages[age].species[species].name);

								/* print management */
								logger(g_annual_log,",%c", sz_management[c->heights[height].dbhs[dbh].ages[age].species[species].management]);

								/* print variables at layer-class level */
								logger(g_annual_log,",%6.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%d,%d,%d,%3.4f"
										",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f"
										",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f",
										s->value[YEARLY_GPP],
										s->value[YEARLY_TOTAL_GROWTH_RESP],
										s->value[YEARLY_TOTAL_MAINT_RESP],
										s->value[YEARLY_TOTAL_AUT_RESP],
										s->value[YEARLY_NPP],
										s->value[YEARLY_CUE],
										s->value[YEARLY_TOTAL_AUT_RESP]/s->value[YEARLY_GPP]*100.,
										s->value[PEAK_LAI_PROJ],
										s->value[MAX_LAI_PROJ],
										s->value[CANOPY_COVER_PROJ],
										s->value[DBHDC_EFF],
										s->value[CROWN_DIAMETER],
										s->value[CROWN_AREA_PROJ],
										s->value[YEARLY_APAR],
										s->value[HD_EFF],
										s->value[HD_MAX],
										s->value[HD_MIN],
										s->counter[N_TREE],
										s->counter[YEARLY_VEG_DAYS],
										s->counter[FIRST_VEG_DAYS],
										s->value[YEARLY_CANOPY_TRANSP],
										s->value[YEARLY_CANOPY_INT],
										s->value[YEARLY_CANOPY_LATENT_HEAT],
										s->value[YEARLY_WUE],
										s->value[EFF_LIVE_TOTAL_WOOD_FRAC],
										s->value[MIN_RESERVE_C],
										s->value[RESERVE_C],
										s->value[STEM_C],
										s->value[STEM_LIVE_WOOD_C],
										s->value[STEM_DEAD_WOOD_C],
										s->value[MAX_LEAF_C],
										s->value[MAX_FROOT_C],
										s->value[CROOT_C],
										s->value[CROOT_LIVE_WOOD_C],
										s->value[CROOT_DEAD_WOOD_C],
										s->value[BRANCH_C],
										s->value[BRANCH_LIVE_WOOD_C],
										s->value[BRANCH_DEAD_WOOD_C],
										s->value[TREE_CAI],
										s->value[TREE_MAI],
										s->value[VOLUME],
										(s->value[TREE_CAI]/s->value[TREE_VOLUME])*100.,
										s->value[DELTA_AGB],
										s->value[DELTA_BGB],
										s->value[AGB],
										s->value[BGB],
										s->value[BGB]/s->value[AGB],
										s->value[DELTA_TREE_AGB],
										s->value[DELTA_TREE_BGB],
										s->value[YEARLY_STEM_AUT_RESP],
										s->value[YEARLY_LEAF_AUT_RESP],
										s->value[YEARLY_FROOT_AUT_RESP],
										s->value[YEARLY_CROOT_AUT_RESP],
										s->value[YEARLY_BRANCH_AUT_RESP]);
							}
							if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) {
								logger(g_annual_log,",*");
							}
						}
						if ( c->heights[height].dbhs[dbh].ages_count > 1 ) {
							logger(g_annual_log,",**");
						}
					}
					if ( c->heights[height].dbhs_count > 1 ) {
						logger(g_annual_log,",***");
					}
				}
				if ( c->tree_layers[layer].layer_n_height_class > 1 ) {
					logger(g_annual_log,",****");
				}
			}
			if ( c->tree_layers_count > 1 )
			{
				logger(g_annual_log,",*****");
			}
		}
		/************************************************************************/
		/* printing variables at cell level only if there's more than one layer */
		logger(g_annual_log, ",%3.4f,%3.4f,%3.4f,%3.4f,%3.2f,%3.2f,%3.2f,%3.2f,%3.2f,%3.2f,%3.2f",
				c->annual_gpp,
				c->annual_npp,
				c->annual_aut_resp,
				(c->annual_aut_resp/c->annual_gpp)*100.0,
				c->annual_et,
				c->annual_lh_flux,
				c->annual_soil_evapo,
				c->asw,
				c->annual_iwue,
				c->volume,
				c->cum_volume);
		/* print meteo variables at cell level */
		logger(g_annual_log, ",%3.4f\n", c->years[year].co2Conc);
	}
	else
	{
		//ALESSIOC TO ALLESSIOR PRINT EMPTY SPACES WHEN N_TREE = 0
	}

	/************************************************************************/

	++years_counter;

	if ( years_counter ==  years_of_simulation)
	{
		g_annual_log->std_output = 1;
		logger(g_annual_log, sz_launched, netcdf_get_version(), datetime_current());
		print_model_paths(g_annual_log);
		//const char* p;
		//p = file_get_name_only(g_annual_log->filename);
		logger(g_annual_log, "#output file = %s\n", g_annual_log->filename);
		print_model_settings(g_annual_log);
	}
}

/*************************************************************************************************************************************/
/*************************************************************************************************************************************/

void EOD_print_output_soil_cell_level(cell_t *const c, const int day, const int month, const int year, const int years_of_simulation )
{
	static int years_counter;

	/* return if monthly logging is off*/
	if ( ! g_daily_soil_log ) return;

	/* heading */
	if ( !day && !month && !year )
	{
		logger(g_daily_soil_log, "YEAR,MONTH,DAY");

		/************************************************************************/
		/* heading variables at cell level only if there's more than one layer */

		/* carbon pools */
		logger(g_daily_soil_log,
				",***,"
				"leaf_litr1C,"
				"leaf_litr2C,"
				"leaf_litr3C,"
				"leaf_litr4C,"
				"froot_litr1C,"
				"froot_litr2C,"
				"froot_litr3C,"
				"froot_litr4C,"
				"litrC,"
				"litr1C,"
				"litr2C,"
				"litr3C,"
				"litr4C,"
				"soilC,"
				"soil1C,"
				"soil2C,"
				"soil3C,"
				"soil4C");

		/* nitrogen pools */
		logger(g_daily_soil_log,
				",***,"
				"leaf_litr1N,"
				"leaf_litr2N,"
				"leaf_litr3N,"
				"leaf_litr4N,"
				"froot_litr1N,"
				"froot_litr2N,"
				"froot_litr3N,"
				"froot_litr4N,"
				"litrN,"
				"litr1N,"
				"litr2N,"
				"litr3N,"
				"litr4N,"
				"soilN,"
				"soil1N,"
				"soil2N,"
				"soil3N,"
				"soil4N\n");
	}

	/* values */
	logger(g_daily_soil_log, "%d,%d,%d", c->years[year].year, month + 1, day + 1);

	/************************************************************************/

	/* carbon pools */
	logger(g_daily_soil_log, ",***,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f",
			c->leaf_litr1C,
			c->leaf_litr2C,
			c->leaf_litr3C,
			c->leaf_litr4C,
			c->froot_litr1C,
			c->froot_litr2C,
			c->froot_litr3C,
			c->froot_litr4C,
			c->litrC,
			c->litr1C,
			c->litr2C,
			c->litr3C,
			c->litr4C,
			c->soilC,
			c->soil1C,
			c->soil2C,
			c->soil3C,
			c->soil4C);

	/* nitrogen pools */
	logger(g_daily_soil_log, ",***,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f\n",
			c->leaf_litr1N,
			c->leaf_litr2N,
			c->leaf_litr3N,
			c->leaf_litr4N,
			c->froot_litr1N,
			c->froot_litr2N,
			c->froot_litr3N,
			c->froot_litr4N,
			c->litrN,
			c->litr1N,
			c->litr2N,
			c->litr3N,
			c->litr4N,
			c->soilN,
			c->soil1N,
			c->soil2N,
			c->soil3N,
			c->soil4N);

	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365 ) )
	{
		++years_counter;

		if ( years_counter ==  years_of_simulation)
		{
			logger(g_daily_soil_log, sz_launched, netcdf_get_version(), datetime_current());
			print_model_paths(g_daily_soil_log);
			//const char* p;
			//p = file_get_name_only(g_daily_soil_log->filename);
			logger(g_daily_soil_log, "#output file = %s\n", g_daily_soil_log->filename);
			print_model_settings(g_daily_soil_log);
		}
	}
}

void EOM_print_output_soil_cell_level(cell_t *const c, const int month, const int year, const int years_of_simulation )
{
	static int years_counter;

	/* return if monthly logging is off*/
	if ( ! g_monthly_soil_log ) return;

	/* heading */
	if ( !month && !year )
	{
		logger(g_monthly_soil_log, "YEAR,MONTH");

		/************************************************************************/
		/* heading variables at cell level only if there's more than one layer */

		/* carbon pools */
		logger(g_monthly_soil_log,
				",***,"
				"leaf_litr1C,"
				"leaf_litr2C,"
				"leaf_litr3C,"
				"leaf_litr4C,"
				"froot_litr1C,"
				"froot_litr2C,"
				"froot_litr3C,"
				"froot_litr4C,"
				"litrC,"
				"litr1C,"
				"litr2C,"
				"litr3C,"
				"litr4C,"
				"soilC,"
				"soil1C,"
				"soil2C,"
				"soil3C,"
				"soil4C");

		/* nitrogen pools */
		logger(g_monthly_soil_log,
				",***,"
				"leaf_litr1N,"
				"leaf_litr2N,"
				"leaf_litr3N,"
				"leaf_litr4N,"
				"froot_litr1N,"
				"froot_litr2N,"
				"froot_litr3N,"
				"froot_litr4N,"
				"litrN,"
				"litr1N,"
				"litr2N,"
				"litr3N,"
				"litr4N,"
				"soilN,"
				"soil1N,"
				"soil2N,"
				"soil3N,"
				"soil4N\n");
	}

	/* values */
	logger (g_monthly_soil_log, "%d,%d", c->years[year].year, month +1 );

	/************************************************************************/

	/* carbon pools */
	logger(g_monthly_soil_log, ",***,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f",
			c->leaf_litr1C,
			c->leaf_litr2C,
			c->leaf_litr3C,
			c->leaf_litr4C,
			c->froot_litr1C,
			c->froot_litr2C,
			c->froot_litr3C,
			c->froot_litr4C,
			c->litrC,
			c->litr1C,
			c->litr2C,
			c->litr3C,
			c->litr4C,
			c->soilC,
			c->soil1C,
			c->soil2C,
			c->soil3C,
			c->soil4C);

	/* nitrogen pools */
	logger(g_monthly_soil_log, ",***,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f\n",
			c->leaf_litr1N,
			c->leaf_litr2N,
			c->leaf_litr3N,
			c->leaf_litr4N,
			c->froot_litr1N,
			c->froot_litr2N,
			c->froot_litr3N,
			c->froot_litr4N,
			c->litrN,
			c->litr1N,
			c->litr2N,
			c->litr3N,
			c->litr4N,
			c->soilN,
			c->soil1N,
			c->soil2N,
			c->soil3N,
			c->soil4N);

	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[11]) : (MonthLength[11] )) == c->doy )
	{
		++years_counter;

		if ( years_counter ==  years_of_simulation)
		{
			logger(g_monthly_soil_log, sz_launched, netcdf_get_version(), datetime_current());
			print_model_paths(g_monthly_soil_log);
			//const char* p;
			//p = file_get_name_only(g_monthly_soil_log->filename);
			logger(g_monthly_soil_log, "#output file = %s\n", g_monthly_soil_log->filename);
			print_model_settings(g_monthly_soil_log);
		}
	}
}

void EOY_print_output_soil_cell_level(cell_t *const c, const int year, const int years_of_simulation )
{
	static int years_counter;

	/* return if annual logging is off*/
	if ( ! g_annual_soil_log ) return;

	/* heading */
	if ( ! year )
	{
		logger(g_annual_soil_log, "%s", "YEAR");

		/************************************************************************/
		/* heading variables at cell level only if there's more than one layer */

		/* carbon pools */
		logger(g_annual_soil_log,
				",***,"
				"leaf_litr1C,"
				"leaf_litr2C,"
				"leaf_litr3C,"
				"leaf_litr4C,"
				"froot_litr1C,"
				"froot_litr2C,"
				"froot_litr3C,"
				"froot_litr4C,"
				"litrC,"
				"litr1C,"
				"litr2C,"
				"litr3C,"
				"litr4C,"
				"soilC,"
				"soil1C,"
				"soil2C,"
				"soil3C,"
				"soil4C");

		/* nitrogen pools */
		logger(g_annual_soil_log,
				",***,"
				"leaf_litr1N,"
				"leaf_litr2N,"
				"leaf_litr3N,"
				"leaf_litr4N,"
				"froot_litr1N,"
				"froot_litr2N,"
				"froot_litr3N,"
				"froot_litr4N,"
				"litrN,"
				"litr1N,"
				"litr2N,"
				"litr3N,"
				"litr4N,"
				"soilN,"
				"soil1N,"
				"soil2N,"
				"soil3N,"
				"soil4N\n");
	}
	/*****************************************************************************************************/

	/* values */
	logger(g_annual_soil_log, "%d", c->years[year].year);

	/************************************************************************/

	/* carbon pools */
	logger(g_annual_soil_log, ",***,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f",
			c->leaf_litr1C,
			c->leaf_litr2C,
			c->leaf_litr3C,
			c->leaf_litr4C,
			c->froot_litr1C,
			c->froot_litr2C,
			c->froot_litr3C,
			c->froot_litr4C,
			c->litrC,
			c->litr1C,
			c->litr2C,
			c->litr3C,
			c->litr4C,
			c->soilC,
			c->soil1C,
			c->soil2C,
			c->soil3C,
			c->soil4C);
	/* nitrogen pools */
	logger(g_annual_soil_log, ",***,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f\n",
			c->leaf_litr1N,
			c->leaf_litr2N,
			c->leaf_litr3N,
			c->leaf_litr4N,
			c->froot_litr1N,
			c->froot_litr2N,
			c->froot_litr3N,
			c->froot_litr4N,
			c->litrN,
			c->litr1N,
			c->litr2N,
			c->litr3N,
			c->litr4N,
			c->soilN,
			c->soil1N,
			c->soil2N,
			c->soil3N,
			c->soil4N);

	/************************************************************************/

	++years_counter;

	if ( years_counter == years_of_simulation )
	{
		g_annual_soil_log->std_output = 1;
		logger(g_annual_soil_log, sz_launched, netcdf_get_version(), datetime_current());
		print_model_paths(g_annual_soil_log);
		//const char* p;
		//p = file_get_name_only(g_annual_soil_log->filename);
		logger(g_annual_log, "#output file = %s\n", g_annual_soil_log->filename);
		print_model_settings(g_annual_soil_log);
	}
}
