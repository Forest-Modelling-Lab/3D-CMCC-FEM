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
extern char *g_sz_input_file;
extern char *g_sz_parameterization_path;
extern char *g_sz_soil_file;
extern char *g_sz_input_met_file;
extern char *g_sz_settings_file;
extern char *g_sz_topo_file;

extern const char sz_launched[];

extern int MonthLength [];
extern int MonthLength_Leap [];

static const char* get_filename(const char *const s) {
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

static void write_paths(logger_t *const _log) {
	assert(_log);

	logger(_log, "\n\nsite: %s\n", get_filename(g_soil_settings->sitename));
	logger(_log, "input file = %s\n", get_filename(g_sz_input_met_file));
	logger(_log, "soil file = %s\n", get_filename(g_sz_soil_file));
	logger(_log, "topo file = %s\n", get_filename(g_sz_topo_file));
	logger(_log, "met file = %s\n", get_filename(g_sz_input_met_file));
	logger(_log, "settings file = %s\n", get_filename(g_sz_settings_file));
}


void EOD_print_cumulative_balance_cell_level(cell_t *const c, const int day, const int month, const int year, const int years_of_simulation )
{
	int layer;
	int height;
	int age;
	int species;

	static int years_counter;

	species_t *s;


	//FIXME this is just an approach for ALESSIOR

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

					for ( age = 0; age < c->heights[height].ages_count ; ++age )
					{
						/* heading for ages */
						logger(g_daily_log,"\t%7s", "AGE");

						for ( species = 0; species < c->heights[height].ages[age].species_count; ++species )
						{
							/* heading for species name */
							logger(g_daily_log,"\t%10s", "SPECIES");

							logger(g_daily_log,"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s"
									"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s",
									"GPP",
									"AR",
									"NPP",
									"LAI",
									"AVDBH",
									"CC",
									"Ntree",
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
						if ( c->heights[height].ages[age].species_count > 1 ) logger(g_daily_log,"\t%10s", "*");
					}
					if ( c->heights[height].ages_count > 1 ) logger(g_daily_log,"\t%10s", "**");
				}
				if ( c->t_layers[layer].layer_n_height_class > 1 ) logger(g_daily_log,"\t%10s", "***");
			}
			if ( c->t_layers_count > 1 ) logger(g_daily_log,"\t%10s", "****");
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

		/************************************************************************/
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

				for ( age = 0; age < c->heights[height].ages_count ; ++age )
				{
					/* print age */
					logger(g_daily_log,"\t%7d", c->heights[height].ages[age].value);

					for ( species = 0; species < c->heights[height].ages[age].species_count; ++species )
					{
						s  = &c->heights[height].ages[age].species[species];

						/* print species name */
						logger(g_daily_log,"\t%8.3s", c->heights[height].ages[age].species[species].name);

						/* print variables at layer-class level */
						logger(g_daily_log,"\t%6.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3d \t%3.4f \t%3.4f"
								"\t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f",
								s->value[DAILY_GPP_gC],
								s->value[TOTAL_AUT_RESP],
								s->value[NPP_gC],
								s->value[LAI],
								s->value[AVDBH],
								s->value[CANOPY_COVER_DBHDC],
								s->counter[N_TREE],
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
					if ( c->heights[height].ages[age].species_count > 1 ) logger(g_daily_log,"\t%10s", "*");
				}
				if ( c->heights[height].ages_count > 1 ) logger(g_daily_log,"\t%10s", "**");
			}
			if ( c->t_layers[layer].layer_n_height_class > 1 ) logger(g_daily_log,"\t%10s", "***");
		}
		if ( c->t_layers_count > 1 )
		{
			logger(g_daily_log,"\t%10s", "****");
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
			c->annual_et,
			c->annual_latent_heat_flux,
			c->asw);


	/************************************************************************/

	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365 ) )
	{
		++years_counter;

		if ( years_counter ==  years_of_simulation)
		{
			logger(g_daily_log, sz_launched, netcdf_get_version(), get_datetime());
			write_paths(g_daily_log);
		}
	}
}

void EOM_print_cumulative_balance_cell_level(cell_t *const c, const int month, const int year, const int years_of_simulation )
{
	static int years_counter;

	//FIXME this is just an approach

	/* heading */
	if ( !month && !year )
	{
		logger(g_monthly_log, "%s \t%2s \t%2s", "YEAR", "MONTH", "LC");
	}

	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[11]) : (MonthLength[11] )) == c->doy )
	{
		++years_counter;

		if ( years_counter ==  years_of_simulation)
		{
			logger(g_monthly_log, sz_launched, netcdf_get_version(), get_datetime());
			write_paths(g_monthly_log);
		}
	}
}

void EOY_print_cumulative_balance_cell_level(cell_t *const c, const int year, const int years_of_simulation )
{
	int layer;
	int height;
	int age;
	int species;

	static int years_counter;

	species_t *s;

	//FIXME this is just an approach for ALESSIOR

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

					for ( age = 0; age < c->heights[height].ages_count ; ++age )
					{
						/* heading for ages */
						logger(g_annual_log,"\t%7s", "AGE");

						for ( species = 0; species < c->heights[height].ages[age].species_count; ++species )
						{
							/* heading for species name */
							logger(g_annual_log,"\t%10s", "SPECIES");

							logger(g_annual_log,"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s"
									"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s",
									"GPP",
									"AR",
									"NPP",
									"Y(%)",
									"LAI",
									"AVDBH",
									"CC",
									"DBHDC",
									"Ntree",
									"CET",
									"CLE",
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
						if ( c->heights[height].ages[age].species_count > 1 ) logger(g_annual_log,"\t%10s", "*");
					}
					if ( c->heights[height].ages_count > 1 ) logger(g_annual_log,"\t%10s", "**");
				}
				if ( c->t_layers[layer].layer_n_height_class > 1 ) logger(g_annual_log,"\t%10s", "***");
			}
			if ( c->t_layers_count > 1 ) logger(g_annual_log,"\t%10s", "****");
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

		/************************************************************************/
	}
	/*****************************************************************************************************/

	/* values */
	logger(g_annual_log, "%d", c->years[year].year);

	/* print class level LAI values */
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

				for ( age = 0; age < c->heights[height].ages_count ; ++age )
				{
					/* print age */
					logger(g_annual_log,"\t%7d", c->heights[height].ages[age].value);

					for ( species = 0; species < c->heights[height].ages[age].species_count; ++species )
					{
						s  = &c->heights[height].ages[age].species[species];

						/* print species name */
						logger(g_annual_log,"\t%8.3s", c->heights[height].ages[age].species[species].name);

						/* print variables at layer-class level */
						logger(g_annual_log,"\t%6.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3d \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f"
								"\t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f",
								s->value[YEARLY_GPP_gC],
								s->value[YEARLY_TOTAL_AUT_RESP],
								s->value[YEARLY_NPP_gC],
								s->value[YEARLY_TOTAL_AUT_RESP]/s->value[YEARLY_GPP_gC]*100,
								s->value[PEAK_LAI],
								s->value[AVDBH],
								s->value[CANOPY_COVER_DBHDC],
								s->value[DBHDC_EFF],
								s->counter[N_TREE],
								s->value[YEARLY_CANOPY_EVAPO_TRANSP],
								s->value[YEARLY_CANOPY_LATENT_HEAT],
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
					if ( c->heights[height].ages[age].species_count > 1 ) logger(g_annual_log,"\t%10s", "*");
				}
				if ( c->heights[height].ages_count > 1 ) logger(g_annual_log,"\t%10s", "**");
			}
			if ( c->t_layers[layer].layer_n_height_class > 1 ) logger(g_annual_log,"\t%10s", "***");
		}
		if ( c->t_layers_count > 1 )
		{
			logger(g_annual_log,"\t%10s", "****");
		}
	}
	/************************************************************************/

	//	/* printing variables at cell level */
	//	logger(g_annual_log, "\t%3.2f \t%3.2f \t%3.2f \t%3.2f \t%3.2f \t%3.2f \t%3.2f\n",
	//			c->annual_gpp,
	//			c->annual_npp_gC,
	//			c->annual_aut_resp,
	//			(c->annual_aut_resp/c->annual_gpp)*100.0,
	//			c->annual_et,
	//			c->annual_latent_heat_flux,
	//			c->asw);

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
	//

	//
	//
	//	/* heading */
	//	if ( !year )
	//	{
	//		logger(g_annual_log, "%s \t%2s", "YEAR", "LC");
	//		logger(g_annual_log, "\t%6s \t%8s \t%8s \t%8s \t%8s \t%8s", "GPP(gC/m2d)", "AR(gC/m2d)", "NPP(gC/m2d)", "Y(%)", "ET", "N_TREE\n");
	//	}
	//	/* values */
	//	logger(g_annual_log, "%d \t%3d \t%10.4f \t%10.4f \t%10.4f \t%10.4f \t%10.4f \t%10d\n",
	//			c->years[year].year,
	//			c->t_layers_count,
	//			c->annual_gpp,
	//			c->annual_aut_resp,
	//			c->annual_npp_gC,
	//			((c->annual_aut_resp/c->annual_gpp)*100),
	//			c->annual_et,
	//			c->cell_n_trees);

	++years_counter;

	if ( years_counter ==  years_of_simulation)
	{
		logger(g_annual_log, sz_launched, netcdf_get_version(), get_datetime());
		write_paths(g_annual_log);
	}
}

void Get_EOD_soil_balance_cell_level(cell_t *const c, const int year, const int month, const int day)
{

	//	static int previous_layer_number;
	//	static int doy;
	//	int soil = 0;

	if(day  == 0 && month == 0 && year == 0)
	{
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_soil_log, "RUNNING DNDC.....\n");
			//fixSergio improve a multilayer based log, specular to the one used for the aboveground

		}
		else
		{
			logger(g_soil_log, "No soil simulation!!!\n");
		}
	}


	if (!string_compare_i(g_settings->dndc, "on"))
	{
		//fixSergio improve a multilayer based log, specular to the one used for the aboveground
		//ALESSIOC
		/*
		if ((day == 0 && month == 0 && year == 0) || previous_layer_number != c->annual_layer_number)
		{
			//			soil_Log ("\n%s \t%s \t%2s \t%2s \t%2s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s "
			//					"\t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s"
			//					"\t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \n",
			//					"DOY", "YEAR", "MONTH", "DAY", "leafLittering","fineRootLittering", "woodLittering", "stemLittering", "coarseRootLittering", "day_C_mine",
			//					"day_N_mine", "day_N_assim", "day_soil_nh3", "day_clayn", "day_clay_N",
			//					"wrcvl", "wrcl", "wrcr", "wcrb", "wcrh","whumus", "wtcavai", "w_no3", "w_nh4", "w_nh3", "wsoc", "day_no2",
			//					"runoff_N", "previousSoilT", "soilSurfaceT", "temp_avet", "yr_avet","base_clay_N", "max_clay_N",
			//					"AddC", "AddCN", "AddC1", "AddC2", "AddC3");
			logger(g_soil_log, "\n%s \t%s \t%2s \t%2s \t%2s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s "
					"\t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s"
					"\t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \n",
					"DOY", "YEAR", "MONTH", "DAY", "soilMoisture","soilTemp", "leafLittering","fineRootLittering", "woodLittering",
					"stemLittering", "coarseRootLittering","soc", "doc", "rcvl","rcl", "rcr", "CRB1", "CRB2", "crhl",
					"crhr", "dphum", "no3", "no2", "nh4","nh3", "co2", "day_O2", "dailyGPP[0]", "dailyAutResp[0]", "dailyTotGPP", "dailyTotRespAut",
					"stemBranchBiomass", "inert_C", "AddC1", "AddC2", "AddC3","sts", "mmm",
					"initialOrganicC", "waterContent", "litco22", "litco23", "CEC");
		}
		if ((day == 0 && month == 0) || previous_layer_number != c->annual_layer_number)
		{
			doy = 1;
		}
		 */
		//		soil_Log ("%d \t%8d \t%5d \t%3d \t%2d \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f "
		//				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f"
		//				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f"
		//				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \n",
		//				doy++, yos[years].year, month+1, day+1,
		//				c->leafLittering,
		//				c->fineRootLittering,
		//				c->woodLittering,
		//				c->stemLittering,
		//				c->coarseRootLittering,
		//				c->day_C_mine,
		//				c->day_N_mine,
		//				c->day_N_assim,
		//				c->day_soil_nh3,
		//				c->day_clayn,
		//				c->day_clay_N,
		//				c->wrcvl,
		//				c->wrcl,
		//				c->wrcr,
		//				c->wcrb,
		//				c->wcrh,
		//				c->whumus,
		//				c->wtcavai,
		//				c->w_no3,
		//				c->w_nh4,
		//				c->w_nh3,
		//				c->wsoc,
		//				c->day_no2,
		//				c->runoff_N,
		//				c->previousSoilT,
		//				c->soilSurfaceT,
		//				c->temp_avet,
		//				c->yr_avet,
		//				c->base_clay_N,
		//				c->max_clay_N,
		//				c->AddC,
		//				c->AddCN,
		//				c->AddC1,
		//				c->AddC2,
		//				c->AddC3);

		//soil structure variables
		//fixSergio improve a multilayer based log, specular to the one used for the aboveground
		//		if ((day == 0 && month == 0 && years == 0) || previous_layer_number != c->annual_layer_number)
		//		{

		//		}
		//		if ((day == 0 && month == 0) || previous_layer_number != c->annual_layer_number)
		//		{
		//			doy = 1;
		//		}

		//ALESSIOC
		/*
		logger(g_soil_log, "%d \t%5d \t%3d \t%2d \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f "
				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f"
				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f"
				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f  \t%8.6f \n ",
				doy++, c->years[year].year, month+1, day+1,
				c->soils[0].soilMoisture,
				c->soils[0].soilTemp,
				c->leafLittering,
				c->fineRootLittering,
				c->stemBrancLittering,
				c->stemLittering,
				c->coarseRootLittering,
				c->soils[0].soc,
				c->soils[0].doc,
				c->soils[0].rcvl,
				c->soils[0].rcl,
				c->soils[0].rcr,
				c->soils[0].CRB1,
				c->soils[0].CRB2,
				c->soils[0].crhl,
				c->soils[0].crhr,
				c->soils[0].dphum,
				c->soils[0].no3,
				c->soils[0].no2,
				c->soils[0].nh4,
				c->soils[0].nh3,
				c->soils[0].co2,
				c->soils[0].day_O2,
				//				c->soils[0].dcbavai,
				//				c->soils[0].drcvl,
				//				c->soils[0].DRCB1,
				//				c->soils[0].DRCB2,
				//				c->soils[0].clay_nh4,
				c->layer_daily_gpp[0],
				c->layer_daily_aut_resp[0],
				c->daily_gpp,
				c->daily_aut_resp,
				c->stemBranchBiomass,

				c->soils[0].inert_C,
				c->AddC1,
				c->AddC2,
				c->AddC3,
				c->soils[0].sts,
				c->soils[0].mmm,
				c->soils[0].initialOrganicC,
				c->soils[0].waterContent,
				c->soils[0].litco22,
				c->soils[0].litco23,
				c->soils[0].CEC);
		 */
		c->leafLittering = 0;
		c->fineRootLittering = 0;
		c->coarseRootLittering =0;
		c->stemLittering =0;
		c->stemBrancLittering = 0;
		c->leaflitN = 0;
		c->fineRootlitN = 0;
		c->coarseRootlitN = 0;
		c->stemlitN = 0;
		c->stemBranclitN = 0;

		c->daily_gpp = 0;
		c->daily_aut_resp = 0;

		//ALESSIOC
		/*
		c->layer_daily_gpp[0] = 0;
		c->layer_daily_aut_resp[0] = 0;
		 */
	}
}
