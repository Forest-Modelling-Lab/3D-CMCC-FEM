/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"
#include "logger.h"
#include "soil.h"
#include "topo.h"


extern logger_t* g_log;
extern soil_t *g_soil;
extern topo_t *g_topo;

void Radiation (SPECIES *const s, CELL *const c, const MET_DATA *const met, int years, int month, int day, int DaysInMonth, int height)
{

	double LightAbsorb, LightAbsorb_sun, LightAbsorb_shade;                               //fraction of light absorbed
	double LightTransm, LightTransm_sun, LightTransm_shade;                               //fraction of light transmitted
	double LightReflec_net_rad;                                                           //fraction of light reflected (for net radiation)
	double LightReflec_par;                                                               //fraction of light reflected (for par)
	double LightReflec_soil;

	double par;
	double par_abs;
	double par_abs_lai_sun, par_abs_lai_shade;
	double par_abs_per_lai_sun, par_abs_per_lai_shade;
	double par_trasm_lai_sun, par_trasm_lai_shade;
	double par_trasm_per_lai_sun, par_trasm_per_lai_shade;

	//double ppfd_coeff = 0.01;                                                             //parameter that quantifies the effect of light on conductance see Schwalm and Ek 2004 and Kimbal et al., 1997

	//following Ritchie et al., 1998 and Hydi et al., (submitted)
	double actual_albedo;

	//fixme move soil albedo into soil.txt file
	//test check albedo for other typos
	double soil_albedo = 0.17; //(see Wiki)

	double canopy_cover_eff;                                                             //effective canopy cover when LAI < 1

	double a = 107.0;                                                                    //(W/m)  empirical constants for long wave radiation computation
	double b = 0.2;                                                                      //(unit less) empirical constants for long wave radiation computation
	double lat_decimal;
	double lat_degrees;
	double lat_rad;
	double atmospheric_transmissivity;
	double atmospheric_emissivity;                                                       //emissivity of the clear-sky atmosphere

	double dr;                                                                           //inverse relative distance Earth-Sun
	double sigma;                                                                        //solar declination (radians)
	double omega_s;                                                                      //sunset hour angle
	int days_of_year;
	double TmaxK, TminK;

	logger(g_log, "\nLIGHT ROUTINE\n");

	/* compute fractions of light intercepted, transmitted and reflected from the canopy */
	/* fraction of light transmitted through the canopy */
	LightTransm = (exp(- s->value[K] * s->value[LAI]));
	LightTransm_sun = (exp(- s->value[K] * s->value[LAI_SUN]));
	LightTransm_shade = (exp(- s->value[K] * s->value[LAI_SHADE]));

	/* fraction of light absorbed by the canopy */
	LightAbsorb = 1.0 - LightTransm;
	LightAbsorb_sun = 1.0 - LightTransm_sun;
	LightAbsorb_shade = 1.0 - LightTransm_shade;

	/* fraction of light reflected by the canopy */
	/* for net radiation and par */
	//AS FOR PAR ALBEDO SHOULD BE TAKEN INTO ACCOUNT ONLY FOR SUN LEAVES THAT REPRESENT LAI_RATIO
	//following BIOME albedo for PAR is 1/3 of albedo
	//The absorbed PAR is calculated similarly except that albedo is 1/3 as large for PAR because less
	//PAR is reflected than net_radiation (Jones 1992)
	if(s->value[LAI] >= 1.0)
	{
		LightReflec_net_rad = s->value[ALBEDO];
		LightReflec_par = s->value[ALBEDO]/3.0;
	}
	else
	{
		LightReflec_net_rad = s->value[ALBEDO] * s->value[LAI];
		LightReflec_par = (s->value[ALBEDO]/3.0) * s->value[LAI];
	}

	logger(g_log, "LightAbsorb_sun = %f %%\n", LightAbsorb_sun);
	logger(g_log, "LightTrasm_sun = %f %%\n", LightTransm_sun);
	logger(g_log, "LightAbsorb_shade = %f %%\n", LightAbsorb_shade);
	logger(g_log, "LightTrasm_sun = %f %%\n", LightTransm_shade);
	logger(g_log, "LightReflec_net_rad = %f %%\n", LightReflec_net_rad);
	logger(g_log, "LightReflec_par = %f %%\n", LightReflec_par);

	/* fraction of light reflected by the soil */
	LightReflec_soil = soil_albedo;
	logger(g_log, "LightReflec_soil = %f %%\n", LightReflec_par);

	//test from where does this stuff come?
	//fixme LAI values should integrated over the cell considering different
	actual_albedo = LightReflec_net_rad * (LightReflec_net_rad-LightReflec_soil * exp(-0.75 * s->value[LAI]));
	//logger(g_log, "actual_albedo = %f\n", actual_albedo);

	TmaxK = met[month].d[day].tmax + TempAbs;
	TminK = met[month].d[day].tmin + TempAbs;

	if(IS_LEAP_YEAR(years))
	{
		days_of_year = 366;
	}
	else
	{
		days_of_year = 365;
	}

	/* Following Allen et al., 1998 */
	/* convert latitude in radians */
	lat_decimal = g_soil->values[SOIL_LAT] - (int)g_soil->values[SOIL_LAT];
	lat_degrees = (int)g_soil->values[SOIL_LAT] + (lat_decimal/60.0);
	lat_rad = (Pi/180.0)*lat_degrees;
	//logger(g_log, "lat_rad = %f\n", lat_rad);

	/* compute inverse relative distance Earth-Sun */
	dr = 1.0 + 0.033 * cos(((2*Pi)/days_of_year)*c->doy);
	//logger(g_log, "dr = %f\n", dr);

	/* compute solar declination */
	sigma = 0.409 * sin((((2*Pi)/days_of_year)*c->doy)-1.39);
	//logger(g_log, "sigma = %f\n",sigma);

	/* compute sunset hour angle */
	omega_s = acos((-tan(lat_rad) * tan(sigma)));
	//logger(g_log, "omega_s = %f\n", omega_s);

	/* compute atmospheric transmissivity */
	atmospheric_transmissivity = (0.75 + 2e-5 * g_topo->values[TOPO_ELEV]);
	logger(g_log, "atmospheric_transmissivity = %f\n", atmospheric_transmissivity);

	/* compute emissivity of the clear-sky atmosphere see Sun et al., 2013; Campbell and Normal 1998; Brutsaert, 1984; from Gao et al., 2008 instead 1.72 uses 1.24*/
	//fixme it should takes into account cloud cover
	atmospheric_emissivity = (1.72 * pow ((met[month].d[day].ea*10)/(met[month].d[day].tavg+TempAbs), 1.0/7.0));
	logger(g_log, "atmospheric_emissivity = %f\n", atmospheric_emissivity);

	//fixme cos(omega) should takes into account slope and aspect (once they will be included in "topo" files)
	//following Allen et al., 2006 Agric and Forest Meteorology (parag. 2)

	/* compute extra terrestrial radiation (MJ/m^2/day) */
	c->extra_terr_radiation_MJ = ((24.0*60.0)/Pi) * Q0_MJ * dr * ((omega_s * sin(lat_rad)* sin(sigma))+(cos(lat_rad)*cos(sigma)*sin(omega_s)));
	logger(g_log, "extra terrestrial radiation = %f (MJ/m^2/day)\n", c->extra_terr_radiation_MJ);

	/* convert into W/m2 */
	c->extra_terr_radiation_W = c->extra_terr_radiation_MJ * MJ_TO_W;
	logger(g_log, "extra terrestrial radiation = %f (W/m2)\n", c->extra_terr_radiation_W);


	/* RADIATION */
	/**************************************************************************/
	/* SHORT WAVE RADIATION */
	logger(g_log, "\nSHORT WAVE RADIATION\n");

	/* INCOMING SHORT WAVE RADIATION */
	logger(g_log, "\(incoming short wave)\n");

	/* compute short wave clear sky radiation (Tasumi et al., 2000)*/
	c->short_wave_clear_sky_radiation_MJ = atmospheric_transmissivity * c->extra_terr_radiation_MJ;
	logger(g_log, "Short wave clear_sky_radiation = %f (MJ/m^2/day)\n", c->short_wave_clear_sky_radiation_MJ);

	/* convert into W/m2 */
	c->short_wave_clear_sky_radiation_W = c->short_wave_clear_sky_radiation_MJ * MJ_TO_W;
	logger(g_log, "Short wave clear_sky_radiation = %f (W/m2)\n", c->short_wave_clear_sky_radiation_W);

	/* from input met data */
	//logger(g_log, "Solar_rad = %f MJ/m^2 day\n", met[month].d[day].solar_rad);

	c->short_wave_radiation_DW_MJ = met[month].d[day].solar_rad;
	logger(g_log, "Short_wave_radiation (downward) = %f MJ/m^2 day\n", c->short_wave_radiation_DW_MJ);

	/* convert into W/m2 */
	c->short_wave_radiation_DW_W = c->short_wave_radiation_DW_MJ * MJ_TO_W;
	logger(g_log, "Short wave radiation (downward) = %f W/m2\n", c->short_wave_radiation_DW_W);

	/* cloud cover fraction from Allen et al., 1998 */
	//note: Allen says that cloud_cover_frac must be li mited to 1.0
	c->cloud_cover_frac = (1.35*(c->short_wave_radiation_DW_MJ/c->short_wave_clear_sky_radiation_MJ)-0.35);
	if(c->cloud_cover_frac > 1.0) c->cloud_cover_frac = 1.0;
	logger(g_log, "cloud_cover_frac = %f %%\n", c->cloud_cover_frac * 100.0);
	logger(g_log, "Short wave radiation (downward) = %f W/m2\n", c->short_wave_radiation_DW_W);

	//fixme the light reflected should consider all reflected light through the cell (soil included)
	c->short_wave_radiation_UW_W = c->short_wave_radiation_DW_W * LightReflec_net_rad;
	logger(g_log, "Short wave radiation (upward) = %f W/m2\n", c->short_wave_radiation_UW_W);

	/* net short wave radiation */
	c->net_short_wave_radiation_W = c->short_wave_radiation_DW_W - c->short_wave_radiation_UW_W;
	logger(g_log, "Net Short wave radiation = %f W/m2\n", c->net_short_wave_radiation_W);


	/*****************************************************************************************/
	/* LONG WAVE RADIATION */
	logger(g_log, "\nLONG WAVE RADIATION\n");

	//	/* INCOMING LONG WAVE RADIATION */
	//	logger(g_log, "\(incoming long wave)\n");
	//
	//	//todo and for Cloudy Sky???
	//	/* following Sicart et al., 2006 */
	//	/* Downward long wave radiation (W/m2) for Clear Sky*/
	//	c->long_wave_radiation_DW_W = atmospheric_emissivity * SBC * pow((met[month].d[day].tavg+TempAbs), 4.0);
	//	logger(g_log, "Long wave radiation (downward) = %f W/m2\n", c->long_wave_radiation_DW_W);
	//
	//	/* convert into MJ/m^2 day */
	//	c->long_wave_radiation_DW_MJ = c->long_wave_radiation_DW_W * W_TO_MJ;
	//	logger(g_log, "Long wave radiation (downward) = %f MJ/m^2 day\n", c->long_wave_radiation_DW_MJ);
	//
	//
	//	/* OUTGOING LONG WAVE RADIATION */
	//	logger(g_log, "\(outgoing long wave)\n");
	//
	//	logger(g_log, "ea = %f\n", met[month].d[day].ea);
	//
	//	//fixme to avoid crash in model for negative "ea" values use different calculation of long_wave_radiation following Prentice (IT HAS TO BE SOLVED ANYWAY)
	//	if(met[month].d[day].ea < 0.0)
	//	{
	//		/* following Allen et al., 1998 */
	//		/* Upward long wave radiation (MJ/m2/day) */
	//		c->long_wave_radiation_UW_MJ = SBC_MJ * (((pow(TmaxK, 4)) + (pow(TminK,4)))/2.0)*(0.34-0.14*(sqrt(met[month].d[day].ea)))*c->cloud_cover_frac;
	//		logger(g_log, "Long wave radiation (upward) (Allen)= %f MJ/m^2 day\n", c->long_wave_radiation_UW_MJ);
	//
	//		/* convert into W/m2 */
	//		c->long_wave_radiation_UW_W = c->long_wave_radiation_UW_MJ * MJ_TO_W;
	//		logger(g_log, "Long wave radiation (upward) (Allen)= %f W/m2\n", c->long_wave_radiation_UW_W);
	//		/***********************************/
	//	}
	//	else
	//	{
	//		/* following Prentice et al., 1993 */
	//		/* Upward long wave radiation based on Monteith, 1973; Prentice et al., 1993; Linacre, 1986 */
	//		c->long_wave_radiation_UW_W = (b+(1.0-b)*c->ni)*(a - met[month].d[day].tavg);
	//		logger(g_log, "Long wave radiation (upward) (Prentice)= %f W/m2\n", c->long_wave_radiation_UW_W);
	//
	//		/* convert into MJ/m^2 day */
	//		c->long_wave_radiation_UW_MJ = c->long_wave_radiation_UW_W * W_TO_MJ;
	//		logger(g_log, "Long wave radiation (upward) (Prentice)= %f MJ/m^2 day\n", c->long_wave_radiation_UW_MJ);
	//		/*****************************************************************************************/
	//	}
	/* net radiation based on 3-PG method */
	//logger(g_log, "Net radiation using Qa and Qb = %f W/m2\n", QA + QB * (met[month].d[day].solar_rad * pow (10.0, 6)/86400.0));
	//logger(g_log, "Net radiation (3-PG method) = %f W/m2\n", c->net_radiation);
	//
	//	/* NET RADIATION */
	//	logger(g_log, "\nNET RADIATION\n");
	//
	//	//fixme IT MUST TAKES INTO ACCOUNT INCOMING LONG WAVE RADIATION
	//	c->net_radiation = c->net_short_wave_radiation_W - c->long_wave_radiation_UW_W;
	//	logger(g_log, "Net radiation = %f W/m2\n", c->net_radiation);


	logger(g_log, "ea = %f\n", met[month].d[day].ea);

	/* NET LONG WAVE RADIATION */
	logger(g_log, "\(net long wave)\n");

	//fixme to avoid crash in model for negative "ea" values use different calculation of long_wave_radiation following Prentice (IT HAS TO BE SOLVED ANYWAY)
	if(met[month].d[day].ea < 0.0)
	{
		/* following Allen et al., 1998 */
		/* Upward long wave radiation (MJ/m2/day) */
		c->net_long_wave_radiation_MJ = SBC_MJ * (((pow(TmaxK, 4)) + (pow(TminK,4)))/2.0)*(0.34-0.14*(sqrt(met[month].d[day].ea)))*c->cloud_cover_frac;
		logger(g_log, "Net Long wave radiation (Allen)= %f MJ/m^2 day\n", c->net_long_wave_radiation_MJ);

		/* convert into W/m2 */
		c->net_long_wave_radiation_W = c->net_long_wave_radiation_MJ * MJ_TO_W;
		logger(g_log, "Net Long wave radiation (Allen)= %f W/m2\n", c->net_long_wave_radiation_W);
		/***********************************/
	}
	else
	{
		//todo check it Prentice says "net upward long-wave flux"
		/* following Prentice et al., 1993 */
		/* Upward long wave radiation based on Monteith, 1973; Prentice et al., 1993; Linacre, 1986 */
		c->net_long_wave_radiation_W = (b+(1.0-b)*c->ni)*(a - met[month].d[day].tavg);
		logger(g_log, "Net Long wave radiation (Prentice)= %f W/m2\n", c->net_long_wave_radiation_W);

		/* convert into MJ/m^2 day */
		c->net_long_wave_radiation_MJ = c->net_long_wave_radiation_W * W_TO_MJ;
		logger(g_log, "Net Long wave radiation (Prentice)= %f MJ/m^2 day\n", c->net_long_wave_radiation_MJ);
		/*****************************************************************************************/
	}

	/* net radiation based on 3-PG method */
	//logger(g_log, "Net radiation using Qa and Qb = %f W/m2\n", QA + QB * (met[month].d[day].solar_rad * pow (10.0, 6)/86400.0));
	//logger(g_log, "Net radiation (3-PG method) = %f W/m2\n", c->net_radiation);

	/* NET RADIATION */
	logger(g_log, "\nNET RADIATION\n");

	//fixme DOES IT MUST TAKES INTO ACCOUNT INCOMING LONG WAVE RADIATION??
	c->net_radiation = c->net_short_wave_radiation_W - c->net_long_wave_radiation_W;
	logger(g_log, "Net radiation = %f W/m2\n", c->net_radiation);

	//fixme useless?
	if(c->net_radiation < 0.00000001)
	{
		c->net_radiation = 0.00000001;
		logger(g_log, "Net radiation = %f W/m2\n", c->net_radiation);
	}
	/*****************************************************************************************/

	/* PAR RADIATION */
	logger(g_log, "\nPAR RADIATION\n");

	//	/* convert MJ/m2/day to molPAR/m2/day (3-PG method)*/
	//	c->par = (c->short_wave_radiation_DW_MJ * MOLPAR_MJ);
	//	logger(g_log, "Par = %f molPAR/m^2 day\n", c->par);
	//test
	/* convert MJ/m2/day to molPAR/m2/day (Biome-BGC method)*/
	c->par = (c->short_wave_radiation_DW_MJ * RAD2PAR * EPAR);
	logger(g_log, "Par = %f molPAR/m^2 day\n", c->par);

	/* Remove the reflected PAR */
	c->par *= (1.0 - LightReflec_par);
	logger(g_log, "Par = %f molPAR/m^2 day\n", c->par);
	/*****************************************************************************************/

	/* PPFD RADIATION */
	logger(g_log, "\nPPFD RADIATION\n");

	/* compute PPFD (umol/m2/sec) */
	c->ppfd = c->short_wave_radiation_DW_W * RAD2PAR * EPAR;

	/* Remove the reflected PPFD */
	c->ppfd *= (1.0 - LightReflec_par);
	logger(g_log, "PPFD = %f umolPPFD/m2/sec\n", c->ppfd);
	/*****************************************************************************************/

	//FIXME CHECK LIGHT REFLECTED FOR DOMINATED LAYERS!!!

	/*if at least one class is in veg period*/
	if (c->Veg_Counter > 0.0)
	{
		/* compute light absorbed from dominant and transmitted to lower layers/soil layer */
		if ( c->heights[height].z == c->top_layer )
		{
			logger(g_log, "**LIGHT DOMINANT**\n");
			logger(g_log, "Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_dominant_counter);

			/*compute APAR (molPAR/m^2 day) for sun and shaded leaves*/
			s->value[APAR_SUN] = c->par * LightAbsorb_sun;
			s->value[TRANSM_PAR_SUN] = c->par - s->value[APAR_SUN];
			s->value[APAR_SHADE] = s->value[TRANSM_PAR_SUN] * LightAbsorb_shade;
			s->value[TRANSM_PAR_SHADE] = s->value[TRANSM_PAR_SUN] - s->value[APAR_SHADE];
			/* overall canopy */
			s->value[APAR] = s->value[APAR_SUN] + s->value[APAR_SHADE];
			s->value[TRANSM_PAR] = s->value[TRANSM_PAR_SHADE];

			logger(g_log, "\nINCOMING par = %f molPAR/m^2 day\n", c->par);
			logger(g_log, "Apar sun = %f molPAR/m^2 day\n", s->value[APAR_SUN]);
			logger(g_log, "Transmitted Par sun = %f molPAR/m^2 day\n", s->value[TRANSM_PAR_SUN]);
			logger(g_log, "Apar shade = %f molPAR/m^2 day\n", s->value[APAR_SHADE]);
			logger(g_log, "Transmitted Par shade = %f molPAR/m^2 day\n", s->value[TRANSM_PAR_SHADE]);
			logger(g_log, "Apar total = %f molPAR/m^2 day\n", s->value[APAR]);
			logger(g_log, "Transmitted Par total = %f molPAR/m^2 day\n", s->value[TRANSM_PAR]);
			CHECK_CONDITION(fabs((s->value[APAR] + s->value[TRANSM_PAR])-c->par),>1e-4);

			/*compute NetRad (W/m^2) for sun and shaded leaves*/
			s->value[NET_RAD_ABS_SUN] = c->net_radiation * LightAbsorb_sun;
			s->value[NET_RAD_TRANSM_SUN] = c->net_radiation - s->value[NET_RAD_ABS_SUN];
			s->value[NET_RAD_ABS_SHADE] = s->value[NET_RAD_TRANSM_SUN] * LightAbsorb_shade;
			s->value[NET_RAD_TRANSM_SHADE] = s->value[NET_RAD_TRANSM_SUN] - s->value[NET_RAD_ABS_SHADE];
			/* overall canopy */
			s->value[NET_RAD_ABS] = s->value[NET_RAD_ABS_SUN] + s->value[NET_RAD_ABS_SHADE];
			s->value[NET_RAD_TRANSM] = s->value[NET_RAD_TRANSM_SHADE];

			logger(g_log, "\nINCOMING net_radiation = %f W/m^2\n", c->net_radiation);;
			logger(g_log, "Absorbed NetRad sun = %f W/m^2\n", s->value[NET_RAD_ABS_SUN]);
			logger(g_log, "Transmitted NetRad sun = %f W/m^2\n", s->value[NET_RAD_TRANSM_SUN]);
			logger(g_log, "Absorbed NetRad shade = %f W/m^2\n", s->value[NET_RAD_ABS_SHADE]);
			logger(g_log, "Transmitted NetRad shade = %f W/m^2\n", s->value[NET_RAD_TRANSM_SHADE]);
			logger(g_log, "Absorbed total = %f W/m^2\n", s->value[NET_RAD_ABS]);
			logger(g_log, "Transmitted total = %f W/m^2\n", s->value[NET_RAD_TRANSM]);
			CHECK_CONDITION(fabs((s->value[NET_RAD_ABS] + s->value[NET_RAD_TRANSM])-c->net_radiation),>1e-4);

			//fixme COMPUTATION SHOULD BE INCLUDED ALSO FOR LOWER LAYERS
			/* compute PPFD (umol/m^2/sec) for sun and shaded leaves*/
			s->value[PPFD_ABS_SUN] = c->ppfd * LightAbsorb_sun;
			s->value[PPFD_TRANSM_SUN] = c->ppfd - s->value[PPFD_ABS_SUN];
			s->value[PPFD_ABS_SHADE] = s->value[PPFD_TRANSM_SUN] * LightAbsorb_shade;
			s->value[PPFD_TRANSM_SHADE] = s->value[PPFD_TRANSM_SUN] - s->value[PPFD_ABS_SHADE];
			/* overall canopy */
			s->value[PPFD_ABS] = s->value[PPFD_ABS_SUN] + s->value[PPFD_ABS_SHADE];
			s->value[PPFD_TRANSM] = s->value[PPFD_TRANSM_SHADE];

			logger(g_log, "\nINCOMING ppfd = %f umol/m2/sec\n", c->ppfd);
			logger(g_log, "Absorbed ppfd sun = %f umol/m2/sec\n", s->value[PPFD_ABS_SUN]);
			logger(g_log, "Transmitted ppfd sun = %f umol/m2/sec\n", s->value[PPFD_TRANSM_SUN]);
			logger(g_log, "Absorbed ppfd shade = %f umol/m2/sec\n", s->value[PPFD_ABS_SHADE]);
			logger(g_log, "Transmitted ppfd shade = %f umol/m2/sec\n", s->value[PPFD_TRANSM_SHADE]);
			logger(g_log, "Absorbed ppfd total  = %f umol/m2/sec\n", s->value[PPFD_ABS]);
			logger(g_log, "Transmitted ppfd total  = %f umol/m2/sec\n", s->value[PPFD_TRANSM]);
			CHECK_CONDITION(fabs((s->value[PPFD_ABS] + s->value[PPFD_TRANSM])-c->ppfd),>1e-4);

			/* it follows rationale of BIOME-BGC to obtain m2 instead m2/m2*/
			if(s->value[PPFD_ABS_SHADE] < 0.0)
			{
				s->value[PPFD_ABS_SHADE]  = 0.0;
			}
			if(s->value[LAI_SUN] > 0.0 && s->value[LAI_SHADE] > 0.0)
			{
				s->value[PPFD_ABS_SUN] /= s->value[LAI_SUN];
				s->value[PPFD_ABS_SHADE] /= s->value[LAI_SHADE];
			}
			else
			{
				s->value[PPFD_ABS_SUN] = s->value[PPFD_ABS_SHADE] = 0.0;

			}

			/* compute effective canopy cover */
			if(s->value[LAI] < 1.0)
			{
				/* special case when LAI = < 1.0 */
				canopy_cover_eff = s->value[LAI] * s->value[CANOPY_COVER_DBHDC];
			}
			else
			{
				canopy_cover_eff = s->value[CANOPY_COVER_DBHDC];
			}
			c->gapcover[c->top_layer] = 1.0 - canopy_cover_eff;

			/* check for the special case in which is allowed to have more 100% of grid cell covered */
			if(c->gapcover[c->top_layer] < 0.0)
			{
				c->gapcover[c->top_layer] = 0.0;
				logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer]);

				canopy_cover_eff = 1.0;
				logger(g_log, "Canopy Cover = %f\n", canopy_cover_eff);
			}

			//todo compute a weighted average Net Radiation, Par and PPFD absorbed and transmitted through the overall gridcell
			/*
			c->heights[height].Abs_par = ;
			c->heights[height].Transm_par = ;
			c->heights[height].Abs_net_rad = ;
			c->heights[height].Transm_net_rad = ;
			c->heights[height].Abs_ppfd = ;
			c->heights[height].Transm_ppfd = ;
			*/


			/* compute available light for lower/soil layer */
			/* soil layer below dominant layer */
			if(c->height_class_in_layer_dominated_counter == 0)
			{
				/* net radiation for soil computed as averaged value between covered and uncovered of dominant layer*/
				//fixme it should takes into account overall classes not only the last one
				c->net_radiation_for_soil = ((c->net_radiation - s->value[NET_RAD_ABS]) * (canopy_cover_eff * settings->sizeCell)
						+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
				/* remove reflected part */
				c->net_radiation_for_soil *= (1.0 - LightReflec_soil);
				logger(g_log, "Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_soil);

				/* PAR for soil computed as averaged value between covered and uncovered of dominant layer*/
				//fixme it should takes into account overall classes not only the last one
				c->par_for_soil = ((c->par - s->value[APAR]) * (canopy_cover_eff * settings->sizeCell)
						+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
				logger(g_log, "Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_soil);
			}
			/* other classes below dominant layer */
			else
			{
				/* net radiation for lower layer computed as averaged value between covered and uncovered of dominant layer */
				//fixme it sholud takes into account overall classes not only the last one
				c->net_radiation_for_dominated = ((c->net_radiation - s->value[NET_RAD_ABS]) * (canopy_cover_eff * settings->sizeCell)
						+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
				logger(g_log, "Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

				/* PAR for lower layer computed as averaged value between covered and uncovered of dominant layer */
				//fixme it should takes into account overall classes not only the last one
				c->par_for_dominated = ((c->par - s->value[APAR]) * (canopy_cover_eff * settings->sizeCell)
						+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
				logger(g_log, "Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
			}
		}

		//fixme set that if gapcover is bigger then 0.5 albedo should be considered also in dominated layer!!!!
		//fixme following MAESPA (Duursma et al.,) dominated layers should have just shaded leaves and  from Campbell&Norman (2000, p. 259)
		/* dominated layers */
		else
		{
			//			if (c->heights[height].z == c->top_layer - 1)
			//			{
			//				logger(g_log, "**LIGHT DOMINATED**\n");
			//				logger(g_log, "Height Classes in Dominated Layer = %d\n", c->height_class_in_layer_dominated_counter);
			//
			//				//Par
			//				c->par = c->par_for_dominated;
			//				logger(g_log, "Par = %f molPAR/m^2 day/month\n", c->par);
			//
			//				//Net Radiation
			//				c->net_radiation = c->net_radiation_for_dominated;
			//				logger(g_log, "Net Radiation = %f W/m^2n", c->net_radiation);
			//
			//				logger(g_log, "**BIOME APPROACH for dominated**\n");
			//				/*compute APAR for sun and shaded leaves*/
			//				//amount of total par that is reflected but leaves*/
			//				s->value[PAR] = c->par *(1.0 - LightReflec_par);
			//				s->value[APAR] = s->value[PAR] * LightAbsorb;
			//				s->value[APAR_SUN] = c->par * s->value[LAI_SUN] * s->value[K];
			//				s->value[APAR_SHADE] = s->value[APAR] - s->value[APAR_SUN];
			//				logger(g_log, "Par = %f molPAR/m^2 day/month\n", s->value[PAR]);
			//				logger(g_log, "Apar = %f molPAR/m^2 day/month\n", s->value[APAR]);
			//				logger(g_log, "Apar sun = %f molPAR/m^2 day/month\n", s->value[APAR_SUN]);
			//				logger(g_log, "Apar shade = %f molPAR/m^2 day/month\n", s->value[APAR_SHADE]);
			//
			//				/*compute NetRad for sun and shaded leaves*/
			//				//amount of Net Rad that is reflected but leaves*/
			//				s->value[NET_RAD] = c->net_radiation * (1.0 - LightReflec_net_rad);
			//				s->value[NET_RAD_ABS] = s->value[NET_RAD] * LightAbsorb;
			//				s->value[NET_RAD_ABS_SUN] = c->net_radiation * s->value[LAI_SUN] * s->value[K];
			//				s->value[NET_RAD_ABS_SHADE] = s->value[NET_RAD_ABS] - s->value[NET_RAD_ABS_SUN];
			//				logger(g_log, "NetRad = %f W/m^2\n", s->value[NET_RAD]);
			//				logger(g_log, "Absorbed NetRad = %f W/m^2\n", s->value[NET_RAD_ABS]);
			//				logger(g_log, "Absorbed NetRad sun = %f W/m^2\n", s->value[NET_RAD_ABS_SUN]);
			//				logger(g_log, "Absorbed NetRad shade = %f W/m^2\n", s->value[NET_RAD_ABS_SHADE]);
			//
			//				/*compute PPFD for sun and shaded leaves*/
			//
			//				//04/05/2016
			//				//TEST
			//				par = c->net_radiation * RAD2PAR * (1.0 - LightReflec_par);
			//				par_abs = par * LightAbsorb;
			//				par_abs_lai_sun = s->value[K]*par*s->value[LAI_SUN];
			//				par_abs_lai_shade = par_abs - par_abs_lai_sun;
			//				if(par_abs_lai_shade < 0.0)
			//				{
			//					par_abs_lai_sun = par_abs;
			//					par_abs_lai_shade = 0.0;
			//				}
			//				if(s->value[LAI_SUN] > 0.0 && s->value[LAI_SHADE] > 0.0)
			//				{
			//					par_abs_per_lai_sun = par_abs_lai_sun / s->value[LAI_SUN];
			//					par_abs_per_lai_shade = par_abs_lai_shade / s->value[LAI_SHADE];
			//				}
			//				else
			//				{
			//					par_abs_per_lai_sun = par_abs_per_lai_shade = 0.0;
			//				}
			//				s->value[PPFD_SUN] = par_abs_lai_sun * EPAR;
			//				s->value[PPFD_SHADE] = par_abs_lai_shade * EPAR;
			//				logger(g_log, "Absorbed PPFD = %f umol/m^2 sec\n", s->value[PPFD]);
			//				logger(g_log, "Absorbed PPFD sun = %f umol/m^2 sec\n", s->value[PPFD_SUN]);
			//				logger(g_log, "Absorbed PPFD shade = %f umol/m^2 sec\n", s->value[PPFD_SHADE]);
			//
			//				if ( c->dominated_veg_counter >= 3 )
			//				{
			//					if ( c->height_class_in_layer_dominated_counter == 1)    //only one height class in layer
			//					{
			//						logger(g_log, "Only one height class in layer dominated\n");
			//						c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
			//						logger(g_log, "gapcover layer %d  = %f\n", c->top_layer, c->gapcover[c->top_layer-1]);
			//						if(c->gapcover[c->top_layer - 1] < 0.0)
			//						{
			//							c->gapcover[c->top_layer - 1] = 0;
			//							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
			//						}
			//
			//						if (c->gapcover[c->top_layer] <= 0.5)
			//						{
			//							//test
			//							//no other classes
			//							if(c->height_class_in_layer_subdominated_counter == 0)
			//							{
			//								c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//										+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
			//								/* remove reflected part */
			//								c->net_radiation_for_soil *= (1.0 - LightReflec_soil);
			//								logger(g_log, "Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
			//								//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
			//								c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//										+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
			//								logger(g_log, "Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
			//							}
			//							else
			//							{
			//								//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
			//								c->net_radiation_for_subdominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//										+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
			//								logger(g_log, "Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);
			//
			//								//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
			//								c->par_for_subdominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//										+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
			//								logger(g_log, "Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
			//							}
			//						}
			//					}
			//					else  //many height classes in this layer
			//					{
			//						logger(g_log, "More height classes in layer dominated\n");
			//						if (c->dominated_veg_counter == 1 ) //first height class processes
			//						{
			//							c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
			//							logger(g_log, "gapcover layer %d  = %f\n", c->top_layer, c->gapcover[c->top_layer-1]);
			//							if(c->gapcover[c->top_layer - 1] < 0.0)
			//							{
			//								c->gapcover[c->top_layer - 1] = 0;
			//								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
			//							}
			//						}
			//						else if ( c->dominated_veg_counter > 1 && c->dominated_veg_counter < c->height_class_in_layer_dominated_counter )
			//						{
			//							logger(g_log, "Second and more height class processed\n");
			//							c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
			//							logger(g_log, "gapcover layer %d  = %f\n", c->top_layer, c->gapcover[c->top_layer-1]);
			//							if(c->gapcover[c->top_layer - 1] < 0.0)
			//							{
			//								c->gapcover[c->top_layer - 1] = 0;
			//								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
			//							}
			//						}
			//						else //last height
			//						{
			//							logger(g_log, "Last height class processed\n");
			//							//Gap_Cover -= s->value[CANOPY_COVER_DBHDC];
			//							//logger(g_log, "Free space in layer = %f\n", Gap_Cover);
			//
			//							if (c->dominant_veg_counter < c->height_class_in_layer_dominant_counter)
			//							{
			//								logger(g_log, "Second but not last height class processing....\n");
			//								c->gapcover[c->top_layer-1] -= s->value[CANOPY_COVER_DBHDC];
			//								if(c->gapcover[c->top_layer - 1] < 0.0)
			//								{
			//									c->gapcover[c->top_layer - 1] = 0;
			//									logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
			//								}
			//							}
			//							//FIXME NEW VERSION
			//							else
			//							{
			//								//test
			//								//no other classes
			//								if(c->height_class_in_layer_subdominated_counter == 0)
			//								{
			//									c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//											+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
			//									/* remove reflected part */
			//									c->net_radiation_for_soil *= (1.0 - LightReflec_soil);
			//									logger(g_log, "Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
			//									//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
			//									c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//											+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
			//									logger(g_log, "Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
			//								}
			//								else
			//								{
			//									//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
			//									c->net_radiation_for_subdominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//											+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
			//									logger(g_log, "Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);
			//
			//									//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
			//									c->par_for_subdominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//											+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
			//									logger(g_log, "Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
			//								}
			//							}
			//							logger(g_log, "Net Radiation for lower layer = %f  W/m^2\n", c->net_radiation_for_subdominated);
			//							logger(g_log, "Par for  lower layer = %f molPAR/m^2 day/month\n", c->par_for_subdominated);
			//						}
			//					}
			//				}
			//				else
			//				{
			//					if ( c->height_class_in_layer_dominated_counter == 1) //first height  class processed
			//					{
			//						c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
			//						logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-1]);
			//
			//						//test
			//						//no other classes
			//						if(c->height_class_in_layer_subdominated_counter == 0)
			//						{
			//							c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//									+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
			//							/* remove reflected part */
			//							c->net_radiation_for_soil *= (1.0 - LightReflec_soil);
			//							logger(g_log, "Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
			//							//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
			//							c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//									+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
			//							logger(g_log, "Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
			//						}
			//						else
			//						{
			//							//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
			//							c->net_radiation_for_subdominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//									+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
			//							logger(g_log, "Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);
			//
			//							//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
			//							c->par_for_subdominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//									+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
			//							logger(g_log, "Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
			//						}
			//					}
			//					else
			//					{
			//						logger(g_log, "More height classes in layer dominated\n");
			//						if (c->dominated_veg_counter == 1 ) //first height class processes
			//						{
			//							logger(g_log, "First height class processed\n");
			//							c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
			//							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-1]);
			//							if(c->gapcover[c->top_layer - 1] < 0.0)
			//							{
			//								c->gapcover[c->top_layer - 1] = 0;
			//								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
			//							}
			//						}
			//						else if (c->dominated_veg_counter > 1 && c->dominated_veg_counter < c->height_class_in_layer_dominated_counter)
			//						{
			//							logger(g_log, "Second and more height class processed\n");
			//							c->gapcover[c->top_layer-1] -= s->value[CANOPY_COVER_DBHDC];
			//							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-1]);
			//							if(c->gapcover[c->top_layer - 1] < 0.0)
			//							{
			//								c->gapcover[c->top_layer - 1] = 0;
			//								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
			//							}
			//						}
			//						else //last height
			//						{
			//							logger(g_log, "Last height class processed\n");
			//							c->gapcover[c->top_layer-1] -= s->value[CANOPY_COVER_DBHDC];
			//							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-1]);
			//							if(c->gapcover[c->top_layer - 1] < 0.0)
			//							{
			//								c->gapcover[c->top_layer - 1] = 0;
			//								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
			//							}
			//
			//							//test
			//							//no other classes
			//							if(c->height_class_in_layer_subdominated_counter == 0)
			//							{
			//								c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//										+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
			//								/* remove reflected part */
			//								c->net_radiation_for_soil *= (1.0 - LightReflec_soil);
			//								logger(g_log, "Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
			//								//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
			//								c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//										+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
			//								logger(g_log, "Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
			//							}
			//							else
			//							{
			//								//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
			//								c->net_radiation_for_subdominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//										+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
			//								logger(g_log, "Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);
			//
			//								//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
			//								c->par_for_subdominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//										+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
			//								logger(g_log, "Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
			//							}
			//						}
			//					}
			//				}
			//			}
			//			//subdominated layer
			//			else
			//			{
			//				logger(g_log, "**LIGHT SUB-DOMINATED**\n");
			//				logger(g_log, "Height Classes in Sub-Dominated Layer = %d\n", c->height_class_in_layer_subdominated_counter);
			//
			//
			//				logger(g_log, "**BIOME APPROACH for dominant**\n");
			//				/*compute APAR for sun and shaded leaves*/
			//				//amount of total par that is reflected but leaves*/
			//				s->value[PAR] = c->par *(1.0 - LightReflec_par);
			//				s->value[APAR] = s->value[PAR] * LightAbsorb;
			//				s->value[APAR_SUN] = c->par * s->value[LAI_SUN] * s->value[K];
			//				s->value[APAR_SHADE] = s->value[APAR] - s->value[APAR_SUN];
			//				logger(g_log, "Par = %f molPAR/m^2 day/month\n", s->value[PAR]);
			//				logger(g_log, "Apar = %f molPAR/m^2 day/month\n", s->value[APAR]);
			//				logger(g_log, "Apar sun = %f molPAR/m^2 day/month\n", s->value[APAR_SUN]);
			//				logger(g_log, "Apar shade = %f molPAR/m^2 day/month\n", s->value[APAR_SHADE]);
			//
			//				/*compute NetRad for sun and shaded leaves*/
			//				//amount of Net Rad that is reflected but leaves*/
			//				s->value[NET_RAD] = c->net_radiation * (1.0 - LightReflec_net_rad);
			//				s->value[NET_RAD_ABS] = s->value[NET_RAD] * LightAbsorb;
			//				s->value[NET_RAD_ABS_SUN] = c->net_radiation * s->value[LAI_SUN] * s->value[K];
			//				s->value[NET_RAD_ABS_SHADE] = s->value[NET_RAD_ABS] - s->value[NET_RAD_ABS_SUN];
			//				logger(g_log, "NetRad = %f W/m^2\n", s->value[NET_RAD]);
			//				logger(g_log, "Absorbed NetRad = %f W/m^2\n", s->value[NET_RAD_ABS]);
			//				logger(g_log, "Absorbed NetRad sun = %f W/m^2\n", s->value[NET_RAD_ABS_SUN]);
			//				logger(g_log, "Absorbed NetRad shade = %f W/m^2\n", s->value[NET_RAD_ABS_SHADE]);
			//				/*compute PPFD for sun and shaded leaves*/
			//
			//				//04/05/2016
			//				//TEST
			//				par = c->net_radiation * RAD2PAR * (1.0 - LightReflec_par);
			//				par_abs = par * LightAbsorb;
			//				par_abs_lai_sun = s->value[K]*par*s->value[LAI_SUN];
			//				par_abs_lai_shade = par_abs - par_abs_lai_sun;
			//				if(par_abs_lai_shade < 0.0)
			//				{
			//					par_abs_lai_sun = par_abs;
			//					par_abs_lai_shade = 0.0;
			//				}
			//				if(s->value[LAI_SUN] > 0.0 && s->value[LAI_SHADE] > 0.0)
			//				{
			//					par_abs_per_lai_sun = par_abs_lai_sun / s->value[LAI_SUN];
			//					par_abs_per_lai_shade = par_abs_lai_shade / s->value[LAI_SHADE];
			//				}
			//				else
			//				{
			//					par_abs_per_lai_sun = par_abs_per_lai_shade = 0.0;
			//				}
			//				s->value[PPFD_SUN] = par_abs_lai_sun * EPAR;
			//				s->value[PPFD_SHADE] = par_abs_lai_shade * EPAR;
			//				logger(g_log, "Absorbed PPFD = %f umol/m^2 sec\n", s->value[PPFD]);
			//				logger(g_log, "Absorbed PPFD sun = %f umol/m^2 sec\n", s->value[PPFD_SUN]);
			//				logger(g_log, "Absorbed PPFD shade = %f umol/m^2 sec\n", s->value[PPFD_SHADE]);
			//
			//				if ( c->heights_count >= 3 )
			//				{
			//					if ( c->height_class_in_layer_subdominated_counter == 1)
			//					{
			//						logger(g_log, "Only one height class in layer subdominated\n");
			//						//percentuale coperta
			//						c->gapcover[c->top_layer-2] = 1 - s->value[CANOPY_COVER_DBHDC];
			//						logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
			//						if(c->gapcover[c->top_layer - 2] < 0.0)
			//						{
			//							c->gapcover[c->top_layer - 2] = 0;
			//							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
			//						}
			//						//NON È IL VALORE IN METRO QUADRATO MA IL VALORE PER SUPERFICiE COPERTA
			//						c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//								+ (c->par_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell)))/settings->sizeCell;
			//						c->net_radiation_for_soil = ((c->net_radiation_for_subdominated * (1 - LightAbsorb) ) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
			//								+(c->net_radiation_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell)))/settings->sizeCell;
			//						/* remove reflected part */
			//						c->net_radiation_for_soil *= (1.0 - LightReflec_soil);
			//						logger(g_log, "Net radiation for soil = %fW/m^2\n", c->net_radiation_for_soil);
			//						logger(g_log, "Par for soil = %f molPAR/m^2 month\n", c->par_for_soil);
			//					}
			//					else
			//					{
			//						logger(g_log, "More height classes in layer subdominated\n");
			//						if (c->subdominated_veg_counter == 1 ) //first height class processed
			//						{
			//							logger(g_log, "First height class processed\n");
			//							c->gapcover[c->top_layer-2] = 1.0 - s->value[CANOPY_COVER_DBHDC];
			//							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
			//							if(c->gapcover[c->top_layer - 2] < 0.0)
			//							{
			//								c->gapcover[c->top_layer - 2] = 0;
			//								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
			//							}
			//							//Par for lower layer
			//						}
			//						else if ( c->subdominated_veg_counter > 1 && c->subdominated_veg_counter < c->height_class_in_layer_subdominated_counter )
			//						{
			//							logger(g_log, "Second and more height class processed\n");
			//							c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
			//							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
			//							if(c->gapcover[c->top_layer - 2] < 0.0)
			//							{
			//								c->gapcover[c->top_layer - 2] = 0;
			//								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
			//							}
			//						}
			//						else //last height
			//						{
			//							logger(g_log, "Last height class processed\n");
			//							c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
			//							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
			//							if(c->gapcover[c->top_layer - 2] < 0.0)
			//							{
			//								c->gapcover[c->top_layer - 2] = 0;
			//								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
			//							}
			//
			//							c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
			//							c->net_radiation_for_soil = ((c->net_radiation_for_subdominated * (1 - LightAbsorb) ) * ((1.0- c->gapcover[c->top_layer-2])
			//									* settings->sizeCell))/settings->sizeCell;
			//							logger(g_log, "Net radiation for soil = %fW/m^2\n", c->net_radiation_for_soil);
			//							logger(g_log, "Par for soil = %f molPAR/m^2 day/month\n", c->par_for_soil);
			//						}
			//					}
			//				}
			//				else
			//				{
			//					if ( c->height_class_in_layer_subdominated_counter == 1) //first height class processed
			//					{
			//						//percentuale coperta
			//						c->gapcover[c->top_layer-2] = 1.0 - s->value[CANOPY_COVER_DBHDC];
			//						if(c->gapcover[c->top_layer - 2] < 0.0)
			//						{
			//							c->gapcover[c->top_layer - 2] = 0;
			//							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
			//						}
			//						c->par_for_soil = (c->par_for_dominated - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);
			//
			//						//percentuale scoperta
			//						c->par_for_soil += (c->par_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell));
			//						c->par_for_soil /= settings->sizeCell;
			//						c->net_radiation_for_soil += c->net_radiation_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell);
			//						c->net_radiation_for_soil /= settings->sizeCell;
			//						logger(g_log, "Par for layer = %f molPAR/m^2 day/month\n", c->par_for_soil);
			//					}
			//					else
			//					{
			//						logger(g_log, "More height classes in layer dominated\n");
			//						if (c->subdominated_veg_counter == 1 ) //first height class processed
			//						{
			//							logger(g_log, "First height class processed\n");
			//							c->gapcover[c->top_layer-2] = 1.0 - s->value[CANOPY_COVER_DBHDC];
			//							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
			//							if(c->gapcover[c->top_layer - 2] < 0.0)
			//							{
			//								c->gapcover[c->top_layer - 2] = 0;
			//								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
			//							}
			//						}
			//						else if (c->subdominated_veg_counter > 1 && c->subdominated_veg_counter < c->height_class_in_layer_subdominated_counter)
			//						{
			//							logger(g_log, "Second and more height class processed\n");
			//							c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
			//							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
			//							if(c->gapcover[c->top_layer - 2] < 0.0)
			//							{
			//								c->gapcover[c->top_layer - 2] = 0;
			//								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
			//							}
			//						}
			//						else //last height
			//						{
			//							logger(g_log, "Last height class processed\n");
			//							c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
			//							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
			//							if(c->gapcover[c->top_layer - 2] < 0.0)
			//							{
			//								c->gapcover[c->top_layer - 2] = 0;
			//								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
			//							}
			//
			//							c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
			//							c->net_radiation_for_soil = ((c->net_radiation_for_subdominated * (1 - LightAbsorb) ) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
			//							/* remove reflected part */
			//							c->net_radiation_for_soil *= (1.0 - LightReflec_soil);
			//							logger(g_log, "Net radiation for soil = %fW/m^2\n", c->net_radiation_for_soil);
			//							logger(g_log, "Par for soil = %f molPAR/m^2 day/month\n", c->par_for_soil);
			//						}
			//					}
			//				}
			//			}
		}
	}
	else
	{
		/* net radiation for the soil outside growing season (bare soil condition) */
		c->net_radiation_for_soil = c->net_radiation * (1.0 - LightReflec_soil);
		logger(g_log, "Net Radiation for soil outside growing season = %f \n", c->net_radiation_for_soil);
	}
	if (c->heights[height].z == 0)
	{
		c->par_for_establishment = (c->par_for_soil / c->par);
		logger(g_log, "PAR OVER CANOPY = %f \n",  c->par);
		logger(g_log, "PAR FOR SOIL = %f \n",c->par_for_soil);
		logger(g_log, "Average Light Absorbed for establishment = %f \n", c->par_for_establishment);
	}
}

/**/
void Light_Recruitment (SPECIES *const s, double Av_Yearly_Par_Soil,  double av_yearly_daylength)
{



}
