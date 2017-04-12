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

	/*** Water Use Efficiency (Tree Level) ***/
	//note: this is based on tree level computation than uses npp and canopy transpiration

	/* daily WUE */
	if( s->value[NPP] > 0 && s->value[CANOPY_TRANSP] > 0.0 )
	{
		s->value[WUE] = s->value[NPP] / s->value[CANOPY_TRANSP];
	}
	else
	{
		s->value[WUE] = 0.0;
	}
	/* monthly WUE */
	/* last day of the month */
	if ( ( IS_LEAP_YEAR ( c->years[year].year ) ? ( MonthLength_Leap[month] ) : ( MonthLength[month] ) ) == c->doy )
	{
		if( s->value[MONTHLY_NPP] > 0 && s->value[MONTHLY_CANOPY_TRANSP] > 0.0 )
		{
			s->value[MONTHLY_WUE] = s->value[MONTHLY_NPP] / s->value[MONTHLY_CANOPY_TRANSP];
		}
		else
		{
			s->value[MONTHLY_WUE] = 0.0;
		}
	}
	/* annual WUE */
	/* last day of the year */
	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365) )
	{
		if( s->value[YEARLY_NPP] > 0 && s->value[YEARLY_CANOPY_TRANSP] > 0.0 )
		{
			s->value[YEARLY_WUE] = s->value[YEARLY_NPP] / s->value[YEARLY_CANOPY_TRANSP];
		}
		else
		{
			s->value[YEARLY_WUE] = 0.0;
		}
	}

	/*** Intrinsic Water Use Efficiency (Tree Level) ***/
	//note: this is based on tree level computation than uses gpp and canopy transpiration

	/* daily iWUE */
	if( s->value[GPP] > 0. && s->value[CANOPY_TRANSP] > 0. )
	{
		s->value[iWUE]       = s->value[GPP]       / s->value[CANOPY_TRANSP];
		s->value[iWUE_SUN]   = s->value[GPP_SUN]   / s->value[CANOPY_TRANSP_SUN];
		s->value[iWUE_SHADE] = s->value[GPP_SHADE] / s->value[CANOPY_TRANSP_SHADE];
	}
	else
	{
		s->value[iWUE]       = 0.;
		s->value[iWUE_SUN]   = 0.;
		s->value[iWUE_SHADE] = 0.;
	}
	/* monthly iWUE */
	/* last day of the month */
	if ( ( IS_LEAP_YEAR ( c->years[year].year ) ? ( MonthLength_Leap[month] ) : ( MonthLength[month] ) ) == c->doy )
	{
		if( s->value[MONTHLY_GPP] > 0. && s->value[MONTHLY_CANOPY_TRANSP] > 0. )
		{
			s->value[MONTHLY_iWUE]       = s->value[MONTHLY_GPP]       / s->value[MONTHLY_CANOPY_TRANSP];
			s->value[MONTHLY_iWUE_SUN]   = s->value[MONTHLY_GPP_SUN]   / s->value[MONTHLY_CANOPY_TRANSP_SUN];
			s->value[MONTHLY_iWUE_SHADE] = s->value[MONTHLY_GPP_SHADE] / s->value[MONTHLY_CANOPY_TRANSP_SHADE];
		}
		else
		{
			s->value[MONTHLY_iWUE]       = 0.;
			s->value[MONTHLY_iWUE_SUN]   = 0.;
			s->value[MONTHLY_iWUE_SHADE] = 0.;
		}
	}
	/* annual iWUE */
	/* last day of the year */
	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365) )
	{
		if( s->value[YEARLY_GPP] > 0. && s->value[YEARLY_CANOPY_TRANSP] > 0. )
		{
			s->value[YEARLY_iWUE]       = s->value[YEARLY_GPP]       / s->value[YEARLY_CANOPY_TRANSP];
			s->value[YEARLY_iWUE_SUN]   = s->value[YEARLY_GPP_SUN]   / s->value[YEARLY_CANOPY_TRANSP_SUN];
			s->value[YEARLY_iWUE_SHADE] = s->value[YEARLY_GPP_SHADE] / s->value[YEARLY_CANOPY_TRANSP_SHADE];
		}
		else
		{
			s->value[YEARLY_iWUE]       = 0.;
			s->value[YEARLY_iWUE_SUN]   = 0.;
			s->value[YEARLY_iWUE_SHADE] = 0.;
		}
	}


	/*** Intrinsic Water Use Efficiency (Cell Level) ***/
	//note: this is based on cell level computation than uses cell evapotranspiration

	/* daily iWUE */
	if( c->daily_gpp > 0 && c->daily_et > 0.0 )
	{
		c->daily_iwue = c->daily_gpp / c->daily_et;
	}
	else
	{
		c->daily_iwue = 0.0;
	}

	/* monthly iWUE */
	/* last day of the month */
	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[month] ) : (MonthLength[month] )) == c->doy )
	{
		if( c->monthly_gpp > 0 && c->monthly_et > 0.0 )
		{
			c->monthly_iwue = c->monthly_gpp / c->monthly_et;
		}
		else
		{
			c->monthly_iwue = 0.0;
		}
	}

	/* annual iWUE */
	/* last day of the year */
	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365) )
	{
		if( c->annual_gpp > 0 && c->annual_et > 0.0 )
		{
			c->annual_iwue = c->annual_gpp / c->annual_et;
		}
		else
		{
			c->annual_iwue = 0.0;
		}
	}
}
