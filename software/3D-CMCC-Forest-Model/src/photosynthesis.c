/*photosynthesis.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


void Get_phosynthesis_monteith (SPECIES *const s, CELL *const c, int month, int DaysInMonth, int height)
{
	float Alpha_C;
	float Epsilon;
	float Optimum_GPP;
	float Optimum_GPP_gC;
	float GPPmolC;
	float DailyGPPgC;
	//float DailyGPPmolC;
	float StandGPPtC;

	Log ("\nGET_PHOTOSYNTHESIS_ROUTINE\n\n");
	Log("************** at Month %d CARBON FLUX-PRODUCTIVITY ******************\n", month);

	Log("VegUnveg = %d\n", s->counter[VEG_UNVEG]);
	Log("Phenology = %g\n", s->value[PHENOLOGY]);


	//Log("X = %g \n", c->x);
	//Log("Y = %g \n", c->y);


	//Veg period
	if (s->counter[VEG_UNVEG] == 1 || s->value[PHENOLOGY] == 1)
	{
		if (s->value[ALPHA] > 0)
		{
			//Log("ALPHA AVAILABLE - MODEL USE ALPHA QUANTUM CANOPY EFFICIENCY!!!!\n");

			Alpha_C = s->value[ALPHA] * s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD] * s->value[F_FROST];
			Log("Alpha C (Effective Quantum Canopy Efficiency)= %g molC/molPAR\n", Alpha_C);

			Epsilon = Alpha_C * MOLPAR_MJ * GDM_MOL;
			//Log("Epsilon (LUE) = %g gDM/MJ\n", Epsilon);
		}
		else
		{
			Log("NO ALPHA - MODEL USE EPSILON LIGHT USE EFFICIENCY!!!!\n");

			Epsilon = s->value[EPSILONgCPAR] * s->value[F_NUTR] * s->value[F_T] * s->value[PHYS_MOD] /* * s->value[F_FROST]*/;
			Log("Epsilon (LUE) = %g gDM/MJ\n", Epsilon);

			Alpha_C = Epsilon / (MOLPAR_MJ * GDM_MOL);
			Log("Alpha C = %g molC/molPAR\n", Alpha_C);
		}


		/*Productivity*/


		Log("**************************** GPP ************************************ \n");

		/*GPP*/

		Optimum_GPP = s->value[ALPHA] * s->value[APAR];
		Log("Optimum GPP (alpha max * apar) = %g molC/m^2 month\n", Optimum_GPP);

		Optimum_GPP_gC = Optimum_GPP * GC_MOL;
		//Log("Monthly Optimum GPP in grams of C for this layer = %g gC/m^2 month\n", Optimum_GPP_gC );


		//Monthly GPP in mol of Carbon
		GPPmolC = s->value[APAR] * Alpha_C;
		//Log("Monthly GPP in mols of C for this layer = %g molC/m^2 month\n",  GPPmolC);


		//Log("Efficiency in GPP = %g %\n", (GPPmolC * 100) / Optimum_GPP);


		//DailyGPPmolC = GPPmolC / DaysInMonth;
		//Log("Daily GPP in mols of C for this layer = %g molC/m^2 day\n", DailyGPPmolC);

		//Monthy layer GPP in grams of C/m^2
		//Convert molC into grams
		s->value[POINT_GPP_g_C] = GPPmolC * GC_MOL;
		Log("Monthly GPP in grams of C for layer %d = %g \n", c->heights[height].z , s->value[POINT_GPP_g_C] );




		//Yearly_GPP +=  GPPgC;
		//Log("Yearly Cumulated GPP  = %g gC/m^2\n",  Yearly_GPP);


		DailyGPPgC = s->value[POINT_GPP_g_C] / DaysInMonth;
		//Log("Daily GPP in grams of C for this layer = %g molC/m^2 day\n", DailyGPPgC);


		//Monthly Stand (area covered by canopy) GPP in grams of C
		s->value[GPP_g_C] =  s->value[POINT_GPP_g_C]  * (settings->sizeCell * s->value[CANOPY_COVER_DBHDC]);
		//Log("CANOPY_COVER_DBHDC = %g\n", s->value[CANOPY_COVER_DBHDC]);
		//Log("Monthly  Stand GPP = %g gC/ha covered month\n", s->value[GPP_g_C]);

		//Monthly Stand (area covered by canopy) GPP in tonnes of C
		//StandGPPtC = s->value[POINT_GPP_g_C] / (1000000) /* * (settings->sizeCell * s->value[CANOPY_COVER_DBHDC])*/;
		//Log("Monthly  Stand GPP = %g tC/ha covered month\n", StandGPPtC);


		/*NPP*/

		Log("***************************** NPP *************************** \n");

		//Log("Assimilate Use Efficiency Y = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[Y]);

		//Monthly layer NPP
		// "*" 2 to convert gC in DM
		// "/" 1000000 to convert gDM into tonsDM

		//Log("saizsel = %g \n", settings->sizeCell);
		//Log("canopicover = %g \n", s->value[CANOPY_COVER_DBHDC]);


		s->value[NPP] = ((s->value[POINT_GPP_g_C] * (settings->sizeCell * s->value[CANOPY_COVER_DBHDC]) * 2 * site->Y)/1000000)  ;    // assumes respiratory rate is constant
		//Log("Respiration rate = %g \n", site->Y);
		Log("Monthly NPP for layer %d = %g \n", c->heights[height].z, s->value[NPP]);


		Log("Monthly Stand GPP = %g \n", s->value[POINT_GPP_g_C] );
		Log("Monthly Stand NPP = %g \n", s->value[NPP] );


	}
	else //Un Veg period
	{
		Log("Unvegetative period !! \n");
		s->value[GPP_g_C] = 0;
		s->value[POINT_GPP_g_C] = 0;
		Log("Monthly GPP in grams of C for layer %d = %g \n", c->heights[height].z , s->value[GPP_g_C]);

		s->value[NPP] = 0;
		Log("Monthly NPP for layer %d = %g \n", c->heights[height].z, s->value[NPP]);

		Log("Monthly Stand GPP = %g \n", s->value[POINT_GPP_g_C] );
		Log("Monthly Stand NPP = %g \n", s->value[NPP]);

	}






	//class level
	s->value[YEARLY_POINT_GPP_G_C] += s->value[POINT_GPP_g_C];
	s->value[YEARLY_NPP] += s->value[NPP];
	Log("CLASS LEVEL\n");
	Log("CLASS LEVEL Yearly GPP = %g\n", s->value[YEARLY_POINT_GPP_G_C]);
	Log("CLASS LEVEL Yearly NPP = %g\n", s->value[YEARLY_NPP]);


	//cell level
	c->gpp += s->value[POINT_GPP_g_C];
	c->npp += s->value[NPP];
	Log("CELL LEVEL\n");
	Log("CELL LEVEL Yearly GPP = %g\n", c->gpp);
	Log("CELL LEVEL Yearly NPP = %g\n", c->npp);

}
