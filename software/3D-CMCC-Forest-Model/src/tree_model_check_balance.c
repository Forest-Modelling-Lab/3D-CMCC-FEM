/*
 * tree_model_check_balance.c
 *
 *  Created on: 03 apr 2017
 *      Author: alessio
 */
/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "common.h"
#include "check_balance.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern logger_t* g_debug_log;


int check_tree_class_radiation_flux_balance(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* DAILY CHECK ON CLASS LEVEL CANOPY POOL-ATMOSPHERE RADIATIVE BALANCE */
	//fixme: for all the balance doesn't takes into account the amount of light previously removed in Radiation function

	/* PAR balance */
	/* sum of sources */
	in = s->value[PAR];

	/* sum of sinks */
	out = s->value[PAR_REFL] + s->value[TRANSM_PAR];

	/* sum of current storage */
	store = s->value[APAR_SUN] + s->value[APAR_SHADE];

	/* check canopy water pool balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL PAR BALANCE\n");

	/* check for PAR balance closure*/
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("PAR in = %g\n", in);
		printf("PAR out = %g\n", out);
		printf("PAR store = %g\n", store);
		printf("PAR balance = %g\n", balance);
		printf("...FATAL ERROR in 'Tree_model_daily' PAR balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok 'Tree_model_daily' PAR balance\n");
	}
	/****************************************************************************************************************/
	/* Net Short-Wave radiation balance */
	/* sum of sources */
	in      = s->value[SW_RAD];

	/* sum of sinks */
	/* it must take into account the overall transmitted NET_RAD (reflected is yet computed for net radiation) */
	out     = s->value[SW_RAD_REFL] + s->value[SW_RAD_TRANSM];

	/* sum of current storage */
	store   = s->value[SW_RAD_ABS_SUN] + s->value[SW_RAD_ABS_SHADE];

	/* check canopy water pool balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL Short Wave BALANCE\n");

	/* check for NET_RAD balance closure*/
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("NET_SW_RAD in = %g\n", in);
		printf("NET_RAD out = %g\n", out);
		printf("NET_RAD store = %g\n", store);
		printf("NET_RAD balance = %g\n", balance);
		printf("...FATAL ERROR AT CLASS LEVEL NET_RAD balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok in 'Tree_model_daily' Short Wave radiation balance \n");
	}
	/****************************************************************************************************************/
	/* PPFD balance */
	/* sum of sources */
	in      = s->value[PPFD];

	/* sum of sinks */
	out     = s->value[PPFD_REFL] +  s->value[PPFD_TRANSM];

	/* sum of current storage */
	store   = s->value[PPFD_ABS_SUN] + s->value[PPFD_ABS_SHADE];

	/* check canopy water pool balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL PPFD BALANCE\n");

	/* check for PPFD balance closure*/
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("PPFD in = %g\n", in);
		printf("PPFD out = %g\n", out);
		printf("PPFD store = %g\n", store);
		printf("PPFD balance = %g\n", balance);
		printf("...FATAL ERROR in 'Tree_model_daily' PPFD balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok in 'Tree_model_daily' PPFD balance\n");
	}

	return 1;
}

int check_tree_class_carbon_flux_balance(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* DAILY CHECK ON CLASS LEVEL CARBON FLUX BALANCE */
	/* check complete tree level carbon flux balance */

	/* sum of sources */
	in      = s->value[GPP];

	/* sum of sinks */
	out     = s->value[TOTAL_AUT_RESP];

	/* sum of current storage */
	store   = s->value[NPP];

	/* check carbon flux balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL CARBON FLUX BALANCE (first)\n");

	/* check for carbon flux balance closure */
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin = %g gC/m2/day\n", in);
		printf("GPP = %g gC/m2/day\n", s->value[GPP]);
		printf("\nout = %g gC/m2/day\n", out);
		printf("TOTAL_AUT_RESP = %g gC/m2/day\n", s->value[TOTAL_AUT_RESP]);
		printf("\nstore = %g gC/m2/day\n", store);
		printf("NPP = %g gC/m2/day\n", s->value[NPP]);
		printf("\nbalance = %g gC/m2/day\n", balance);
		printf("...FATAL ERROR in 'Tree_model_daily' carbon flux balance (first) (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok in 'Tree_model_daily' carbon flux balance (first)\n");
	}

	/* DAILY CHECK ON CLASS LEVEL CARBON BALANCE */
	/* check complete tree level carbon balance */

	/* sum of sources */
	in      = s->value[GPP_tC];

	/* sum of sinks */
	out     = s->value[TOTAL_MAINT_RESP_tC] + s->value[TOTAL_GROWTH_RESP_tC] +
			( s->value[C_LEAF_TO_LITR] + s->value[C_FROOT_TO_LITR] + s->value[C_FRUIT_TO_LITR]);

	/* sum of current storage */
	store   = s->value[C_TO_LEAF]  +
			s->value[C_TO_STEM]    +
			s->value[C_TO_FROOT]   +
			s->value[C_TO_CROOT]   +
			s->value[C_TO_BRANCH]  +
			s->value[C_TO_RESERVE] +
			s->value[C_TO_FRUIT]   ;

	/* check carbon flux balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL CARBON FLUX BALANCE (second)\n");

	/* check for carbon flux balance closure */
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin = %g tC/sizecell/day\n", in);
		printf("GPP_gC = %g tC/sizecell/day\n", s->value[GPP_tC]);
		printf("\nout = %g tC/sizecell/day\n", out);
		printf("TOTAL_MAINT_RESP = %g tC/sizecell/day\n", s->value[TOTAL_MAINT_RESP_tC]);
		printf("TOTAL_GROWTH_RESP = %g tC/sizecell/day\n", s->value[TOTAL_GROWTH_RESP_tC]);
		printf("C_LEAF_TO_LITR = %g tC/sizecell/day\n", s->value[C_LEAF_TO_LITR]);
		printf("C_FROOT_TO_LITR = %g tC/sizecell/day\n", s->value[C_FROOT_TO_LITR]);
		printf("\nstore = %g tC/sizecell/day\n", store);
		printf("C_TO_LEAF = %g tC/sizecell/day\n", s->value[C_TO_LEAF]);
		printf("C_TO_FROOT = %g tC/sizecell/day\n", s->value[C_TO_FROOT]);
		printf("C_TO_CROOT = %g tC/sizecell/day\n", s->value[C_TO_CROOT]);
		printf("C_TO_STEM = %g tC/sizecell/day\n", s->value[C_TO_STEM]);
		printf("C_TO_RESERVE = %g tC/sizecell/day\n", s->value[C_TO_RESERVE]);
		printf("C_TO_BRANCH = %g tC/sizecell/day\n", s->value[C_TO_BRANCH]);
		printf("C_TO_FRUIT = %g tC/sizecell/day\n", s->value[C_TO_FRUIT]);
		printf("\nbalance = %g tC/sizecell/day\n", balance);
		printf("...FATAL ERROR in 'Tree_model_daily'  carbon flux balance (second) (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok in 'Tree_model_daily' carbon flux balance (second)\n");
	}

	/*******************************************************************************************************************/
	return 1;
}

int check_tree_class_carbon_mass_balance(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;
	static double old_store;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* DAILY CHECK ON CLASS LEVEL CARBON MASS BALANCE */
	/* check complete tree level carbon mass balance */

	/* sum of sources */
	in      = s->value[GPP_tC];

	/* sum of sinks */
	out     = s->value[TOTAL_MAINT_RESP_tC] +
			s->value[TOTAL_GROWTH_RESP_tC] +
			s->value[C_TO_LITR] + s->value[C_TO_CWD];

	/* sum of current storage */
	store   = s->value[LEAF_C]  +
			s->value[FROOT_C]   +
			s->value[CROOT_C]   +
			s->value[STEM_C]    +
			s->value[BRANCH_C]  +
			s->value[RESERVE_C] +
			s->value[FRUIT_C]   ;

	/* check carbon pool balance */
	balance = in - out - (store - old_store);

	logger(g_debug_log, "\nCLASS LEVEL CARBON MASS BALANCE\n");

	/* check for carbon mass balance closure */
	if ( ( fabs( balance ) > eps ) && ( c->dos > 1 ) )
	{
		printf("DOS = %d\n", c->dos);
		printf("\nin = %g tC/sizecell/day\n", in);
		printf("GPP = %g tC/sizecell\n", s->value[GPP_tC]);
		printf("\nout = %g tC/sizecell/day\n", out);
		printf("TOTAL_MAINT_RESP_tC = %g tC/sizecell/day\n", s->value[TOTAL_MAINT_RESP_tC]);
		printf("TOTAL_GROWTH_RESP_tC = %g tC/sizecell/day\n", s->value[TOTAL_GROWTH_RESP_tC]);
		printf("C_TO_LITR = %g tC/sizecell/day\n", s->value[C_TO_LITR]);
		printf("C_TO_CWD = %g tC/sizecell/day\n", s->value[C_TO_CWD]);
		printf("\nold_store = %g tC/sizecell\n", old_store);
		printf("store = %g tC/sizecell\n", store);
		printf("store - old_tore = %g tC/sizecell\n", store - old_store);
		printf("LEAF_C = %g tC/cell/day\n", s->value[LEAF_C]);
		printf("FROOT_C = %g tC/cell/day\n", s->value[FROOT_C]);
		printf("CROOT_C = %g tC/cell/day\n", s->value[CROOT_C]);
		printf("STEM_C = %g tC/cell/day\n", s->value[STEM_C]);
		printf("BRANCH_C = %g tC/cell/day\n", s->value[BRANCH_C]);
		printf("RESERVE_C = %g tC/cell/day\n", s->value[RESERVE_C]);
		printf("FRUIT_C = %g tC/cell/day\n", s->value[FRUIT_C]);
		printf("\nbalance = %g tC/sizecell\n", balance);
		printf("...FATAL ERROR in 'Tree_model_daily' carbon mass balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		old_store = store;
		logger(g_debug_log, "...ok in 'Tree_model_daily' carbon mass balance\n");
	}

	/*******************************************************************************************************************/
	return 1;
}
int check_tree_class_nitrogen_flux_balance (cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* DAILY CHECK ON CLASS LEVEL NITROGEN BALANCE */
	/* check complete tree level nitrogen balance */

	/* sum of sources */
	in      = s->value[NPP_tN];

	/* sum of sinks */
	out     =  s->value[N_LEAF_TO_LITR] +
			s->value[N_FROOT_TO_LITR]   +
			s->value[N_FRUIT_TO_LITR]   +
			s->value[N_TO_CWD];

	/* sum of current storage */
	store   = s->value[N_TO_LEAF] +
			s->value[N_TO_STEM]   +
			s->value[N_TO_FROOT]  +
			s->value[N_TO_CROOT]  +
			s->value[N_TO_BRANCH] +
			s->value[N_TO_RESERVE];

	/* check nitrogen flux balance */
	balance = in - out - store;

	logger(g_debug_log, "\nCLASS LEVEL NITROGEN FLUX BALANCE\n");

	/* check for nitrogen flux balance closure */
	if ( fabs( balance ) > eps )
	{
		printf("DOY = %d\n", c->doy);
		printf("\nin = %g tN/sizecell/day\n", in);
		printf("NPP_tN = %g tN/sizecell/day\n", s->value[NPP_tN]);
		printf("\nout = %g tN/sizecell/day\n", out);
		printf("N_LEAF_TO_LITR = %g tN/sizecell/day\n", s->value[N_LEAF_TO_LITR]);
		printf("N_FROOT_TO_LITR = %g tN/sizecell/day\n", s->value[N_FROOT_TO_LITR]);
		printf("\nstore = %g tN/sizecell/day\n", store);
		printf("N_TO_LEAF = %g tN/sizecell/day\n", s->value[N_TO_LEAF]);
		printf("N_TO_FROOT = %g tN/sizecell/day\n", s->value[N_TO_FROOT]);
		printf("N_TO_CROOT = %g tN/sizecell/day\n", s->value[N_TO_CROOT]);
		printf("N_TO_STEM = %g tN/sizecell/day\n", s->value[N_TO_STEM]);
		printf("N_TO_RESERVE = %g tN/sizecell/day\n", s->value[N_TO_RESERVE]);
		printf("N_TO_BRANCH = %g tN/sizecell/day\n", s->value[N_TO_BRANCH]);
		printf("N_TO_FRUIT = %g tN/sizecell/day\n", s->value[N_TO_FRUIT]);
		printf("\nbalance = %g tN/sizecell/day\n", balance);
		printf("...FATAL ERROR in 'Tree_model_daily'  nitrogen flux balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		logger(g_debug_log, "...ok in 'Tree_model_daily' nitrogen flux balance\n");
	}

	/*******************************************************************************************************************/
	return 1;
}

int check_tree_class_water_flux_balance(cell_t *const c, const int layer, const int height, const int dbh, const int age, const int species)
{
	double in;
	double out;
	double store;
	double balance;
	static double old_store;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* DAILY CHECK ON CLASS LEVEL CANOPY POOL-ATMOSPHERE WATER BALANCE */

	/* sum of sources */
	in      = s->value[CANOPY_INT];

	/* sum of sinks */
	out     = s->value[CANOPY_EVAPO];

	/* sum of current storage */
	store   = s->value[CANOPY_WATER];

	/* check canopy water pool balance */
	balance = in - out - ( store - old_store );

	logger(g_debug_log, "\nCLASS LEVEL WATER BALANCE\n");

	if ( ( fabs( balance ) > eps ) && ( c->dos > 1 ) )
	{
		printf("DOY = %d\n", c->doy);
		printf("canopy water in = %g\n", in);
		printf("canopy water out = %g\n", out);
		printf("canopy water store = %g\n", store);
		printf("canopy water balance = %g\n", balance);
		printf("...FATAL ERROR in 'Tree_model_daily' canopy water balance (exit)\n");
		CHECK_CONDITION (fabs( balance ), >, eps);

		return 0;
	}
	else
	{
		old_store = store;
		logger(g_debug_log, "...ok in 'Tree_model_daily' canopy water balance\n");
	}

	return 1;
}

