/*
 * cue.c
 *
 *  Created on: 25/dic/2016
 *      Author: alessio-cmcc
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "wue.h"
#include "common.h"

/* Last cumulative days in months */
extern int MonthLength [];
extern int MonthLength_Leap [];

void carbon_use_efficiency( cell_t *const c, const int height, const int dbh, const int age, const int species, const int day, const int month, const int year)
{
	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* CARBON USE EFFICIENCY */
	/* it computes daily, monthly and annual carbon use efficiency following:
	DeLucia et al., GCB 2007
	*/
	/* daily CUE */
	if( s->value[NPP] > 0. && s->value[ASSIMILATION] > 0 )
	{
		s->value[CUE] = s->value[NPP] / s->value[ASSIMILATION];
	}
	else
	{
		s->value[CUE] = 0.0;
	}
	/* monthly CUE */
	/* last day of the month */
	if ( ( IS_LEAP_YEAR ( c->years[year].year ) ? ( MonthLength_Leap[month] ) : ( MonthLength[month] ) ) == c->doy )
	{
		if( s->value[MONTHLY_NPP] > 0 && s->value[MONTHLY_ASSIMILATION > 0.])
		{
			s->value[MONTHLY_CUE] = s->value[MONTHLY_NPP] / s->value[MONTHLY_ASSIMILATION];
		}
		else
		{
			s->value[MONTHLY_CUE] = 0.0;
		}
	}
	/* annual CUE */
	/* last day of the year */
	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365) )
	{
		if( s->value[YEARLY_NPP] > 0 && s->value[YEARLY_ASSIMILATION] > 0.)
		{
			s->value[YEARLY_CUE] = s->value[YEARLY_NPP] / s->value[YEARLY_ASSIMILATION];
		}
		else
		{
			s->value[YEARLY_CUE] = 0.0;
		}
	}
}
