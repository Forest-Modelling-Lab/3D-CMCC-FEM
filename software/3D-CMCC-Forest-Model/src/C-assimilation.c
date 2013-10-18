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


void Get_carbon_assimilation (SPECIES *const s, CELL *const c, int years, int month, int day, int height)
{

	//float DailyNPP;
	//float MonthlyNPP;
	static float prev_negative_npp;


	Log ("\nGET_C-ASSIMILATION_ROUTINE\n");


	if (s->counter[VEG_UNVEG] == 1 && s->value[GPP_g_C] > 0.0)
	{

		Log("GPP = %g\n", s->value[GPP_g_C]);
		Log("Total aut respiration = %g\n", s->value[TOTAL_AUT_RESP]);
		Log("Fraction of respiration = %g %%\n", (s->value[TOTAL_AUT_RESP]*100.0)/s->value[GPP_g_C]);

		if (day == 0 && month == 0 && years == 0)
		{

			s->value[NPP_g_C] = s->value[GPP_g_C] - s->value[TOTAL_AUT_RESP];
		}
		else
		{
			//conservation of biomass
			/*
			if (s->value[NPP_g_C] < 0.0)
			{
				/used if previous day NPP is negative to maintain carbon balance
				Log("previous negative NPP day value = %g gCm^2yr \n", s->value[NPP_g_C]);
				s->value[NPP_g_C] -= (s->value[GPP_g_C] - s->value[TOTAL_AUT_RESP]);
				Log("-NPP day value = %g gCm^2yr \n", s->value[NPP_g_C]);
			}
			else
			{
				s->value[NPP_g_C] = s->value[GPP_g_C] - s->value[TOTAL_AUT_RESP];
			}
			*/
			s->value[NPP_g_C] = s->value[GPP_g_C] - s->value[TOTAL_AUT_RESP];
		}



		/*
		if (s->value[NPP_g_C] < 0.0)
		{
			s->value[NPP_g_C] = 0.0;
			s->value[NPP] = 0.0;
		}
		else
		{
			//upscale class NPP to class cell level
			s->value[NPP] = ((s->value[NPP_g_C] * GC_GDM) / 1000000) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);
		}
		*/

		//upscale class NPP to class cell level
		s->value[NPP] = ((s->value[NPP_g_C] * GC_GDM) / 1000000) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);
		//s->value[NPP] = (((s->value[GPP_g_C] * settings->sizeCell * GC_GDM)-(s->value[TOTAL_AUT_RESP])) / 1000000);



		if (settings->time == 'm')
		{
			//Monthly layer GPP in grams of C/m^2
			//Convert molC into grams
			Log("Monthly NPP = %g gC/m^2\n",  s->value[NPP_g_C]);
			Log("Monthly NPP = %g tDM/area\n", s->value[NPP]);

			//DailyNPP = s->value[NPP] / DaysInMonth;
			//Log("Daily GPP in grams of C for this layer = %g molC/m^2 day\n", DailyGPPgC);
		}
		else
		{
			//Daily layer GPP in grams of C/m^2
			//Convert molC into grams
			if (day == 0)
			{
				//MonthlyNPP = 0;
			}
			Log("Daily NPP = %g gC/m^2\n", s->value[NPP_g_C]);
			Log("Daily NPP = %g tDM/area\n",  s->value[NPP]);

			//MonthlyNPP += s->value[NPP];
			//Log("Monthly NPP (per area covered) for layer %d = %g tDM/area\n", c->heights[height].z, MonthlyNPP);
		}
	}
	else
	{
		s->value[NPP_g_C] = 0.0;
		s->value[NPP] = 0.0;
		Log("Daily/Monthly NPP = %g gC/m^2\n", s->value[NPP_g_C]);
		Log("Daily/Monthly NPP (per area covered) = %g  tDM/sizecell yr\n", s->value[NPP]);
	}


	if (c->annual_layer_number == 1)
	{
		c->daily_npp[0] += s->value[NPP];
		c->monthly_npp[0] += s->value[NPP];
		c->annual_npp[0] += s->value[NPP];
	}
	if (c->annual_layer_number == 2)
	{
		if (c->heights[height].z == 1)
		{
			c->daily_npp[1] += s->value[NPP];
			c->monthly_npp[1] += s->value[NPP];
			c->annual_npp[1] += s->value[NPP];
		}
		else
		{
			c->daily_npp[0] += s->value[NPP];
			c->monthly_npp[0] += s->value[NPP];
			c->annual_npp[0] += s->value[NPP];
		}
	}
	if (c->annual_layer_number == 3)
	{
		if (c->heights[height].z == 2)
		{
			c->daily_npp[2] += s->value[NPP];
			c->monthly_npp[2] += s->value[NPP];
			c->annual_npp[2] += s->value[NPP];
		}
		if (c->heights[height].z == 1)
		{
			c->daily_npp[1] += s->value[NPP];
			c->monthly_npp[1] += s->value[NPP];
			c->annual_npp[1] += s->value[NPP];
		}
		if (c->heights[height].z == 0)
		{
			c->daily_npp[0] += s->value[NPP];
			c->monthly_npp[0] += s->value[NPP];
			c->annual_npp[0] += s->value[NPP];
		}
	}

	c->daily_tot_npp += s->value[NPP];
	c->monthly_tot_npp += s->value[NPP];
	c->annual_tot_npp += s->value[NPP];

	Log("***************************** ANNUAL NPP *************************** \n");

	Log("*********************** CLASS LEVEL ANNUAL NPP ********************** \n");
	//class level
	s->value[YEARLY_NPP] += s->value[NPP];
	Log("-CLASS LEVEL\n");
	Log("-CLASS LEVEL Yearly NPP (per area covered) = %g tDM/sizecell yr\n", s->value[YEARLY_NPP]);

	Log("*********************** STAND LEVEL ANNUAL NPP ********************** \n");

	//cell level
	c->npp += s->value[NPP];
	Log("-CELL LEVEL\n");
	Log("-CELL LEVEL Yearly NPP (per area covered) = %g tDM/sizecell yr\n", c->npp);

}



