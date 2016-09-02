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
extern soil_settings_t* g_soil_settings;
extern logger_t* g_daily_log;
extern logger_t* g_monthly_log;
extern logger_t* g_annual_log;
extern logger_t* g_soil_log;
//extern char *g_sz_input_file;
//extern char *g_sz_parameterization_path;
extern char *g_sz_dataset_file;
extern char *g_sz_soil_file;
extern char *g_sz_input_met_file;
extern char *g_sz_settings_file;
extern char *g_sz_topo_file;

extern const char sz_launched[];

extern int MonthLength [];
extern int MonthLength_Leap [];

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

static void write_paths(logger_t *const _log)
{
	assert(_log);

	logger(_log, "\n\nsite: %s\n", get_filename(g_soil_settings->sitename));
	logger(_log, "input file = %s\n", get_filename(g_sz_dataset_file));
	logger(_log, "soil file = %s\n", get_filename(g_sz_soil_file));
	logger(_log, "topo file = %s\n", get_filename(g_sz_topo_file));
	logger(_log, "met file = %s\n", get_filename(g_sz_input_met_file));
	logger(_log, "settings file = %s\n", get_filename(g_sz_settings_file));
}

void EOD_print_cumulative_balance_cell_level(cell_t *const c, const int day, const int month, const int year, const int years_of_simulation )
{
	int layer;
	int height;
	int dbh;
	int age;
	int species;

	static int years_counter;

	species_t *s;

	/* heading */
	if ( !day && !month && !year )
	{
		logger(g_daily_log, "%s \t%2s \t%s", "YEAR", "MONTH", "DAY");

		for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
		{
			for ( height = c->heights_count - 1; height >= 0 ; --height )
			{
				if( layer == c->heights[height].height_z )
				{
					/* heading for layers */
					logger(g_daily_log,"\t%s", "LAYER");

					/* heading for heights value */
					logger(g_daily_log,"\t%4s", "HEIGHT");

					for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
					{
						logger(g_daily_log,"\t%4s", "DBH");

						for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
						{
							/* heading for ages */
							logger(g_daily_log,"\t%7s", "AGE");

							for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
							{
								/* heading for species name */
								logger(g_daily_log,"\t%10s", "SPECIES");

								logger(g_daily_log,"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s"
										"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s",
										"GPP",
										"AR",
										"NPP",
										"LAI",
										"CC",
										"Ntree",
										"VEG_D",
										"CET",
										"CLE",
										"dWRes",
										"dWS",
										"dWL",
										"dWFR",
										"dWCR",
										"dWBB",
										"SAR",
										"LAR",
										"FAR",
										"CRAR",
										"BBAR");
							}
							if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) logger(g_daily_log,"\t%10s", "*");
						}
						if ( c->heights[height].dbhs[dbh].ages_count > 1 ) logger(g_daily_log,"\t%10s", "**");
					}
					if ( c->heights[height].dbhs_count > 1 ) logger(g_daily_log,"\t%10s", "***");
				}
				if ( c->t_layers[layer].layer_n_height_class > 1 ) logger(g_daily_log,"\t%10s", "****");
			}
			if ( c->t_layers_count > 1 ) logger(g_daily_log,"\t%10s", "*****");
		}
		/************************************************************************/

		/* heading variables at cell level only if there's more than one layer */
		if( c->heights_count > 1 )
		{
			logger(g_daily_log,"\t%10s \t%10s \t%10s \t%10s",
					"***",
					"gpp",
					"npp",
					"ar");
		}
		/* heading variables at cell level also if there's more than one layer */
		else
		{
			logger(g_daily_log,"\t%10s", "*****");
		}
		/* heading variables only at cell level */
		logger(g_daily_log,"\t%10s \t%10s \t%10s",
				"et",
				"le",
				"asw\n");

	}
	/*****************************************************************************************************/

	/* values */
	logger(g_daily_log, "%d \t%4d \t%4d", c->years[year].year, month + 1, day + 1);

	/* print class level LAI values */
	for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
	{
		for ( height = c->heights_count - 1; height >= 0 ; --height )
		{
			if( layer == c->heights[height].height_z )
			{
				/* print layer */
				logger(g_daily_log,"\t%6d", layer);

				/* print height */
				logger(g_daily_log,"\t%5.3g", c->heights[height].value);

				for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
				{
					/* print dbh */
					logger(g_daily_log,"\t%5.3g", c->heights[height].dbhs[dbh].value);

					for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
					{
						/* print age */
						logger(g_daily_log,"\t%7d", c->heights[height].dbhs[dbh].ages[age].value);

						for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
						{
							s  = &c->heights[height].dbhs[dbh].ages[age].species[species];

							/* print species name */
							logger(g_daily_log,"\t%8.3s", c->heights[height].dbhs[dbh].ages[age].species[species].name);

							/* print variables at layer-class level */
							logger(g_daily_log,"\t%6.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3d \t%3d \t%3.4f \t%3.4f"
									"\t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f",
									s->value[DAILY_GPP_gC],
									s->value[TOTAL_AUT_RESP],
									s->value[NPP_gC],
									s->value[LAI],
									s->value[CANOPY_COVER_DBHDC],
									s->counter[N_TREE],
									s->counter[VEG_DAYS],
									s->value[CANOPY_EVAPO_TRANSP],
									s->value[CANOPY_LATENT_HEAT],
									s->value[C_TO_RESERVE],
									s->value[C_TO_STEM],
									s->value[C_TO_LEAF],
									s->value[C_TO_FINEROOT],
									s->value[C_TO_COARSEROOT],
									s->value[C_TO_BRANCH],
									s->value[STEM_AUT_RESP],
									s->value[LEAF_AUT_RESP],
									s->value[FINE_ROOT_AUT_RESP],
									s->value[COARSE_ROOT_AUT_RESP],
									s->value[BRANCH_AUT_RESP]);
						}
						if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) logger(g_daily_log,"\t%10s", "*");
					}
					if ( c->heights[height].dbhs[dbh].ages_count > 1 ) logger(g_daily_log,"\t%10s", "**");
				}
				if ( c->heights[height].dbhs_count > 1 ) logger(g_daily_log,"\t%10s", "***");
			}
			if ( c->t_layers[layer].layer_n_height_class > 1 ) logger(g_daily_log,"\t%10s", "****");
		}
		if ( c->t_layers_count > 1 )
		{
			logger(g_daily_log,"\t%10s", "*****");
		}
	}
	/************************************************************************/

	/* printing variables at cell level only if there's more than one layer */
	if( c->heights_count > 1 )
	{
		logger(g_daily_log, "\t%4s \t%3.4f \t%3.4f \t%3.4f ",
				"***",
				c->daily_gpp,
				c->daily_npp_gC,
				c->daily_aut_resp);
	}
	/* printing variables at cell level also if there's more than one layer */
	else
	{
		logger(g_daily_log,"\t%10s", "*****");
	}
	/* printing variables only at cell level */
	logger(g_daily_log, "\t%3.2f \t%3.2f \t%3.2f\n",
			c->daily_et,
			c->daily_latent_heat_flux,
			c->asw);

	/************************************************************************/

	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365 ) )
	{
		++years_counter;

		if ( years_counter ==  years_of_simulation)
		{
			logger(g_daily_log, sz_launched, netcdf_get_version(), get_datetime());
			write_paths(g_daily_log);

			logger(g_daily_log, "*model setting*\n");
			logger(g_daily_log, "-CO2_mod = %s\n", g_settings->CO2_mod);
			logger(g_daily_log, "-CO2 fixed = %s\n", g_settings->CO2_fixed);
			logger(g_daily_log, "-Management = %s\n", g_settings->management);
		}
	}
}

void EOM_print_cumulative_balance_cell_level(cell_t *const c, const int month, const int year, const int years_of_simulation )
{
	int layer;
	int height;
	int dbh;
	int age;
	int species;

	static int years_counter;

	species_t *s;

	/* heading */
	if ( !month && !year )
	{
		logger(g_monthly_log, "%s \t%2s", "YEAR", "MONTH");

		for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
		{
			for ( height = c->heights_count - 1; height >= 0 ; --height )
			{
				if( layer == c->heights[height].height_z )
				{
					/* heading for layers */
					logger(g_monthly_log,"\t%s", "LAYER");

					/* heading for heights value */
					logger(g_monthly_log,"\t%4s", "HEIGHT");

					for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
					{
						/* heading for dbhs value */
						logger(g_monthly_log, "\t%4s", "DBH");

						for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
						{
							/* heading for ages */
							logger(g_monthly_log,"\t%7s", "AGE");

							for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
							{
								/* heading for species name */
								logger(g_monthly_log,"\t%10s", "SPECIES");

								logger(g_monthly_log,"\t%4s \t%4s \t%4s \t%4s \t%4s",
										"GPP",
										"AR",
										"NPP",
										"CET",
										"CLE");
							}
							if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) logger(g_monthly_log,"\t%10s", "*");
						}
						if ( c->heights[height].dbhs[dbh].ages_count > 1 ) logger(g_monthly_log,"\t%10s", "**");
					}
					if ( c->heights[height].dbhs_count > 1 ) logger(g_monthly_log,"\t%10s", "***");
				}
				if ( c->t_layers[layer].layer_n_height_class > 1 ) logger(g_monthly_log,"\t%10s", "****");
			}
			if ( c->t_layers_count > 1 ) logger(g_monthly_log,"\t%10s", "*****");
		}
		/************************************************************************/
		/* heading variables at cell level only if there's more than one layer */

		if( c->heights_count > 1 )
		{
			logger(g_monthly_log,"\t%10s \t%10s \t%10s \t%10s",
					"***",
					"gpp",
					"npp",
					"ar");
		}
		/* heading variables at cell level also if there's more than one layer */
		else
		{
			logger(g_monthly_log,"\t%10s", "*****");
		}
		/* heading variables only at cell level */
		logger(g_monthly_log,"\t%10s \t%10s \t%10s",
				"et",
				"le",
				"asw\n");
	}
	/************************************************************************/
	/* values */
	logger ( g_monthly_log, "%d \t%4d", c->years[year].year, month +1 );

	/* print class level values */
	for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
	{
		for ( height = c->heights_count - 1; height >= 0 ; --height )
		{
			if( layer == c->heights[height].height_z )
			{
				/* print layer */
				logger(g_monthly_log,"\t%6d", layer);

				/* print height */
				logger(g_monthly_log,"\t%5.3g", c->heights[height].value);

				for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
				{
					/* print age */
					logger(g_monthly_log,"\t%5.3g", c->heights[height].dbhs[dbh].value);

					for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
					{
						/* print age */
						logger(g_monthly_log,"\t%7d", c->heights[height].dbhs[dbh].ages[age].value);

						for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
						{
							s  = &c->heights[height].dbhs[dbh].ages[age].species[species];

							/* print species name */
							logger(g_monthly_log,"\t%8.3s", c->heights[height].dbhs[dbh].ages[age].species[species].name);

							/* print variables at layer-class level */
							logger(g_monthly_log,"\t%6.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f ",
									s->value[MONTHLY_GPP_gC],
									s->value[MONTHLY_TOTAL_AUT_RESP],
									s->value[MONTHLY_NPP_gC],
									s->value[MONTHLY_CANOPY_EVAPO_TRANSP],
									s->value[MONTHLY_CANOPY_LATENT_HEAT]);
						}
						if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) logger(g_monthly_log,"\t%10s", "*");
					}
					if ( c->heights[height].dbhs[dbh].ages_count > 1 ) logger(g_monthly_log,"\t%10s", "**");
				}
				if ( c->heights[height].dbhs_count > 1 ) logger(g_monthly_log,"\t%10s", "***");
			}
			if ( c->t_layers[layer].layer_n_height_class > 1 ) logger(g_monthly_log,"\t%10s", "****");
		}
		if ( c->t_layers_count > 1 )
		{
			logger(g_monthly_log,"\t%10s", "*****");
		}
	}
	/************************************************************************/
	/* printing variables at cell level only if there's more than one layer */

	if( c->heights_count > 1 )
	{
		logger(g_monthly_log, "\t%4s \t%3.4f \t%3.4f \t%3.4f ",
				"***",
				c->monthly_gpp,
				c->monthly_npp_gC,
				c->monthly_aut_resp);
	}
	/* printing variables at cell level also if there's more than one layer */
	else
	{
		logger(g_monthly_log,"\t%10s", "*****");
	}
	/* printing variables only at cell level */
	logger(g_monthly_log, "\t%3.2f \t%3.2f \t%3.2f\n",
			c->monthly_et,
			c->monthly_latent_heat_flux,
			c->asw);

	/************************************************************************/


	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[11]) : (MonthLength[11] )) == c->doy )
	{
		++years_counter;

		if ( years_counter ==  years_of_simulation)
		{
			logger(g_monthly_log, sz_launched, netcdf_get_version(), get_datetime());
			write_paths(g_monthly_log);

			logger(g_monthly_log, "*model setting*\n");
			logger(g_monthly_log, "-CO2_mod = %s\n", g_settings->CO2_mod);
			logger(g_monthly_log, "-CO2 fixed = %s\n", g_settings->CO2_fixed);
			logger(g_monthly_log, "-Management = %s\n", g_settings->management);
		}
	}
}

void EOY_print_cumulative_balance_cell_level(cell_t *const c, const int year, const int years_of_simulation )
{
	int layer;
	int height;
	int dbh;
	int age;
	int species;

	static int years_counter;

	species_t *s;

	/* heading */
	if ( !year )
	{
		logger(g_annual_log, "%s", "YEAR");

		for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
		{
			for ( height = c->heights_count - 1; height >= 0 ; --height )
			{
				if( layer == c->heights[height].height_z )
				{
					/* heading for layers */
					logger(g_annual_log,"\t%s", "LAYER");

					/* heading for heights value */
					logger(g_annual_log,"\t%4s", "HEIGHT");

					for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
					{
						/* heading for dbhs value */
						logger(g_annual_log, "\t%4s", "DBH");

						for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
						{
							/* heading for ages */
							logger(g_annual_log,"\t%7s", "AGE");

							for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
							{
								/* heading for species name */
								logger(g_annual_log,"\t%10s", "SPECIES");

								logger(g_annual_log,"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s"
										"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s",
										"GPP",
										"AR",
										"NPP",
										"Y(%)",
										"LAI",
										"CC",
										"DBHDC",
										"Ntree",
										"VEG_D",
										"CET",
										"CLE",
										"LTR",
										"WRes",
										"WS",
										"WSL",
										"WSD",
										"PWL",
										"PWFR",
										"WCR",
										"WCRL",
										"WCRD",
										"WBB",
										"WBBL",
										"WBBD",
										"SAR",
										"LAR",
										"FAR",
										"CRAR",
										"BBAR");
							}
							if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) logger(g_annual_log,"\t%10s", "*");
						}
						if ( c->heights[height].dbhs[dbh].ages_count > 1 ) logger(g_annual_log,"\t%10s", "**");
					}
					if ( c->heights[height].dbhs_count > 1 ) logger(g_annual_log,"\t%10s", "***");
				}
				if ( c->t_layers[layer].layer_n_height_class > 1 ) logger(g_annual_log,"\t%10s", "****");
			}
			if ( c->t_layers_count > 1 ) logger(g_annual_log,"\t%10s", "*****");
		}
		/************************************************************************/
		/* heading variables at cell level only if there's more than one layer */

		if( c->heights_count > 1 )
		{
			logger(g_annual_log,"\t%10s \t%10s \t%10s \t%10s \t%10s",
					"***",
					"gpp",
					"npp",
					"ar",
					"y(%)");
		}
		/* heading variables at cell level also if there's more than one layer */
		else
		{
			logger(g_annual_log,"\t%10s", "*****");
		}
		/* heading variables only at cell level */
		logger(g_annual_log,"\t%10s \t%10s \t%10s",
				"et",
				"le",
				"asw\n");
	}
	/*****************************************************************************************************/

	/* values */
	logger(g_annual_log, "%d", c->years[year].year);

	/* print class level values */
	for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
	{
		for ( height = c->heights_count - 1; height >= 0 ; --height )
		{
			if( layer == c->heights[height].height_z )
			{
				/* print layer */
				logger(g_annual_log,"\t%6d", layer);

				/* print height */
				logger(g_annual_log,"\t%5.3g", c->heights[height].value);

				for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
				{
					/* print age */
					logger(g_annual_log,"\t%5.3g", c->heights[height].dbhs[dbh].value);

					for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
					{
						/* print age */
						logger(g_annual_log,"\t%7d", c->heights[height].dbhs[dbh].ages[age].value);

						for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
						{
							s  = &c->heights[height].dbhs[dbh].ages[age].species[species];

							/* print species name */
							logger(g_annual_log,"\t%8.3s", c->heights[height].dbhs[dbh].ages[age].species[species].name);

							/* print variables at layer-class level */
							logger(g_annual_log,"\t%6.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3d \t%3d \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f"
									"\t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f",
									s->value[YEARLY_GPP_gC],
									s->value[YEARLY_TOTAL_AUT_RESP],
									s->value[YEARLY_NPP_gC],
									s->value[YEARLY_TOTAL_AUT_RESP]/s->value[YEARLY_GPP_gC]*100,
									s->value[PEAK_LAI],
									s->value[CANOPY_COVER_DBHDC],
									s->value[DBHDC_EFF],
									s->counter[N_TREE],
									s->counter[YEARLY_VEG_DAYS],
									s->value[YEARLY_CANOPY_EVAPO_TRANSP],
									s->value[YEARLY_CANOPY_LATENT_HEAT],
									s->value[EFF_LIVE_TOTAL_WOOD_FRAC],
									s->value[RESERVE_C],
									s->value[STEM_C],
									s->value[STEM_LIVE_WOOD_C],
									s->value[STEM_DEAD_WOOD_C],
									s->value[MAX_LEAF_C],
									s->value[MAX_FINE_ROOT_C],
									s->value[COARSE_ROOT_C],
									s->value[COARSE_ROOT_LIVE_WOOD_C],
									s->value[COARSE_ROOT_DEAD_WOOD_C],
									s->value[BRANCH_C],
									s->value[BRANCH_LIVE_WOOD_C],
									s->value[BRANCH_DEAD_WOOD_C],
									s->value[YEARLY_STEM_AUT_RESP],
									s->value[YEARLY_LEAF_AUT_RESP],
									s->value[YEARLY_FINE_ROOT_AUT_RESP],
									s->value[YEARLY_COARSE_ROOT_AUT_RESP],
									s->value[YEARLY_BRANCH_AUT_RESP]);
						}
						if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) logger(g_annual_log,"\t%10s", "*");
					}
					if ( c->heights[height].dbhs[dbh].ages_count > 1 ) logger(g_annual_log,"\t%10s", "**");
				}
				if ( c->heights[height].dbhs_count > 1 ) logger(g_annual_log,"\t%10s", "***");
			}
			if ( c->t_layers[layer].layer_n_height_class > 1 ) logger(g_annual_log,"\t%10s", "****");
		}
		if ( c->t_layers_count > 1 )
		{
			logger(g_annual_log,"\t%10s", "*****");
		}
	}
	/************************************************************************/
	/* printing variables at cell level only if there's more than one layer */

	if( c->heights_count > 1 )
	{
		logger(g_annual_log, "\t%4s \t%3.4f \t%3.4f \t%3.4f \t%3.4f",
				"***",
				c->annual_gpp,
				c->annual_npp_gC,
				c->annual_aut_resp,
				(c->annual_aut_resp/c->annual_gpp)*100.0);
	}
	/* printing variables at cell level also if there's more than one layer */
	else
	{
		logger(g_annual_log,"\t%10s", "*****");
	}
	/* printing variables only at cell level */
	logger(g_annual_log, "\t%3.2f \t%3.2f \t%3.2f\n",
			c->annual_et,
			c->annual_latent_heat_flux,
			c->asw);

	/************************************************************************/

	++years_counter;

	if ( years_counter ==  years_of_simulation)
	{
		logger(g_annual_log, sz_launched, netcdf_get_version(), get_datetime());
		write_paths(g_annual_log);

		logger(g_annual_log, "*model setting*\n");
		logger(g_annual_log, "-CO2_mod = %s\n", g_settings->CO2_mod);
		logger(g_annual_log, "-CO2 fixed = %s\n", g_settings->CO2_fixed);
		logger(g_annual_log, "-Management = %s\n", g_settings->management);
	}
}

void Get_EOD_soil_balance_cell_level(cell_t *const c, const int year, const int month, const int day)
{
	if( !day && !month && !year )
	{
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_soil_log, "RUNNING DNDC.....\n");
		}
		else
		{
			logger(g_soil_log, "No soil simulation!!!\n");
		}
	}
	if (!string_compare_i(g_settings->dndc, "on"))
	{
		//fixme to complete
	}
}
