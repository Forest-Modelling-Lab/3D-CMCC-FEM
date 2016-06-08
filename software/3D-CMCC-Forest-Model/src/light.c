/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"
#include "logger.h"
#include "soil.h"


extern logger_t* g_log;
extern soil_t *g_soil;

 void Radiation (SPECIES *const s, CELL *const c, const MET_DATA *const met, int years, int month, int day, int DaysInMonth, int height)
{

	double LightAbsorb, LightAbsorb_sun, LightAbsorb_shade;       //fraction of light absorbed
	double LightTransm, LightTransm_sun, LightTransm_shade;       //fraction of light transmitted
	double LightReflec, LightReflec_sun, LightReflec_shade;       //fraction of light reflected
	double LightReflec_soil;
	double a = 107.0;//(W/m)  empirical constants for long wave radiation computation
	double b = 0.2; //(unitless) empirical constants for long wave radiation computation
	double ni; //proportion of daylength

	/* for biome's ppfd */
	double par;
	double par_abs;
	double par_abs_lai_sun, par_abs_lai_shade;
	double par_abs_per_lai_sun, par_abs_per_lai_shade;
	double par_trasm_lai_sun, par_trasm_lai_shade;
	double par_trasm_per_lai_sun, par_trasm_per_lai_shade;

	double ppfd_coeff = 0.01; /* parameter that quantifies the effect of light on conductance see Schwalm and Ek 2004 and Kimbal et al., 1997 */

	//following Ritchie et al., 1998 and Hydi et al., (submitted)
	double actual_albedo;
	double soil_albedo = 0.17; //(see Wiki)


	double lat_decimal;
	double lat_degrees;
	double lat_rad;

	double dr;                                                     //inverse relative distance Earth-Sun
	double sigma;                                                  //solar declination (radians)
	double omega_s;                                                //sunset hour angle
	int days_of_year;

	if(IS_LEAP_YEAR(years))
	{
		days_of_year = 365;
	}
	else
	{
		days_of_year = 366;
	}


	logger(g_log, "\nRADIATION ROUTINE\n");


	/* Following Allen et al., 1998 */
	/* convert latitude in radians */
	lat_decimal = g_soil->values[SOIL_LAT] - (int)g_soil->values[SOIL_LAT];
	lat_degrees = (int)g_soil->values[SOIL_LAT] + (lat_decimal/60.0);
	lat_rad = (Pi/180.0)*lat_degrees;
	logger(g_log, "lat_rad = %f\n", lat_rad);

	/* compute inverse relative distance Earth-Sun */
	dr = 1.0 + 0.033 * cos(((2*Pi)/days_of_year)*c->doy);
	logger(g_log, "dr = %f\n", dr);

	/* compute solar declination */
	sigma = 0.409 * sin((((2*Pi)/days_of_year)*c->doy)-1.39);
	logger(g_log, "sigma = %f\n",sigma);

	/* compute sunset hour angle */
	omega_s = acos((-tan(lat_rad) * tan(sigma)));
	logger(g_log, "omega_s = %f\n", omega_s);

	/* compute extraterrestrial radiation (MJ/m^2/day) */
	c->extra_terr_radiation = ((24.0*60.0)/Pi) * Q0_MJ * dr * ((omega_s * sin(lat_rad)* sin(sigma))+(cos(lat_rad)*cos(sigma)*sin(omega_s)));
	logger(g_log, "extra terrestrial radiation = %f (W/m2)\n", c->extra_terr_radiation);

	if(month == 2)getchar();


	//test from where does this stuff come?
	LightReflec_soil = soil_albedo;
	//fixme LAI values should integrated over the cell considering different
	actual_albedo = LightReflec * (LightReflec-LightReflec_soil * exp(-0.75 * s->value[LAI]));
	//logger(g_log, "actual_albedo = %f\n", actual_albedo);



	/*proportion of day length*/
	ni = met[month].d[day].daylength/24.0;

	logger(g_log, "Solar_rad = %f MJ/m^2 day\n", met[month].d[day].solar_rad);


	/* RADIATION BALANCE */
	/* convert solar radiation from MJ/m2/day to W/m2 */
	c->short_wave_radiation_DW = met[month].d[day].solar_rad * pow (10.0, 6)/86400.0;
	logger(g_log, "Short wave radiation (downward) = %f W/m2\n", c->short_wave_radiation_DW);

	//test
	//fixme change albedo with variable LightReflect
	//fixme the light reflected should consider all reflected light through the cell (soil included)
	c->short_wave_radiation_UW = c->short_wave_radiation_DW * s->value[ALBEDO];
	logger(g_log, "Short wave radiation (upward) = %f W/m2\n", c->short_wave_radiation_UW);

	/* net short wave radiation */
	c->net_short_wave_radiation = c->short_wave_radiation_DW - c->short_wave_radiation_UW;
	logger(g_log, "Net Short wave radiation = %f W/m2\n", c->net_short_wave_radiation);

	/* Upward long wave radiation following Prentice et al., 1993, Linacre, 1986*/
	c->long_wave_radiation_UW = (b+(1.0-b)*ni)*(a - met[month].d[day].tavg);
	logger(g_log, "Long wave radiation (upward) = %f W/m2\n", c->long_wave_radiation_UW);

	/* being Rnet(Q0) = Sw-down - Sw-up + Lw-down - Lw-up */
	/* Downward long wave radiation */
	c->long_wave_radiation_DW = Q0 - c->short_wave_radiation_DW + c->short_wave_radiation_UW - c->long_wave_radiation_UW;
	logger (g_log, "Long wave radiation (downward) = %f W/m2\n", c->long_wave_radiation_DW);

	/* net long wave radiation */
	c->net_long_wave_radiation = c->short_wave_radiation_UW - c->long_wave_radiation_UW;
	logger(g_log, "Net Long wave radiation = %f W/m2\n", c->net_long_wave_radiation);

	CHECK_CONDITION((c->short_wave_radiation_DW - c->short_wave_radiation_UW + c->long_wave_radiation_DW - c->long_wave_radiation_UW), == Q0);

	logger(g_log, "PROVA Net radiation = %f W/m2\n", (c->short_wave_radiation_DW + c->short_wave_radiation_UW) - (c->long_wave_radiation_DW + c->long_wave_radiation_UW));


	c->net_radiation = c->short_wave_radiation_DW - c->long_wave_radiation_UW;
	//fixme is it correct to avoid negative values?
	logger(g_log, "Net radiation = %f W/m2\n", c->net_radiation);
	//fixme which one?
	logger(g_log, "Net radiation using Qa and Qb = %f W/m2\n", QA + QB * (met[month].d[day].solar_rad * pow (10.0, 6)/86400.0));


	/* convert MJ/m2/day to molPAR/m2/day */
	c->par = (met[month].d[day].solar_rad * MOLPAR_MJ);
	logger(g_log, "Par = %f molPAR/m^2 day\n", c->par);

	if(c->net_radiation < 0.00000001)
	{
		c->net_radiation = 0.00000001;
		logger(g_log, "Net radiation = %f W/m2\n", c->net_radiation);
	}

	/*if at least one class is in veg period*/
	if (c->Veg_Counter > 0.0)
	{

		/* fraction of light transmitted through the canopy */
		LightTransm = (exp(- s->value[K] * s->value[LAI]));
		LightTransm_sun = (exp(- s->value[K] * s->value[LAI_SUN]));
		LightTransm_shade = (exp(- s->value[K] * s->value[LAI_SHADE]));

		/* fraction of light absorbed by the canopy */
		LightAbsorb = 1.0 - LightTransm;
		LightAbsorb_sun = 1.0 - LightTransm_sun;
		LightAbsorb_shade = 1.0 - LightTransm_shade;

		//fixme
		/* fraction of light reflected by the canopy */
		LightReflec = s->value[ALBEDO];
		LightReflec_sun = s->value[ALBEDO];
		LightReflec_shade = s->value[ALBEDO];

		logger(g_log, "LightAbsorb_sun = %f\n", LightAbsorb_sun);
		logger(g_log, "LightTrasm_sun = %f\n", LightTransm_sun);
		logger(g_log, "LightAbsorb_shade = %f\n", LightAbsorb_shade);
		logger(g_log, "LightTrasm_sun = %f\n", LightTransm_shade);


		//LIGHT DOMINANT
		if ( c->heights[height].z == c->top_layer )
		{
			logger(g_log, "**LIGHT DOMINANT**\n");
			logger(g_log, "Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_dominant_counter);

			//AS FOR PAR ALBEDO SHOULD BE TAKEN INTO ACCOUNT ONLY FOR SUN LEAVES THAT REPRESENT LAI_RATIO
			//following BIOME albedo for PAR is 1/3 of albedo
			//The absorbed PAR is calculated similarly except that albedo is 1/3 as large for PAR because less
			//PAR is reflected than net_radiation (Jones 1992)

			logger(g_log, "**BIOME APPROACH for dominant**\n");
			/*compute APAR for sun and shaded leaves*/
			//amount of total par that is reflected by the leaves*/

			s->value[PAR] = c->par *(1.0 - (LightReflec/(3.0)));
			s->value[APAR] = s->value[PAR] * LightAbsorb;
			s->value[APAR_SUN] = s->value[PAR] * LightAbsorb_sun;
			s->value[TRASM_PAR_SUN] = s->value[PAR] - s->value[APAR_SUN];
			s->value[APAR_SHADE] = s->value[TRASM_PAR_SUN] * LightAbsorb_shade;
			s->value[TRASM_PAR_SHADE] = s->value[TRASM_PAR_SUN] - s->value[APAR_SHADE];

			logger(g_log, "INCOMING par = %f molPAR/m^2 day\n", c->par);
			logger(g_log, "Par = %f molPAR/m^2 day\n", s->value[PAR]);
			logger(g_log, "Apar sun = %f molPAR/m^2 day\n", s->value[APAR_SUN]);
			logger(g_log, "Trasmitted Par sun = %f molPAR/m^2 day\n", s->value[TRASM_PAR_SUN]);
			logger(g_log, "Apar shade = %f molPAR/m^2 day\n", s->value[APAR_SHADE]);
			logger(g_log, "Trasmitted Par shade = %f molPAR/m^2 day\n", s->value[TRASM_PAR_SHADE]);

			/*compute NetRad for sun and shaded leaves*/
			//amount of Net Rad that is reflected but leaves*/

			//test 11 May 2016 test
			s->value[NET_RAD] = c->net_radiation * (1.0 - LightReflec);
			s->value[NET_RAD_ABS] = s->value[NET_RAD] * LightAbsorb;
			s->value[NET_RAD_ABS_SUN] = s->value[NET_RAD] * LightAbsorb_sun;
			s->value[NET_RAD_TRASM_SUN] = s->value[NET_RAD] - s->value[NET_RAD_ABS_SUN];
			s->value[NET_RAD_ABS_SHADE] = s->value[NET_RAD_TRASM_SUN] * LightAbsorb_shade;
			s->value[NET_RAD_TRASM_SHADE] = s->value[NET_RAD_TRASM_SUN] - s->value[NET_RAD_ABS_SHADE];
			logger(g_log, "INCOMING net_radiation = %f W/m^2\n", c->net_radiation);
			logger(g_log, "NetRad = %f W/m^2\n", s->value[NET_RAD]);
			logger(g_log, "Absorbed NetRad sun = %f W/m^2\n", s->value[NET_RAD_ABS_SUN]);
			logger(g_log, "Trasmitted NetRad sun = %f W/m^2\n", s->value[NET_RAD_TRASM_SUN]);
			logger(g_log, "Absorbed NetRad shade = %f W/m^2\n", s->value[NET_RAD_ABS_SHADE]);
			logger(g_log, "Trasmitted NetRad shade = %f W/m^2\n", s->value[NET_RAD_TRASM_SHADE]);
			logger(g_log, "Absorbed total = %f W/m^2\n", s->value[NET_RAD_ABS_SUN]+s->value[NET_RAD_ABS_SHADE]);

			/*compute PPFD for sun and shaded leaves*/
			//04/05/2016
/*			par = c->net_radiation * RAD2PAR * (1.0 - (s->value[ALBEDO]/3.0)) * ppfd_coeff;
			par_abs = par * LightAbsorb;
			par_abs_lai_sun = s->value[K]*par*s->value[LAI_SUN];
			par_abs_lai_shade = par_abs - par_abs_lai_sun;
			logger(g_log, "par = %f\n",par);
			logger(g_log, "par_abs = %f\n", par_abs);
			logger(g_log, "par_abs_lai_sun = %f\n", par_abs_lai_sun);
			logger(g_log, "par_abs_lai_shade NetRad shade = %f\n", par_abs_lai_shade);
*/
			//test 12 May 2016 test
			par = c->net_radiation * RAD2PAR * (1.0 - (LightReflec/3.0)) * ppfd_coeff;
			par_abs = par * LightAbsorb;
			par_abs_lai_sun = par * LightAbsorb_sun;
			par_trasm_lai_sun = par - par_abs_lai_sun;
			par_abs_lai_shade = par_trasm_lai_sun * LightAbsorb_shade;
			par_trasm_lai_shade = par_trasm_lai_sun - par_abs_lai_shade;
			logger(g_log, "par = %f\n",par);
			logger(g_log, "par_abs_lai_sun = %f\n", par_abs_lai_sun);
			logger(g_log, "par_trasm_lai_sun = %f\n", par_trasm_lai_sun);
			logger(g_log, "par_abs_lai_shade shade = %f\n", par_abs_lai_shade);
			logger(g_log, "par_trasm_lai_shade = %f\n", par_trasm_lai_shade);

			/* it follows rationale of BIOME-BGC */
			if(par_abs_lai_shade < 0.0)
			{
				par_abs_lai_sun = par_abs;
				par_abs_lai_shade = 0.0;
			}
			if(s->value[LAI_SUN] > 0.0 && s->value[LAI_SHADE] > 0.0)
			{
				par_abs_per_lai_sun = par_abs_lai_sun / s->value[LAI_SUN];
				par_abs_per_lai_shade = par_abs_lai_shade / s->value[LAI_SHADE];
			}
			else
			{
				par_abs_per_lai_sun = par_abs_per_lai_shade = 0.0;
			}

			s->value[PPFD_SUN] = par_abs_lai_sun * EPAR;
			s->value[PPFD_SHADE] = par_abs_lai_shade * EPAR;
			logger(g_log, "Absorbed PPFD = %f umol/m^2 sec\n", s->value[PPFD]);
			logger(g_log, "Absorbed PPFD sun = %f umol/m^2 sec\n", s->value[PPFD_SUN]);
			logger(g_log, "Absorbed PPFD shade = %f umol/m^2 sec\n", s->value[PPFD_SHADE]);
			//test 12 May 2016 test
			s->value[PPFD_SUN] = par_abs_per_lai_sun * EPAR;
			s->value[PPFD_SHADE] = par_abs_per_lai_shade * EPAR;
			logger(g_log, "Absorbed PPFD = %f umol/m^2 sec\n", s->value[PPFD]);
			logger(g_log, "Absorbed PPFD sun = %f umol/m^2 sec\n", s->value[PPFD_SUN]);
			logger(g_log, "Absorbed PPFD shade = %f umol/m^2 sec\n", s->value[PPFD_SHADE]);


			//only one height class in layer
			if ( c->height_class_in_layer_dominant_counter == 1 )
			{
				logger(g_log, "Only one height class in dominant light\n");
				c->gapcover[c->top_layer]= 1 - s->value[CANOPY_COVER_DBHDC];
				logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer]);
				if(c->gapcover[c->top_layer] < 0.0)
				{
					c->gapcover[c->top_layer] = 0;
					logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer]);
				}

				//test
				//no other classes
				if(c->height_class_in_layer_dominated_counter == 0)
				{
					c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
					logger(g_log, "Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
					//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
					c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
					logger(g_log, "Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
				}
				else
				{
					//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
					c->net_radiation_for_dominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
					logger(g_log, "Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

					//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
					c->par_for_dominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
					logger(g_log, "Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
				}
			}
			else
			{
				logger(g_log, "More than one height class in dominant light\n");
				logger(g_log, "dominant counter = %d\n", c->dominant_veg_counter);
				//first and higher height class enter
				//first height class processes
				if (c->dominant_veg_counter == 1 )
				{
					logger(g_log, "First height class processing....\n");
					c->gapcover[c->top_layer] = 1 - s->value[CANOPY_COVER_DBHDC];
					logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer]);
					if(c->gapcover[c->top_layer] < 0.0)
					{
						c->gapcover[c->top_layer] = 0;
						logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer]);
					}

				}
				else if ( c->dominant_veg_counter > 1 && c->dominant_veg_counter < c->height_class_in_layer_dominant_counter)
				{
					logger(g_log, "Second but not last height class processing....\n");
					c->gapcover[c->top_layer] -= s->value[CANOPY_COVER_DBHDC];
					logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer]);
					if(c->gapcover[c->top_layer] < 0.0)
					{
						c->gapcover[c->top_layer] = 0;
						logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer]);
					}
				}
				else //last height
				{
					logger(g_log, "Last height class processing....\n");
					c->gapcover[c->top_layer] -= s->value[CANOPY_COVER_DBHDC];
					logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer]);

					if(c->gapcover[c->top_layer] < 0.0)
					{
						c->gapcover[c->top_layer] = 0;
						logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer]);
					}
					if (c->dominant_veg_counter < c->height_class_in_layer_dominant_counter)
					{
						logger(g_log, "Second but not last height class processing....\n");
						c->gapcover[c->top_layer] -= s->value[CANOPY_COVER_DBHDC];
						if(c->gapcover[c->top_layer] < 0.0)
						{
							c->gapcover[c->top_layer] = 0;
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer]);
						}
					}
					else
					{
						//test
						//no other classes
						if(c->height_class_in_layer_dominated_counter == 0)
						{
							c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
							logger(g_log, "Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
							//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
							c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
							logger(g_log, "Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
						}
						else
						{
							//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
							c->net_radiation_for_dominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
							logger(g_log, "Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

							//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
							c->par_for_dominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
							logger(g_log, "Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
						}
					}
					logger(g_log, "Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);
					logger(g_log, "Average Par for lower layer = %f molPAR/m^2 month\n", c->par_for_dominated);
				}
			}
		}
		//fixme set that if gapcover is bigger then 0.5 albedo should be considered also in dominated layer!!!!
		//fixme following MAESPA (Duursma et al.,) dominated layers should have just shaded leaves and  from Campbell&Norman (2000, p. 259)
		else //DOMINATED LAYERS
		{
			//dominated layer
			if (c->heights[height].z == c->top_layer - 1)
			{
				logger(g_log, "**LIGHT DOMINATED**\n");
				logger(g_log, "Height Classes in Dominated Layer = %d\n", c->height_class_in_layer_dominated_counter);

				//Par
				c->par = c->par_for_dominated;
				logger(g_log, "Par = %f molPAR/m^2 day/month\n", c->par);

				//Net Radiation
				c->net_radiation = c->net_radiation_for_dominated;
				logger(g_log, "Net Radiation = %f W/m^2n", c->net_radiation);

				logger(g_log, "**BIOME APPROACH for dominted**\n");
				/*compute APAR for sun and shaded leaves*/
				//amount of total par that is reflected but leaves*/
				s->value[PAR] = c->par *(1.0 - (LightReflec/(3.0)));
				s->value[APAR] = s->value[PAR] * LightAbsorb;
				s->value[APAR_SUN] = c->par * s->value[LAI_SUN] * s->value[K];
				s->value[APAR_SHADE] = s->value[APAR] - s->value[APAR_SUN];
				logger(g_log, "Par = %f molPAR/m^2 day/month\n", s->value[PAR]);
				logger(g_log, "Apar = %f molPAR/m^2 day/month\n", s->value[APAR]);
				logger(g_log, "Apar sun = %f molPAR/m^2 day/month\n", s->value[APAR_SUN]);
				logger(g_log, "Apar shade = %f molPAR/m^2 day/month\n", s->value[APAR_SHADE]);

				/*compute NetRad for sun and shaded leaves*/
				//amount of Net Rad that is reflected but leaves*/
				s->value[NET_RAD] = c->net_radiation * (1.0 - LightReflec);
				s->value[NET_RAD_ABS] = s->value[NET_RAD] * LightAbsorb;
				s->value[NET_RAD_ABS_SUN] = c->net_radiation * s->value[LAI_SUN] * s->value[K];
				s->value[NET_RAD_ABS_SHADE] = s->value[NET_RAD_ABS] - s->value[NET_RAD_ABS_SUN];
				logger(g_log, "NetRad = %f W/m^2\n", s->value[NET_RAD]);
				logger(g_log, "Absorbed NetRad = %f W/m^2\n", s->value[NET_RAD_ABS]);
				logger(g_log, "Absorbed NetRad sun = %f W/m^2\n", s->value[NET_RAD_ABS_SUN]);
				logger(g_log, "Absorbed NetRad shade = %f W/m^2\n", s->value[NET_RAD_ABS_SHADE]);

				/*compute PPFD for sun and shaded leaves*/

				//04/05/2016
				//TEST
				par = c->net_radiation * RAD2PAR * (1.0 - (LightReflec/3.0));
				par_abs = par * LightAbsorb;
				par_abs_lai_sun = s->value[K]*par*s->value[LAI_SUN];
				par_abs_lai_shade = par_abs - par_abs_lai_sun;
				if(par_abs_lai_shade < 0.0)
				{
					par_abs_lai_sun = par_abs;
					par_abs_lai_shade = 0.0;
				}
				if(s->value[LAI_SUN] > 0.0 && s->value[LAI_SHADE] > 0.0)
				{
					par_abs_per_lai_sun = par_abs_lai_sun / s->value[LAI_SUN];
					par_abs_per_lai_shade = par_abs_lai_shade / s->value[LAI_SHADE];
				}
				else
				{
					par_abs_per_lai_sun = par_abs_per_lai_shade = 0.0;
				}
				s->value[PPFD_SUN] = par_abs_lai_sun * EPAR;
				s->value[PPFD_SHADE] = par_abs_lai_shade * EPAR;
				logger(g_log, "Absorbed PPFD = %f umol/m^2 sec\n", s->value[PPFD]);
				logger(g_log, "Absorbed PPFD sun = %f umol/m^2 sec\n", s->value[PPFD_SUN]);
				logger(g_log, "Absorbed PPFD shade = %f umol/m^2 sec\n", s->value[PPFD_SHADE]);

				if ( c->dominated_veg_counter >= 3 )
				{
					if ( c->height_class_in_layer_dominated_counter == 1)    //only one height class in layer
					{
						logger(g_log, "Only one height class in layer dominated\n");
						c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
						logger(g_log, "gapcover layer %d  = %f\n", c->top_layer, c->gapcover[c->top_layer-1]);
						if(c->gapcover[c->top_layer - 1] < 0.0)
						{
							c->gapcover[c->top_layer - 1] = 0;
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
						}

						if (c->gapcover[c->top_layer] <= 0.5)
						{
							//test
							//no other classes
							if(c->height_class_in_layer_subdominated_counter == 0)
							{
								c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
								logger(g_log, "Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
								//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
								c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
								logger(g_log, "Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
							}
							else
							{
								//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
								c->net_radiation_for_subdominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
								logger(g_log, "Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

								//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
								c->par_for_subdominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
								logger(g_log, "Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
							}
						}
					}
					else  //many height classes in this layer
					{
						logger(g_log, "More height classes in layer dominated\n");
						if (c->dominated_veg_counter == 1 ) //first height class processes
						{
							c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
							logger(g_log, "gapcover layer %d  = %f\n", c->top_layer, c->gapcover[c->top_layer-1]);
							if(c->gapcover[c->top_layer - 1] < 0.0)
							{
								c->gapcover[c->top_layer - 1] = 0;
								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
							}
						}
						else if ( c->dominated_veg_counter > 1 && c->dominated_veg_counter < c->height_class_in_layer_dominated_counter )
						{
							logger(g_log, "Second and more height class processed\n");
							c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
							logger(g_log, "gapcover layer %d  = %f\n", c->top_layer, c->gapcover[c->top_layer-1]);
							if(c->gapcover[c->top_layer - 1] < 0.0)
							{
								c->gapcover[c->top_layer - 1] = 0;
								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
							}
						}
						else //last height
						{
							logger(g_log, "Last height class processed\n");
							//Gap_Cover -= s->value[CANOPY_COVER_DBHDC];
							//logger(g_log, "Free space in layer = %f\n", Gap_Cover);

							if (c->dominant_veg_counter < c->height_class_in_layer_dominant_counter)
							{
								logger(g_log, "Second but not last height class processing....\n");
								c->gapcover[c->top_layer-1] -= s->value[CANOPY_COVER_DBHDC];
								if(c->gapcover[c->top_layer - 1] < 0.0)
								{
									c->gapcover[c->top_layer - 1] = 0;
									logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
								}
							}
							//FIXME NEW VERSION
							else
							{
								//test
								//no other classes
								if(c->height_class_in_layer_subdominated_counter == 0)
								{
									c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
											+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
									logger(g_log, "Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
									//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
									c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
											+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
									logger(g_log, "Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
								}
								else
								{
									//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
									c->net_radiation_for_subdominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
											+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
									logger(g_log, "Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

									//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
									c->par_for_subdominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
											+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
									logger(g_log, "Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
								}
							}
							logger(g_log, "Net Radiation for lower layer = %f  W/m^2\n", c->net_radiation_for_subdominated);
							logger(g_log, "Par for  lower layer = %f molPAR/m^2 day/month\n", c->par_for_subdominated);
						}
					}
				}
				else
				{
					if ( c->height_class_in_layer_dominated_counter == 1) //first height  class processed
					{
						c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
						logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-1]);

						//test
						//no other classes
						if(c->height_class_in_layer_subdominated_counter == 0)
						{
							c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
							logger(g_log, "Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
							//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
							c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
							logger(g_log, "Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
						}
						else
						{
							//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
							c->net_radiation_for_subdominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
							logger(g_log, "Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

							//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
							c->par_for_subdominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
							logger(g_log, "Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
						}
					}
					else
					{
						logger(g_log, "More height classes in layer dominated\n");
						if (c->dominated_veg_counter == 1 ) //first height class proccessess
						{
							logger(g_log, "First height class processed\n");
							c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-1]);
							if(c->gapcover[c->top_layer - 1] < 0.0)
							{
								c->gapcover[c->top_layer - 1] = 0;
								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
							}
						}
						else if (c->dominated_veg_counter > 1 && c->dominated_veg_counter < c->height_class_in_layer_dominated_counter)
						{
							logger(g_log, "Second and more height class processed\n");
							c->gapcover[c->top_layer-1] -= s->value[CANOPY_COVER_DBHDC];
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-1]);
							if(c->gapcover[c->top_layer - 1] < 0.0)
							{
								c->gapcover[c->top_layer - 1] = 0;
								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
							}
						}
						else //last height
						{
							logger(g_log, "Last height class processed\n");
							c->gapcover[c->top_layer-1] -= s->value[CANOPY_COVER_DBHDC];
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-1]);
							if(c->gapcover[c->top_layer - 1] < 0.0)
							{
								c->gapcover[c->top_layer - 1] = 0;
								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 1]);
							}

							//test
							//no other classes
							if(c->height_class_in_layer_subdominated_counter == 0)
							{
								c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
								logger(g_log, "Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
								//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
								c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
								logger(g_log, "Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
							}
							else
							{
								//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
								c->net_radiation_for_subdominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
								logger(g_log, "Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

								//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
								c->par_for_subdominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
								logger(g_log, "Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
							}
						}
					}
				}
			}
			//subdominated layer
			else
			{
				logger(g_log, "**LIGHT SUB-DOMINATED**\n");
				logger(g_log, "Height Classes in Sub-Dominated Layer = %d\n", c->height_class_in_layer_subdominated_counter);


				logger(g_log, "**BIOME APPROACH for dominant**\n");
				/*compute APAR for sun and shaded leaves*/
				//amount of total par that is reflected but leaves*/
				s->value[PAR] = c->par *(1.0 - (LightReflec/(3.0)));
				s->value[APAR] = s->value[PAR] * LightAbsorb;
				s->value[APAR_SUN] = c->par * s->value[LAI_SUN] * s->value[K];
				s->value[APAR_SHADE] = s->value[APAR] - s->value[APAR_SUN];
				logger(g_log, "Par = %f molPAR/m^2 day/month\n", s->value[PAR]);
				logger(g_log, "Apar = %f molPAR/m^2 day/month\n", s->value[APAR]);
				logger(g_log, "Apar sun = %f molPAR/m^2 day/month\n", s->value[APAR_SUN]);
				logger(g_log, "Apar shade = %f molPAR/m^2 day/month\n", s->value[APAR_SHADE]);

				/*compute NetRad for sun and shaded leaves*/
				//amount of Net Rad that is reflected but leaves*/
				s->value[NET_RAD] = c->net_radiation * (1.0 - LightReflec);
				s->value[NET_RAD_ABS] = s->value[NET_RAD] * LightAbsorb;
				s->value[NET_RAD_ABS_SUN] = c->net_radiation * s->value[LAI_SUN] * s->value[K];
				s->value[NET_RAD_ABS_SHADE] = s->value[NET_RAD_ABS] - s->value[NET_RAD_ABS_SUN];
				logger(g_log, "NetRad = %f W/m^2\n", s->value[NET_RAD]);
				logger(g_log, "Absorbed NetRad = %f W/m^2\n", s->value[NET_RAD_ABS]);
				logger(g_log, "Absorbed NetRad sun = %f W/m^2\n", s->value[NET_RAD_ABS_SUN]);
				logger(g_log, "Absorbed NetRad shade = %f W/m^2\n", s->value[NET_RAD_ABS_SHADE]);
				/*compute PPFD for sun and shaded leaves*/

				//04/05/2016
				//TEST
				par = c->net_radiation * RAD2PAR * (1.0 - (LightReflec/3.0));
				par_abs = par * LightAbsorb;
				par_abs_lai_sun = s->value[K]*par*s->value[LAI_SUN];
				par_abs_lai_shade = par_abs - par_abs_lai_sun;
				if(par_abs_lai_shade < 0.0)
				{
					par_abs_lai_sun = par_abs;
					par_abs_lai_shade = 0.0;
				}
				if(s->value[LAI_SUN] > 0.0 && s->value[LAI_SHADE] > 0.0)
				{
					par_abs_per_lai_sun = par_abs_lai_sun / s->value[LAI_SUN];
					par_abs_per_lai_shade = par_abs_lai_shade / s->value[LAI_SHADE];
				}
				else
				{
					par_abs_per_lai_sun = par_abs_per_lai_shade = 0.0;
				}
				s->value[PPFD_SUN] = par_abs_lai_sun * EPAR;
				s->value[PPFD_SHADE] = par_abs_lai_shade * EPAR;
				logger(g_log, "Absorbed PPFD = %f umol/m^2 sec\n", s->value[PPFD]);
				logger(g_log, "Absorbed PPFD sun = %f umol/m^2 sec\n", s->value[PPFD_SUN]);
				logger(g_log, "Absorbed PPFD shade = %f umol/m^2 sec\n", s->value[PPFD_SHADE]);

				if ( c->heights_count >= 3 )
				{
					if ( c->height_class_in_layer_subdominated_counter == 1)
					{
						logger(g_log, "Only one height class in layer subdominated\n");
						//percentuale coperta
						c->gapcover[c->top_layer-2] = 1 - s->value[CANOPY_COVER_DBHDC];
						logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
						if(c->gapcover[c->top_layer - 2] < 0.0)
						{
							c->gapcover[c->top_layer - 2] = 0;
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
						}
						//NON È IL VALORE IN METRO QUADRATO MA IL VALORE PER SUPERFICiE COPERTA
						c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
								+ (c->par_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell)))/settings->sizeCell;
						c->net_radiation_for_soil = ((c->net_radiation_for_subdominated * (1 - LightAbsorb) ) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
								+(c->net_radiation_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell)))/settings->sizeCell;
						logger(g_log, "Net radiation for soil = %fW/m^2\n", c->net_radiation_for_soil);
						logger(g_log, "Par for soil = %f molPAR/m^2 month\n", c->par_for_soil);
					}
					else
					{
						logger(g_log, "More height classes in layer subdominated\n");
						if (c->subdominated_veg_counter == 1 ) //first height class processes
						{
							logger(g_log, "First height class processed\n");
							c->gapcover[c->top_layer-2] = 1.0 - s->value[CANOPY_COVER_DBHDC];
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
							if(c->gapcover[c->top_layer - 2] < 0.0)
							{
								c->gapcover[c->top_layer - 2] = 0;
								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
							}
							//Par for lower layer
						}
						else if ( c->subdominated_veg_counter > 1 && c->subdominated_veg_counter < c->height_class_in_layer_subdominated_counter )
						{
							logger(g_log, "Second and more height class processed\n");
							c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
							if(c->gapcover[c->top_layer - 2] < 0.0)
							{
								c->gapcover[c->top_layer - 2] = 0;
								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
							}
						}
						else //last height
						{
							logger(g_log, "Last height class processed\n");
							c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
							if(c->gapcover[c->top_layer - 2] < 0.0)
							{
								c->gapcover[c->top_layer - 2] = 0;
								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
							}

							c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
							c->net_radiation_for_soil = ((c->net_radiation_for_subdominated * (1 - LightAbsorb) ) * ((1.0- c->gapcover[c->top_layer-2])
									* settings->sizeCell))/settings->sizeCell;
							logger(g_log, "Net radiation for soil = %fW/m^2\n", c->net_radiation_for_soil);
							logger(g_log, "Par for soil = %f molPAR/m^2 day/month\n", c->par_for_soil);
						}
					}
				}
				else
				{
					if ( c->height_class_in_layer_subdominated_counter == 1) //first height  class processed
					{
						//percentuale coperta
						c->gapcover[c->top_layer-2] = 1.0 - s->value[CANOPY_COVER_DBHDC];
						if(c->gapcover[c->top_layer - 2] < 0.0)
						{
							c->gapcover[c->top_layer - 2] = 0;
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
						}
						c->par_for_soil = (c->par_for_dominated - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);

						//percentuale scoperta
						c->par_for_soil += (c->par_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell));
						c->par_for_soil /= settings->sizeCell;
						c->net_radiation_for_soil += c->net_radiation_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell);
						c->net_radiation_for_soil /= settings->sizeCell;
						logger(g_log, "Par for layer = %f molPAR/m^2 day/month\n", c->par_for_soil);
					}
					else
					{
						logger(g_log, "More height classes in layer dominated\n");
						if (c->subdominated_veg_counter == 1 ) //first height class proccessess
						{
							logger(g_log, "First height class processed\n");
							c->gapcover[c->top_layer-2] = 1.0 - s->value[CANOPY_COVER_DBHDC];
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
							if(c->gapcover[c->top_layer - 2] < 0.0)
							{
								c->gapcover[c->top_layer - 2] = 0;
								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
							}
						}
						else if (c->subdominated_veg_counter > 1 && c->subdominated_veg_counter < c->height_class_in_layer_subdominated_counter)
						{
							logger(g_log, "Second and more height class processed\n");
							c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
							if(c->gapcover[c->top_layer - 2] < 0.0)
							{
								c->gapcover[c->top_layer - 2] = 0;
								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
							}
						}
						else //last height
						{
							logger(g_log, "Last height class processed\n");
							c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
							logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer-2]);
							if(c->gapcover[c->top_layer - 2] < 0.0)
							{
								c->gapcover[c->top_layer - 2] = 0;
								logger(g_log, "GapCover = %f\n", c->gapcover[c->top_layer - 2]);
							}

							c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
							c->net_radiation_for_soil = ((c->net_radiation_for_subdominated * (1 - LightAbsorb) ) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
							logger(g_log, "Net radiation for soil = %fW/m^2\n", c->net_radiation_for_soil);
							logger(g_log, "Par for soil = %f molPAR/m^2 day/month\n", c->par_for_soil);
						}
					}
				}
			}
		}
	}
	else
	{
		c->net_radiation_for_soil = c->net_radiation;
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
