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
#include "CN-allocation.h"
#include "C-fruit-partitioning.h"

extern logger_t* g_debug_log;
extern settings_t* g_settings;

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
	//Log("Fraction of NPP allocated for Seed from Logistic Equation = %.4g %%\n", (WseedLE * 100) /s->value[YEARLY_NPP_tC] );

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
            /*************************NEW MODULE OF SEEDS PRODUCTION SIMILAR TO PASCIONA PROCESS**********************************/

// Saponaro 10/2022
#if 0

int Fruit_to_seeds_function (cell_t *const c, const int layer, const age_t *const a, species_t *const s, const int year)
{


 long int NumberSeed = 0.;               //Number of Seeds per tree
 int NumberFruit = 0.;                   //Number of Fruit
 double carbon_tank = 0.;                //Carbon accumulation for seeds
 long int tank_seeds = 0.;               // Tank accumulation seeds

//Variable inizialized in matrix.h
 carbon_tank = s->value[CARBON_TANK];
 tank_seeds = s->counter[TANK_SEEDS];


     //***************************************SEEDS PRODUCTION**********************************************//

  if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365 )) {

   if (a->value > s->value[SEXAGE]) //MAX_FRUIT_C expressed as (tC/cell/year)
     {
       //Cumulate carbon from fruit allocation every year in tank
       s->value[CARBON_TANK] += (s->value[MAX_FRUIT_C] * 1000000); // convert tC(fruit) to gC/cell/year(fruit)


        if ( s->value[CARBON_TANK] >= s->value[MAX_CS]) {

            //Calculates the number of fruit per stand based on species specific weight of fruit(nfruit/ha)
            NumberFruit = (s->value[CARBON_TANK] / s->value[WEIGHTFRUIT]);


            // Calculates the number of seed per stand based on species specific weight of seed (nseed/tree/year)
            NumberSeed = (NumberFruit / s->value[WEIGHTSHELL]);


            //Number of seed per population (nseed/cell/year)
            s->counter[N_SEED] = NumberSeed;


            //When carbon tank for seed is used carbon tank will empty
            s->value[CARBON_TANK] = 0.;

            //Accumulation of seeds over years
            s->counter[TANK_SEEDS] += s->counter[N_SEED];


       } else {

       NumberFruit = 0.;
     //NumberSeed = 0.;
       s->counter[N_SEED] = 0.;

     }

   }
 //printf("MAX_FRUIT_C = %f\n", s->value[MAX_FRUIT_C]);
 //printf("CARBON_TANK = %f\n", s->value[CARBON_TANK]);
 //printf("Number fruit = %d\n", NumberFruit);
 //printf("Number seed = %ld\n", NumberSeed);
 //printf("N_SEED = %d\n", s->counter[N_SEED]);
 //printf("TANK_SEEDS = %ld\n", (long)tank_seeds);
    }

  return 0;
}

#endif

       //***************************************SEEDS PRODUCTION EVERY YEAR WRONG**********************************************//

#if 0

int Fruit_to_seeds_function (cell_t *const c, const int layer, const age_t *const a, species_t *const s, const int year) {


 long int NumberSeed = 0.;                    //Number of Seeds
 long int NumberFruit = 0.;                   //Number of Fruit
 double carbon_tank = 0.;                     //Carbon for seeds


   if (a->value >= s->value[SEXAGE]) {

               carbon_tank = (s->value[MAX_FRUIT_C] * 1000000); // convert tC(fruit) to gC/cell/year(fruit); MAX_FRUIT_C expressed as (tC/cell/year)
              //printf("MAX_FRUIT_C = %f\n", s->value[MAX_FRUIT_C]);
              //printf("Carbontank = %f\n", carbon_tank);
              //printf("AGE = %d\n", a->value);

               //Calculates the number of fruit per stand based on species specific weight of fruit(nfruit/ha)
               NumberFruit = (carbon_tank / s->value[WEIGHTFRUIT]);
               //printf("Number fruit = %ld\n", NumberFruit);

               // Calculates the number of seed per stand based on species specific weight of seed (nseed/tree/year)
               NumberSeed = (NumberFruit / s->value[WEIGHTSHELL]);
               //printf("Number seed = %ld\n", NumberSeed);


               //Number of seed per population (nseed/cell/year)
               s->counter[N_SEED] = NumberSeed;
               //printf("N_SEED = %ld\n", s->counter[N_SEED]);

             } else {

             NumberFruit = 0.;
            NumberSeed = 0.;
        }

       if ( c->doy == ( IS_LEAP_YEAR( c->years[year].year ) ? 366 : 365 )) {

       //Accumulation of seeds over years
       s->counter[TANK_SEEDS] += s->counter[N_SEED];
       //printf("TANK_SEED = %ld\n", s->counter[TANK_SEEDS]);

     }

  return 0;
}

#endif
            //***************************************SEEDS PRODUCTION EVERY YEAR**********************************************//

#if 1

int Fruit_to_seeds_function (cell_t *const c, const age_t *const a, species_t *const s, const int day, const int month, const int year) {


 long int NumberSeed = 0;                    //Number of Seeds
 long int NumberFruit = 0;                   //Number of Fruit
 double carbon_tank = 0.;                     //Carbon for seeds


           if (a->value >= s->value[SEXAGE]) {


               carbon_tank = (s->value[MAX_FRUIT_C] * 1e6); //Convert tC(fruit) to gC/cell(fruit);
              //printf("MAX_FRUIT_C = %f\n", s->value[MAX_FRUIT_C]);
              //printf("Carbontank = %f\n", carbon_tank);
              //printf("AGE = %d\n", a->value);

               //Calculates the number of fruit per stand based on species specific weight of fruit(nfruit/ha)
               NumberFruit = (carbon_tank / s->value[WEIGHTFRUIT]);
               //printf("Number fruit = %ld\n", NumberFruit);

               //Calculates the number of seed per stand based on species specific weight of seed (nseed/tree/year)
               NumberSeed = (NumberFruit / s->value[WEIGHTSHELL]);
               //printf("Number seed = %ld\n", NumberSeed);

               //Number of seed per species (nseed/cell/year)
               s->counter[N_SEED] = NumberSeed;
               //printf("N_SEED = %ld\n", s->counter[N_SEED]);


             } else {

             NumberFruit = 0;

             NumberSeed = 0;
        }
/*
        if ( c->doy == ( IS_LEAP_YEAR ( c->years[year].year ) ? 366 : 365)) {

       s->counter[TANK_SEEDS] += NumberSeed;
       s->counter[N_SEED] = 0.;

    }
*/

  return 0;
}
#endif // 1
