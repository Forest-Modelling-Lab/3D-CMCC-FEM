/* photosynthesis.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "photosynthesis.h"
#include "canopy_evapotranspiration.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_log;

void Phosynthesis (species_t *const s, cell_t *const c, const int month, const int day, const int DaysInMonth, const int height, const int age, const int species)
{
	int i;
	double Alpha_C;
	double Epsilon;
	double GPPmolC, GPPmolC_sun, GPPmolC_shaded, GPPmolC_tot;
	double leaf_cell_cover_eff;                                                                //fraction of square meter covered by leaf over the gridcell
//	double cell_coverage;

	logger(g_log, "\n**PHOTOSYNTHESIS**\n");

	logger(g_log, "VegUnveg = %d\n", s->counter[VEG_UNVEG]);


	//fixme in this case when LAI = 0 there's no respiration
//	/* compute effective canopy cover */
//	if(s->value[LAI] < 1.0)
//	{
//		/* special case when LAI = < 1.0 */
//		leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
//	}
//	else
//	{
//		leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];
//	}
//	/* check for the special case in which is allowed to have more 100% of grid cell covered */
//	if(leaf_cell_cover_eff > 1.0)
//	{
//		leaf_cell_cover_eff = 1.0;
//	}

	leaf_cell_cover_eff = s->value[CANOPY_COVER_DBHDC];
	if(leaf_cell_cover_eff > 1.0) leaf_cell_cover_eff = 1.0;

//	if(s->value[CANOPY_COVER_DBHDC] > 1.0)
//	{
//		cell_coverage = 1.0;
//	}
//	else
//	{
//		cell_coverage = s->value[CANOPY_COVER_DBHDC];
//	}

	//Veg period
	//fixme
	//photosynthesis should occurs only in the fraction of the days in which also daily transp occurs
	if (s->counter[VEG_UNVEG] == 1 /* && s->value[DAILY_TRANSP] != 0.0*/)
	{
		//test without F_CO2
		if (s->value[ALPHA] != NO_DATA)
		{
			Alpha_C = (s->value[ALPHA] * /* s->value[F_LIGHT] */ /* s->value[F_CO2] * */ s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD] * s->value[F_FROST])
					 /**s->value[FRAC_DAYTIME_TRANSP] */;
			logger(g_log, "Alpha C (Effective Quantum Canopy Efficiency)= %f molC/molPAR\n", Alpha_C);

			//convert epsilon from gCMJ^-1 to molCmolPAR^-1
			Epsilon = Alpha_C * MOLPAR_MJ * GC_MOL;
		}
		else
		{
			logger(g_log, "NO ALPHA - MODEL USE EPSILON LIGHT USE EFFICIENCY!!!!\n");

			Epsilon = s->value[EPSILONgCMJ] * /* s->value[F_LIGHT]*//* s->value[F_CO2] * */s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD]* s->value[F_FROST];
			logger(g_log, "Epsilon (LUE) = %f gDM/MJ\n", Epsilon);

			Alpha_C = Epsilon / (MOLPAR_MJ * GC_MOL);
			logger(g_log, "Alpha C = %f molC/molPAR\n", Alpha_C);
		}
		logger(g_log, "**************************** GPP-'%c' ************************************ \n", g_settings->time);

		//test 12 May 2016 test
		//GPP depends on canopy wet (no photosynthesis occurs if canopy is wet)
		//fraction depend on partial canopy wet
		//Alpha_C *= s->value[CANOPY_FRAC_DAY_TRANSP];

		/* GPP */
		logger(g_log, "Apar for GPP = %f\n", s->value[APAR]);

		//DailyGPP in mol of Carbon
		GPPmolC = s->value[APAR] * Alpha_C;
		GPPmolC_sun = s->value[APAR_SUN]* Alpha_C;
		GPPmolC_shaded = s->value[APAR_SHADE]* Alpha_C;
		GPPmolC_tot = GPPmolC_sun + GPPmolC_shaded;
		logger(g_log, "GPPmolC = %f molC/m^2 day/month\n", GPPmolC);
		logger(g_log, "GPPmolC_sun = %f molC/m^2 day/month\n", GPPmolC_sun);
		logger(g_log, "GPPmolC_shade = %f molC/m^2 day/month\n", GPPmolC_shaded);
		logger(g_log, "GPPmolC_tot = %f molC/m^2 day/month\n", GPPmolC_tot);

		CHECK_CONDITION(fabs(GPPmolC - GPPmolC_tot), > 1e-4);

		/* Daily GPP in grams of C/m^2 */
		/* Convert molC into grams */
		s->value[DAILY_POINT_GPP_gC] = GPPmolC_tot * GC_MOL;
		logger(g_log, "DAILY_POINT_GPP_gC = %f gC/m2/day \n", s->value[DAILY_POINT_GPP_gC] );

		/* it converts value of GPP gC/m2/day in gC/m2 area covered/day */
		//test 5 July 2016 without leaf_cell_cover_eff
		s->value[DAILY_GPP_gC] =  s->value[DAILY_POINT_GPP_gC] /* * leaf_cell_cover_eff*/;
		logger(g_log, "DAILY_GPP_gC = %f gC/m2 area covered/day\n", s->value[DAILY_GPP_gC]);
	}
	else //Un Veg period
	{
		logger(g_log, "Un-vegetative period !! \n");
		s->value[DAILY_GPP_gC] = 0;
		s->value[DAILY_POINT_GPP_gC] = 0;
		logger(g_log, "DAILY_GPP_gC = %f gC/m2 area covered/day\n", s->value[DAILY_GPP_gC]);
	}

	s->value[MONTHLY_GPP_gC] += s->value[DAILY_POINT_GPP_gC];

	i = c->heights[height].z;

	c->layer_daily_gpp[i] += s->value[DAILY_GPP_gC];
	c->layer_monthly_gpp[i] += s->value[DAILY_GPP_gC];
	c->layer_annual_gpp[i] += s->value[DAILY_GPP_gC];

	logger(g_log, "-CELL LEVEL\n");
	logger(g_log, "-CELL LEVEL Yearly GPP (absolute) = %f gC/m^2 area covered/yr\n", c->daily_gpp);
	c->daily_gpp += s->value[DAILY_GPP_gC];
	c->monthly_gpp += s->value[DAILY_GPP_gC];
	c->annual_gpp += s->value[DAILY_GPP_gC];

	logger(g_log, "***************************** ANNUAL GPP *************************** \n");

	logger(g_log, "*********************** CLASS LEVEL ANNUAL GPP ********************** \n");
	//class level
	s->value[YEARLY_POINT_GPP_gC] += s->value[DAILY_GPP_gC];
	logger(g_log, "-CLASS LEVEL\n");
	logger(g_log, "-CLASS LEVEL Yearly GPP (absolute) = %f gC/m^2 yr\n", s->value[YEARLY_POINT_GPP_gC]);

	logger(g_log, "*********************** STAND LEVEL ANNUAL GPP ********************** \n");
}
