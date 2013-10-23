/*litterfall.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

extern void Get_litter (CELL *c, SPECIES *const s, const int years)
{
	Log("****LITTER BIOMASS****\n");
	//inserire anche la biomassa dei semi non germogliati

    if (!years)
      {
          c->litter += s->value[DEL_LITTER] + site->initialLitter;
          Log("Total Litter = %g tDM/area\n", c->litter);
      }
      else
      {
          c->litter += s->value[DEL_LITTER];
          Log("Total Litter = %g tDM/area\n", c->litter);
      }

	//reset
	s->value[DEL_LITTER] = 0;
}


void Get_litterfall_deciduous (SPECIES *const s)
{
	//height->ages[age].species[species].value[BIOME_LITTERFALL_FRAC]
	s->counter[MONTH_FRAC_FOLIAGE_REMOVE] = floor ( 0.2 * s->counter[MONTH_VEG_FOR_LITTERFALL_RATE]);

	Log("Months of litterfall for deciduous = %d \n", s->counter[MONTH_FRAC_FOLIAGE_REMOVE]);

}


void Get_litterfall_evergreen (HEIGHT *height, float oldWf, const int age, const int species, int years)
{
	float gammaF;


	Log("*LITTERFALL*\n");

	if ( height->ages[age].species[species].value[PHENOLOGY] == 0.1 || height->ages[age].species[species].value[PHENOLOGY] == 0.2) /*for deciduous*/
	{
		Log("DECIDUOUS POPULATION\n");

		//LITTERFALL FOR DECIDUOUS
		// pedita di foglie che cadono nella lettiera
		//BISOGNA PRENDERE QUELLE DELL'ANNO PRECEDENTE
		height->ages[age].species[species].value[DEL_LITTER] = oldWf;
		//Log("Foliage to litter of previuos year = %g tDM/ha \n", oldWf);
		Log("Yearly Foliage Biomass to litter from deciduous population = %g tDM/area\n", height->ages[age].species[species].value[DEL_LITTER]);
	}
	else
	{
		Log("EVERGREEN POPULATION\n");  /*for evergreen*/

		/*Litterfall rate*/
/*
		gammaF = height->ages[age].species[species].value[GAMMAFX] * height->ages[age].species[species].value[GAMMAF0] /
				(height->ages[age].species[species].value[GAMMAF0] +
						(height->ages[age].species[species].value[GAMMAFX] -
								height->ages[age].species[species].value[GAMMAF0]) *
								exp(-12 * log(1 + height->ages[age].species[species].value[GAMMAFX] /
										height->ages[age].species[species].value[GAMMAF0]) *
										height->ages[age].value /
										height->ages[age].species[species].value[TGAMMAF]));
		//Log("Litterfall rate = %g\n", gammaF);
*/

		Log("OldWf = %g\n", oldWf);
		height->ages[age].species[species].value[DEL_LITTER] = gammaF * oldWf;
		Log("Yearly Foliage Biomass to litter from evergreen population = %g tDM/area\n", height->ages[age].species[species].value[DEL_LITTER]);

		height->ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM] +=  (oldWf - height->ages[age].species[species].value[DEL_LITTER]);
		Log("Yearly Foliage Biomass at the end of month less Litterfall (Wf + oldWf) in tDM/ha = %g\n", height->ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM]);

	}


}

extern void Get_litterfall_evergreen_CTEM (SPECIES *const s)
{
	static float normal_daily_turnover_rate;
	static float normal_monthly_turnover_rate;

	Log("\n\n-(CTEM) LITTERFALL RATE-\n");

	if (settings->time == 'd')
	{
		normal_daily_turnover_rate = s->value[LEAF_LIFE_SPAN]* (1.0 / 365.0);
		Log("normal_daily_turnover_rate = %g\n", normal_daily_turnover_rate);
		s->value[LITTERFALL_RATE] = (s->value[F_SW] * s->value[F_T] * normal_daily_turnover_rate);
		//Log("*LITTERFALL_RATE_FROM_CTEM = %g\n\n", s->value[LITTERFALL_RATE]);

	}
	else
	{
		normal_monthly_turnover_rate = s->value[LEAF_LIFE_SPAN]/12.0;
		s->value[LITTERFALL_RATE] = (s->value[F_SW] * s->value[F_T] * normal_monthly_turnover_rate);
		//Log("*LITTERFALL_RATE_FROM_CTEM = %g\n\n", s->value[LITTERFALL_RATE]);
	}
}
