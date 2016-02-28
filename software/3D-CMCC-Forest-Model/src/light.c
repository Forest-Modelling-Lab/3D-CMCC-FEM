/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"




void Get_light ( SPECIES *const s, CELL *const c, const MET_DATA *const met, int month, int day, int DaysInMonth, int height)
{

	//todo control par for soil and net rad for soil


	double LightTrasmitted;
	double LightAbsorb;


	//double DailyPar;
	//double Gap_Cover = 0;

	Log("\nGET LIGHT ROUTINE\n");
	/*if at least one class is in veg period*/
	if (c->Veg_Counter > 0.0)
	{

		if (settings->spatial == 's')
		{
			LightTrasmitted = (exp(- s->value[K] * met[month].d[day].ndvi_lai));
			//Log("NDVI-LAI = %f \n", met[month].ndvi_lai );
		}
		else
		{
			LightTrasmitted = (exp(- s->value[K] * s->value[LAI]));
			Log("day %d month %d MODEL_LAI = %f \n", day+1, month+1, s->value[LAI] );
		}


		Log("Light Transmitted = %f\n", LightTrasmitted);
		Log("Vertical Percentage of Light Transmitted through this layer = %f %%\n", LightTrasmitted * 100);

		LightAbsorb = 1 - LightTrasmitted;
		Log("Vertical Fraction of Light Absorbed by this layer = %f \n", LightAbsorb);
		Log("Vertical Percentage of Light Absorbed by this layer = %f %%\n", LightAbsorb * 100);



		Log("dominant counter %d \n", c->dominant_veg_counter);
		Log("Top_layer = %d\n", c->top_layer);



		//SE SI È IN FASE VEGETATIVA ALLORA IL PIU ALTO SARÀ ANCHE DOMINANTE PER CIÒ CHE RIGUARDA LA LUCE
		//LIGHT DOMINANT
		if ( c->heights[height].z == c->top_layer )
		{
			Log("**LIGHT DOMINANT**\n");
			Log("Height Classes in Dominant Layer = %d\n", c->height_class_in_layer_dominant_counter);

			//Net Radiation

			Log("Global Solar Radiation = %f MJ/m^2/day\n", met[month].d[day].solar_rad);
			//4 Dec 2012 add Albedo
			//todo check if albedo is necessary
			//AS FOR PAR ALBEDO SHOULD BE TAKEN INTO ACCOUNT ONLY FOR SUN LEAVES THAT REPRESENT 50% OF LEAVES THAT'S WHY MULTPLY FOR
			//ALBEDO/2
			Log("albedo = %f\n", s->value[ALBEDO]);
			//fixme for very low values of solar rad QB causes negative values for Net rad
			c->net_radiation = (/*QA + QB * */ (met[month].d[day].solar_rad * pow (10.0, 6)/ 86400) * (1 - (s->value[ALBEDO]/2.0)));
			Log("Net Radiation = %f W/m^2\n", c->net_radiation);

			/*the no albedo computation is used for gap*/
			c->net_radiation_no_albedo = (/*QA + QB * */(met[month].d[day].solar_rad * pow (10.0, 6)) / 86400);
			Log("Net Radiation NO ALBEDO = %f W/m^2\n", c->net_radiation_no_albedo);


			//4 Dec 2012 add Albedo
			//following BIOME albedo for PAR is 1/3 of albedo
			//The absorbed PAR is calculated similarly except that albedo is 1/3 as large for PAR because less
			//PAR is reflected than net_radiation (Jones 1992)
			//albedo is not considered for lower layers as BIOME doesn't considers albedo for shaded leaves
			//CONSIDERING THAT ALBEDO SHOULD BE CONSIDERED ONLY FOR SUN LEAVES AND THAT SUN LEAVES REPRESENT 50%
			//ALBEDO IS 1/6
			c->par = (met[month].d[day].solar_rad * MOLPAR_MJ) * (1 - (s->value[ALBEDO]/6));
			Log("Par for layer '%d' = %f molPAR/m^2 day\n", c->heights[height].z, c->par);

			/*the no albedo computation is used for gap*/
			c->par_no_albedo = (met[month].d[day].solar_rad * MOLPAR_MJ);
			Log("Par for layer '%d' NO ALBEDO= %f molPAR/m^2 day\n", c->heights[height].z, c->par_no_albedo);

			c->par_over_dominant_canopy = c->par;

			//Apar
			s->value[APAR] = c->par * LightAbsorb;
			Log("Apar for layer '%d' = %f molPAR/m^2 day/month\n", c->heights[height].z, s->value[APAR]);


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

				//Net Radiation for lower layer computed as averaged value between covered and uncovered of dominant layer
				c->net_radiation_for_dominated = ((c->net_radiation * LightTrasmitted) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)+(c->net_radiation_no_albedo * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
				Log("Hourly Net Radiation for lower layer = %f  W/m^2/hour\n", c->net_radiation_for_dominated);

				//PAR for lower layer computed as averaged value between covered and uncovered of dominant layer
				c->par_for_dominated = ((c->par - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) + (c->par_no_albedo * (c->gapcover[c->top_layer] * settings->sizeCell)))/settings->sizeCell;
				Log("Average Par for lower layer = %f molPAR/m^2 day/month\n", c->par_for_dominated);
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
					//FIXME NEW VERSION
					else
					{
						c->net_radiation_for_dominated = (((c->net_radiation * LightTrasmitted) * ((1.0- c->gapcover[c->top_layer]) * settings->sizeCell))
								+ (c->net_radiation_no_albedo * (c->gapcover[c->top_layer] * settings->sizeCell))) / settings->sizeCell;
						c->par_for_dominated = (c->par - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer]) * settings->sizeCell)+
								c->par_no_albedo * (c->gapcover[c->top_layer] * settings->sizeCell)/ settings->sizeCell;
					}
					Log("Net Radiation for lower layer = %f  W/m^2\n", c->net_radiation_for_dominated);
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

				//Net Radiation
				c->net_radiation = c->net_radiation_for_dominated;
				Log("Net Radiation = %f W/m^2n", c->net_radiation);

				c->net_radiation_no_albedo = c->net_radiation * (1.0 - s->value[ALBEDO]/2.0);
				Log("Net Radiation NO ALBEDO = %f W/m^2\n", c->net_radiation_no_albedo);

				//Par
				c->par = c->par_for_dominated;
				Log("Available Par for dominated = %f molPAR/m^2 day/month\n", c->par);

				c->par_no_albedo = c->par * (1 - (s->value[ALBEDO]/6));
				Log("Available Par for dominated NO ALBEDO = %f molPAR/m^2 day/month\n", c->par_no_albedo);

				//Apar
				s->value[APAR] = c->par * LightAbsorb;;
				Log("Apar = %f molPAR/m^2 day/month\n", s->value[APAR]);

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
							//Net Radiation for lower layer with no consideration of reflectance from dominated
							c->net_radiation_for_subdominated = ((c->net_radiation * LightTrasmitted) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+ c->net_radiation * (c->gapcover[c->top_layer-1] * (double)settings->sizeCell)) / settings->sizeCell;
							Log("Net Radiation for lower layer = %f  W/m^2\n", c->net_radiation_for_subdominated);

							c->par_for_subdominated = ((c->par - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+ (c->par * (c->gapcover[c->top_layer-1] * settings->sizeCell)))/settings->sizeCell;
							Log("Par for subdominated = %f molPAR/m^2 day/month\n", c->par_for_subdominated);

						}
						else
						{
							//Net Radiation for lower layer
							c->net_radiation_for_subdominated = ((c->net_radiation * LightTrasmitted) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+ c->net_radiation_no_albedo * (c->gapcover[c->top_layer-1] * (double)settings->sizeCell)) / settings->sizeCell;
							Log("Net Radiation for lower layer with albedo = %f  W/m^2\n", c->net_radiation_for_subdominated);

							c->par_for_subdominated = ((c->par - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
									+ (c->par_no_albedo * (c->gapcover[c->top_layer-1] * settings->sizeCell)))/settings->sizeCell;
							Log("Par for subdominated with albedo = %f molPAR/m^2 day/month\n", c->par_for_subdominated);
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
								c->net_radiation_for_subdominated = (((c->net_radiation * LightTrasmitted) * ((1.0- c->gapcover[c->top_layer-1]) * settings->sizeCell))
										+ (c->net_radiation * (c->gapcover[c->top_layer-1] * settings->sizeCell))) / settings->sizeCell;
								c->par_for_subdominated = ((c->par - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer-1]) * settings->sizeCell)
										+ c->par * (c->gapcover[c->top_layer-1] * settings->sizeCell))/settings->sizeCell;
							}

							//FIXME OLDVERSION
							//Net Radiation for lower layer
							//c->net_radiation_for_subdominated += (((c->net_radiation * LightTrasmitted) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))
							//		+ c->net_radiation * (Gap_Cover * (double)settings->sizeCell)) / settings->sizeCell;
							//c->par_for_subdominated += (c->par - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell);
							//c->par_for_subdominated += c->par * (Gap_Cover * settings->sizeCell);
							//c->par_for_subdominated /= settings->sizeCell;
							Log("Net Radiation for lower layer = %f  W/m^2\n", c->net_radiation_for_subdominated);
							Log("Par for  lower layer = %f molPAR/m^2 day/month\n", c->par_for_subdominated);
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
						Log("GapCover = %f\n", c->gapcover[c->top_layer-1]);

						//Net Radiation for lower layer
						c->net_radiation_for_subdominated = ((c->net_radiation * LightTrasmitted) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) + c->net_radiation
								* (c->gapcover[c->top_layer-1] * (double)settings->sizeCell)) / settings->sizeCell;
						Log("Net Radiation for lower layer = %f  W/m^2\n", c->net_radiation_for_subdominated);

						c->par_for_subdominated = ((c->par - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
								+ (c->par * (c->gapcover[c->top_layer-1] * settings->sizeCell)))/settings->sizeCell;
						Log("Par for lower layer or soil = %f molPAR/m^2 day/month\n", c->par_for_subdominated);
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

							//Net Radiation for lower layer
							c->net_radiation_for_subdominated += (((c->net_radiation * LightTrasmitted) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))
									+ c->net_radiation * (c->gapcover[c->top_layer-1] * (double)settings->sizeCell)) / settings->sizeCell;
							Log("Net Radiation for lower layer = %f  W/m^2\n", c->net_radiation_for_subdominated);

							c->par_for_subdominated += ((c->par - s->value[APAR]) * ((1.0-c->gapcover[c->top_layer-1]) * settings->sizeCell)
									+c->par * (c->gapcover[c->top_layer-1] * settings->sizeCell))/settings->sizeCell;
							Log("Par for  lower layer = %f molPAR/m^2 day/month\n", c->par_for_subdominated);
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
				Log("Net Radiation = % g W/m^2\n", c->net_radiation);

				//Par
				c->par = c->par_for_subdominated;
				Log("Available Par from upper layer for subdominated = %f molPAR/m^2 day/month\n", c->par);

				//Apar
				s->value[APAR] = c->par * LightAbsorb;
				Log("Apar = %f molPAR/m^2 month\n", s->value[APAR]);

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
						c->par_for_soil = ((c->par_for_subdominated - s->value[APAR]) * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)
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

							c->par_for_soil = ((c->par_for_subdominated - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
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

							c->par_for_soil = ((c->par_for_subdominated - s->value[APAR]) * ((1.0- c->gapcover[c->top_layer-2]) * settings->sizeCell))/settings->sizeCell;
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

		Log("Daily solar rad = %f MJ/m^2/day\n", met[month].d[day].solar_rad);

		c->net_radiation = (/*QA + QB * */ (met[month].d[day].solar_rad * pow (10.0, 6))) / met[month].d[day].daylength;
		Log("Net Radiation = %f W/m^2n", c->net_radiation);


		c->par = met[month].d[day].solar_rad * MOLPAR_MJ;
		Log("Par for layer '%d' = %f molPAR/m^2 day\n", c->heights[height].z, c->par);

		c->net_radiation_for_soil = c->net_radiation;

	}


}

/**/
void Get_Light_Recruitment (SPECIES *const s, double Av_Yearly_Par_Soil,  double av_yearly_daylength)
{



}
