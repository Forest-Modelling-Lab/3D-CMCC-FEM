/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t *g_settings;
extern logger_t* g_log;

/**/
int Establishment_LPJ (cell_t *const c, species_t *const s)
{

	//double FProCov;   //LPJ Foliage Projective  Cover for Seed Establishment
	double EstabRate;   //Seed establishment rate Under Dominant Canopy
	static int Nsapling;

	logger(g_log, "\n--LPJ ESTABLISHMENT--\n");

	logger(g_log, "Intrinsic Germinability Rate = %f %% \n", s->value[GERMCAPACITY] * 100);

	logger(g_log, "Annual Number of seeds using LPJ  = %d seeds/ha/year\n", s->counter[N_SEED]);

	//da ricontrollare per vedere quale Lai prende di quale layer e semmai non utilizzare il FProCov




	/*
	   LightAbsorb = 1 -  (exp(- s->value[K] * s->value[LAI]));

	   logger(g_log, "Light Absorb in Dominant Canopy = %f \n", LightAbsorb);
	   logger(g_log, "Canopy Cover in  Dominant layer with DBHDC function = %f \n", canopy_cover_dominant);
	   FProCov = canopy_cover_dominant * LightAbsorb;
	   logger(g_log, "Foliage Projective Cover = %f \n", FProCov);


	   logger(g_log, "LPJ Fractional Projective Cover FPC = %f \n", FProCov);

	   EstabRate = s->value[GERMCAPACITY] * (1 - exp((-5) * (1 - FProCov))) * (1 - FProCov);
	   logger(g_log, "Seed Establishment Rate from LPJ = %f saplings/m^2 \n", EstabRate);
	 */

	EstabRate = (s->value[GERMCAPACITY] * (1 - exp((-5) * (1 - c->par_for_establishment))) * (1 - c->par_for_establishment)) / s->counter[N_TREE];
	logger(g_log, "Light absorb for establishment = %f \n", c->par_for_establishment);
	logger(g_log, "Seed Establishment Rate from LPJ = %f saplings/m^2 \n", EstabRate);



	Nsapling = (int)(s->counter[N_SEED] * EstabRate);
	logger(g_log, "Annual Number of Saplings per hectare using LPJ = %d Saplings/year hectare\n", Nsapling);
	logger(g_log, "Annual Number of Saplings using LPJ = %f Saplings/year m^2\n", (double) Nsapling / g_settings->sizeCell );
	logger(g_log, "Percentage of seeds survived using LPJ = %f %% seeds/year hectare\n", ((double)Nsapling * 100)/(double)s->counter[N_SEED] );


	return Nsapling;
}
