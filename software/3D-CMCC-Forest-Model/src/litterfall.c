/*litterfall.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"

extern void Get_litterfall (CELL *c, SPECIES *const s, const int years)
{
    if (!years)
      {
          c->litter += s->value[DEL_LITTER] + site->initialLitter;
          Log("Total Litter = %g tDM/ha\n", c->litter);
      }
      else
      {
          c->litter += s->value[DEL_LITTER];
          Log("Total Litter = %g tDM/ha\n", c->litter);
      }
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

	if ( height->ages[age].species[species].phenology == 0) /*for deciduous*/
	{
		Log("DECIDUOUS POPULATION\n");

		//LITTERFALL FOR DECIDUOUS
		// pedita di foglie che cadono nella lettiera
		//BISOGNA PRENDERE QUELLE DELL'ANNO PRECEDENTE
		height->ages[age].species[species].value[DEL_LITTER] = oldWf;
		//Log("Foliage to litter of previuos year = %g tDM/ha \n", oldWf);
		Log("Yearly Foliage Biomass to litter from deciduous population = %g tDM/ha\n", height->ages[age].species[species].value[DEL_LITTER]);
	}
	else
	{
		Log("EVERGREEN POPULATION\n");  /*for evergreen*/

		/*Litterfall rate*/

		gammaF = height->ages[age].species[species].value[GAMMAFX] * height->ages[age].species[species].value[GAMMAF0] /
				(height->ages[age].species[species].value[GAMMAF0] +
						(height->ages[age].species[species].value[GAMMAFX] -
								height->ages[age].species[species].value[GAMMAF0]) *
								exp(-12 * log(1 + height->ages[age].species[species].value[GAMMAFX] /
										height->ages[age].species[species].value[GAMMAF0]) *
										height->ages[age].value /
										height->ages[age].species[species].value[TGAMMAF]));
		//Log("Litterfall rate = %g\n", gammaF);


		Log("OldWf = %g\n", oldWf);
		height->ages[age].species[species].value[DEL_LITTER] = gammaF * oldWf;
		Log("Yearly Foliage Biomass to litter from evergreen population = %g tDM/ha\n", height->ages[age].species[species].value[DEL_LITTER]);

		height->ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM] +=  (oldWf - height->ages[age].species[species].value[DEL_LITTER]);
		Log("Yearly Foliage Biomass at the end of year less Litterfall (Wf + oldWf) in tDM/ha = %g\n", height->ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM]);

	}


}
