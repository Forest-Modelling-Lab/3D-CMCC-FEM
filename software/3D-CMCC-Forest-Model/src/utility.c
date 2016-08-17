/*utility.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utility.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void reset_daily_cell_variables(cell_t *const c)
{
	logger(g_log, "...resetting cell level daily variables...\n");

	c->cell_cover = 0.;

	/* reset daily radiative variables */
	c->par = 0.;
	c->apar = 0.;
	c->par_transm = 0.;
	c->par_refl = 0.;
	c->par_for_soil = 0.;
	c->par_refl_soil = 0.;

	c->sw_rad_down_W = 0.;
	c->sw_rad_refl = 0.;
	c->sw_rad_for_soil_refl = 0.;
	c->sw_rad_abs = 0.;
	c->sw_rad_transm = 0.;
	c->sw_rad_for_soil = 0.;

	c->ppfd_abs = 0.;
	c->ppfd_transm = 0.;
	c->ppfd_refl = 0.;
	c->ppfd_for_soil = 0.;
	c->ppfd_refl_soil = 0.;

	c->canopy_temp_k = 0.;

	/*reset daily carbon variables*/
	c->daily_gpp = 0.;
	c->daily_npp_tDM = 0.;
	c->daily_npp_gC = 0.;
	c->daily_leaf_maint_resp = 0.;
	c->daily_stem_maint_resp = 0.;
	c->daily_fine_root_maint_resp = 0.;
	c->daily_branch_maint_resp = 0.;
	c->daily_maint_resp = 0.;
	c->daily_coarse_root_maint_resp = 0.;
	c->daily_leaf_growth_resp = 0.;
	c->daily_stem_growth_resp = 0.;
	c->daily_fine_root_growth_resp = 0.;
	c->daily_branch_growth_resp = 0.;
	c->daily_coarse_root_growth_resp = 0.;
	c->daily_growth_resp = 0.;
	c->daily_aut_resp = 0.;
	c->daily_aut_resp_tC = 0.;
	c->daily_leaf_aut_resp = 0.;
	c->daily_stem_aut_resp = 0.;
	c->daily_branch_aut_resp = 0.;
	c->daily_fine_root_aut_resp = 0.;
	c->daily_coarse_root_aut_resp = 0.;
	c->daily_leaf_carbon = 0.;
	c->daily_stem_carbon = 0.;
	c->daily_fine_root_carbon = 0.;
	c->daily_coarse_root_carbon = 0.;
	c->daily_root_carbon = 0.;
	c->daily_branch_carbon = 0.;
	c->daily_reserve_carbon = 0.;
	c->daily_litter_carbon = 0.;
	c->daily_fruit_carbon = 0.;

	/*reset daily water variables*/
	c->snow_melt = 0.;
	c->snow_subl = 0.;
	c->out_flow = 0.;
	c->daily_c_transp = 0.;
	c->daily_c_rain_int = 0.;
	c->daily_c_snow_int = 0.;
	c->daily_c_evapo = 0.;
	c->daily_soil_evapo = 0.;
	c->daily_c_sensible_heat_flux = 0.;
	c-> daily_soil_latent_heat_flux = 0.;
	c->daily_latent_heat_flux = 0.;
	c->daily_sensible_heat_flux = 0.;
}

void reset_monthly_cell_variables(cell_t *const c)
{
	logger(g_log, "...resetting cell level monthly variables...\n");

	/*reset cell level variables*/
	c->monthly_gpp = 0.;
	c->monthly_npp_gC = 0.;
	c->monthly_npp_tDM = 0.;
	c->monthly_aut_resp = 0.;
	c->monthly_aut_resp_tC = 0.;
	c->monthly_maint_resp = 0.;
	c->monthly_growth_resp = 0.;
	c->monthly_r_eco = 0.;
	c->monthly_het_resp = 0.;
	c->monthly_gpp = 0.;
	c->monthly_C_flux = 0.;
	c->monthly_nee = 0.;
	c->monthly_tot_w_flux = 0.;
	c->monthly_c_rain_int = 0.;
	c->monthly_c_transp = 0.;
	c->monthly_c_evapo = 0.;
	c->monthly_c_water_stored = 0.;
	c->monthly_c_evapotransp = 0.;
	c->monthly_soil_evapo = 0.;
	c->monthly_et = 0.;
	c->monthly_latent_heat_flux = 0.;
	c->monthly_sensible_heat_flux = 0.;


}

void reset_annual_cell_variables(cell_t *const c)
{
	logger(g_log, "...resetting cell level annual variables...\n");

	/* reset cell level variables */
	c->doy = 0.;

	c->annual_gpp = 0.;
	c->annual_npp_gC = 0.;
	c->annual_npp_tDM = 0.;
	c->annual_aut_resp = 0.;
	c->annual_aut_resp_tC = 0.;
	c->annual_maint_resp = 0.;
	c->annual_growth_resp = 0.;
	c->annual_r_eco = 0.;
	c->annual_het_resp = 0.;
	c->annual_c_rain_int = 0.;
	c->annual_c_transp = 0.;
	c->annual_c_evapo = 0.;
	c->annual_c_water_stored = 0.;
	c->annual_c_evapotransp = 0.;
	c->annual_soil_evapo = 0.;
	c->annual_et = 0.;
	c->annual_latent_heat_flux = 0.;
	c->annual_sensible_heat_flux = 0.;
	c->basal_area = 0.;
	c->agb = 0.;
	c->bgb = 0.;
	//c->dead_tree = 0;
	c->annual_soil_evapo = 0.;

}
void reset_daily_layer_variables(cell_t *const c)
{
	int layer;

	tree_layer_t *l;

	logger(g_log, "...resetting layer level daily variables...\n");

	for ( layer = c->t_layers_count -1 ; layer >= 0; --layer )
	{
		l = &c->t_layers[layer];

		/* reset layer level variables */
		l->height_class = 0.;
		l->n_trees = 0.;
		l->density = 0.;
		l->layer_cover = 0.;
	}
}
void reset_monthly_layer_variables(cell_t *const c)
{
//	int layer;

	logger(g_log, "...resetting layer level monthly variables...\n");

//	for ( layer = c->t_layers_count -1 ; layer >= 0; --layer )
//	{
//
//	}
}
void reset_annual_layer_variables(cell_t *const c)
{
	int layer;

	logger(g_log, "...resetting layer level annual variables...\n");

	c->t_layers_count = 0.;

	for ( layer = c->t_layers_count -1 ; layer >= 0; --layer )
	{

	}
}

void reset_daily_class_variables(cell_t *const c)
{
	int height;
	int age;
	int species;

	height_t *h;
	age_t *a;
	species_t *s;

	logger(g_log, "\n...resetting class level daily variables...\n");

	/* height class level */
	for ( height = c->heights_count - 1; height >= 0; height--)
	{
		h = &c->heights[height];

		/* age class level */
		for (age = h->ages_count - 1; age >= 0; age --)
		{
			a = &h->ages[age];

			/* species level */
			for (species = a->species_count - 1; species >= 0; species --)
			{
				s = &a->species[species];

				/* reset daily radiative variables */
				s->value[PAR] = 0.;
				s->value[APAR] = 0.;
				s->value[APAR_SUN] = 0.;
				s->value[APAR_SHADE] = 0.;
				s->value[TRANSM_PAR] = 0.;
				s->value[TRANSM_PAR_SUN] = 0.;
				s->value[TRANSM_PAR_SHADE] = 0.;
				s->value[SW_RAD] = 0.;
				s->value[SW_RAD_ABS] = 0.;
				s->value[SW_RAD_ABS_SUN] = 0.;
				s->value[SW_RAD_ABS_SHADE] = 0.;
				s->value[SW_RAD_TRANSM] = 0.;
				s->value[SW_RAD_TRANSM_SUN] = 0.;
				s->value[SW_RAD_TRANSM_SHADE] = 0.;
				s->value[PPFD] = 0.;
				s->value[PPFD_ABS] = 0.;
				s->value[PPFD_ABS_SUN] = 0.;
				s->value[PPFD_ABS_SHADE] = 0.;
				s->value[PPFD_TRANSM] = 0.;
				s->value[PPFD_TRANSM_SUN] = 0.;
				s->value[PPFD_TRANSM_SHADE] = 0.;

				/* reset daily carbon fluxes */
				s->value[DAILY_GPP_gC] = 0.;
				s->value[NPP_gC] = 0.;
				s->value[NPP_tC] = 0.;
				s->value[C_FLUX] = 0.;

				/* reset daily water fluxes */
				s->value[CANOPY_INT] = 0.;
				s->value[CANOPY_EVAPO] = 0.;
				s->value[CANOPY_TRANSP] = 0.;
				s->value[CANOPY_EVAPO_TRANSP] = 0.;

				/* reset daily multipliers */
				s->value[F_CO2] = 0.;
				s->value[F_LIGHT] = 0.;
				s->value[F_T] = 0.;
				s->value[F_FROST] = 0.;
				s->value[F_VPD] = 0.;
				s->value[F_AGE] = 0.;
				s->value[F_NUTR] = 0.;
				s->value[F_SW] = 0.;
				s->value[F_PSI] = 0.;
				s->value[PHYS_MOD] = 0.;

				/* reset daily carbon fluxes among pools */
				s->value[C_TO_LEAF] = 0.;
				s->value[C_TO_ROOT] = 0.;
				s->value[C_TO_FINEROOT] = 0.;
				s->value[C_TO_COARSEROOT] = 0.;
				s->value[C_TO_TOT_STEM] = 0.;
				s->value[C_TO_STEM] = 0.;
				s->value[C_TO_BRANCH] = 0.;
				s->value[C_TO_RESERVE] = 0.;
				s->value[C_TO_FRUIT] = 0.;
				s->value[C_TO_LITTER] = 0.;
				s->value[C_LEAF_TO_RESERVE] = 0.;
				s->value[C_FINEROOT_TO_RESERVE] = 0.;

				/* reset daily maint and growth respiration */
				s->value[DAILY_LEAF_MAINT_RESP] = 0.;
				s->value[NIGHTLY_LEAF_MAINT_RESP] = 0.;
				s->value[TOT_DAY_LEAF_MAINT_RESP] = 0.;
				s->value[FINE_ROOT_MAINT_RESP] = 0.;
				s->value[STEM_MAINT_RESP] = 0.;
				s->value[BRANCH_MAINT_RESP] = 0.;
				s->value[COARSE_ROOT_MAINT_RESP] = 0.;
				s->value[TOTAL_MAINT_RESP] = 0.;
				s->value[LEAF_GROWTH_RESP] = 0.;
				s->value[FINE_ROOT_GROWTH_RESP] = 0.;
				s->value[COARSE_ROOT_GROWTH_RESP] = 0.;
				s->value[STEM_GROWTH_RESP] = 0.;
				s->value[BRANCH_GROWTH_RESP] = 0.;
				s->value[TOTAL_GROWTH_RESP] = 0.;
			}
		}
	}
}

void reset_monthly_class_variables(cell_t *const c)
{
	int height;
	int age;
	int species;

	height_t *h;
	age_t *a;
	species_t *s;

	logger(g_log, "\n...resetting class level monthly variables...\n");

	/* height class level */
	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		h = &c->heights[height];

		/* age class level */
		for (age = h->ages_count - 1; age >= 0; age --)
		{
			a = &h->ages[age];

			/* species level */
			for (species = a->species_count - 1; species >= 0; species -- )
			{
				s = &a->species[species];

				s->value[MONTHLY_EVAPOTRANSPIRATION] = 0.;
			}
		}
	}
}


void reset_annual_class_variables(cell_t *const c)
{
	int height;
	int age;
	int species;

	height_t *h;
	age_t *a;
	species_t *s;

	logger(g_log, "\n...resetting class level annual variables...\n");

	/* height class level */
	for (height = c->heights_count - 1; height >= 0; height--)
	{
		h = &c->heights[height];

		h->z = 0.;

		/* age class level */
		for (age = h->ages_count - 1; age >= 0; age --)
		{
			a = &c->heights[height].ages[age];

			/* species level */
			for (species = a->species_count - 1; species >= 0; species --)
			{
				s = &c->heights[height].ages[age].species[species];

				s->value[PEAK_LAI] = 0.;
				s->value[MAX_LEAF_C] = 0.;

				/*reset cumulative values*/
				s->counter[LEAF_FALL_COUNTER] = 0;

				s->counter[VEG_DAYS] = 0;
				s->value[YEARLY_PHYS_MOD] = 0;

				s->value[YEARLY_GPP_gC] = 0;
				s->value[YEARLY_NPP_gC] = 0;
				s->value[YEARLY_NPP_tC] = 0;

				s->counter[DEAD_STEMS] = 0;
				s->counter[N_TREE_SAP] = 0;

				//INITIALIZE AVERAGE YEARLY MODIFIERS
				s->value[AVERAGE_F_VPD]  = 0.;
				s->value[AVERAGE_F_T]  = 0.;
				s->value[AVERAGE_F_SW]  = 0.;
				s->value[F_AGE]  = 0.;

				s->value[DEL_Y_WS] = 0.;
				s->value[DEL_Y_WF] = 0.;
				s->value[DEL_Y_WFR] = 0.;
				s->value[DEL_Y_WCR] = 0.;
				s->value[DEL_Y_WRES] = 0.;
				s->value[DEL_Y_WR] = 0.;
				s->value[DEL_Y_BB] = 0.;
				s->value[OLD_BIOMASS_ROOTS_COARSE] = s->value[BIOMASS_COARSE_ROOT_tDM];
				s->value[OLD_BIOMASS_FINE_ROOT_tDM] = s->value[BIOMASS_FINE_ROOT_tDM];
				s->value[OLD_BIOMASS_STEM] = s->value[BIOMASS_STEM_tDM];
				s->value[OLD_BIOMASS_BRANCH] = s->value[BIOMASS_BRANCH_tDM];
				s->value[OLD_BIOMASS_LEAVES] = s->value[BIOMASS_FOLIAGE_tDM];
				s->value[OLD_BIOMASS_STEM_LIVE_WOOD] = s->value[BIOMASS_STEM_LIVE_WOOD_tDM];
				s->value[OLD_BIOMASS_COARSE_ROOT_LIVE_WOOD] = s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM];
				s->value[OLD_BIOMASS_STEM_BRANCH_LIVE_WOOD] = s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM];
			}
		}
	}
}

void First_day(cell_t *const c, int layer, int height, int age, int species)
{
	height_t *h;
	age_t *a;
	species_t *s;

	h = &c->heights[height];
	a = &c->heights[height].ages[age];
	s = &c->heights[height].ages[age].species[species];

	//fixme maybe useless function in this way, better implement elsewhere

	logger(g_log, "..first day..\n");

	/* height class level */
	for (height = c->heights_count - 1; height >= 0; height--)
	{
		/* age class level */
		for (age = h->ages_count - 1; age >= 0; age --)
		{
			/* species level */
			for (species = a->species_count - 1; species >= 0; species --)
			{
				//fixme move elsewhere
				/* compute cell level number of trees */
				c->n_tree += s->counter[N_TREE];

				//fixme move elsewhere
//				s->turnover->FINERTOVER = (int)(365 / s->value[LEAF_FINEROOT_TURNOVER]);
//				s->turnover->COARSERTOVER = (int)(365 / s->value[COARSEROOT_TURNOVER]);
//				s->turnover->STEMTOVER = (int)(365 / s->value[LIVE_WOOD_TURNOVER]);
//				s->turnover->BRANCHTOVER = (int)(365 / s->value[BRANCHTTOVER]);

				//fixme move elsewhere
				/* compute value for volume for next years comparisons (CAI-MAI) */
				s->value[MASS_DENSITY] = s->value[RHOMAX] +	(s->value[RHOMIN] - s->value[RHOMAX]) *	exp(-ln2 * (h->value / s->value[TRHO]));
				s->value[PREVIOUS_VOLUME] = s->value[STEM_C] * GC_GDM *	(1 - s->value[FRACBB]) / s->value[MASS_DENSITY];
			}
		}
	}
}



