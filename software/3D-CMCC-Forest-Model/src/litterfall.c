/*litterfall.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"



/*not used*/
/*
void Get_litterfall_deciduous (SPECIES *const s)
{
	//height->ages[age].species[species].value[BIOME_LITTERFALL_FRAC]
	s->counter[MONTH_FRAC_FOLIAGE_REMOVE] = floor ( 0.2 * s->counter[MONTH_VEG_FOR_LITTERFALL_RATE]);

	Log("Months of litterfall for deciduous = %d \n", s->counter[MONTH_FRAC_FOLIAGE_REMOVE]);

}
*/
/*
void Get_litterfall_evergreen (HEIGHT *height, double oldWf, const int age, const int species, int years)
{
	double gammaF;


	Log("*LITTERFALL*\n");

	if ( height->ages[age].species[species].value[PHENOLOGY] == 0.1 || height->ages[age].species[species].value[PHENOLOGY] == 0.2)
	{
		Log("DECIDUOUS POPULATION\n");

		//LITTERFALL FOR DECIDUOUS
		// pedita di foglie che cadono nella lettiera
		//BISOGNA PRENDERE QUELLE DELL'ANNO PRECEDENTE
		height->ages[age].species[species].value[DAILY_DEL_LITTER] = oldWf;
		//Log("Foliage to litter of previuos year = %f tDM/ha \n", oldWf);
		Log("Yearly Foliage Biomass to litter from deciduous population = %f tDM/area\n", height->ages[age].species[species].value[DAILY_DEL_LITTER]);
	}
	else
	{
		Log("EVERGREEN POPULATION\n");

		Log("OldWf = %f\n", oldWf);
		height->ages[age].species[species].value[DAILY_DEL_LITTER] = gammaF * oldWf;
		Log("Yearly Foliage Biomass to litter from evergreen population = %f tDM/area\n", height->ages[age].species[species].value[DAILY_DEL_LITTER]);

		height->ages[age].species[species].value[BIOMASS_FOLIAGE] +=  (oldWf - height->ages[age].species[species].value[DAILY_DEL_LITTER]);
		Log("Yearly Foliage Biomass at the end of month less Litterfall (Wf + oldWf) in tDM/ha = %f\n", height->ages[age].species[species].value[BIOMASS_FOLIAGE]);

	}
}
*/
/*not used*/
/*
extern void Get_litterfall_evergreen_CTEM (SPECIES *const s)
{
	static double normal_daily_turnover_rate;
	static double normal_monthly_turnover_rate;

	Log("\n\n-(CTEM) LITTERFALL RATE-\n");

	if (settings->time == 'd')
	{
		normal_daily_turnover_rate = s->value[LEAF_LIFE_SPAN]* (1.0 / 365.0);
		Log("normal_daily_turnover_rate = %f\n", normal_daily_turnover_rate);
		s->value[LITTERFALL_RATE] = (s->value[F_SW] * s->value[F_T] * normal_daily_turnover_rate);
		//Log("*LITTERFALL_RATE_FROM_CTEM = %f\n\n", s->value[LITTERFALL_RATE]);

	}
	else
	{
		normal_monthly_turnover_rate = s->value[LEAF_LIFE_SPAN]/12.0;
		s->value[LITTERFALL_RATE] = (s->value[F_SW] * s->value[F_T] * normal_monthly_turnover_rate);
		//Log("*LITTERFALL_RATE_FROM_CTEM = %f\n\n", s->value[LITTERFALL_RATE]);
	}
}
*/
