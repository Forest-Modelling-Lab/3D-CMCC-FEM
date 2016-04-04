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

void Canopy_transpiration_biome (SPECIES *const s, CELL *const c, const MET_DATA *const met, int month, int day, int height, int age, int species)
{
	double g_corr;
	double gl_bl;
	double gl_s, gl_s_sun, gl_s_shade;
	double gl_c;
	double m_ppfd, m_ppfd_sun, m_ppfd_shade;
	double m_final, m_final_sun, m_final_shade;
	double gl_e_wv;
	double gl_t_wv, gl_t_wv_sun, gl_t_wv_shade;
	double gl_sh;
	double gc_e_wv;
	double gc_sh;
	double cwe, trans;

	//within penmon
	double tk;
	double esse;
	double t1, t2;
	double pvs1, pvs2;
	double evap, evap_sun, evap_shade;
	double evap_watt, evap_sun_watt, evap_shade_watt;
	double rr, rh, rhr;
	double rv_sun, rv_shade;
	double dt = 0.2;
	double cell_coverage;
	double transp_ratio;

	Log("\n**BIOME CANOPY_TRANSPIRATION_ROUTINE**\n");

	if(s->value[CANOPY_COVER_DBHDC] > 1.0)
	{
		cell_coverage = 1.0;
	}
	else
	{
		cell_coverage = s->value[CANOPY_COVER_DBHDC];
	}

	/* temperature and pressure correction factor for conductances */
	g_corr = pow((met[month].d[day].tday+273.15)/293.15, 1.75) * 101300/met[month].d[day].air_pressure;
	Log("g_corr BIOME = %f\n", g_corr);

	/* calculate leaf- and canopy-level conductances to water vapor and
	sensible heat fluxes */

	/* leaf boundary-layer conductance */
	gl_bl = s->value[BLCOND] * g_corr;
	Log("BLCOND BIOME = %f\n", gl_bl);

	/* leaf cuticular conductance */
	gl_c = s->value[CUTCOND] * g_corr;
	Log("CUTCOND BIOME = %f\n", gl_c);

	/* leaf stomatal conductance: first generate multipliers, then apply them
	to maximum stomatal conductance */


	/* photosynthetic photon flux density conductance control */
	m_ppfd_sun = s->value[PPFD_SUN] /(PPFD50 + s->value[PPFD_SUN]);
	Log("m_ppfd_sun for biome = %f mol/sec\n", m_ppfd_sun);
	m_ppfd_shade = s->value[PPFD_SHADE] /(PPFD50 + s->value[PPFD_SHADE]);
	Log("m_ppfd_shade for biome = %f mol/sec\n", m_ppfd_shade);


	/* photosynthetic photon flux density conductance control usign APAR (not particular dieffrences has been found*/
	/*
	m_ppfd_sun = (s->value[APAR_SUN] * (met[month].d[day].daylength * 3600.0))/(PPFD50 + (s->value[APAR_SUN]* (met[month].d[day].daylength * 3600.0)));
	Log("m_ppfd_sun for biome = %f mol/sec\n", m_ppfd_sun);
	m_ppfd_shade = (s->value[APAR_SHADE] * (met[month].d[day].daylength * 3600.0))/(PPFD50 + (s->value[APAR_SHADE]* (met[month].d[day].daylength * 3600.0)));
	Log("m_ppfd_shade for biome = %f mol/sec\n", m_ppfd_shade);
	*/


	/* apply all multipliers to the maximum stomatal conductance */
	//m_final = m_ppfd * s->value[F_SW] * s->value[F_CO2] * s->value[F_T] * s->value[F_VPD];
	m_final_sun = m_ppfd_sun * s->value[F_SW] * s->value[F_CO2] * s->value[F_T] * s->value[F_VPD];
	m_final_shade = m_ppfd_shade * s->value[F_SW] * s->value[F_CO2] * s->value[F_T] * s->value[F_VPD];

	//if (m_final < .00000001) m_final_sun = 0.00000001;
	if (m_final_sun < 0.00000001) m_final_sun = 0.00000001;
	if (m_final_shade < 0.00000001) m_final_shade = 0.00000001;

	//gl_s = s->value[MAXCOND] * m_final * g_corr;
	gl_s_sun = s->value[MAXCOND] * m_final_sun * g_corr;
	gl_s_shade = s->value[MAXCOND] * m_final_shade * g_corr;

	/* calculate leaf-and canopy-level conductances to water vapor and
	sensible heat fluxes, to be used in Penman-Monteith calculations of
	canopy evaporation and canopy transpiration. */

	/* Leaf conductance to evaporated water vapor, per unit projected LAI */
	gl_e_wv = gl_bl;

	/* Leaf conductance to transpired water vapor, per unit projected
	LAI.  This formula is derived from stomatal and cuticular conductances
	in parallel with each other, and both in series with leaf boundary
	layer conductance. */
	//gl_t_wv = (gl_bl * (gl_s + gl_c)) / (gl_bl + gl_s + gl_c);
	gl_t_wv_sun = (gl_bl * (gl_s_sun + gl_c)) / (gl_bl + gl_s_sun + gl_c);
	gl_t_wv_shade = (gl_bl * (gl_s_shade + gl_c)) / (gl_bl + gl_s_shade + gl_c);

	/* Leaf conductance to sensible heat, per unit all-sided LAI */
	gl_sh = gl_bl;

	/* Canopy conductance to evaporated water vapor */
	gc_e_wv = gl_e_wv * s->value[LAI];

	Log("Canopy conductance BIOME = %f\n", gc_e_wv);

	/* Canopy conductance to sensible heat */
	/* not clear why not shared between sun and shaded */
	gc_sh = gl_sh * s->value[LAI];

	cwe = trans = 0.0;


	/* FROM HERE PENMON USED */

	/* assign ta (Celsius) and tk (Kelvins) */
	tk = met[month].d[day].tday + 273.15;

	/* calculate resistance to radiative heat transfer through air, rr */
	rr = met[month].d[day].rho_air * CP / (4.0 * SBC * (tk*tk*tk));

	/* resistance to convective heat transfer */
	rh = 1.0/gl_sh;

	/* resistance to latent heat transfer */
	rv_sun = 1.0/gl_t_wv_sun;
	rv_shade = 1.0/gl_t_wv_shade;


	/* calculate combined resistance to convective and radiative heat transfer,
    parallel resistances : rhr = (rh * rr) / (rh + rr) */
	rhr = (rh * rr) / (rh + rr);

	/* calculate temperature offsets for slope estimate */
	t1 = met[month].d[day].tday+dt;
	t2 = met[month].d[day].tday-dt;

	/* calculate saturation vapor pressures at t1 and t2 */
	pvs1 = 610.7 * exp(17.38 * t1 / (239.0 + t1));
	pvs2 = 610.7 * exp(17.38 * t2 / (239.0 + t2));

	/* calculate slope of pvs vs. T curve, at ta */
	esse = (pvs1-pvs2) / (t1-t2);

	/* calculate evaporation, in W/m^2 */
	//    evap =_watt ((esse * s->value[NET_RAD_ABS]) + (met[month].d[day].rho_air * CP * (met[month].d[day].vpd / 100.0) / rhr)) /
	//        	(((c->air_pressure * CP * rv) / (c->lh_vap * EPS * rhr)) + esse);
	//    Log("latent heat of transpiration from BIOME = %f W/m^2\n", evap);
	//
	//    evap = (evap /c->lh_vap) * (met[month].d[day].daylength * 3600.0) * s->value[LAI];
	//    Log("transpiration from BIOME = %f\n", evap);

	//TEST
	/* for sunlit foliage */
	evap_sun_watt = ((esse * s->value[NET_RAD_ABS_SUN]) + (met[month].d[day].rho_air * CP * (met[month].d[day].vpd / 100.0) / rhr)) /
			(((met[month].d[day].air_pressure * CP * rv_sun) / (met[month].d[day].lh_vap * EPS * rhr)) + esse);
	Log("latent heat of transpiration from BIOME = %f W/m^2\n", evap_sun_watt);

	evap_sun = (evap_sun_watt /met[month].d[day].lh_vap) * (met[month].d[day].daylength * 3600.0) * s->value[LAI_SUN];
	Log("transpiration for sunlit from BIOME = %f\n", evap_sun);

	/* for shaded foliage */
	evap_shade_watt = ((esse * s->value[NET_RAD_ABS_SHADE]) + (met[month].d[day].rho_air * CP * (met[month].d[day].vpd / 100.0) / rhr)) /
			(((met[month].d[day].air_pressure * CP * rv_shade) / (met[month].d[day].lh_vap * EPS * rhr)) + esse);
	Log("latent heat of transpiration from BIOME = %f W/m^2\n", evap_shade_watt);

	evap_shade = (evap_shade_watt /met[month].d[day].lh_vap) * (met[month].d[day].daylength * 3600.0) * s->value[LAI_SHADE];
	Log("transpiration for shaded from BIOME = %f mm/m^2/day\n", evap_shade);

	/* upscale to cell coverage level */
	s->value[DAILY_TRANSP_W] = (evap_sun_watt + evap_shade_watt) * cell_coverage;
	Log("Daily latent canopy heat (sun + shade)= %f W/m^2\n", s->value[DAILY_TRANSP_W]);

	transp_ratio = (evap_sun + evap_shade)/s->value[MAXCOND];
	Log("daily transp ratio = %f %%", transp_ratio);

	/* upscale to cell coverage level */
	s->value[DAILY_TRANSP] = (evap_sun + evap_shade) * cell_coverage;
	Log("Daily Canopy Transpiration (sun + shade)= %f mm/m^2/day\n", s->value[DAILY_TRANSP]);

	/* compute energy balance transpiration from canopy */
	c->daily_c_transp_watt = s->value[DAILY_TRANSP_W];
	Log("Daily latent canopy heat = %f W/m^2\n", c->daily_c_transp_watt);

	c->daily_c_transp += s->value[DAILY_TRANSP];
	Log("Daily total canopy transpiration = %f mm/m^2/day\n", c->daily_c_transp);


	//	c->daily_c_transp_watt = c->daily_c_transp * c->lh_vap / 86400.0;
	//	Log("Latent heat canopy transpiration = %f W/m^2\n", c->daily_c_transp * c->lh_vap / 86400.0);



}


void Canopy_transpiration (SPECIES *const s,  CELL *const c, const MET_DATA *const met, int month, int day, int height, int age, int species)
{
	static double defTerm;
	static double duv;                      // 'div' in 3pg
	static double PotEvap;
	double g_corr; //corrector factor from biome
	double maximum_c_conductance;
	double boundary_layer_conductance;

	Log("\n**CANOPY_TRANSPIRATION_ROUTINE**\n");


	/* temperature and pressure correction factor for conductances */
	g_corr = pow((met[month].d[day].tday+273.15)/293.15, 1.75) * 101300/met[month].d[day].air_pressure;

	/*upscale maximum stomatal conductance to maximum canopy conductance*/
	Log("LAI %f\n", s->value[LAI]);
	Log("MAXCOND = %f m/sec\n", s->value[MAXCOND]);
	maximum_c_conductance = s->value[MAXCOND] * s->value[LAI] * g_corr;
	Log("maximum_c_conductance = %f m/sec\n", maximum_c_conductance);
	//unexpectd high values in same cases
	boundary_layer_conductance = s->value[BLCOND] * g_corr;
	Log("BLCOND = %f m/sec\n", s->value[BLCOND]);
	Log("boundary_layer_conductance = %f m/sec\n", boundary_layer_conductance);

	/*Transpiration occurs only if the canopy is dry (see Lawrence et al., 2007)*/
	//Veg period

	/*Canopy Conductance*/
	if (settings->spatial == 's')
	{
		s->value[CANOPY_CONDUCTANCE] = maximum_c_conductance * s->value[PHYS_MOD] * Minimum(1.0, met[month].d[day].ndvi_lai / s->value[LAIGCX]);
	}
	else
	{
		s->value[CANOPY_CONDUCTANCE] = maximum_c_conductance * s->value[PHYS_MOD] * Minimum(1.0, s->value[LAI] / s->value[LAIGCX]);
	}
	Log("Potential Canopy Conductance = %f m^2/sec\n", s->value[CANOPY_CONDUCTANCE]);
	//Log("Potential Canopy Conductance = %f m^2/day\n", s->value[CANOPY_CONDUCTANCE]*met[month].d[day].daylength * 3600.0);

	/*Canopy Transpiration*/
	//todo change all functions with BIOME's or Gerten

	// Penman-Monteith equation for computing canopy transpiration
	// in kg/m2/day, which is converted to mm/day.
	// The following are constants in the PM formula (Landsberg & Gower, 1997)
	Log("rhoair = %f\n", met[month].d[day].rho_air);
	Log("lh_vap = %f\n", met[month].d[day].lh_vap);
	Log("vpd = %f\n", met[month].d[day].vpd);
	Log("BLCOND = %f\n", s->value[BLCOND]);
	Log("air_pressure = %f\n", met[month].d[day].air_pressure);

	defTerm = met[month].d[day].rho_air * met[month].d[day].lh_vap * (met[month].d[day].vpd * VPDCONV) * s->value[BLCOND];
	Log("defTerm = %f\n", defTerm);
	duv = (1.0 + E20 + boundary_layer_conductance / s->value[CANOPY_CONDUCTANCE]);
	//Log("duv = %f\n", duv);
	PotEvap = (E20 * s->value[NET_RAD_ABS]+ defTerm) / duv; // in J/m2/s
	Log("PotEvap = %f\n", PotEvap);

	/*compute transpiration*/
	if(met[month].d[day].tavg > s->value[GROWTHTMIN] && PotEvap > 0.0)
	{
		s->value[DAILY_TRANSP] = ((PotEvap / met[month].d[day].lh_vap * (met[month].d[day].daylength * 3600.0)) * s->value[CANOPY_COVER_DBHDC]) *
				s->value[FRAC_DAYTIME_TRANSP] * s->value[F_CO2];
		Log("Canopy transpiration = %f mm/m2\n", s->value[DAILY_TRANSP]);
	}
	else
	{
		s->value[DAILY_TRANSP] = 0.0;
	}

	/*cumulate values of transpiration*/
	/*dominant layer*/
	if (c->heights[height].z == c->top_layer)
	{
		c->layer_daily_c_transp[c->top_layer] += s->value[DAILY_TRANSP];
		Log("Canopy transpiration from dominant layer = %f mm \n", c->layer_daily_c_transp[c->top_layer]);
		/*last height dominant class processed*/
		if (c->dominant_veg_counter == c->height_class_in_layer_dominant_counter)
		{
			/*control*/
			if (c->asw < c->layer_daily_c_transp[c->top_layer])
			{
				Log("ATTENTION DAILY TRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
				c->layer_daily_c_transp[c->top_layer] = c->asw;
			}
		}
	}
	/*dominated*/
	else
	{
		/*dominated layer*/
		if (c->heights[height].z == c->top_layer-1)
		{
			Log("Canopy transpiration  water from dominated layer = %f mm \n", c->layer_daily_c_transp[c->top_layer-1]);
			/*last height dominated class processed*/
			if (c->dominated_veg_counter == c->height_class_in_layer_dominated_counter)
			{
				/*control*/
				if (c->asw < c->layer_daily_c_transp[c->top_layer-1])
				{
					Log("ATTENTION DAILY TRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
					c->layer_daily_c_transp[c->top_layer-1] = c->asw;
				}
			}
		}
		/*subdominated layer*/
		else
		{
			c->layer_daily_c_transp[c->top_layer-2] += s->value[DAILY_TRANSP];
			Log("Canopy transpiration  water from dominated layer = %f mm \n", c->layer_daily_c_transp[c->top_layer-2]);
			/*last height subdominated class processed*/
			if (c->subdominated_veg_counter == c->height_class_in_layer_subdominated_counter)
			{
				/*control*/
				if (c->asw < c->layer_daily_c_transp[c->top_layer-2])
				{
					Log("ATTENTION DAILY TRANSPIRATION EXCEEDS AVAILABLE SOIL WATER!!!\n");
					c->layer_daily_c_transp[c->top_layer-2] = c->asw;
				}
			}
		}
	}
	/*compute total daily transpiration*/
	c->daily_c_transp += c->layer_daily_c_transp[c->heights[height].z];
	Log("Daily total canopy transpiration = %f \n", c->daily_c_transp);

	/*compute energy balance transpiration from canopy*/
	c->daily_c_transp_watt = c->daily_c_transp * met[month].d[day].lh_vap / 86400.0;
	Log("Latent heat canopy transpiration = %f W/m^2\n", c->daily_c_transp_watt);

}




















































































