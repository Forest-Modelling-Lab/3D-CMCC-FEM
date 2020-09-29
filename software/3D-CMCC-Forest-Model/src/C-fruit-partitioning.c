/* C-fruit-partitioning-allocation.c */

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"
#include "lai.h"
#include "turnover.h"
#include "dendometry.h"
#include "biomass.h"

#if 0
int M_Fruit_Allocation_Logistic_Equation(age_t *const a, const int species)
{
	/*USING A LOGISTIC EQUATION*/
	static int NumberSeed;                  //Number of Seeds per tree
	static int PopNumberSeeds;              //Number of Seeds per Population
	static int MaxSeed = 2000;              //Maximum seeds number
	static int OptSexAge = 100;             //Age at maximum seeds production
	static int MinSexAge = 20;              //Minimum age for sex maturity
	static double WseedLE ;                  //Weight of seeds of population from Logistic Equation

	species_t *s;
	s = &a->species[species];

	//Log("------LOGISTIC EQUATION FRUIT ALLOCATION------\n");



	NumberSeed = (MaxSeed/ (1 + OptSexAge * exp (-0.1 * (a->value - MinSexAge))));
	//Log("Annual Number of Seeds for Tree from Logistic Equation = %d seeds/tree/year\n", NumberSeed);

	PopNumberSeeds = NumberSeed * s->counter[N_TREE];
	//Log("Annual Number of Seeds for Population from Logistic Equation = %d seeds/area/year\n", PopNumberSeeds);

	WseedLE = ((double)PopNumberSeeds * s->value[WEIGHTSEED]) / 1000000 /* to convert in tonnes*/;
	//Log("Biomass for Seed from Logistic Equation = %f tDM/area\n", WseedLE);
	//Log("Fraction of Biomass allocated for Seed from Logistic Equation = %f tDM/area\n", WseedLE);
	//Log("Fraction of NPP allocated for Seed from Logistic Equation = %.4g %%\n", (WseedLE * 100) /s->value[YEARLY_NPP_tDM] );

	return NumberSeed;
}
#endif
/**/
#if 0
int M_Fruit_Allocation_TREEMIG (age_t *const a, const int species)
{
	static int NumberSeed;
	static double heigthdependence;
	static double WseedT;            //height dependence factor

	species_t *s;
	s = &a->species[species];

	//Log("------TREEMIG FRUIT ALLOCATION------\n");

	heigthdependence = s->value[LAI_PROJ] / s->value[PEAK_LAI_PROJ];
	//Log("heigthdependence = %f \n", heigthdependence);

	//numero semi prodotti
	NumberSeed = (double)s->counter[N_TREE] * s->value[MAXSEED] * heigthdependence * 0.51 *
			( 1 + sin((2 * Pi * (double)a->value ) / s->value[MASTSEED]));
	//Log("Nseed per cell at the End of the This Year = %d seeds per cell\n", NumberSeed);

	//Biomassa allocata nei semi in tDM/area
	WseedT = (double)NumberSeed * s->value[WEIGHTSEED] / 1000000;  //per convertire in tonnellate biomassa allocata per i semi
	//Log("Seeds Biomass per cell at the End of the This Year = %f tonnes of seeds/area \n", WseedT);
	//Log("Fraction of NPP allocated for Seed from TREEMIG = %.4f %%\n", (WseedT * 100) /s->value[YEARLY_NPP_tDM] );

	return NumberSeed;

}
#endif


