
/*spatial version monthly partitionign_allocation.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"

/**/


//VERSION CURRENTLY USED
void SP_V_M_Get_Partitioning_Allocation_CTEM (SPECIES *const s,  CELL *const c, const MET_DATA *const met, int month, int z, int management,  float daylength,  int DaysInMonth, int years, int Veg_UnVeg)
{


    	//allocation parameter. their sum must be = 1

    float NPP_residual;  //NPP less LAI biomass
	float  s0Ctem = s->value[S0CTEM];
	float  r0Ctem = s->value[R0CTEM];
	float  f0Ctem = s->value[F0CTEM];
	float const omegaCtem = s->value[OMEGA_CTEM];

    float pS_CTEM;
	float pR_CTEM;
	float pF_CTEM;

	//determines the maximum amount of drymatter to allocate to foliage
              //CURRENTLY NOT USED
    float const epsilon = s->value[EPSILON_CTEM];      //in KgC/m^2)^-0.6  allocation parameter see also Frankfurt biosphere model
	float const kappa = s->value[KAPPA];               //(dimensionless) allocation parameter


    float Perc_fine;
	float Perc_coarse;


	float Light_trasm;
	float Par_over;
	float Monthly_solar_radiation;

    Monthly_solar_radiation = met[month].solar_rad * MOLPAR_MJ * DaysInMonth;
	Par_over = c->par - s->value[APAR];
	Light_trasm = Par_over / Monthly_solar_radiation;




    //7 May 2012
    //compute static ratio of allocation between fine and coarse root
    //deriving data from values reported for BIOME-BGC
    s->value[FR_CR] = (s->value[FINE_ROOT_LEAF] / s->value[COARSE_ROOT_STEM]) * (1.0 / s->value[STEM_LEAF]);
    //Log("Fine/Coarse root ratio = %g\n", s->value[FR_CR] );
    Perc_fine = s->value[FR_CR] / (s->value[FR_CR] + 1.0);
    //Log("Percentage of fine root against total root= %g %%\n", Perc_fine * 100 );
    Perc_coarse = 1- Perc_fine;
    //Log("Percentage of coarse root against total root= %g %%\n", Perc_coarse * 100 );


	s->value[BIOMASS_FOLIAGE_CTEM] = (met[month].lai / 1000) * ((s->value[CANOPY_COVER_DBHDC] * sizeCell)/s->value[SLAmkg]);
	Log("Biomass foliage from LAI-NDVI = %g tDM/cell resolution \n", s->value[BIOMASS_FOLIAGE_CTEM]);

	NPP_residual = s->value[NPP] - s->value[BIOMASS_FOLIAGE_CTEM];
	Log("NPP less foliage biomass = %g tDM/cell resolution \n", NPP_residual);

	//compute NPP for remaining pools
    pR_CTEM = (r0Ctem + (omegaCtem * ( 1.0 - s->value[F_SW] ))) / (1.0 + (omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
    //Log("Roots CTEM ratio layer %d = %g %%\n", z, pR_CTEM * 100);
    pS_CTEM = (s0Ctem + (omegaCtem * ( 1.0 - Light_trasm))) / (1.0 + ( omegaCtem * ( 2.0 - Light_trasm - s->value[F_SW] )));
    //Log("Stem CTEM ratio = %g %%\n", pS_CTEM * 100);

    //compute ratio relative to 100%

    float p_Tot_CTEM;

    p_Tot_CTEM = pR_CTEM + pS_CTEM;

    pR_CTEM = pR_CTEM / p_Tot_CTEM;
    Log("pR_CTEM = \n", pR_CTEM);

    pS_CTEM = 1 - pR_CTEM;
    Log("pS_CTEM = \n", pS_CTEM);


    // Biomass allocation

    s->value[DEL_ROOTS_TOT_CTEM] = NPP_residual * pR_CTEM;
    Log("BiomassRoots increment CTEM = %g tDM/ha\n", s->value[DEL_ROOTS_TOT_CTEM]);


    //7 May 2012
    //comnpute fine and coarse root biomass
    s->value[DEL_ROOTS_FINE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM]  * Perc_fine;
    Log("BiomassRoots into fine roots = %g tDM/ha\n", s->value[DEL_ROOTS_FINE_CTEM]);
    s->value[DEL_ROOTS_COARSE_CTEM] = s->value[DEL_ROOTS_TOT_CTEM] * Perc_coarse;
    Log("BiomassRoots into coarse roots = %g tDM/ha\n", s->value[DEL_ROOTS_COARSE_CTEM]);


    s->value[DEL_STEMS_CTEM] = NPP_residual *  pS_CTEM;
    Log("BiomassStem increment CTEM = %g tDM/ha\n", s->value[DEL_STEMS_CTEM]);

    //Total Stem Biomass
    //remove the part allocated to the branch and bark
    s->value[DEL_BB] = s->value[DEL_STEMS_CTEM] * s->value[FRACBB];
    //Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
    //Log("Branch and bark Biomass (del_BB)= %g tDM/ha\n", s->value[DEL_BB]);

    //allocation to stem
    s->value[DEL_STEMS_CTEM] -= s->value[DEL_BB];
    s->value[BIOMASS_STEM_CTEM] +=  s->value[DEL_STEMS_CTEM];
    Log("Stem Biomass (Ws) = %g tDM/ha\n", s->value[BIOMASS_STEM_CTEM]);



    //allocation to roots
    s->value[BIOMASS_ROOTS_TOT_CTEM] +=  s->value[DEL_ROOTS_TOT_CTEM];
    Log("Total Root Biomass (Wr TOT) = %g tDM/ha\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);

    s->value[BIOMASS_ROOTS_FINE_CTEM] += s->value[DEL_ROOTS_FINE_CTEM];
    Log("Fine Root Biomass (Wrf) = %g tDM/ha\n", s->value[BIOMASS_ROOTS_FINE_CTEM]);
    s->value[BIOMASS_ROOTS_COARSE_CTEM] += s->value[DEL_ROOTS_COARSE_CTEM];
    Log("Coarse Root Biomass (Wrc) = %g tDM/ha\n", s->value[BIOMASS_ROOTS_COARSE_CTEM]);


    Log("delta_F %d = %g \n", z, s->value[DEL_FOLIAGE_CTEM] );
    Log("delta_fR %d = %g \n", z, s->value[DEL_ROOTS_FINE_CTEM]);
    Log("delta_cR %d = %g \n", z, s->value[DEL_ROOTS_COARSE_CTEM]);
    Log("delta_S %d = %g \n", z, s->value[DEL_STEMS_CTEM]);
    Log("delta_Res %d = %g \n", z, s->value[DEL_RESERVE_CTEM]);
    Log("delta_BB %d = %g \n", z, s->value[DEL_BB]);









}
