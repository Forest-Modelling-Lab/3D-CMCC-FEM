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
extern settings_t* g_settings;

void dendrometry(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species, const meteo_daily_t *const meteo_daily)
{
	double oldavDBH;
	double oldTreeHeight;
	double oldBasalArea;

	double pot_max_crown_diam;         /* potential maximum crown diameter */
	double pot_max_crown_area;         /* potential maximum crown area */
	double pot_apar;                   /* potential absorbed par */
	double pot_light_abs;              /* potential light absorption */
	double current_ccf;                /* crown competition factor */
	double current_hdf;                /* height-diameter competition factor */
	double current_lcf;                /* current light competition factor */
	double delta_C_stem;               /* stem C increment (in kgC/tree) */
	double delta_dbh;                  /* dbh increment (in cm) */
	double delta_dbh_m;                /* dbh increment (in m) */
	double delta_height;               /* height increment (in m) */
	double dbh_m;                      /* dbh in meter */
	double hd_factor;                  /* HD factor based on minimum between light and crow competition */


	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	/* this function compute at the temporal scale at which is called:
	 * -mass density
	 * -delta dbh
	 * -delta tree height
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

	logger(g_debug_log, "\n**Mass density**\n");

	/* compute annual mass density */
	s->value[MASS_DENSITY] = s->value[RHOMAX] + (s->value[RHOMIN] - s->value[RHOMAX]) * exp(-ln2 * (a->value / s->value[TRHO]));
	logger(g_debug_log, "-Mass Density = %g\n", s->value[MASS_DENSITY]);


	/*************************************************************************************************************************/
	/*************************************************************************************************************************/
	/* - Bossel 1996 Ecological Modelling 90, 187-227
	 - Peng et al., 2002, Ecological Modelling 153, 109-130
	 - Seidl et al., 2012, Ecological Modelling 231, 87-100
	*/

	logger(g_debug_log, "*Physically based height diameter approach*\n");

	/************************************************************************/

	/* compute dbh related HDMAX and HDMIN following Seidl et al., 2012 */

	/* compute HD_MAX */
	s->value[HD_MAX] = s->value[HDMAX_A] * pow(d->value, s->value[HDMAX_B]);
	logger(g_debug_log, "HD_MAX = %g \n", s->value[HD_MAX]);
	/*compute HD_MIN */
	s->value[HD_MIN] = s->value[HDMIN_A] * pow(d->value, s->value[HDMIN_B]);
	logger(g_debug_log, "HD_MIN = %g \n", s->value[HD_MIN]);
	/************************************************************************/


	/* dbh cm --> m */
	dbh_m = a->value / 100.;

	/* compute potential maximum crown area */
	pot_max_crown_diam = d->value * s->value[DBHDCMAX];
	pot_max_crown_area = ( Pi / 4) * pow (pot_max_crown_diam, 2. );
	logger(g_debug_log, "pot_max_crown_area = %g\n", pot_max_crown_area);
	logger(g_debug_log, "current_crown_area = %g\n", s->value[CROWN_AREA_DBHDC]);

	/* current crown competition factor (current_ccf) */
	pot_apar = meteo_daily->incoming_par * (1. - (exp(- s->value[K] * s->value[LAI])));
	logger(g_debug_log, "pot_apar = %g\n", pot_apar);

	/* current crown competition factor (current_ccf) */
	current_ccf = s->value[CROWN_AREA_DBHDC] / pot_max_crown_area;
	logger(g_debug_log, "crown_competition factor = %g\n", current_ccf);

	/* current light competition factor */
	current_lcf = pot_apar / meteo_daily->incoming_par;
	logger(g_debug_log, "light_competition factor = %g\n", current_lcf);

	/* current height diameter factor (current_ccf) */
	current_hdf = h->value / dbh_m;
	logger(g_debug_log, "height/diameter factor = %g\n", current_hdf);


	/* Peng et al., 2002 method */
//	/* compute effective H/D ratio */
//	/* case 1, no competition */
//	if ( current_ccf < s->value[LIGHT_TOL] && current_hdf >= s->value[HD_MIN] )
//	{
//		logger(g_debug_log, "case1\n");
//
//		s->value[HD_EFF] = s->value[HD_MIN];
//	}
//	/* case 2, high competition */
//	else if ( current_ccf >= s->value[LIGHT_TOL] && current_hdf <= s->value[HD_MAX] )
//	{
//		logger(g_debug_log, "case2\n");
//
//		s->value[HD_EFF] = s->value[HD_MAX];
//	}
//	/* case 3, high competition low age */
//	else if ( current_ccf >= s->value[LIGHT_TOL] && current_hdf <= s->value[HD_MAX] && a->value < ( 0.5 * s->value[MAXAGE] ) )
//	{
//		logger(g_debug_log, "case3\n");
//
//		s->value[HD_EFF] = s->value[HD_MIN];
//	}
//	/* case 4 */
//	else if ( current_hdf < s->value[HD_MIN] )
//	{
//		logger(g_debug_log, "case4\n");
//
//		s->value[HD_EFF] = s->value[HD_MAX];
//	}
//	/* case 5 */
//	else if ( current_hdf > s->value[HD_MAX] )
//	{
//		logger(g_debug_log, "case5\n");
//
//		s->value[HD_EFF] = 0.5 * s->value[HD_MIN];
//	}
//	/* case 6 */
//	else if ( a->value > ( 0.75 * s->value[MAXAGE] ) )
//	{
//		logger(g_debug_log, "case6\n");
//
//		s->value[HD_EFF] = 0.;
//	}
//	logger(g_debug_log, "Effective H/D ratio = %g\n", s->value[HD_EFF]);

	/* partially inspired to Seidl et al., 2012 method */
	/* HD factor based on minimum between light and crow competition */
	hd_factor = MIN (current_ccf, current_lcf);
	logger(g_debug_log, "minimum factor = %g\n", hd_factor);

	/* HD effective using Seidl et al., 2012 */
	s->value[HD_EFF] = (s->value[HD_MAX] * (1. - hd_factor)) + (s->value[HD_MIN] * hd_factor);
	logger(g_debug_log, "Effective H/D ratio = %g\n", s->value[HD_EFF]);


	/* compute individual delta carbon stem and tC --> kgDM / tree */
	delta_C_stem = (s->value[C_TO_STEM] / s->counter[N_TREE]) * GC_GDM * 1000.;
	logger(g_debug_log, "delta_carbon stem = %g tC/month tree \n", delta_C_stem);

	/* compute diameter increment (in m) */
	delta_dbh_m = ( 4. * delta_C_stem) / ( Pi * s->value[FORM_FACTOR] * pow( ( dbh_m ), 2. ) * ( ( 2. * current_hdf ) + s->value[HD_EFF] ) );
	/* convert m --> cm */
	delta_dbh = delta_dbh_m * 100.;
	logger(g_debug_log, "delta_dbh = %g cm \n", delta_dbh);

	/* compute tree height increment (in m) */
	delta_height = delta_dbh_m * s->value[HD_EFF];
	logger(g_debug_log, "delta_height = %g cm \n", delta_height);


	/* check */
	CHECK_CONDITION( s->value[HD_EFF], < s->value[HD_MIN]);
	CHECK_CONDITION( s->value[HD_EFF], > s->value[HD_MAX]);


	/*************************************************************************************************************************/
	/*************************************************************************************************************************/

	logger(g_debug_log, "\n**Average DBH**\n");

	/* compute tree AVDBH */
	d->value += delta_dbh;
	logger(g_debug_log, "-Old AVDBH = %g cm\n", oldavDBH);
	logger(g_debug_log, "-New Average DBH = %g cm\n", d->value);

	/* check */
	CHECK_CONDITION( d->value, < oldavDBH - eps );

	/*************************************************************************************************************************/

	logger(g_debug_log, "\n**Average Tree Height**\n");

	/* compute tree Height */
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

	logger(g_debug_log, "\n**Mass density**\n");

	/* compute annual mass density */
	s->value[MASS_DENSITY] = s->value[RHOMAX] + (s->value[RHOMIN] - s->value[RHOMAX]) * exp(-ln2 * (a->value / s->value[TRHO]));
	logger(g_debug_log, "-Mass Density = %g\n", s->value[MASS_DENSITY]);


	logger(g_debug_log, "\n**Average DBH**\n");

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


	logger(g_debug_log, "-Average stem mass = %g kgC/tree\n", s->value[AV_STEM_MASS_KgC]);
	logger(g_debug_log, "-Old AVDBH = %g cm\n", oldavDBH);
	logger(g_debug_log, "-New Average DBH = %g cm\n", d->value);


	/* check */
	CHECK_CONDITION( d->value, < oldavDBH - eps );

	/*************************************************************************************************************************/

	/* compute tree Height */
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
	/* for references see also: R. Pilli et al. / Forest Ecology and Management 237 (2006) 583–593 */
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
