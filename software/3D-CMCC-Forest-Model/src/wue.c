/*
 * wue.c
 *
 *  Created on: 14/nov/2012
 *      Author: alessio
 */
// Modified by Saponaro Vincenzo 2024-2025 : computation of iWUE  


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "wue.h"
#include "common.h"
#include "constants.h"   // S.Vincenzo 
#include "met_data.h"    // S.Vincenzo
#include "matrix.h"     // S.Vincenzo
#include "aut_respiration.h" // S.Vincenzo

/* Last cumulative days in months */
extern int MonthLength [];
extern int MonthLength_Leap [];


void water_use_efficiency(cell_t *const c, meteo_daily_t *meteo_daily, const int height, const int dbh, const int age, const int species, const int day, const int month, const int year)
{

    double gs_mol = 0.;
    double gs_mol_sun = 0.;
    double gs_mol_shade = 0.;
    //double to_mol = 0.;
    //double net_ass_umolday = 0.;
    //double net_ass_umolsec = 0.;
    //double resp_umolday = 0.;
    //double resp_umolsec = 0.;
    //double gross_assimilation = 0.;
    //double gross_ass_umolday = 0.;
    //double gross_ass_umolsec = 0.;
    double conv = 0.;

    //conversion factor for conductance from m/s to mol/m2/sec Chen et al., 1999
    //Air pressure is in (Pa), R gas in (m3*Pa*K-1*mol-1), air temperature in K when summed to TempAbs) fon any doubts check constants.h
    conv = ( meteo_daily->air_pressure ) / ( Rgas * ( meteo_daily->tday + TempAbs ) );

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
	if( s->value[NPP] > 0. && s->value[CANOPY_TRANSP] > 0. )
	{
		s->value[WUE] = s->value[NPP] / s->value[CANOPY_TRANSP];
	}
	else
	{
		s->value[WUE] = 0.;
	}
	/* monthly WUE */
	/* last day of the month */
	if ( ( IS_LEAP_YEAR ( c->years[year].year ) ? ( MonthLength_Leap[month] ) : ( MonthLength[month] ) ) == c->doy )
	{
		if( s->value[MONTHLY_NPP] > 0. && s->value[MONTHLY_CANOPY_TRANSP] > 0. )
		{
			s->value[MONTHLY_WUE] = s->value[MONTHLY_NPP] / s->value[MONTHLY_CANOPY_TRANSP];
		}
		else
		{
			s->value[MONTHLY_WUE] = 0.;
		}
	}
	/* annual WUE */
	/* last day of the year */
	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365) )
	{
		if( s->value[YEARLY_NPP] > 0. && s->value[YEARLY_CANOPY_TRANSP] > 0. )
		{
			s->value[YEARLY_WUE] = s->value[YEARLY_NPP] / s->value[YEARLY_CANOPY_TRANSP];
		}
		else
		{
			s->value[YEARLY_WUE] = 0.;
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


	/*** Stomatal Water Use Efficiency (Tree Level) ***/
	//note: this is based on tree level computation that uses gpp and stomatal conductance

	/*// daily gsWUE
	if( s->value[GPP] > 0. && s->value[STOMATAL_CONDUCTANCE] > 0. )
	{
		s->value[gsWUE]       = s->value[GPP]       / s->value[STOMATAL_CONDUCTANCE];
		s->value[gsWUE_SUN]   = s->value[GPP_SUN]   / s->value[STOMATAL_SUN_CONDUCTANCE];
		s->value[gsWUE_SHADE] = s->value[GPP_SHADE] / s->value[STOMATAL_SHADE_CONDUCTANCE];
	}
	else
	{
		s->value[gsWUE]       = 0.;
		s->value[gsWUE_SUN]   = 0.;
		s->value[gsWUE_SHADE] = 0.;
	}
	//printf("gsWUE = %f\n", s->value[gsWUE]);
	printf("GPP = %f\n", s->value[NPP]);
	printf("gs = %f\n", s->value[STOMATAL_CONDUCTANCE]);
} */

/****************************************** SAPONARO FUNCTION iWUE ************************************/

//Stomatal Water Use Efficiency with net assimilation (umolCO2/molH2O) (Tree Level)
//note: this is based on tree level computation that uses NET assimilation (A_TOT) in umolCO2/m2 sec and stomatal conductance in mol/m2 sec
/*
if (s->counter[VEG_DAYS] > 0) {

  if (s->value[A_TOT] > 0. && s->value[STOMATAL_CONDUCTANCE] > 0.)
	{
	  //To convert to mol/m2 sec
    gs_mol = (s->value[STOMATAL_CONDUCTANCE] * conv);

         //To mol of CO2
         //to_mol = (s->value[ASSIMILATION] / GC_MOL);
         to_mol = (s->value[A_TOT] / GC_MOL);

                //From mol to micromol of CO2
                net_ass_umolday = (to_mol * 1e6);

                //To micromolCO2/m2 sec
                net_ass_umolsec = (net_ass_umolday / meteo_daily->daylength_sec);

             //umolCO2/molH2O
             s->value[gsWUE] = (net_ass_umolsec / gs_mol);

        //Attribution of values to variable in order to export them (check also in matrix.h)
		s->value[STOMATAL_CONDUCTANCE_molsec] = gs_mol;
		s->value[ASSIMILATION_umol] = net_ass_umolsec;
	}
}
else
{
		s->value[gsWUE] = 0;
    s->value[STOMATAL_CONDUCTANCE_molsec] = 0;
    s->value[ASSIMILATION_umol] = 0;
}
  //printf("A_TOT = %f\n", s->value[A_TOT]);
  //printf("psn = %f\n", s->value[PSN]);
  //printf("Assimilation = %f\n", s->value[ASSIMILATION]);
	//printf("gsWUE = %f\n", s->value[gsWUE]);
  //printf("net_ass_umolsec = %f\n", net_ass_umolsec);
  //printf("to_mol = %f\n", to_mol);
  //printf("net_ass_umolday = %f\n", net_ass_umolday);
	//printf("gs_mol = %f\n", gs_mol);
	//printf("gs_ms = %f\n", s->value[STOMATAL_CONDUCTANCE]);
  //printf("dailylenght_seconds in wue = %f\n", meteo_daily->daylength_sec);
}
*/

/****************************************** SAPONARO FUNCTION iWUE ************************************/

//Stomatal Water Use Efficiency with net assimilation (umolCO2/molH2O) (Tree Level)
//note: this is based on tree level computation that uses NET assimilation (A_TOT) in umolCO2/m2 sec and stomatal conductance in mol/m2 sec

//Daily iWUE
if (s->counter[VEG_DAYS] > 0) {

  if (s->value[A_TOT] > 0. && s->value[STOMATAL_CONDUCTANCE] > 0.)
	{
	  //To convert to mol/m2 sec
    gs_mol = s->value[STOMATAL_CONDUCTANCE] * conv;
    gs_mol_sun = s->value[STOMATAL_SUN_CONDUCTANCE] * conv;
    gs_mol_shade = s->value[STOMATAL_SHADE_CONDUCTANCE] * conv;

    //umolCO2/molH2O
    s->value[gsWUE_SUN] = s->value[A_SUN] / gs_mol_sun;
    s->value[gsWUE_SHADE] = s->value[A_SHADE] / gs_mol_shade;
    s->value[gsWUE] = s->value[A_TOT] / gs_mol;

    //Attribution of values to variable in order to export them (check also in matrix.h)
    s->value[GS_mol] = gs_mol;
    s->value[GS_mol_sun] = gs_mol_sun;
    s->value[GS_mol_shade] = gs_mol_shade;
	}
}
else
{
  s->value[gsWUE] = 0;
  s->value[gsWUE_SUN] = 0;
  s->value[gsWUE_SHADE] = 0;

 }
} //End iWUE

#if 0 
void water_use_efficiency_old( cell_t *const c, const int height, const int dbh, const int age, const int species, const int day, const int month, const int year)
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
	if( s->value[NPP] > 0. && s->value[CANOPY_TRANSP] > 0. )
	{
		s->value[WUE] = s->value[NPP] / s->value[CANOPY_TRANSP];
	}
	else
	{
		s->value[WUE] = 0.;
	}
	/* monthly WUE */
	/* last day of the month */
	if ( ( IS_LEAP_YEAR ( c->years[year].year ) ? ( MonthLength_Leap[month] ) : ( MonthLength[month] ) ) == c->doy )
	{
		if( s->value[MONTHLY_NPP] > 0. && s->value[MONTHLY_CANOPY_TRANSP] > 0. )
		{
			s->value[MONTHLY_WUE] = s->value[MONTHLY_NPP] / s->value[MONTHLY_CANOPY_TRANSP];
		}
		else
		{
			s->value[MONTHLY_WUE] = 0.;
		}
	}
	/* annual WUE */
	/* last day of the year */
	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365) )
	{
		if( s->value[YEARLY_NPP] > 0. && s->value[YEARLY_CANOPY_TRANSP] > 0. )
		{
			s->value[YEARLY_WUE] = s->value[YEARLY_NPP] / s->value[YEARLY_CANOPY_TRANSP];
		}
		else
		{
			s->value[YEARLY_WUE] = 0.;
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

	/*** Stomatal Water Use Efficiency (Tree Level) ***/
	//note: this is based on tree level computation than uses gpp and stomatal conductance

	/* daily gsWUE */
	if( s->value[GPP] > 0. && s->value[STOMATAL_CONDUCTANCE] > 0. )
	{
		s->value[gsWUE]       = s->value[GPP]       / s->value[STOMATAL_CONDUCTANCE];
		s->value[gsWUE_SUN]   = s->value[GPP_SUN]   / s->value[STOMATAL_SUN_CONDUCTANCE];
		s->value[gsWUE_SHADE] = s->value[GPP_SHADE] / s->value[STOMATAL_SHADE_CONDUCTANCE];
	}
	else
	{
		s->value[gsWUE]       = 0.;
		s->value[gsWUE_SUN]   = 0.;
		s->value[gsWUE_SHADE] = 0.;
	}
}
#endif

void cell_water_use_efficiency ( cell_t *const c, const int day, const int month, const int year )
{
	/*** Intrinsic Water Use Efficiency (Cell Level) ***/
	//note: this is based on cell level computation than uses cell evapotranspiration

	/* daily iWUE */
	if( c->daily_ass > 0 && c->daily_et > 0.0 )
	{
		c->daily_iwue = c->daily_ass / c->daily_et;
	}
	else
	{
		c->daily_iwue = 0.0;
	}

	/* monthly iWUE */
	/* last day of the month */
	if ( ( IS_LEAP_YEAR( c->years[year].year ) ? (MonthLength_Leap[month] ) : (MonthLength[month] )) == c->doy )
	{
		if( c->monthly_ass > 0 && c->monthly_et > 0.0 )
		{
			c->monthly_iwue = c->monthly_ass / c->monthly_et;
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
		if( c->annual_ass > 0 && c->annual_et > 0.0 )
		{
			c->annual_iwue = c->annual_ass / c->annual_et;
		}
		else
		{
			c->annual_iwue = 0.0;
		}
	}
}
