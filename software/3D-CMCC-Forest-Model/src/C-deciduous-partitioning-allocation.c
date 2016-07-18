/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "C-deciduous-partitioning-allocation.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"
#include "leaffall.h"
#include "biomass.h"
#include "dendometry.h"
#include "turnover.h"
#include "lai.h"
#include "C-allocation.h"

extern settings_t* g_settings;
extern logger_t* g_log;

/* Deciduous carbon allocation routine */
void Daily_C_Deciduous_Partitioning_Allocation(cell_t *const c, const int layer, const int height, const int age, const int species, const meteo_daily_t *const meteo_daily, const int day, const int year)
{
	//int i;
	double  s0Ctem;
	double  r0Ctem;
	double omegaCtem;
	double pS_CTEM = 0.0;
	double pR_CTEM = 0.0;
	double pF_CTEM = 0.0;
	double Light_trasm;
	double Perc_fine;
	//static double frac_to_foliage_fineroot;
	static double reserve_for_foliage_budburst;
	static double reserve_for_fine_root_budburst;
	static double reserve_for_budburst;

	/* for check */
	double npp_to_alloc;
	double npp_alloc;

	height_t *h;
	h = &c->heights;

	age_t *a;
	a = &c->heights[height].ages;

	species_t *s;
	s = &c->heights[height].ages[age].species[species];

	s0Ctem = s->value[S0CTEM];
	r0Ctem = s->value[R0CTEM];
	omegaCtem = s->value[OMEGA_CTEM];

	/* in Biome a constant proportion (50%) (Growth:storage parameter) of NPP that goes to the cpools is allocated
	 *  to each storage_pool, i.e. each carbon pools receive just a part of NPP (50%) the remaining remain as storage
	 * and used to maintain trees when NPP is < 0 */
	//i = c->t_layers[layer].heights[height].z;

	logger(g_log, "\n**ALLOCATION_ROUTINE**\n\n");
	logger(g_log, "Carbon allocation routine for deciduous\n");

	/* following Arora and Boer 2005 */
	Light_trasm = exp(- s->value[K] * s->value[LAI]);

	/* partitioning block using CTEM approach */
	logger(g_log, "\n*Partitioning ratios*\n");
	pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW]))) / (1.0 + (omegaCtem * (2.0 - Light_trasm - s->value[F_SW])));
	logger(g_log, "Roots CTEM ratio = %f %%\n", pR_CTEM * 100);
	pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * (2.0 - Light_trasm - s->value[F_SW])));
	logger(g_log, "Stem CTEM ratio = %f %%\n", pS_CTEM * 100);
	pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
	logger(g_log, "Reserve CTEM ratio = %f %%\n", pF_CTEM * 100);

	//fixme to check it, values are too high for fine root
	/* fine root vs. coarse root ratio */
	s->value[FR_CR] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * (1.0 / s->value[STEM_LEAF]);
	logger(g_log, "Fine/Coarse root ratio = %f\n", s->value[FR_CR] );
	Perc_fine = s->value[FR_CR] / (s->value[FR_CR] + 1.0);
	logger(g_log, "Percentage of fine root against total root= %f %%\n", Perc_fine * 100 );


	if (s->counter[VEG_DAYS] == 1)
	{
		s->counter[BUD_BURST_COUNTER] = (int)s->value[BUD_BURST];
		logger(g_log, "First day of budburst\n");
		logger(g_log, "Days for bud burst = %f\n", s->value[BUD_BURST]);
	}
	if (s->counter[VEG_DAYS] > 1 && s->counter[VEG_DAYS] <= s->value[BUD_BURST])
	{
		s->counter[BUD_BURST_COUNTER] --;
		logger(g_log, "++Remaining days for bud burst = %d\n", s->counter[BUD_BURST_COUNTER]);
	}
	if (s->counter[VEG_DAYS] > s->value[BUD_BURST])
	{
		s->counter[BUD_BURST_COUNTER] = 0;
	}

	//I could try to get in instead F_SW the minimum value among F_SW and F_VPD and F_NUTR 2 apr 2012
	//reductor = Minimum (s->value[F_SW], s->value[F_VPD], s->value[F_NUTR]);

	logger(g_log, "CARBON PARTITIONING-ALLOCATION FOR LAYER %d\n", c->t_layers[layer].z);

	/* it mainly follows Arora V. K., Boer G. J., GCB, 2005 */

	logger(g_log, "PHENOLOGICAL PHASE = %d\n", s->phenology_phase);
	logger(g_log, "LAI = %f \n", s->value[LAI]);
	logger(g_log, "PEAK LAI = %f \n", s->value[PEAK_LAI]);

	/* assign NPP to local variable */
	npp_to_alloc = s->value[NPP_tC];

	switch (s->phenology_phase)
	{
	/************************************************************/
	case 1:
		logger(g_log, "BUDBURST\n");
		logger(g_log, "Bud burst phase using both reserve pools and npp\n");
		logger(g_log, "Allocating only into foliage and fine root\n");
		logger(g_log, "LAI = %f \n", s->value[LAI]);
		logger(g_log, "Tot biomass reserve = %f\n", s->value[RESERVE_C]);
		logger(g_log, "++Remaining days for bud burst = %d\n", s->counter[BUD_BURST_COUNTER]);

		/* test "This has recently been confirmed by Dyckmans et al. (2000)
		who  showed  that  only  44%  of  carbon  in  leaves  came  from
		carbon reserves in beech trees" (Barbaroux et al., 2003) */

		/* following Friedlingstein et al.,1998 and Krinner et al.,2005 during budburst model allocates
		 to leaf and fine root */

		/*following Campioli et al., 2008, Maillard et al., 1994, Barbaroux et al., 2003*/

		//test check it it seem that doesn't work!!
		//SERGIO
		//frac_to_foliage_fineroot = (s->value[RESERVE]) / s->counter[BUD_BURST_COUNTER];
		//parameter = 2.0 / pow(s->value[BUD_BURST],2.0);
		//frac_to_foliage_fineroot = (s->value[RESERVE]) * parameter * (s->value[BUD_BURST]+1.0 - s->counter[BUD_BURST_COUNTER]);
		//logger(g_log, "fraction of reserve for foliage and fine root = %f\n", frac_to_foliage_fineroot);

		reserve_for_foliage_budburst = s->value[MAX_LEAF_C] / (s->value[BUD_BURST]+1.0);
		logger(g_log, "daily amount of reserve for foliage budburst %f = tC/cell/day\n", reserve_for_foliage_budburst);

		reserve_for_fine_root_budburst = s->value[MAX_FINE_ROOT_C] / (s->value[BUD_BURST]+1.0);
		logger(g_log, "daily amount of reserve for foliage budburst %f = tC/cell/day\n", reserve_for_foliage_budburst);

		reserve_for_budburst = reserve_for_foliage_budburst + reserve_for_fine_root_budburst;
		logger(g_log, "daily amount of reserve for foliage  and fine roots budburst %f = tC/cell/day\n", reserve_for_budburst);

		CHECK_CONDITION(s->value[RESERVE_C], < 0.0);
		s->value[C_TO_LEAF] = reserve_for_foliage_budburst;
		s->value[C_TO_FINEROOT] = reserve_for_fine_root_budburst;
		s->value[C_TO_RESERVE] = npp_to_alloc - reserve_for_budburst;
		s->value[C_TO_COARSEROOT] = 0.0;
		s->value[C_TO_STEM] = 0.0;
		s->value[C_TO_BRANCH] = 0.0;
		s->value[C_TO_FRUIT] = 0.0;
		s->value[C_TO_LITTER] = 0.0;

		break;

	case 2:
		logger(g_log, "(LAI == PEAK LAI)\n");
		logger(g_log, "allocating into the three pools Ws(Ws+Wbb)+Wr(Wrc)+Wreserve\n");
		/*see Barbaroux et al., 2002, Scartazza et al., 2013*/

		if (npp_to_alloc > 0.0)
		{
			/* check if minimum reserve pool needs to be refilled */
			/* it doesn't need */
			if(s->value[RESERVE_C] >= s->value[MIN_RESERVE_C])
			{
				/* allocating into c pools */
				s->value[C_TO_RESERVE] = npp_to_alloc * pF_CTEM;
				s->value[C_TO_FINEROOT] = 0.0;
				s->value[C_TO_COARSEROOT] = s->value[NPP_tC] * pR_CTEM;
				s->value[C_TO_STEM] = (s->value[NPP_tC] * pS_CTEM) * (1.0 - s->value[FRACBB]);
				s->value[C_TO_BRANCH] = (s->value[NPP_tC] * pS_CTEM) * s->value[FRACBB];
				s->value[C_TO_LEAF] = 0.0;
				s->value[C_TO_FRUIT] = 0.0;
				s->value[C_TO_LITTER] = 0.0;
			}
			/* it needs */
			else if (s->value[RESERVE_C] > 0.0 && s->value[RESERVE_C] < s->value[MIN_RESERVE_C])
			{
				/* allocating into c pools */
				s->value[C_TO_RESERVE] = npp_to_alloc;
				s->value[C_TO_FINEROOT] = 0.0;
				s->value[C_TO_COARSEROOT] = 0.0;
				s->value[C_TO_TOT_STEM] = 0.0;
				s->value[C_TO_BRANCH] = 0.0;
				s->value[C_TO_LEAF] = 0.0;
				s->value[C_TO_FRUIT] = 0.0;
				s->value[C_TO_LITTER] = 0.0;
			}
			else
			{
				CHECK_CONDITION(s->value[RESERVE_C], < 0.0);
			}
		}
		else
		{
			s->value[C_TO_RESERVE] = npp_to_alloc;
			s->value[C_TO_FINEROOT] = 0.0;
			s->value[C_TO_COARSEROOT] = 0.0;
			s->value[C_TO_STEM] = 0.0;
			s->value[C_TO_BRANCH] = 0.0;
			s->value[C_TO_LEAF] = 0.0;
			s->value[C_TO_FRUIT] = 0.0;
			s->value[C_TO_LITTER] = 0.0;
		}
		CHECK_CONDITION(s->value[RESERVE_C], < 0.0);
		break;
		/**********************************************************************/
	case 3:
		logger(g_log, "(DayLength < MINDAYLENGTH)\n");
		logger(g_log, "LEAF FALL\n");
		logger(g_log, "allocating into W reserve pool\n");

		if (npp_to_alloc > 0.0)
		{
			//fixme do it also for 0.1
			/* reproduction only for needle leaf */
			if (s->value[PHENOLOGY] == 0.2)
			{
				s->value[C_TO_FRUIT] = npp_to_alloc * s->value[FRUIT_PERC];
				npp_to_alloc -= s->value[C_TO_FRUIT];
			}
		}

		Leaf_fall(c, height, age, species);
		/* note: these are computed in Leaf_fall function */
		//		s->value[C_TO_LEAF] = ;
		//		s->value[C_TO_FINEROOT] = ;
		//		s->value[C_TO_LITTER] = ;
		s->value[C_TO_COARSEROOT] = 0.0;
		s->value[C_TO_STEM] = 0.0;
		s->value[C_TO_BRANCH] = 0.0;
		s->value[C_TO_FRUIT] = 0.0;
		s->value[C_TO_RESERVE] = s->value[NPP_tC] + s->value[C_LEAF_TO_RESERVE] + s->value[C_FINEROOT_TO_RESERVE];


		break;
	case 0:

		logger(g_log, "Unvegetative period \n");
		s->value[C_TO_LEAF] = 0.0;
		s->value[C_TO_FINEROOT] = 0.0;
		s->value[C_TO_COARSEROOT] = 0.0;
		s->value[C_TO_STEM] = 0.0;
		s->value[C_TO_BRANCH] = 0.0;
		s->value[C_TO_FRUIT] = 0.0;
		s->value[C_TO_LITTER] = 0.0;
		s->value[C_TO_RESERVE] = npp_to_alloc;
		break;
	}

	//todo to be checked
	/* CHECK */
	/* sum all biomass pools increments */
	logger(g_log, "C_TO_LEAF = %g tC/cell/day\n", s->value[C_TO_LEAF]);
	logger(g_log, "C_TO_FINEROOT = %g tC/cell/day\n", s->value[C_TO_FINEROOT]);
	logger(g_log, "C_TO_COARSEROOT = %g tC/cell/day\n", s->value[C_TO_COARSEROOT]);
	logger(g_log, "C_TO_STEM = %g tC/cell/day\n", s->value[C_TO_STEM]);
	logger(g_log, "C_TO_RESERVE = %g tC/cell/day\n", s->value[C_TO_RESERVE]);
	logger(g_log, "C_TO_BRANCH = %g tC/cell/day\n", s->value[C_TO_BRANCH]);
	logger(g_log, "C_TO_FRUIT = %g tC/cell/day\n", s->value[C_TO_FRUIT]);
	npp_alloc = s->value[C_TO_RESERVE] +
			s->value[C_TO_FINEROOT] +
			s->value[C_TO_COARSEROOT] +
			s->value[C_TO_STEM] +
			s->value[C_TO_BRANCH] +
			s->value[C_TO_LEAF] +
			s->value[C_TO_FRUIT];
	//CHECK_CONDITION(fabs(npp_to_alloc - npp_alloc), >1e-4)

	/* update live_total wood fraction based on age */
	live_total_wood_age (a, species);

	/* allocate daily carbon */
	C_allocation(s);

	Average_tree_biomass(s);

	/* to avoid "jumps" of dbh it has computed only one monthly */
	if(day == 0)
	{
		Dendrometry(c, height, age, species, year);
	}

	logger(g_log, "\n-Daily increment in carbon pools-\n");
	logger(g_log, "C_TO_LEAF = %f tC/cell/day\n", s->value[C_TO_LEAF]);
	logger(g_log, "C_TO_FINEROOT = %f tC/cell/day\n", s->value[C_TO_FINEROOT]);
	logger(g_log, "C_TO_COARSEROOT = %f tC/cell/day\n", s->value[C_TO_COARSEROOT]);
	logger(g_log, "C_TO_STEM = %f tC/cell/day\n", s->value[C_TO_STEM]);
	logger(g_log, "C_TO_RESERVE = %f tC/cell/day\n", s->value[C_TO_RESERVE]);
	logger(g_log, "C_TO_BRANCH = %f tC/cell/day\n", s->value[C_TO_BRANCH]);
	logger(g_log, "C_TO_FRUIT = %f tC/cell/day\n", s->value[C_TO_FRUIT]);
	logger(g_log, "C_TO_LITTER = %f tC/cell/day\n", s->value[C_TO_LITTER]);

	/* update Leaf Area Index */
	Daily_lai(s);

	/* turnover */
	Turnover(s);

	/* update class level annual carbon biomass increment in tC/cell/year */
	s->value[DEL_Y_WTS] += s->value[C_TO_TOT_STEM];
	s->value[DEL_Y_WS] += s->value[C_TO_STEM];
	s->value[DEL_Y_WF] += s->value[C_TO_LEAF];
	s->value[DEL_Y_WFR] += s->value[C_TO_FINEROOT];
	s->value[DEL_Y_WCR] += s->value[C_TO_COARSEROOT];
	s->value[DEL_Y_WRES] += s->value[C_TO_RESERVE];
	s->value[DEL_Y_WR] += s->value[C_TO_ROOT];
	s->value[DEL_Y_BB] += s->value[C_TO_BRANCH];

	/* update cell level carbon biomass in gC/m2/day */
	c->daily_leaf_carbon += s->value[C_TO_LEAF] * 1000000.0 / g_settings->sizeCell ;
	c->daily_stem_carbon += s->value[C_TO_STEM] * 1000000.0 / g_settings->sizeCell ;
	c->daily_fine_root_carbon += s->value[C_TO_FINEROOT] * 1000000.0 / g_settings->sizeCell ;
	c->daily_coarse_root_carbon += s->value[C_TO_COARSEROOT] * 1000000.0 / g_settings->sizeCell ;
	c->daily_branch_carbon += s->value[C_TO_BRANCH] * 1000000.0 / g_settings->sizeCell ;
	c->daily_reserve_carbon += s->value[C_TO_RESERVE] * 1000000.0 / g_settings->sizeCell ;
	c->daily_root_carbon += s->value[C_TO_ROOT] * 1000000.0 / g_settings->sizeCell ;
	c->daily_litter_carbon += s->value[C_TO_LITTER] * 1000000.0 / g_settings->sizeCell ;

	/* update cell level carbon biomass in tC/cell/day */
	c->daily_leaf_carbon_tC += s->value[C_TO_LEAF];
	c->daily_stem_carbon_tC += s->value[C_TO_STEM];
	c->daily_fine_root_carbon_tC += s->value[C_TO_FINEROOT];
	c->daily_coarse_root_carbon_tC += s->value[C_TO_COARSEROOT];
	c->daily_branch_carbon_tC += s->value[C_TO_BRANCH];
	c->daily_reserve_carbon_tC += s->value[C_TO_RESERVE];
	c->daily_root_carbon_tC += s->value[C_TO_ROOT];
	c->daily_litter_carbon_tC += s->value[C_TO_LITTER];

	logger(g_log, "******************************\n");
}

//void simple_Deciduous_Partitioning_Allocation (species_t *const s, cell_t *const c, const meteo_t *const met, int day, int month, int years, int DaysInMonth, int height, int age, int species)
//{
//
//	int i;
//
//	//int phenology_phase;
//	//allocation parameter. their sum must be = 1
//
//	double  s0Ctem = s->value[S0CTEM];
//	double  r0Ctem = s->value[R0CTEM];
//	//double  f0Ctem = s->value[F0CTEM];
//	double const omegaCtem = s->value[OMEGA_CTEM];
//	double pS_CTEM = 0.0;
//	double pR_CTEM = 0.0;
//	double pF_CTEM = 0.0;
//	//double max_DM_foliage;
//	//double reductor;           //instead soil water the routine take into account the minimum between F_VPD and F_SW and F_NUTR
//
//	double oldW;
//	double Daily_solar_radiation;
//	//double Monthly_solar_radiation;
//	double Light_trasm;
//	double Par_over;
//	double Perc_fine;
//
//
//	static double frac_to_foliage_fineroot;
//
//	//static double biomass_tot_budburst;
//	static double biomass_foliage_budburst;
//	//static double biomass_fine_root_budburst;
//
//	//Marconi
//	double parameter; // parameter for exponential function to be used to gradually allocate biomass reserve during bud burst
//
//
//	i = c->heights[height].z;
//
//
//	logger(g_log, "\n**ALLOCATION_ROUTINE**\n\n");
//	logger(g_log, "Carbon allocation routine for deciduous\n");
//
//
//	Daily_solar_radiation = met[month].d[day].solar_rad * MOLPAR_MJ;
//
//	Par_over = c->par - s->value[APAR];
//	Light_trasm = Par_over /Daily_solar_radiation;
//
//	//if (day == 0 && month == 0)
//	//leaf_fall_counter = 0;
//
//
//	if (day == 0 && month == 0 && years == 0)
//	{
//		s->value[BIOMASS_FRUIT_tDM] = 0;
//	}
//	s->value[FR_CR] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * (1.0 / s->value[STEM_LEAF]);
//	logger(g_log, "Fine/Coarse root ratio = %f\n", s->value[FR_CR] );
//	Perc_fine = s->value[FR_CR] / (s->value[FR_CR] + 1.0);
//	logger(g_log, "Percentage of fine root against total root= %f %%\n", Perc_fine * 100 );
//
//
//	if (s->counter[VEG_DAYS] == 1)
//	{
//		s->counter[BUD_BURST_COUNTER] = s->value[BUD_BURST];
//		logger(g_log, "First day of budburst\n");
//		logger(g_log, "Days for bud burst = %f\n", s->value[BUD_BURST]);
//	}
//	if (s->counter[VEG_DAYS] > 1 && s->counter[VEG_DAYS] <= s->value[BUD_BURST])
//	{
//		s->counter[BUD_BURST_COUNTER] --;
//		logger(g_log, "++Remaining days for bud burst = %d\n", s->counter[BUD_BURST_COUNTER]);
//	}
//	if (s->counter[VEG_DAYS] > s->value[BUD_BURST])
//	{
//		s->counter[BUD_BURST_COUNTER] = 0;
//	}
//
//	//I could try to get in instead F_SW the minimum value between F_SW and F_VPD  2 apr 2012
//	//reductor = Minimum (s->value[F_SW], s->value[F_VPD]);
//	//I could try to get in instead F_SW the minimum value between F_SW and F_NUTR  18 apr 2012
//	//reductor = Minimum (s->value[F_SW], s->value[F_NUTR]);
//	//reductor = s->value[F_SW];
//
//	//todo use it if a better function of fSW is developed
//	/*
//		if (reductor == s->value[F_SW])
//		{
//			logger(g_log, "reductor in CTEM is F_SW \n");
//		}
//		else
//		{
//			logger(g_log, "reductor in CTEM is F_NUTR \n");
//		}
//	 */
//
//	logger(g_log, "BIOMASS PARTITIONING-ALLOCATION FOR LAYER %d\n", c->heights[height].z);
//	if (g_settings->spatial == 'u')
//	{
//		oldW = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_BRANCH_tDM];
//
//		//(Arora V. K., Boer G. J., GCB, 2005)
//
//		if (s->management == 0)
//		{
//			//logger(g_log, "Management type = TIMBER\n");
//		}
//		else
//		{
//			//logger(g_log, "Management type = COPPICE\n");
//
//			//recompute allocation parameter for coppice
//
//			//TODO: PUT INTO INPUT.TXT
//			//OR CONSIDER YEARS_FROM_COPPICING AS THE AGE SETTED INTO INPUT.TXT
//			//double years_from_coppicing = 25;
//
//			double r0Ctem_increment;
//			double old_r0Ctem = r0Ctem;
//			double s0Ctem_increment;
//			double old_s0Ctem = s0Ctem;
//
//
//
//			logger(g_log, "min r0 ctem = %f\n",s->value[MIN_R0CTEM] );
//			logger(g_log, "max s0 ctem = %f\n",s->value[MAX_S0CTEM] );
//			logger(g_log, "years for conversion = %f\n",s->value[YEARS_FOR_CONVERSION] );
//
//
//			//considering a LINEAR increment
//			//allocation ratio to roots
//			r0Ctem -= s->value[MIN_R0CTEM];
//			r0Ctem_increment = r0Ctem / s->value[YEARS_FOR_CONVERSION];
//			r0Ctem = s->value[MIN_R0CTEM] + (r0Ctem_increment * s->value[F_AGE]);
//			logger(g_log, "new r0_CTEM = %f \n", r0Ctem);
//
//			if (r0Ctem > old_r0Ctem || r0Ctem < s->value[MIN_R0CTEM])
//			{
//				logger(g_log, "ERROR IN r0Ctem !!! \n");
//			}
//			//considering a LINEAR decrement
//			//allocation ratio to stem + bb
//			s0Ctem = s->value[MAX_S0CTEM] - s0Ctem;
//			s0Ctem_increment = s0Ctem / s->value[YEARS_FOR_CONVERSION];
//			s0Ctem = s->value[MAX_S0CTEM] - (s0Ctem_increment * s->value[F_AGE]);
//			logger(g_log, "new s0_CTEM = %f \n", s0Ctem);
//
//			if (s0Ctem > s->value[MAX_S0CTEM] || s0Ctem < old_s0Ctem)
//			{
//				logger(g_log, "ERROR IN s0Ctem !!! \n");
//			}
//		}
//		logger(g_log, "PHENOLOGICAL PHASE = %d\n", s->phenology_phase);
//		logger(g_log, "LAI = %f \n", s->value[LAI]);
//		logger(g_log, "PEAK LAI = %f \n", s->value[PEAK_LAI]);
//
//
//		switch (s->phenology_phase)
//		{
//		/************************************************************/
//		case 1:
//			logger(g_log, "BUDBURST\n");
//			logger(g_log, "Bud burst phase using both reserve pools and npp\n");
//			logger(g_log, "Allocating only into foliage\n");
//			logger(g_log, "LAI = %f \n", s->value[LAI]);
//
//			s->value[DAILY_DEL_LITTER] = 0;
//
//			/*following Campioli et al., 2008, Maillard et al., 1994, Barbaroux et al., 2003*/
//			CHECK_CONDITION(s->value[RESERVE_tDM], < 0.0)
//
//			//test check it it seem that doesn't work!!
//			//frac_to_foliage_fineroot = (s->value[RESERVE]) / s->counter[BUD_BURST_COUNTER];
//			//parameter = 2.0 / pow(s->value[BUD_BURST],2.0);
//			//frac_to_foliage_fineroot = (s->value[RESERVE]) * parameter * (s->value[BUD_BURST]+1.0 - s->counter[BUD_BURST_COUNTER]);
//			logger(g_log, "Tot biomass reserve = %f\n", s->value[RESERVE_tDM]);
//			//logger(g_log, "fraction of reserve for foliage and fine root = %f\n", frac_to_foliage_fineroot);
//			logger(g_log, "++Remaining days for bud burst = %d\n", s->counter[BUD_BURST_COUNTER]);
//
//			//			if (s->value[MAX_BIOMASS_BUDBURST_tDM] > s->value[RESERVE_tDM])
//			//			{
//			//				s->value[MAX_BIOMASS_BUDBURST_tDM] = s->value[RESERVE_tDM];
//			//				s->value[RESERVE_tDM] = 0.0;
//			//			}
//			//			else
//			//			{
//			/* +2 is to avoid excees in biomass for foliage */
//			biomass_foliage_budburst = s->value[MAX_BIOMASS_FOLIAGE_tDM] / (s->value[BUD_BURST]+2.0);
//			logger(g_log, "daily amount of biomass for foliage budburst %f\n", biomass_foliage_budburst);
//			//			}
//
//			if (s->value[NPP_tDM] > 0.0)
//			{
//				//if(s->value[NPP] > biomass_tot_budburst)
//				if(s->value[NPP_tDM] > biomass_foliage_budburst)
//				{
//					//fixme where the exceeding NPP goes?
//					s->value[DEL_FOLIAGE] = biomass_foliage_budburst;
//					s->value[DEL_RESERVE] = -(s->value[NPP_tDM] - biomass_foliage_budburst);
//				}
//				else
//				{
//					s->value[DEL_FOLIAGE] = biomass_foliage_budburst;
//					s->value[DEL_RESERVE] = -(biomass_foliage_budburst - s->value[NPP_tDM]);
//				}
//				s->value[DEL_ROOTS_FINE] = 0.0;
//				s->value[DEL_STEMS] = 0.0;
//				s->value[DEL_ROOTS_COARSE] = 0.0;
//				s->value[DEL_ROOTS_TOT] = 0.0;
//				s->value[DEL_TOT_STEM] = 0.0;
//				s->value[DEL_BB] = 0.0;
//			}
//			else
//			{
//				CHECK_CONDITION(s->value[RESERVE_tDM], < 0.0);
//
//				logger(g_log, "Using ONLY reserve\n");
//				s->value[DEL_FOLIAGE] = biomass_foliage_budburst;
//				s->value[DEL_ROOTS_FINE] = 0.0;
//				//fixme remove CANOPY_COVER??
//				s->value[DEL_RESERVE] = - (((fabs(s->value[C_FLUX]) * GC_GDM)/1000000.0) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell) +	biomass_foliage_budburst);
//				s->value[DEL_STEMS] = 0.0;
//				s->value[DEL_ROOTS_COARSE] = 0.0;
//				s->value[DEL_ROOTS_TOT] = 0.0;
//				s->value[DEL_TOT_STEM] = 0.0;
//				s->value[DEL_BB]= 0.0;
//			}
//
//			// Total Biomass
//			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] + s->value[BIOMASS_ROOTS_TOT_tDM] + s->value[RESERVE_tDM] + s->value[BIOMASS_BRANCH_tDM];
//			logger(g_log, "Previous Total W = %f tDM/area\n", oldW);
//			logger(g_log, "Total Biomass = %f tDM/area\n", s->value[TOTAL_W]);
//
//			c->daily_leaf_drymatter += s->value[DEL_FOLIAGE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_stem_drymatter += s->value[DEL_STEMS]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_fine_root_drymatter += s->value[DEL_ROOTS_FINE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_coarse_root_drymatter += s->value[DEL_ROOTS_COARSE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_branch_drymatter += s->value[DEL_BB]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_reserve_drymatter += s->value[DEL_RESERVE] / GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//
//			break;
//		case 2:
//			logger(g_log, "(LAI == PEAK LAI)\n");
//			logger(g_log, "allocating into the three pools Ws+Wr(Wrc+Wrf)+Wreserve\n");
//			/*see Barbaroux et al., 2002, Scartazza et al., 2013*/
//
//			s->value[DAILY_DEL_LITTER] = 0;
//
//			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * (2.0 - Light_trasm - s->value[F_SW])));
//			logger(g_log, "Roots CTEM ratio layer = %f %%\n", pR_CTEM * 100);
//			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * (2.0 - Light_trasm - s->value[F_SW])));
//			logger(g_log, "Stem CTEM ratio = %f %%\n", pS_CTEM * 100);
//			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
//			logger(g_log, "Reserve CTEM ratio = %f %%\n", pF_CTEM * 100);
//
//			/*partitioning*/
//			if (s->value[NPP_tDM] > 0.0)
//			{
//				//fixme do it also for 0.1
//				//REPRODUCTION ONLY FOR NEEDLE LEAF
//				if (s->value[PHENOLOGY] == 0.2)
//				{
//					//NPP for reproduction
//					s->value[BIOMASS_FRUIT_tDM] = s->value[NPP_tDM] * s->value[FRUIT_PERC];
//					s->value[NPP_tDM] -= s->value[BIOMASS_FRUIT_tDM];
//					logger(g_log, "Biomass increment into cones = %f tDM/area\n", s->value[BIOMASS_FRUIT_tDM]);
//
//					//reproductive life span
//					s->value[BIOMASS_FRUIT_tDM] -= (s->value[BIOMASS_FRUIT_tDM] * (1 / s->value[CONES_LIFE_SPAN]));
//				}
//
//				s->value[DEL_RESERVE] = s->value[NPP_tDM] * pF_CTEM;
//				s->value[DEL_ROOTS_TOT] = s->value[NPP_tDM] * pR_CTEM;
//				s->value[DEL_ROOTS_FINE] = s->value[DEL_ROOTS_TOT] * Perc_fine;
//				s->value[DEL_ROOTS_COARSE] = s->value[DEL_ROOTS_TOT] - s->value[DEL_ROOTS_FINE];
//				s->value[DEL_TOT_STEM] = s->value[NPP_tDM] * pS_CTEM;
//				s->value[DEL_STEMS] = (s->value[NPP_tDM] * pS_CTEM) * (1.0 - s->value[FRACBB]);
//				s->value[DEL_BB] = (s->value[NPP_tDM] * pS_CTEM) * s->value[FRACBB];
//				s->value[DEL_FOLIAGE] = 0;
//			}
//			else
//			{
//				s->value[DEL_FOLIAGE] = 0.0;
//				s->value[DEL_ROOTS_FINE] = 0.0;
//				s->value[DEL_RESERVE] = -((fabs(s->value[C_FLUX]) * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//				s->value[DEL_ROOTS_COARSE] = 0.0;
//				s->value[DEL_ROOTS_TOT] = 0.0;
//				s->value[DEL_TOT_STEM] = 0.0;
//				s->value[DEL_STEMS]= 0.0;
//				s->value[DEL_BB]= 0.0;
//				CHECK_CONDITION(s->value[RESERVE_tDM], < 0.0);
//			}
//
//			c->daily_leaf_drymatter += s->value[DEL_FOLIAGE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_stem_drymatter += s->value[DEL_STEMS]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_fine_root_drymatter += s->value[DEL_ROOTS_FINE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_coarse_root_drymatter += s->value[DEL_ROOTS_COARSE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_branch_drymatter += s->value[DEL_BB]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_reserve_drymatter += s->value[DEL_RESERVE] / GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//
//			break;
//			/**********************************************************************/
//		case 3:
//			logger(g_log, "(DayLength < MINDAYLENGTH)\n");
//			logger(g_log, "LEAF FALL\n");
//			logger(g_log, "allocating into W reserve pool\n");
//			logger(g_log, "++Lai before Leaffall= %f\n", s->value[LAI]);
//			logger(g_log, "Biomass foliage = %f \n", s->value[BIOMASS_FOLIAGE_tDM]);
//			logger(g_log, "Biomass fine root = %f \n", s->value[BIOMASS_FINE_ROOT_tDM]);
//			logger(g_log, "foliage reduction rate %f \n", s->value[FOLIAGE_REDUCTION_RATE]);
//			//logger(g_log, "biomass foliage to remove %f \n", s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE]);
//
//			if(s->counter[LEAF_FALL_COUNTER] == 1)
//			{
//				logger(g_log, "First day of Leaf fall\n");
//				//s->counter[SENESCENCE_DAYONE] = c->doy;
//				//s->counter[DAY_FRAC_FOLIAGE_REMOVE] =  endOfYellowing(met, &c->heights[height].ages[age].species[species]) - c->heights[height].ages[age].species[species].counter[SENESCENCE_DAYONE];
//				logger(g_log, "DAYS FOR FOLIAGE and FINE ROOT for_REMOVING = %d\n", s->counter[DAY_FRAC_FOLIAGE_REMOVE]);
//				//Marconi: assumed that fine roots for deciduos species progressively die togheter with leaves
//
//				/* assuming linear leaf fall */
//				s->value[DEL_FOLIAGE] = -(s->value[BIOMASS_FOLIAGE_tDM] / s->counter[DAY_FRAC_FOLIAGE_REMOVE]);
//				logger(g_log, "daily amount of foliage to remove = %f\n", s->value[DEL_FOLIAGE]);
//				s->value[DEL_ROOTS_FINE]= -(s->value[BIOMASS_FINE_ROOT_tDM] / s->counter[DAY_FRAC_FOLIAGE_REMOVE]);
//				logger(g_log, "daily amount of fine root to remove = %f\n", s->value[DEL_ROOTS_FINE]);
//
//				/* following Campioli et al., 2013 and Bossel 1996 10% of foliage and fine root biomass is retranslocated as reserve in the reserve pool */
//				/* compute amount of fine root biomass to retranslocate as reserve */
//				s->value[RESERVE_FOLIAGE_TO_RETRANSL_tDM] = (s->value[BIOMASS_FOLIAGE_tDM] *0.1) / (int)s->counter[DAY_FRAC_FOLIAGE_REMOVE];
//				s->value[RESERVE_FINEROOT_TO_RETRANSL_tDM] = (s->value[BIOMASS_FINE_ROOT_tDM] *0.1) / (int)s->counter[DAY_FRAC_FOLIAGE_REMOVE];
//				logger(g_log, "RESERVE_FOLIAGE_TO_RETRANSL = %f\n", s->value[RESERVE_FOLIAGE_TO_RETRANSL_tDM]);
//				logger(g_log, "RESERVE_FINEROOT_TO_RETRANSL = %f\n", s->value[RESERVE_FINEROOT_TO_RETRANSL_tDM]);
//
//			}
//
//			if (s->value[NPP_tDM] > 0.0)
//			{
//				//REPRODUCTION ONLY FOR NEEDLE LEAF
//				if (s->value[PHENOLOGY] == 0.2)
//				{
//					//NPP for reproduction
//					s->value[BIOMASS_FRUIT_tDM] = s->value[NPP_tDM] * s->value[FRUIT_PERC];
//					s->value[NPP_tDM] -= s->value[BIOMASS_FRUIT_tDM];
//					logger(g_log, "Biomass increment into cones = %f tDM/area\n", s->value[BIOMASS_FRUIT_tDM]);
//
//					//reproductive life span
//					s->value[BIOMASS_FRUIT_tDM] -= (s->value[BIOMASS_FRUIT_tDM] * (1 / s->value[CONES_LIFE_SPAN]));
//				}
//				s->value[DAILY_DEL_LITTER] = - s->value[DEL_FOLIAGE];
//				//fixme do it with the total amount of foliage biomass
//				s->value[DEL_RESERVE] = s->value[NPP_tDM] + s->value[RESERVE_FOLIAGE_TO_RETRANSL_tDM] + s->value[RESERVE_FINEROOT_TO_RETRANSL_tDM];
//				s->value[DEL_TOT_STEM] = 0;
//				s->value[DEL_STEMS] = 0;
//				s->value[DEL_ROOTS_COARSE] = 0.0;
//				s->value[DEL_ROOTS_TOT] = 0;
//				s->value[DEL_BB] = 0;
//			}
//			else
//			{
//				s->value[DEL_TOT_STEM] = 0;
//				s->value[DEL_STEMS] = 0;
//				s->value[DEL_ROOTS_COARSE] = 0;
//				s->value[DEL_ROOTS_TOT] = 0;
//				s->value[DEL_BB] = 0;
//				s->value[DEL_RESERVE] = -((fabs(s->value[C_FLUX]) * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell) +
//						(s->value[RESERVE_FOLIAGE_TO_RETRANSL_tDM] + s->value[RESERVE_FINEROOT_TO_RETRANSL_tDM]);
//			}
//
//
//			c->daily_leaf_drymatter += 0.0/* s->value[DEL_FOLIAGE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell)*/;
//			c->daily_stem_drymatter += s->value[DEL_STEMS]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_fine_root_drymatter += 0.0/*s->value[DEL_ROOTS_FINE_CTEM]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell)*/;
//			c->daily_coarse_root_drymatter += s->value[DEL_ROOTS_COARSE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_branch_drymatter += s->value[DEL_BB]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_reserve_drymatter += s->value[DEL_RESERVE] / GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->leafLittering += fabs(s->value[DEL_FOLIAGE]) / GC_GDM * 1000 / g_settings->sizeCell;
//			c->leaflitN = c->leafLittering /GC_GDM * 1000 / g_settings->sizeCell /s->value[CN_DEAD_WOODS];
//			c->fineRootLittering +=  s->value[DAILY_FINEROOT_BIOMASS_TO_REMOVE] / GC_GDM * 1000 / g_settings->sizeCell;
//			c->fineRootlitN += s->value[BIOMASS_FINE_ROOT_tDM] / GC_GDM * 1000 / g_settings->sizeCell  / s->value[CN_FINE_ROOTS];
//
//			break;
//
//		case 0:
//
//			logger(g_log, "Unvegetative period \n");
//
//			s->value[DAILY_DEL_LITTER] = 0;
//
//			/*partitioning*/
//			s->value[DEL_RESERVE] = -((s->value[TOTAL_AUT_RESP] * GC_GDM)/1000000.0) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//
//			//SERGIOM
//			//using the sigmoid it is necessary to leave the remainder at the very begining of the phase 0; that
//			//because sigmoid decreases asymptotically to 0
//			if (s->value[BIOMASS_FOLIAGE_tDM] > 0)
//			{
//				s->value[DEL_FOLIAGE] = -s->value[BIOMASS_FOLIAGE_tDM];
//			}
//			else
//			{
//				s->value[DEL_FOLIAGE] = 0;
//			}
//			if (s->value[BIOMASS_FINE_ROOT_tDM] > 0)
//			{
//				s->value[BIOMASS_FINE_ROOT_tDM] = -s->value[BIOMASS_FINE_ROOT_tDM];
//			}
//			s->value[DEL_ROOTS_FINE] = 0;
//			s->value[DEL_ROOTS_COARSE] = 0;
//			s->value[DEL_ROOTS_TOT] = 0;
//			s->value[DEL_TOT_STEM] = 0;
//			s->value[DEL_STEMS]= 0;
//			s->value[DEL_BB]= 0;
//
//			c->leafLittering += fabs(s->value[DEL_FOLIAGE]) / GC_GDM * 1000 / g_settings->sizeCell;
//			c->leaflitN = c->leafLittering /GC_GDM * 1000 / g_settings->sizeCell /s->value[CN_DEAD_WOODS];
//			c->daily_leaf_drymatter += 0.0;
//			c->daily_stem_drymatter += s->value[DEL_STEMS]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_fine_root_drymatter += 0.0;
//			c->daily_coarse_root_drymatter += s->value[DEL_ROOTS_COARSE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_branch_drymatter += s->value[DEL_BB]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_reserve_drymatter += s->value[DEL_RESERVE] / GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//
//
//
//			break;
//		}
//	}
//
//	/*allocation*/
//	s->value[BIOMASS_FOLIAGE_tDM] += s->value[DEL_FOLIAGE];
//	logger(g_log, "Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_FOLIAGE_tDM]);
//	s->value[BIOMASS_STEM_tDM] += s->value[DEL_STEMS];
//	logger(g_log, "Branch and Bark Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_STEM_tDM]);
//	s->value[BIOMASS_BRANCH_tDM] += s->value[DEL_BB];
//	logger(g_log, "Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH_tDM]);
//	s->value[RESERVE_tDM] +=  s->value[DEL_RESERVE];
//	logger(g_log, "Reserve Biomass (Wres) = %f tDM/area\n", s->value[RESERVE_tDM]);
//	s->value[BIOMASS_ROOTS_TOT_tDM] +=  s->value[DEL_ROOTS_TOT];
//	logger(g_log, "Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
//	s->value[BIOMASS_FINE_ROOT_tDM] += s->value[DEL_ROOTS_FINE];
//	logger(g_log, "Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_FINE_ROOT_tDM]);
//	s->value[BIOMASS_COARSE_ROOT_tDM] += s->value[DEL_ROOTS_COARSE];
//	logger(g_log, "Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_tDM]);
//	s->value[BIOMASS_TOT_STEM_tDM] += s->value[DEL_TOT_STEM];
//	logger(g_log, "Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM_tDM]);
//	s->value[BIOMASS_STEM_LIVE_WOOD_tDM] += (s->value[DEL_STEMS] * s->value[LIVE_TOTAL_WOOD_FRAC]);
//	logger(g_log, "Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM]);
//	s->value[BIOMASS_STEM_DEAD_WOOD_tDM] += s->value[DEL_STEMS]*(1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]);
//	logger(g_log, "Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);
//	s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] += (s->value[DEL_ROOTS_COARSE] * s->value[LIVE_TOTAL_WOOD_FRAC]);
//	logger(g_log, "Live Coarse Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
//	s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM] += s->value[DEL_ROOTS_COARSE] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]);
//	logger(g_log, "Dead Coarse Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
//	s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] += (s->value[DEL_BB] * s->value[LIVE_TOTAL_WOOD_FRAC]);
//	logger(g_log, "Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
//	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM] += (s->value[DEL_BB] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]));
//	logger(g_log, "Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);
//
//	if (fabs((s->value[BIOMASS_STEM_LIVE_WOOD_tDM] + s->value[BIOMASS_STEM_DEAD_WOOD_tDM]) - s->value[BIOMASS_STEM_tDM])>1e-4)
//	{
//		logger(g_log, "Live stem + dead stem = %f\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM] + s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);
//		logger(g_log, "Total stem = %f\n", s->value[BIOMASS_STEM_tDM]);
//		exit(1);
//	}
//	if (fabs((s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] + s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]) - s->value[BIOMASS_COARSE_ROOT_tDM])>1e-4)
//	{
//		logger(g_log, "Live coarse + dead coarse = %f\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] + s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
//		logger(g_log, "Total coarse = %f\n", s->value[BIOMASS_COARSE_ROOT_tDM]);
//		exit(1);
//	}
//	if (fabs((s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] + s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]) - s->value[BIOMASS_BRANCH_tDM])>1e-4)
//	{
//		logger(g_log, "Live branch + dead branch = %f\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] + s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);
//		logger(g_log, "Total branch = %f\n", s->value[BIOMASS_BRANCH_tDM]);
//		exit(1);
//	}
//
//
//	Daily_lai (&c->heights[height].ages[age].species[species]);
//
//
//	//fixme compute from unspatial
//	s->value[DEL_Y_WTS] += s->value[DEL_TOT_STEM];
//	s->value[DEL_Y_WS] += s->value[DEL_STEMS];
//	s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE];
//	s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE];
//	s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE];
//	s->value[DEL_Y_WRES] += s->value[DEL_RESERVE];
//	s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT];
//	s->value[DEL_Y_BB] += s->value[DEL_BB];
//
//	logger(g_log, "delta_WTS %d = %f \n", c->heights[height].z, s->value[DEL_TOT_STEM]);
//	logger(g_log, "delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE]);
//	logger(g_log, "delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE]);
//	logger(g_log, "delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE]);
//	logger(g_log, "delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS]);
//	logger(g_log, "delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE]);
//	logger(g_log, "delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);
//
//	c->daily_delta_wts[i] = s->value[DEL_TOT_STEM];
//	c->daily_delta_ws[i] = s->value[DEL_STEMS];
//	c->daily_delta_wf[i] = s->value[DEL_FOLIAGE];
//	c->daily_delta_wbb[i] = s->value[DEL_BB];
//	c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE];
//	c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE];
//	c->daily_delta_wres[i] = s->value[DEL_RESERVE];
//
//
//	//	c->daily_lai[i] = s->value[LAI];
//	//	c->annual_delta_ws[i] += s->value[DEL_STEMS];
//	//	c->annual_ws[i] = s->value[BIOMASS_STEM_tDM];
//	//	c->annual_delta_wres[i] += s->value[DEL_RESERVE];
//	//	c->annual_wres[i] = s->value[RESERVE_tDM];
//	//	c->annual_wf[i]= s->value[BIOMASS_FOLIAGE_tDM];
//	//	c->annual_wbb[i]= s->value[BIOMASS_BRANCH_tDM];
//	//	c->annual_wfr[i]= s->value[BIOMASS_FINE_ROOT_tDM];
//	//	c->annual_wcr[i]= s->value[BIOMASS_COARSE_ROOT_tDM];
//
//	if (c->leafLittering < 0.0) c->leafLittering = 0;
//
//	logger(g_log, "******************************\n");
//}



////VERSION CURRENTLY not USED
////Deciduous carbon allocation routine
//void Deciduous_Partitioning_Allocation (species_t *const s, cell_t *const c, const meteo_t *const met, int day, int month, int years, int DaysInMonth, int height, int age, int species)
//{
//
//	int i;
//
//	//int phenology_phase;
//	//allocation parameter. their sum must be = 1
//
//	double  s0Ctem = s->value[S0CTEM];
//	double  r0Ctem = s->value[R0CTEM];
//	//double  f0Ctem = s->value[F0CTEM];
//	double const omegaCtem = s->value[OMEGA_CTEM];
//	double pS_CTEM = 0.0;
//	double pR_CTEM = 0.0;
//	double pF_CTEM = 0.0;
//	//double max_DM_foliage;
//	//double reductor;           //instead soil water the routine take into account the minimum between F_VPD and F_SW and F_NUTR
//
//	double oldW;
//	double Daily_solar_radiation;
//	//double Monthly_solar_radiation;
//	double Light_trasm;
//	double Par_over;
//	double Perc_fine;
//	double Perc_coarse;
//	//double Perc_leaves;              //percentage of leaves in first growing season
//
//	static double frac_to_foliage_fineroot;
//
//	//static double biomass_tot_budburst;
//	static double biomass_foliage_budburst;
//	//static double biomass_fine_root_budburst;
//
//	//Marconi
//	double parameter; // parameter for exponential function to be used to gradually allocate biomass reserve during bud burst
//
//
//	i = c->heights[height].z;
//
//	//CTEM VERSION
//
//	logger(g_log, "\n**ALLOCATION_ROUTINE**\n\n");
//	logger(g_log, "Carbon allocation routine for deciduous\n");
//
//
//	Daily_solar_radiation = met[month].d[day].solar_rad * MOLPAR_MJ;
//
//	Par_over = c->par - s->value[APAR];
//	Light_trasm = Par_over /Daily_solar_radiation;
//
//	//if (day == 0 && month == 0)
//	//leaf_fall_counter = 0;
//
//
//	if (day == 0 && month == 0 && years == 0)
//	{
//		s->value[BIOMASS_FRUIT_tDM] = 0.0;
//	}
//
//	//7 May 2012
//	//compute static ratio of allocation between fine and coarse root
//	//deriving data from values reported for BIOME-BGC
//	//fixme see if change with new parameters checked in "Pool_fraction"
//	s->value[FR_CR] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * (1.0 / s->value[STEM_LEAF]);
//	//logger(g_log, "Fine/Coarse root ratio = %f\n", s->value[FR_CR] );
//	Perc_fine = s->value[FR_CR] / (s->value[FR_CR] + 1.0);
//	//logger(g_log, "Percentage of fine root against total root= %f %%\n", Perc_fine * 100 );
//	Perc_coarse = 1- Perc_fine;
//
//	if (s->counter[VEG_DAYS] == 1)
//	{
//		s->counter[BUD_BURST_COUNTER] = s->value[BUD_BURST];
//		logger(g_log, "First day of budburst\n");
//		logger(g_log, "Days for bud burst = %f\n", s->value[BUD_BURST]);
//	}
//	if (s->counter[VEG_DAYS] > 1 && s->counter[VEG_DAYS] <= s->value[BUD_BURST])
//	{
//		s->counter[BUD_BURST_COUNTER] --;
//		logger(g_log, "++Remaining days for bud burst = %d\n", s->counter[BUD_BURST_COUNTER]);
//	}
//	if (s->counter[VEG_DAYS] > s->value[BUD_BURST])
//	{
//		s->counter[BUD_BURST_COUNTER] = 0;
//	}
//
//	//I could try to get in instead F_SW the minimum value between F_SW and F_VPD  2 apr 2012
//	//reductor = Minimum (s->value[F_SW], s->value[F_VPD]);
//	//I could try to get in instead F_SW the minimum value between F_SW and F_NUTR  18 apr 2012
//	//reductor = Minimum (s->value[F_SW], s->value[F_NUTR]);
//	//reductor = s->value[F_SW];
//
//	//todo use it if a better function of fSW is developed
//	/*
//		if (reductor == s->value[F_SW])
//		{
//			logger(g_log, "reductor in CTEM is F_SW \n");
//		}
//		else
//		{
//			logger(g_log, "reductor in CTEM is F_NUTR \n");
//		}
//	 */
//
//	logger(g_log, "BIOMASS PARTITIONING-ALLOCATION FOR LAYER %d\n", c->heights[height].z);
//	if (g_settings->spatial == 'u')
//	{
//		oldW = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] + s->value[BIOMASS_COARSE_ROOT_tDM] + s->value[BIOMASS_FINE_ROOT_tDM] + s->value[BIOMASS_BRANCH_tDM];
//
//		//(Arora V. K., Boer G. J., GCB, 2005)
//
//		if (s->management == 0)
//		{
//			//logger(g_log, "Management type = TIMBER\n");
//		}
//		else
//		{
//			//logger(g_log, "Management type = COPPICE\n");
//
//			//recompute allocation parameter for coppice
//
//			//TODO: PUT INTO INPUT.TXT
//			//OR CONSIDER YEARS_FROM_COPPICING AS THE AGE SETTED INTO INPUT.TXT
//			//double years_from_coppicing = 25;
//
//			double r0Ctem_increment;
//			double old_r0Ctem = r0Ctem;
//			double s0Ctem_increment;
//			double old_s0Ctem = s0Ctem;
//
//
//
//			logger(g_log, "min r0 ctem = %f\n",s->value[MIN_R0CTEM] );
//			logger(g_log, "max s0 ctem = %f\n",s->value[MAX_S0CTEM] );
//			logger(g_log, "years for conversion = %f\n",s->value[YEARS_FOR_CONVERSION] );
//
//
//			//considering a LINEAR increment
//			//allocation ratio to roots
//			r0Ctem -= s->value[MIN_R0CTEM];
//			r0Ctem_increment = r0Ctem / s->value[YEARS_FOR_CONVERSION];
//			r0Ctem = s->value[MIN_R0CTEM] + (r0Ctem_increment * s->value[F_AGE]);
//			logger(g_log, "new r0_CTEM = %f \n", r0Ctem);
//
//			if (r0Ctem > old_r0Ctem || r0Ctem < s->value[MIN_R0CTEM])
//			{
//				logger(g_log, "ERROR IN r0Ctem !!! \n");
//			}
//			//considering a LINEAR decrement
//			//allocation ratio to stem + bb
//			s0Ctem = s->value[MAX_S0CTEM] - s0Ctem;
//			s0Ctem_increment = s0Ctem / s->value[YEARS_FOR_CONVERSION];
//			s0Ctem = s->value[MAX_S0CTEM] - (s0Ctem_increment * s->value[F_AGE]);
//			logger(g_log, "new s0_CTEM = %f \n", s0Ctem);
//
//			if (s0Ctem > s->value[MAX_S0CTEM] || s0Ctem < old_s0Ctem)
//			{
//				logger(g_log, "ERROR IN s0Ctem !!! \n");
//			}
//		}
//		logger(g_log, "PHENOLOGICAL PHASE = %d\n", s->phenology_phase);
//		logger(g_log, "LAI = %f \n", s->value[LAI]);
//		logger(g_log, "PEAK LAI = %f \n", s->value[PEAK_LAI]);
//
//
//		switch (s->phenology_phase)
//		{
//		/************************************************************/
//		case 1:
//			logger(g_log, "BUDBURST\n");
//			logger(g_log, "Bud burst phase using both reserve pools and npp\n");
//			logger(g_log, "Allocating only into foliage\n");
//			logger(g_log, "LAI = %f \n", s->value[LAI]);
//
//			s->value[DAILY_DEL_LITTER] = 0;
//
//			/*following Campioli et al., 2008, Maillard et al., 1994, Barbaroux et al., 2003*/
//			CHECK_CONDITION(s->value[RESERVE_tDM], < 0.0)
//
//			//test check it it seem that doesn't work!!
//			//frac_to_foliage_fineroot = (s->value[RESERVE]) / s->counter[BUD_BURST_COUNTER];
//			parameter = 2.0 / pow(s->value[BUD_BURST],2.0);
//			frac_to_foliage_fineroot = (s->value[RESERVE_tDM]) * parameter * (s->value[BUD_BURST]+1.0 - s->counter[BUD_BURST_COUNTER]);
//			logger(g_log, "Tot biomass reserve = %f\n", s->value[RESERVE_tDM]);
//			logger(g_log, "fraction of reserve for foliage and fine root = %f\n", frac_to_foliage_fineroot);
//			logger(g_log, "++Remaining days for bud burst = %d\n", s->counter[BUD_BURST_COUNTER]);
//
//			//			if (s->value[MAX_BIOMASS_BUDBURST_tDM] > s->value[RESERVE_tDM])
//			//			{
//			//				s->value[MAX_BIOMASS_BUDBURST_tDM] = s->value[RESERVE_tDM];
//			//				s->value[RESERVE_tDM] = 0.0;
//			//			}
//			//			else
//			//			{
//			//				biomass_tot_budburst = s->value[MAX_BIOMASS_BUDBURST] / s->value[BUD_BURST];
//			//				logger(g_log, "daily amount of biomass for total budburst %f\n", biomass_tot_budburst);
//			biomass_foliage_budburst = s->value[MAX_BIOMASS_FOLIAGE_tDM] / s->value[BUD_BURST];
//			logger(g_log, "daily amount of biomass for foliage budburst %f\n", biomass_foliage_budburst);
//			//				biomass_fine_root_budburst = s->value[MAX_BIOMASS_FINE_ROOTS] / s->value[BUD_BURST];
//			//				logger(g_log, "daily amount of biomass for fine root budburst %f\n", biomass_fine_root_budburst);
//
//			//			}
//
//			//test this part have to be the new one
//
//			if (s->value[NPP_tDM] > 0.0)
//			{
//				//if(s->value[NPP] > biomass_tot_budburst)
//				if(s->value[NPP_tDM] > biomass_foliage_budburst)
//				{
//					//fixme where the exceeding NPP goes?
//					s->value[DEL_FOLIAGE] = biomass_foliage_budburst;
//					s->value[DEL_RESERVE] = -(s->value[NPP_tDM] - biomass_foliage_budburst);
//					//s->value[DEL_ROOTS_FINE_CTEM] = biomass_fine_root_budburst;
//				}
//				else
//				{
//					s->value[DEL_FOLIAGE] = biomass_foliage_budburst;
//					s->value[DEL_RESERVE] = -(biomass_foliage_budburst - s->value[NPP_tDM]);
//					//s->value[DEL_ROOTS_FINE_CTEM] = biomass_fine_root_budburst;
//				}
//				s->value[DEL_ROOTS_FINE] = 0.0;
//				s->value[DEL_STEMS] = 0.0;
//				s->value[DEL_ROOTS_COARSE] = 0.0;
//				s->value[DEL_ROOTS_TOT] = 0.0;
//				s->value[DEL_TOT_STEM] = 0.0;
//				s->value[DEL_BB] = 0.0;
//			}
//			else
//			{
//				CHECK_CONDITION(s->value[RESERVE_tDM], < 0.0);
//
//				logger(g_log, "Using ONLY reserve\n");
//				s->value[DEL_FOLIAGE] = biomass_foliage_budburst;
//				s->value[DEL_ROOTS_FINE] = 0.0;
//				s->value[DEL_RESERVE] = - (((fabs(s->value[C_FLUX]) * GC_GDM)/1000000.0) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell) +	biomass_foliage_budburst);
//				s->value[DEL_STEMS] = 0.0;
//				s->value[DEL_ROOTS_COARSE] = 0.0;
//				s->value[DEL_ROOTS_TOT] = 0.0;
//				s->value[DEL_TOT_STEM] = 0.0;
//				s->value[DEL_BB]= 0.0;
//			}
//
//			/*just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
//			 * and Barbaroux et al., 2002,
//								the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
//			 */
//			/*the fraction of reserve to allocate for foliage is re-computed for each of the BUD_BURST days
//			 * sharing the daily remaining amount (taking into account respiration costs)of NSC */
//
//			/*partitioning*/
//			/*
//			if (s->value[NPP] > 0.0)
//			{
//				logger(g_log, "Using reserve and npp\n");
//				s->value[DEL_FOLIAGE] = (frac_to_foliage_fineroot * (1.0 - s->value[FINE_ROOT_LEAF_FRAC])) + s->value[NPP];
//				s->value[DEL_STEMS] = 0.0;
//				s->value[DEL_RESERVE] -= frac_to_foliage_fineroot;
//				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
//				s->value[DEL_ROOTS_FINE_CTEM] = (frac_to_foliage_fineroot - s->value[DEL_FOLIAGE]);
//				s->value[DEL_ROOTS_TOT] = 0;
//				s->value[DEL_TOT_STEM] = 0.0;
//				s->value[DEL_BB] = 0;
//			}
//			else
//			{
//				if (s->value[RESERVE] > 0)
//				{
//					logger(g_log, "Using ONLY reserve\n");
//					s->value[DEL_FOLIAGE] = (frac_to_foliage_fineroot * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]));
//					s->value[DEL_STEMS] = 0.0;
//					s->value[DEL_RESERVE] = - (((fabs(s->value[C_FLUX]) * GC_GDM)/1000000.0) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell) + frac_to_foliage_fineroot);
//					s->value[DEL_ROOTS_FINE_CTEM] = (frac_to_foliage_fineroot - s->value[DEL_FOLIAGE]);
//					s->value[DEL_ROOTS_COARSE_CTEM] = 0;
//					s->value[DEL_ROOTS_TOT] = 0.0;
//					s->value[DEL_TOT_STEM] = 0.0;
//					s->value[DEL_BB]= 0;
//				}
//				else
//				{
//					logger(g_log, "No reserve no NPP\n");
//					s->value[DEL_FOLIAGE] = 0.0;
//					s->value[DEL_ROOTS_FINE_CTEM] = 0.0;
//					s->value[DEL_RESERVE] = 0.0;
//					s->value[DEL_ROOTS_COARSE_CTEM] = 0.0;
//					s->value[DEL_ROOTS_TOT] = 0.0;
//					s->value[DEL_TOT_STEM] = 0.0;
//					s->value[DEL_STEMS]= 0.0;
//					s->value[DEL_BB]= 0.0;
//					ERROR(s->value[RESERVE],"s->value[RESERVE]");
//				}
//			}
//			 */
//			//			Daily_lai (&c->heights[height].ages[age].species[species]);
//
//			/*check if re-transfer foliage biomass to reserve*/
//			if (s->value[LAI] > s->value[PEAK_LAI])
//			{
//				logger(g_log, "LAI exceeds Peak Lai\n");
//				CHECK_CONDITION(s->value[LAI], < s->value[PEAK_LAI])
//
//				/*partitioning*/
//				/* no need to re-transfer mass to reserve diving for BUDBURST at the end of BUDBURST period all the c is perfectly allocated */
//				//				s->value[DEL_FOLIAGE] = 0.0 ;
//				//				s->value[DEL_ROOTS_FINE_CTEM] = 0.0;
//				//				s->value[DEL_RESERVE] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE])+(s->value[MAX_BIOMASS_FINE_ROOTS] - s->value[MAX_BIOMASS_FINE_ROOTS]);
//				//				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
//				//				s->value[DEL_ROOTS_TOT] = 0;
//				//				s->value[DEL_TOT_STEM] = 0;
//				//				s->value[DEL_STEMS] = 0;
//				//				s->value[DEL_BB] = 0;
//				//
//				//				/*allocation*/
//				//				s->value[BIOMASS_FOLIAGE] = s->value[MAX_BIOMASS_FOLIAGE];
//				//				logger(g_log, "Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_FOLIAGE]);
//				//				s->value[BIOMASS_TOT_STEM] += s->value[DEL_TOT_STEM];
//				//				logger(g_log, "Total Stem Biomass (Wts) = %f\n", s->value[BIOMASS_TOT_STEM]);
//				//				s->value[BIOMASS_STEM] += s->value[DEL_STEMS];
//				//				logger(g_log, "Stem Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_STEM]);
//				//				s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
//				//				logger(g_log, "Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);
//				//				s->value[RESERVE] +=  s->value[DEL_RESERVE];
//				//				logger(g_log, "Reserve Biomass (Wres) = %f tDM/area\n", s->value[RESERVE]);
//				//				s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
//				//				logger(g_log, "Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
//				//				s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
//				//				logger(g_log, "Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
//				//				s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
//				//				logger(g_log, "Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);
//				//
//				//				//check for live and dead tissues
//				//				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
//				//				logger(g_log, "Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
//				//				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
//				//				logger(g_log, "Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
//				//				s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
//				//				logger(g_log, "Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
//				//				s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
//				//				logger(g_log, "Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
//				//				s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
//				//				logger(g_log, "Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
//				//				s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
//				//				logger(g_log, "Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);
//				//
//				//				/*recompute correct LAI*/
//				//				Daily_lai (&c->heights[height].ages[age].species[species]);
//			}
//
//			// Total Biomass
//			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE_tDM] + s->value[BIOMASS_STEM_tDM] + s->value[BIOMASS_ROOTS_TOT_tDM] + s->value[RESERVE_tDM] + s->value[BIOMASS_BRANCH_tDM];
//			logger(g_log, "Previous Total W = %f tDM/area\n", oldW);
//			logger(g_log, "Total Biomass = %f tDM/area\n", s->value[TOTAL_W]);
//
//			c->daily_leaf_carbon += s->value[DEL_FOLIAGE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_stem_carbon += s->value[DEL_STEMS]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_fine_root_carbon += s->value[DEL_ROOTS_FINE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_coarse_root_carbon += s->value[DEL_ROOTS_COARSE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_branch_carbon += s->value[DEL_BB]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_reserve_carbon += s->value[DEL_RESERVE] / GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//
//			break;
//
//		case 2:
//			//			logger(g_log, "(PEAK_Y_LAI * 0.5 < LAI < PEAK_Y_LAI) \n");
//			//			logger(g_log, "LAI = %f \n", s->value[LAI]);
//			//			logger(g_log, "**Maximum Growth**\n");
//			//			logger(g_log, "Allocating only into foliage and fine root pools\n");
//			//
//			//			s->value[DAILY_DEL_LITTER] = 0;
//			//
//			//			//fixme scegliere se usare Magnani o meno
//			//			//just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
//			//			//the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
//			//			/*partitioning*/
//			//			if(s->value[NPP] > 0.0)
//			//			{
//			//				/*allocating into fine root and foliage*/
//			//				s->value[DEL_FOLIAGE] = (s->value[NPP] * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]));
//			//				s->value[DEL_ROOTS_FINE_CTEM] = (s->value[NPP] * s->value[FINE_ROOT_LEAF_FRAC]);
//			//				s->value[DEL_RESERVE] = 0;
//			//				s->value[DEL_ROOTS_TOT] = 0;
//			//				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
//			//				s->value[DEL_TOT_STEM] = 0;
//			//				s->value[DEL_STEMS]= 0;
//			//				s->value[DEL_BB]= 0;
//			//			}
//			//			else
//			//			{
//			//				logger(g_log, "NPP < 0, nothing to allocate\n");
//			//				//TEST
//			//				//				s->value[DEL_FOLIAGE] = (frac_to_foliage_fineroot * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]));
//			//				//				s->value[DEL_RESERVE] =-(((fabs(s->value[C_FLUX]) * GC_GDM)/1000000.0) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell) + frac_to_foliage_fineroot);
//			//				//				s->value[DEL_ROOTS_FINE_CTEM] = (frac_to_foliage_fineroot - s->value[DEL_FOLIAGE]);
//			//
//			//				s->value[DEL_FOLIAGE] = 0;
//			//				s->value[DEL_ROOTS_FINE_CTEM] = 0;
//			//				s->value[DEL_RESERVE] = -((fabs(s->value[C_FLUX]) * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			//				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
//			//				s->value[DEL_ROOTS_TOT] = 0;
//			//				s->value[DEL_TOT_STEM] = 0;
//			//				s->value[DEL_STEMS]= 0;
//			//				s->value[DEL_BB]= 0;
//			//
//			//				CHECK_CONDITION(s->value[RESERVE], < 0)
//			//			}
//			//			//recompute LAI
//			//			Daily_lai (&c->heights[height].ages[age].species[species]);
//			//
//			//			//control if new Lai exceeds Peak Lai
//			//			if (s->value[LAI] > s->value[PEAK_LAI])
//			//			{
//			//				logger(g_log, "LAI exceeds Peak Lai\n");
//			//				CHECK_CONDITION(s->value[LAI], < s->value[PEAK_LAI])
//			//				//				logger(g_log, "PHENOLOGICAL PHASE = 2.1 \n (TOO MUCH BIOMASS INTO FOLIAGE)\n");
//			//				//				logger(g_log, "LAI = %f \n", s->value[LAI]);
//			//				//
//			//				//				logger(g_log, "LAI exceeds Peak Lai\n");
//			//				//				logger(g_log, "Re-allocating %f leaves exceeding biomass into the three pools Ws+Wr+Wreserve\n", (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]));
//			//				//
//			//				//				/*partitioning*/
//			//				//				/*re-transfer mass to other pools*/
//			//				//				s->value[DEL_FOLIAGE] = (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]);
//			//				//				s->value[DEL_ROOTS_TOT] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * pR_CTEM;
//			//				//				s->value[DEL_ROOTS_FINE_CTEM] += (s->value[DEL_ROOTS_TOT] * Perc_fine);
//			//				//				s->value[DEL_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_TOT] * Perc_coarse;
//			//				//				s->value[DEL_STEMS] += ((s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) *  pS_CTEM) * ( 1.0 - s->value[FRACBB]);
//			//				//				s->value[DEL_BB] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * (pS_CTEM * s->value[FRACBB]);
//			//				//				s->value[DEL_RESERVE] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * pF_CTEM;
//			//				//				s->value[DEL_TOT_STEM] = s->value[DEL_STEMS] + s->value[DEL_BB];
//			//				//
//			//				//				/*allocation*/
//			//				//				s->value[BIOMASS_FOLIAGE] += s->value[DEL_FOLIAGE];
//			//				//				logger(g_log, "Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_FOLIAGE]);
//			//				//				s->value[BIOMASS_TOT_STEM] += s->value[DEL_TOT_STEM];
//			//				//				logger(g_log, "Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM]);
//			//				//				s->value[BIOMASS_STEM] += s->value[DEL_STEMS];
//			//				//				logger(g_log, "Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM]);
//			//				//				s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
//			//				//				logger(g_log, "Branch and Bark Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);
//			//				//				s->value[RESERVE] +=  s->value[DEL_RESERVE];
//			//				//				logger(g_log, "Reserve Biomass (Wres) = %f tDM/area\n", s->value[RESERVE]);
//			//				//				s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
//			//				//				logger(g_log, "Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
//			//				//				s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
//			//				//				logger(g_log, "Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
//			//				//				s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
//			//				//				logger(g_log, "Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);
//			//				//
//			//				//				//check for live and dead tissues
//			//				//				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
//			//				//				logger(g_log, "Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
//			//				//				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
//			//				//				logger(g_log, "Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
//			//				//				s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
//			//				//				logger(g_log, "Live Coarse Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
//			//				//				s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
//			//				//				logger(g_log, "Dead Coarse Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
//			//				//				s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
//			//				//				logger(g_log, "Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
//			//				//				s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
//			//				//				logger(g_log, "Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);
//			//				//
//			//				//				//recompute LAI
//			//				//				Daily_lai (&c->heights[height].ages[age].species[species]);
//			//			}
//			//			// Total Biomass
//			//			s->value[TOTAL_W] = s->value[BIOMASS_FOLIAGE] + s->value[BIOMASS_STEM] + s->value[BIOMASS_ROOTS_TOT] + s->value[RESERVE] + s->value[BIOMASS_BRANCH];
//			//			logger(g_log, "Previous Total W = %f tDM/area\n", oldW);
//			//			logger(g_log, "Total Biomass = %f tDM/area\n", s->value[TOTAL_W]);
//			//
//			//			c->daily_leaf_carbon += s->value[DEL_FOLIAGE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			//			c->daily_stem_carbon += s->value[DEL_STEMS]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			//			c->daily_fine_root_carbon += s->value[DEL_ROOTS_FINE_CTEM]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			//			c->daily_coarse_root_carbon += s->value[DEL_ROOTS_COARSE_CTEM]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			//			c->daily_branch_carbon += s->value[DEL_BB]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			//			c->daily_reserve_carbon += s->value[DEL_RESERVE] / GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//
//			break;
//			/************************************************************/
//		case 3:
//			//			logger(g_log, "**Normal Growth**\n");
//			//			logger(g_log, "Day length > %f \n", /*c->abscission_daylength*/s->value[MINDAYLENGTH] );
//			//			logger(g_log, "allocating into the three pools Ws+Wr(Wrc+Wrf))+Wf\n");
//			//
//			//			s->value[DAILY_DEL_LITTER] = 0;
//			//
//			//
//			//			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
//			//			//logger(g_log, "Roots CTEM ratio layer %d = %f %%\n", z, pR_CTEM * 100);
//			//			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
//			//			//logger(g_log, "Stem CTEM ratio = %f %%\n", pS_CTEM * 100);
//			//			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
//			//			//logger(g_log, "Foliage CTEM ratio = %f %%\n", pF_CTEM * 100);
//			//
//			//			/*partitioning*/
//			//			if (s->value[NPP] > 0.0)
//			//			{
//			//				//fixme do it also for 0.1
//			//				//REPRODUCTION ONLY FOR NEEDLE LEAF DECIDUOUS
//			//				if (s->value[PHENOLOGY] == 0.2)
//			//				{
//			//					//NPP for reproduction
//			//					s->value[DEL_FRUIT] = s->value[NPP] * s->value[FRUIT_PERC];
//			//					s->value[NPP] -= s->value[DEL_FRUIT];
//			//					s->value[BIOMASS_FRUIT] += s->value[DEL_FRUIT];
//			//					logger(g_log, "Biomass increment into cones = %f tDM/area\n", s->value[DEL_FRUIT]);
//			//					//reproductive life span
//			//					s->value[BIOMASS_FRUIT] -= (s->value[BIOMASS_FRUIT] * (1 / s->value[CONES_LIFE_SPAN]));
//			//				}
//			//
//			//				s->value[DEL_RESERVE] = 0;
//			//				s->value[DEL_FOLIAGE] = s->value[NPP] * pF_CTEM;
//			//				s->value[DEL_ROOTS_TOT] = s->value[NPP] * pR_CTEM;
//			//				s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT] * Perc_fine;
//			//				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT] - s->value[DEL_ROOTS_FINE_CTEM];
//			//				s->value[DEL_TOT_STEM] = s->value[NPP] * pS_CTEM;
//			//				s->value[DEL_STEMS] = (s->value[NPP] * pS_CTEM) * (1.0 - s->value[FRACBB]);
//			//				s->value[DEL_BB] = s->value[DEL_TOT_STEM] * s->value[FRACBB];
//			//			}
//			//			else
//			//			{
//			//				s->value[DEL_FOLIAGE] = 0;
//			//				s->value[DEL_ROOTS_FINE_CTEM] = 0;
//			//				s->value[DEL_RESERVE] = -((fabs(s->value[C_FLUX]) * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			//				s->value[DEL_ROOTS_COARSE_CTEM] = 0;
//			//				s->value[DEL_ROOTS_TOT] = 0;
//			//				s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT] - s->value[DEL_ROOTS_FINE_CTEM];
//			//				s->value[DEL_TOT_STEM] = 0;
//			//				s->value[DEL_STEMS]= 0;
//			//				s->value[DEL_BB]= 0;
//			//
//			//				CHECK_CONDITION(s->value[RESERVE], < 0);
//			//			}
//			//			//recompute LAI
//			//			Daily_lai (&c->heights[height].ages[age].species[species]);
//			//
//			//			//control if new Lai exceeds Peak Lai
//			//			if (s->value[LAI] > s->value[PEAK_LAI])
//			//			{
//			//				logger(g_log, "LAI exceeds Peak Lai\n");
//			//				CHECK_CONDITION(s->value[LAI], < s->value[PEAK_LAI])
//			//				//				logger(g_log, "PHENOLOGICAL PHASE = 2.1 \n (TOO MUCH BIOMASS INTO FOLIAGE)\n");
//			//				//				logger(g_log, "LAI = %f \n", s->value[LAI]);
//			//				//
//			//				//				logger(g_log, "LAI exceeds Peak Lai\n");
//			//				//				logger(g_log, "Re-allocating foliar exceeding biomass into the three pools Ws+Wr+Wreserve\n");
//			//				//
//			//				//				/*partitioning*/
//			//				//				/*re-transfer mass to other pools*/
//			//				//				s->value[DEL_FOLIAGE] = (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]);
//			//				//				s->value[DEL_ROOTS_TOT] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * pR_CTEM;
//			//				//				s->value[DEL_ROOTS_FINE_CTEM] += (s->value[DEL_ROOTS_TOT] * Perc_fine);
//			//				//				s->value[DEL_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_TOT] * Perc_coarse;
//			//				//				s->value[DEL_STEMS] += ((s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) *  pS_CTEM) * ( 1.0 - s->value[FRACBB]);
//			//				//				s->value[DEL_BB] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * (pS_CTEM * s->value[FRACBB]);
//			//				//				s->value[DEL_RESERVE] += (s->value[BIOMASS_FOLIAGE] - s->value[MAX_BIOMASS_FOLIAGE]) * pF_CTEM;
//			//				//				s->value[DEL_TOT_STEM] = s->value[DEL_STEMS] + s->value[DEL_BB];
//			//				//
//			//				//				/*allocation*/
//			//				//				s->value[BIOMASS_FOLIAGE] = s->value[MAX_BIOMASS_FOLIAGE];
//			//				//				logger(g_log, "Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_STEM]);
//			//				//				s->value[BIOMASS_TOT_STEM] += s->value[DEL_TOT_STEM];
//			//				//				logger(g_log, "Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM]);
//			//				//				s->value[BIOMASS_STEM] += s->value[DEL_STEMS];
//			//				//				logger(g_log, "Branch and Bark Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_STEM]);
//			//				//				s->value[BIOMASS_BRANCH] += s->value[DEL_BB];
//			//				//				logger(g_log, "Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH]);
//			//				//				s->value[RESERVE] +=  s->value[DEL_RESERVE];
//			//				//				logger(g_log, "Reserve Biomass (Wres) = %f tDM/area\n", s->value[RESERVE]);
//			//				//				s->value[BIOMASS_ROOTS_TOT] +=  s->value[DEL_ROOTS_TOT];
//			//				//				logger(g_log, "Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT]);
//			//				//				s->value[BIOMASS_ROOTS_FINE] += s->value[DEL_ROOTS_FINE_CTEM];
//			//				//				logger(g_log, "Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_ROOTS_FINE]);
//			//				//				s->value[BIOMASS_ROOTS_COARSE] += s->value[DEL_ROOTS_COARSE_CTEM];
//			//				//				logger(g_log, "Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_ROOTS_COARSE]);
//			//				//
//			//				//				//check for live and dead tissues
//			//				//				s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
//			//				//				logger(g_log, "Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
//			//				//				//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
//			//				//				logger(g_log, "Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
//			//				//				s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
//			//				//				logger(g_log, "Live Coarse Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
//			//				//				s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
//			//				//				logger(g_log, "Dead Coarse Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
//			//				//				s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
//			//				//				logger(g_log, "Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
//			//				//				s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
//			//				//				logger(g_log, "Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);
//			//			}
//			//
//			//			c->daily_leaf_carbon += s->value[DEL_FOLIAGE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			//			c->daily_stem_carbon += s->value[DEL_STEMS]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			//			c->daily_fine_root_carbon += s->value[DEL_ROOTS_FINE_CTEM]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			//			c->daily_coarse_root_carbon += s->value[DEL_ROOTS_COARSE_CTEM]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			//			c->daily_branch_carbon += s->value[DEL_BB]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			//			c->daily_reserve_carbon += s->value[DEL_RESERVE] / GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//
//			break;
//			/************************************************************************/
//		case 4:
//			logger(g_log, "(LAI == PEAK LAI)\n");
//			logger(g_log, "allocating into the three pools Ws+Wr(Wrc+Wrf)+Wreserve\n");
//			/*see Barbaroux et al., 2002, Scartazza et al., 2013*/
//
//			s->value[DAILY_DEL_LITTER] = 0;
//
//			pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * (2.0 - Light_trasm - s->value[F_SW])));
//			logger(g_log, "Roots CTEM ratio layer = %f %%\n", pR_CTEM * 100);
//			pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * (2.0 - Light_trasm - s->value[F_SW])));
//			logger(g_log, "Stem CTEM ratio = %f %%\n", pS_CTEM * 100);
//			pF_CTEM = (1.0 - pS_CTEM - pR_CTEM);
//			logger(g_log, "Reserve CTEM ratio = %f %%\n", pF_CTEM * 100);
//
//			/*partitioning*/
//			if (s->value[NPP_tDM] > 0.0)
//			{
//				//fixme do it also for 0.1
//				//REPRODUCTION ONLY FOR NEEDLE LEAF
//				if (s->value[PHENOLOGY] == 0.2)
//				{
//					//NPP for reproduction
//					s->value[BIOMASS_FRUIT_tDM] = s->value[NPP_tDM] * s->value[FRUIT_PERC];
//					s->value[NPP_tDM] -= s->value[BIOMASS_FRUIT_tDM];
//					logger(g_log, "Biomass increment into cones = %f tDM/area\n", s->value[BIOMASS_FRUIT_tDM]);
//
//					//reproductive life span
//					s->value[BIOMASS_FRUIT_tDM] -= (s->value[BIOMASS_FRUIT_tDM] * (1 / s->value[CONES_LIFE_SPAN]));
//				}
//
//				s->value[DEL_RESERVE] = s->value[NPP_tDM] * pF_CTEM;
//				s->value[DEL_ROOTS_TOT] = s->value[NPP_tDM] * pR_CTEM;
//				s->value[DEL_ROOTS_FINE] = s->value[DEL_ROOTS_TOT] * Perc_fine;
//				s->value[DEL_ROOTS_COARSE] = s->value[DEL_ROOTS_TOT] - s->value[DEL_ROOTS_FINE];
//				s->value[DEL_TOT_STEM] = s->value[NPP_tDM] * pS_CTEM;
//				s->value[DEL_STEMS] = (s->value[NPP_tDM] * pS_CTEM) * (1.0 - s->value[FRACBB]);
//				s->value[DEL_BB] = (s->value[NPP_tDM] * pS_CTEM) * s->value[FRACBB];
//				s->value[DEL_FOLIAGE] = 0;
//			}
//			else
//			{
//				s->value[DEL_FOLIAGE] = 0.0;
//				s->value[DEL_ROOTS_FINE] = 0.0;
//				s->value[DEL_RESERVE] = -((fabs(s->value[C_FLUX]) * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//				s->value[DEL_ROOTS_COARSE] = 0.0;
//				s->value[DEL_ROOTS_TOT] = 0.0;
//				s->value[DEL_TOT_STEM] = 0.0;
//				s->value[DEL_STEMS]= 0.0;
//				s->value[DEL_BB]= 0.0;
//				CHECK_CONDITION(s->value[RESERVE_tDM], < 0.0);
//			}
//
//			c->daily_leaf_carbon += s->value[DEL_FOLIAGE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_stem_carbon += s->value[DEL_STEMS]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_fine_root_carbon += s->value[DEL_ROOTS_FINE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_coarse_root_carbon += s->value[DEL_ROOTS_COARSE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_branch_carbon += s->value[DEL_BB]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_reserve_carbon += s->value[DEL_RESERVE] / GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//
//			break;
//			/**********************************************************************/
//		case 5:
//			logger(g_log, "(DayLength < MINDAYLENGTH)\n");
//			logger(g_log, "LEAF FALL\n");
//			logger(g_log, "allocating into W reserve pool\n");
//			logger(g_log, "++Lai before Leaffall= %f\n", s->value[LAI]);
//			logger(g_log, "Biomass foliage = %f \n", s->value[BIOMASS_FOLIAGE_tDM]);
//			logger(g_log, "Biomass fine root = %f \n", s->value[BIOMASS_FINE_ROOT_tDM]);
//			logger(g_log, "foliage reduction rate %f \n", s->value[FOLIAGE_REDUCTION_RATE]);
//			//logger(g_log, "biomass foliage to remove %f \n", s->value[DAILY_FOLIAGE_BIOMASS_TO_REMOVE]);
//
//			if(s->counter[LEAF_FALL_COUNTER] == 1)
//			{
//				logger(g_log, "First day of Leaf fall\n");
//				//s->counter[SENESCENCE_DAYONE] = c->doy;
//				//s->counter[DAY_FRAC_FOLIAGE_REMOVE] =  endOfYellowing(met, &c->heights[height].ages[age].species[species]) - c->heights[height].ages[age].species[species].counter[SENESCENCE_DAYONE];
//				logger(g_log, "DAYS FOR FOLIAGE and FINE ROOT for_REMOVING = %d\n", s->counter[DAY_FRAC_FOLIAGE_REMOVE]);
//				//Marconi: assumed that fine roots for deciduos species progressively die togheter with leaves
//
//				/* assuming linear leaf fall */
//				s->value[DEL_FOLIAGE] = -(s->value[BIOMASS_FOLIAGE_tDM] / s->counter[DAY_FRAC_FOLIAGE_REMOVE]);
//				logger(g_log, "daily amount of foliage to remove = %f\n", s->value[DEL_FOLIAGE]);
//				s->value[DEL_ROOTS_FINE]= -(s->value[BIOMASS_FINE_ROOT_tDM] / s->counter[DAY_FRAC_FOLIAGE_REMOVE]);
//				logger(g_log, "daily amount of fine root to remove = %f\n", s->value[DEL_ROOTS_FINE]);
//
//				/* following Campioli et al., 2013 and Bossel 1996 10% of foliage and fine root biomass is retranslocated as reserve in the reserve pool */
//				/* compute amount of fine root biomass to retranslocate as reserve */
//				s->value[RESERVE_FOLIAGE_TO_RETRANSL_tDM] = (s->value[BIOMASS_FOLIAGE_tDM] *0.1) / (int)s->counter[DAY_FRAC_FOLIAGE_REMOVE];
//				s->value[RESERVE_FINEROOT_TO_RETRANSL_tDM] = (s->value[BIOMASS_FINE_ROOT_tDM] *0.1) / (int)s->counter[DAY_FRAC_FOLIAGE_REMOVE];
//				logger(g_log, "RESERVE_FOLIAGE_TO_RETRANSL = %f\n", s->value[RESERVE_FOLIAGE_TO_RETRANSL_tDM]);
//				logger(g_log, "RESERVE_FINEROOT_TO_RETRANSL = %f\n", s->value[RESERVE_FINEROOT_TO_RETRANSL_tDM]);
//
//			}
//
//			//			/* leaffall */
//			//			leaffall(&c->heights[height].ages[age].species[species], met,
//			//					&c->doy, &c->top_layer, i);
//
//			//			Daily_lai (&c->heights[height].ages[age].species[species]);
//
//			if (s->value[NPP_tDM] > 0.0)
//			{
//				//REPRODUCTION ONLY FOR NEEDLE LEAF
//				if (s->value[PHENOLOGY] == 0.2)
//				{
//					//NPP for reproduction
//					s->value[BIOMASS_FRUIT_tDM] = s->value[NPP_tDM] * s->value[FRUIT_PERC];
//					s->value[NPP_tDM] -= s->value[BIOMASS_FRUIT_tDM];
//					logger(g_log, "Biomass increment into cones = %f tDM/area\n", s->value[BIOMASS_FRUIT_tDM]);
//
//					//reproductive life span
//					s->value[BIOMASS_FRUIT_tDM] -= (s->value[BIOMASS_FRUIT_tDM] * (1 / s->value[CONES_LIFE_SPAN]));
//				}
//				s->value[DAILY_DEL_LITTER] = - s->value[DEL_FOLIAGE];
//				//fixme do it with the total amount of foliage biomass
//				s->value[DEL_RESERVE] = s->value[NPP_tDM] + s->value[RESERVE_FOLIAGE_TO_RETRANSL_tDM] + s->value[RESERVE_FINEROOT_TO_RETRANSL_tDM];
//				s->value[DEL_TOT_STEM] = 0;
//				s->value[DEL_STEMS] = 0;
//				s->value[DEL_ROOTS_COARSE] = 0.0;
//				s->value[DEL_ROOTS_TOT] = 0;
//				s->value[DEL_BB] = 0;
//			}
//			else
//			{
//				s->value[DEL_TOT_STEM] = 0;
//				s->value[DEL_STEMS] = 0;
//				s->value[DEL_ROOTS_COARSE] = 0;
//				s->value[DEL_ROOTS_TOT] = 0;
//				s->value[DEL_BB] = 0;
//				s->value[DEL_RESERVE] = -((fabs(s->value[C_FLUX]) * GC_GDM)/1000000) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell) +
//						(s->value[RESERVE_FOLIAGE_TO_RETRANSL_tDM] + s->value[RESERVE_FINEROOT_TO_RETRANSL_tDM]);
//			}
//
//			c->daily_leaf_carbon += 0.0/* s->value[DEL_FOLIAGE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell)*/;
//			c->daily_stem_carbon += s->value[DEL_STEMS]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_fine_root_carbon += 0.0/*s->value[DEL_ROOTS_FINE_CTEM]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell)*/;
//			c->daily_coarse_root_carbon += s->value[DEL_ROOTS_COARSE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_branch_carbon += s->value[DEL_BB]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_reserve_carbon += s->value[DEL_RESERVE] / GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->leafLittering += fabs(s->value[DEL_FOLIAGE]) / GC_GDM * 1000 / g_settings->sizeCell;
//			c->leaflitN = c->leafLittering /GC_GDM * 1000 / g_settings->sizeCell /s->value[CN_DEAD_WOODS];
//			c->fineRootLittering +=  s->value[DAILY_FINEROOT_BIOMASS_TO_REMOVE] / GC_GDM * 1000 / g_settings->sizeCell;
//			c->fineRootlitN += s->value[BIOMASS_FINE_ROOT_tDM] / GC_GDM * 1000 / g_settings->sizeCell  / s->value[CN_FINE_ROOTS];
//
//			break;
//
//		case 0:
//
//			logger(g_log, "Unvegetative period \n");
//
//			s->value[DAILY_DEL_LITTER] = 0;
//
//			/*partitioning*/
//			s->value[DEL_RESERVE] = -((s->value[TOTAL_AUT_RESP] * GC_GDM)/1000000.0) * (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//
//			//SERGIOM
//			//using the sigmoid it is necessary to leave the remainder at the very begining of the phase 0; that
//			//because sigmoid decreases asymptotically to 0
//			if (s->value[BIOMASS_FOLIAGE_tDM] > 0)
//			{
//				s->value[DEL_FOLIAGE] = -s->value[BIOMASS_FOLIAGE_tDM];
//			}
//			else
//			{
//				s->value[DEL_FOLIAGE] = 0;
//			}
//			if (s->value[BIOMASS_FINE_ROOT_tDM] > 0)
//			{
//				s->value[BIOMASS_FINE_ROOT_tDM] = -s->value[BIOMASS_FINE_ROOT_tDM];
//			}
//			s->value[DEL_ROOTS_FINE] = 0;
//			s->value[DEL_ROOTS_COARSE] = 0;
//			s->value[DEL_ROOTS_TOT] = 0;
//			s->value[DEL_TOT_STEM] = 0;
//			s->value[DEL_STEMS]= 0;
//			s->value[DEL_BB]= 0;
//
//			c->leafLittering += fabs(s->value[DEL_FOLIAGE]) / GC_GDM * 1000 / g_settings->sizeCell;
//			c->leaflitN = c->leafLittering /GC_GDM * 1000 / g_settings->sizeCell /s->value[CN_DEAD_WOODS];
//			c->daily_leaf_carbon += 0.0;
//			c->daily_stem_carbon += s->value[DEL_STEMS]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_fine_root_carbon += 0.0;
//			c->daily_coarse_root_carbon += s->value[DEL_ROOTS_COARSE]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_branch_carbon += s->value[DEL_BB]/ GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//			c->daily_reserve_carbon += s->value[DEL_RESERVE] / GC_GDM * 1000000.0 / (s->value[CANOPY_COVER_DBHDC]* g_settings->sizeCell);
//
//
//
//			break;
//		}
//	}
//
//	/*allocation*/
//	s->value[BIOMASS_FOLIAGE_tDM] += s->value[DEL_FOLIAGE];
//	logger(g_log, "Foliage Biomass (Wf) = %f tDM/area\n", s->value[BIOMASS_FOLIAGE_tDM]);
//	s->value[BIOMASS_STEM_tDM] += s->value[DEL_STEMS];
//	logger(g_log, "Branch and Bark Biomass (Wbb) = %f tDM/area\n", s->value[BIOMASS_STEM_tDM]);
//	s->value[BIOMASS_BRANCH_tDM] += s->value[DEL_BB];
//	logger(g_log, "Branch and Bark Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_BRANCH_tDM]);
//	s->value[RESERVE_tDM] +=  s->value[DEL_RESERVE];
//	logger(g_log, "Reserve Biomass (Wres) = %f tDM/area\n", s->value[RESERVE_tDM]);
//	s->value[BIOMASS_ROOTS_TOT_tDM] +=  s->value[DEL_ROOTS_TOT];
//	logger(g_log, "Total Root Biomass (Wr TOT) = %f tDM/area\n", s->value[BIOMASS_ROOTS_TOT_tDM]);
//	s->value[BIOMASS_FINE_ROOT_tDM] += s->value[DEL_ROOTS_FINE];
//	logger(g_log, "Fine Root Biomass (Wrf) = %f tDM/area\n", s->value[BIOMASS_FINE_ROOT_tDM]);
//	s->value[BIOMASS_COARSE_ROOT_tDM] += s->value[DEL_ROOTS_COARSE];
//	logger(g_log, "Coarse Root Biomass (Wrc) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_tDM]);
//	s->value[BIOMASS_TOT_STEM_tDM] += s->value[DEL_TOT_STEM];
//	logger(g_log, "Total Stem Biomass (Wts)= %f\n", s->value[BIOMASS_TOT_STEM_tDM]);
//
//	Daily_lai (&c->heights[height].ages[age].species[species]);
//
//	//check for live and dead tissues
//	//	s->value[BIOMASS_STEM_LIVE_WOOD] += (s->value[DEL_STEMS] /** s->value[LIVE_TOTAL_WOOD]*/);
//	//	logger(g_log, "Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD]);
//	//	//s->value[BIOMASS_STEM_DEAD_WOOD] += (s->value[DEL_STEMS] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
//	//	logger(g_log, "Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD]);
//	//	s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** s->value[LIVE_TOTAL_WOOD]*/);
//	//	logger(g_log, "Live Coarse Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD]);
//	//	s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD] += (s->value[DEL_ROOTS_COARSE_CTEM] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
//	//	logger(g_log, "Dead Coarse Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD]);
//	//	s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD] += (s->value[DEL_BB] /** s->value[LIVE_TOTAL_WOOD]*/);
//	//	logger(g_log, "Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD]);
//	//	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD] += (s->value[DEL_BB] /** (1.0 -s->value[LIVE_TOTAL_WOOD])*/);
//	//	logger(g_log, "Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD]);
//
//	s->value[BIOMASS_STEM_LIVE_WOOD_tDM] += (s->value[DEL_STEMS] * s->value[LIVE_TOTAL_WOOD_FRAC]);
//	logger(g_log, "Live Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM]);
//	s->value[BIOMASS_STEM_DEAD_WOOD_tDM] += s->value[DEL_STEMS]*(1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]);
//	logger(g_log, "Dead Stem Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);
//	s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] += (s->value[DEL_ROOTS_COARSE] * s->value[LIVE_TOTAL_WOOD_FRAC]);
//	logger(g_log, "Live Coarse Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM]);
//	s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM] += s->value[DEL_ROOTS_COARSE] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]);
//	logger(g_log, "Dead Coarse Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
//	s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] += (s->value[DEL_BB] * s->value[LIVE_TOTAL_WOOD_FRAC]);
//	logger(g_log, "Live Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM]);
//	s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM] += (s->value[DEL_BB] * (1.0 -s->value[LIVE_TOTAL_WOOD_FRAC]));
//	logger(g_log, "Dead Stem Branch Biomass (Ws) = %f tDM/area\n", s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);
//
//	if (fabs((s->value[BIOMASS_STEM_LIVE_WOOD_tDM] + s->value[BIOMASS_STEM_DEAD_WOOD_tDM]) - s->value[BIOMASS_STEM_tDM])>1e-4)
//	{
//		logger(g_log, "Live stem + dead stem = %f\n", s->value[BIOMASS_STEM_LIVE_WOOD_tDM] + s->value[BIOMASS_STEM_DEAD_WOOD_tDM]);
//		logger(g_log, "Total stem = %f\n", s->value[BIOMASS_STEM_tDM]);
//		exit(1);
//	}
//	if (fabs((s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] + s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]) - s->value[BIOMASS_COARSE_ROOT_tDM])>1e-4)
//	{
//		logger(g_log, "Live coarse + dead coarse = %f\n", s->value[BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM] + s->value[BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM]);
//		logger(g_log, "Total coarse = %f\n", s->value[BIOMASS_COARSE_ROOT_tDM]);
//		exit(1);
//	}
//	if (fabs((s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] + s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]) - s->value[BIOMASS_BRANCH_tDM])>1e-4)
//	{
//		logger(g_log, "Live branch + dead branch = %f\n", s->value[BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM] + s->value[BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM]);
//		logger(g_log, "Total branch = %f\n", s->value[BIOMASS_BRANCH_tDM]);
//		exit(1);
//	}
//
//
//	//fixme compute from unspatial
//	s->value[DEL_Y_WTS] += s->value[DEL_TOT_STEM];
//	s->value[DEL_Y_WS] += s->value[DEL_STEMS];
//	s->value[DEL_Y_WF] += s->value[DEL_FOLIAGE];
//	s->value[DEL_Y_WFR] += s->value[DEL_ROOTS_FINE];
//	s->value[DEL_Y_WCR] += s->value[DEL_ROOTS_COARSE];
//	s->value[DEL_Y_WRES] += s->value[DEL_RESERVE];
//	s->value[DEL_Y_WR] += s->value[DEL_ROOTS_TOT];
//	s->value[DEL_Y_BB] += s->value[DEL_BB];
//
//	logger(g_log, "delta_WTS %d = %f \n", c->heights[height].z, s->value[DEL_TOT_STEM]);
//	logger(g_log, "delta_F %d = %f \n", c->heights[height].z, s->value[DEL_FOLIAGE]);
//	logger(g_log, "delta_fR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_FINE]);
//	logger(g_log, "delta_cR %d = %f \n", c->heights[height].z, s->value[DEL_ROOTS_COARSE]);
//	logger(g_log, "delta_S %d = %f \n", c->heights[height].z, s->value[DEL_STEMS]);
//	logger(g_log, "delta_Res %d = %f \n", c->heights[height].z, s->value[DEL_RESERVE]);
//	logger(g_log, "delta_BB %d = %f \n", c->heights[height].z, s->value[DEL_BB]);
//
//	c->daily_lai[i] = s->value[LAI];
//
//	//	c->daily_delta_wts[i] = s->value[DEL_TOT_STEM];
//	//	c->daily_delta_ws[i] = s->value[DEL_STEMS];
//	//	c->daily_delta_wf[i] = s->value[DEL_FOLIAGE];
//	//	c->daily_delta_wbb[i] = s->value[DEL_BB];
//	//	c->daily_delta_wfr[i] = s->value[DEL_ROOTS_FINE];
//	//	c->daily_delta_wcr[i] = s->value[DEL_ROOTS_COARSE];
//	//	c->daily_delta_wres[i] = s->value[DEL_RESERVE];
//	//	c->daily_wres[i] = s->value[RESERVE_tDM];
//	//
//	//	c->daily_lai[i] = s->value[LAI];
//	//	c->annual_delta_ws[i] += s->value[DEL_STEMS];
//	//	c->annual_ws[i] = s->value[BIOMASS_STEM_tDM];
//	//	c->annual_delta_wres[i] += s->value[DEL_RESERVE];
//	//	c->annual_wres[i] = s->value[RESERVE_tDM];
//	//	c->annual_wf[i]= s->value[BIOMASS_FOLIAGE_tDM];
//	//	c->annual_wbb[i]= s->value[BIOMASS_BRANCH_tDM];
//	//	c->annual_wfr[i]= s->value[BIOMASS_FINE_ROOT_tDM];
//	//	c->annual_wcr[i]= s->value[BIOMASS_COARSE_ROOT_tDM];
//
//	if (c->leafLittering < 0.0) c->leafLittering = 0;
//
//	logger(g_log, "******************************\n");
//}

/**/


