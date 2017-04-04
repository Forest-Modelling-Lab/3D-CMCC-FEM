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
		logger_all("DOY = %d\n", c->doy);
		logger_all("PAR in = %g\n", in);
		logger_all("PAR out = %g\n", out);
		logger_all("PAR store = %g\n", store);
		logger_all("PAR balance = %g\n", balance);
		logger_all("...FATAL ERROR in 'Tree_model_daily' PAR balance (exit)\n");

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
		logger_all("DOY = %d\n", c->doy);
		logger_all("NET_SW_RAD in = %g\n", in);
		logger_all("NET_RAD out = %g\n", out);
		logger_all("NET_RAD store = %g\n", store);
		logger_all("NET_RAD balance = %g\n", balance);
		logger_all("...FATAL ERROR AT CLASS LEVEL NET_RAD balance (exit)\n");

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
		logger_all("DOY = %d\n", c->doy);
		logger_all("PPFD in = %g\n", in);
		logger_all("PPFD out = %g\n", out);
		logger_all("PPFD store = %g\n", store);
		logger_all("PPFD balance = %g\n", balance);
		logger_all("...FATAL ERROR in 'Tree_model_daily' PPFD balance (exit)\n");

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
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin = %g gC/m2/day\n", in);
		logger_all("GPP = %g gC/m2/day\n", s->value[GPP]);
		logger_all("\nout = %g gC/m2/day\n", out);
		logger_all("TOTAL_AUT_RESP = %g gC/m2/day\n", s->value[TOTAL_AUT_RESP]);
		logger_all("\nstore = %g gC/m2/day\n", store);
		logger_all("NPP = %g gC/m2/day\n", s->value[NPP]);
		logger_all("\nbalance = %g gC/m2/day\n", balance);
		logger_all("...FATAL ERROR in 'Tree_model_daily' carbon flux balance (first) (exit)\n");

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
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin = %g tC/sizecell/day\n", in);
		logger_all("GPP_gC = %g tC/sizecell/day\n", s->value[GPP_tC]);
		logger_all("\nout = %g tC/sizecell/day\n", out);
		logger_all("TOTAL_MAINT_RESP = %g tC/sizecell/day\n", s->value[TOTAL_MAINT_RESP_tC]);
		logger_all("TOTAL_GROWTH_RESP = %g tC/sizecell/day\n", s->value[TOTAL_GROWTH_RESP_tC]);
		logger_all("C_LEAF_TO_LITR = %g tC/sizecell/day\n", s->value[C_LEAF_TO_LITR]);
		logger_all("C_FROOT_TO_LITR = %g tC/sizecell/day\n", s->value[C_FROOT_TO_LITR]);
		logger_all("\nstore = %g tC/sizecell/day\n", store);
		logger_all("C_TO_LEAF = %g tC/sizecell/day\n", s->value[C_TO_LEAF]);
		logger_all("C_TO_FROOT = %g tC/sizecell/day\n", s->value[C_TO_FROOT]);
		logger_all("C_TO_CROOT = %g tC/sizecell/day\n", s->value[C_TO_CROOT]);
		logger_all("C_TO_STEM = %g tC/sizecell/day\n", s->value[C_TO_STEM]);
		logger_all("C_TO_RESERVE = %g tC/sizecell/day\n", s->value[C_TO_RESERVE]);
		logger_all("C_TO_BRANCH = %g tC/sizecell/day\n", s->value[C_TO_BRANCH]);
		logger_all("C_TO_FRUIT = %g tC/sizecell/day\n", s->value[C_TO_FRUIT]);
		logger_all("\nbalance = %g tC/sizecell/day\n", balance);
		logger_all("...FATAL ERROR in 'Tree_model_daily'  carbon flux balance (second) (exit)\n");

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
		logger_all("DOS = %d\n", c->dos);
		logger_all("\nin = %g tC/sizecell/day\n", in);
		logger_all("GPP = %g tC/sizecell\n", s->value[GPP_tC]);
		logger_all("\nout = %g tC/sizecell/day\n", out);
		logger_all("TOTAL_MAINT_RESP_tC = %g tC/sizecell/day\n", s->value[TOTAL_MAINT_RESP_tC]);
		logger_all("TOTAL_GROWTH_RESP_tC = %g tC/sizecell/day\n", s->value[TOTAL_GROWTH_RESP_tC]);
		logger_all("C_TO_LITR = %g tC/sizecell/day\n", s->value[C_TO_LITR]);
		logger_all("C_TO_CWD = %g tC/sizecell/day\n", s->value[C_TO_CWD]);
		logger_all("\nold_store = %g tC/sizecell\n", old_store);
		logger_all("store = %g tC/sizecell\n", store);
		logger_all("store - old_tore = %g tC/sizecell\n", store - old_store);
		logger_all("LEAF_C = %g tC/cell/day\n", s->value[LEAF_C]);
		logger_all("FROOT_C = %g tC/cell/day\n", s->value[FROOT_C]);
		logger_all("CROOT_C = %g tC/cell/day\n", s->value[CROOT_C]);
		logger_all("STEM_C = %g tC/cell/day\n", s->value[STEM_C]);
		logger_all("BRANCH_C = %g tC/cell/day\n", s->value[BRANCH_C]);
		logger_all("RESERVE_C = %g tC/cell/day\n", s->value[RESERVE_C]);
		logger_all("FRUIT_C = %g tC/cell/day\n", s->value[FRUIT_C]);
		logger_all("\nbalance = %g tC/sizecell\n", balance);
		logger_all("...FATAL ERROR in 'Tree_model_daily' carbon mass balance (exit)\n");

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
		logger_all("DOY = %d\n", c->doy);
		logger_all("\nin = %g tN/sizecell/day\n", in);
		logger_all("NPP_tN = %g tN/sizecell/day\n", s->value[NPP_tN]);
		logger_all("\nout = %g tN/sizecell/day\n", out);
		logger_all("N_LEAF_TO_LITR = %g tN/sizecell/day\n", s->value[N_LEAF_TO_LITR]);
		logger_all("N_FROOT_TO_LITR = %g tN/sizecell/day\n", s->value[N_FROOT_TO_LITR]);
		logger_all("\nstore = %g tN/sizecell/day\n", store);
		logger_all("N_TO_LEAF = %g tN/sizecell/day\n", s->value[N_TO_LEAF]);
		logger_all("N_TO_FROOT = %g tN/sizecell/day\n", s->value[N_TO_FROOT]);
		logger_all("N_TO_CROOT = %g tN/sizecell/day\n", s->value[N_TO_CROOT]);
		logger_all("N_TO_STEM = %g tN/sizecell/day\n", s->value[N_TO_STEM]);
		logger_all("N_TO_RESERVE = %g tN/sizecell/day\n", s->value[N_TO_RESERVE]);
		logger_all("N_TO_BRANCH = %g tN/sizecell/day\n", s->value[N_TO_BRANCH]);
		logger_all("N_TO_FRUIT = %g tN/sizecell/day\n", s->value[N_TO_FRUIT]);
		logger_all("\nbalance = %g tN/sizecell/day\n", balance);
		logger_all("...FATAL ERROR in 'Tree_model_daily'  nitrogen flux balance (exit)\n");

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
		logger_all("DOY = %d\n", c->doy);
		logger_all("canopy water in = %g\n", in);
		logger_all("canopy water out = %g\n", out);
		logger_all("canopy water store = %g\n", store);
		logger_all("canopy water balance = %g\n", balance);
		logger_all("...FATAL ERROR in 'Tree_model_daily' canopy water balance (exit)\n");

		return 0;
	}
	else
	{
		old_store = store;
		logger(g_debug_log, "...ok in 'Tree_model_daily' canopy water balance\n");
	}

	return 1;
}

