/*photosynthesis.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


void Get_phosynthesis_monteith (SPECIES *const s, CELL *const c, int month, int day, int DaysInMonth, int height, int age, int species)
{
	float Alpha_C;
	float Epsilon;
	float Optimum_GPP;
	float Optimum_GPP_gC;
	float GPPmolC;
	float DailyGPPgC;
	float DailyNPP;
	static float MonthlyNPP;
	float DailyGPPmolC;
	float StandGPPtC;

	Log ("\nGET_PHOTOSYNTHESIS_ROUTINE\n\n");
	if (settings->time == 'm')
	{
		Log("************** at Month %d CARBON FLUX-PRODUCTIVITY ******************\n", month+1);
	}
	else
	{
		Log("************** at Day %d Month %d CARBON FLUX-PRODUCTIVITY ******************\n",day+1, month+1);
	}

	Log("VegUnveg = %d\n", s->counter[VEG_UNVEG]);
	//Log("Phenology = %g\n", s->value[PHENOLOGY]);


	//Log("X = %g \n", c->x);
	//Log("Y = %g \n", c->y);


	//Veg period
	if (s->counter[VEG_UNVEG] == 1 || (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2))
	{
		if (s->value[ALPHA] > 0.0)
		{
			//Log("ALPHA AVAILABLE - MODEL USE ALPHA QUANTUM CANOPY EFFICIENCY!!!!\n");

			Alpha_C = s->value[ALPHA] * s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD] * s->value[F_FROST];
			Log("Alpha C (Effective Quantum Canopy Efficiency)= %g molC/molPAR\n", Alpha_C);


			//convert epsilon from gCMJ^-1 to molCmolPAR^-1
			Epsilon = Alpha_C * MOLPAR_MJ * GC_MOL;
			//Log("Epsilon (LUE) = %g gDM/MJ\n", Epsilon);
		}
		else
		{
			Log("NO ALPHA - MODEL USE EPSILON LIGHT USE EFFICIENCY!!!!\n");

			Epsilon = s->value[EPSILONgCMJ] * s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD]  * s->value[F_FROST];
			Log("Epsilon (LUE) = %g gDM/MJ\n", Epsilon);

			Alpha_C = Epsilon / (MOLPAR_MJ * GC_MOL);
			Log("Alpha C = %g molC/molPAR\n", Alpha_C);
		}


		/*Productivity*/


		Log("**************************** GPP-'%c' ************************************ \n", settings->time);

		/*GPP*/
		Log("Apar for GPP = %g\n", s->value[APAR]);
		Optimum_GPP = s->value[ALPHA] * s->value[APAR];
		Log("Optimum GPP (alpha max * apar) = %g molC/m^2 day/month\n", Optimum_GPP);

		Optimum_GPP_gC = Optimum_GPP * GC_MOL;
		//Log("Daily/Monthly Optimum GPP in grams of C for this layer = %g gC/m^2 day/month\n", Optimum_GPP_gC );


		//Daily/Monthly GPP in mol of Carbon
		GPPmolC = s->value[APAR] * Alpha_C;
		//Log("Daily/Monthly GPP in mols of C for this layer = %g molC/m^2 day/month\n",  GPPmolC);
		//Log("Efficiency in GPP = %g %\n", (GPPmolC * 100) / Optimum_GPP);


		//Daily/Monthly layer GPP in grams of C/m^2
		//Convert molC into grams
		s->value[POINT_GPP_g_C] = GPPmolC * GC_MOL;
		//Log("day %d month %d Daily/Monthly GPP in grams of C (if CC = 100%) for layer %d = %g \n", day+1, month+1, c->heights[height].z , s->value[POINT_GPP_g_C] );




		//Yearly_GPP +=  GPPgC;
		//Log("Yearly Cumulated GPP  = %g gC/m^2\n",  Yearly_GPP);
		if (settings->time == 'm')
		{
			//Monthy layer GPP in grams of C/m^2
			//Convert molC into grams
			s->value[POINT_GPP_g_C] = GPPmolC * GC_MOL;
			Log("Monthly GPP in grams of C for layer %d = %g \n", c->heights[height].z , s->value[POINT_GPP_g_C] );
			DailyGPPgC = s->value[POINT_GPP_g_C] / DaysInMonth;
			//Log("Averaged Daily GPP in grams of C for this layer = %g molC/m^2 day\n", DailyGPPgC);
		}
		else
		{
			//Daily layer GPP in grams of C/m^2
			//Convert molC into grams
			//reset at the beginning of each month
			if (day == 0)
			{
				s->value[MONTHLY_GPP_g_C] = 0.0;
			}
			s->value[POINT_GPP_g_C] = GPPmolC * GC_MOL;
			Log("POINT_GPP_g_C day %d month %d Daily/Monthly GPP in grams of C for layer %d = %g \n", day+1, month+1, c->heights[height].z , s->value[POINT_GPP_g_C] );
			s->value[MONTHLY_GPP_g_C] += s->value[POINT_GPP_g_C];
			//Log("Monthly GPP in grams of C for layer %d = %g \n", c->heights[height].z , s->value[MONTHLY_GPP_g_C]);
		}




		s->value[GPP_g_C] =  s->value[POINT_GPP_g_C] * s->value[CANOPY_COVER_DBHDC];
		Log("GPP_g_C day %d month %d Daily/Monthly GPP in grams of C for layer %d = %g \n", day+1, month+1, c->heights[height].z , s->value[GPP_g_C] );
		//Log("CANOPY_COVER_DBHDC = %g\n", s->value[CANOPY_COVER_DBHDC]);
		//Log("Monthly  Stand GPP (per area covered)= %g gC/ha covered month\n", s->value[GPP_g_C]);

		//Monthly Stand (area covered by canopy) GPP in tonnes of C
		//StandGPPtC = s->value[POINT_GPP_g_C] / (1000000) /* * (settings->sizeCell * s->value[CANOPY_COVER_DBHDC])*/;
		//Log("Monthly  Stand GPP = %g tC/ha covered month\n", StandGPPtC);


		/*NPP*/

		Log("***************************** NPP-'%c' *************************** \n", settings->time);

		//Log("Assimilate Use Efficiency Y = %g\n", site->Y);

		//Monthly layer NPP
		// "*" GC_GDM to convert gC in DM
		// "/" 1000000 to convert gDM into tonsDM


		//todo move NPP calculation in a different source file and call it after photosynthesis>mainteinance_respiration>growth>respiration

		s->value[NPP] = ((s->value[GPP_g_C] * settings->sizeCell * GC_GDM * site->Y) / 1000000);    // assumes respiratory rate is constant
		s->value[NPP_g_C] = s->value[GPP_g_C] * site->Y;


		if (settings->time == 'm')
		{
			//Monthy layer GPP in grams of C/m^2
			//Convert molC into grams
			Log("Monthly NPP (per area covered) for layer %d = %g tDM/area\n", c->heights[height].z, s->value[NPP]);
			Log("Monthly NPP (per area covered) for layer %d = %g gC/m^2\n", c->heights[height].z, s->value[NPP_g_C]);
			DailyNPP = s->value[NPP] / DaysInMonth;
			//Log("Daily GPP in grams of C for this layer = %g molC/m^2 day\n", DailyGPPgC);
		}
		else
		{
			//Daily layer GPP in grams of C/m^2
			//Convert molC into grams
			if (day == 0)
			{
				MonthlyNPP = 0;
			}
			Log("Daily NPP (per area covered) for layer %d = %g tDM/area\n", c->heights[height].z , s->value[NPP]);
			Log("Daily NPP (per area covered) for layer %d = %g gC/m^2\n", c->heights[height].z, s->value[NPP_g_C]);
			MonthlyNPP += s->value[NPP];
			Log("Monthly NPP (per area covered) for layer %d = %g tDM/area\n", c->heights[height].z, MonthlyNPP);
		}
	}
	else if (s->counter[VEG_UNVEG] == 0)//Un Veg period
	{
		Log("Unvegetative period !! \n");
		s->value[GPP_g_C] = 0;
		s->value[POINT_GPP_g_C] = 0;
		Log("day %d month %d Daily/Monthly GPP in grams of C (if CC = 100%) for layer %d = %g \n", day+1, month+1, c->heights[height].z , s->value[POINT_GPP_g_C] );

		s->value[NPP] = 0;
		Log("Daily/Monthly NPP for layer %d = %g  tDM/sizecell yr\n", c->heights[height].z, s->value[NPP]);

		Log("Daily/Monthly Stand GPP (absolute) = %g gC/m^2 yr\n", s->value[POINT_GPP_g_C] );
		Log("Daily/Monthly Stand NPP (per area covered) = %g  tDM/sizecell yr\n", s->value[NPP]);
	}

	//TODO change all if with a for

	if (c->annual_layer_number == 1)
	{
		c->daily_gpp[0] += s->value[GPP_g_C];
		c->daily_npp[0] += s->value[NPP];
		c->monthly_gpp[0] += s->value[GPP_g_C];
		c->monthly_npp[0] += s->value[NPP];
		c->annual_gpp[0] += s->value[GPP_g_C];
		c->annual_npp[0] += s->value[NPP];
	}
	if (c->annual_layer_number == 2)
	{
		if (c->heights[height].z == 1)
		{
			c->daily_gpp[1] += s->value[GPP_g_C];
			c->daily_npp[1] += s->value[NPP];
			c->monthly_gpp[1] += s->value[GPP_g_C];
			c->monthly_npp[1] += s->value[NPP];
			c->annual_gpp[1] += s->value[GPP_g_C];
			c->annual_npp[1] += s->value[NPP];
		}
		else
		{
			c->daily_gpp[0] += s->value[GPP_g_C];
			c->daily_npp[0] += s->value[NPP];
			c->monthly_gpp[0] += s->value[GPP_g_C];
			c->monthly_npp[0] += s->value[NPP];
			c->annual_gpp[0] += s->value[GPP_g_C];
			c->annual_npp[0] += s->value[NPP];
		}
	}
	if (c->annual_layer_number == 3)
	{
		if (c->heights[height].z == 2)
		{
			c->daily_gpp[2] += s->value[GPP_g_C];
			c->daily_npp[2] += s->value[NPP];
			c->monthly_gpp[2] += s->value[GPP_g_C];
			c->monthly_npp[2] += s->value[NPP];
			c->annual_gpp[2] += s->value[GPP_g_C];
			c->annual_npp[2] += s->value[NPP];
		}
		if (c->heights[height].z == 1)
		{
			c->daily_gpp[1] += s->value[GPP_g_C];
			c->daily_npp[1] += s->value[NPP];
			c->monthly_gpp[1] += s->value[GPP_g_C];
			c->monthly_npp[1] += s->value[NPP];
			c->annual_gpp[1] += s->value[GPP_g_C];
			c->annual_npp[1] += s->value[NPP];
		}
		if (c->heights[height].z == 0)
		{
			c->daily_gpp[0] += s->value[GPP_g_C];
			c->daily_npp[0] += s->value[NPP];
			c->monthly_gpp[0] += s->value[GPP_g_C];
			c->monthly_npp[0] += s->value[NPP];
			c->annual_gpp[0] += s->value[GPP_g_C];
			c->annual_npp[0] += s->value[NPP];
		}
	}

	c->daily_tot_gpp += s->value[GPP_g_C];
	c->daily_tot_npp += s->value[NPP];

	c->monthly_tot_gpp += s->value[GPP_g_C];
	c->monthly_tot_npp += s->value[NPP];

	c->annual_tot_gpp += s->value[GPP_g_C];
	c->annual_tot_npp += s->value[NPP];

	Log("***************************** ANNUAL GPP-NPP *************************** \n");

	Log("*********************** CLASS LEVEL ANNUAL GPP-NPP ********************** \n");
	//class level
	s->value[YEARLY_POINT_GPP_G_C] += s->value[POINT_GPP_g_C];
	s->value[YEARLY_NPP] += s->value[NPP];
	Log("-CLASS LEVEL\n");
	Log("-CLASS LEVEL Yearly GPP (absolute) = %g gC/m^2 yr\n", s->value[YEARLY_POINT_GPP_G_C]);
	Log("-CLASS LEVEL Yearly NPP (per area covered) = %g tDM/sizecell yr\n", s->value[YEARLY_NPP]);

	Log("*********************** STAND LEVEL ANNUAL GPP-NPP ********************** \n");

	//cell level
	c->gpp += s->value[POINT_GPP_g_C];
	c->npp += s->value[NPP];
	Log("-CELL LEVEL\n");
	Log("-CELL LEVEL Yearly GPP (absolute) = %g gC/m^2 yr\n", c->gpp);
	Log("-CELL LEVEL Yearly NPP (per area covered) = %g tDM/sizecell yr\n", c->npp);

}
