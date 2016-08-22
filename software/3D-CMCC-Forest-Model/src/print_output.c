/* cumulative_balance.c */
#include <stdlib.h>
#include "print_output.h"
#include "common.h"
#include "settings.h"
#include "logger.h"
#include "g-function.h"

extern settings_t* g_settings;
//extern logger_t* g_log;
extern logger_t* g_daily_log;
extern logger_t* g_monthly_log;
extern logger_t* g_annual_log;
extern logger_t* g_soil_log;

extern int MonthLength [];
extern int MonthLength_Leap [];

void EOD_print_cumulative_balance_cell_level(cell_t *const c, const int day, const int month, const int year, const int years_of_simulation )
{
	int layer;
	int height;
	int age;
	int species;

	static int years_counter;

	if (c->doy == 1)

	//FIXME this is just an approach

	//qsort(c->heights, c->heights_count, sizeof(height_t), sort_by_heights_desc);

	/* heading */
	if ( !day && !month && !year )
	{
		logger(g_daily_log, "%s \t%2s \t%s", "YEAR", "MONTH", "DAY");
		/* print class level LAI values */
		for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
		{
			for ( height = c->heights_count - 1; height >= 0 ; --height )
			{
				if( layer == c->heights[height].height_z )
				{
					/* heading for layers */
					logger(g_daily_log,"\t%s", "-LAYER");
					logger(g_daily_log,"\t%s", "HEIGHT");

					for ( age = 0; age < c->heights[height].ages_count ; ++age )
					{
						for ( species = 0; species < c->heights[height].ages[age].species_count; ++species )
						{
							/* heading for height class */
							if( !height )
							{
								logger(g_daily_log, "\t%2s \t%6s \t%6s \t%10s \t%8s \t%8s \t%8s \t%8s \t%8s",
										"LAI-",
										"GPP(gC/m2)",
										"AR(gC/m2)",
										"NPP(gC/m2)",
										"ET(mm/m2)",
										"LE(W/m2)",
										"N_TREE",
										"ASW",
										"Res_C\n");
							}
							else logger(g_daily_log,"\t%2s", "LAI-");
						}
					}
				}
			}
		}
	}


	/* values */
	logger(g_daily_log, "%d \t%2d \t%4d",
			c->years[year].year,
			month+1,
			day+1);

	/* print class level LAI values */
	for ( layer = c->t_layers_count - 1; layer >= 0; --layer )
	{
		for ( height = c->heights_count - 1; height >= 0 ; --height )
		{
			if( layer == c->heights[height].height_z )
			{
				logger(g_daily_log,"\t%6d", layer);
				logger(g_daily_log,"\t%7.3g", c->heights[height].value);

				for ( age = 0; age < c->heights[height].ages_count ; ++age )
				{
					for ( species = 0; species < c->heights[height].ages[age].species_count; ++species )
					{
						if( !height)
						{
							logger(g_daily_log,"\t%2.3g \t%7.4g \t%10.2g \t%10.2g \t%12.2g \t%7.4g \t%7d \t%11.4g \t%11.4g\n",
									c->heights[height].ages[age].species[species].value[LAI],
									c->daily_gpp,
									c->daily_aut_resp,
									c->daily_npp_gC,
									c->daily_et,
									c->daily_latent_heat_flux,
									c->cell_n_trees,
									c->asw,
									c->heights[height].ages[age].species[species].value[RESERVE_C]);
						}
						else logger(g_daily_log,"\t%3.4f", c->heights[height].ages[age].species[species].value[LAI]);
					}
				}
			}
		}
	}



	//ALESSIOR at the end of simulation
	//logger(g_annual_log, "\n3D-CMCC Forest Ecosystem Model v."PROGRAM_VERSION"\n");
	//logger(g_daily_log, "\nrunned: "__DATE__" at "__TIME__"\n");
	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[11]) : (MonthLength[11] )) == c->doy )
	{
		++years_counter;

		if ( years_counter ==  years_of_simulation)
		{
			//ALESSIOR
			//logger(g_annual_log, "\n3D-CMCC Forest Ecosystem Model v."PROGRAM_VERSION"\n");
			logger(g_daily_log, "\nrunned: "__DATE__" at "__TIME__"\n");
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
		logger(g_monthly_log, "\t%6s \t%8s \t%8s \t%8s", "GPP(gC/m2d)", "AR(gC/m2d)", "NPP(gC/m2d)", "ET\n");
	}
	/* values */
	logger(g_monthly_log,"%d \t%3d \t%3d \t%10.4f \t%10.4f \t%10.4f \t%10.4f\n",
			c->years[year].year,
			month + 1,
			c->t_layers_count,
			c->monthly_gpp,
			c->monthly_aut_resp,
			c->monthly_npp_gC,
			c->monthly_et);

	//ALESSIOR at the end of simulation
	//logger(g_annual_log, "\n3D-CMCC Forest Ecosystem Model v."PROGRAM_VERSION"\n");
	//logger(g_monthly_log, "\nrunned: "__DATE__" at "__TIME__"\n");
	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[11]) : (MonthLength[11] )) == c->doy )
	{
		++years_counter;

		if ( years_counter ==  years_of_simulation)
		{
			//ALESSIOR
			//logger(g_annual_log, "\n3D-CMCC Forest Ecosystem Model v."PROGRAM_VERSION"\n");
			logger(g_monthly_log, "\nrunned: "__DATE__" at "__TIME__"\n");
		}
	}
}

void EOY_print_cumulative_balance_cell_level(cell_t *const c, const int year, const int years_of_simulation )
{
	static int years_counter;

	//FIXME this is just an approach

	/* heading */
	if ( !year )
	{
		logger(g_annual_log, "%s \t%2s", "YEAR", "LC");
		logger(g_annual_log, "\t%6s \t%8s \t%8s \t%8s \t%8s \t%8s", "GPP(gC/m2d)", "AR(gC/m2d)", "NPP(gC/m2d)", "Y(%)", "ET", "N_TREE\n");
	}
	/* values */
	logger(g_annual_log, "%d \t%3d \t%10.4f \t%10.4f \t%10.4f \t%10.4f \t%10.4f \t%10d\n",
			c->years[year].year,
			c->t_layers_count,
			c->annual_gpp,
			c->annual_aut_resp,
			c->annual_npp_gC,
			((c->annual_aut_resp/c->annual_gpp)*100),
			c->annual_et,
			c->cell_n_trees);

	++years_counter;

	if ( years_counter ==  years_of_simulation)
	{
		//ALESSIOR
		//logger(g_annual_log, "\n3D-CMCC Forest Ecosystem Model v."PROGRAM_VERSION"\n");
		logger(g_annual_log, "\nrunned: "__DATE__" at "__TIME__"\n");
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
