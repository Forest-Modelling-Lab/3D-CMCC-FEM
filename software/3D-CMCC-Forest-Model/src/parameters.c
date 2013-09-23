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
			Log("ALPHA= %g \n"
					"EPSILONgCMJ= %g \n"
					//"Y= %g \n"
					"K= %g \n"
					"LAIGCX= %g \n"
					"LAIMAXINTCPTN= %g \n"
					"MAXINTCPTN= %g \n"
					"SLA= %g \n"
					"FRACBB0= %g \n"
					"FRACBB1= %g \n"
					"TBB= %g \n"
					"RHOMIN= %g \n"
					"RHOMAX= %g \n"
					"TRHO= %g \n"
					"COEFFCOND= %g \n"
					"BLCOND= %g \n"
					"MAXCOND= %g \n"
					"MAXAGE= %g \n"
					"RAGE= %g \n"
					"NAGE= %g \n"
					"GROWTHTMIN= %g \n"
					"GROWTHTMAX= %g \n"
					"GROWTHTOPT= %g \n"
					"GROWTHSTART= %g \n"
					"GROWTHEND= %g \n"
					"PFS2= %g \n"
					"PFS20= %g \n"
					"PRX= %g \n"
					"PRN= %g \n"
					"STEMCONST= %g \n"
					"STEMPOWER_A= %g \n"
					"STEMPOWER_B= %g \n"
					"STEMPOWER_C= %g \n",
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
					s->value[PFS2],
					s->value[PFS20],
					s->value[PRX],
					s->value[PRN],
					s->value[STEMCONST],
					STEMPOWER_A,
					STEMPOWER_B,
					STEMPOWER_C
			);


		}

	}


}
