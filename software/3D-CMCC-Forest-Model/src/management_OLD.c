/*management.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <math.h>
#include <assert.h>
#include "new_forest_tree_class.h"
#include "g-function.h"
#include "management.h"
#include "constants.h"
#include "settings.h"
#include "common.h"
#include "logger.h"
#include "remove_tree_class.h"
#include "biomass.h"
#include "allometry.h"
#include "structure.h"
#include "initialization.h"

extern management_t* g_management;
extern settings_t* g_settings;
extern logger_t* g_debug_log;
extern dataset_t* g_dataset;

static int harvesting (cell_t *const c, const int height, const int dbh, const int age, const int species, const int rsi)
{
	int nat_man;   /* natural or managed mortality 0 = natural; 1 = managed */
	species_t *s;

	nat_man = 1;

	
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* at the moment it considers a complete harvesting for all classes (if considered) */
	logger(g_debug_log, "\n** Management options: Harvesting ** \n");

	/* add harvested trees */
	s->counter[THINNED_TREE]  += s->counter[N_TREE];
	s->counter[HARVESTING_HAPPENS] = 1;

	/* update C and N biomass */
	tree_biomass_remove ( c, height, dbh, age, species, s->counter[N_TREE], nat_man );
     
        // update cell level C and N pools  // ddalmo august 2021 
        // note: to be more precise we should update all the pools considered in the cell
        // including livewood,sapwood etc (see for instance initialization_forest_class_C). 
        // At this point we keep it in this way:

	/*** update cell level carbon pools (tC/cell) ***/
	c->leaf_carbon              -= (s->value[C_LEAF_TO_LITR]   * 1e6 / g_settings->sizeCell);
	c->froot_carbon             -= (s->value[C_FROOT_TO_LITR]  * 1e6 / g_settings->sizeCell);
	c->stem_carbon              -= (s->value[C_STEM_TO_CWD]    * 1e6 / g_settings->sizeCell);
	c->branch_carbon            -= (s->value[C_BRANCH_TO_CWD]  * 1e6 / g_settings->sizeCell);
	c->croot_carbon             -= (s->value[C_CROOT_TO_CWD]   * 1e6 / g_settings->sizeCell);
	c->reserve_carbon           -= (s->value[C_RESERVE_TO_CWD] * 1e6 / g_settings->sizeCell);
	c->fruit_carbon             -= (s->value[C_FRUIT_TO_CWD]   * 1e6 / g_settings->sizeCell);
      
	/* check */
	CHECK_CONDITION ( c->leaf_carbon,    < , ZERO );
	CHECK_CONDITION ( c->froot_carbon,   < , ZERO );
	CHECK_CONDITION ( c->stem_carbon,    < , ZERO );
	CHECK_CONDITION ( c->branch_carbon,  < , ZERO );
	CHECK_CONDITION ( c->croot_carbon,   < , ZERO );
	CHECK_CONDITION ( c->reserve_carbon, < , ZERO );
	CHECK_CONDITION ( c->fruit_carbon,   < , ZERO );

        //ddalmo August 2021 FIXME : to update the cell level N pools too!

	/* remove completely all trees */
	return tree_class_remove (c, height, dbh, age, species );
}


int forest_management (cell_t *const c, const int day, const int month, const int year)
{
           
	//int layer;
	int height;
	int dbh;
	int age;
	int species;
        int THINNING_INDEX =0;
            
        //ddalmo august 2021
        double harvested_carbon; 
        double harvested_carbon_cum ; 
        double harvested_volume ;       
        double harvested_volume_cum ;
                                                 
        int thinned_plants ; 

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;


	// sort by above or below ?
	// ALESSIOR TO ALESSIOC FIXME
	// ALESSIOR commented on 10/07/2017
#ifndef USE_NEW_OUTPUT
	qsort ( c->heights, c->heights_count, sizeof (height_t),
		
			(THINNING_REGIME_ABOVE == g_settings->thinning_regime) ? sort_by_heights_asc : sort_by_heights_desc
	);
#endif

	/* loop on each heights starting from highest to lower */
	for ( height = c->heights_count -1 ; height >= 0; --height )
	{
		/* assign shortcut */
		h = &c->heights[height];

		/* loop on each dbh starting from highest to lower */
		for ( dbh = h->dbhs_count - 1; dbh >= 0; --dbh )
		{
			/* assign shortcut */
			d = &h->dbhs[dbh];

			/* loop on each age class */
			for ( age = d->ages_count - 1 ; age >= 0 ; --age )
			{
				/* assign shortcut */
				a = &d->ages[age];

				/* loop on each species class */
				for ( species = 0; species < a->species_count; ++species )
				{
					int flag = 0;

					/* assign shortcut */
					s = &a->species[species];

					/* initilIze management */
					s->counter[THINNING_HAPPENS] = 0;
					c->harvesting                = 0;
                                        //s->value[THINNING_INDEX]=      0.;

					if ( MANAGEMENT_ON == g_settings->management )
					{
						/* check at the beginning of simulation */
						if( ! year )
						{
							CHECK_CONDITION ( c->years[year].year, >, g_settings->year_start_management );
							CHECK_CONDITION ( (g_settings->year_start_management - g_settings->year_start), >, s->value[THINNING] );
						}

						if ( ( c->years[year].year == g_settings->year_start_management) ||
							
							(( c->years[year].year >= g_settings->year_start_management )
							&& ( s->value[THINNING] == s->counter[YEARS_THINNING] )) )
						{
							flag = 1;
						}
					}
					else if ( (MANAGEMENT_VAR == g_settings->management) || (MANAGEMENT_VAR1 == g_settings->management) )
					{
						/* management forced by stand data */
						if ( year )
						{
							s->counter[THINNING_HAPPENS] = 1;

							prescribed_thinning ( c, height, dbh, age, species, c->years[year].year );
						}

                                                // ddalmo june2021: management prescribed by external file 
						if ( g_management && g_management->thinning_years_count )
						{
                                                 
							int i;
							for ( i = 0; i < g_management->thinning_years_count; i++ )
							{
								if ( c->years[year].year == g_management->thinning_years[i] )
								{
									flag = 1;
                                                                        //s->value[THINNING_INDEX]= i;    
                                                                        THINNING_INDEX= i;            // to set eventually the 
                                                                                                      // intensity of the thinning from external file
                                                                       //printf(" thinning index= %d\n"); //ddalmo 
									break;
								}
							}
						}
					}

					/* thinning */
					if ( flag )
					{
						logger(g_debug_log,"**FOREST MANAGEMENT**\n");
						logger(g_debug_log,"**THINNING**\n");
                                              // printf(" i am in forest_management function, cutting prescribed \n"); //ddalmo
						s->counter[THINNING_HAPPENS] = 1;

						thinning ( c, height, dbh, age, species, year ,THINNING_INDEX);
                                                
						/* reset counter */
						s->counter[YEARS_THINNING] = 0;
					}

					/* increment counter */
					++s->counter[YEARS_THINNING];

					/*************************************** HARVESTING *************************************/
				     
					flag = 0;
					if ( MANAGEMENT_ON == g_settings->management )
					{
						/* check */
						CHECK_CONDITION( s->counter[YEARS_THINNING], >, s->value[ROTATION] );

						/* check for harvesting */
						if ( a->value == s->value[ROTATION] )
						{
							flag = 1;
						}
					}
					else if ( MANAGEMENT_VAR == g_settings->management )
					{
						if ( g_management && g_management->harvesting_years_count )
						{
							int i;
							for ( i = 0; i < g_management->harvesting_years_count; i++ )
							{
								if ( c->years[year].year == g_management->harvesting_years[i] )
								{
									flag = 1;
									break;
								}
							}						
						}							
					}
					if ( flag )
					{
						int rsi;               /* replanted species index */

						logger(g_debug_log,"**FOREST MANAGEMENT**\n");
						logger(g_debug_log,"**HARVESTING**\n");

                                          	// ddalmo note august 2021:
                                          	// the following part of code has been written - very likely - with the idea to provide replanted settings for 
                                          	// different species, and then trough the loop identify rsi in a such way that the replanted specie
                                          	// corresponds to the current specie in the layer. However, this does not allow the change in specie after clear cut
 
#if 0
						/* get replanted_species_index */
						for ( rsi = 0; rsi < g_settings->replanted_count; rsi++ )
						{
							if ( ! string_compare_i(c->heights[height].dbhs[dbh].ages[age].species[species].name
														, g_settings->replanted[rsi].species) )
							{
								/* index found */
								break;
							}
						}

                                               
						/* species found ? */
						CHECK_CONDITION(rsi, ==, g_settings->replanted_count );
#endif

                                            	//ddalmo note august 2021
                                            	// As currently set, we need to assume that the clear cut is performed in one layer/specie only, and 
                                            	// only one new class is added (thus in the setting file, only one replanting dataset is provided)
                                            	// And the replanted specie can be different from the removed old layer. 
                                            	// rsi is simply g_settings->replanted_count -1
                                      
                                            	rsi =  g_settings->replanted_count -1 ;

                                                thinned_plants =       s->counter[N_TREE] ; //save number of harvested trees 

						/* remove tree class */
						if (  ! harvesting ( c, height, dbh, age, species, rsi ) )
						{
							logger_error(g_debug_log, "unable to harvesting! (exit)\n");
							exit(1);
						}

						/* note: RESET c->dos */
						c->dos = 0;
                                             
#if 1
                                                 // ddalmo august 2021

                                                 // with 'harvest' we remove the class, with replanting we create a new class in the same  
                                                 // virtual space (as result the value of e.g. s->value[C_HWP] is simply set to 0 once the replanted class  
                                                 // is added. This happens with the version with only one layer/class/species.  With multilayer or multispecie
                                                 // no tests have been yet performed.

                                                 // We save hence some key variables belonging to the old removed layer and we save it in the new replanted 
                                                 // layer object

                                                 harvested_carbon =      s->value[C_HWP]     ;      /* woody biomass removed (tC/ha/yr) */
                                                 harvested_carbon_cum =  s->value[CUM_C_HWP] ;
                                                 harvested_volume =      s->value[VOLUME_HWP]     ; /*  stem volume removed (m3/ha/yr) */
                                                 harvested_volume_cum =  s->value[CUM_VOLUME_HWP] ;

#endif

						/* compute single tree average biomass */
						//fixme it should be also here!!!!
						//average_tree_pools ( c );  // ddalmo note: actually when initializating the new forest class after replanting
                                                                             // the new average tree pools are also computed

						/* check that all mandatory variables are filled */
						CHECK_CONDITION (g_settings->replanted[rsi].n_tree, <, ZERO);
						CHECK_CONDITION (g_settings->replanted[rsi].height, <, 1.3);
						CHECK_CONDITION (g_settings->replanted[rsi].avdbh,  <, ZERO);
						CHECK_CONDITION (g_settings->replanted[rsi].age,    <, ZERO);
                                                
						/* re-planting tree class */
						if( g_settings->replanted[rsi].n_tree )
						{
							if ( ! add_tree_class_for_replanting( c , day, month, year, rsi ) )
							{
								logger_error(g_debug_log, "unable to add new replanted class! (exit)\n");
								exit(1);
							}

							// indexes
							h = &c->heights[height];
							d = &h->dbhs[dbh];
							a = &d->ages[age];
							s = &a->species[species];
    
							/* reset years_for_thinning */
							s->counter[YEARS_THINNING] = 1;
                                                     
#if 1                                                   
                                                        // ddalmo august 2021 
                                                        // save in the new class object the following variables wich refer to the 
                                                        // removed old class.
                            
                                                        s->value[C_HWP]          =  harvested_carbon      ;  
                                                        s->value[CUM_C_HWP]      =  harvested_carbon_cum  ;  
                                                        s->value[VOLUME_HWP]     =  harvested_volume      ;
                                                        s->value[CUM_VOLUME_HWP] =  harvested_volume_cum  ;
                                                        s->counter[THINNED_TREE] =  thinned_plants        ;  
                       
#endif                                             
                                                     
						}

						s->counter[THINNING_HAPPENS] = 1;
						c->harvesting                = 1;
                                                
					}

                                        /*************************************** PRESCRIBED REGENERATION *************************************/

                                        //ddalmo august 2021

                                        if( g_settings->regeneration || (MANAGEMENT_VAR == g_settings->management))
                                        {
   
                                        flag = 0; 
                                        if ( g_management->regeneration_years_count )
						{
							int i;
							for ( i = 0; i < g_management->regeneration_years_count; i++ )
							{
								if ( c->years[year].year == g_management->regeneration_years[i] )
								{
									flag = 1;
									break;
								}
							}						
						}			


                                          if ( flag )    
                                          //if (year == 5 )  // current test to see if it works
                                          {
                                          printf("YEAR %d\n",year);// ddalmo
                                           //int rsi;               /* replanted species index */
                                           //rsi =  g_settings->replanted_count -1 ;

                                           printf("management.c g_settings->regeneration_management %d\n", g_settings->regeneration);// ddalm

                                 
                 
                                           
                                          /* check that all mandatory variables are filled */
                                                 
						CHECK_CONDITION (g_settings->regeneration_n_tree, <, ZERO); 
						CHECK_CONDITION (g_settings->regeneration_height, <, 1.3);
						CHECK_CONDITION (g_settings->regeneration_avdbh,  <, ZERO);
						CHECK_CONDITION (g_settings->regeneration_age,    <, ZERO);

                                          /* mimic natural regeneration with a re-planting tree class */
						
	                                        if ( ! add_tree_class_for_replanting_reg( c , day, month, year) )
						{
							logger_error(g_debug_log, "unable to add new replanted class! (exit)\n");
							exit(1);
						}

						// indexes
						h = &c->heights[height];
						d = &h->dbhs[dbh];
						a = &d->ages[age];
						s = &a->species[species];
                                                               
        					printf("management.c height %d\n",height); //ddalmo
        					printf("amanagement.c dbh %d\n",dbh); //ddalmo
        					printf("management.c age  %d\n",age ); //ddalmo
 						printf("management.c age specie specie %d\n",species ); //ddalmo
                                                
 						c->harvesting                = 1;   //ddalmo comment: in this way the annual_structure function is not 
                                                                                    // called in the tree_model.c
                                          
                                          }
                                        } 

				}
			}
		}
	}

	return 0;
}

/*****************************************************************************************************************************************/

void thinning (cell_t *const c, const int height, const int dbh, const int age, const int species, const int year, const int THINNING_INDEX)
{
	int trees_to_remove = 0;
	int nat_man;                        /* natural or managed mortality 0 = natural; 1 = managed */
        int thinning_intensity_prescribed; // thinning intensity is given from an external file  //ddalmo
        int index= 0 ; 
       

	species_t *s;

	nat_man = 1;
	
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	/* thinning function based on basal area */

	//TODO
	if (THINNING_REGIME_ABOVE == s->value[THINNING_REGIME] )
	{

	}
	else
	{

	}

	logger(g_debug_log, "** Management options: Thinning ** \n");

	/* BAU MANAGEMENT */

        // currently management_type is not set, hence it is automatically equal to 0

	if ( 0 == g_settings->management_type ) 
	{
               // ddalmo new: added the possibility to prescribe thinning intensity for each thinning event via the management file.
               if ( g_management && g_management->thinning_intensity_count )
		{
				 
                                      index =	 THINNING_INDEX;  //ddalmo new
                                 	 
                                      thinning_intensity_prescribed= g_management->thinning_intensity[index];
	
					
                /* compute integer number of trees to remove */
		trees_to_remove = ROUND((thinning_intensity_prescribed / 100. ) * s->counter[N_TREE]);

               }
               else 
               { 
                // thinning intensity constant for each thinning event as set in the specie-file
		/* compute integer number of trees to remove */
		trees_to_remove = ROUND((s->value[THINNING_INTENSITY] / 100. ) * s->counter[N_TREE]);
               }
	}
	else
	{
		/* LOCAL MANAGEMENT */ // ddalmo comment: or ADAPTIVE MANAGEMENT?
		if ( ! IS_INVALID_VALUE(s->value[MINSTOCKGROW]) && ! IS_INVALID_VALUE(s->value[VOLUME]) )
		{
			int perc;
			double v;

			perc = 0;
			v = (s->value[VOLUME] - s->value[MINSTOCKGROW]) * 100. / s->value[MINSTOCKGROW];

			if ( v >= 80 )
			{
				perc = (int)s->value[THINNING_80];
			}
			else if ( (v >= 60) && (v < 80) )
			{
				perc = (int)s->value[THINNING_60_80];
			}
			else if ( (v >= 40) && (v < 60) )
			{
				perc = (int)s->value[THINNING_40_60];
			}
			else if ( (v >= 20) && (v < 40) )
			{
				perc = (int)s->value[THINNING_40_20];
			}
			else
			{
				perc = (int)s->value[THINNING_0];
			}

			if ( IS_INVALID_VALUE(perc) || ! perc )
			{
				trees_to_remove = 0;
			}
			else
			{
				trees_to_remove = s->counter[N_TREE] * perc / 100.;
			}
		}
	}

	/* added thinned trees */
	s->counter[THINNED_TREE] += trees_to_remove;

	if ( trees_to_remove < s->counter[N_TREE] )
	{
		/* update C and N biomass */   //ddalmo NO: no update of the stand data is performed!!! FIXME
		tree_biomass_remove ( c, height, dbh, age, species, trees_to_remove, nat_man );

		/* remove trees */
		s->counter[N_TREE] -= trees_to_remove;


	}
	else   // FIXME ddalmo comment: this should not happen! anyhow we should set a minimum we have to leave in the stand (if thinning is used 
               // instead to mimic a disturbance, then could also remove the entire class, but we should set the possibility of post fire regeneration
	{
		/* update C and N biomass */
		tree_biomass_remove ( c, height, dbh, age, species, s->counter[N_TREE], nat_man );

		/* remove completely all trees */
		tree_class_remove   (c, height, dbh, age, species );
	}

	/* check */
	CHECK_CONDITION(s->counter[N_TREE], <, ZERO );

	/*********************************************************************************************************************************************************************/

	/* Total class C at the end */
	s->value[TOTAL_C]    = s->value[LEAF_C] + s->value[CROOT_C] + s->value[FROOT_C] + s->value[STEM_C] + s->value[BRANCH_C] + s->value[RESERVE_C];

	/* update stand trees */
	c->n_trees          -= trees_to_remove;
	c->annual_dead_tree += trees_to_remove;

}
/*****************************************************************************************************************************************/

void prescribed_thinning (cell_t *const c, const int height, const int dbh, const int age, const int species, const int year)
{
	int row;
	int nat_man;   /* natural or managed mortality 0 = natural; 1 = managed */

	nat_man = 1;

	assert(g_dataset);


	for ( row = 0; row < g_dataset->rows_count; ++row )
	{
		if ( year == g_dataset->rows[row].year_stand )
		{
			if ( ! string_compare_i(g_dataset->rows[row].species, c->heights[height].dbhs[dbh].ages[age].species[species].name) )
			{
				if ( g_dataset->rows[row].n != c->heights[height].dbhs[dbh].ages[age].species[species].counter[N_TREE] )
				{
					int tree_remove;

					CHECK_CONDITION(c->heights[height].dbhs[dbh].ages[age].species[species].counter[N_TREE], <, g_dataset->rows[row].n);

					/* compute number of tree to remove */
					tree_remove = c->heights[height].dbhs[dbh].ages[age].species[species].counter[N_TREE] - g_dataset->rows[row].n;

					/* added thinned trees */
					c->heights[height].dbhs[dbh].ages[age].species[species].counter[THINNED_TREE] += tree_remove;

					logger(g_debug_log, "\n** Management options: Prescribed Thinning **\n");

					tree_biomass_remove(c, height, dbh, age, species, tree_remove, nat_man);

					c->heights[height].dbhs[dbh].ages[age].species[species].counter[N_TREE] = g_dataset->rows[row].n;

					// added on 7 September 2017
					// with 'var1' model gets also changes in dbh and height
					if ( MANAGEMENT_VAR1 == g_settings->management )
					{
						c->heights[height].value = g_dataset->rows[row].height;
						c->heights[height].dbhs[dbh].value = g_dataset->rows[row].avdbh;
						c->heights[height].dbhs[dbh].ages[age].value = g_dataset->rows[row].age;

						/* initialize power function */
						allometry_power_function                ( c );

						/* initialize carbon pool fraction */
						carbon_pool_fraction                    ( c );

						/* initialize forest structure */
						annual_forest_structure                 ( c, year );

						/* initialize class carbon pools */
						initialization_forest_class_C           ( c, height, dbh, age, species );

						/* initialize cell carbon pools */
						initialization_forest_cell_C            ( c, height, dbh, age, species );

						/* initialize class nitrogen pools */
						initialization_forest_class_N           ( c, height, dbh, age, species );

						/* initialize cell nitrogen pools */
						initialization_forest_cell_N            ( c, height, dbh, age, species );
					}				
				}
			}
		}
	}
}


void management_free(management_t* p)
{
	if ( p )
	{
		if ( p->harvesting_years_count )
		{
			free(p->harvesting_years);
		}

		if  ( p->thinning_years_count )
		{
			free(p->thinning_years);
		}
                //ddalmo
                if  ( p->thinning_intensity_count )
		{
			free(p->thinning_intensity);
		}


		free(p);
	}
}

#if 0  //ddalmo test
management_t* management_load(const char* const filename)
{
#define BUFFER_SIZE 512
	char* token;
	char* p;
	char buffer[BUFFER_SIZE]; // should be enough
	int** p_years;
	int* p_years_count;
	int thinning_flag = 1;
	FILE *f;
	management_t* management;

	const char sz_delimiters[] = " ,\t\r\n";
	const char sz_harvesting[] = "harvesting";
	const char sz_thinning[] = "thinning";

        //const char sz_thinning_int[] = "thinning_int";   //to be added ddalmo may 2021 add also thinning intensity 

	assert(filename);

	f = fopen(filename, "r");
	if ( ! f )
	{
		logger_error(g_debug_log, "file not found\n");
		return NULL;
	}

	management = malloc(sizeof*management);
	if ( ! management )
	{
		logger_error(g_debug_log, "out of memory\n");
		fclose(f);
		return NULL;
	}
	memset(management, 0, sizeof(management_t));

	while ( fgets(buffer, BUFFER_SIZE, f) )
	{
		// skip empty lines
		if ( '\0' == buffer[0] )
		{
			continue;
		}

		token = string_tokenizer(buffer, sz_delimiters, &p);
		if ( ! token ) continue;

		if ( ! string_compare_i(token, sz_harvesting) )
		{
			// parse harvesting
			thinning_flag = 0;
		}
		else if ( ! string_compare_i(token, sz_thinning) )
		{
			// parse thinning
			thinning_flag = 1;
		}
		else
		{
			// do nothing
			continue;
		}

		// set pointers
		if ( thinning_flag )
		{
			p_years = &management->thinning_years;
			p_years_count = &management->thinning_years_count;
		}
		else
		{
			p_years = &management->harvesting_years;
			p_years_count = &management->harvesting_years_count;
		}

		if ( *p_years_count )
		{
			logger_error(g_debug_log, "too many rows of %s specified\n"
						, thinning_flag ? sz_thinning : sz_harvesting
			);
			management_free(management);
			fclose(f);
			return NULL;
		}

		while ( ( token = string_tokenizer(NULL, sz_delimiters, &p ) ) )
		{
			int err;
			int* int_no_leak;
			int year = convert_string_to_int(token, &err);
			if ( err )
			{
				logger_error(g_debug_log, "unable to convert year '%s' for %s\n"
							, token
							, thinning_flag ? sz_thinning : sz_harvesting
				);
				management_free(management);
				fclose(f);
				return NULL;
			}

			int_no_leak = realloc(*p_years, (*p_years_count+1)*sizeof*int_no_leak);
			if ( ! int_no_leak )
			{
				logger_error(g_debug_log, "out of memory\n");
				management_free(management);
				fclose(f);
				return NULL;

			}
			*p_years = int_no_leak;
			(*p_years)[*p_years_count] = year;
			++*p_years_count;
		}
	}
	fclose(f);

	if ( ! management->harvesting_years_count && ! management->thinning_years_count )
	{
		logger_error(g_debug_log, "file is empty ?\n");
		management_free(management);
		management = NULL;
	}

	return management;
#undef BUFFER_SIZE
}
#endif 

// ddalmo june 2021: new load function to load management external information oof harvest,thinning and thinning intensity
management_t* management_load_dani(const char* const filename)
{
#define BUFFER_SIZE 512
	char* token;
	char* p;
	char buffer[BUFFER_SIZE]; // should be enough
	int** p_years;
	int* p_years_count;
        int** p_years_int;        // ddalmo for each year the intensity of removal is provided
        int* p_years_int_count;
        int** p_years_reg;        // ddalmo for each year the intensity of removal is provided
        int* p_years_reg_count;
   
	int thinning_flag = 0;
         int harvesting_flag = 0; //ddalmo
        int thinning_int_flag = 0; //ddalmo
        int regeneration_flag = 0; //ddalmo
	
        FILE *f;
	management_t* management;

	const char sz_delimiters[] = " ,\t\r\n";
	const char sz_harvesting[] = "harvesting";
	const char sz_thinning[]   = "thinning";
        const char sz_thinning_int[] = "thinning_int";   //ddalmo may 2021 :add also thinning intensity 
        const char sz_regeneration[] = "regeneration";   //ddalmo august 2021 :add also years where a new regeneration layer can be added (shelterwood cut)

	assert(filename);

	f = fopen(filename, "r");
	if ( ! f )
	{
		logger_error(g_debug_log, "file not found\n");
		return NULL;
	}

	management = malloc(sizeof*management);
	if ( ! management )
	{
		logger_error(g_debug_log, "out of memory\n");
		fclose(f);
		return NULL;
	}
	memset(management, 0, sizeof(management_t));

	while ( fgets(buffer, BUFFER_SIZE, f) )
	{
		// skip empty lines
		if ( '\0' == buffer[0] )
		{
			continue;
		}

		token = string_tokenizer(buffer, sz_delimiters, &p);
		if ( ! token ) continue;

		if ( ! string_compare_i(token, sz_harvesting) )
		{
			// parse harvesting
                        harvesting_flag = 1;  
			thinning_flag = 0;
                        thinning_int_flag = 0;
                        regeneration_flag = 0;
		}
		else if ( ! string_compare_i(token, sz_thinning) )
		{
			// parse thinning
                        harvesting_flag = 0; 
			thinning_flag = 1;
                        thinning_int_flag = 0; 
                        regeneration_flag = 0;
		}
                else if ( ! string_compare_i(token, sz_thinning_int) )  //ddalmo
		{
			// parse thinning_intensity
                        harvesting_flag = 0; 
			thinning_int_flag = 1;
                        thinning_flag = 0;
                        regeneration_flag = 0;
		}
                else if ( ! string_compare_i(token, sz_regeneration) )  //ddalmo
		{
			// parse regeneration
                        harvesting_flag = 0; 
			thinning_int_flag = 0;
                        thinning_flag = 0;
                        regeneration_flag = 1;
		}  
		else
		{
			// do nothing
			continue;
		}

                //if (!thinning_int_flag) 
                //{
		// set pointers
		if ( thinning_flag )
		{
			p_years = &management->thinning_years;
			p_years_count = &management->thinning_years_count;
		}
		else if ( harvesting_flag )
		{
			p_years = &management->harvesting_years;
			p_years_count = &management->harvesting_years_count;
		} 
                else if ( thinning_int_flag  )
		{
			p_years = &management->thinning_intensity;
			p_years_count = &management->thinning_intensity_count;
		} 
                else if ( regeneration_flag )
		{
			p_years = &management->regeneration_years;
			p_years_count = &management->regeneration_years_count;
		} 
                else
		{
			// do nothing
			continue;
		}

		if ( *p_years_count )
		{
			logger_error(g_debug_log, "too many rows of %s specified\n"
						, thinning_flag ? sz_thinning : sz_harvesting
			);
			management_free(management);
			fclose(f);
			return NULL;
		}

		while ( ( token = string_tokenizer(NULL, sz_delimiters, &p ) ) )
		{
			int err;
			int* int_no_leak;
			int year = convert_string_to_int(token, &err);
			if ( err )
			{
				logger_error(g_debug_log, "unable to convert year '%s' for %s\n"
							, token
							, thinning_flag ? sz_thinning : sz_harvesting
				);
				management_free(management);
				fclose(f);
				return NULL;
			}

			int_no_leak = realloc(*p_years, (*p_years_count+1)*sizeof*int_no_leak);
			if ( ! int_no_leak )
			{
				logger_error(g_debug_log, "out of memory\n");
				management_free(management);
				fclose(f);
				return NULL;

			}
			*p_years = int_no_leak;
			(*p_years)[*p_years_count] = year;
			++*p_years_count;
		}
                  
                

               

               

	}
        
	fclose(f);
printf("management->thinning_years[1]  %d\n",management->thinning_years[1] );
printf("management->thinning_years[0]  %d\n",management->thinning_years[0] );
printf("management->harvesting_years[1]  %d\n",management->harvesting_years[1] );
printf("management->thinning_int_years[3]  %d\n",management->thinning_intensity[3] );
printf("management->regeneration_years[0]  %d\n",management->regeneration_years[0] );
printf("management->regeneration_int_years[2]  %d\n",management->regeneration_years[2] );
        //CHECK!

	if ( ! management->harvesting_years_count && ! management->thinning_years_count)
	{
		logger_error(g_debug_log, "file is empty? Or missing data?(e.g. only thinning intensity/regeneration years data have been provided)\n");
		management_free(management);
		management = NULL;
	}

        if ( ! management->thinning_years_count && management->thinning_intensity_count)
	{
		logger_error(g_debug_log, "Missing prescribed years for thinning while thinning intensity data is provided\n");
		management_free(management);
		management = NULL;
	} 

        if ( g_settings->regeneration && ! management->regeneration_years_count)
	{
		logger_error(g_debug_log, "Missing prescribed years for regeneration while regeneration= ON\n");
		management_free(management);
		management = NULL;
	}
 

	return management;
#undef BUFFER_SIZE
}


