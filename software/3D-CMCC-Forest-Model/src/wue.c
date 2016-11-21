/*
 * wue.c
 *
 *  Created on: 14/nov/2012
 *      Author: alessio
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "wue.h"
#include "common.h"

/* Last cumulative days in months */
extern int MonthLength [];
extern int MonthLength_Leap [];

void water_use_efficiency( cell_t *const c, const int height, const int dbh, const int age, const int species, const int day, const int month, const int year)
{

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* WATER USE EFFICIENCY */
	/* it computes daily, monthly and annual water use efficiency following:
	 * if considering ET
	 * Beer et al., 2009;
	 * Zhang et al., 2016 Agricultural and Forest Meteorology
	 * if considering Transpiration:
	 * Farquhar et al., 1989
	 * De Kauwe et al., 2013 GCB
	 */
#if 1
	/* daily WUE */
	if( s->value[DAILY_GPP_gC] > 0 && s->value[CANOPY_EVAPO_TRANSP] > 0.0 )
	{
		s->value[WUE] = s->value[DAILY_GPP_gC] / s->value[CANOPY_TRANSP];
	}
	else
	{
		s->value[WUE] = 0.0;
	}
	/* monthly WUE */
	/* last day of the month */
	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[month] ) : (MonthLength[month] )) == c->doy )
	{
		if( s->value[MONTHLY_GPP_gC] > 0 && s->value[MONTHLY_CANOPY_EVAPO_TRANSP] > 0.0 )
		{
			s->value[M_WUE] = s->value[MONTHLY_GPP_gC] / s->value[MONTHLY_CANOPY_TRANSP];
		}
		else
		{
			s->value[M_WUE] = 0.0;
		}
	}
	/* annual WUE */
	/* last day of the year */
	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365) )
	{
		if( s->value[YEARLY_GPP_gC] > 0 && s->value[YEARLY_CANOPY_EVAPO_TRANSP] > 0.0 )
		{
			s->value[Y_WUE] = s->value[YEARLY_GPP_gC] / s->value[YEARLY_CANOPY_TRANSP];
		}
		else
		{
			s->value[Y_WUE] = 0.0;
		}
	}
#else
	/* daily WUE */
	if( s->value[DAILY_GPP_gC] > 0 && s->value[CANOPY_EVAPO_TRANSP] > 0.0 )
	{
		s->value[WUE] = s->value[DAILY_GPP_gC] / s->value[CANOPY_EVAPO_TRANSP];
	}
	else
	{
		s->value[WUE] = 0.0;
	}

	/* monthly WUE */
	/* last day of the month */
	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[month] ) : (MonthLength[month] )) == c->doy )
	{
		if( s->value[MONTHLY_GPP_gC] > 0 && s->value[MONTHLY_CANOPY_EVAPO_TRANSP] > 0.0 )
		{
			s->value[M_WUE] = s->value[MONTHLY_GPP_gC] / s->value[MONTHLY_CANOPY_EVAPO_TRANSP];
		}
		else
		{
			s->value[M_WUE] = 0.0;
		}
	}

	/* annual WUE */
	/* last day of the year */
	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365) )
	{
		if( s->value[YEARLY_GPP_gC] > 0 && s->value[YEARLY_CANOPY_EVAPO_TRANSP] > 0.0 )
		{
			s->value[Y_WUE] = s->value[YEARLY_GPP_gC] / s->value[YEARLY_CANOPY_EVAPO_TRANSP];
		}
		else
		{
			s->value[Y_WUE] = 0.0;
		}
	}
#endif

}
