/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"




/**/
void Get_Light_Recruitment (SPECIES *const s, float Av_Yearly_Par_Soil,  float av_yearly_daylength)
{



}



void Get_light ( SPECIES *const s, CELL *const c, const MET_DATA *const met, int month, int day, int DaysInMonth, int height)
{

	//todo control par for soil and net rad for soil


	float LightTrasmitted;
	float LightAbsorb;
	float Month_Radiation;
	float Daily_Radiation;
	//float DailyPar;
	//float Gap_Cover = 0;

	Log("\nGET LIGHT ROUTINE\n\n");

	if (settings->time == 'm')
	{
		if (settings->spatial == 's')
		{
			LightTrasmitted = (exp(- s->value[K] * met[month].ndvi_lai));
			Log("NDVI-LAI = %g \n", met[month].ndvi_lai );
		}
		else
		{
			LightTrasmitted = (exp(- s->value[K] * s->value[LAI]));
			Log("day %d month %d MODEL_LAI = %g \n", day, month, s->value[LAI] );
		}
	}
	else
	{
		if (settings->spatial == 's')
		{
			LightTrasmitted = (exp(- s->value[K] * met[month].d[day].ndvi_lai));
			Log("NDVI-LAI = %g \n", met[month].ndvi_lai );
		}
		else
		{
			LightTrasmitted = (exp(- s->value[K] * s->value[LAI]));
			Log("day %d month %d MODEL_LAI = %g \n", day+1, month+1, s->value[LAI] );
		}
	}

	Log("Light Transmitted = %g\n", LightTrasmitted);
	Log("Vertical Percentage of Light Transmitted through this layer = %g %%\n", LightTrasmitted * 100);

	LightAbsorb = 1 - LightTrasmitted;
	Log("Vertical Fraction of Light Absorbed by this layer = %g \n", LightAbsorb);
	Log("Vertical Percentage of Light Absorbed by this layer = %g %%\n", LightAbsorb * 100);



	Log("dominant counter %d \n", c->dominant_veg_counter);
	Log("Top_layer = %d\n", c->top_layer);



	//SE SI È IN FASE VEGETATIVA ALLORA IL PIU ALTO SARÀ ANCHE DOMINANTE PER CIÒ CHE RIGUARDA LA LUCE
	//LIGHT DOMINANT
	if ( c->heights[height].z == c->top_layer )
	{
		Log("**LIGHT DOMINANT**\n");
		Log("Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_dominant_counter);

		//Net Radiation
		if (settings->time == 'm')
		{
			Log("Global Solar Radiation = %g MJ/m^2/day\n", met[month].solar_rad);
			//s->value[NET_RAD] = Get_Net_Radiation (met, years, month, daylength);
			//4 Dec 2012 add Albedo
			c->net_radiation = (QA + QB * (met[month].solar_rad * pow (10.0,  6)) / met[month].daylength) * (1 - s->value[ALBEDO]);
			Log("Hourly Net Radiation = %g W/m^2/hour\n", c->net_radiation);


			Month_Radiation = met[month].solar_rad * DaysInMonth;
			//Log("Monthly Global Solar Radiation = %g MJ/m^2/month\n", Month_Radiation);


			//Par
			//DailyPar = met[month].solar_rad * MOLPAR_MJ;
			//Log("Daily Average Par = %g molPAR/m^2 day\n", DailyPar);

			//4 Dec 2012 add Albedo
			//following BIOME albedo for PAR is 1/3 of albedo
			//The absorbed PAR is calculated similarly except that albedo is 1/3 as large for PAR because less
			//PAR is reflected than net_radiation (Jones 1992)
			//albedo is not considered for lower layers as BIOME doesn't considers albedo for shaded leaves
			c->par = (Month_Radiation * MOLPAR_MJ) * (1 - (s->value[ALBEDO]/3));
			Log("Par for layer '%d' = %g molPAR/m^2 month\n", c->heights[height].z, c->par);
		}
		else
		{
			Log("Global Solar Radiation = %g MJ/m^2/day\n", met[month].d[day].solar_rad);
			//s->value[NET_RAD] = Get_Net_Radiation (met, years, month, daylength);
			//4 Dec 2012 add Albedo
			c->net_radiation = (QA + QB * (met[month].d[day].solar_rad * pow (10.0,  6)) / met[month].d[day].daylength) * (1 - s->value[ALBEDO]);
			Log("Hourly Net Radiation = %g W/m^2/hour\n", c->net_radiation);

			Daily_Radiation = met[month].d[day].solar_rad;

			//Par
			//DailyPar = met[month].solar_rad * MOLPAR_MJ;
			//Log("Daily Average Par = %g molPAR/m^2 day\n", DailyPar);

			//4 Dec 2012 add Albedo
			//following BIOME albedo for PAR is 1/3 of albedo
			//The absorbed PAR is calculated similarly except that albedo is 1/3 as large for PAR because less
			//PAR is reflected than net_radiation (Jones 1992)
			//albedo is not considered for lower layers as BIOME doesn't considers albedo for shaded leaves
			c->par = (Daily_Radiation * MOLPAR_MJ) * (1 - (s->value[ALBEDO]/3));
			Log("Par for layer '%d' = %g molPAR/m^2 day\n", c->heights[height].z, c->par);
		}

		c->par_over_dominant_canopy = c->par;

		//Apar
		s->value[APAR] = c->par * LightAbsorb;
		Log("Apar for layer '%d' = %g molPAR/m^2 day/month\n", c->heights[height].z, s->value[APAR]);

		//only one height class in layer
		if ( c->height_class_in_layer_dominant_counter == 1 )
		{
			Log("Only one height class in dominant light\n");

			c->gapcover[c->top_layer]= 1 - s->value[CANOPY_COVER_DBHDC];

			//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
			c->net_radiation_for_dominated = ((c->net_radiation * LightTrasmitted) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)+(c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
			Log("Hourly Net Radiation for lower layer = %g  W/m^2/hour\n", c->net_radiation_for_dominated);

			//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
			c->par_for_dominated = ((c->par - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) + (c->par * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
			Log("Average Par for lower layer = %g molPAR/m^2 day/month\n", c->par_for_dominated);
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

			}
			else if ( c->dominant_veg_counter > 1 && c->dominant_veg_counter < c->height_class_in_layer_dominant_counter)
			{
				Log("Second but not last height class processing....\n");
				c->gapcover[c->top_layer] -= s->value[CANOPY_COVER_DBHDC];

				if (c->gapcover[c->top_layer] < 0)
				{
					c->gapcover[c->top_layer] = 0.05;
				}
			}
			else //last height
			{
				Log("Last height class processing....\n");
				c->gapcover[c->top_layer] -= s->value[CANOPY_COVER_DBHDC];

				if (c->gapcover[c->top_layer] < 0)
				{
					c->gapcover[c->top_layer] = 0.05;
				}
				if (c->dominant_veg_counter < c->height_class_in_layer_dominant_counter)
				{
					Log("Second but not last height class processing....\n");
					c->gapcover[c->top_layer] -= s->value[CANOPY_COVER_DBHDC];

					if (c->gapcover[c->top_layer] < 0)
					{
						c->gapcover[c->top_layer] = 0.05;
					}
				}
				//FIXME NEW VERSION
				else
				{
					c->net_radiation_for_dominated = (((c->net_radiation * LightTrasmitted) * ((1.0- c->gapcover[c->top_layer]) * settings->sizeCell))
							+ (c->net_radiation * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
					c->par_for_dominated = (c->par - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer]) * settings->sizeCell)+
							c->par * (c->gapcover[c->top_layer] * settings->sizeCell)/ settings->sizeCell;
				}
				//FIXME OLD VERSION
				//Net Radiation for lower layer
				//c->net_radiation_for_dominated = (((c->net_radiation * LightTrasmitted) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))
				//		+ (c->net_radiation * (Gap_Cover * settings->sizeCell))) / settings->sizeCell;
				//Par for lower layer
				//c->par_for_dominated += (c->par - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);
				//c->par_for_dominated += c->par * (Gap_Cover * settings->sizeCell);
				//c->par_for_dominated /= settings->sizeCell;
				Log("Hourly Net Radiation for lower layer = %g  W/m^2/hour\n", c->net_radiation_for_dominated);
				Log("Average Par for lower layer = %g molPAR/m^2 month\n", c->par_for_dominated);
			}
		}
/*
		if( !c->height_class_in_layer_dominated_counter )  //no trees in dominated layer COMPUTE LIGHT FOR SOIL
		{
			c->net_radiation_for_subdominated = c->net_radiation_for_dominated;
			c->par_for_subdominated = c->par_for_dominated;
			if ( !c->height_class_in_layer_subdominated_counter ) //no trees in sub-dominated layer
			{
				if ( c->height_class_in_layer_dominant_counter == 1)
				{
					c->par_for_soil = c->par_for_dominated;
					c->par_for_soil += c->par * (Gap_Cover * settings->sizeCell);
					c->par_for_soil /= settings->sizeCell;

					c->net_radiation_for_soil = c->net_radiation_for_dominated;
					c->net_radiation_for_soil += c->net_radiation * (Gap_Cover * settings->sizeCell);
					c->net_radiation_for_soil /= settings->sizeCell;

					Log("Par for soil = %g molPAR/m^2 day/month\n", c->par_for_soil );
				}
				else
				{
					if (c->dominant_veg_counter == 1)
					{
						c->par_for_soil = c->par_for_dominated;
						c->net_radiation_for_soil = c->net_radiation_for_dominated;
						Log("Par for soil = %g molPAR/m^2 day/month\n", c->par_for_soil / settings->sizeCell);
					}
					else if (c->dominant_veg_counter > 1 && c->dominant_veg_counter < c->height_class_in_layer_dominant_counter)
					{
						c->par_for_soil += c->par_for_dominated;
						c->net_radiation_for_soil = c->net_radiation_for_dominated;
						Log("Par for soil = %g molPAR/m^2 day/month\n", c->par_for_soil / settings->sizeCell);
					}
					else
					{
						c->par_for_soil += c->par_for_dominated * (Gap_Cover * settings->sizeCell);
						c->par_for_soil /= settings->sizeCell;
						c->net_radiation_for_soil += c->net_radiation_for_dominated * (Gap_Cover * settings->sizeCell);
						c->net_radiation_for_soil /= settings->sizeCell;
						Log("Par for soil = %g molPAR/m^2 day/month\n", c->par_for_soil);
					}
				}
			}
		}*/
	}
	else //DOMINATED LAYERS
	{
		//dominated layer
		if (c->heights[height].z == c->top_layer - 1)
		{
			Log("**LIGHT DOMINATED**\n");
			Log("Height Classes in Dominated Layer = %d\n", c->height_class_in_layer_dominated_counter);

			//Net Radiation
			c->net_radiation = c->net_radiation_for_dominated;
			Log("Hourly Net Radiation = %g W/m^2/hour\n", c->net_radiation);

			//Par
			c->par = c->par_for_dominated;
			Log("Available Par from upper layer for dominated = %g molPAR/m^2 day/month\n", c->par);

			//Apar
			s->value[APAR] = c->par * LightAbsorb;
			Log("Apar = %g molPAR/m^2 day/month\n", s->value[APAR]);

			if ( c->dominated_veg_counter >= 3 )
			{
				if ( c->height_class_in_layer_dominated_counter == 1)    //only one height class in layer
				{
					Log("Only one height class in layer dominated\n");
					c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
					Log("gapcover layer %d  = %g\n", c->top_layer, c->gapcover[c->top_layer-1]);

					//percentuale coperta
					//NON È IL VALORE IN METRO QUADRATO MA IL VALORE PER SUPERFICIE COPERTA

					//Net Radiation for lower layer
					c->net_radiation_for_subdominated = ((c->net_radiation * LightTrasmitted) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+ c->net_radiation * (c->gapcover[c->top_layer-1] * (float)settings->sizeCell)) / settings->sizeCell;
					Log("Hourly Net Radiation for lower layer = %g  W/m^2/hour\n", c->net_radiation_for_subdominated);

					c->par_for_subdominated = ((c->par - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+ (c->par * (c->gapcover[c->top_layer-1] * settings->sizeCell)))/settings->sizeCell;

					Log("Par for subdominated = %g molPAR/m^2 day/month\n", c->par_for_subdominated);
				}
				else  //many height classes in this layer
				{
					Log("More height classes in layer dominated\n");
					if (c->dominated_veg_counter == 1 ) //first height class proccessess
					{
						c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
						Log("gapcover layer %d  = %g\n", c->top_layer, c->gapcover[c->top_layer-1]);
					}
					else if ( c->dominated_veg_counter > 1 && c->dominated_veg_counter < c->height_class_in_layer_dominated_counter )
					{
						Log("Second and more height class processed\n");
						c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
						Log("gapcover layer %d  = %g\n", c->top_layer, c->gapcover[c->top_layer-1]);
					}
					else //last height
					{
						Log("Last height class processed\n");
						//Gap_Cover -= s->value[CANOPY_COVER_DBHDC];
						//Log("Free space in layer = %g\n", Gap_Cover);

						if (c->dominant_veg_counter < c->height_class_in_layer_dominant_counter)
						{
							Log("Second but not last height class processing....\n");
							c->gapcover[c->top_layer-1] -= s->value[CANOPY_COVER_DBHDC];

							if (c->gapcover[c->top_layer-1] < 0)
							{
								c->gapcover[c->top_layer-1] = 0.05;
							}
						}
						//FIXME NEW VERSION
						else
						{
							c->net_radiation_for_subdominated = (((c->net_radiation * LightTrasmitted) * ((1.0- c->gapcover[c->top_layer-1]) * settings->sizeCell))
									+ (c->net_radiation * (c->gapcover[c->top_layer-1] * settings->sizeCell))) / settings->sizeCell;
							c->par_for_subdominated = ((c->par - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer-1]) * settings->sizeCell)
									+ c->par * (c->gapcover[c->top_layer-1] * settings->sizeCell))/settings->sizeCell;
						}

						//FIXME OLDVERSION
						//Net Radiation for lower layer
						//c->net_radiation_for_subdominated += (((c->net_radiation * LightTrasmitted) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))
						//		+ c->net_radiation * (Gap_Cover * (float)settings->sizeCell)) / settings->sizeCell;
						//c->par_for_subdominated += (c->par - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);
						//c->par_for_subdominated += c->par * (Gap_Cover * settings->sizeCell);
						//c->par_for_subdominated /= settings->sizeCell;
						Log("Hourly Net Radiation for lower layer = %g  W/m^2/hour\n", c->net_radiation_for_subdominated);
						Log("Par for  lower layer = %g molPAR/m^2 day/month\n", c->par_for_subdominated);
					}
				}
			}
			else
			{
				if ( c->height_class_in_layer_dominated_counter == 1) //first height  class processed
				{
					//percentuale coperta
					//NON È IL VALORE IN METRO QUADRATO MA IL VALORE PER SUPERFICIE COPERTA
					c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];

					//Net Radiation for lower layer
					c->net_radiation_for_subdominated = ((c->net_radiation * LightTrasmitted) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) + c->net_radiation
													* (c->gapcover[c->top_layer-1] * (float)settings->sizeCell)) / settings->sizeCell;
					Log("Hourly Net Radiation for lower layer = %g  W/m^2/hour\n", c->net_radiation_for_subdominated);

					c->par_for_subdominated = ((c->par - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+ (c->par * (c->gapcover[c->top_layer-1] * settings->sizeCell)))/settings->sizeCell;
					Log("Par for lower layer or soil = %g molPAR/m^2 day/month\n", c->par_for_subdominated);
				}
				else
				{
					Log("More height classes in layer dominated\n");
					if (c->dominated_veg_counter == 1 ) //first height class proccessess
					{
						Log("First height class processed\n");
						c->gapcover[c->top_layer-1] = 1.0 - s->value[CANOPY_COVER_DBHDC];
					}
					else if (c->dominated_veg_counter > 1 && c->dominated_veg_counter < c->height_class_in_layer_dominated_counter)
					{
						Log("Second and more height class processed\n");
						c->gapcover[c->top_layer-1] -= s->value[CANOPY_COVER_DBHDC];
					}
					else //last height
					{
						Log("Last height class processed\n");
						c->gapcover[c->top_layer-1] -= s->value[CANOPY_COVER_DBHDC];

						//Net Radiation for lower layer
						c->net_radiation_for_subdominated += (((c->net_radiation * LightTrasmitted) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))
								+ c->net_radiation * (c->gapcover[c->top_layer-1] * (float)settings->sizeCell)) / settings->sizeCell;
						Log("Hourly Net Radiation for lower layer = %g  W/m^2/hour\n", c->net_radiation_for_subdominated);

						c->par_for_subdominated += ((c->par - s->value[APAR]) * ((1.0-c->gapcover[c->top_layer-1]) * settings->sizeCell)
								+c->par * (c->gapcover[c->top_layer-1] * settings->sizeCell))/settings->sizeCell;
						Log("Par for  lower layer = %g molPAR/m^2 day/month\n", c->par_for_subdominated);
					}
				}
			}
		}
		//subdominated layer
		else
		{
			Log("**LIGHT SUB-DOMINATED**\n");
			Log("Height Classes in Sub-Dominated Layer = %d\n", c->height_class_in_layer_subdominated_counter);

			//Net Radiation
			c->net_radiation = c->net_radiation_for_subdominated;
			Log("Hourly Net Radiation = % g W/m^2/hour\n", c->net_radiation);

			//Par
			c->par = c->par_for_subdominated;
			Log("Available Par from upper layer for subdominated = %g molPAR/m^2 day/month\n", c->par);

			//Apar
			s->value[APAR] = c->par * LightAbsorb;
			Log("Apar = %g molPAR/m^2 month\n", s->value[APAR]);

			if ( c->heights_count >= 3 )
			{
				if ( c->height_class_in_layer_subdominated_counter == 1)
				{
					Log("Only one height class in layer subdominated\n");
					//percentuale coperta
					c->gapcover[c->top_layer-2] = 1 - s->value[CANOPY_COVER_DBHDC];
					//NON È IL VALORE IN METRO QUADRATO MA IL VALORE PER SUPERFICiE COPERTA
					c->par_for_soil = ((c->par_for_subdominated - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+ (c->par_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell)))/settings->sizeCell;
					c->net_radiation_for_soil = ((c->net_radiation_for_subdominated * (1 - LightAbsorb) ) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
							+(c->net_radiation_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell)))/settings->sizeCell;
					Log("Net radiation for soil = %gW/m^2/hour \n", c->net_radiation_for_soil);
					Log("Par for soil = %g molPAR/m^2 month\n", c->par_for_soil);
				}
				else
				{
					Log("More height classes in layer subdominated\n");
					if (c->subdominated_veg_counter == 1 ) //first height class processes
					{
						Log("First height class processed\n");
						c->gapcover[c->top_layer-2] = 1.0 - s->value[CANOPY_COVER_DBHDC];
						//Par for lower layer
					}
					else if ( c->subdominated_veg_counter > 1 && c->subdominated_veg_counter < c->height_class_in_layer_subdominated_counter )
					{
						Log("Second and more height class processed\n");
						c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
					}
					else //last height
					{
						Log("Last height class processed\n");
						c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
						c->par_for_soil = ((c->par_for_subdominated - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
						c->net_radiation_for_soil = ((c->net_radiation_for_subdominated * (1 - LightAbsorb) ) * ((1.0- c->gapcover[c->top_layer-2])
								* settings->sizeCell))/settings->sizeCell;
						Log("Net radiation for soil = %gW/m^2/hour \n", c->net_radiation_for_soil);
						Log("Par for soil = %g molPAR/m^2 day/month\n", c->par_for_soil);
					}
				}
			}
			else
			{
				if ( c->height_class_in_layer_subdominated_counter == 1) //first height  class processed
				{
					//percentuale coperta
					c->gapcover[c->top_layer-2] = 1.0 - s->value[CANOPY_COVER_DBHDC];
					//NON È IL VALORE IN METRO QUADRATO MA IL VALORE PER SUPERFICIE COPERTA
					c->par_for_soil = (c->par_for_dominated - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);

					//percentuale scoperta
					c->par_for_soil += (c->par_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell));
					c->par_for_soil /= settings->sizeCell;
					c->net_radiation_for_soil += c->net_radiation_for_dominated * (c->gapcover[c->top_layer-2] * settings->sizeCell);
					c->net_radiation_for_soil /= settings->sizeCell;
					Log("Par for layer = %g molPAR/m^2 day/month\n", c->par_for_soil);
				}
				else
				{
					Log("More height classes in layer dominated\n");
					if (c->subdominated_veg_counter == 1 ) //first height class proccessess
					{
						Log("First height class processed\n");
						c->gapcover[c->top_layer-2] = 1.0 - s->value[CANOPY_COVER_DBHDC];
					}
					else if (c->subdominated_veg_counter > 1 && c->subdominated_veg_counter < c->height_class_in_layer_subdominated_counter)
					{
						Log("Second and more height class processed\n");
						c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
					}
					else //last height
					{
						Log("Last height class processed\n");
						c->gapcover[c->top_layer-2] -= s->value[CANOPY_COVER_DBHDC];
						c->par_for_soil = ((c->par_for_subdominated - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
						c->net_radiation_for_soil = ((c->net_radiation_for_subdominated * (1 - LightAbsorb) ) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
						Log("Net radiation for soil = %gW/m^2/hour \n", c->net_radiation_for_soil);
						Log("Par for soil = %g molPAR/m^2 day/month\n", c->par_for_soil);
					}
				}
			}
		}
	}
}
