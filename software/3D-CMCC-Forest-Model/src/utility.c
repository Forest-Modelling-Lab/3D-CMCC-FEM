/*utility.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utility.h"
#include "constants.h"
#include "logger.h"

extern logger_t* g_debug_log;

void reset_daily_cell_variables(cell_t *const c)
{
	logger(g_debug_log, "...resetting cell level daily variables...\n");

	c->cell_cover =                     0.;
	c->cell_n_trees =                   0;
	c->cell_heights_count =             0;

	/* reset daily radiative variables */
	c->apar =                           0.;
	c->apar_soil =                      0.;
	c->apar_snow =                      0.;
	c->par_transm =                     0.;
	c->par_refl =                       0.;
	c->par_refl_soil =                  0.;
	c->par_refl_snow =                  0.;
	c->sw_rad_refl =                    0.;
	c->sw_rad_for_soil_refl =           0.;
	c->sw_rad_for_snow_refl =           0.;
	c->sw_rad_abs =                     0.;
	c->sw_rad_abs_soil =                0.;
	c->sw_rad_abs_snow =                0.;
	c->sw_rad_transm =                  0.;
	c->ppfd_abs =                       0.;
	c->ppfd_abs_soil =                  0.;
	c->ppfd_abs_snow =                  0.;
	c->ppfd_transm =                    0.;
	c->ppfd_refl =                      0.;
	c->ppfd_refl_soil =                 0.;
	c->ppfd_refl_snow =                 0.;
	c->canopy_temp_k =                  0.;

	/*reset daily carbon variables*/
	c->daily_gpp =                      0.;
	c->daily_npp =                      0.;
	c->daily_npp_tC =                   0.;
	c->daily_npp_tDM =                  0.;
	c->daily_leaf_maint_resp =          0.;
	c->daily_stem_maint_resp =          0.;
	c->daily_froot_maint_resp =         0.;
	c->daily_branch_maint_resp =        0.;
	c->daily_maint_resp =               0.;
	c->daily_croot_maint_resp=          0.;
	c->daily_leaf_growth_resp =         0.;
	c->daily_stem_growth_resp =         0.;
	c->daily_froot_growth_resp =        0.;
	c->daily_branch_growth_resp =       0.;
	c->daily_croot_growth_resp =        0.;
	c->daily_growth_resp =              0.;
	c->daily_aut_resp =                 0.;
	c->daily_aut_resp_tC =              0.;
	c->daily_leaf_aut_resp =            0.;
	c->daily_stem_aut_resp =            0.;
	c->daily_branch_aut_resp =          0.;
	c->daily_froot_aut_resp =           0.;
	c->daily_croot_aut_resp =           0.;
	c->daily_leaf_carbon =              0.;
	c->daily_stem_carbon =              0.;
	c->daily_froot_carbon =             0.;
	c->daily_croot_carbon =             0.;
	c->daily_root_carbon =              0.;
	c->daily_branch_carbon =            0.;
	c->daily_reserve_carbon =           0.;
	c->daily_litter_carbon =            0.;
	c->daily_soil_carbon =              0.;
	c->daily_fruit_carbon =             0.;
	c->daily_litrC =                    0.;
	c->daily_leaf_litrC =               0.;
	c->daily_leaf_litr1C =              0.;
	c->daily_leaf_litr2C =              0.;
	c->daily_leaf_litr3C =              0.;
	c->daily_leaf_litr4C =              0.;
	c->daily_froot_litrC =              0.;
	c->daily_froot_litr1C =             0.;
	c->daily_froot_litr2C =             0.;
	c->daily_froot_litr3C =             0.;
	c->daily_froot_litr4C =             0.;


	/*reset daily water variables*/
	c->snow_melt =                      0.;
	c->snow_subl =                      0.;
	c->out_flow =                       0.;
	c->daily_c_transp =                 0.;
	c->daily_c_rain_int =               0.;
	c->daily_c_snow_int =               0.;
	c->daily_c_evapo =                  0.;
	c->daily_soil_evapo =               0.;
	c->daily_c_evapotransp =            0.;
	c->daily_et =                       0.;
	c->daily_c_sensible_heat_flux =     0.;
	c->daily_soil_latent_heat_flux =    0.;
	c->daily_latent_heat_flux =         0.;
	c->daily_sensible_heat_flux =       0.;
}

void reset_monthly_cell_variables(cell_t *const c)
{
	logger(g_debug_log, "...resetting cell level monthly variables...\n");

	/*reset cell level variables*/
	c->monthly_gpp =                    0.;
	c->monthly_npp =                    0.;
	c->monthly_npp_tC =                 0.;
	c->monthly_npp_tDM =                0.;
	c->monthly_aut_resp =               0.;
	c->monthly_aut_resp_tC =            0.;
	c->monthly_maint_resp =             0.;
	c->monthly_growth_resp =            0.;
	c->monthly_r_eco =                  0.;
	c->monthly_het_resp =               0.;
	c->monthly_gpp =                    0.;
	c->monthly_C_flux =                 0.;
	c->monthly_nee =                    0.;
	c->monthly_tot_w_flux =             0.;
	c->monthly_c_rain_int =             0.;
	c->monthly_c_transp =               0.;
	c->monthly_c_evapo =                0.;
	c->monthly_c_water_stored =         0.;
	c->monthly_c_evapotransp =          0.;
	c->monthly_soil_evapo =             0.;
	c->monthly_et =                     0.;
	c->monthly_latent_heat_flux =       0.;
	c->monthly_sensible_heat_flux =     0.;
}

void reset_annual_cell_variables(cell_t *const c)
{
	int height;

	logger(g_debug_log, "...resetting cell level annual variables...\n");

	for ( height = c->heights_count -1 ; height >= 0; --height )
	{
		c->heights[height].height_z = 0;
	}

	/* reset cell level variables */
	c->tree_layers_count =              0;
	c->cell_cover =                     0;
	c->doy =                            0;
	c->annual_gpp =                     0.;
	c->annual_npp =                     0.;
	c->annual_npp_tC =                  0.;
	c->annual_npp_tDM =                 0.;
	c->annual_aut_resp =                0.;
	c->annual_aut_resp_tC =             0.;
	c->annual_maint_resp =              0.;
	c->annual_growth_resp =             0.;
	c->annual_r_eco =                   0.;
	c->annual_het_resp =                0.;
	c->annual_c_rain_int =              0.;
	c->annual_c_transp =                0.;
	c->annual_c_evapo =                 0.;
	c->annual_c_water_stored =          0.;
	c->annual_c_evapotransp =           0.;
	c->annual_soil_evapo =              0.;
	c->annual_et =                      0.;
	c->annual_latent_heat_flux =        0.;
	c->annual_sensible_heat_flux =      0.;
	c->basal_area =                     0.;
	c->agb =                            0.;
	c->bgb =                            0.;
	//c->dead_tree =                    0;
	c->annual_soil_evapo =              0.;
}
void reset_daily_layer_variables(cell_t *const c)
{
	int layer;

	tree_layer_t *l;

	logger(g_debug_log, "...resetting layer level daily variables...\n");

	for ( layer = c->tree_layers_count -1 ; layer >= 0; --layer )
	{
		l = &c->tree_layers[layer];

		l->layer_avg_tree_height = 0.;
		l->layer_tree_height_modifier = 0.;
	}
}
void reset_monthly_layer_variables(cell_t *const c)
{
	//	int layer;

	logger(g_debug_log, "...resetting layer level monthly variables...\n");

	//	for ( layer = c->t_layers_count -1 ; layer >= 0; --layer )
	//	{
	//
	//	}
}
void reset_annual_layer_variables(cell_t *const c)
{
	logger(g_debug_log, "...resetting layer level annual variables...\n");

	if ( c->tree_layers_count )
	{
		free(c->tree_layers);
		c->tree_layers = NULL;
		c->tree_layers_count = 0;
	}
}

void reset_daily_class_variables(cell_t *const c)
{
	int height;
	int dbh;
	int age;
	int species;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	logger(g_debug_log, "\n...resetting class level daily variables...\n");

	/* height class level */
	for ( height = c->heights_count - 1; height >= 0; height--)
	{
		h = &c->heights[height];

		/* dbh class level */
		for ( dbh = h->dbhs_count - 1 ; dbh >= 0 ; --dbh )
		{
			d = &h->dbhs[dbh];

			/* age class level */
			for (age = d->ages_count - 1; age >= 0; age --)
			{
				a = &d->ages[age];

				/* species level */
				for (species = a->species_count - 1; species >= 0; species --)
				{
					s = &a->species[species];

					/* reset daily radiative variables */
					s->value[PAR] =                     0.;
					s->value[APAR] =                    0.;
					s->value[APAR_SUN] =                0.;
					s->value[APAR_SHADE] =              0.;
					s->value[TRANSM_PAR] =              0.;
					s->value[TRANSM_PAR_SUN] =          0.;
					s->value[TRANSM_PAR_SHADE] =        0.;
					s->value[SW_RAD] =                  0.;
					s->value[SW_RAD_ABS] =              0.;
					s->value[SW_RAD_ABS_SUN] =          0.;
					s->value[SW_RAD_ABS_SHADE] =        0.;
					s->value[SW_RAD_TRANSM] =           0.;
					s->value[SW_RAD_TRANSM_SUN] =       0.;
					s->value[SW_RAD_TRANSM_SHADE] =     0.;
					s->value[PPFD] =                    0.;
					s->value[PPFD_ABS] =                0.;
					s->value[PPFD_ABS_SUN] =            0.;
					s->value[PPFD_ABS_SHADE] =          0.;
					s->value[PPFD_TRANSM] =             0.;
					s->value[PPFD_TRANSM_SUN] =         0.;
					s->value[PPFD_TRANSM_SHADE] =       0.;

					/* reset daily carbon fluxes */
					s->value[GPP_gC] =                  0.;
					s->value[NPP_gC] =                  0.;
					s->value[NPP_tC] =                  0.;
					s->value[C_FLUX] =                  0.;
					s->value[CUE] =                     0.;

					/* reset daily water fluxes */
					s->value[CANOPY_INT] =              0.;
					s->value[CANOPY_EVAPO] =            0.;
					s->value[CANOPY_TRANSP] =           0.;
					s->value[CANOPY_EVAPO_TRANSP] =     0.;
					s->value[WUE] =                     0.;
					s->value[iWUE] =                    0.;

					/* reset daily multipliers */
					s->value[F_CO2] =                   0.;
					s->value[F_LIGHT] =                 0.;
					s->value[F_T] =                     0.;
					s->value[F_VPD] =                   0.;
					s->value[F_AGE] =                   0.;
					s->value[F_NUTR] =                  0.;
					s->value[F_SW] =                    0.;
					s->value[F_PSI] =                   0.;
					s->value[PHYS_MOD] =                0.;

					/*****************************************/
					/* reset daily carbon fluxes among pools */
					s->value[C_TO_LEAF] =               0.;
					s->value[C_TO_ROOT] =               0.;
					s->value[C_TO_FROOT] =              0.;
					s->value[C_TO_CROOT] =              0.;
					s->value[C_TO_TOT_STEM] =           0.;
					s->value[C_TO_STEM] =               0.;
					s->value[C_TO_BRANCH] =             0.;
					s->value[C_TO_RESERVE] =            0.;
					s->value[C_TO_FRUIT] =              0.;
					s->value[C_TO_LITR] =               0.;
					s->value[C_LEAF_TO_RESERVE] =       0.;
					s->value[C_FROOT_TO_RESERVE] =      0.;
					s->value[C_LEAF_TO_LITR] =          0.;
					s->value[C_FROOT_TO_LITR] =         0.;

					/* reset daily nitrogen fluxes among pools */
					s->value[N_TO_LEAF] =               0.;
					s->value[N_TO_ROOT] =               0.;
					s->value[N_TO_FROOT] =              0.;
					s->value[N_TO_CROOT] =              0.;
					s->value[N_TO_TOT_STEM] =           0.;
					s->value[N_TO_STEM] =               0.;
					s->value[N_TO_BRANCH] =             0.;
					s->value[N_TO_RESERVE] =            0.;
					s->value[N_TO_FRUIT] =              0.;
					s->value[N_TO_LITTER] =             0.;
					s->value[N_LEAF_TO_RESERVE] =       0.;
					s->value[N_FROOT_TO_RESERVE] =      0.;
					s->value[N_LEAF_TO_LITR] =          0.;
					s->value[N_FROOT_TO_LITR] =         0.;
					/****************************************/

					/* reset daily maintenance, growth and total respiration */
					s->value[DAILY_LEAF_MAINT_RESP] =   0.;
					s->value[NIGHTLY_LEAF_MAINT_RESP] = 0.;
					s->value[TOT_DAY_LEAF_MAINT_RESP] = 0.;
					s->value[FROOT_MAINT_RESP] =        0.;
					s->value[STEM_MAINT_RESP] =         0.;
					s->value[BRANCH_MAINT_RESP] =       0.;
					s->value[CROOT_MAINT_RESP] =        0.;
					s->value[TOTAL_MAINT_RESP] =        0.;
					s->value[LEAF_GROWTH_RESP] =        0.;
					s->value[FROOT_GROWTH_RESP] =       0.;
					s->value[CROOT_GROWTH_RESP] =       0.;
					s->value[STEM_GROWTH_RESP] =        0.;
					s->value[BRANCH_GROWTH_RESP] =      0.;
					s->value[TOTAL_GROWTH_RESP] =       0.;
					s->value[LEAF_AUT_RESP] =           0.;
					s->value[FROOT_AUT_RESP] =          0.;
					s->value[STEM_AUT_RESP] =           0.;
					s->value[BRANCH_AUT_RESP] =         0.;
					s->value[CROOT_AUT_RESP] =          0.;
					s->value[TOTAL_AUT_RESP] =          0.;
				}
			}
		}
	}
}

void reset_monthly_class_variables(cell_t *const c)
{
	int height;
	int dbh;
	int age;
	int species;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	logger(g_debug_log, "\n...resetting class level monthly variables...\n");

	/* height class level */
	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		h = &c->heights[height];

		/* dbh class level */
		for ( dbh = h->dbhs_count - 1 ; dbh >= 0 ; --dbh )
		{
			d = &h->dbhs[dbh];

			/* age class level */
			for (age = d->ages_count - 1; age >= 0; age --)
			{
				a = &d->ages[age];

				/* species level */
				for (species = a->species_count - 1; species >= 0; species -- )
				{
					s = &a->species[species];

					s->value[MONTHLY_GPP_gC] =               0;
					s->value[MONTHLY_TOTAL_AUT_RESP] =       0.;
					s->value[MONTHLY_NPP_gC] =               0.;
					s->value[MONTHLY_CUE] =                  0.;
					s->value[MONTHLY_WUE] =                  0.;
					s->value[MONTHLY_iWUE] =                 0.;
					s->value[MONTHLY_CANOPY_LATENT_HEAT] =   0.;
					s->value[MONTHLY_TOTAL_MAINT_RESP] =     0.;
					s->value[MONTHLY_TOTAL_GROWTH_RESP] =    0.;
					s->value[MONTHLY_LEAF_AUT_RESP] =        0.;
					s->value[MONTHLY_FROOT_AUT_RESP] =       0.;
					s->value[MONTHLY_STEM_AUT_RESP] =        0.;
					s->value[MONTHLY_CROOT_AUT_RESP] =       0.;
					s->value[MONTHLY_BRANCH_AUT_RESP] =      0.;
					s->value[MONTHLY_TOTAL_AUT_RESP] =       0.;
					s->value[MONTHLY_CANOPY_TRANSP] =        0.;
					s->value[MONTHLY_CANOPY_INT] =           0.;
					s->value[MONTHLY_CANOPY_EVAPO_TRANSP] =  0.;
					s->value[MONTHLY_CANOPY_LATENT_HEAT] =   0.;
					s->value[M_C_TO_TOT_STEM] =              0.;
					s->value[M_C_TO_STEM] =                  0.;
					s->value[M_C_TO_LEAF] =                  0.;
					s->value[M_C_TO_FROOT] =                 0.;
					s->value[M_C_TO_CROOT] =                 0.;
					s->value[M_C_TO_RESERVE] =               0.;
					s->value[M_C_TO_ROOT] =                  0.;
					s->value[M_C_TO_BRANCH] =                0.;
					s->value[M_C_TO_FRUIT] =                 0.;
				}
			}
		}
	}
}


void reset_annual_class_variables(cell_t *const c)
{
	int height;
	int dbh;
	int age;
	int species;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	logger(g_debug_log, "\n...resetting class level annual variables...\n");

	/* height class level */
	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		h = &c->heights[height];

		/* dbh class level */
		for ( dbh = h->dbhs_count - 1 ; dbh >= 0 ; --dbh )
		{
			d = &h->dbhs[dbh];

			/* age class level */
			for (age = d->ages_count - 1; age >= 0; age --)
			{
				a = &d->ages[age];

				/* species level */
				for (species = a->species_count - 1; species >= 0; species -- )
				{
					s = &a->species[species];

					s->value[PEAK_LAI_PROJ] =               0.;
					s->value[MAX_LEAF_C] =                  0.;

					/* leaf fine root fall */
					s->value[LEAF_TO_REMOVE] =              0.;
					s->value[FROOT_TO_REMOVE] =             0.;

					/*reset cumulative values*/
					s->counter[LEAF_FALL_COUNTER] =         0;
					s->counter[VEG_DAYS] =                  0;
					s->counter[YEARLY_VEG_DAYS] =           0;
					s->counter[FIRST_VEG_DAYS] =            0;
					s->value[YEARLY_GPP_gC] =               0;
					s->value[YEARLY_NPP_gC] =               0.;
					s->value[YEARLY_NPP_tC] =               0.;
					s->value[YEARLY_CUE] =                  0.;
					s->value[YEARLY_WUE] =                  0.;
					s->value[YEARLY_iWUE] =                 0.;
					s->value[YEARLY_APAR] =                 0.;
					s->value[YEARLY_TOTAL_MAINT_RESP] =     0.;
					s->value[YEARLY_TOTAL_GROWTH_RESP] =    0.;
					s->value[YEARLY_LEAF_AUT_RESP] =        0.;
					s->value[YEARLY_FROOT_AUT_RESP] =       0.;
					s->value[YEARLY_STEM_AUT_RESP] =        0.;
					s->value[YEARLY_CROOT_AUT_RESP] =       0.;
					s->value[YEARLY_BRANCH_AUT_RESP] =      0.;
					s->value[YEARLY_TOTAL_AUT_RESP] =       0.;
					s->value[YEARLY_CANOPY_TRANSP] =        0.;
					s->value[YEARLY_CANOPY_INT] =           0.;
					s->value[YEARLY_CANOPY_EVAPO_TRANSP] =  0.;
					s->value[YEARLY_CANOPY_LATENT_HEAT] =   0.;
					s->value[Y_C_TO_TOT_STEM] =             0.;
					s->value[Y_C_TO_STEM] =                 0.;
					s->value[Y_C_TO_LEAF] =                 0.;
					s->value[Y_C_TO_FROOT] =                0.;
					s->value[Y_C_TO_CROOT] =                0.;
					s->value[Y_C_TO_RESERVE] =              0.;
					s->value[Y_C_TO_ROOT] =                 0.;
					s->value[Y_C_TO_BRANCH] =               0.;
					s->value[Y_C_TO_FRUIT] =                0.;
					s->counter[DEAD_STEMS] =                0;
					s->counter[N_TREE_SAP] =                0;
				}
			}
		}
	}
}

