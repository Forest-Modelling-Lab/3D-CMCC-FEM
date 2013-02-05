/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

/**/
int Get_Establishment_LPJ (SPECIES *const s, float Light_Absorb_for_establishment, float  canopy_cover_dominant)
{

	//float FProCov;   //LPJ Foliage Projective  Cover for Seed Establishment
	float EstabRate;   //Seed establishment rate Under Dominant Canopy
	static int Nsapling;

	Log("\n--LPJ ESTABLISHMENT--\n");

	Log("Intrinsic Germinability Rate = %g %% \n", s->value[GERMCAPACITY] * 100);

	Log("Annual Number of seeds using LPJ  = %d seeds/ha/year\n", s->counter[N_SEED]);

	//da ricontrollare per vedere quale Lai prende di quale layer e semmai non utilizzare il FProCov




	/*
	   LightAbsorb = 1 -  (exp(- s->value[K] * s->value[LAI]));

	   Log("Light Absorb in Dominant Canopy = %g \n", LightAbsorb);
	   Log("Canopy Cover in  Dominant layer with DBHDC function = %g \n", canopy_cover_dominant);
	   FProCov = canopy_cover_dominant * LightAbsorb;
	   Log("Foliage Projective Cover = %g \n", FProCov);


	   Log("LPJ Fractional Projective Cover FPC = %g \n", FProCov);

	   EstabRate = s->value[GERMCAPACITY] * (1 - exp((-5) * (1 - FProCov))) * (1 - FProCov);
	   Log("Seed Establishment Rate from LPJ = %g saplings/m^2 \n", EstabRate);
	 */

	EstabRate = (s->value[GERMCAPACITY] * (1 - exp((-5) * (1 - Light_Absorb_for_establishment))) * (1 - Light_Absorb_for_establishment)) / s->counter[N_TREE];
	Log("Light absorb for establishment = %g \n", Light_Absorb_for_establishment);
	Log("Seed Establishment Rate from LPJ = %g saplings/m^2 \n", EstabRate);



	Nsapling = s->counter[N_SEED] * EstabRate ;
	Log("Annual Number of Saplings per hectare using LPJ = %d Saplings/year hectare\n", Nsapling);
	Log("Annual Number of Saplings using LPJ = %g Saplings/year m^2\n", (float) Nsapling / settings->sizeCell );
	Log("Percentage of seeds survived using LPJ = %g %% seeds/year hectare\n", ((float)Nsapling * 100)/(float)s->counter[N_SEED] );


	return Nsapling;
}
