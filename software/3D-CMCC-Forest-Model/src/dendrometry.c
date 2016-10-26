/* dendrometry.c */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dendometry.h"
#include "common.h"
#include "constants.h"
#include "logger.h"
#include "settings.h"

extern logger_t* g_debug_log;
//extern int MonthLength [];
//extern int MonthLength_Leap [];

void dendrometry ( cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily, const int month, const int year )
{
	double oldavDBH;
	double oldTreeHeight;
	double oldBasalArea;
	double mass_density_kg;

	double pot_par;                    /* potential absorbable incoming par */
	double pot_apar;                   /* potential absorbed par */
	double pot_apar_sun;               /* potential absorbed par sun */
	double pot_apar_shade;             /* potential absorbed par shade */
	double leaf_cell_cover_eff;
	double Light_refl_par_frac;
	double current_ccf;                /* crown competition factor */
	double current_hdf;                /* height-diameter competition factor */
	double current_lcf;                /* current light competition factor */
	double delta_stem;                 /* stem increment (kgDM/tree) */
	double delta_dbh;                  /* dbh increment (cm) */
	double delta_dbh_m;                /* dbh increment (m) */
	double delta_height;               /* height increment (m) */
	double dbh_m;                      /* dbh (m) */
	double hd_factor;                  /* HD factor based on minimum between light and crown competition */

	double slope;
	double phi;


	static double annual_lcf;
	static int counter_annual_lcf;
	static double annual_ccf;
	static int counter_annual_ccf;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	/* this function compute at the temporal scale at which is called
	 * globally:
	 * -mass density
	 * -delta dbh
	 * -delta tree height
	 * -basal area
	 * -sapwood and heartwood
	 * -recompute fractions of live and dead wood
	 * locally:
	 * -mass density in kg
	 * -volume
	 * */

	h = &c->heights[height];
	d = &h->dbhs[dbh];
	a = &c->heights[height].dbhs[dbh].ages[age];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* assign previous month values */
	oldavDBH = d->value;
	oldTreeHeight = h->value;
	oldBasalArea = s->value[BASAL_AREA];

	logger(g_debug_log, "\n**DENDROMETRY**\n");

	/*************************************************************************************************************************/

	logger(g_debug_log, "\n**Mass density**\n");

	/* compute mass density tDM/m3 */
	s->value[MASS_DENSITY] = s->value[RHOMAX] + (s->value[RHOMIN] - s->value[RHOMAX]) * exp(-ln2 * (a->value / s->value[TRHO]));
	logger(g_debug_log, "-Mass Density = %g tDM/m3\n", s->value[MASS_DENSITY]);

	/* convert to tDM-->kgDM m3 */
	mass_density_kg = s->value[MASS_DENSITY] * 1000.;

	/*************************************************************************************************************************/
	/*
	 - Bossel 1996 Ecological Modelling 90, 187-227
	 - Peng et al., 2002, Ecological Modelling 153, 109-130
	 - Seidl et al., 2012, Ecological Modelling 231, 87-100
	 */

	logger(g_debug_log, "*Physically based height diameter approach*\n");

	/* compute phi (kg) */
	phi = (mass_density_kg * s->value[FORM_FACTOR] * Pi ) / 4.;

	/* convert dbh cm --> m as in Bossel et al., 1996; Peng et al., 2002; Seidl et al., 2012 */
	dbh_m = a->value / 100.;

	/*******************************************************************************************/

	//	logger(g_debug_log, "Ind old stem mass = %g kgDM/tree\n", (s->value[STEM_C] * GC_GDM * 1000.)/s->counter[N_TREE]);
	//	logger(g_debug_log, "Ind old stem volume = %g m3/tree\n", ((s->value[STEM_C] * GC_GDM)/s->counter[N_TREE])/s->value[MASS_DENSITY]);
	//
	//	/* old stem mass from volume (cylinder) */
	//	old_stem_mass_from_volume = pow(dbh_m,2.) * h->value * phi;
	//	logger(g_debug_log, "Ind old stem mass from volume = %g kgDM/tree\n", old_stem_mass_from_volume);
	//
	//	/* old stem volume (cylinder) */
	//	old_stem_volume = s->value[FORM_FACTOR] * pow(d->value,2.) * h->value * 0.0001;
	//	logger(g_debug_log, "Ind stem volume = %g m3/tree\n", old_stem_volume);

	/*******************************************************************************************/

	/* compute dbh related HDMAX and HDMIN following Seidl et al., 2012 */
	/* compute HD_MAX */
	s->value[HD_MAX] = s->value[HDMAX_A] * pow(d->value, s->value[HDMAX_B]);
	logger(g_debug_log, "HD_MAX = %g \n", s->value[HD_MAX]);
	/*compute HD_MIN */
	s->value[HD_MIN] = s->value[HDMIN_A] * pow(d->value, s->value[HDMIN_B]);
	logger(g_debug_log, "HD_MIN = %g \n", s->value[HD_MIN]);

	/*******************************************************************************************/

	/* current height diameter factor (current_ccf) following Seidl et al., 2012 (eq.1b) */
	current_hdf = s->value[STEMCONST_P]*pow(dbh_m, s->value[STEMPOWER_P]);
	logger(g_debug_log, "height/diameter factor (Seidl method) = %g\n", current_hdf);

	/* current height diameter factor (current_ccf) */
	//fixme????
	//current_hdf = h->value / dbh_m;
	//logger(g_debug_log, "height/diameter factor = %g\n", current_hdf);

	/*******************************************************************************************/

	/* compute current light competition factor */

	/* compute exposed canopy cover */
	/* special case when LAI = < 1.0 */
	/* note: 26 Ottobre 2016 */
	if(s->value[LAI] < 1.0) leaf_cell_cover_eff = s->value[LAI] * s->value[CANOPY_SURFACE_COVER];
	else leaf_cell_cover_eff = s->value[CANOPY_SURFACE_COVER];

	/* check for the special case in which is allowed to have more 100% of grid cell covered */
	if(leaf_cell_cover_eff > 1.0) leaf_cell_cover_eff = 1.0;
	logger(g_debug_log, "single height class canopy cover = %g %%\n", leaf_cell_cover_eff*100.0);


	if( s->value[LAI] >= 1.0 )
	{
		Light_refl_par_frac = s->value[ALBEDO]/3.0;
	}
	else if ( ! s->value[LAI] )
	{
		Light_refl_par_frac = 0.0;
	}
	else
	{
		Light_refl_par_frac = (s->value[ALBEDO]/3.0) * s->value[LAI];
	}

	if ( s->value[LAI] > 0.)
	{
		/* compute potential absorbable incoming par less reflected */
		pot_par = meteo_daily->incoming_par - (meteo_daily->incoming_par * Light_refl_par_frac  * leaf_cell_cover_eff);

		/* compute potential absorbed incoming par */
		pot_apar_sun = pot_par * (1. - (exp(- s->value[K] * (s->value[LAI_SUN]/leaf_cell_cover_eff)))) * leaf_cell_cover_eff;
		pot_apar_shade = (pot_par - pot_apar_sun) * (1. - (exp(- s->value[K] * (s->value[LAI_SHADE]/leaf_cell_cover_eff)))) * leaf_cell_cover_eff;
		pot_apar = pot_apar_sun + pot_apar_shade;

		/* current light competition factor */
		current_lcf = s->value[APAR] / pot_apar;
		logger(g_debug_log, "light_competition factor = %g\n", current_lcf);

		/*check */
		CHECK_CONDITION(current_lcf, > 1);
		CHECK_CONDITION(current_lcf, < 0);

		/* cumulate annual light competition factor */
		annual_lcf += current_lcf;
		++counter_annual_lcf;

		/*******************************************************************************************/

		/* current crown competition factor (current_ccf) */
		slope = s->value[DBHDCMAX]- s->value[DBHDCMIN];
		current_ccf = (s->value[DBHDC_EFF] - s->value[DBHDCMIN])/slope;
		logger(g_debug_log, "crown_competition factor = %g\n", current_ccf);

		/*check */
		CHECK_CONDITION(current_ccf, > 1);
		CHECK_CONDITION(current_ccf, < 0);

		/* cumulate annual crown competition factor */
		annual_ccf += current_ccf;
		++counter_annual_ccf;

	}

	/* annual computation */
	if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365) )
	{
		logger(g_debug_log, "\n**ANNUAL DENDROMETRY**\n");

		/* compute average annual light competition factor */
		annual_lcf /= (double)counter_annual_lcf;

		/* check */
		CHECK_CONDITION(annual_lcf, > 1);
		CHECK_CONDITION(annual_lcf, < 0);

		/* compute average annual crown competition factor */
		annual_ccf /= (double)counter_annual_ccf;

		/* check */
		CHECK_CONDITION(annual_ccf, > 1);
		CHECK_CONDITION(annual_ccf, < 0);

		/*******************************************************************************************/

		/* partially inspired to Seidl et al., 2012 method */
		/* HD factor based on minimum value between light and crown competition */
		hd_factor = MIN (annual_ccf, annual_lcf);
		logger(g_debug_log, "minimum factor = %g\n", hd_factor);

		/* reset values */
		annual_lcf = 0.;
		counter_annual_lcf = 0.;
		annual_ccf = 0.;
		counter_annual_ccf = 0.;

		/*******************************************************************************************/

		/* compute HD effective using a partially inspired to Seidl et al., 2012 */
		s->value[HD_EFF] = (s->value[HD_MAX] * (1. - hd_factor)) + (s->value[HD_MIN] * hd_factor);
		logger(g_debug_log, "Effective H/D ratio = %g\n", s->value[HD_EFF]);

		/* check */
		CHECK_CONDITION( s->value[HD_EFF], < s->value[HD_MIN]);
		CHECK_CONDITION( s->value[HD_EFF], > s->value[HD_MAX]);

		/*******************************************************************************************/

		//	/* convert individual delta C stem and tDM/sizeCell --> kgDM/ tree */
		//	delta_stem = (s->value[C_TO_STEM] * GC_GDM * 1000. ) / s->counter[N_TREE];
		//	logger(g_debug_log, "delta_carbon stem = %g KgDM/month/tree \n", delta_stem);
		//
		//	/* compute individual dbh and height increment */
		//
		//	/* compute diameter increment (in m) Peng et al., 2002; Seidl et al., 2012 */
		//	delta_dbh_m = delta_stem / (phi * pow(dbh_m,2.) * ( ( 2. * current_hdf ) + s->value[HD_EFF]) );
		//	logger(g_debug_log, "delta_dbh = %g m \n", delta_dbh_m);
		//
		//	/* convert dbh from m --> cm */
		//	delta_dbh = delta_dbh_m * 100.;
		//	logger(g_debug_log, "delta_dbh = %g cm \n", delta_dbh);
		//
		//	/* compute tree height increment (in m) Peng et al., 2002; Seidl et al., 2012 */
		//	delta_height = delta_dbh_m * s->value[HD_EFF];
		//	logger(g_debug_log, "delta_height = %g m \n", delta_height);

		/* following Peng et al., 2002*/
		s->value[HD_EFF] /= 100.;
		delta_stem = s->value[DEL_Y_WS] * GC_GDM;
		logger(g_debug_log, "-Annual stem increment = %g tC/year\n", delta_stem);

		delta_dbh = (4.*delta_stem)/(Pi*s->value[FORM_FACTOR]*s->value[MASS_DENSITY]*pow(d->value,2.)*((2*(h->value/d->value)+s->value[HD_EFF])));

		delta_height = delta_dbh * s->value[HD_EFF];

		/*******************************************************************************************/

		logger(g_debug_log, "\n**Average DBH**\n");

		/* compute tree dbh */
		d->value += delta_dbh;
		logger(g_debug_log, "-Old AVDBH = %g cm\n", oldavDBH);
		logger(g_debug_log, "-New Average DBH = %g cm\n", d->value);

		/* convert dbh cm --> m as in Bossel 1996; Peng et al., 2002; Seidl et al., 2012 */
		dbh_m = a->value / 100.;

		/* check */
		CHECK_CONDITION( d->value, < oldavDBH - eps );

		/*******************************************************************************************/

		logger(g_debug_log, "\n**Average Tree Height**\n");

		/* compute tree height */
		h->value += delta_height;
		logger(g_debug_log, "-Old Tree Height = %g m\n", oldTreeHeight);
		logger(g_debug_log, "-New Tree Height = %g m\n", h->value);

		/* check */
		CHECK_CONDITION( h->value, < oldTreeHeight - eps );
		//fixme once change CRA with HMAX
		//CHECK_CONDITION( h->value, > s->value[CRA] - eps );

		/*************************************************************************************************************************/

		/* compute Basal Area and sapwood-heartwood area */

		logger(g_debug_log, "\n**Basal Area and sapwood-heartwood area**\n");

		s->value[BASAL_AREA] = ((pow((d->value / 2.0), 2.0)) * Pi);
		logger(g_debug_log, " BASAL AREA = %g cm^2\n", s->value[BASAL_AREA]);
		s->value[BASAL_AREA_m2]= s->value[BASAL_AREA] * 0.0001;
		logger(g_debug_log, " BASAL BASAL_AREA_m2 = %g m^2\n", s->value[BASAL_AREA_m2]);
		s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (d->value, s->value[SAP_B]);
		logger(g_debug_log, " SAPWOOD_AREA = %g cm^2\n", s->value[SAPWOOD_AREA]);
		s->value[HEARTWOOD_AREA] = s->value[BASAL_AREA] - s->value[SAPWOOD_AREA];
		logger(g_debug_log, " HEART_WOOD_AREA = %g cm^2\n", s->value[HEARTWOOD_AREA]);
		s->value[SAPWOOD_PERC] = (s->value[SAPWOOD_AREA]) / s->value[BASAL_AREA];
		logger(g_debug_log, " sapwood perc = %g%%\n", s->value[SAPWOOD_PERC]*100);
		s->value[STEM_SAPWOOD_C] = (s->value[STEM_C] * s->value[SAPWOOD_PERC]);
		logger(g_debug_log, " Sapwood stem biomass = %g tC class cell \n", s->value[STEM_SAPWOOD_C]);
		s->value[COARSE_ROOT_SAPWOOD_C] =  (s->value[COARSE_ROOT_C] * s->value[SAPWOOD_PERC]);
		logger(g_debug_log, " Sapwood coarse root biomass = %g tC class cell \n", s->value[COARSE_ROOT_SAPWOOD_C]);
		s->value[BRANCH_SAPWOOD_C] = (s->value[BRANCH_C] * s->value[SAPWOOD_PERC]);
		logger(g_debug_log, " Sapwood branch and bark biomass = %g tC class cell \n", s->value[BRANCH_SAPWOOD_C]);
		s->value[TOT_SAPWOOD_C] = s->value[STEM_SAPWOOD_C] + s->value[COARSE_ROOT_SAPWOOD_C] + s->value[BRANCH_SAPWOOD_C];
		logger(g_debug_log, " Total Sapwood biomass = %g tc class cell \n", s->value[TOT_SAPWOOD_C]);
		s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
		logger(g_debug_log, " Stand level class basal area = %g cm^2/class cell\n", s->value[STAND_BASAL_AREA]);
		s->value[STAND_BASAL_AREA_m2] = s->value[BASAL_AREA_m2] * s->counter[N_TREE];
		logger(g_debug_log, " Stand level class basal area (meters) = %g m^2/class cell\n", s->value[STAND_BASAL_AREA_m2]);

		logger(g_debug_log, "-Old Basal Area = %g cm^2\n", oldBasalArea);
		logger(g_debug_log, "-New Basal Area = %g cm^2\n", s->value[BASAL_AREA]);

		/* check */
		CHECK_CONDITION( s->value[BASAL_AREA], < oldBasalArea - eps );
	}
}
void annual_minimum_reserve (species_t *const s)
{
	/* recompute annual minimum reserve pool for  year allocation */
	/* these values are taken from: Schwalm and Ek, 2004 Ecological Modelling */
	/* following Krinner et al., 2005 */

	/* IMPORTANT! reserve computation if not in init data are computed from DM */
	logger(g_debug_log, "\n*annual minimum reserve*\n");


	/* compute total sapwood biomass */
	/* note: since SAP_WRES is parameterized for DryMatter it must be converted into DryMatter */
	s->value[WTOT_sap_tDM] = s->value[TOT_SAPWOOD_C] * GC_GDM;
	logger(g_debug_log, "--WTOT_sap_tDM = %f tDM/class \n", s->value[WTOT_sap_tDM]);

	/* compute minimum annual reserve */
	s->value[MIN_RESERVE_C]= s->value[WTOT_sap_tDM] * s->value[SAP_WRES];
	logger(g_debug_log, "--MINIMUM Reserve Biomass = %g t res/class \n", s->value[RESERVE_C]);

	s->value[AV_MIN_RESERVE_KgC] = s->value[MIN_RESERVE_C] *1000.0 /s->counter[N_TREE];
	logger(g_debug_log, "--Average MINIMUM Reserve Biomass = %g Kgres/class tree \n", s->value[RESERVE_C]);
}

void dendrometry_old(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	double oldavDBH;
	double oldTreeHeight;
	double oldBasalArea;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	/* this function compute at the temporal scale at which is called:
	 * -mass density
	 * -average dbh
	 * -tree height
	 * -basal area
	 * -recompute fractions of live and dead wood
	 * */

	h = &c->heights[height];
	d = &h->dbhs[dbh];
	a = &c->heights[height].dbhs[dbh].ages[age];
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* assign previous month values */
	oldavDBH = d->value;
	oldTreeHeight = h->value;
	oldBasalArea = s->value[BASAL_AREA];

	logger(g_debug_log, "\n**DENDROMETRY**\n");

	/* compute tree AVDBH */
	if (s->value[STEMCONST_P] == NO_DATA && s->value[STEMPOWER_P] == NO_DATA)
	{
		/* use generic stemconst stempower values */
		if (oldavDBH < 9)
		{
			d->value = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM / s->value[STEMCONST]), (1.0 / STEMPOWER_A));
		}
		else if (oldavDBH > 9 && oldavDBH < 15)
		{
			d->value = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM / s->value[STEMCONST]), (1.0 / STEMPOWER_B));
		}
		else
		{
			d->value = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM / s->value[STEMCONST]), (1.0 / STEMPOWER_C));
		}
	}
	else
	{
		/* use site specific stemconst stempower values */
		logger(g_debug_log, "Using site related stemconst stempower\n");
		d->value = pow((s->value[AV_STEM_MASS_KgC] * GC_GDM) / s->value[STEMCONST_P], (1.0 / s->value[STEMPOWER_P]));
	}
	logger(g_debug_log, "-Old AVDBH = %g cm\n", oldavDBH);
	logger(g_debug_log, "-New Average DBH = %g cm\n", d->value);


	/* check */
	CHECK_CONDITION( d->value, < oldavDBH - eps );

	/*************************************************************************************************************************/

	/* compute Tree Height */
	/* using Chapman_Richards Function */
	/*
	 * CRA, CRB, CRC are species-specific regression coefficients
	 *-CRA coefficient is the asymptotic maximum height this coefficient represents the theoretic maximum height obtainable
	 * for a given stand or plot, this parameter could be used to represent maximum tree height.
	 * By modifying equation so that CRA changes as a function of stand age, site index, dominant height,
	 * or stand density, equation can be generalized to be more applicable to a wider range of	sites and stand ages,
	 * or more regionally applied
	 *-CRB represents exponential decay parameter
	 *-CRC represents shape parameters
	 * for references see also: M. Sharma, J. Parton, Forest Ecology and Management 249 (2007) 187–198
	 */

	logger(g_debug_log, "\n**Average Tree Height**\n");

	/* Chapman-Richards functions */
	//note: this shouldn't be applied to saplings that are lower than 1.3 meter
	h->value = DBH_ref + s->value[CRA] * pow (1.0 - exp ( - s->value[CRB] * d->value) , s->value[CRC]);

	if ( h->value > s->value[CRA] )
	{
		h->value = s->value[CRA];
	}
	logger(g_debug_log, "-Tree Height using Chapman-Richard function = %g m\n", h->value);

	/* check */
	CHECK_CONDITION (h->value, > s->value[CRA] + eps )

	logger(g_debug_log, "-Old Tree Height = %g m\n", oldTreeHeight);
	logger(g_debug_log, "-New Tree Height = %g m\n", h->value);

	/* Weibull function */
	/* for references see also: R. Pilli et al. Forest Ecology and Management 237 (2006) 583–593 */
	/*
	 h->value = DBH_ref + s->value[WA] *(1.0 - exp ( - s->value[WB] * d->value * s->value[WC]);
	 */

	logger(g_debug_log, "-Old Tree Height = %g m\n", oldTreeHeight);
	logger(g_debug_log, "-New Tree Height = %g m\n", h->value);

	/* check */
	CHECK_CONDITION( h->value, < oldTreeHeight - eps );

	/*************************************************************************************************************************/

	/* compute Basal Area and sapwood-heartwood area */

	logger(g_debug_log, "\n**Basal Area and sapwood-heartwood area**\n");

	s->value[BASAL_AREA] = ((pow((d->value / 2.0), 2.0)) * Pi);
	logger(g_debug_log, " BASAL AREA = %g cm^2\n", s->value[BASAL_AREA]);
	s->value[BASAL_AREA_m2]= s->value[BASAL_AREA] * 0.0001;
	logger(g_debug_log, " BASAL BASAL_AREA_m2 = %g m^2\n", s->value[BASAL_AREA_m2]);
	s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (d->value, s->value[SAP_B]);
	logger(g_debug_log, " SAPWOOD_AREA = %g cm^2\n", s->value[SAPWOOD_AREA]);
	s->value[HEARTWOOD_AREA] = s->value[BASAL_AREA] - s->value[SAPWOOD_AREA];
	logger(g_debug_log, " HEART_WOOD_AREA = %g cm^2\n", s->value[HEARTWOOD_AREA]);
	s->value[SAPWOOD_PERC] = (s->value[SAPWOOD_AREA]) / s->value[BASAL_AREA];
	logger(g_debug_log, " sapwood perc = %g%%\n", s->value[SAPWOOD_PERC]*100);
	s->value[STEM_SAPWOOD_C] = (s->value[STEM_C] * s->value[SAPWOOD_PERC]);
	logger(g_debug_log, " Sapwood stem biomass = %g tC class cell \n", s->value[STEM_SAPWOOD_C]);
	s->value[COARSE_ROOT_SAPWOOD_C] =  (s->value[COARSE_ROOT_C] * s->value[SAPWOOD_PERC]);
	logger(g_debug_log, " Sapwood coarse root biomass = %g tC class cell \n", s->value[COARSE_ROOT_SAPWOOD_C]);
	s->value[BRANCH_SAPWOOD_C] = (s->value[BRANCH_C] * s->value[SAPWOOD_PERC]);
	logger(g_debug_log, " Sapwood branch and bark biomass = %g tC class cell \n", s->value[BRANCH_SAPWOOD_C]);
	s->value[TOT_SAPWOOD_C] = s->value[STEM_SAPWOOD_C] + s->value[COARSE_ROOT_SAPWOOD_C] + s->value[BRANCH_SAPWOOD_C];
	logger(g_debug_log, " Total Sapwood biomass = %g tc class cell \n", s->value[TOT_SAPWOOD_C]);
	s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
	logger(g_debug_log, " Stand level class basal area = %g cm^2/class cell\n", s->value[STAND_BASAL_AREA]);
	s->value[STAND_BASAL_AREA_m2] = s->value[BASAL_AREA_m2] * s->counter[N_TREE];
	logger(g_debug_log, " Stand level class basal area (meters) = %g m^2/class cell\n", s->value[STAND_BASAL_AREA_m2]);

	logger(g_debug_log, "-Old Basal Area = %g cm^2\n", oldBasalArea);
	logger(g_debug_log, "-New Basal Area = %g cm^2\n", s->value[BASAL_AREA]);

	/* check */
	CHECK_CONDITION( s->value[BASAL_AREA], < oldBasalArea - eps );

}
