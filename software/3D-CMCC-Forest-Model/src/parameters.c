/*parameters.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"



void Print_parameters (SPECIES *const s, int species_count, int month, int years)
{
	int species;

	if (!month && !years)
	{
		for (species = 0; species < species_count; species++)
		{
			Log("\n\n\n\n***********************************************************************************\n");
			Log("VALUE LIST FOR THE MAIN PARAMETERS FOR = %s\n", s->name);
			Log("ALPHA= %f \n"
					"EPSILONgCMJ= %f \n"
					//"Y= %f \n"
					"K= %f \n"
					"LAIGCX= %f \n"
					"LAIMAXINTCPTN= %f \n"
					"MAXINTCPTN= %f \n"
					"SLA= %f \n"
					"FRACBB0= %f \n"
					"FRACBB1= %f \n"
					"TBB= %f \n"
					"RHOMIN= %f \n"
					"RHOMAX= %f \n"
					"TRHO= %f \n"
					"COEFFCOND= %f \n"
					"BLCOND= %f \n"
					"MAXCOND= %f \n"
					"MAXAGE= %f \n"
					"RAGE= %f \n"
					"NAGE= %f \n"
					"GROWTHTMIN= %f \n"
					"GROWTHTMAX= %f \n"
					"GROWTHTOPT= %f \n"
					"GROWTHSTART= %f \n"
					"GROWTHEND= %f \n"
					"STEMCONST= %f \n"
					"STEMPOWER_A= %f \n"
					"STEMPOWER_B= %f \n"
					"STEMPOWER_C= %f \n",
					s->value[ALPHA],
					s->value[EPSILONgCMJ],
					//s->value[Y],
					s->value[K],
					s->value[LAIGCX],
					s->value[LAIMAXINTCPTN],
					s->value[MAXINTCPTN],
					s->value[SLA],
					s->value[FRACBB0],
					s->value[FRACBB1],
					s->value[TBB],
					s->value[RHOMIN],
					s->value[RHOMAX],
					s->value[TRHO],
					s->value[COEFFCOND],
					s->value[BLCOND],
					s->value[MAXCOND],
					s->value[MAXAGE],
					s->value[RAGE],
					s->value[NAGE],
					s->value[GROWTHTMIN],
					s->value[GROWTHTMAX],
					s->value[GROWTHTOPT],
					s->value[GROWTHSTART],
					s->value[GROWTHEND],
					s->value[STEMCONST],
					STEMPOWER_A,
					STEMPOWER_B,
					STEMPOWER_C
			);


		}

	}


}
