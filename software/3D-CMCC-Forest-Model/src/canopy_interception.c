/*canopy_interception.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"




extern void Get_evapotranspiration (SPECIES *const s, CELL *c, const MET_DATA *const met, int month, int height)
{

	float RainIntercepted;
	float lessrain;
	//quantitatively computes the amount of rain intercepted by each strata

	if ( c->heights[height].z == c->top_layer)
	{
		//CANOPY INTERCEPTION
		if (s->value[LAIMAXINTCPTN] <= 0)
		{
			s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN];
			Log("Frac Rain Interception = MAXINTCPTN\n");
			Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
		}
		else
		{
			if (settings->version == 's')
			{
				s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , met[month].ndvi_lai / s->value[LAIMAXINTCPTN]);
			}
			else
			{
				s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , s->value[LAI] / s->value[LAIMAXINTCPTN]);
			}
			Log("Frac Rain Interception not use MAXINTCPTN\n");
			Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
		}

		//Rainfall intercepted
		//heighest height class
		//interception for the highest of the the dominant class
		//todo: RIFARE TUTTA QUESTA PARTE
		if (c->dominant_veg_counter == 1)
		{
			Log("Highest class\n");
			RainIntercepted = met[month].rain * s->value[FRAC_RAIN_INTERC]* s->value[CANOPY_COVER_DBHDC];
			Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
			Log("Percentage of Rain Intercepted from dominant canopy = %g%%\n", (RainIntercepted * 100) / met[month].rain );

			//control
			if (RainIntercepted >= met[month].rain)
			{
				RainIntercepted = met[month].rain;
				Log("Rain is completely intercepted by the highest height class in the dominant layer\n");
				lessrain = 0;
			}
			else
			{
				lessrain = met[month].rain - RainIntercepted;
			}

			//Evapotranspiration
			c->evapotranspiration = ((met[month].rain * s->value[FRAC_RAIN_INTERC] + s->value[MONTH_TRANSP]) * s->value[CANOPY_COVER_DBHDC]);
			Log("Class evapotranspiration = %g\,", c->evapotranspiration):

		}
		//top layer but not highest tree height class
		else
		{
			Log("Less Rain = %g mm\n", lessrain);
			if (lessrain <= 0)
			{
				Log("Rainfall is completely intercepted from the upper layer\n");
			}
			else
			{
				RainIntercepted = lessrain * s->value[FRAC_RAIN_INTERC] * s->value[CANOPY_COVER_DBHDC];
				Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
			}


			c->evapotranspiration += ((RainIntercepted * s->value[FRAC_RAIN_INTERC] + s->value[MONTH_TRANSP]) * s->value[CANOPY_COVER_DBHDC]);
			Log("Class evapotranspiration = %g\,", c->evapotranspiration):

			lessrain -= RainIntercepted;


		}
	}
	//dominated layers
	else if (c->heights[height].z == c->top_layer - 1)
	{
		//CANOPY INTERCEPTION
		if (s->value[LAIMAXINTCPTN] <= 0)
		{
			s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN];
			Log("Frac Rain Interception = MAXINTCPTN\n");
			Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
		}
		else
		{
			if (settings->version == 's')
			{
				s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , met[month].ndvi_lai / s->value[LAIMAXINTCPTN]);
			}
			else
			{
				s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , s->value[LAI] / s->value[LAIMAXINTCPTN]);
			}
			Log("Frac Rain Interception not use MAXINTCPTN\n");
			Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
		}

		Log("Rain Interception rate for dominated layer = %g\n", s->value[FRAC_RAIN_INTERC] );

		if (lessrain > 0)
		{
			RainIntercepted = lessrain * s->value[FRAC_RAIN_INTERC] * s->value[CANOPY_COVER_DBHDC];
			Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
		}
		else
		{
			RainIntercepted = 0;
			Log("No Rainfall for this layer\n");
		}

		c->evapotranspiration += ((RainIntercepted * s->value[FRAC_RAIN_INTERC] + s->value[MONTH_TRANSP])* s->value[CANOPY_COVER_DBHDC]);
		Log("Class evapotranspiration = %g\,", c->evapotranspiration):
		lessrain -= RainIntercepted;
	}
	else
	{
		//CANOPY INTERCEPTION
		if (s->value[LAIMAXINTCPTN] <= 0)
		{
			s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN];
			Log("Frac Rain Interception = MAXINTCPTN\n");
			Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
		}
		else
		{
			if (settings->version == 's')
			{
				s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , met[month].ndvi_lai / s->value[LAIMAXINTCPTN]);
			}
			else
			{
				s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , s->value[LAI] / s->value[LAIMAXINTCPTN]);
			}
			Log("Frac Rain Interception not use MAXINTCPTN\n");
			Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
		}

		Log("Rain Interception rate for subdominated layer = %g\n", s->value[FRAC_RAIN_INTERC] );

		if (lessrain > 0)
		{
			RainIntercepted = lessrain * s->value[FRAC_RAIN_INTERC] * s->value[CANOPY_COVER_DBHDC];
			Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
		}
		else
		{
			RainIntercepted = 0;
			Log("No Rainfall for this layer\n");
		}

		c->evapotranspiration += ((RainIntercepted * s->value[FRAC_RAIN_INTERC] + s->value[MONTH_TRANSP]) * s->value[CANOPY_COVER_DBHDC]);
		Log("Class evapotranspiration = %g\,", c->evapotranspiration):
		lessrain -= RainIntercepted;

	}
	//add soil evaporation
	if (height == 0)
	{
		c->evapotranspiration += c->soil_evaporation;
		Log("Class evapotranspiration = %g\,", c->evapotranspiration):
	}

	//todo mettere nella func Get_canopye evapotrans
	s->value[MONTHLY_EVAPOTRANSPIRATION] += c->evapotranspiration;
	Log("Cumulated Evapotranspiration for this layer = %g mm\n", s->value[MONTHLY_EVAPOTRANSPIRATION]);

	c->total_yearly_evapotransipration += c->evapotranspiration;
	//Log("TOTAL Cumulated Evapotranspiration = %g mm\n",c->total_yearly_evapotransipration);

}

//compute fraction of rain intercepted by canopy

extern void Get_frac_canopy_interception (SPECIES *const s, const MET_DATA *const met, int month)
{

	Log("\n GET_CANOPY_INTERCEPTION_ROUTINE \n");

	//interception is a rate not a quantity


	//see also CLM model for rain interception
	/*
	Interception = 1 - exp (-0.5 * m->cells[cell].heights[height].ages[age].species[species].value[LAI])
	 */



	if (s->value[LAIMAXINTCPTN] <= 0)
	{
		s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN];
		Log("Frac Rain Interception = MAXINTCPTN\n");
		Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
	}
	else
	{
		if (settings->version == 's')
		{
			s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , met[month].ndvi_lai / s->value[LAIMAXINTCPTN]);
		}
		else
		{
			s->value[FRAC_RAIN_INTERC] = s->value[MAXINTCPTN] * Minimum ( 1 , s->value[LAI] / s->value[LAIMAXINTCPTN]);
		}
		Log("Frac Rain Interception not use MAXINTCPTN\n");
		Log("Frac Rain Interception = %g \n", s->value[FRAC_RAIN_INTERC]);
	}
}
