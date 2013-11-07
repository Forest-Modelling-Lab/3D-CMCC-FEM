/*
 * canopy_transpiration2.c
 *
 *  Created on: 07/nov/2013
 *      Author: alessio
 */




/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


extern void Get_canopy_transpiration (SPECIES *const s,  CELL *const c, const MET_DATA *const met, int month, int day, int DaysInMonth, float vpd, int height, int age, int species)
{

	int i;

	static float CanCond;
	static float CanopyTranspiration;
	//float rhoAir = 1.2;       // density of air, kg/m3
	float rhoAir;
	static float defTerm;
	static float duv;                      // 'div' in 3pg
	static float Etransp;
	static float DailyTransp;
	float alpha_evapo = 0.65;
	float beta_evapo = 0.95;
	float t1, t2;
	//static float MonthTransp;

	//CANOPY TRASPIRATION FOLLOWING BIOME APPROACH
	float tav_k; //Average temperature in Kelvin
	float rr; //resistance to radiative heat transfer through air
	float gl_bl_corr;  //leaf boundary layer conductance corrected for temperature and pressure
	float gl_c_corr; //leaf cuticolar conductance corrected for temperature and pressure
	float ppfd_sun;  //photosynthetic photon flux density (umol/m2/s) PPFD for 1/2 stomatal closure
	float final_ppfd_sun;
	float gl_s_sun;   //maximum stomatal condictance

	float gl_e_wv;
	float gl_t_wv_sun;
	float gc_e_wv;
	float gl_sh;
	float lai;
	float gc_sh;
	float rv;  //resistance to latent heat transfer
	float rh;  //resistance to convective heat transfer
	float swabs; //absorbed shortwave radiation in W/m^2
	float rhr;
	float lhvap;
	float dt = 0.2; //set the temperature offset for slope calculation
	float pvs1, pvs2;
	float esse;
	float evap;
	float evap_dayl ; //calculate the time required to evaporate all the canopy water

	Log("\nGET_CANOPY_TRANSPIRATION_ROUTINE \n\n");

	if (settings->time == 'd')
	{
		rhoAir = met[month].d[day].rho_air;
	}
	else
	{
		rhoAir = met[month].rho_air;
	}


	//Veg period
	if (s->counter[VEG_UNVEG] == 1)
	{
		/*Canopy Conductance*/
		//todo get maximum stomatal conductance from biome data instead canopy max conductance

		if (settings->time == 'm')
		{
			if (settings->spatial == 's')
			{
				CanCond = s->value[MAXCOND] * s->value[PHYS_MOD] * Minimum(1.0, met[month].ndvi_lai / s->value[LAIGCX]);
				//Log("MAXCOND da biome = %g\n", 0.006 * met[month].ndvi_lai);
				//Log("Cancond biome = %g\n", (0.006 * met[month].ndvi_lai * c->gcorr));
				//Log("CanCond 3PG = %g\n", CanCond);
			}
			else
			{
				CanCond = s->value[MAXCOND] * s->value[PHYS_MOD] * Minimum(1.0, s->value[LAI]  / s->value[LAIGCX]);
				//Log("MAXCOND da biome = %g\n", 0.006 * s->value[LAI]);
				//Log("Cancond biome = %g\n", (0.006 * s->value[LAI] * c->gcorr));
				//Log("CanCond 3PG = %g\n", CanCond);
			}
		}
		else
		{
			if (settings->spatial == 's')
			{
				CanCond = s->value[MAXCOND] * s->value[PHYS_MOD] * Minimum(1.0, met[month].d[day].ndvi_lai / s->value[LAIGCX]);
				//Log("MAXCOND da biome = %g\n", 0.006 * met[month].ndvi_lai);
				//Log("Cancond biome = %g\n", (0.006 * met[month].ndvi_lai * c->gcorr));
				//Log("CanCond 3PG = %g\n", CanCond);
			}
			else
			{
				CanCond = s->value[MAXCOND] * s->value[PHYS_MOD] * Minimum(1.0, s->value[LAI]  / s->value[LAIGCX]);
				//Log("MAXCOND da biome = %g\n", 0.006 * s->value[LAI]);
				//Log("Cancond biome = %g\n", (0.006 * s->value[LAI] * c->gcorr));
				//Log("CanCond 3PG = %g\n", CanCond);
			}
		}

		//Log("Canopy Conductance  = %g\n", CanCond);

		/*Canopy Transpiration*/
		//todo change all functions with BIOME's

		// Penman-Monteith equation for computing canopy transpiration
		// in kg/m2/day, which is converted to mm/day.
		// The following are constants in the PM formula (Landsberg & Gower, 1997)

		defTerm = rhoAir * LAMBDA * (vpd * VPDCONV) * s->value[BLCOND];
		//Log("defTerm = %g\n", defTerm);
		duv = (1.0 + E20 + s->value[BLCOND] / CanCond);
		//Log("duv = %g\n", duv);


		if(settings->time == 'm')
		{
			switch (c->monthly_layer_number)
			{
			case 1:
				//convert radiation from hours to seconds
				Etransp = (E20 * (c->net_radiation/3600.0) + defTerm) / duv;  // in J/m2/s
				Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
				Log("Net radiation = %g W/m^2/hour\n", c->net_radiation);
				break;
			case 2:
				if ( c->heights[height].z == c->top_layer )
				{
					//convert radiation from hours to seconds
					Etransp = (E20 * (c->net_radiation/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %g \n", c->net_radiation);
				}
				else
				{
					Etransp = (E20 * (c->net_radiation_for_dominated/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %g \n", c->net_radiation);
				}
				break;
			case 3:
				if ( c->heights[height].z == c->top_layer )
				{
					//convert radiation from hours to seconds
					Etransp = (E20 * (c->net_radiation/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %g \n", c->net_radiation);
				}
				if ( c->heights[height].z == c->top_layer - 1 )
				{
					Etransp = (E20 * (c->net_radiation_for_dominated/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %g \n", c->net_radiation);
				}
				else
				{
					Etransp = (E20 * (c->net_radiation_for_subdominated/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for subdominant layer = %g J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %g \n", c->net_radiation);
				}

				break;
			}
			//FIXME BEING ALL IN SECONDS SHOUD I MULPLY FOR SECONDS IN DAYLENGTH NON FOR HOURS IN DAYLENGTH??
			CanopyTranspiration = Etransp / LAMBDA * (met[month].daylength * 3600.0);         // converted to kg-mm H2o/m2/day

		}
		else
		{
			Log("daily vegetative layer number = %d\n", c->daily_layer_number);
			switch (c->daily_layer_number)
			{
			case 1:
				//convert radiation from hours to seconds
				Etransp = (E20 * (c->net_radiation/3600.0) + defTerm) / duv;  // in J/m2/s
				Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
				Log("NET RADIATION = %g \n", c->net_radiation);
				break;
			case 2:
				if ( c->heights[height].z == c->top_layer )
				{
					//convert radiation from hours to seconds
					Etransp = (E20 * (c->net_radiation/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %g \n", c->net_radiation);
				}
				else
				{
					Etransp = (E20 * (c->net_radiation_for_dominated/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominated layer = %g J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %g \n", c->net_radiation_for_dominated);
				}
				break;
			case 3:
				if ( c->heights[height].z == c->top_layer )
				{
					//convert radiation from hours to seconds
					Etransp = (E20 * (c->net_radiation/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %g \n", c->net_radiation);
				}
				if ( c->heights[height].z == c->top_layer - 1 )
				{
					Etransp = (E20 * (c->net_radiation_for_dominated/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominated layer = %g J/m^2/sec\n", Etransp);
					Log("Net radiation = %g \n", c->net_radiation);
				}
				else
				{
					Etransp = (E20 * (c->net_radiation_for_subdominated/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for subdominant layer = %g J/m^2/sec\n", Etransp);
					Log("Net radiation = %g \n", c->net_radiation_for_subdominated);
				}

				break;
			}


			//canopy transpiration is computed in seconds needs to be converted in hours of sun then day
			CanopyTranspiration = Etransp / LAMBDA * (met[month].d[day].daylength * 3600.0);   // converted to kg-mm H2o/m2/day
			Log("Daily Canopy Transpiration = %g mm-Kg H2o/m^2/day\n", CanopyTranspiration);
			DailyTransp = CanopyTranspiration;
		}
	}

	/*daily*/
	if (settings->time == 'd' )
	{
		if (s->counter[VEG_UNVEG] == 1)
		{
			/*dominant layer*/
			if (c->heights[height].z == c->top_layer)
			{
				s->value[DAILY_TRANSP] = CanopyTranspiration * s->value[CANOPY_COVER_DBHDC];
				Log("Canopy trasnpiration = %g mm\n", s->value[DAILY_TRANSP]);
				c->daily_c_transp[c->top_layer] += s->value[DAILY_TRANSP];
				Log("Canopy transpiration from dominant layer = %g mm \n", c->daily_c_transp[c->top_layer]);
				/*last height dominant class processed*/
				if (c->dominant_veg_counter == c->height_class_in_layer_dominant_counter)
				{
					/*control*/
					if (c->available_soil_water < c->daily_c_transp[c->top_layer])
					{
						Log("ATTENTION DAILY TRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
						c->daily_c_transp[c->top_layer] = c->available_soil_water;
					}
				}
			}
			/*dominated*/
			else
			{
				/*dominated layer*/
				if (c->heights[height].z == c->top_layer-1)
				{
					s->value[DAILY_TRANSP] =  CanopyTranspiration * s->value[CANOPY_COVER_DBHDC];
					Log("Canopy trasnpiration = %g mm\n", s->value[DAILY_TRANSP]);
					c->daily_c_transp[c->top_layer-1] += s->value[DAILY_TRANSP];
					Log("Canopy transpiration  water from dominated layer = %g mm \n", c->daily_c_transp[c->top_layer-1]);
					/*last height dominated class processed*/
					if (c->dominated_veg_counter == c->height_class_in_layer_dominated_counter)
					{
						/*control*/
						if (c->available_soil_water < c->daily_c_transp[c->top_layer-1])
						{
							Log("ATTENTION DAILY TRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
							c->daily_c_transp[c->top_layer-1] = c->available_soil_water;
						}
					}
				}
				/*subdominated layer*/
				else
				{
					s->value[DAILY_TRANSP] =  CanopyTranspiration * s->value[CANOPY_COVER_DBHDC];
					Log("Canopy trasnpiration = %g mm\n", s->value[DAILY_TRANSP]);
					c->daily_c_transp[c->top_layer-2] += s->value[DAILY_TRANSP];
					Log("Canopy transpiration  water from dominated layer = %g mm \n", c->daily_c_transp[c->top_layer-2]);
					/*last height subdominated class processed*/
					if (c->subdominated_veg_counter == c->height_class_in_layer_subdominated_counter)
					{
						/*control*/
						if (c->available_soil_water < c->daily_c_transp[c->top_layer-2])
						{
							Log("ATTENTION DAILY TRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
							c->daily_c_transp[c->top_layer-2] = c->available_soil_water;
						}
					}
				}
			}
		}
		else
		{
			s->value[DAILY_TRANSP] = 0.0;
			Log("Canopy interception = %g mm\n", s->value[DAILY_TRANSP]);
			c->daily_c_transp[c->heights[height].z] = 0.0;
			Log("Intercepted water from dominant layer = %g mm \n", c->daily_c_transp[c->heights[height].z]);
			c->water_to_atmosphere = 0.0 ;
			Log("water to atmosphere = %g mm\n", c->water_to_atmosphere);
		}
	}
	/*no interception if tavg < 0 (snow), or outside growing season*/

	//fixme do it as in daily version and following daily transpiration!!!
	/*monthly*/
	/*
	if (settings->time == 'm' )
	{
		if (met[month].tavg > 0.0 && met[month].rain > 0.0)
		{
			if (s->counter[VEG_UNVEG] == 1)
			{
				if (c->heights[height].z == c->top_layer)
				{
					s->value[RAIN_INTERCEPTED] = (met[month].rain * s->value[FRAC_RAIN_INTERC]) * s->value[CANOPY_COVER_DBHDC];
					Log("Canopy interception = %g mm\n", s->value[RAIN_INTERCEPTED]);
					c->daily_c_int[c->top_layer] += s->value[RAIN_INTERCEPTED];
					//fixme do the same thing for canopy transpiration!!!!
					//last height dominant class processed
					if (c->dominant_veg_counter == c->height_class_in_layer_dominant_counter)
					{
						//compute effective amount of water intercepted
						c->daily_c_int[c->top_layer] *= met[month].rain * c->gapcover[c->top_layer];
						Log("intercepted water from top layer = %g mm \n", c->daily_c_int[c->top_layer]);
					}
				}
			}
			else
			{
				s->value[RAIN_INTERCEPTED] = 0.0;
				Log("Canopy interception = %g mm\n", s->value[RAIN_INTERCEPTED]);
				c->daily_c_int[c->top_layer] = 0.0;
				Log("Intercepted water from top layer = %g mm \n", c->daily_c_int[c->top_layer]);
			}
		}
		else
		{
			s->value[RAIN_INTERCEPTED] = 0.0;
			Log("Canopy interception = %g mm\n", s->value[RAIN_INTERCEPTED]);
			c->daily_c_int[c->top_layer] = 0.0;
			Log("intercepted water from top layer = %g mm \n", c->daily_c_int[c->top_layer]);
		}
	}
	*/








	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*BIOME VERSION*/

	/* leaf boundary-layer conductance */
		gl_bl_corr = s->value[BLCOND]* c->gcorr;
		//Log("gl_bl_corr = %g\n", gl_bl_corr);

		/* leaf cuticular conductance */
		//todo insert CCOND into species.txt
		gl_c_corr = /*s->value[CCOND]*/ 0.00006 * c->gcorr;
		//Log("gl_c_corr = %g\n", gl_c_corr);


		/* leaf stomatal conductance: first generate multipliers, then apply them
		to maximum stomatal conductance */
		/* calculate stomatal conductance radiation multiplier: */


		/* photosynthetic photon flux density  */
		ppfd_sun = s->value[APAR] * EPAR;
		//Log("ppfd_sun = %g\n", ppfd_sun);

		/* photosynthetic photon flux density conductance control */
		ppfd_sun /= (PPFD50 + ppfd_sun);
		//Log("ppfd_sun = %g\n", ppfd_sun);


		/* apply all multipliers to the maximum stomatal conductance */
		//Currently I will use the 3-PG modifiers
		final_ppfd_sun = ppfd_sun * s->value[F_SW] * s->value[F_T] * s->value[F_VPD];
		//Log("final_ppfd_sun = %g\n", final_ppfd_sun);
		gl_s_sun = /*s->value[MAX_STOM_COND]*/ 0.006 * final_ppfd_sun * c->gcorr;
		//Log("gl_s_sun = %g\n", gl_s_sun);

		/* Leaf conductance to evaporated water vapor, per unit projected LAI */

		gl_e_wv = gl_bl_corr;
		//Log("gl_e_wv = %g\n", gl_e_wv);



		/* Leaf conductance to transpired water vapor, per unit projected
		LAI.  This formula is derived from stomatal and cuticular conductances
		in parallel with each other, and both in series with leaf boundary
		layer conductance. */

		gl_t_wv_sun = (gl_bl_corr * (gl_s_sun))/(gl_bl_corr + gl_s_sun + gl_c_corr);
		//Log("gl_t_wv_sun = %g\n", gl_t_wv_sun);

		/* Leaf conductance to sensible heat, per unit all-sided LAI */

		gl_sh = gl_bl_corr;
		//Log("gl_sh = %g\n", gl_sh);


		if (settings->time == 'm')
		{
			if (settings->spatial == 'u')
			{
				lai = s->value[LAI];
			}
			else
			{
				lai = met[month].ndvi_lai;
			}
		}
		else
		{
			if (settings->spatial == 'u')
			{
				lai = s->value[LAI];
			}
			else
			{
				lai = met[month].d[day].ndvi_lai;
			}
		}

		/* Canopy conductance to evaporated water vapor */

		gc_e_wv = gl_e_wv * lai;
		//Log("LAI = %g\n", lai);
		//Log("gc_e_wv = %g\n", gc_e_wv);

		/* Canopy conductane to sensible heat */

		gc_sh = gl_sh * lai;
		//Log("gc_sh = %g\n", gc_sh);


		rv = 1.0/gc_e_wv;
		//Log("rv = %g\n", rv);

		rh = 1.0/gc_sh;
		//Log("rh = %g\n", rh);
		//todo make a better function using values from light.c

		swabs = c->net_radiation * (1 -exp (-s->value[K]+lai));
		//Log("swabs = %g\n", swabs);


		/*PENMAN-MONTEITH*/
		/* call penman-monteith function, returns e in kg/m2/s */

		//todo per finire la parte di BIOME devo inserire anche la parte di VPD
		/* assign tavg (Celsius) and tav_k (Kelvins) */
		if (settings->time == 'm')
		{
			if (met[month].tday == NO_DATA)
			{
				tav_k = met[month].tavg + 273.15;
				/* calculate density of air (rho) as a function of air temperature */
				rhoAir = 1.292 - (0.00428 * met[month].tavg);
				/* calculate latent heat of vaporization as a function of ta */
				lhvap = 2.5023e6 - 2430.54 * met[month].tavg;
				/* calculate temperature offsets for slope estimate */
				t1 = met[month].tavg+dt;
				t2 = met[month].tavg-dt;
			}
			else
			{
				tav_k = met[month].tday + 273.15;
				/* calculate density of air (rho) as a function of air temperature */
				rhoAir = 1.292 - (0.00428 * met[month].tday);
				/* calculate latent heat of vaporization as a function of ta */
				lhvap = 2.5023e6 - 2430.54 * met[month].tday;
				/* calculate temperature offsets for slope estimate */
				t1 = met[month].tday+dt;
				t2 = met[month].tday-dt;
			}
		}
		else
		{
			if (met[month].tday == NO_DATA)
			{
				tav_k = met[month].d[day].tavg + 273.15;
				/* calculate density of air (rho) as a function of air temperature */
				rhoAir = 1.292 - (0.00428 * met[month].d[day].tavg);
				/* calculate latent heat of vaporization as a function of ta */
				lhvap = 2.5023e6 - 2430.54 * met[month].d[day].tavg;
				/* calculate temperature offsets for slope estimate */
				t1 = met[month].d[day].tavg+dt;
				t2 = met[month].d[day].tavg-dt;
			}
			else
			{
				tav_k = met[month].d[day].tday + 273.15;
				/* calculate density of air (rho) as a function of air temperature */
				rhoAir = 1.292 - (0.00428 * met[month].d[day].tday);
				/* calculate latent heat of vaporization as a function of ta */
				lhvap = 2.5023e6 - 2430.54 * met[month].d[day].tday;
				/* calculate temperature offsets for slope estimate */
				t1 = met[month].d[day].tday+dt;
				t2 = met[month].d[day].tday-dt;
			}
		}



		/* calculate resistance to radiative heat transfer through air, rr */
		rr = rhoAir * CP / (4.0 * SBC * (tav_k*tav_k*tav_k));

		/* calculate combined resistance to convective and radiative heat transfer,
	    parallel resistances : rhr = (rh * rr) / (rh + rr) */
		rhr = (rh * rr) / (rh + rr);

		/* calculate saturation vapor pressures at t1 and t2 */
		pvs1 = 610.7 * exp(17.38 * t1 / (239.0 + t1));
		pvs2 = 610.7 * exp(17.38 * t2 / (239.0 + t2));

		/* calculate slope of pvs vs. T curve, at ta */
		esse = (pvs1-pvs2) / (t1-t2);

		/* calculate evaporation, in W/m^2  */
		evap = ( ( esse * swabs ) + ( rhoAir * CP * vpd / rhr ) ) /
				( ( ( c->air_pressure * CP * rv ) / ( lhvap * EPS * rhr ) ) + esse );

		/* covert evaporation into kg/m^2/s */
		evap /= lhvap;

		/* convert evaporation into kg/m^2/day */
		evap *= 84600;
		//Log("Daily transpiration from biome = %g\n", evap);

		if (settings->time == 'm')
		{
			/* convert evaporation into kg/m^2/month */
			evap = evap * DaysInMonth;
			//Log("Montlhy transpiration from biome = %g\n", evap);
		}
		else
		{
			/* convert evaporation into kg/m^2/month */
			//Log("Daily transpiration from biome = %g\n", evap);
			//Log("Montlhy transpiration from biome = %g\n", evap*DaysInMonth);
		}


		/* calculate the time required to evaporate all the canopy water */
		evap_dayl = met[month].d[day].rain/e;
		Log("evap_dayl = %g\n", evap_dayl);


}


















































































