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
//extern soil_settings_t* g_soil_settings;
extern logger_t* g_daily_log;
extern logger_t* g_monthly_log;
extern logger_t* g_annual_log;
extern char *g_sz_dataset_file;
extern char *g_sz_soil_file;
extern char *g_sz_input_met_file;
extern char *g_sz_settings_file;
extern char *g_sz_topo_file;
extern char *g_sz_co2_conc_file;

extern const char sz_launched[];

extern int MonthLength [];
extern int MonthLength_Leap [];

extern int g_do_txt;

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

	logger(_log, "\n\nsite: %s\n", g_settings->sitename);
	if ( g_sz_dataset_file )
		logger(_log, "input file = %s\n", get_filename(g_sz_dataset_file));
	logger(_log, "soil file = %s\n", get_filename(g_sz_soil_file));
	logger(_log, "topo file = %s\n", get_filename(g_sz_topo_file));
	logger(_log, "met file = %s\n", get_filename(g_sz_input_met_file));
	logger(_log, "settings file = %s\n", get_filename(g_sz_settings_file));
	if ( g_settings->CO2_trans )
		logger(_log, "CO2 file = %s\n", get_filename(g_sz_co2_conc_file));
}

void print_model_settings(logger_t*const log)
{
	//assert(log);

	logger(log, "*model settings*\n");
	logger(log, "CO2_mod = %s\n", g_settings->CO2_mod ? "on" : "off");
	logger(log, "CO2 trans = %s\n", (CO2_TRANS_VAR == g_settings->CO2_trans) ? "var" : (CO2_TRANS_ON == g_settings->CO2_trans) ? "on" : "off");
	if ( CO2_TRANS_OFF == g_settings->CO2_trans )
	{
		logger(log, "fixed co2 concentration = %g ppmv\n", g_settings->co2Conc);
	}
	else if ( CO2_TRANS_VAR == g_settings->CO2_trans )
	{
		logger(log, "year %d at which co2 concentration is fixed at value = %g ppmv\n", g_settings->year_start_co2_fixed, g_settings->co2Conc);
	}
	logger(log, "Resp accl = %s\n", g_settings->Resp_accl ? "on" : "off");
	logger(log, "regeneration = %s\n", g_settings->regeneration ? "on" : "off");
	logger(log, "Management = %s\n", g_settings->management ? "on" : "off");
	if ( g_settings->management )
	{
		logger(log, "Year Start Management = %d\n", g_settings->year_start_management);
	}
}

void EOD_print_cumulative_balance_cell_level(cell_t *const c, const int day, const int month, const int year, const int years_of_simulation)
{
	int layer;
	int height;
	int dbh;
	int age;
	int species;

	static int years_counter;

	species_t *s;
	meteo_t *met;

	met = c->years[year].m;

	/* return if daily logging is off*/
	if ( ! g_daily_log ) return;

	/* heading */
	if ( !day && !month && !year )
	{
		if ( g_do_txt )
			logger(g_daily_log, "%s \t%2s \t%s", "YEAR", "MONTH", "DAY");
		else
			logger(g_daily_log, "YEAR,MONTH,DAY");

		for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
		{
			for ( height = c->heights_count - 1; height >= 0 ; --height )
			{
				if( layer == c->heights[height].height_z )
				{
					/* heading for layers */
					if ( g_do_txt )
						logger(g_daily_log,"\t%s", "LAYER");
					else
						logger(g_daily_log, ",LAYER");

					/* heading for heights value */
					if ( g_do_txt )
						logger(g_daily_log,"\t%4s", "HEIGHT");
					else
						logger(g_daily_log,",HEIGHT");

					for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
					{
						if ( g_do_txt )
							logger(g_daily_log,"\t%4s", "DBH");
						else
							logger(g_daily_log,",DBH");
						

						for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
						{
							/* heading for ages */
							if ( g_do_txt )
								logger(g_daily_log,"\t%7s", "AGE");
							else
								logger(g_daily_log,",AGE");
							

							for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
							{
								/* heading for species name */
								if ( g_do_txt )
									logger(g_daily_log,"\t%10s", "SPECIES");
								else
									logger(g_daily_log,",SPECIES");

								/* heading for management */
								if ( g_do_txt )
									logger(g_daily_log, "\t%s", "MANAGEMENT");
								else
									logger(g_daily_log, ",MANAGEMENT");

								if ( g_do_txt )
										logger(g_daily_log,"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s"
												"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s"
												"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s "
												"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s",
												"GPP",
												"RG",
												"RM",
												"RA",
												"NPP",
												"CUE",
												"LAI",
												"CC_P",
												"CC_E",
												"Ntree",
												"VEG_D",
												"C_INT",
												"C_WAT",
												"C_EVA",
												"C_TRA",
												"C_ET",
												"C_LE",
												"S_EVA",
												"WUE",
												"WRes",
												"WS",
												"WSsap",
												"WSL",
												"WSD",
												"WL",
												"WFR",
												"WCR",
												"WCRsap",
												"WCRL",
												"WCRD",
												"WBB",
												"WBBsap",
												"WBBL",
												"WBBD",
												"dWRes",
												"dWS",
												"dWL",
												"dWFR",
												"dWCR",
												"dWBB",
												"SAR",
												"LAR",
												"FRAR",
												"CRAR",
												"BBAR",
												"FCO2",
												"FCO2_TR",
												"FAGE",
												"FT",
												"FVPD",
												"FN",
												"FSW"
										);
								else
									logger(g_daily_log,
												",GPP"
												",RG"
												",RM"
												",RA"
												",NPP"
												",CUE"
												",LAI"
												",CC_P"
												",CC_E"
												",Ntree"
												",VEG_D"
												",C_INT"
												",C_WAT"
												",C_EVA"
												",C_TRA"
												",C_ET"
												",C_LE"
												",S_EVA"
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
												",dWRes"
												",dWS"
												",dWL"
												",dWFR"
												",dWCR"
												",dWBB"
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
										);

							}
							if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 )
								if ( g_do_txt )
									logger(g_daily_log,"\t%10s", "*");
								else
									logger(g_daily_log,",*");
						}
						if ( c->heights[height].dbhs[dbh].ages_count > 1 )
							if ( g_do_txt )
								logger(g_daily_log,"\t%10s", "**");
							else
								logger(g_daily_log, ",**");
					}
					if ( c->heights[height].dbhs_count > 1 )
						if ( g_do_txt )
							logger(g_daily_log,"\t%10s", "***");
						else
							logger(g_daily_log,",***");
				}
				if ( c->tree_layers[layer].layer_n_height_class > 1 )
					if ( g_do_txt )
						logger(g_daily_log,"\t%10s", "****");
					else
						logger(g_daily_log,",****");
			}
			if ( c->tree_layers_count > 1 )
				if ( g_do_txt )
					logger(g_daily_log,"\t%10s", "*****");
				else
					logger(g_daily_log,",*****");
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

		/* heading variables at cell level only if there's more than one layer */
		if( c->heights_count > 1 )
		{
			if ( g_do_txt )
				logger(g_daily_log,"\t%10s \t%10s \t%10s \t%10s",
						"***",
						"gpp",
						"npp",
						"ar"
				);
			else
				logger(g_daily_log,",***,gpp,npp,ar");
		}
		/* heading variables at cell level also if there's more than one layer */
		else
		{
			if ( g_do_txt )
				logger(g_daily_log,"\t%10s", "*****");
			else
				logger(g_daily_log,",*****");
			
		}
		/* heading variables only at cell level */
		if ( g_do_txt )
			logger(g_daily_log,"\t%10s \t%10s \t%10s \t%10s \t%10s",
					"et",
					"le",
					"snow_pack",
					"asw",
					"iWue\n"
			);
		else
			logger(g_daily_log,",et,le,snow_pack,asw,iWue\n");

	}
	/*****************************************************************************************************/

	/* values */
	if ( g_do_txt )
		logger(g_daily_log, "%d \t%4d \t%4d", c->years[year].year, month + 1, day + 1);
	else
		logger(g_daily_log, "%d,%d,%d", c->years[year].year, month + 1, day + 1);

	/* print class level LAI values */
	for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
	{
		qsort(c->heights, c->heights_count, sizeof(height_t), sort_by_heights_asc);

		for ( height = c->heights_count - 1; height >= 0 ; --height )
		{
			if( layer == c->heights[height].height_z )
			{
				/* print layer */
				if ( g_do_txt )
					logger(g_daily_log,"\t%6d", layer);
				else
					logger(g_daily_log,",%d", layer);

				/* print height */
				if ( g_do_txt )
					logger(g_daily_log,"\t%5.4g", c->heights[height].value);
				else
					logger(g_daily_log,",%g", c->heights[height].value);

				for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
				{
					/* print dbh */
					if ( g_do_txt )
						logger(g_daily_log,"\t%5.4g", c->heights[height].dbhs[dbh].value);
					else
						logger(g_daily_log,",%g", c->heights[height].dbhs[dbh].value);

					for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
					{
						/* print age */
						if ( g_do_txt )
							logger(g_daily_log,"\t%7d", c->heights[height].dbhs[dbh].ages[age].value);
						else
							logger(g_daily_log,",%d", c->heights[height].dbhs[dbh].ages[age].value);

						for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
						{
							s  = &c->heights[height].dbhs[dbh].ages[age].species[species];

							/* print species name */
							if ( g_do_txt )
								logger(g_daily_log,"\t%8.3s", c->heights[height].dbhs[dbh].ages[age].species[species].name);
							else
								logger(g_daily_log,",%.3s", c->heights[height].dbhs[dbh].ages[age].species[species].name);

							/* print management */
							if ( g_do_txt )
								logger(g_daily_log,"\t%c", sz_management[c->heights[height].dbhs[dbh].ages[age].species[species].management]);
							else
								logger(g_daily_log,",%c", sz_management[c->heights[height].dbhs[dbh].ages[age].species[species].management]);

							/* print variables at layer-class level */
							if ( g_do_txt )
								logger(g_daily_log,"\t%6.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3d \t%3d \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f"
										"\t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f"
										"\t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f"
										"\t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f",
										s->value[DAILY_GPP_gC],
										s->value[TOTAL_GROWTH_RESP],
										s->value[TOTAL_MAINT_RESP],
										s->value[TOTAL_AUT_RESP],
										s->value[NPP_gC],
										s->value[CUE],
										s->value[LAI_PROJ],
										s->value[DAILY_CANOPY_COVER_PROJ],
										s->value[DAILY_CANOPY_COVER_EXP],
										s->counter[N_TREE],
										s->counter[VEG_DAYS],
										s->value[CANOPY_INT],
										s->value[CANOPY_WATER],
										s->value[CANOPY_EVAPO],
										s->value[CANOPY_TRANSP],
										s->value[CANOPY_EVAPO_TRANSP],
										s->value[CANOPY_LATENT_HEAT],
										c->daily_soil_evapo,
										s->value[WUE],
										s->value[RESERVE_C],
										s->value[STEM_C],
										s->value[STEM_SAPWOOD_C],
										s->value[STEM_LIVE_WOOD_C],
										s->value[STEM_DEAD_WOOD_C],
										s->value[LEAF_C],
										s->value[FINE_ROOT_C],
										s->value[COARSE_ROOT_C],
										s->value[COARSE_ROOT_SAPWOOD_C],
										s->value[COARSE_ROOT_LIVE_WOOD_C],
										s->value[COARSE_ROOT_DEAD_WOOD_C],
										s->value[BRANCH_C],
										s->value[BRANCH_SAPWOOD_C],
										s->value[BRANCH_LIVE_WOOD_C],
										s->value[BRANCH_DEAD_WOOD_C],
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
										s->value[BRANCH_AUT_RESP],
										s->value[F_CO2],
										s->value[F_CO2_TR],
										s->value[F_AGE],
										s->value[F_T],
										s->value[F_VPD],
										s->value[F_NUTR],
										s->value[F_SW]
									);
							else
								logger(g_daily_log,",%6.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%d,%d,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f"
										",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f"
										",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f"
										",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f",
										s->value[DAILY_GPP_gC],
										s->value[TOTAL_GROWTH_RESP],
										s->value[TOTAL_MAINT_RESP],
										s->value[TOTAL_AUT_RESP],
										s->value[NPP_gC],
										s->value[CUE],
										s->value[LAI_PROJ],
										s->value[DAILY_CANOPY_COVER_PROJ],
										s->value[DAILY_CANOPY_COVER_EXP],
										s->counter[N_TREE],
										s->counter[VEG_DAYS],
										s->value[CANOPY_INT],
										s->value[CANOPY_WATER],
										s->value[CANOPY_EVAPO],
										s->value[CANOPY_TRANSP],
										s->value[CANOPY_EVAPO_TRANSP],
										s->value[CANOPY_LATENT_HEAT],
										c->daily_soil_evapo,
										s->value[WUE],
										s->value[RESERVE_C],
										s->value[STEM_C],
										s->value[STEM_SAPWOOD_C],
										s->value[STEM_LIVE_WOOD_C],
										s->value[STEM_DEAD_WOOD_C],
										s->value[LEAF_C],
										s->value[FINE_ROOT_C],
										s->value[COARSE_ROOT_C],
										s->value[COARSE_ROOT_SAPWOOD_C],
										s->value[COARSE_ROOT_LIVE_WOOD_C],
										s->value[COARSE_ROOT_DEAD_WOOD_C],
										s->value[BRANCH_C],
										s->value[BRANCH_SAPWOOD_C],
										s->value[BRANCH_LIVE_WOOD_C],
										s->value[BRANCH_DEAD_WOOD_C],
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
										s->value[BRANCH_AUT_RESP],
										s->value[F_CO2],
										s->value[F_CO2_TR],
										s->value[F_AGE],
										s->value[F_T],
										s->value[F_VPD],
										s->value[F_NUTR],
										s->value[F_SW]
									);
						}

						if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 )
							if ( g_do_txt )
								logger(g_daily_log,"\t%10s", "*");
							else
								logger(g_daily_log,",*");
					}
					if ( c->heights[height].dbhs[dbh].ages_count > 1 )
						if ( g_do_txt )
							logger(g_daily_log,"\t%10s", "**");
						else
							logger(g_daily_log,",**");
				}
				if ( c->heights[height].dbhs_count > 1 )
					if ( g_do_txt )
						logger(g_daily_log,"\t%10s", "***");
					else
						logger(g_daily_log,",***");
			}
			if ( c->tree_layers[layer].layer_n_height_class > 1 )
				if ( g_do_txt )
					logger(g_daily_log,"\t%10s", "****");
				else
					logger(g_daily_log,",****");
		}
		if ( c->tree_layers_count > 1 )
		{
			if ( g_do_txt )
				logger(g_daily_log,"\t%10s", "*****");
			else
				logger(g_daily_log,",*****");
		}
		/*
						if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 ) logger(g_daily_log, ",*");
					}
					if ( c->heights[height].dbhs[dbh].ages_count > 1 ) logger(g_daily_log, ",**");
				}
				if ( c->heights[height].dbhs_count > 1 ) logger(g_daily_log,",***");
			}
			if ( c->tree_layers[layer].layer_n_height_class > 1 ) logger(g_daily_log,",****");
		}
		if ( c->tree_layers_count > 1 )
		{
			logger(g_daily_log,",*****");
		}
		 */

	}
	/************************************************************************/

	/* printing variables at cell level only if there's more than one layer */
	if( c->heights_count > 1 )
	{
		if ( g_do_txt )
			logger(g_daily_log, "\t%4s \t%3.4f \t%3.4f \t%3.4f",
				"***",
				c->daily_gpp,
				c->daily_npp_gC,
				c->daily_aut_resp);
		else
			logger(g_daily_log, ",%s,%3.4f,%3.4f,%3.4f",
				"***",
				c->daily_gpp,
				c->daily_npp_gC,
				c->daily_aut_resp);
	
	}
	/* printing variables at cell level also if there's more than one layer */
	else
	{
		if ( g_do_txt )
			logger(g_daily_log,"\t%10s", "*****");
		else
			logger(g_daily_log,",*****");
	}
	/* printing variables only at cell level */
	if ( g_do_txt )
		logger(g_daily_log, "\t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f\n",
				c->daily_et,
				c->daily_latent_heat_flux,
				c->snow_pack,
				c->asw,
				c->daily_iwue
		);
	else
		logger(g_daily_log, ",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f\n",
				c->daily_et,
				c->daily_latent_heat_flux,
				c->snow_pack,
				c->asw,
				c->daily_iwue
		);
	

	/************************************************************************/

	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365 ) )
	{
		++years_counter;

		if ( years_counter ==  years_of_simulation)
		{
			logger(g_daily_log, sz_launched, netcdf_get_version(), datetime_current());
			print_model_paths(g_daily_log);
			if ( g_daily_log )
			{
				//const char* p;
				//p = file_get_name_only(g_daily_log->filename);
				logger(g_daily_log, "output file = %s\n", g_daily_log->filename);
			}
			print_model_settings(g_daily_log);
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

	/* return if monthly logging is off*/
	if ( ! g_monthly_log ) return;

	/* heading */
	if ( !month && !year )
	{
		if ( g_do_txt )
			logger(g_monthly_log, "%s \t%2s", "YEAR", "MONTH");
		else
			logger(g_monthly_log, "YEAR,MONTH");

		for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
		{
			for ( height = c->heights_count - 1; height >= 0 ; --height )
			{
				if( layer == c->heights[height].height_z )
				{
					/* heading for layers */
					if ( g_do_txt )
						logger(g_monthly_log,"\t%s", "LAYER");
					else
						logger(g_monthly_log,",LAYER");


					/* heading for heights value */
					if ( g_do_txt )
						logger(g_monthly_log,"\t%4s", "HEIGHT");
					else
						logger(g_monthly_log,",HEIGHT");

					for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
					{
						/* heading for dbhs value */
						if ( g_do_txt )
							logger(g_monthly_log, "\t%4s", "DBH");
						else
							logger(g_monthly_log, ",DBH");

						for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
						{
							/* heading for ages */
							if ( g_do_txt )
								logger(g_monthly_log,"\t%7s", "AGE");
							else
								logger(g_monthly_log,",AGE");

							for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
							{
								/* heading for species name */
								if ( g_do_txt )
									logger(g_monthly_log,"\t%10s", "SPECIES");
								else
									logger(g_monthly_log,",SPECIES");

								/* heading for management */
								if ( g_do_txt )
									logger(g_monthly_log,"\t%s", "MANAGEMENT");
								else
									logger(g_monthly_log,",MANAGEMENT");

								if ( g_do_txt )
									logger(g_monthly_log,"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s"
											"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s\t%4s \t%4s \t%4s \t%4s",
											"GPP",
											"RA",
											"NPP",
											"CUE",
											"CTRANSP",
											"CET",
											"CLE",
											"CC",
											"DBHDC",
											"HD_EFF",
											"HDMAX",
											"HDMIN",
											"N_TREE",
											"WUE",
											"DEL_M_WS",
											"DEL_M_WL",
											"DEL_M_WFR",
											"DEL_M_WCR",
											"DEL_M_WRES",
											"DEL_M_BB",
											"DEL_M_FRUIT",
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
											"WBBD");
								else
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
											",DEL_M_WS"
											",DEL_M_WL"
											",DEL_M_WFR"
											",DEL_M_WCR"
											",DEL_M_WRES"
											",DEL_M_BB"
											",DEL_M_FRUIT"
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
											",WBBD");

							}
							if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 )
								if ( g_do_txt )
									logger(g_monthly_log,"\t%10s", "*");
								else
									logger(g_monthly_log,",*");
						}
						if ( c->heights[height].dbhs[dbh].ages_count > 1 )
							if ( g_do_txt )
								logger(g_monthly_log,"\t%10s", "**");
							else
								logger(g_monthly_log,",**");
					}
					if ( c->heights[height].dbhs_count > 1 )
						if ( g_do_txt )
							logger(g_monthly_log,"\t%10s", "***");
						else
							logger(g_monthly_log,",***");
				}
				if ( c->tree_layers[layer].layer_n_height_class > 1 )
					if ( g_do_txt )
						logger(g_monthly_log,"\t%10s", "****");
					else
						logger(g_monthly_log,",****");
			}
			if ( c->tree_layers_count > 1 )
				if ( g_do_txt )
					logger(g_monthly_log,"\t%10s", "*****");
				else
					logger(g_monthly_log,",*****");
		}
		/************************************************************************/
		/* heading variables at cell level only if there's more than one layer */

		if( c->heights_count > 1 )
		{
			if ( g_do_txt )
				logger(g_monthly_log,"\t%10s \t%10s \t%10s \t%10s",
					"***",
					"gpp",
					"npp",
					"ar");
			else
				logger(g_monthly_log,",***,gpp,npp,ar");
		}
		/* heading variables at cell level also if there's more than one layer */
		else
		{
			if ( g_do_txt )
				logger(g_monthly_log,"\t%10s", "*****");
			else
				logger(g_monthly_log,",*****");
		}
		/* heading variables only at cell level */
		if ( g_do_txt )
			logger(g_monthly_log,"\t%10s \t%10s \t%10s \t%10s",
					"et",
					"le",
					"asw",
					"iWue\n");
		else
			logger(g_monthly_log,",et,le,asw,iWue\n");
	}

	/************************************************************************/
	/* values */
	if ( g_do_txt )
		logger ( g_monthly_log, "%d \t%4d", c->years[year].year, month +1 );
	else
		logger ( g_monthly_log, "%d,%d", c->years[year].year, month +1 );

	/* print class level values */
	for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
	{
		for ( height = c->heights_count - 1; height >= 0 ; --height )
		{
			if( layer == c->heights[height].height_z )
			{
				/* print layer */
				if ( g_do_txt )
					logger(g_monthly_log,"\t%6d", layer);
				else
					logger(g_monthly_log,",%d", layer);

				/* print height */
				if ( g_do_txt )
					logger(g_monthly_log,"\t%5.3g", c->heights[height].value);
				else
					logger(g_monthly_log,",%g", c->heights[height].value);

				for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
				{
					/* print age */
					if ( g_do_txt )
						logger(g_monthly_log,"\t%5.3g", c->heights[height].dbhs[dbh].value);
					else
						logger(g_monthly_log,",%g", c->heights[height].dbhs[dbh].value);

					for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
					{
						/* print age */
						if ( g_do_txt )
							logger(g_monthly_log,"\t%7d", c->heights[height].dbhs[dbh].ages[age].value);
						else
							logger(g_monthly_log,",%d", c->heights[height].dbhs[dbh].ages[age].value);

						for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
						{
							s  = &c->heights[height].dbhs[dbh].ages[age].species[species];

							/* print species name */
							if ( g_do_txt )
								logger(g_monthly_log,"\t%8.3s", c->heights[height].dbhs[dbh].ages[age].species[species].name);
							else
								logger(g_monthly_log,",%.3s", c->heights[height].dbhs[dbh].ages[age].species[species].name);

							/* print management */
							if ( g_do_txt )
								logger(g_monthly_log,"\t%c", sz_management[c->heights[height].dbhs[dbh].ages[age].species[species].management]);
							else
								logger(g_monthly_log,",%c", sz_management[c->heights[height].dbhs[dbh].ages[age].species[species].management]);

							/* print variables at layer-class level */
							if ( g_do_txt )
								logger(g_monthly_log,"\t%6.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3d \t%3.4f \t%3.4f \t%3.4f \t%3.4f"
										"\t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f",
										s->value[MONTHLY_GPP_gC],
										s->value[MONTHLY_TOTAL_AUT_RESP],
										s->value[MONTHLY_NPP_gC],
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
										s->value[M_C_TO_STEM] ,
										s->value[M_C_TO_LEAF] ,
										s->value[M_C_TO_FINEROOT] ,
										s->value[M_C_TO_COARSEROOT] ,
										s->value[M_C_TO_RESERVE] ,
										s->value[M_C_TO_BRANCH] ,
										s->value[M_C_TO_FRUIT] ,
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
										s->value[BRANCH_DEAD_WOOD_C]);
							else
								logger(g_monthly_log,",%6.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%d,%3.4f,%3.4f,%3.4f,%3.4f"
										",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f",
										s->value[MONTHLY_GPP_gC],
										s->value[MONTHLY_TOTAL_AUT_RESP],
										s->value[MONTHLY_NPP_gC],
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
										s->value[M_C_TO_STEM] ,
										s->value[M_C_TO_LEAF] ,
										s->value[M_C_TO_FINEROOT] ,
										s->value[M_C_TO_COARSEROOT] ,
										s->value[M_C_TO_RESERVE] ,
										s->value[M_C_TO_BRANCH] ,
										s->value[M_C_TO_FRUIT] ,
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
										s->value[BRANCH_DEAD_WOOD_C]);
						}
						if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 )
							if ( g_do_txt )
								logger(g_monthly_log,"\t%10s", "*");
							else
								logger(g_monthly_log,",*");
					}
					if ( c->heights[height].dbhs[dbh].ages_count > 1 )
						if ( g_do_txt )
							logger(g_monthly_log,"\t%10s", "**");
						else
							logger(g_monthly_log,",**");
				}
				if ( c->heights[height].dbhs_count > 1 )
					if ( g_do_txt )
						logger(g_monthly_log,"\t%10s", "***");
					else
						logger(g_monthly_log,",***");
			}
			if ( c->tree_layers[layer].layer_n_height_class > 1 )
				if ( g_do_txt )
					logger(g_monthly_log,"\t%10s", "****");
				else
					logger(g_monthly_log,",****");
		}
		if ( c->tree_layers_count > 1 )
		{
			if ( g_do_txt )
				logger(g_monthly_log,"\t%10s", "*****");
			else
				logger(g_monthly_log,",*****");
		}
	}
	/************************************************************************/
	/* printing variables at cell level only if there's more than one layer */

	if( c->heights_count > 1 )
	{
		if ( g_do_txt )
			logger(g_monthly_log, "\t%4s \t%3.4f \t%3.4f \t%3.4f ",
					"***",
					c->monthly_gpp,
					c->monthly_npp_gC,
					c->monthly_aut_resp);
		else
			logger(g_monthly_log, ",***,%3.4f,%3.4f,%3.4f",
					c->monthly_gpp,
					c->monthly_npp_gC,
					c->monthly_aut_resp);
			
	}
	/* printing variables at cell level also if there's more than one layer */
	else
	{
		if ( g_do_txt )
			logger(g_monthly_log,"\t%10s", "*****");
		else
			logger(g_monthly_log,",*****");
	}
	/* printing variables only at cell level */
	if ( g_do_txt )
		logger(g_monthly_log, "\t%3.2f \t%3.2f \t%3.2f \t%3.2f\n",
			c->monthly_et,
			c->monthly_latent_heat_flux,
			c->asw,
			c->monthly_iwue);
	else
		logger(g_monthly_log, ",%3.2f,%3.2f,%3.2f,%3.2f\n",
			c->monthly_et,
			c->monthly_latent_heat_flux,
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
			if ( g_monthly_log )
			{
				//const char* p;
				//p = file_get_name_only(g_monthly_log->filename);
				logger(g_monthly_log, "output file = %s\n", g_monthly_log->filename);
			}
			print_model_settings(g_monthly_log);
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
					if ( g_do_txt )
						logger(g_annual_log,"\t%s", "LAYER");
					else
						logger(g_annual_log,",LAYER");

					/* heading for heights value */
					if ( g_do_txt )
						logger(g_annual_log,"\t%4s", "HEIGHT");
					else
						logger(g_annual_log,",HEIGHT");

					for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
					{
						/* heading for dbhs value */
						if ( g_do_txt )
							logger(g_annual_log, "\t%4s", "DBH");
						else
							logger(g_annual_log, ",DBH");

						for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
						{
							/* heading for ages */
							if ( g_do_txt )
								logger(g_annual_log,"\t%7s", "AGE");
							else
								logger(g_annual_log,",AGE");

							for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
							{
								/* heading for species name */
								if ( g_do_txt )
									logger(g_annual_log,"\t%10s", "SPECIES");
								else
									logger(g_annual_log,",SPECIES");

								/* heading for management name */
								if ( g_do_txt )
									logger(g_annual_log,"\t%s", "MANAGEMENT");
								else
									logger(g_annual_log,",MANAGEMENT");

								if ( g_do_txt )
									logger(g_annual_log,"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s"
											"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s",
											"GPP",
											"GR",
											"MR",
											"RA",
											"NPP",
											"CUE",
											"Y(%%)",
											"PeakLAI",
											"CC",
											"DBHDC",
											"HD",
											"HDMAX",
											"HDMIN",
											"Ntree",
											"VEG_D",
											"VEG_FIRST",
											"CTRANSP",
											"CINT",
											"CLE",
											"WUE",
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
											"FRAR",
											"CRAR",
											"BBAR");
								else
									logger(g_annual_log,
											",GPP"
											",GR"
											",MR"
											",RA"
											",NPP"
											",CUE"
											",Y(%%)"
											",PeakLAI"
											",CC"
											",DBHDC"
											",HD"
											",HDMAX"
											",HDMIN"
											",Ntree"
											",VEG_D"
											",VEG_FIRST"
											",CTRANSP"
											",CINT"
											",CLE"
											",WUE"
											",LTR"
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
											",SAR"
											",LAR"
											",FRAR"
											",CRAR"
											",BBAR");

							}
							if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 )
								if ( g_do_txt )
									logger(g_annual_log,"\t%10s", "*");
								else
									logger(g_annual_log,",*");
						}
						if ( c->heights[height].dbhs[dbh].ages_count > 1 )
							if ( g_do_txt )
								logger(g_annual_log,"\t%10s", "**");
							else
								logger(g_annual_log,",**");
					}
					if ( c->heights[height].dbhs_count > 1 )
						if ( g_do_txt )
							logger(g_annual_log,"\t%10s", "***");
						else
							logger(g_annual_log,",***");
				}
				if ( c->tree_layers[layer].layer_n_height_class > 1 )
					if ( g_do_txt )
						logger(g_annual_log,"\t%10s", "****");
					else
						logger(g_annual_log,",****");
			}
			if ( c->tree_layers_count > 1 )
				if ( g_do_txt )
					logger(g_annual_log,"\t%10s", "*****");
				else
					logger(g_annual_log,",*****");
		}
		/************************************************************************/
		/* heading variables at cell level only if there's more than one layer */

		if( c->heights_count > 1 )
		{
			if ( g_do_txt )
				logger(g_annual_log,"\t%10s \t%10s \t%10s \t%10s \t%10s",
						"***",
						"gpp",
						"npp",
						"ar",
						"y(%%)");
			else
				logger(g_annual_log,",***,gpp,npp,ar,y(%%)");

		}
		/* heading variables at cell level also if there's more than one layer */
		else
		{
			if ( g_do_txt )
				logger(g_annual_log,"\t%10s", "*****");
			else
				logger(g_annual_log,",*****");
		}
		/* heading variables only at cell level */
		if ( g_do_txt )
			logger(g_annual_log,"\t%10s \t%10s \t%10s \t%10s",
					"et",
					"le",
					"asw",
					"iWue\n");
		else
			logger(g_annual_log,",et,le,asw,iWue\n");
	}
	/*****************************************************************************************************/

	/* values */
	logger(g_annual_log, "%d", c->years[year].year);

	/* print class level values */
	if ( c->heights_count )
	{
		for ( layer = c->tree_layers_count - 1; layer >= 0; --layer )
		{
			qsort(c->heights, c->heights_count, sizeof (height_t), sort_by_heights_desc);

			for ( height = c->heights_count - 1; height >= 0 ; --height )
			{
				if( layer == c->heights[height].height_z )
				{
					/* print layer */
					if ( g_do_txt )
						logger(g_annual_log,"\t%6d", layer);
					else
						logger(g_annual_log,",%d", layer);

					/* print height */
					if ( g_do_txt )
						logger(g_annual_log,"\t%5.5g", c->heights[height].value);
					else
						logger(g_annual_log,",%g", c->heights[height].value);

					for ( dbh = c->heights[height].dbhs_count - 1; dbh >= 0; --dbh )
					{
						/* print age */
						if ( g_do_txt )
							logger(g_annual_log,"\t%5.5g", c->heights[height].dbhs[dbh].value);
						else
							logger(g_annual_log,",%g", c->heights[height].dbhs[dbh].value);

						for ( age = 0; age < c->heights[height].dbhs[dbh].ages_count ; ++age )
						{
							/* print age */
							if ( g_do_txt )
								logger(g_annual_log,"\t%7d", c->heights[height].dbhs[dbh].ages[age].value);
							else
								logger(g_annual_log,",%d", c->heights[height].dbhs[dbh].ages[age].value);

							for ( species = 0; species < c->heights[height].dbhs[dbh].ages[age].species_count; ++species )
							{
								s  = &c->heights[height].dbhs[dbh].ages[age].species[species];

								/* print species name */
								if ( g_do_txt )
									logger(g_annual_log,"\t%8.3s", c->heights[height].dbhs[dbh].ages[age].species[species].name);
								else
									logger(g_annual_log,",%.3s", c->heights[height].dbhs[dbh].ages[age].species[species].name);

								/* print management */
								if ( g_do_txt )
									logger(g_annual_log,"\t%c", sz_management[c->heights[height].dbhs[dbh].ages[age].species[species].management]);
								else
									logger(g_annual_log,",%c", sz_management[c->heights[height].dbhs[dbh].ages[age].species[species].management]);

								/* print variables at layer-class level */
								if ( g_do_txt )
									logger(g_annual_log,"\t%6.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3d \t%3d \t%3d \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f"
											"\t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f",
											s->value[YEARLY_GPP_gC],
											s->value[YEARLY_TOTAL_GROWTH_RESP],
											s->value[YEARLY_TOTAL_MAINT_RESP],
											s->value[YEARLY_TOTAL_AUT_RESP],
											s->value[YEARLY_NPP_gC],
											s->value[YEARLY_CUE],
											s->value[YEARLY_TOTAL_AUT_RESP]/s->value[YEARLY_GPP_gC]*100,
											s->value[PEAK_LAI_PROJ],
											s->value[CANOPY_COVER_PROJ],
											s->value[DBHDC_EFF],
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
								else
									logger(g_annual_log,",%6.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%d,%d,%d,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f"
											",%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f,%3.4f",
											s->value[YEARLY_GPP_gC],
											s->value[YEARLY_TOTAL_GROWTH_RESP],
											s->value[YEARLY_TOTAL_MAINT_RESP],
											s->value[YEARLY_TOTAL_AUT_RESP],
											s->value[YEARLY_NPP_gC],
											s->value[YEARLY_CUE],
											s->value[YEARLY_TOTAL_AUT_RESP]/s->value[YEARLY_GPP_gC]*100,
											s->value[PEAK_LAI_PROJ],
											s->value[CANOPY_COVER_PROJ],
											s->value[DBHDC_EFF],
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
							if ( c->heights[height].dbhs[dbh].ages[age].species_count > 1 )
								if ( g_do_txt )
									logger(g_annual_log,"\t%10s", "*");
								else
									logger(g_annual_log,",*");
						}
						if ( c->heights[height].dbhs[dbh].ages_count > 1 )
							if ( g_do_txt )
								logger(g_annual_log,"\t%10s", "**");
							else
								logger(g_annual_log,",**");
					}
					if ( c->heights[height].dbhs_count > 1 )
						if ( g_do_txt )
							logger(g_annual_log,"\t%10s", "***");
						else
							logger(g_annual_log,",***");
				}
				if ( c->tree_layers[layer].layer_n_height_class > 1 )
					if ( g_do_txt )
						logger(g_annual_log,"\t%10s", "****");
					else
						logger(g_annual_log,",****");
			}
			if ( c->tree_layers_count > 1 )
			{
				if ( g_do_txt )
					logger(g_annual_log,"\t%10s", "*****");
				else
					logger(g_annual_log,",*****");
			}
		}
		/************************************************************************/
		/* printing variables at cell level only if there's more than one layer */

		if( c->heights_count > 1 )
		{
			if ( g_do_txt )
				logger(g_annual_log, "\t%4s \t%3.4f \t%3.4f \t%3.4f \t%3.4f",
					"***",
					c->annual_gpp,
					c->annual_npp_gC,
					c->annual_aut_resp,
					(c->annual_aut_resp/c->annual_gpp)*100.0);
			else
				logger(g_annual_log, "***,%3.4f,%3.4f,%3.4f,%3.4f",
					c->annual_gpp,
					c->annual_npp_gC,
					c->annual_aut_resp,
					(c->annual_aut_resp/c->annual_gpp)*100.0);

		}
		/* printing variables at cell level also if there's more than one layer */
		else
		{
			if ( g_do_txt )
				logger(g_annual_log,"\t%10s", "*****");
			else
				logger(g_annual_log,",*****");
		}
	}
	else
	{
		//ALESSIOC TO ALLESSIOR PRINT EMPTY SPACES WHEN TREE = 0
	}

	/* printing variables only at cell level */
	if ( g_do_txt )
		logger(g_annual_log, "\t%3.2f \t%3.2f \t%3.2f \t%3.2f\n",
				c->annual_et,
				c->annual_latent_heat_flux,
				c->asw,
				c->annual_iwue);
	else
		logger(g_annual_log, ",%3.2f,%3.2f,%3.2f,%3.2f\n",
				c->annual_et,
				c->annual_latent_heat_flux,
				c->asw,
				c->annual_iwue);

	/************************************************************************/

	++years_counter;

	if ( years_counter ==  years_of_simulation)
	{
		if (g_annual_log) g_annual_log->std_output = 1;
		logger(g_annual_log, sz_launched, netcdf_get_version(), datetime_current());
		print_model_paths(g_annual_log);
		if (g_annual_log)
		{
			//const char* p;
			//p = file_get_name_only(g_annual_log->filename);
			logger(g_annual_log, "output file = %s\n", g_annual_log->filename);
		}
		print_model_settings(g_annual_log);
	}
}
//
//void EOD_print_cumulative_balance_cell_level(cell_t *const c, const int day, const int month, const int year, const int years_of_simulation )
//{
//	int layer;
//	int height;
//	int dbh;
//	int age;
//	int species;
//
//	static int years_counter;
//
//	species_t *s;
//
//	/* return if daily logging is off*/
//	if ( ! g_daily_log ) return;
//
//	/* heading */
//	if ( !day && !month && !year )
//	{
//		logger(g_daily_log,"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s"
//				"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s"
//				"\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s",
//				"YEAR",	"MONTH", "DAY",	"GPP", "RA", "NPP", "CUE", "LAI", "CC_P", "CC_E", "Ntree", "C_INT",
//				"C_EVA", "C_TRA", "C_ET", "C_LE", "S_EVA", "WUE", "WRes", "WS", "WSsap", "WSL", "WSD", "WL",
//				"WFR", "WCR", "WCRsap", "WCRL", "WCRD", "WBB", "WBBsap", "WBBL", "WBBD", "FCO2", "FCO2_TR");
//	}
//	/*
//
//	logger(g_daily_log,"\t%6.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f"
//			"\t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f"
//			"\t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f \t%3.4f",
//			c->daily_gpp;
//			c->daily_aut_resp,
//			c->daily_npp_gC,
//
//
//			)
//			*/
//
//}


