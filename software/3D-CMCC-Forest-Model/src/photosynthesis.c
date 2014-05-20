/*photosynthesis.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


void Get_phosynthesis_monteith (SPECIES *const s, CELL *const c, int month, int day, int DaysInMonth, int height, int age, int species)
{
	int i;
	double Alpha_C;
	double Epsilon;
	double Optimum_GPP;
	double Optimum_GPP_gC;
	double GPPmolC;
	double DailyGPPgC;
	double StandGPPtC;

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
	//Log("Phenology = %f\n", s->value[PHENOLOGY]);


	//Log("X = %f \n", c->x);
	//Log("Y = %f \n", c->y);


	//Veg period
	if (s->counter[VEG_UNVEG] == 1 || (s->value[PHENOLOGY] == 1.1 || s->value[PHENOLOGY] == 1.2))
	{
		if (s->value[ALPHA] > 0.0)
		{
			//Log("ALPHA AVAILABLE - MODEL USE ALPHA QUANTUM CANOPY EFFICIENCY!!!!\n");

			Alpha_C = s->value[ALPHA] * /* s->value[F_LIGHT] */ s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD] * s->value[F_FROST];
			Log("Alpha C (Effective Quantum Canopy Efficiency)= %f molC/molPAR\n", Alpha_C);


			//convert epsilon from gCMJ^-1 to molCmolPAR^-1
			Epsilon = Alpha_C * MOLPAR_MJ * GC_MOL;
			//Log("Epsilon (LUE) = %f gDM/MJ\n", Epsilon);
		}
		else
		{
			Log("NO ALPHA - MODEL USE EPSILON LIGHT USE EFFICIENCY!!!!\n");

			Epsilon = s->value[EPSILONgCMJ] * /*s->value[F_LIGHT]*/ s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD]* s->value[F_FROST];
			Log("Epsilon (LUE) = %f gDM/MJ\n", Epsilon);

			Alpha_C = Epsilon / (MOLPAR_MJ * GC_MOL);
			Log("Alpha C = %f molC/molPAR\n", Alpha_C);
		}

		Log("**************************** GPP-'%c' ************************************ \n", settings->time);

		/*GPP*/
		Log("Apar for GPP = %f\n", s->value[APAR]);
		Optimum_GPP = s->value[ALPHA] * s->value[APAR];
		Log("Optimum GPP (alpha max * apar) = %f molC/m^2 day/month\n", Optimum_GPP);

		Optimum_GPP_gC = Optimum_GPP * GC_MOL;
		//Log("Daily/Monthly Optimum GPP in grams of C for this layer = %f gC/m^2 day/month\n", Optimum_GPP_gC );


		//Daily/Monthly GPP in mol of Carbon
		GPPmolC = s->value[APAR] * Alpha_C;
		//Log("Daily/Monthly GPP in mols of C for this layer = %f molC/m^2 day/month\n",  GPPmolC);
		//Log("Efficiency in GPP = %f %\n", (GPPmolC * 100) / Optimum_GPP);


		//Daily/Monthly layer GPP in grams of C/m^2
		//Convert molC into grams
		s->value[POINT_GPP_g_C] = GPPmolC * GC_MOL;
		//Log("day %d month %d Daily/Monthly GPP in grams of C (if CC = 100%) for layer %d = %f \n", day+1, month+1, c->heights[height].z , s->value[POINT_GPP_g_C] );




		//Yearly_GPP +=  GPPgC;
		//Log("Yearly Cumulated GPP  = %f gC/m^2\n",  Yearly_GPP);
		if (settings->time == 'm')
		{
			//Monthy layer GPP in grams of C/m^2
			//Convert molC into grams
			s->value[POINT_GPP_g_C] = GPPmolC * GC_MOL;
			Log("Monthly GPP in grams of C for layer %d = %f \n", c->heights[height].z , s->value[POINT_GPP_g_C] );
			DailyGPPgC = s->value[POINT_GPP_g_C] / DaysInMonth;
			//Log("Averaged Daily GPP in grams of C for this layer = %f molC/m^2 day\n", DailyGPPgC);
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
			Log("POINT_GPP_g_C day %d month %d Daily/Monthly GPP in grams of C for layer %d = %f \n", day+1, month+1, c->heights[height].z , s->value[POINT_GPP_g_C] );
			s->value[MONTHLY_GPP_g_C] += s->value[POINT_GPP_g_C];
			//Log("Monthly GPP in grams of C for layer %d = %f \n", c->heights[height].z , s->value[MONTHLY_GPP_g_C]);
		}




		s->value[GPP_g_C] =  s->value[POINT_GPP_g_C] * s->value[CANOPY_COVER_DBHDC];
		Log("GPP_g_C day %d month %d Daily/Monthly GPP in grams of C for layer %d = %f \n", day+1, month+1, c->heights[height].z , s->value[GPP_g_C] );
	}
	else if (s->counter[VEG_UNVEG] == 0)//Un Veg period
	{
		Log("Un-vegetative period !! \n");
		s->value[GPP_g_C] = 0;
		s->value[POINT_GPP_g_C] = 0;
		Log("day %d month %d Daily/Monthly GPP in grams of C (if CC = 100%) for layer %d = %f \n", day+1, month+1, c->heights[height].z , s->value[POINT_GPP_g_C] );
	}

	i = c->heights[height].z;

	c->daily_gpp[i] += s->value[GPP_g_C];
	c->monthly_gpp[i] += s->value[GPP_g_C];
	c->annual_gpp[i] += s->value[GPP_g_C];

	c->daily_tot_gpp += s->value[GPP_g_C];
	c->monthly_tot_gpp += s->value[GPP_g_C];
	c->annual_tot_gpp += s->value[GPP_g_C];

	Log("***************************** ANNUAL GPP *************************** \n");

	Log("*********************** CLASS LEVEL ANNUAL GPP ********************** \n");
	//class level
	s->value[YEARLY_POINT_GPP_G_C] += s->value[GPP_g_C];
	Log("-CLASS LEVEL\n");
	Log("-CLASS LEVEL Yearly GPP (absolute) = %f gC/m^2 yr\n", s->value[YEARLY_POINT_GPP_G_C]);

	Log("*********************** STAND LEVEL ANNUAL GPP ********************** \n");

	//cell level
	c->gpp += s->value[GPP_g_C];
	Log("-CELL LEVEL\n");
	Log("-CELL LEVEL Yearly GPP (absolute) = %f gC/m^2 yr\n", c->gpp);



}
