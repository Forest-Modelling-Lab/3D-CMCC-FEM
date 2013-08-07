/*structure.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "types.h"
#include "constants.h"



/* todo : implement a better comparison for equality */
int sort_by_heights_asc(const void * a, const void * b)
{
	if ( ((HEIGHT *)a)->value < ((HEIGHT *)b)->value )
	{
		return -1;
	}
	else if ( ((HEIGHT *)a)->value > ((HEIGHT *)b)->value )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/* todo : implement a better comparison for equality */
int sort_by_heights_desc(const void * a, const void * b)
{
	if ( ((HEIGHT *)a)->value < ((HEIGHT *)b)->value )
	{
		return 1;
	}
	else if ( ((HEIGHT *)a)->value > ((HEIGHT *)b)->value )
	{
		return -1;
	}
	else
	{
		return 0;
	}
}



void Get_annual_numbers_of_layers (CELL *const c)
{
	//determines number of layer in function of:
	//-differences between tree height classes
	//-vegetative or un-vegetative period
	//to determine crowding competition
	int height;
	int age;
	int species;
	float current_height;
	float previous_height;



	//height differences in meter to consider trees in two different layers

	Log("****GET_ANNUAL_FOREST_STRUCTURE_ROUTINE for cell (%d, %d)****\n", c->x, c->y);

	Log("--GET NUMBER OF ANNUAL LAYERS--\n");

	if (settings->spatial == 'u')
	{
		//the model sorts starting from highest tree class
		qsort (c->heights, c->heights_count, sizeof (HEIGHT), sort_by_heights_asc);

		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			for ( age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
			{
				for (species = 0; species < c->heights[height].ages[age].species_count; species++)
				{
					current_height = c->heights[height].value;
					if (height == c->heights_count - 1 )
					{
						c->annual_layer_number = 1;
						previous_height = current_height;
					}
					else
					{
						if ((previous_height -current_height ) > settings->tree_layer_limit)
						{
							c->annual_layer_number += 1;
							previous_height = current_height;
						}
						else
						{
							previous_height = current_height;
						}
					}
				}
			}
		}
		Log("ANNUAL LAYERS NUMBER = %d\n", c->annual_layer_number);

		//ASSIGN ANNUAL Z VALUE
		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			switch (c->annual_layer_number)
			{
			case 1:
				c->heights[height].z = c->annual_layer_number - 1;
				//Log("height %g, z %d\n", c->heights[height].value, c->heights[height].z);
				break;
			case 2:
				if (height == c->heights_count - 1 )
				{
					c->heights[height].z = c->annual_layer_number - 1;
					//Log("height %g, z %d\n", c->heights[height].value, c->heights[height].z);
				}
				else
				{
					if ((c->heights[height+1].value - c->heights[height].value) > settings->tree_layer_limit)
					{
						c->heights[height].z = c->heights[height+1].z - 1;
						//Log("height = %g, z = %d\n", c->heights[height].value, c->heights[height].z);
					}
					else
					{
						c->heights[height].z = c->heights[height+1].z;
						//Log("height = %g, z = %d\n", c->heights[height].value, c->heights[height].z);
					}
				}
				break;
			case 3:
				if (height == c->heights_count - 1)
				{
					c->heights[height].z = c->annual_layer_number - 1;
					//Log("height = %g, z = %d\n", c->heights[height].value, c->heights[height].z);
				}
				else
				{
					if ((c->heights[height+1].value - c->heights[height].value) > settings->tree_layer_limit)
					{
						c->heights[height].z = c->heights[height+1].z - 1;
						//Log("height = %g, z = %d\n", c->heights[height].value, c->heights[height].z);
					}
					else
					{
						c->heights[height].z = c->heights[height+1].z;
						//Log("height = %g, z = %d\n", c->heights[height].value, c->heights[height].z);
					}
				}
				break;
			}
		}
		Log("NUMBER OF DIFFERENT LAYERS = %d\n", c->annual_layer_number);
	}
	else
	{
		//for spatial version only one layer
		Log("SPATIAL VERSION ONLY ONE LAYER\n");
		c->annual_layer_number = 1;
		Log("ANNUAL LAYERS NUMBER = %d\n", c->annual_layer_number);
		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			c->heights[height].z = 0;
		}

	}
}


void Get_forest_structure (CELL *const c)
{
	int height;
	int age;
	int species;
	float DBHDCeffective;
	int tree_number;
	float layer_cover;

	c->height_class_in_layer_dominant_counter = 0;
	c->height_class_in_layer_dominated_counter = 0;
	c->height_class_in_layer_subdominated_counter = 0;
	c->tree_number_dominant = 0;
	c->tree_number_dominated = 0;
	c->tree_number_subdominated = 0;
	c->layer_cover_dominant = 0;
	c->layer_cover_dominated = 0;
	c->layer_cover_subdominated = 0;

	Log("Get_forest_structure\n");


	if (settings->spatial == 'u')
	{
		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			qsort (c->heights, c->heights_count, sizeof (HEIGHT), sort_by_heights_asc);

			for (age = c->heights[height].ages_count - 1; age >= 0; age --)
			{
				for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
				{
					//define numbers of height classes, tree number and density for each layer
					switch (c->annual_layer_number)
					{
					case 1:
						c->height_class_in_layer_dominant_counter += 1;
						c->tree_number_dominant += c->heights[height].ages[age].species[species].counter[N_TREE];
						c->density_dominant = c->tree_number_dominant / settings->sizeCell;
						break;
					case 2:
						if (c->heights[height].z == c->annual_layer_number- 1)
						{
							c->height_class_in_layer_dominant_counter += 1;
							c->tree_number_dominant += c->heights[height].ages[age].species[species].counter[N_TREE];
							c->density_dominant = c->tree_number_dominant / settings->sizeCell;
						}
						else
						{
							c->height_class_in_layer_dominated_counter += 1;
							c->tree_number_dominated += c->heights[height].ages[age].species[species].counter[N_TREE];
							c->density_dominated = c->tree_number_dominated / settings->sizeCell;
						}
						break;
					case 3:
						if (c->heights[height].z == c->annual_layer_number - 1)
						{
							c->height_class_in_layer_dominant_counter += 1;
							c->tree_number_dominant += c->heights[height].ages[age].species[species].counter[N_TREE];
							c->density_dominant = c->tree_number_dominant / settings->sizeCell;
						}
						else if (c->heights[height].z == c->annual_layer_number - 2)
						{
							c->height_class_in_layer_dominated_counter += 1;
							c->tree_number_dominated += c->heights[height].ages[age].species[species].counter[N_TREE];
							c->density_dominated = c->tree_number_dominated / settings->sizeCell;
						}
						else
						{
							c->height_class_in_layer_subdominated_counter += 1;
							c->tree_number_subdominated  += c->heights[height].ages[age].species[species].counter[N_TREE];
							c->density_subdominated = c->tree_number_subdominated / settings->sizeCell;
						}
						break;
					}
				}
			}		//Log("Height class = %g is in layer %d \n", c->heights[height].value, c->heights[height].z);
		}

		if (c->heights_count == 1)
		{
			Log("Number of adult height classes in layer 0 = %d\n", c->height_class_in_layer_dominant_counter);
			Log("Tree number in layer 0 = %d \n", c->tree_number_dominant);
			Log("Density in layer 0 = %g trees/area\n", c->density_dominant);
		}
		if (c->heights_count == 2)
		{
			Log("Number of adult height classes in layer 1 = %d\n", c->height_class_in_layer_dominant_counter);
			Log("Number of adult height classes in layer 0 = %d\n", c->height_class_in_layer_dominated_counter);
			Log("Tree number in layer 1 = %d \n", c->tree_number_dominant);
			Log("Tree number in layer 0 = %d \n", c->tree_number_dominated);
			Log("Density in layer 1 = %g trees/area\n", c->density_dominant);
			Log("Density in layer 0 = %g trees/area\n", c->density_dominated);
		}
		if (c->heights_count > 2)
		{
			Log("Number of adult height classes in layer 2 = %d\n", c->height_class_in_layer_dominant_counter);
			Log("Number of adult height classes in layer 1 = %d\n", c->height_class_in_layer_dominated_counter);
			Log("Number of adult height classes in layer 0 = %d\n", c->height_class_in_layer_subdominated_counter);
			Log("Tree number in layer 2 = %d \n", c->tree_number_dominant);
			Log("Tree number in layer 1 = %d \n", c->tree_number_dominated);
			Log("Tree number in layer 0 = %d \n", c->tree_number_subdominated);
			Log("Density in layer 2 = %g trees/area\n", c->density_dominant);
			Log("Density in layer 1 = %g trees/area\n", c->density_dominated);
			Log("Density in layer 0 = %g trees/area\n", c->density_subdominated);
		}

		height = 0;


		//todo: control if with a drastic tree number reduction (e.g. management) there's a unrealistic strong variation in DBHDCeffective


		//compute class canopy cover
		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			qsort (c->heights, c->heights_count, sizeof (HEIGHT), sort_by_heights_asc);

			for (age = c->heights[height].ages_count - 1; age >= 0; age --)
			{
				for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
				{
					Log("** CANOPY COVER from DBH-DC Function FOR LAYER %d  species %s **\n", c->heights[height].z, c->heights[height].ages[age].species[species].name);


					if (c->heights_count == 1)
					{
						DBHDCeffective = (( c->heights[height].ages[age].species[species].value[DBHDCMAX] - c->heights[height].ages[age].species[species].value[DBHDCMIN] )
								/ (c->heights[height].ages[age].species[species].value[DENMAX] - c->heights[height].ages[age].species[species].value[DENMIN] )
								* (c->density_dominant - c->heights[height].ages[age].species[species].value[DENMIN] ) + c->heights[height].ages[age].species[species].value[DBHDCMIN]);
						Log("DBHDC effective to apply = %g\n", DBHDCeffective);
					}
					else
					{
						if (c->heights_count >= 3 )//3 heights classes or more
						{
							if (c->heights[height].z == 2)
							{
								//settings->dominant
								DBHDCeffective = (( c->heights[height].ages[age].species[species].value[DBHDCMAX] - c->heights[height].ages[age].species[species].value[DBHDCMIN] )
										/ (c->heights[height].ages[age].species[species].value[DENMAX] - c->heights[height].ages[age].species[species].value[DENMIN] )
										* (c->density_dominant - c->heights[height].ages[age].species[species].value[DENMIN] ) + c->heights[height].ages[age].species[species].value[DBHDCMIN]);
								Log("DBHDC effective to apply = %g\n", DBHDCeffective);
							}
							else if (c->heights[height].z == 1)
							{
								DBHDCeffective = (( c->heights[height].ages[age].species[species].value[DBHDCMAX] - c->heights[height].ages[age].species[species].value[DBHDCMIN] )
										/ (c->heights[height].ages[age].species[species].value[DENMAX] - c->heights[height].ages[age].species[species].value[DENMIN] )
										* (c->density_dominated - c->heights[height].ages[age].species[species].value[DENMIN] ) + c->heights[height].ages[age].species[species].value[DBHDCMIN]);
								Log("DBHDC effective to apply = %g\n", DBHDCeffective);
							}
							else
							{
								//SUBDOMINATED
								DBHDCeffective = (( c->heights[height].ages[age].species[species].value[DBHDCMAX] - c->heights[height].ages[age].species[species].value[DBHDCMIN] )
										/ (c->heights[height].ages[age].species[species].value[DENMAX] - c->heights[height].ages[age].species[species].value[DENMIN] )
										* (c->density_subdominated - c->heights[height].ages[age].species[species].value[DENMIN] ) + c->heights[height].ages[age].species[species].value[DBHDCMIN]);
								Log("DBHDC effective to apply = %g\n", DBHDCeffective);;

							}
						}
						else //2 height classes
						{
							if (c->heights[height].z == 2)
							{
								//settings->dominant
								DBHDCeffective = (( c->heights[height].ages[age].species[species].value[DBHDCMAX] - c->heights[height].ages[age].species[species].value[DBHDCMIN] )
										/ (c->heights[height].ages[age].species[species].value[DENMAX] - c->heights[height].ages[age].species[species].value[DENMIN] )
										* (c->density_dominant - c->heights[height].ages[age].species[species].value[DENMIN] ) + c->heights[height].ages[age].species[species].value[DBHDCMIN]);
								Log("DBHDC effective to apply = %g\n", DBHDCeffective);
							}
							else
							{
								DBHDCeffective = (( c->heights[height].ages[age].species[species].value[DBHDCMAX] - c->heights[height].ages[age].species[species].value[DBHDCMIN] )
										/ (c->heights[height].ages[age].species[species].value[DENMAX] - c->heights[height].ages[age].species[species].value[DENMIN] )
										* (c->density_dominated - c->heights[height].ages[age].species[species].value[DENMIN] ) + c->heights[height].ages[age].species[species].value[DBHDCMIN]);
								Log("DBHDC effective to apply = %g\n", DBHDCeffective);
							}
						}
					}



					if (DBHDCeffective > c->heights[height].ages[age].species[species].value[DBHDCMAX])
					{
						//Log("DBHDC effective for Dominant Layer > DBHDCMAX!!!\n");
						DBHDCeffective = c->heights[height].ages[age].species[species].value[DBHDCMAX];
						Log("DBHDC effective applied = %g\n", DBHDCeffective);
					}


					//Crown Diameter using DBH-DC

					Log("-AvDBH  = %g cm\n", c->heights[height].ages[age].species[species].value[AVDBH]);

					c->heights[height].ages[age].species[species].value[CROWN_DIAMETER_DBHDC_FUNC] = c->heights[height].ages[age].species[species].value[AVDBH] * DBHDCeffective;
					Log("-Crown Diameter from DBHDC function  = %g m\n", c->heights[height].ages[age].species[species].value[CROWN_DIAMETER_DBHDC_FUNC]);

					//Crown Area using DBH-DC
					c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC] = ( Pi / 4) * pow (c->heights[height].ages[age].species[species].value[CROWN_DIAMETER_DBHDC_FUNC], 2 );
					Log("-Crown Area from DBHDC function = %g m^2\n", c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]);


					//Canopy Cover using DBH-DC

					c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] = c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC] * c->heights[height].ages[age].species[species].counter[N_TREE] / settings->sizeCell;

				}
			}
		}

		//compute layer cover
		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			qsort (c->heights, c->heights_count, sizeof (HEIGHT), sort_by_heights_asc);

			for (age = c->heights[height].ages_count - 1; age >= 0; age --)
			{
				for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
				{
					//define numbers of height classes, tree number and density for each layer
					switch (c->annual_layer_number)
					{
					case 1:
						c->layer_cover_dominant += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
						break;
					case 2:
						if (c->heights[height].z == c->annual_layer_number- 1)
						{
							c->layer_cover_dominant += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
						}
						else
						{
							c->layer_cover_dominated += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
						}
						break;
					case 3:
						if (c->heights[height].z == c->annual_layer_number - 1)
						{
							c->layer_cover_dominant += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
						}
						else if (c->heights[height].z == c->annual_layer_number - 2)
						{
							c->layer_cover_dominated += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
						}
						else
						{
							c->layer_cover_subdominated += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
						}
						break;
					}
				}
			}		//Log("Height class = %g is in layer %d \n", c->heights[height].value, c->heights[height].z);
		}

		if (c->heights_count == 1)
		{
			Log("Layer cover in layer 0 = %g %% \n", c->layer_cover_dominant * 100);

			c->daily_cc[0] = c->layer_cover_dominant * 100;
		}
		if (c->heights_count == 2)
		{
			Log("Layer cover in layer 1 = %g %%\n", c->layer_cover_dominant * 100);
			Log("Layer cover in layer 0 = %g %% \n", c->layer_cover_dominated * 100);

			c->daily_cc[1] = c->layer_cover_dominant * 100;
			c->daily_cc[0] = c->layer_cover_dominated * 100;
		}
		if (c->heights_count > 2)
		{
			Log("Layer cover in layer 2 = %g %%\n", c->layer_cover_dominant * 100);
			Log("Layer cover in layer 1 = %g %% \n", c->layer_cover_dominated * 100);
			Log("Layer cover in layer 0 = %g %% \n", c->layer_cover_subdominated * 100);

			c->daily_cc[2] = c->layer_cover_dominant * 100;
			c->daily_cc[1] = c->layer_cover_dominated * 100;
			c->daily_cc[0] = c->layer_cover_subdominated * 100;
		}


		//the model makes die trees of the lower height class for that layer because
		//it passes through the function sort_by_height_desc the height classes starting from the lowest
		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			qsort (c->heights, c->heights_count, sizeof (HEIGHT), sort_by_heights_desc);

			for (age = c->heights[height].ages_count - 1; age >= 0; age --)
			{
				for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
				{
					//define numbers of height classes, tree number and density for each layer
					switch (c->annual_layer_number)
					{
					case 1:
						if (c->layer_cover_dominant >= settings->max_layer_cover)
						{
							//mortality
							layer_cover = c->layer_cover_dominant;
							tree_number = c->tree_number_dominant;
							Get_layer_cover_mortality (&c->heights[height].ages[age].species[species], &c, layer_cover, tree_number, c->heights[height].z, height);
						}
						else
						{
							//Log ("NO MORTALITY BASED ON HIGH CANOPY COVER layer %d !!!\n", c->heights[height].z);
						}
						break;
					case 2:
						if (c->heights[height].z == c->annual_layer_number- 1)
						{
							if (c->layer_cover_dominant >= settings->max_layer_cover)
							{
								//mortality
								layer_cover = c->layer_cover_dominant;
								tree_number = c->tree_number_dominant;
								Get_layer_cover_mortality (&c->heights[height].ages[age].species[species], &c,layer_cover, tree_number, c->heights[height].z, height);
							}
							else
							{
								Log ("NO MORTALITY BASED ON HIGH CANOPY COVER layer %d !!!\n", c->heights[height].z);
							}
						}
						else
						{
							if (c->layer_cover_dominated >= settings->max_layer_cover)
							{
								//mortality
								layer_cover = c->layer_cover_dominated;
								tree_number = c->tree_number_dominated;
								Get_layer_cover_mortality (&c->heights[height].ages[age].species[species], &c, layer_cover, tree_number, c->heights[height].z, height);
							}
							else
							{
								Log ("NO MORTALITY BASED ON HIGH CANOPY COVER layer %d !!!\n", c->heights[height].z);
							}
						}

						break;
					case 3:
						if (c->heights[height].z == c->annual_layer_number- 1)
						{
							if (c->layer_cover_dominant >= settings->max_layer_cover)
							{
								//mortality
								layer_cover = c->layer_cover_dominant;
								tree_number = c->tree_number_dominant;
								Get_layer_cover_mortality (&c->heights[height].ages[age].species[species], &c, layer_cover, tree_number, c->heights[height].z, height);
							}
							else
							{
								//Log ("NO MORTALITY BASED ON HIGH CANOPY COVER layer %d !!!\n", c->heights[height].z);
							}
						}
						else if (c->heights[height].z == c->annual_layer_number - 2)
						{
							if (c->layer_cover_dominated >= settings->max_layer_cover)
							{
								//mortality
								layer_cover = c->layer_cover_dominated;
								tree_number = c->tree_number_dominated;
								Get_layer_cover_mortality (&c->heights[height].ages[age].species[species], &c,layer_cover, tree_number, c->heights[height].z, height);
							}
							else
							{
								//Log ("NO MORTALITY BASED ON HIGH CANOPY COVER layer %d !!!\n", c->heights[height].z);
							}
						}
						else
						{
							if (c->layer_cover_subdominated >= settings->max_layer_cover)
							{
								//mortality
								layer_cover = c->layer_cover_subdominated;
								tree_number = c->tree_number_subdominated;
								Get_layer_cover_mortality (&c->heights[height].ages[age].species[species], &c,layer_cover, tree_number, c->heights[height].z, height);
							}
							else
							{
								//Log ("NO MORTALITY BASED ON HIGH CANOPY COVER layer %d !!!\n", c->heights[height].z);
							}
						}
						break;
					}
				}
			}		//Log("Height class = %g is in layer %d \n", c->heights[height].value, c->heights[height].z);
		}
	}
	else
	{
		for ( height = c->heights_count - 1; height >= 0; height-- )
		{
			qsort (c->heights, c->heights_count, sizeof (HEIGHT), sort_by_heights_asc);

			for (age = c->heights[height].ages_count - 1; age >= 0; age --)
			{
				for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
				{

					c->height_class_in_layer_dominant_counter += 1;
					c->tree_number_dominant += c->heights[height].ages[age].species[species].counter[N_TREE];
					c->density_dominant = c->tree_number_dominant / settings->sizeCell;

					DBHDCeffective = (( c->heights[height].ages[age].species[species].value[DBHDCMAX] - c->heights[height].ages[age].species[species].value[DBHDCMIN] )
							/ (c->heights[height].ages[age].species[species].value[DENMAX] - c->heights[height].ages[age].species[species].value[DENMIN] )
							* (c->density_dominant - c->heights[height].ages[age].species[species].value[DENMIN] ) + c->heights[height].ages[age].species[species].value[DBHDCMIN]);
					Log("DBHDC effective to apply = %g\n", DBHDCeffective);


					//control
					if (DBHDCeffective > c->heights[height].ages[age].species[species].value[DBHDCMAX])
					{
						DBHDCeffective = c->heights[height].ages[age].species[species].value[DBHDCMAX];
						Log("DBHDC effective to apply = %g\n", DBHDCeffective);
					}
					else if (DBHDCeffective < c->heights[height].ages[age].species[species].value[DBHDCMIN])
					{
						DBHDCeffective = c->heights[height].ages[age].species[species].value[DBHDCMIN];
						Log("DBHDC effective to apply = %g\n", DBHDCeffective);
					}



					//Crown Diameter using DBH-DC

					c->heights[height].ages[age].species[species].value[CROWN_DIAMETER_DBHDC_FUNC] = c->heights[height].ages[age].species[species].value[AVDBH] * DBHDCeffective;
					Log("-Crown Diameter from DBHDC function  = %g m\n", c->heights[height].ages[age].species[species].value[CROWN_DIAMETER_DBHDC_FUNC]);

					//Crown Area using DBH-DC
					c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC] = ( Pi / 4) * pow (c->heights[height].ages[age].species[species].value[CROWN_DIAMETER_DBHDC_FUNC], 2 );
					Log("-Crown Area from DBHDC function = %g m^2\n", c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]);


					//Canopy Cover using DBH-DC

					c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] = c->heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC] * c->heights[height].ages[age].species[species].counter[N_TREE] / settings->sizeCell;
					Log("-Canopy Cover from DBHDC function = %g \n", c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC]);
					//Canopy Layer Cover
					c->layer_cover_dominant += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];



				}
			}
		}
		Log("Number of adult height classes in layer 0 = %d\n", c->height_class_in_layer_dominant_counter);
		Log("Tree number in layer 0 = %d \n", c->tree_number_dominant);
		c->density_dominant = c->tree_number_dominant / settings->sizeCell;
		Log("Density in layer 0 = %g trees/area\n", c->density_dominant);

		if (c->layer_cover_dominant >=  settings->max_layer_cover)
		{
			Log("Layer cover exceeds max layer cover!!!\n");
		}

	}
	Log("*************************************************** \n");
}


void Get_monthly_vegetative_period (CELL *c, const MET_DATA *const met, int month)
{

	static int height;
	static int age;
	static int species;
	static int counter;

	static int leaf_fall_counter;

	if (month == 0)
		leaf_fall_counter = 0;

	counter = 0;

	Log("\n\n\n****GET_MONTHLY_FOREST_STRUCTURE_ROUTINE for cell (%g, %g)****\n", c->x, c->y);

	//assign value for VEG_UNVEG (1 for veg, 0 for Unveg) and compute number of classes in veg period

	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		for ( age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			for (species = 0; species < c->heights[height].ages[age].species_count; species++)
			{
				Log("--GET VEGETATIVE PERIOD for height = %g, age = %d, species %s --\n", c->heights[height].value, c->heights[height].ages[age].value, c->heights[height].ages[age].species[species].name);

				if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
				{
					if (settings->version == 's')
					{
						//Log("Spatial version \n");

						//veg period
						if (met[month].ndvi_lai > 0.1)
						{
							c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 1;
							counter += 1;
						}
						//unveg period
						else
						{
							c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 0;
						}
					}
					else
					{

						//compute months of leaf fall taking an integer value
						c->heights[height].ages[age].species[species].value[MONTH_FRAC_FOLIAGE_REMOVE] =  (c->heights[height].ages[age].species[species].value[LEAF_FALL_FRAC_GROWING]
						                                                                                                                                       * c->heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
						Log("Months of leaf fall for deciduous = %g \n", c->heights[height].ages[age].species[species].value[MONTH_FRAC_FOLIAGE_REMOVE]);
						//monthly rate of foliage reduction

						//currently the model considers a linear reduction in leaf fall
						//it should be a negative sigmoid function
						//todo: create a sigmoid function
						c->heights[height].ages[age].species[species].value[FOLIAGE_REDUCTION_RATE] = 1.0 / (c->heights[height].ages[age].species[species].value[MONTH_FRAC_FOLIAGE_REMOVE] + 1);
						Log("foliage reduction rate = %g \n", c->heights[height].ages[age].species[species].value[FOLIAGE_REDUCTION_RATE] );




						//todo decidere su usare tavg o tday
						if(((met[month].tavg >= c->heights[height].ages[age].species[species].value[GROWTHSTART] && month < 6)
								|| (met[month].tavg >= c->heights[height].ages[age].species[species].value[GROWTHEND] && month >= 6)) && c->north == 0)
						{
							c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 1;
							counter += 1;
							Log("counter = %d\n\n", counter);
						}
						else
						{
							if (met[month].daylength <= c->heights[height].ages[age].species[species].value[MINDAYLENGTH] && month >= 6 && c->north == 0)
							{

								leaf_fall_counter += 1;

								//check
								if(leaf_fall_counter <= (int)c->heights[height].ages[age].species[species].value[MONTH_FRAC_FOLIAGE_REMOVE])
								{
									/*days of leaf fall*/
									c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 1;
								}
								else
								{
									/*outside days of leaf fall*/
									c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 0;
								}

							}
							else
							{
								c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 0;
							}
							Log("counter = %d\n\n", counter);
						}
					}
				}
				else
				{
					c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 1;
					Log("Veg period = %d \n", c->heights[height].ages[age].species[species].counter[VEG_UNVEG]);
					counter += 1;
					Log("counter = %d\n\n", counter);
				}
			}
		}
	}
	Log("species in veg period = %d\n", counter);
}


//define VEG_UNVEG for deciduous species
void Get_daily_vegetative_period (CELL *c, const MET_DATA *const met, int month, int day)
{

	static int height;
	static int age;
	static int species;
	static int counter;





	/*VEG_UNVEG = 1 for veg period, = 0 for Un-Veg period*/


	counter = 0;

	Log("\n\n\n****GET_DAILY_FOREST_STRUCTURE_ROUTINE for cell (%d, %d)****\n", c->x, c->y);

	//assign value for VEG_UNVEG (1 for veg, 0 for Unveg) and compute number of classes in veg period

	for (height = c->heights_count - 1; height >= 0; height-- )
	{
		for (age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			for (species = 0; species < c->heights[height].ages[age].species_count; species++)
			{

				if (day == 0 && month == 0)
				{
					c->heights[height].ages[age].species[species].counter[LEAF_FALL_COUNTER] = 0;
				}

				/*PHENOLOGY = 0 FOR DECIDUOUS*/
				if (c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.1 || c->heights[height].ages[age].species[species].value[PHENOLOGY] == 0.2)
				{
					Log("-GET ANNUAL VEGETATIVE DAYS for species %s -\n", c->heights[height].ages[age].species[species].name);
					if (settings->spatial == 's')
					{
						//Log("Spatial version \n");

						//veg period
						if (met[month].d[day].ndvi_lai > 0.1)
						{
							c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 1;
							counter += 1;
							Log("%s is in veg period\n", c->heights[height].ages[age].species[species].name);
						}
						//unveg period
						else
						{
							c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 0;
							Log("%s is in un-veg period\n", c->heights[height].ages[age].species[species].name);
						}
					}
					else
					{

						/*compute annual days of leaf fall*/
						c->heights[height].ages[age].species[species].value[DAY_FRAC_FOLIAGE_REMOVE] =  ( c->heights[height].ages[age].species[species].value[LEAF_FALL_FRAC_GROWING]
						                                                                                                                                      * c->heights[height].ages[age].species[species].counter[DAY_VEG_FOR_LITTERFALL_RATE]);
						//Log("Days of leaf fall for deciduous = %g day\n", c->heights[height].ages[age].species[species].value[DAY_FRAC_FOLIAGE_REMOVE]);
						//monthly rate of foliage reduction

						//currently the model considers a linear reduction in leaf fall
						//it should be a negative sigmoid function
						//todo: create a sigmoid function
						c->heights[height].ages[age].species[species].value[FOLIAGE_REDUCTION_RATE] = 1.0 / (c->heights[height].ages[age].species[species].value[DAY_FRAC_FOLIAGE_REMOVE] + 1);
						//Log("foliage reduction rate = %g,  = %g%\n", c->heights[height].ages[age].species[species].value[FOLIAGE_REDUCTION_RATE], c->heights[height].ages[age].species[species].value[FOLIAGE_REDUCTION_RATE] * 100);


						//todo decidere se utlizzare growthend o mindaylenght
						//lo stesso approccio deve essere usato anche in Get_Veg_Days func
						//currently model can simulate only forests in boreal hemisphere
						if ((met[month].d[day].thermic_sum >= c->heights[height].ages[age].species[species].value[GROWTHSTART] && month <= 6)
								|| (met[month].d[day].daylength >= c->heights[height].ages[age].species[species].value[MINDAYLENGTH] && month >= 6 && c->north == 0))
						{
							c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 1;
							counter += 1;
							Log("%s is in veg period\n", c->heights[height].ages[age].species[species].name);
						}
						else
						{
							//check for case 0 of allocation
							if (met[month].d[day].daylength <= c->heights[height].ages[age].species[species].value[MINDAYLENGTH] && month >= 6 && c->north == 0 )
							{

								Log("DAY_FRAC_FOLIAGE_REMOVE %g\n", c->heights[height].ages[age].species[species].value[DAY_FRAC_FOLIAGE_REMOVE]);

								c->heights[height].ages[age].species[species].counter[LEAF_FALL_COUNTER]  += 1;
								//check
								if(c->heights[height].ages[age].species[species].counter[LEAF_FALL_COUNTER]  == 1)
								{
									//assign value of thermic sum
									c->heights[height].ages[age].species[species].value[THERMIC_SUM_FOR_END_VEG] = met[month].d[day].thermic_sum;
									//Log("thermic_sum END OF VEG = %g °C\n", c->heights[height].ages[age].species[species].value[THERMIC_SUM_FOR_END_VEG]);
								}

								//check
								if(c->heights[height].ages[age].species[species].counter[LEAF_FALL_COUNTER]  <= (int)c->heights[height].ages[age].species[species].value[DAY_FRAC_FOLIAGE_REMOVE])
								{
									/*days of leaf fall*/
									c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 1;
								}
								else
								{
									/*outside days of leaf fall*/
									c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 0;
								}

							}
							else
							{
								c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 0;
								Log("%s is in un-veg period\n", c->heights[height].ages[age].species[species].name);
							}
						}
					}
				}
				/*PHENOLOGY = 1 FOR EVERGREEN*/
				else
				{
					c->heights[height].ages[age].species[species].counter[VEG_UNVEG] = 1;
					Log("Veg period = %d \n", c->heights[height].ages[age].species[species].counter[VEG_UNVEG]);
					counter += 1;
					Log("%s is in veg period\n", c->heights[height].ages[age].species[species].name);
				}
			}
		}
	}
	Log("classes in veg period = %d\n", counter);
}



extern void Get_monthly_numbers_of_layers (CELL *const c)
{
	//determines number of layer in function of:
	//-differences between tree height classes
	//-vegetative or un-vegetative period
	static int height;
	static int age;
	static int species;
	static float current_height;
	static float previous_height;

	//height differences in meter to consider trees in two different layers
	c->monthly_layer_number = 0;

	Log("--GET NUMBER OF MONTHLY LAYERS (Layer in Veg)--\n");


	qsort (c->heights, c->heights_count, sizeof (HEIGHT), sort_by_heights_asc);

	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		for ( age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			for (species = 0; species < c->heights[height].ages[age].species_count; species++)
			{
				current_height = c->heights[height].value;
				if (c->heights_count > 1 )
				{
					if (height == c->heights_count -1 && c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
					{
						c->monthly_layer_number += 1;
						previous_height = current_height;
					}
					if ((previous_height - current_height ) > settings->tree_layer_limit && c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
					{
						c->monthly_layer_number += 1;
						previous_height = current_height;
					}
				}
				if (c->heights_count == 1  && c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
				{
					c->monthly_layer_number = 1;
				}
			}
		}
	}
	Log("number of vegetative layers = %d\n", c->monthly_layer_number);

	//Log("height count = %d \n", c->heights_count);
}

extern void Get_daily_numbers_of_layers (CELL *const c)
{
	//determines number of layer in function of:
	//-differences between tree height classes
	//-vegetative or un-vegetative period
	static int height;
	static int age;
	static int species;
	static float current_height;
	static float previous_height;


	c->daily_layer_number = 0;

	Log("\n--GET NUMBER OF DAILY LAYERS (Layer in Veg)--\n");


	qsort (c->heights, c->heights_count, sizeof (HEIGHT), sort_by_heights_asc);

	for ( height = c->heights_count - 1; height >= 0; height-- )
	{
		for ( age = c->heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			for (species = 0; species < c->heights[height].ages[age].species_count; species++)
			{
				current_height = c->heights[height].value;

				if (c->heights_count > 1 )
				{
					if (height == c->heights_count -1 && c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
					{
						c->daily_layer_number += 1;
						previous_height = current_height;
					}
					if ((previous_height - current_height ) > settings->tree_layer_limit && c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
					{
						c->daily_layer_number += 1;
						previous_height = current_height;
					}
				}
				if (c->heights_count == 1  && c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
				{
					c->daily_layer_number = 1;
				}
			}
		}
	}
	Log("number of vegetative layers = %d\n", c->daily_layer_number);

	//Log("height count = %d \n", c->heights_count);
}


void Get_monthly_layer_cover (CELL * c, const MET_DATA *const met, int month)
{

	//compute if is in veg period
	static int height;
	static int age;
	static int species;


	c->layer_cover_dominant = 0;
	c->layer_cover_dominated = 0;
	c->layer_cover_subdominated = 0;


	Log("\nGET_MONTHLY_FOREST_STRUCTURE_ROUTINE, MONTH = %d\n", month+1);

	Log("Determines Effective Layer Cover \n");

	for (height = c->heights_count - 1; height >= 0; height -- )
	{
		for (age = c->heights[height].ages_count - 1; age >= 0; age --)
		{
			for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
			{
				if ( c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
				{
					if (c->monthly_layer_number == 3)
					{
						switch (c->heights[height].z)
						{
						case 2:
							//Log("z = %d\n", c->heights[height].z);
							c->layer_cover_dominant += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							//Log("Layer cover in layer 2 = %g %% \n", c->layer_cover_dominant * 100);
							break;
						case 1:
							//Log("z = %d\n", c->heights[height].z);
							c->layer_cover_dominated += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							//Log("Layer cover in layer 1 = %g %% \n", c->layer_cover_dominated * 100);
							break;
						case 0:
							//Log("z = %d\n", c->heights[height].z);
							c->layer_cover_subdominated  += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							//Log("Layer cover in layer 0 = %g %% \n", c->layer_cover_subdominated * 100);
							break;
						}
					}
					if (c->monthly_layer_number == 2)
					{
						switch (c->heights[height].z)
						{
						case 1:
							//Log("z = %d\n", c->heights[height].z);
							c->layer_cover_dominant += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							//Log("Layer cover in layer 1 = %g %% \n", c->layer_cover_dominant * 100);
							break;
						case 0:
							//Log("z = %d\n", c->heights[height].z);
							c->layer_cover_dominated  += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							//Log("Layer cover in layer 0 = %g %% \n", c->layer_cover_dominated * 100);
							break;
						}
					}
					else
					{
						//Log("z = %d\n", c->heights[height].z);
						c->layer_cover_dominant  += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
						//Log("Layer cover in layer 0 = %g %% \n", c->layer_cover_dominant * 100);
					}
				}
				else
				{
					//Log("Un-Vegetating layers\n");
					//Log("z = %d\n", c->heights[height].z);
				}
			}
		}
	}

	Log("Monthly layer number = %d\n", c->monthly_layer_number);

	if (c->monthly_layer_number == 1)
	{
		Log("Vegetated Layer cover in layer 0 = %g %% \n", c->layer_cover_dominant * 100);
	}
	if (c->monthly_layer_number == 2)
	{
		Log("Vegetated Layer cover in layer 1 = %g %%\n", c->layer_cover_dominant * 100);
		Log("Vegetated Layer cover in layer 0 = %g %% \n", c->layer_cover_dominated * 100);
	}
	if (c->monthly_layer_number > 2)
	{
		Log("Vegetated Layer cover in layer 2 = %g %%\n", c->layer_cover_dominant * 100);
		Log("Vegetated Layer cover in layer 1 = %g %% \n", c->layer_cover_dominated * 100);
		Log("Vegetated Layer cover in layer 0 = %g %% \n", c->layer_cover_subdominated * 100);
	}
	Log("*************************************************** \n");

}

void Get_daily_layer_cover (CELL * c, const MET_DATA *const met, int month, int day)
{

	//compute if is in veg period
	static int height;
	static int age;
	static int species;


	c->layer_cover_dominant = 0;
	c->layer_cover_dominated = 0;
	c->layer_cover_subdominated = 0;


	Log("\nGET_DAILY_FOREST_STRUCTURE_ROUTINE\n");

	Log("Determine Effective Layer Cover \n");

	for (height = c->heights_count - 1; height >= 0; height -- )
	{
		for (age = c->heights[height].ages_count - 1; age >= 0; age --)
		{
			for (species = c->heights[height].ages[age].species_count - 1; species >= 0; species -- )
			{
				if ( c->heights[height].ages[age].species[species].counter[VEG_UNVEG] == 1)
				{
					if (c->daily_layer_number == 3)
					{
						switch (c->heights[height].z)
						{
						case 2:
							//Log("z = %d\n", c->heights[height].z);
							c->layer_cover_dominant += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							//Log("Layer cover in layer 2 = %g %% \n", c->layer_cover_dominant * 100);
							break;
						case 1:
							//Log("z = %d\n", c->heights[height].z);
							c->layer_cover_dominated += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							//Log("Layer cover in layer 1 = %g %% \n", c->layer_cover_dominated * 100);
							break;
						case 0:
							//Log("z = %d\n", c->heights[height].z);
							c->layer_cover_subdominated  += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							//Log("Layer cover in layer 0 = %g %% \n", c->layer_cover_subdominated * 100);
							break;
						}
					}
					if (c->daily_layer_number == 2)
					{
						switch (c->heights[height].z)
						{
						case 1:
							//Log("z = %d\n", c->heights[height].z);
							c->layer_cover_dominant += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							//Log("Layer cover in layer 1 = %g %% \n", c->layer_cover_dominant * 100);
							break;
						case 0:
							//Log("z = %d\n", c->heights[height].z);
							c->layer_cover_dominated  += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
							//Log("Layer cover in layer 0 = %g %% \n", c->layer_cover_dominated * 100);
							break;
						}
					}
					else
					{
						//Log("z = %d\n", c->heights[height].z);
						c->layer_cover_dominant  += c->heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
						//Log("Layer cover in layer 0 = %g %% \n", c->layer_cover_dominant * 100);
					}
				}
				else
				{
					//Log("Un-Vegetating layers\n");
					//Log("z = %d\n", c->heights[height].z);
				}
			}
		}
	}

	Log("Daily layer number = %d\n", c->daily_layer_number);

	if (c->daily_layer_number == 1)
	{
		Log("Vegetated Layer cover in layer 0 = %g %% \n", c->layer_cover_dominant * 100);
	}
	if (c->daily_layer_number == 2)
	{
		Log("Vegetated Layer cover in layer 1 = %g %%\n", c->layer_cover_dominant * 100);
		Log("Vegetated Layer cover in layer 0 = %g %% \n", c->layer_cover_dominated * 100);
	}
	if (c->daily_layer_number > 2)
	{
		Log("Vegetated Layer cover in layer 2 = %g %%\n", c->layer_cover_dominant * 100);
		Log("Vegetated Layer cover in layer 1 = %g %% \n", c->layer_cover_dominated * 100);
		Log("Vegetated Layer cover in layer 0 = %g %% \n", c->layer_cover_subdominated * 100);
	}
	Log("*************************************************** \n");

}

/*
void Get_top_layer (CELL *const c, int heights_count, HEIGHT *heights)
{

	Log(" GET_TOP_LAYER_FUNCTION \n");
	int height;

	assert (heights);



	for ( height = heights_count - 1; height >= 0; height-- )
	{
		Log("height dominance = %d \n", c->heights[height].dominance);
		if (c->heights[height].dominance == 1)
		{
			c->heights[height].top_layer = c->heights[height].z ;
			Log("-Top layer and in Dominant Light is layer with z = %d\n", c->heights[height].top_layer);
			break;
		}
		if ( c->heights[height].top_layer == -1)
		{
			Log("--NO TREES IN VEGETATIVE PERIOD!!!\n");
			Log("**********************************************\n");
			break;
		}
	}
}
 */

//This function compute if there is a class age in veg period
void Get_Dominant_Light(HEIGHT *heights, CELL* c, const int count, const MET_DATA *const met, const int month, const int DaysInMonth)
{
	int height;
	int age;
	int species;

	assert(heights);

	Log("!!!!!!!!!!!!!!!!daily_layer number = %d\n", c->daily_layer_number);

	if (c->daily_layer_number != 0)
	{
		Log("-Dominant Light Index Function-\n");

		//highest z value in veg period determines top_layer value
		for ( height = count- 1; height >= 0; height-- )
		{
			qsort (c->heights, c->heights_count, sizeof (HEIGHT), sort_by_heights_desc);
			for ( age = 0; age < heights[height].ages_count; age++ )
			{
				for ( species = 0; species < heights[height].ages[age].species_count; species++ )
				{
					if (heights[height].ages[age].species[species].counter[VEG_UNVEG]==1)
					{

						if (settings->spatial == 'u')
						{
							c->top_layer = c->heights[height].z;
						}
						else
						{
							c->top_layer = 0;
						}
					}
				}
			}
		}
		Log("Daily/Monthly Dominant layer is z = %d\n", c->top_layer);
	}
	//Log("-Species in veg period = %d\n", c->Veg_Counter);
}


int Get_number_of_layers (CELL *c)
{
	int number_of_layers;
	//compute number of layers
	if (c->dominant_veg_counter > 0 && c->dominated_veg_counter > 0 && c->subdominated_veg_counter > 0)
	{
		number_of_layers = 3;
		Log("THREE LAYERS \n");
	}
	else if (c->dominant_veg_counter > 0 && c->dominated_veg_counter > 0 && c->subdominated_veg_counter == 0)
	{
		number_of_layers = 2;
		Log("TWO LAYERS \n");
	}
	else if (c->dominant_veg_counter == 0 && c->dominated_veg_counter > 0 && c->subdominated_veg_counter == 0)
	{
		number_of_layers = 1;
		Log("ONE LAYER \n");
	}
	return number_of_layers;

}


extern void Get_monthly_veg_counter (CELL *c, SPECIES *s, int height)
{
	switch (c->monthly_layer_number)
	{
	case 3:
		if (c->heights[height].z == 2)
		{
			c->dominant_veg_counter += 1;
		}
		if (c->heights[height].z == 1)
		{
			c->dominated_veg_counter += 1;
		}
		else
		{
			c->subdominated_veg_counter += 1;
		}
		break;
	case 2:
		if (c->heights[height].z == 1)
		{
			c->dominant_veg_counter += 1;
		}
		else
		{
			c->dominated_veg_counter += 1;
		}
		break;
	case 1:
		c->dominant_veg_counter += 1;
		break;
	}

}

extern void Get_daily_veg_counter (CELL *c, SPECIES *s, int height)
{
	switch (c->daily_layer_number)
	{
	case 3:
		if (c->heights[height].z == 2)
		{
			c->dominant_veg_counter += 1;
		}
		if (c->heights[height].z == 1)
		{
			c->dominated_veg_counter += 1;
		}
		else
		{
			c->subdominated_veg_counter += 1;
		}
		break;
	case 2:
		if (c->heights[height].z == 1)
		{
			c->dominant_veg_counter += 1;
		}
		else
		{
			c->dominated_veg_counter += 1;
		}
		break;
	case 1:
		c->dominant_veg_counter += 1;
		break;
	}

}






