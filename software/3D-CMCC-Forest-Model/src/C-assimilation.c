/*
 * C-assimilation.c
 *
 *  Created on: 14/ott/2013
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


void Carbon_assimilation (SPECIES *const s, CELL *const c, int years, int month, int day, int height)
{
	int i;

	Log ("\nGET_C-ASSIMILATION_ROUTINE\n");

	if (s->counter[VEG_UNVEG] == 1)
	{
		Log("GPP = %f gC m^2 day\n", s->value[GPP_g_C]);
		Log("Reserve biomass = %f\n", s->value[RESERVE]);
		Log("Total aut respiration = %f gC m^2 day \n", s->value[TOTAL_AUT_RESP]);
		if (s->value[RESERVE] > 0.0)
		{
			/*for principle of conservation of mass*/
			/*used if previous day NPP is negative to conserve mass assuming the loss of reserve*/
			if (s->value[C_FLUX] < 0.0 )
			{
				if (s->value[RESERVE] < 0.0)
				{
					ERROR(s->value[RESERVE],"s->value[RESERVE]");
				}
				s->value[NPP_g_C] = 0;
				s->value[NPP] = 0;
			}
			else
			{
				s->value[NPP_g_C] = s->value[GPP_g_C] - s->value[TOTAL_AUT_RESP];
				Log("Fraction of respiration = %f %%\n", (s->value[TOTAL_AUT_RESP]*100.0)/s->value[GPP_g_C]);
				Log("NPP_g_C = %f\n", s->value[NPP_g_C]);
				//upscale class NPP to class cell level
				s->value[NPP] = ((s->value[NPP_g_C] * GC_GDM) / 1000000) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);
				//s->value[NPP] = (((s->value[GPP_g_C] * settings->sizeCell * GC_GDM)-(s->value[TOTAL_AUT_RESP])) / 1000000);

			}
		}
		else
		{
			ERROR(s->value[RESERVE],"s->value[RESERVE]");
		}
		Log("Daily NPP = %f gC/m^2\n", s->value[NPP_g_C]);
		Log("Daily NPP = %f tDM/area\n",  s->value[NPP]);
		//MonthlyNPP += s->value[NPP];
		//Log("Monthly NPP (per area covered) for layer %d = %f tDM/area\n", c->heights[height].z, MonthlyNPP);

	}
	else
	{
		if (s->value[RESERVE] < 0.0)
		{
			ERROR(s->value[RESERVE],"s->value[RESERVE]");
		}
		s->value[NPP_g_C] = 0.0;
		s->value[NPP] = 0.0;
		Log("Daily/Monthly NPP = %f gC/m^2\n", s->value[NPP_g_C]);
		Log("Daily/Monthly NPP (per area covered) = %f  tDM/day\n", s->value[NPP]);
	}

	i = c->heights[height].z;
	c->layer_daily_npp_tDM[i] += s->value[NPP];
	c->layer_daily_npp_gC[i] += s->value[NPP_g_C];
	c->layer_monthly_npp_tDM[i] += s->value[NPP];
	c->layer_monthly_npp_gC[i] += s->value[NPP_g_C];
	c->layer_annual_npp_tDM[i] += s->value[NPP];
	c->layer_annual_npp_gC[i] += s->value[NPP_g_C];

	c->daily_npp_tDM += s->value[NPP];
	c->daily_npp_gC += s->value[NPP_g_C];
	c->monthly_npp_tDM += s->value[NPP];
	c->monthly_npp_gC += s->value[NPP_g_C];
	c->annual_npp_tDM += s->value[NPP];
	c->annual_npp_gC += s->value[NPP_g_C];

	Log("***************************** ANNUAL NPP *************************** \n");

	Log("*********************** CLASS LEVEL ANNUAL NPP ********************** \n");
	//class level
	s->value[YEARLY_NPP] += s->value[NPP];
	Log("-CLASS LEVEL\n");
	Log("-CLASS LEVEL Yearly NPP (per area covered) = %f tDM/sizecell yr\n", s->value[YEARLY_NPP]);

	Log("*********************** STAND LEVEL ANNUAL NPP ********************** \n");

	//cell level
	c->daily_npp_tDM += s->value[NPP];
	Log("-CELL LEVEL\n");
	Log("-CELL LEVEL Yearly NPP (per area covered) = %f tDM/sizecell yr\n", c->daily_npp_tDM);

}



