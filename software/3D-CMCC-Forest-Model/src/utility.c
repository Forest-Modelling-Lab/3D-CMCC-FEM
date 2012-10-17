/*utility.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"



void Reset_cumulative_variables (CELL *const c, const int count)
{
    int height;
    int age;
    int species;

    for ( height = count - 1; height >= 0; height-- )
    {
        for (age = c->heights[height].ages_count - 1; age >= 0; age --)
        {
            for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
            {
			    /*reset cumulative values*/

			    c->canopy_cover_dominant = 0;
			    c->canopy_cover_dominated = 0;
			    c->canopy_cover_subdominated = 0;


                c->gpp = 0;
                c->npp = 0;
                c->stand_agb = 0;
                c->stand_bgb = 0;
                c->evapotranspiration = 0;
                c->heights[height].ages[age].species[species].counter[VEG_MONTHS] = 0;
                c->heights[height].ages[age].species[species].value[YEARLY_PHYS_MOD] = 0;
                c->heights[height].ages[age].species[species].value[POINT_GPP_g_C]  = 0;
                c->heights[height].ages[age].species[species].value[NPP]  = 0;
                c->heights[height].ages[age].species[species].value[YEARLY_GPP_G_C]  = 0;
                c->heights[height].ages[age].species[species].value[YEARLY_POINT_GPP_G_C]  = 0;
                c->heights[height].ages[age].species[species].value[YEARLY_NPP]  = 0;
                c->heights[height].ages[age].species[species].value[MONTHLY_EVAPOTRANSPIRATION]  = 0;
                c->heights[height].ages[age].species[species].counter[DEL_STEMS] = 0;
                c->heights[height].ages[age].species[species].counter[N_TREE_SAP] = 0;

                //INITIALIZE AVERAGE YEARLY MODIFIERS
                c->heights[height].ages[age].species[species].value[AVERAGE_F_VPD]  = 0;
                c->heights[height].ages[age].species[species].value[AVERAGE_F_T]  = 0;
                c->heights[height].ages[age].species[species].value[AVERAGE_F_SW]  = 0;
                c->heights[height].ages[age].species[species].value[F_AGE]  = 0;


                c->heights[height].ages[age].species[species].value[DEL_Y_WS] = 0;
                c->heights[height].ages[age].species[species].value[DEL_Y_WF] = 0;
                c->heights[height].ages[age].species[species].value[DEL_Y_WFR] = 0;
                c->heights[height].ages[age].species[species].value[DEL_Y_WCR] = 0;
                c->heights[height].ages[age].species[species].value[DEL_Y_WRES] = 0;
                c->heights[height].ages[age].species[species].value[DEL_Y_WR] = 0;
                c->heights[height].ages[age].species[species].value[DEL_Y_BB] = 0;
			}
		}
	}
}
