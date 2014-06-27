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


extern void Get_canopy_transpiration (SPECIES *const s,  CELL *const c, const MET_DATA *const met, int month, int day, int DaysInMonth, double vpd, int height, int age, int species)
{

	int i;

	static double CanCond;
	static double CanopyTranspiration;
	//double rhoAir = 1.2;       // density of air, kg/m3
	double rhoAir;
	static double defTerm;
	static double duv;                      // 'div' in 3pg
	static double Etransp;
	static double DailyTransp;
	//double alpha_evapo = 0.65;
	//double beta_evapo = 0.95;
	double t1, t2;
	//static double MonthTransp;

	//CANOPY TRASPIRATION FOLLOWING BIOME APPROACH
	double tav_k; //Average temperature in Kelvin
	double rr; //resistance to radiative heat transfer through air
	double gl_bl_corr;  //leaf boundary layer conductance corrected for temperature and pressure
	double gl_c_corr; //leaf cuticolar conductance corrected for temperature and pressure
	double ppfd_sun;  //photosynthetic photon flux density (umol/m2/s) PPFD for 1/2 stomatal closure
	double final_ppfd_sun;
	double gl_s_sun;   //maximum stomatal condictance

	double gl_e_wv;
	double gl_t_wv_sun;
	double gc_e_wv;
	double gl_sh;
	double lai;
	double gc_sh;
	double rv;  //resistance to latent heat transfer
	double rh;  //resistance to convective heat transfer
	double swabs; //absorbed shortwave radiation in W/m^2
	double rhr;
	double lhvap;
	double dt = 0.2; //set the temperature offset for slope calculation
	double pvs1, pvs2;
	double esse;
	double evap;
	double evap_dayl ; //calculate the time required to evaporate all the canopy water

	Log("\nGET_CANOPY_TRANSPIRATION_ROUTINE\n");

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
				//Log("MAXCOND da biome = %f\n", 0.006 * met[month].ndvi_lai);
				//Log("Cancond biome = %f\n", (0.006 * met[month].ndvi_lai * c->gcorr));
				//Log("CanCond 3PG = %f\n", CanCond);
			}
			else
			{
				CanCond = s->value[MAXCOND] * s->value[PHYS_MOD] * Minimum(1.0, s->value[LAI]  / s->value[LAIGCX]);
				//Log("MAXCOND da biome = %f\n", 0.006 * s->value[LAI]);
				//Log("Cancond biome = %f\n", (0.006 * s->value[LAI] * c->gcorr));
				//Log("CanCond 3PG = %f\n", CanCond);
			}
		}
		else
		{
			if (settings->spatial == 's')
			{
				CanCond = s->value[MAXCOND] * s->value[PHYS_MOD] * Minimum(1.0, met[month].d[day].ndvi_lai / s->value[LAIGCX]);
				//Log("MAXCOND da biome = %f\n", 0.006 * met[month].ndvi_lai);
				//Log("Cancond biome = %f\n", (0.006 * met[month].ndvi_lai * c->gcorr));
				//Log("CanCond 3PG = %f\n", CanCond);
			}
			else
			{
				CanCond = s->value[MAXCOND] * s->value[PHYS_MOD] * Minimum(1.0, s->value[LAI]  / s->value[LAIGCX]);
				//Log("MAXCOND da biome = %f\n", 0.006 * s->value[LAI]);
				//Log("Cancond biome = %f\n", (0.006 * s->value[LAI] * c->gcorr));
				//Log("CanCond 3PG = %f\n", CanCond);
			}
		}

		//Log("Canopy Conductance  = %f\n", CanCond);

		/*Canopy Transpiration*/
		//todo change all functions with BIOME's

		// Penman-Monteith equation for computing canopy transpiration
		// in kg/m2/day, which is converted to mm/day.
		// The following are constants in the PM formula (Landsberg & Gower, 1997)

		defTerm = rhoAir * LAMBDA * (vpd * VPDCONV) * s->value[BLCOND];
		//Log("defTerm = %f\n", defTerm);
		duv = (1.0 + E20 + s->value[BLCOND] / CanCond);
		//Log("duv = %f\n", duv);


		if(settings->time == 'm')
		{
			switch (c->monthly_layer_number)
			{
			case 1:
				//convert radiation from hours to seconds
				Etransp = (E20 * (c->net_radiation/3600.0) + defTerm) / duv;  // in J/m2/s
				Log("Etransp for dominant layer = %f J/m^2/sec\n", Etransp);
				Log("Net radiation = %f W/m^2/hour\n", c->net_radiation);
				break;
			case 2:
				if ( c->heights[height].z == c->top_layer )
				{
					//convert radiation from hours to seconds
					Etransp = (E20 * (c->net_radiation/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominant layer = %f J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %f \n", c->net_radiation);
				}
				else
				{
					Etransp = (E20 * (c->net_radiation_for_dominated/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominant layer = %f J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %f \n", c->net_radiation);
				}
				break;
			case 3:
				if ( c->heights[height].z == c->top_layer )
				{
					//convert radiation from hours to seconds
					Etransp = (E20 * (c->net_radiation/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominant layer = %f J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %f \n", c->net_radiation);
				}
				if ( c->heights[height].z == c->top_layer - 1 )
				{
					Etransp = (E20 * (c->net_radiation_for_dominated/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominant layer = %f J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %f \n", c->net_radiation);
				}
				else
				{
					Etransp = (E20 * (c->net_radiation_for_subdominated/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for subdominant layer = %f J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %f \n", c->net_radiation);
				}

				break;
			}
			CanopyTranspiration = Etransp / LAMBDA * (met[month].daylength * 3600.0);         // converted to kg-mm H2o/m2/day

		}
		else
		{
			switch (c->daily_layer_number)
			{
			case 1:
				//convert radiation from hours to seconds
				Etransp = (E20 * (c->net_radiation/3600.0) + defTerm) / duv;  // in J/m2/s
				Log("Etransp for dominant layer = %f J/m^2/sec\n", Etransp);
				Log("NET RADIATION = %f \n", c->net_radiation);
				break;
			case 2:
				if ( c->heights[height].z == c->top_layer )
				{
					//convert radiation from hours to seconds
					Etransp = (E20 * (c->net_radiation/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominant layer = %f J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %f \n", c->net_radiation);
				}
				else
				{
					Etransp = (E20 * (c->net_radiation_for_dominated/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominated layer = %f J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %f \n", c->net_radiation_for_dominated);
				}
				break;
			case 3:
				if ( c->heights[height].z == c->top_layer )
				{
					//convert radiation from hours to seconds
					Etransp = (E20 * (c->net_radiation/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominant layer = %f J/m^2/sec\n", Etransp);
					Log("NET RADIATION = %f \n", c->net_radiation);
				}
				if ( c->heights[height].z == c->top_layer - 1 )
				{
					Etransp = (E20 * (c->net_radiation_for_dominated/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for dominated layer = %f J/m^2/sec\n", Etransp);
					Log("Net radiation = %f \n", c->net_radiation);
				}
				else
				{
					Etransp = (E20 * (c->net_radiation_for_subdominated/3600.0) + defTerm) / duv;  // in J/m2/s
					Log("Etransp for subdominant layer = %f J/m^2/sec\n", Etransp);
					Log("Net radiation = %f \n", c->net_radiation_for_subdominated);
				}

				break;
			}


			//canopy transpiration is computed in seconds needs to be converted in hours of sun then day
			CanopyTranspiration = Etransp / LAMBDA * (met[month].d[day].daylength * 3600.0);   // converted to kg-mm H2o/m2/day
			Log("Daily Canopy Transpiration = %f mm-Kg H2o/m^2/day\n", CanopyTranspiration);
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
				Log("Canopy trasnpiration = %f mm\n", s->value[DAILY_TRANSP]);
				c->daily_c_transp[c->top_layer] += s->value[DAILY_TRANSP];
				Log("Canopy transpiration from dominant layer = %f mm \n", c->daily_c_transp[c->top_layer]);
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
					Log("Canopy trasnpiration = %f mm\n", s->value[DAILY_TRANSP]);
					c->daily_c_transp[c->top_layer-1] += s->value[DAILY_TRANSP];
					Log("Canopy transpiration  water from dominated layer = %f mm \n", c->daily_c_transp[c->top_layer-1]);
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
					Log("Canopy trasnpiration = %f mm\n", s->value[DAILY_TRANSP]);
					c->daily_c_transp[c->top_layer-2] += s->value[DAILY_TRANSP];
					Log("Canopy transpiration  water from dominated layer = %f mm \n", c->daily_c_transp[c->top_layer-2]);
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
			Log("Canopy transpiration = %f mm\n", s->value[DAILY_TRANSP]);
			c->daily_c_transp[c->heights[height].z] = 0.0;
			Log("Traspirated water from layer %d = %f mm \n", c->heights[height].z, c->daily_c_transp[c->heights[height].z]);
		}

		/*compute total daily transpiration*/
		c->daily_tot_c_transp += c->daily_c_transp[c->heights[height].z];
		Log("Daily total canopy transpiration = %f \n", c->daily_tot_c_transp);

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
					Log("Canopy interception = %f mm\n", s->value[RAIN_INTERCEPTED]);
					c->daily_c_int[c->top_layer] += s->value[RAIN_INTERCEPTED];
					//fixme do the same thing for canopy transpiration!!!!
					//last height dominant class processed
					if (c->dominant_veg_counter == c->height_class_in_layer_dominant_counter)
					{
						//compute effective amount of water intercepted
						c->daily_c_int[c->top_layer] *= met[month].rain * c->gapcover[c->top_layer];
						Log("intercepted water from top layer = %f mm \n", c->daily_c_int[c->top_layer]);
					}
				}
			}
			else
			{
				s->value[RAIN_INTERCEPTED] = 0.0;
				Log("Canopy interception = %f mm\n", s->value[RAIN_INTERCEPTED]);
				c->daily_c_int[c->top_layer] = 0.0;
				Log("Intercepted water from top layer = %f mm \n", c->daily_c_int[c->top_layer]);
			}
		}
		else
		{
			s->value[RAIN_INTERCEPTED] = 0.0;
			Log("Canopy interception = %f mm\n", s->value[RAIN_INTERCEPTED]);
			c->daily_c_int[c->top_layer] = 0.0;
			Log("intercepted water from top layer = %f mm \n", c->daily_c_int[c->top_layer]);
		}
	}
	*/








	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/*BIOME VERSION*/

	/* leaf boundary-layer conductance */
		gl_bl_corr = s->value[BLCOND]* c->gcorr;
		//Log("gl_bl_corr = %f\n", gl_bl_corr);

		/* leaf cuticular conductance */
		//todo insert CCOND into species.txt
		gl_c_corr = /*s->value[CCOND]*/ 0.00006 * c->gcorr;
		//Log("gl_c_corr = %f\n", gl_c_corr);


		/* leaf stomatal conductance: first generate multipliers, then apply them
		to maximum stomatal conductance */
		/* calculate stomatal conductance radiation multiplier: */


		/* photosynthetic photon flux density  */
		ppfd_sun = s->value[APAR] * EPAR;
		//Log("ppfd_sun = %f\n", ppfd_sun);

		/* photosynthetic photon flux density conductance control */
		ppfd_sun /= (PPFD50 + ppfd_sun);
		//Log("ppfd_sun = %f\n", ppfd_sun);


		/* apply all multipliers to the maximum stomatal conductance */
		//Currently I will use the 3-PG modifiers
		final_ppfd_sun = ppfd_sun * s->value[F_SW] * s->value[F_T] * s->value[F_VPD];
		//Log("final_ppfd_sun = %f\n", final_ppfd_sun);
		gl_s_sun = /*s->value[MAX_STOM_COND]*/ 0.006 * final_ppfd_sun * c->gcorr;
		//Log("gl_s_sun = %f\n", gl_s_sun);

		/* Leaf conductance to evaporated water vapor, per unit projected LAI */

		gl_e_wv = gl_bl_corr;
		//Log("gl_e_wv = %f\n", gl_e_wv);



		/* Leaf conductance to transpired water vapor, per unit projected
		LAI.  This formula is derived from stomatal and cuticular conductances
		in parallel with each other, and both in series with leaf boundary
		layer conductance. */

		gl_t_wv_sun = (gl_bl_corr * (gl_s_sun))/(gl_bl_corr + gl_s_sun + gl_c_corr);
		//Log("gl_t_wv_sun = %f\n", gl_t_wv_sun);

		/* Leaf conductance to sensible heat, per unit all-sided LAI */

		gl_sh = gl_bl_corr;
		//Log("gl_sh = %f\n", gl_sh);


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
		//Log("LAI = %f\n", lai);
		//Log("gc_e_wv = %f\n", gc_e_wv);

		/* Canopy conductane to sensible heat */

		gc_sh = gl_sh * lai;
		//Log("gc_sh = %f\n", gc_sh);


		rv = 1.0/gc_e_wv;
		//Log("rv = %f\n", rv);

		rh = 1.0/gc_sh;
		//Log("rh = %f\n", rh);
		//todo make a better function using values from light.c

		swabs = c->net_radiation * (1 -exp (-s->value[K]+lai));
		//Log("swabs = %f\n", swabs);


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
		//Log("Daily transpiration from biome = %f\n", evap);

		if (settings->time == 'm')
		{
			/* convert evaporation into kg/m^2/month */
			evap = evap * DaysInMonth;
			//Log("Montlhy transpiration from biome = %f\n", evap);
		}
		else
		{
			/* convert evaporation into kg/m^2/month */
			//Log("Daily transpiration from biome = %f\n", evap);
			//Log("Montlhy transpiration from biome = %f\n", evap*DaysInMonth);
		}


		/* calculate the time required to evaporate all the canopy water */
		evap_dayl = met[month].d[day].rain/e;
		//Log("evap_dayl = %f\n", evap_dayl);


}


















































































