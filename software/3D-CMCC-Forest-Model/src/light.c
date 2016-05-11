/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"




void Radiation ( SPECIES *const s, CELL *const c, const MET_DATA *const met, int month, int day, int DaysInMonth, int height)
{

	double LightAbsorb, LightAbsorb_sun, LightAbsorb_shade; //fraction of light absorbed
	double LightTrasm, LightTrasm_sun, LightTrasm_shade; //fraction of light trasmitted
	double a = 107.0;//(W/m)  empirical constants for long wave radiation computation
	double b = 0.2; //(unitless) empirical constants for long wave radiation computation
	double ni; //proportion of daylength

	/* for biome's ppfd */
	double par;
	double par_abs;
	double par_abs_lai_sun, par_abs_lai_shade;
	double par_abs_per_lai_sun, par_abs_per_lai_shade;

	double ppfd_coeff = 0.01; /* parameter that quantifies the effect of light on conductance see Schwalm and Ek 2004 and Kimbal et al., 1997 */

	//following Ritchie et al., 1998 and Hydi et al., (submitted)
	double actual_albedo;
	double soil_albedo = 0.17; //(see Wiki)

	//test
	//fixme LAI values should integrated over the cell considering different
	actual_albedo = s->value[ALBEDO] * (s->value[ALBEDO]-soil_albedo * exp(-0.75 * s->value[LAI]));
	//Log("actual_albedo = %f\n", actual_albedo);


	Log("\nRADIATION ROUTINE\n");

	/*proportion of daylength*/
	ni = met[month].d[day].daylength/24.0;

	c->par = (met[month].d[day].solar_rad * MOLPAR_MJ);
	Log("Par = %f molPAR/m^2 day\n", c->par);
	c->short_wave_radiation = met[month].d[day].solar_rad * pow (10.0, 6)/86400.0;
	Log("Short wave radiation (downward) = %f W/m2\n", c->short_wave_radiation);
	/*following LPJ approach*/
	c->long_wave_radiation = (b+(1.0-b)*ni)*(a - met[month].d[day].tday);
	Log("Long wave radiation (upward) = %f W/m2\n", c->long_wave_radiation);
	c->net_radiation = c->short_wave_radiation - c->long_wave_radiation;
	//fixme is it correct to avoid negative values?
	Log("Net radiation = %f W/m2\n", c->net_radiation);
	if(c->net_radiation < 0.00000001)
	{
		c->net_radiation = 0.00000001;
		Log("Net radiation = %f W/m2\n", c->net_radiation);
	}

	/*if at least one class is in veg period*/
	if (c->Veg_Counter > 0.0)
	{
		if (settings->spatial == 's')
		{
			//fixme
			LightTrasm = (exp(- s->value[K] * met[month].d[day].ndvi_lai));
			//Log("NDVI-LAI = %f \n", met[month].ndvi_lai );
			LightTrasm_sun = (exp(- s->value[K] * s->value[LAI_SUN]));
			LightTrasm_shade = (exp(- s->value[K] * s->value[LAI_SHADE]));
		}
		else
		{
			LightTrasm = (exp(- s->value[K] * s->value[LAI]));
			LightTrasm_sun = (exp(- s->value[K] * s->value[LAI_SUN]));
			LightTrasm_shade = (exp(- s->value[K] * s->value[LAI_SHADE]));

		}
		LightAbsorb = 1.0 - LightTrasm;
		LightAbsorb_sun = 1.0 - LightTrasm_sun;
		LightAbsorb_shade = 1.0 - LightTrasm_shade;

		Log("LightAbsorb_sun = %f\n", LightAbsorb_sun);
		Log("LightTrasm_sun = %f\n", LightTrasm_sun);
		Log("LightAbsorb_shade = %f\n", LightAbsorb_shade);
		Log("LightTrasm_sun = %f\n", LightTrasm_shade);


		//LIGHT DOMINANT
		if ( c->heights[height].z == c->top_layer )
		{
			Log("**LIGHT DOMINANT**\n");
			Log("Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_dominant_counter);

			//todo check if albedo is necessary
			//AS FOR PAR ALBEDO SHOULD BE TAKEN INTO ACCOUNT ONLY FOR SUN LEAVES THAT REPRESENT LAI_RATIO
			//following BIOME albedo for PAR is 1/3 of albedo
			//The absorbed PAR is calculated similarly except that albedo is 1/3 as large for PAR because less
			//PAR is reflected than net_radiation (Jones 1992)

			Log("**BIOME APPROACH for dominant**\n");
			/*compute APAR for sun and shaded leaves*/
			//amount of total par that is reflected but leaves*/

			//test 11 May 2016 test
			s->value[PAR] = c->par *(1.0 - (s->value[ALBEDO]/(3.0)));
			s->value[APAR] = s->value[PAR] * LightAbsorb;
			s->value[APAR_SUN] = s->value[PAR] * LightAbsorb_sun;
			s->value[TRASM_PAR_SUN] = s->value[PAR] - s->value[APAR_SUN];
			s->value[APAR_SHADE] = s->value[TRASM_PAR_SUN] * LightAbsorb_shade;
			s->value[TRASM_PAR_SHADE] = s->value[TRASM_PAR_SUN] - s->value[APAR_SHADE];

			Log("Par = %f molPAR/m^2 day\n", s->value[PAR]);
			Log("Apar = %f molPAR/m^2 day\n", s->value[APAR]);
			Log("Apar sun = %f molPAR/m^2 day\n", s->value[APAR_SUN]);
			Log("Apar shade = %f molPAR/m^2 day\n", s->value[APAR_SHADE]);

			/*compute NetRad for sun and shaded leaves*/
			//amount of Net Rad that is reflected but leaves*/

			//test 11 May 2016 test
			s->value[NET_RAD] = c->net_radiation * (1.0 - s->value[ALBEDO]);
			s->value[NET_RAD_ABS] = s->value[NET_RAD] * LightAbsorb;
			s->value[NET_RAD_ABS_SUN] = s->value[NET_RAD] * LightAbsorb_sun;
			s->value[NET_RAD_TRASM_SUN] = s->value[NET_RAD] - s->value[NET_RAD_ABS_SUN];
			s->value[NET_RAD_ABS_SHADE] = s->value[NET_RAD_TRASM_SUN] * LightAbsorb_shade;
			s->value[NET_RAD_TRASM_SHADE] = s->value[NET_RAD_TRASM_SUN] - s->value[NET_RAD_ABS_SHADE];
			Log("INCOMING net_radiation = %f W/m^2\n", c->net_radiation);
			Log("NetRad = %f W/m^2\n", s->value[NET_RAD]);
			Log("Absorbed NetRad sun = %f W/m^2\n", s->value[NET_RAD_ABS_SUN]);
			Log("Trasmitted NetRad sun = %f W/m^2\n", s->value[NET_RAD_TRASM_SUN]);
			Log("Absorbed NetRad shade = %f W/m^2\n", s->value[NET_RAD_ABS_SHADE]);
			Log("Trasmitted NetRad shade = %f W/m^2\n", s->value[NET_RAD_TRASM_SHADE]);


			/*compute PPFD for sun and shaded leaves*/
			//04/05/2016
			//TEST
			//FIXME FOLLOWING WHAT DONE FOR NET_RAD
			par = c->net_radiation * RAD2PAR * (1.0 - (s->value[ALBEDO]/3.0)) * ppfd_coeff;
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
			Log("Absorbed PPFD = %f umol/m^2 sec\n", s->value[PPFD]);
			Log("Absorbed PPFD sun = %f umol/m^2 sec\n", s->value[PPFD_SUN]);
			Log("Absorbed PPFD shade = %f umol/m^2 sec\n", s->value[PPFD_SHADE]);

			//only one height class in layer
			if ( c->height_class_in_layer_dominant_counter == 1 )
			{
				Log("Only one height class in dominant light\n");
				c->gapcover[c->top_layer]= 1 - s->value[CANOPY_COVER_DBHDC];
				Log("GapCover = %f\n", c->gapcover[c->top_layer]);
				if(c->gapcover[c->top_layer] < 0.0)
				{
					c->gapcover[c->top_layer] = 0;
					Log("GapCover = %f\n", c->gapcover[c->top_layer]);
				}

				//test
				//no other classes
				if(c->height_class_in_layer_dominated_counter == 0)
				{
					c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
					Log("Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
					//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
					c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
					Log("Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
				}
				else
				{
					//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
					c->net_radiation_for_dominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
					Log("Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

					//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
					c->par_for_dominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
					Log("Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
				}
			}
			else
			{
				Log("More than one height class in dominant light\n");
				Log("dominant counter = %d\n", c->dominant_veg_counter);
				//first and higher height class enter
				//first height class processes
				if (c->dominant_veg_counter == 1 )
				{
					Log("First height class processing....\n");
					c->gapcover[c->top_layer] = 1 - s->value[CANOPY_COVER_DBHDC];
					Log("GapCover = %f\n", c->gapcover[c->top_layer]);
					if(c->gapcover[c->top_layer] < 0.0)
					{
						c->gapcover[c->top_layer] = 0;
						Log("GapCover = %f\n", c->gapcover[c->top_layer]);
					}

				}
				else if ( c->dominant_veg_counter > 1 && c->dominant_veg_counter < c->height_class_in_layer_dominant_counter)
				{
					Log("Second but not last height class processing....\n");
					c->gapcover[c->top_layer] -= s->value[CANOPY_COVER_DBHDC];
					Log("GapCover = %f\n", c->gapcover[c->top_layer]);
					if(c->gapcover[c->top_layer] < 0.0)
					{
						c->gapcover[c->top_layer] = 0;
						Log("GapCover = %f\n", c->gapcover[c->top_layer]);
					}
				}
				else //last height
				{
					Log("Last height class processing....\n");
					c->gapcover[c->top_layer] -= s->value[CANOPY_COVER_DBHDC];
					Log("GapCover = %f\n", c->gapcover[c->top_layer]);

					if(c->gapcover[c->top_layer] < 0.0)
					{
						c->gapcover[c->top_layer] = 0;
						Log("GapCover = %f\n", c->gapcover[c->top_layer]);
					}
					if (c->dominant_veg_counter < c->height_class_in_layer_dominant_counter)
					{
						Log("Second but not last height class processing....\n");
						c->gapcover[c->top_layer] -= s->value[CANOPY_COVER_DBHDC];
						if(c->gapcover[c->top_layer] < 0.0)
						{
							c->gapcover[c->top_layer] = 0;
							Log("GapCover = %f\n", c->gapcover[c->top_layer]);
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
							Log("Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
							//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
							c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
							Log("Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
						}
						else
						{
							//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
							c->net_radiation_for_dominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
							Log("Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

							//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
							c->par_for_dominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
							Log("Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
						}
					}
					Log("Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);
					Log("Average Par for lower layer = %f molPAR/m^2 month\n", c->par_for_dominated);
				}
			}
		}
		//fixme set that if gapcover is bigger then 0.5 albedo should be considered also in dominated layer!!!!
		else //DOMINATED LAYERS
		{
			//dominated layer
			if (c->heights[height].z == c->top_layer - 1)
			{
				Log("**LIGHT DOMINATED**\n");
				Log("Height Classes in Dominated Layer = %d\n", c->height_class_in_layer_dominated_counter);

				//Par
				c->par = c->par_for_dominated;
				Log("Par = %f molPAR/m^2 day/month\n", c->par);

				//Net Radiation
				c->net_radiation = c->net_radiation_for_dominated;
				Log("Net Radiation = %f W/m^2n", c->net_radiation);

				Log("**BIOME APPROACH for dominted**\n");
				/*compute APAR for sun and shaded leaves*/
				//amount of total par that is reflected but leaves*/
				s->value[PAR] = c->par *(1.0 - (s->value[ALBEDO]/(3.0)));
				s->value[APAR] = s->value[PAR] * LightAbsorb;
				s->value[APAR_SUN] = c->par * s->value[LAI_SUN] * s->value[K];
				s->value[APAR_SHADE] = s->value[APAR] - s->value[APAR_SUN];
				Log("Par = %f molPAR/m^2 day/month\n", s->value[PAR]);
				Log("Apar = %f molPAR/m^2 day/month\n", s->value[APAR]);
				Log("Apar sun = %f molPAR/m^2 day/month\n", s->value[APAR_SUN]);
				Log("Apar shade = %f molPAR/m^2 day/month\n", s->value[APAR_SHADE]);

				/*compute NetRad for sun and shaded leaves*/
				//amount of Net Rad that is reflected but leaves*/
				s->value[NET_RAD] = c->net_radiation * (1.0 - s->value[ALBEDO]);
				s->value[NET_RAD_ABS] = s->value[NET_RAD] * LightAbsorb;
				s->value[NET_RAD_ABS_SUN] = c->net_radiation * s->value[LAI_SUN] * s->value[K];
				s->value[NET_RAD_ABS_SHADE] = s->value[NET_RAD_ABS] - s->value[NET_RAD_ABS_SUN];
				Log("NetRad = %f W/m^2\n", s->value[NET_RAD]);
				Log("Absorbed NetRad = %f W/m^2\n", s->value[NET_RAD_ABS]);
				Log("Absorbed NetRad sun = %f W/m^2\n", s->value[NET_RAD_ABS_SUN]);
				Log("Absorbed NetRad shade = %f W/m^2\n", s->value[NET_RAD_ABS_SHADE]);

				/*compute PPFD for sun and shaded leaves*/

				//04/05/2016
				//TEST
				par = c->net_radiation * RAD2PAR * (1.0 - (s->value[ALBEDO]/3.0));
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
				Log("Absorbed PPFD = %f umol/m^2 sec\n", s->value[PPFD]);
				Log("Absorbed PPFD sun = %f umol/m^2 sec\n", s->value[PPFD_SUN]);
				Log("Absorbed PPFD shade = %f umol/m^2 sec\n", s->value[PPFD_SHADE]);

				if ( c->dominated_veg_counter >= 3 )
				{
					if ( c->height_class_in_layer_dominated_counter == 1)    //only one height class in layer
					{
						Log("Only one height class in layer dominated\n");
						c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
						Log("gapcover layer %d  = %f\n", c->top_layer, c->gapcover[c->top_layer-1]);
						if(c->gapcover[c->top_layer - 1] < 0.0)
						{
							c->gapcover[c->top_layer - 1] = 0;
							Log("GapCover = %f\n", c->gapcover[c->top_layer - 1]);
						}

						if (c->gapcover[c->top_layer] <= 0.5)
						{
							//test
							//no other classes
							if(c->height_class_in_layer_subdominated_counter == 0)
							{
								c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
								Log("Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
								//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
								c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
								Log("Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
							}
							else
							{
								//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
								c->net_radiation_for_subdominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
								Log("Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

								//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
								c->par_for_subdominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
								Log("Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
							}
						}
					}
					else  //many height classes in this layer
					{
						Log("More height classes in layer dominated\n");
						if (c->dominated_veg_counter == 1 ) //first height class processes
						{
							c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
							Log("gapcover layer %d  = %f\n", c->top_layer, c->gapcover[c->top_layer-1]);
							if(c->gapcover[c->top_layer - 1] < 0.0)
							{
								c->gapcover[c->top_layer - 1] = 0;
								Log("GapCover = %f\n", c->gapcover[c->top_layer - 1]);
							}
						}
						else if ( c->dominated_veg_counter > 1 && c->dominated_veg_counter < c->height_class_in_layer_dominated_counter )
						{
							Log("Second and more height class processed\n");
							c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
							Log("gapcover layer %d  = %f\n", c->top_layer, c->gapcover[c->top_layer-1]);
							if(c->gapcover[c->top_layer - 1] < 0.0)
							{
								c->gapcover[c->top_layer - 1] = 0;
								Log("GapCover = %f\n", c->gapcover[c->top_layer - 1]);
							}
						}
						else //last height
						{
							Log("Last height class processed\n");
							//Gap_Cover -= s->value[CANOPY_COVER_DBHDC];
							//Log("Free space in layer = %f\n", Gap_Cover);

							if (c->dominant_veg_counter < c->height_class_in_layer_dominant_counter)
							{
								Log("Second but not last height class processing....\n");
								c->gapcover[c->top_layer-1] -= s->value[CANOPY_COVER_DBHDC];
								if(c->gapcover[c->top_layer - 1] < 0.0)
								{
									c->gapcover[c->top_layer - 1] = 0;
									Log("GapCover = %f\n", c->gapcover[c->top_layer - 1]);
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
									Log("Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
									//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
									c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
											+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
									Log("Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
								}
								else
								{
									//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
									c->net_radiation_for_subdominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
											+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
									Log("Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

									//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
									c->par_for_subdominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
											+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
									Log("Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
								}
							}
							Log("Net Radiation for lower layer = %f  W/m^2\n", c->net_radiation_for_subdominated);
							Log("Par for  lower layer = %f molPAR/m^2 day/month\n", c->par_for_subdominated);
						}
					}
				}
				else
				{
					if ( c->height_class_in_layer_dominated_counter == 1) //first height  class processed
					{
						c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
						Log("GapCover = %f\n", c->gapcover[c->top_layer-1]);

						//test
						//no other classes
						if(c->height_class_in_layer_subdominated_counter == 0)
						{
							c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
							Log("Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
							//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
							c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
							Log("Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
						}
						else
						{
							//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
							c->net_radiation_for_subdominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
							Log("Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

							//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
							c->par_for_subdominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
							Log("Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
						}
					}
					else
					{
						Log("More height classes in layer dominated\n");
						if (c->dominated_veg_counter == 1 ) //first height class proccessess
						{
							Log("First height class processed\n");
							c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
							Log("GapCover = %f\n", c->gapcover[c->top_layer-1]);
							if(c->gapcover[c->top_layer - 1] < 0.0)
							{
								c->gapcover[c->top_layer - 1] = 0;
								Log("GapCover = %f\n", c->gapcover[c->top_layer - 1]);
							}
						}
						else if (c->dominated_veg_counter > 1 && c->dominated_veg_counter < c->height_class_in_layer_dominated_counter)
						{
							Log("Second and more height class processed\n");
							c->gapcover[c->top_layer-1] -= s->value[CANOPY_COVER_DBHDC];
							Log("GapCover = %f\n", c->gapcover[c->top_layer-1]);
							if(c->gapcover[c->top_layer - 1] < 0.0)
							{
								c->gapcover[c->top_layer - 1] = 0;
								Log("GapCover = %f\n", c->gapcover[c->top_layer - 1]);
							}
						}
						else //last height
						{
							Log("Last height class processed\n");
							c->gapcover[c->top_layer-1] -= s->value[CANOPY_COVER_DBHDC];
							Log("GapCover = %f\n", c->gapcover[c->top_layer-1]);
							if(c->gapcover[c->top_layer - 1] < 0.0)
							{
								c->gapcover[c->top_layer - 1] = 0;
								Log("GapCover = %f\n", c->gapcover[c->top_layer - 1]);
							}

							//test
							//no other classes
							if(c->height_class_in_layer_subdominated_counter == 0)
							{
								c->net_radiation_for_soil = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
								Log("Net Radiation for soil = %f  W/m2\n", c->net_radiation_for_dominated);
								//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
								c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
								Log("Average Par for soil = %f molPAR/m^2 day/month\n", c->par_for_dominated);
							}
							else
							{
								//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
								c->net_radiation_for_subdominated = ((s->value[NET_RAD] - s->value[NET_RAD_ABS]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
								Log("Net Radiation for lower layer = %f  W/m2\n", c->net_radiation_for_dominated);

								//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
								c->par_for_subdominated = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
										+ (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
								Log("Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
							}
						}
					}
				}
			}
			//subdominated layer
			else
			{
				Log("**LIGHT SUB-DOMINATED**\n");
				Log("Height Classes in Sub-Dominated Layer = %d\n", c->height_class_in_layer_subdominated_counter);


				Log("**BIOME APPROACH for dominant**\n");
				/*compute APAR for sun and shaded leaves*/
				//amount of total par that is reflected but leaves*/
				s->value[PAR] = c->par *(1.0 - (s->value[ALBEDO]/(3.0)));
				s->value[APAR] = s->value[PAR] * LightAbsorb;
				s->value[APAR_SUN] = c->par * s->value[LAI_SUN] * s->value[K];
				s->value[APAR_SHADE] = s->value[APAR] - s->value[APAR_SUN];
				Log("Par = %f molPAR/m^2 day/month\n", s->value[PAR]);
				Log("Apar = %f molPAR/m^2 day/month\n", s->value[APAR]);
				Log("Apar sun = %f molPAR/m^2 day/month\n", s->value[APAR_SUN]);
				Log("Apar shade = %f molPAR/m^2 day/month\n", s->value[APAR_SHADE]);

				/*compute NetRad for sun and shaded leaves*/
				//amount of Net Rad that is reflected but leaves*/
				s->value[NET_RAD] = c->net_radiation * (1.0 - s->value[ALBEDO]);
				s->value[NET_RAD_ABS] = s->value[NET_RAD] * LightAbsorb;
				s->value[NET_RAD_ABS_SUN] = c->net_radiation * s->value[LAI_SUN] * s->value[K];
				s->value[NET_RAD_ABS_SHADE] = s->value[NET_RAD_ABS] - s->value[NET_RAD_ABS_SUN];
				Log("NetRad = %f W/m^2\n", s->value[NET_RAD]);
				Log("Absorbed NetRad = %f W/m^2\n", s->value[NET_RAD_ABS]);
				Log("Absorbed NetRad sun = %f W/m^2\n", s->value[NET_RAD_ABS_SUN]);
				Log("Absorbed NetRad shade = %f W/m^2\n", s->value[NET_RAD_ABS_SHADE]);
				/*compute PPFD for sun and shaded leaves*/

				//04/05/2016
				//TEST
				par = c->net_radiation * RAD2PAR * (1.0 - (s->value[ALBEDO]/3.0));
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
				Log("Absorbed PPFD = %f umol/m^2 sec\n", s->value[PPFD]);
				Log("Absorbed PPFD sun = %f umol/m^2 sec\n", s->value[PPFD_SUN]);
				Log("Absorbed PPFD shade = %f umol/m^2 sec\n", s->value[PPFD_SHADE]);

				if ( c->heights_count >= 3 )
				{
					if ( c->height_class_in_layer_subdominated_counter == 1)
					{
						Log("Only one height class in layer subdominated\n");
						//percentuale coperta
						c->gapcover[c->top_layer-2] = 1 - s->value[CANOPY_COVER_DBHDC];
						Log("GapCover = %f\n", c->gapcover[c->top_layer-2]);
						if(c->gapcover[c->top_layer - 2] < 0.0)
						{
							c->gapcover[c->top_layer - 2] = 0;
							Log("GapCover = %f\n", c->gapcover[c->top_layer - 2]);
						}
						//NON È IL VALORE IN METRO QUADRATO MA IL VALORE PER SUPERFICiE COPERTA
						c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
								+ (c->par_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell)))/settings->sizeCell;
						c->net_radiation_for_soil = ((c->net_radiation_for_subdominated * (1 - LightAbsorb) ) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
								+(c->net_radiation_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell)))/settings->sizeCell;
						Log("Net radiation for soil = %fW/m^2\n", c->net_radiation_for_soil);
						Log("Par for soil = %f molPAR/m^2 month\n", c->par_for_soil);
					}
					else
					{
						Log("More height classes in layer subdominated\n");
						if (c->subdominated_veg_counter == 1 ) //first height class processes
						{
							Log("First height class processed\n");
							c->gapcover[c->top_layer-2] = 1.0 - s->value[CANOPY_COVER_DBHDC];
							Log("GapCover = %f\n", c->gapcover[c->top_layer-2]);
							if(c->gapcover[c->top_layer - 2] < 0.0)
							{
								c->gapcover[c->top_layer - 2] = 0;
								Log("GapCover = %f\n", c->gapcover[c->top_layer - 2]);
							}
							//Par for lower layer
						}
						else if ( c->subdominated_veg_counter > 1 && c->subdominated_veg_counter < c->height_class_in_layer_subdominated_counter )
						{
							Log("Second and more height class processed\n");
							c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
							Log("GapCover = %f\n", c->gapcover[c->top_layer-2]);
							if(c->gapcover[c->top_layer - 2] < 0.0)
							{
								c->gapcover[c->top_layer - 2] = 0;
								Log("GapCover = %f\n", c->gapcover[c->top_layer - 2]);
							}
						}
						else //last height
						{
							Log("Last height class processed\n");
							c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
							Log("GapCover = %f\n", c->gapcover[c->top_layer-2]);
							if(c->gapcover[c->top_layer - 2] < 0.0)
							{
								c->gapcover[c->top_layer - 2] = 0;
								Log("GapCover = %f\n", c->gapcover[c->top_layer - 2]);
							}

							c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
							c->net_radiation_for_soil = ((c->net_radiation_for_subdominated * (1 - LightAbsorb) ) * ((1.0- c->gapcover[c->top_layer-2])
									* settings->sizeCell))/settings->sizeCell;
							Log("Net radiation for soil = %fW/m^2\n", c->net_radiation_for_soil);
							Log("Par for soil = %f molPAR/m^2 day/month\n", c->par_for_soil);
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
							Log("GapCover = %f\n", c->gapcover[c->top_layer - 2]);
						}
						c->par_for_soil = (c->par_for_dominated - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);

						//percentuale scoperta
						c->par_for_soil += (c->par_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell));
						c->par_for_soil /= settings->sizeCell;
						c->net_radiation_for_soil += c->net_radiation_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell);
						c->net_radiation_for_soil /= settings->sizeCell;
						Log("Par for layer = %f molPAR/m^2 day/month\n", c->par_for_soil);
					}
					else
					{
						Log("More height classes in layer dominated\n");
						if (c->subdominated_veg_counter == 1 ) //first height class proccessess
						{
							Log("First height class processed\n");
							c->gapcover[c->top_layer-2] = 1.0 - s->value[CANOPY_COVER_DBHDC];
							Log("GapCover = %f\n", c->gapcover[c->top_layer-2]);
							if(c->gapcover[c->top_layer - 2] < 0.0)
							{
								c->gapcover[c->top_layer - 2] = 0;
								Log("GapCover = %f\n", c->gapcover[c->top_layer - 2]);
							}
						}
						else if (c->subdominated_veg_counter > 1 && c->subdominated_veg_counter < c->height_class_in_layer_subdominated_counter)
						{
							Log("Second and more height class processed\n");
							c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
							Log("GapCover = %f\n", c->gapcover[c->top_layer-2]);
							if(c->gapcover[c->top_layer - 2] < 0.0)
							{
								c->gapcover[c->top_layer - 2] = 0;
								Log("GapCover = %f\n", c->gapcover[c->top_layer - 2]);
							}
						}
						else //last height
						{
							Log("Last height class processed\n");
							c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
							Log("GapCover = %f\n", c->gapcover[c->top_layer-2]);
							if(c->gapcover[c->top_layer - 2] < 0.0)
							{
								c->gapcover[c->top_layer - 2] = 0;
								Log("GapCover = %f\n", c->gapcover[c->top_layer - 2]);
							}

							c->par_for_soil = ((s->value[PAR] - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
							c->net_radiation_for_soil = ((c->net_radiation_for_subdominated * (1 - LightAbsorb) ) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
							Log("Net radiation for soil = %fW/m^2\n", c->net_radiation_for_soil);
							Log("Par for soil = %f molPAR/m^2 day/month\n", c->par_for_soil);
						}
					}
				}
			}
		}
	}
	else
	{
		c->net_radiation_for_soil = c->net_radiation;
		Log("Net Radiation for soil outside growing season = %f \n", c->net_radiation_for_soil);
	}
	if (c->heights[height].z == 0)
	{
		c->par_for_establishment = (c->par_for_soil / c->par);
		Log("PAR OVER CANOPY = %f \n",  c->par);
		Log("PAR FOR SOIL = %f \n",c->par_for_soil);
		Log("Average Light Absorbed for establishment = %f \n", c->par_for_establishment);
	}
}

/**/
void Light_Recruitment (SPECIES *const s, double Av_Yearly_Par_Soil,  double av_yearly_daylength)
{



}
