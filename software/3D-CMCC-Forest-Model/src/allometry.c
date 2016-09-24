/* allometry.c */
#include "allometry.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "constants.h"
#include "logger.h"

extern logger_t* g_log;

void allometry_power_function(cell_t *const c)
{
	int height;
	int dbh;
	int age;
	int species;

	int mod_age;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	logger(g_log,"\nAllometry Power Function\n");

	for ( height = 0; height < c->heights_count; ++height )
	{
		h = &c->heights[height];

		for ( dbh = 0; dbh < h->dbhs_count; ++dbh )
		{
			d =  &h->dbhs[dbh];

			for ( age = 0; age < d->ages_count; ++age )
			{
				a = &d->ages[age];

				for ( species = 0; species < a->species_count; ++species )
				{
					s = &a->species[species];

					logger(g_log, "Species = %s\n", s->name);

					logger(g_log, "Age = %d\n", a->value);

					/* note: ISIMIP special case */
					if (a->value == 0)
					{
						mod_age = 1;
					}
					else
					{
						mod_age = a->value;
					}
					logger(g_log, "Age (used in function) = %d\n", mod_age);

					s->value[MASS_DENSITY] = s->value[RHOMAX] + (s->value[RHOMIN] - s->value[RHOMAX]) * exp(-ln2 * (mod_age / s->value[TRHO]));
					logger(g_log, "-Mass Density = %g\n", s->value[MASS_DENSITY]);

					if ( d->value < 9 )
					{
						s->value[STEMCONST] = pow (e, -1.6381);
					}
					else if ( d->value > 9 && d->value < 15 )
					{
						s->value[STEMCONST] = pow (e, -3.51+1.27* s->value[MASS_DENSITY]);
					}
					else
					{
						s->value[STEMCONST] = pow (e, -3.51+1.27*s->value[MASS_DENSITY]);
					}
					logger(g_log, "-Stem const = %f\n", s->value[STEMCONST]);
				}
			}
		}
	}
}

////not used
//void Get_allometry (species_t* const s, age_t* const a, int years)
//{
//	logger(g_log, "\n*********ALLOMETRY FUNCTION *********\n");
//	/*
//	   logger(g_log, "-Chapman-Richards allometry function-\n");
//	//tree height from champan-richards function
//	if (!years)
//	{
//	s->value[INITIAL_TREE_HEIGHT_CR] = 1.3 + s->value[CRA] * pow (1 - exp ( - s->value[CRB] * s->value[AVDBH]) , s->value[CRC]);
//	logger(g_log, "Initial Tree Height from Chapman-Richards function = %f m\n", s->value[INITIAL_TREE_HEIGHT_CR]);
//	//control
//	if (s->value[INITIAL_TREE_HEIGHT_CR] > s->value[HMAX])
//	{
//	s->value[INITIAL_TREE_HEIGHT_CR] = s->value[HMAX];
//	logger(g_log, "Initial Tree Height CR > HMAX !!  \n");
//	logger(g_log, "Initial Tree Height = HMAX  = %f m\n", s->value[INITIAL_TREE_HEIGHT_CR]);
//	}
//	}
//	else
//	{
//	s->value[TREE_HEIGHT_CR] = 1.3 + s->value[CRA] * pow (1 - exp ( - s->value[CRB] * s->value[AVDBH]) , s->value[CRC]);
//	logger(g_log, "Tree Height from Chapman-Richards function = %f m\n", s->value[TREE_HEIGHT_CR]);
//	//control
//	if (s->value[TREE_HEIGHT_CR] > s->value[HMAX])
//	{
//	s->value[TREE_HEIGHT_CR] = s->value[HMAX];
//	logger(g_log, "Tree Height CR > HMAX !!  \n");
//	logger(g_log, "Tree Height = HMAX  = %f m\n", s->value[TREE_HEIGHT_CR]);
//	}
//	}
//	 */
//
//	/*LPJ allometry function*/
//	/*
//	   logger(g_log, "-LPJ-allometry function-\n");
//	   if(!years)
//	   {
//	   s->value[TREE_HEIGHT_LPJ] = s->value[KALLOMB] * pow (( s->value[AVDBH] / 100) , s->value[KALLOMC] );
//	// Based on inversion of Reinecke's Rule
//	s->value[CROWN_AREA_LPJ] = s->value[KALLOMA] * pow (( s->value[AVDBH] / 100) , s->value[KRP] );
//
//	}
//	else
//	{
//	s->value[TREE_HEIGHT_LPJ] = s->value[KALLOMB] * pow (( s->value[AVDBH] / 100) , s->value[KALLOMC] );
//	// Based on inversion of Reinecke's Rule
//	s->value[CROWN_AREA_LPJ] = s->value[KALLOMA] * pow (( s->value[AVDBH] / 100) , s->value[KRP] );
//	}
//
//	logger(g_log, "LPJ Height = %f m\n", s->value[TREE_HEIGHT_LPJ]);
//	logger(g_log, "LPJ Crown Area = %f m^2\n", s->value[CROWN_AREA_LPJ] );
//
//	s->value[CROWN_DIAMETER_LPJ] = sqrt (s->value[CROWN_AREA_LPJ]  / ( Pi / 4) ) ;
//	logger(g_log, "LPJ Crown Diameter = %f m\n", s->value[CROWN_DIAMETER_LPJ]);
//
//
//	s->value[CANOPY_COVER_LPJ] = s->counter[N_TREE] * s->value[CROWN_AREA_LPJ] / SIZECELL ;
//
//	logger(g_log, "LPJ Canopy Cover = %f m^2\n", s->value[CANOPY_COVER_LPJ]);
//	logger(g_log, "-------------------------\n");
//	 */
//
//
//	/*Sortie Allometry Function*/
//	/*
//	   logger(g_log, "-SORTIE allometry function-\n");
//
//	   if(!years)
//	   {
//	// Tree Height in m
//	s->value[TREE_HEIGHT_SORTIE] = (1.35 +(s->value[HMAX] - 1.35) * ( 1 - exp ( - s->value[HPOWER] * s->value[AVDBH] )));
//	// Crown Radius in m
//	s->value[CROWN_RADIUS_SORTIE] = s->value[RPOWER] * s->value[AVDBH] ;
//
//	}
//	else
//	{
//	// Tree Height in m
//	s->value[TREE_HEIGHT_SORTIE] = (1.35 +(s->value[HMAX] - 1.35) * ( 1 - exp ( - s->value[HPOWER] * s->value[AVDBH] )));
//	// Crown Radius in m
//	s->value[CROWN_RADIUS_SORTIE] = s->value[RPOWER] * (s->value[AVDBH] * exp (b_RPOWER));
//	}
//	logger(g_log, "Tree Height from Sortie Allometry Function = %f m\n", s->value[TREE_HEIGHT_SORTIE] );
//	logger(g_log, "Crown Radius from Sortie Allometry Function = %f m\n", s->value[CROWN_RADIUS_SORTIE]);
//
//	// Crown Height in m
//	s->value[CROWN_HEIGHT_SORTIE] = (s->value[CHPOWER] * (s->value[TREE_HEIGHT_SORTIE] * exp (b_CHPOWER));
//	logger(g_log, "Crown Height from Sortie Allometry Function = %f m\n", s->value[CROWN_HEIGHT_SORTIE]);
//
//	// Crown Diameter in m
//	s->value[CROWN_DIAMETER_SORTIE] = s->value[CROWN_RADIUS_SORTIE] * 2;
//	logger(g_log, "Crown Diameter from Sortie Allometry Function = %f m\n", s->value[CROWN_DIAMETER_SORTIE]);
//	logger(g_log, "-------------------------\n");
//	 */
//
//
//	//la canopy cover totale deve essere = 1
//	//deve essere la somma quindi di tutte le specie che compongono lo strato dominante!!!!!!!!!!!!!!!!!!!!!!!!!
//	//anche se difficile che specie diverse abbiano esasttamente la stessa altezza per far parte dello stesso layer
//
//
//}
