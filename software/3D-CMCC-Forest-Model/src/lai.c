/*lai.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"


void Get_lai (SPECIES *const s, CELL *const c, const int years, const int month, const int day, const int height)
{

	static float frac_to_foliage_stem;
	float biomass_for_peak_lai;


	/*for deciduous*/
	if (s->value[PHENOLOGY] == 0.1 || s->value[PHENOLOGY] == 0.2)
	{
		if (settings->time == 'm')
		{

			Log("\n--GET_INITIAL_MONTH_LAI--\n");

			if (s->counter[VEG_MONTHS]  == 1)
			{
				Log("++Reserves pools = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);
				Log("++Reserve biomass for each tree in g = %g \n", (s->value[BIOMASS_RESERVE_CTEM] * 1000000) / s->counter[N_TREE]);

				//just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
				//but Scartazza et al 2013 says only in foliages
				//the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
				s->value[BIOMASS_FOLIAGE_CTEM] = s->value[BIOMASS_RESERVE_CTEM] * (1.0 - s->value[STEM_LEAF_FRAC]);
				Log("Biomass foliage = %g\n", s->value[BIOMASS_FOLIAGE_CTEM]);

				Log("ratio of reserve for foliage = %g% \n", (1.0 - s->value[STEM_LEAF_FRAC] * 100));

				s->value[BIOMASS_RESERVE_CTEM] -= s->value[BIOMASS_FOLIAGE_CTEM];
				Log("++Reserves pools less foliage = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);


				//not sure if allocate the remaining reserves for stem
				s->value[BIOMASS_STEM_CTEM] += (s->value[BIOMASS_RESERVE_CTEM] * s->value[STEM_LEAF_FRAC]);
				Log("Biomass stem = %g\n", s->value[BIOMASS_STEM_CTEM]);

				Log("ratio of reserve for stem = %g% \n", s->value[STEM_LEAF_FRAC] * 100);

				s->value[BIOMASS_RESERVE_CTEM] = 0;
				Log("++Reserves pools less foliage + stem = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);


				Log ("++Biomass foliage from reserves for initial LAI = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);


				/*for dominant layer with sunlit foliage*/
				if (c->heights[height].top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
				}

				Log("++Lai from reserves = %g\n", s->value[LAI]);
				//Log("++Canopy Cover = %g\n", s->value[CANOPY_COVER_DBHDC]);
				//Log("++Size Cell = %g\n", settings->sizeCell);
				//Log("++Sla = %g\n", s->value[SLAmkg]);
			}
			else
			{
				Log("++Lai = %g\n", s->value[LAI]);
			}
		}
		else
		{
			Log("\n--GET_DAILY_LAI--\n");
			Log("VEG_DAYS = %d\n", s->counter[VEG_DAYS]);
			Log("BIOMASS_RESERVE_CTEM = %g\n", s->value[BIOMASS_RESERVE_CTEM]);
			Log("PEAK_Y_LAI = %g\n", s->value[PEAK_Y_LAI]);
			Log("LAI = %g\n", s->counter[LAI]);

			/*following Campioli et al., 2008, Maillard et al., 1994, Barbaroux et al., 2003*/
			if (s->counter[VEG_DAYS] == 1)
			{
				s->counter[BUD_BURST_COUNTER] = s->value[BUD_BURST];
				Log("Days for bud burst = %g\n", s->value[BUD_BURST]);
			}
			if (s->value[BIOMASS_RESERVE_CTEM] < 0.0)
			{
				Log("ATTENTION BIOMASS RESERVE < 0.0\n");
			}
			/* to prevent deficit in NSC model allocates into foliage only if this amount isn't negative */
			if (s->counter[VEG_DAYS] <= 30 && s->value[LAI] < s->value[PEAK_Y_LAI] && s->value[BIOMASS_RESERVE_CTEM] > 0)
			{
				Log("VEG_DAYS < 30, LAI < PEAK_LAI, RESERVE > 0\n");
				/*just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
				 * and Barbaroux et al., 2002,
				the ratio is driven by the BIOME_BGC newStem:newLeaf ratio
				 */
				/*the fraction of reserve to allocate for foliage is re-computed for each of the BUD_BURST days
				 * sharing the daily remaining amount (taking into account respiration costs)of NSC */
				frac_to_foliage_stem = s->value[BIOMASS_RESERVE_CTEM] / s->counter[BUD_BURST_COUNTER];
				Log("recomputed fraction of reserve for foliage and stems = %g\n", frac_to_foliage_stem);

				s->counter[BUD_BURST_COUNTER] --;
				Log("++Remaining days for bud burst = %d\n", s->counter[BUD_BURST_COUNTER]);

				Log("++Lai before reserve allocation = %g\n", s->value[LAI]);
				Log("++Peak Lai = %g\n", s->value[PEAK_Y_LAI]);

				Log("Reserves pools = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);

				Log("ratio of reserve for foliage = %g% \n", (1.0 - s->value[STEM_LEAF_FRAC]) * 100 );
				Log("ratio of reserve for stem = %g% \n", s->value[STEM_LEAF_FRAC] * 100);

				s->value[BIOMASS_FOLIAGE_CTEM] += (frac_to_foliage_stem * (1.0 - s->value[STEM_LEAF_FRAC]));
				Log("Biomass foliage = %g\n", s->value[BIOMASS_FOLIAGE_CTEM]);

				s->value[BIOMASS_RESERVE_CTEM] -= frac_to_foliage_stem;

				s->value[BIOMASS_STEM_CTEM] += (frac_to_foliage_stem * s->value[STEM_LEAF_FRAC]);
				Log("Biomass stem = %g\n", s->value[BIOMASS_STEM_CTEM]);



				Log("++Reserves pools less foliage + stem = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);
				Log ("++Biomass foliage from reserves for initial LAI = %g \n", s->value[BIOMASS_FOLIAGE_CTEM]);

				/*for dominant layer with sunlit foliage*/
				if (c->heights[height].top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
				}

				/*check if re-transfer foliage biomass to reserve*/
				if (s->value[LAI] > s->value[PEAK_Y_LAI])
				{
					Log("LAI exceeds Peak Lai\n");

					/*for dominant layer with sunlit foliage*/
					if (c->heights[height].top_layer == c->heights[height].z)
					{
						biomass_for_peak_lai = ((s->value[PEAK_Y_LAI] * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))/ (s->value[SLAmkg]* GC_GDM)) / 1000;
					}
					/*for dominated shaded foliage*/
					else
					{
						biomass_for_peak_lai = ((s->value[PEAK_Y_LAI] * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))/ ((s->value[SLAmkg] * s->value[SLA_RATIO])* GC_GDM)) / 1000;
					}
					/*re-transfer mass to reserve*/
					s->value[BIOMASS_RESERVE_CTEM] += (biomass_for_peak_lai - s->value[BIOMASS_FOLIAGE_CTEM]);
					Log("++Reserves pools plus exceeding foliage biomass = %g tDM/area\n", s->value [BIOMASS_RESERVE_CTEM]);
					s->value[BIOMASS_FOLIAGE_CTEM] = biomass_for_peak_lai;
					/*recompute correct LAI*/
					/*for dominant layer with sunlit foliage*/
					if (c->heights[height].top_layer == c->heights[height].z)
					{
						s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
					}
					/*for dominated shaded foliage*/
					else
					{
						s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
					}
				}

				Log("++Lai from reserves = %g\n", s->value[LAI]);
			}
			if(s->value[LAI] > s->value[PEAK_Y_LAI])
			{
				Log("LAI > PEAK_Y_LAI\n");
				Log("Unused reserve = %g tDM/cell \n", s->value [BIOMASS_RESERVE_CTEM]);
				Log("++Lai = %g\n", s->value[LAI]);
			}
			else
			{
				Log("++Lai = %g\n", s->value[LAI]);
			}
		}
	}
	/*for evergreen species*/
	//fixme use reserve
	else
	{
		//Get biomass from reserves
		if (settings->time == 'm')
		{
			Log("\n--GET_MONTH_LAI--\n");

			if (s->counter[VEG_MONTHS]  == 1)
			{
				if(s->value[LAI] == 0)
				{
					/*for dominant layer with sunlit foliage*/
					if (c->heights[height].top_layer == c->heights[height].z)
					{
						s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
					}
					/*for dominated shaded foliage*/
					else
					{
						s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
					}
					Log("++Lai from foliage-reserve = %g\n", s->value[LAI]);
				}
			}
			else
			{
				Log("++Lai = %g\n", s->value[LAI]);
			}
		}
		else
		{
			Log("\n--GET_DAILY_LAI--\n");

			//todo make a better function that 'share' over a time the biomass_reserve and not only on one day


			//just a fraction of biomass reserve is used for foliage the other part is allocated to the stem (Magnani pers comm),
			//the ratio is driven by the BIOME_BGC newStem:newLeaf ratio

			if(s->value[LAI] == 0)
			{
				/*for dominant layer with sunlit foliage*/
				if (c->heights[height].top_layer == c->heights[height].z)
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * (s->value[SLAmkg] * GC_GDM);
				}
				/*for dominated shaded foliage*/
				else
				{
					s->value[LAI] = (s->value[BIOMASS_FOLIAGE_CTEM] * 1000) / (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell) * ((s->value[SLAmkg] * s->value[SLA_RATIO]) * GC_GDM);
				}
				Log("++Lai from foliage or reserve = %g\n", s->value[LAI]);
			}
			else
			{
				Log("++Lai from previous day = %g\n", s->value[LAI]);
			}
			if (s->value[LAI] > s->value[PEAK_Y_LAI])
			{
				Log("ATTENTION LAI > PEAK_Y_LAI!!! reallocate exceeding biomass \n");
			}
		}
	}
}


