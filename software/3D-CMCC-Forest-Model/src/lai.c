/*lai.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

// CUURENTLY NOT USED
void Get_lai (SPECIES *const s, CELL *const c, const int years, const int month, const int day, const int height)
{

	static double frac_to_foliage_fineroot;
	double biomass_for_peak_lai;


	/*for deciduous*/
	if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
	{

		Log("\n--GET_DAILY_LAI--\n");
		Log("VEG_DAYS = %d\n", s->counter[VEG_DAYS]);
		Log("BIOMASS_RESERVE = %f\n", s->value[BIOMASS_RESERVE]);
		Log("PEAK_Y_LAI = %f\n", s->value[PEAK_Y_LAI]);
		Log("LAI = %f\n", s->value[LAI]);


		/*following Campioli et al., 2008, Maillard et al., 1994, Barbaroux et al., 2003*/
		if (s->counter[VEG_DAYS] == 1)
		{
			s->counter[BUD_BURST_COUNTER] = s->value[BUD_BURST];
			Log("Days for bud burst = %f\n", s->value[BUD_BURST]);
		}
		if (s->value[BIOMASS_RESERVE] < 0.0)
		{
			Log("ATTENTION BIOMASS RESERVE < 0.0\n");
		}

		/* to prevent deficit in NSC model allocates into foliage only if this amount isn't negative */
		if (s->counter[VEG_DAYS] <= s->value[BUD_BURST] && s->value[LAI] < s->value[PEAK_Y_LAI] && s->value[BIOMASS_RESERVE] > 0)
		{
			// ALESSIOR s->counter was using BIOMASS_RESERVE
			Log("VEG_DAYS < %d LAI < PEAK_LAI, RESERVE > 0\n", s->counter[VEG_DAYS]);
			/*just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
			 * and Barbaroux et al., 2002,
				the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
			 */
			/*the fraction of reserve to allocate for foliage is re-computed for each of the BUD_BURST days
			 * sharing the daily remaining amount (taking into account respiration costs)of NSC */
			//Angelo try to change with a exponential function as frac_to_foliage = s->value[BIOMASS_RESERVE] * (e^-s->value[BUD_BURST])
			//fixme try to allocate just a part of total reserve not all
			frac_to_foliage_fineroot = (s->value[BIOMASS_RESERVE] * 0.5) / s->counter[BUD_BURST_COUNTER];
			Log("fraction of reserve for foliage and fine root = %f\n", frac_to_foliage_fineroot);

			s->value[BIOMASS_RESERVE] -= frac_to_foliage_fineroot;

			s->counter[BUD_BURST_COUNTER] --;
			Log("++Remaining days for bud burst = %d\n", s->counter[BUD_BURST_COUNTER]);

			Log("++Lai before reserve allocation = %f\n", s->value[LAI]);
			Log("++Peak Lai = %f\n", s->value[PEAK_Y_LAI]);

			Log("Reserves pools = %f tDM/area\n", s->value [BIOMASS_RESERVE]);

			//FIXME ALLOCATING into foliage and fine root
			Log("ratio of reserve for foliage = %f% \n", (1.0 - s->value[FINE_ROOT_LEAF_FRAC]) * 100 );
			Log("ratio of reserve for fine root = %f% \n", s->value[FINE_ROOT_LEAF_FRAC] * 100);

			s->value[BIOMASS_FOLIAGE] += (frac_to_foliage_fineroot * (1.0 - s->value[FINE_ROOT_LEAF_FRAC]));
			Log("Biomass reserve allocated to foliage pool = %f\n", s->value[BIOMASS_FOLIAGE]);


			s->value[BIOMASS_ROOTS_FINE] += (frac_to_foliage_fineroot * s->value[FINE_ROOT_LEAF_FRAC]);
			Log("Biomass reserve allocated to fine root pool = %f\n", s->value[BIOMASS_ROOTS_FINE]);

			/*
 				Log("ratio of reserve for foliage = %f% \n", (1.0 - s->value[FINE_ROOT_LEAF_FRAC]) * 100 );
				Log("ratio of reserve for stem = %f% \n", s->value[STEM_LEAF_FRAC] * 100);
				Log("ratio of reserve for foliage = %f% \n", (1.0 - s->value[STEM_LEAF_FRAC]) * 100 );
				Log("ratio of reserve for stem = %f% \n", s->value[STEM_LEAF_FRAC] * 100);

				s->value[BIOMASS_FOLIAGE] += (frac_to_foliage_stem * (1.0 - s->value[STEM_LEAF_FRAC]));
				Log("Biomass foliage = %f\n", s->value[BIOMASS_FOLIAGE]);

				s->value[BIOMASS_STEM] += (frac_to_foliage_stem * s->value[STEM_LEAF_FRAC]);
				Log("Biomass stem = %f\n", s->value[BIOMASS_STEM]);
			 */


			//s->value[BIOMASS_FOLIAGE] += frac_to_foliage_fineroot;

			Log("++Reserves pools less foliage + fine root = %f tDM/area\n", s->value [BIOMASS_RESERVE]);

			/*for dominant layer with sunlit foliage*/
			if (c->top_layer == c->heights[height].z)
			{
				Log("computing LAI for dominant trees\n");
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLA_AVG] * GC_GDM);
			}
			/*for dominated shaded foliage*/
			else
			{
				Log("computing LAI for dominated trees\n");
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLA_AVG] * s->value[SLA_RATIO]) * GC_GDM);
			}

			/*check if re-transfer foliage biomass to reserve*/
			if (s->value[LAI] > s->value[PEAK_Y_LAI])
			{
				Log("LAI exceeds Peak Lai\n");

				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					biomass_for_peak_lai = ((s->value[PEAK_Y_LAI] * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))/ (s->value[SLA_AVG]* GC_GDM)) / 1000;
				}
				/*for dominated shaded foliage*/
				else
				{
					biomass_for_peak_lai = ((s->value[PEAK_Y_LAI] * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))/ ((s->value[SLA_AVG] * s->value[SLA_RATIO])* GC_GDM)) / 1000;
				}
				/*re-transfer mass to reserve*/
				s->value[BIOMASS_RESERVE] += (biomass_for_peak_lai - s->value[BIOMASS_FOLIAGE]);
				Log("++Reserves pools plus exceeding foliage biomass = %f tDM/area\n", s->value [BIOMASS_RESERVE]);
				s->value[BIOMASS_FOLIAGE] = biomass_for_peak_lai;
				/*recompute correct LAI*/
				/*for dominant layer with sunlit foliage*/
				if (c->top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLA_AVG] * GC_GDM);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLA_AVG] * s->value[SLA_RATIO]) * GC_GDM);
				}
			}

			Log("++Lai from reserves = %f\n", s->value[LAI]);
		}
		if(s->value[LAI] > s->value[PEAK_Y_LAI])
		{
			Log("LAI > PEAK_Y_LAI\n");
			Log("Unused reserve = %f tDM/cell \n", s->value [BIOMASS_RESERVE]);
			Log("++Lai = %f\n", s->value[LAI]);
		}
		else
		{
			Log("++Lai = %f\n", s->value[LAI]);
		}

	}
	/*for evergreen species*/
	//fixme use reserve
	else
	{

		Log("\n--GET_DAILY_LAI--\n");

		//todo make a better function that 'share' over a time the biomass_reserve and not only on one day


		//just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
		//the ratio is driven by the BIOME_BGC newStem:newLeaf ratio

		if(s->value[LAI] == 0)
		{
			/*for dominant layer with sunlit foliage*/
			if (c->top_layer == c->heights[height].z)
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLA_AVG] * GC_GDM);
			}
			/*for dominated shaded foliage*/
			else
			{
				s->value[LAI] = (s->value[BIOMASS_FOLIAGE] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLA_AVG] * s->value[SLA_RATIO]) * GC_GDM);
			}
			Log("++Lai from foliage or reserve = %f\n", s->value[LAI]);
		}
		else
		{
			Log("++Lai from previous day allocation = %f\n", s->value[LAI]);
		}
		if (s->value[LAI] > s->value[PEAK_Y_LAI])
		{
			Log("ATTENTION LAI > PEAK_Y_LAI!!! reallocate exceeding biomass \n");
		}

	}
}


