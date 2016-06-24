/*partitionign_allocation.c*/

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


/**/

void M_Fruit_Allocation_LPJ (species_t *const s, int z, int years, double Yearly_Rain, double canopy_cover_dominant )
{
	//static double WseedTree;    //fruit biomass per Tree
	static int NseedTree;      //Number of fruits per Tree

	//static double FProCov;   //LPJ Foliage Projective  Cover for Seed Establishment
	//static double LightAbsorb;
	//static double EstabRate;   //Seed establishment rate Under Dominant Canopy
	//static int Nsapling;      //Number of saplings Under and Outside Dominant Canopy


	//Log("**SEEDS-ALLOCATION**\n");

	//Log("\n--LPJ FRUIT ALLOCATION--\n");





	//il 10% lo do ai frutti
	//fraction of total NPP to Fruit compart
	//biomass to seeds
	s->value[W_SEED] = s->value[YEARLY_NPP_tDM] * s->value[FRACFRUIT];
	//Log("Costant Fraction Rate of Annual NPP for Fruit Production using LPJ = %f %%\n", s->value[FRACFRUIT] * 100);
	//Log("Annual NPP to Seeds Biomass Compart = %f tDM/area/year\n", s->value[W_SEED]);


	//WseedTree = s->value[W_SEED] / s->counter[N_TREE];



	//Log("Annual Biomass for Seeds Compart for Tree  = %f in tDM/tree/year\n", WseedTree);
	//Log("Annual Biomass for Seeds Compart for Tree  = %f in Kg/tree/year\n", WseedTree * 1000);
	//Number of seeds from tDM to grammes
	s->counter[N_SEED] = (s->value[W_SEED] * 1000000)/ s->value[WEIGHTSEED];
	//Log("Annual Number of seeds using LPJ  = %d seeds/area/year\n", s->counter[N_SEED]);
	/*
	   Log("NSEED %f\n", s->counter[N_SEED]);
	   Log("NTREE %f\n", s->counter[N_TREE]);
	   NseedTree = s->counter[N_SEED] / s->counter[N_TREE];
	 */
	//Log("Annual Number of Seeds Produced for Tree using LPJ = %d seeds/area/year\n", s->counter[N_SEED]);
	//Log("Annual Number of Seeds Produced for Tree using LPJ = %d seeds/tree/year\n", NseedTree);

	/*
	   Log("Intrinsic Germinability Rate = %f %% \n", s->value[GERMCAPACITY] * 100);


	   if (Yearly_Rain > s->value[MINRAIN])
	   {

	   Log("Annual Number of seeds using LPJ  = %d seeds/area/year\n", s->counter[N_SEED]);

	//da ricontrollare per vedere quale Lai prende di quale layer e semmai non utilizzare il FProCov





	LightAbsorb = 1 -  (exp(- s->value[K] * s->value[LAI]));

	Log("Light Absorb in Dominant Canopy = %f \n", LightAbsorb);
	FProCov = canopy_cover_dominant * LightAbsorb;
	Log("Canopy Cover in  Dominant layer with DBHDC function = %f \n", canopy_cover_dominant);

	Log("LPJ Fractional Projective Cover FPC = %f \n", FProCov);


	EstabRate = s->value[GERMCAPACITY] * (1 - exp((-5) * (1 - FProCov))) * (1 - FProCov);
	Log("Seed Establishment Rate from LPJ = %f saplings/m^2 \n", EstabRate);

	Nsapling = s->counter[N_SEED] * EstabRate ;
	Log("Annual Number of Saplings per hectare using LPJ = %d Saplings/year hectare\n", Nsapling);
	Log("Annual Number of Saplings using LPJ = %f Saplings/year m^2\n", (double) Nsapling / SIZECELL );
	Log("Percentage of seeds survived using LPJ = %f %% seeds/year hectare\n", ((double)Nsapling * 100)/(double)s->counter[N_SEED] );
	//Log("------THE MODEL STILL DOESN'T TAKE INTO ACCOUNT THE PRESENTS OF SUBDOMINANTS SPECIES!!!!------\n");
	}
	else
	{
	Log("Not enough Rain for Seeds Establishment!!!!\n");
	}

	//s->value[YEARLY_NPP] = s->value[YEARLY_NPP] - s->value[W_SEED];
	//Log("Annual NPP  less Seeds Biomass using LPJ = %f tDM/area/year\n", s->value[YEARLY_NPP]);
	 */

}


/**/
int M_Fruit_Allocation_Logistic_Equation (species_t *const s, age_t *const a)
{
	/*USING A LOGISTIC EQUATION*/
	static int NumberSeed;                  //Number of Seeds per tree
	static int PopNumberSeeds;              //Number of Seeds per Population
	static int MaxSeed = 2000;              //Maximum seeds number
	static int OptSexAge = 100;             //Age at maximum seeds production
	static int MinSexAge = 20;              //Minimum age for sex maturity
	static double WseedLE ;                  //Weight of seeds of population from Logistic Equation

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
/**/

int M_Fruit_Allocation_TREEMIG (species_t *const s, age_t *const a)
{
	static int NumberSeed;
	static double heigthdependence;
	static double WseedT;            //height dependence factor

	//Log("------TREEMIG FRUIT ALLOCATION------\n");

	heigthdependence = s->value[LAI] / s->value[LAIGCX] ;//sarebbe Lai  / Lai max;
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



