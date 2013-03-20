#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "types.h"
#include "constants.h"

/* constants */
#define EQUAL_CELL                     1
#define EQUAL_HEIGHT                   2
#define EQUAL_AGE                      4

/* structures */
/* NON CAMBIARE IL LORO ORDINE */
static const char *species_values[] = {
		/*valori relativi alla specie*/

		"LIGHT_TOL",                  //light Tolerance

		"PHENOLOGY",				//PHENOLOGY 0 = deciduous,  1 = evergreen

		"ALPHA",                      // Canopy quantum efficiency (molC/molPAR)
		"EPSILONgCPAR",               // Light Use Efficiency  (gC/molPAR)(used if ALPHA is not available) for Quercus rubra (Waring et al, 1995)
		//"Y",                          // Assimilate use efficiency-Respiration rate-GPP/NP
		//"EPSILONgCMJ",                // = ALPHA * GC_MOL / MOLPAR_MJ = gC/MJ


		"K",                          //Extinction coefficient for absorption of PAR by canopy for Quercus cerris L. (A. Cutini, Ann Sci For, 1996)
		"ALBEDO",



		//LEAF AREA INDEX
		"LAIGCX",                     //LAI for maximum canopy conductance
		"LAIMAXINTCPTN",              //LAI for maximum rainfall interception
		"MAXINTCPTN",                 //Maximum proportion of rainfall interception evaporated from canopy for Quercus spp Breuer et al 2003


		//SPECIFIC LEAF AREA 3PG
		"SLA0",                       //Specific Leaf Area at age 0 (m^2/kg)
		"SLA1",                       //Specific Laef Area for mature leaves (m^2/kg)
		"TSLA",                       //Age at which Specific Leaf Area = (SLA0 + SLA1)/2 (years)

		//SPECIFIC LEAF AREA
		"SLA",                        //Specific Leaf Area cm^2/g

		//FRACTION BRANCH-BARK
		"FRACBB0",                    //Branch and Bark fraction at age 0 (m^2/kg)
		"FRACBB1",                    //Branch and Bark fraction for mature stands (m^2/kg)
		"TBB",                        //Age at which fracBB = (FRACBB0 + FRACBB1 )/ 2

		//MASS DENSITY
		"RHOMIN",                     //Minimum Basic Density for young Trees
		"RHOMAX",                     //Maximum Basic Density for young Trees (Ferrara-Nolè)
		"TRHO",                       //Age at which rho = (RHOMIN + RHOMAX )/2
		"DRYMAT_FRESHMAT",				//Conversion from Drymatter to Freshmatter

		//VPD
		"COEFFCOND",                  //Define stomatal responsee to VPD in m/sec
		"BLCOND",                     //Canopy Boundary Layer conductance
		"MAXCOND",                    //Maximum Canopy Conductance in m/sec

		//AGE for timber
		"MAXAGE",
		"RAGE",                       //Relative Age to give fAGE = 0.5
		"NAGE",                       //Power of relative Age in function for Age
		//AGE for SHOOTS
		"MAXAGE_S",
		"RAGE_S",                       //Relative Age to give fAGE = 0.5
		"NAGE_S",                       //Power of relative Age in function for Age


		//TEMPERATURE
		"GROWTHTMIN",                 //Minimum temperature for growth
		"GROWTHTMAX",                 //Maximum temperature for growth
		"GROWTHTOPT",                 //Optimum temperature for growth
		"GROWTHSTART",                //Thermic sum  value for starting growth in °C
		"GROWTHEND",                  //Thermic sum  value for ending growth in °C
		"MINDAYLENGTH",               //minimum day length for phenology

		//SOIL WATER
		"SWPOPEN",
		"SWPCLOSE",
		"SWCONST",                    //Costant in Soil Water modifier vs Moist Ratio
		"SWPOWER",                    //Power in Soil Water modifier vs Moist Ratio

		//BIOMASS PARTITIONING for timber
		"PFS2",                       //Foliage:Stem Partitioning Ratio @ D = 2 cm
		"PFS20",                      //Foliage:Stem Partitioning Ratio @ D = 20 cm
		"PRX",                        //Maximum fraction of NPP to Roots
		"PRN",                        //Minimum fraction of NPP to Roots

		//BIOMASS PARTITIONING for coppice
		"PFS2_C",                      //Foliage:Stem Partitioning Ratio @ D = 2 cm
		"PFS20_C",                     //Foliage:Stem Partitioning Ratio @ D = 20 cm
		"PRX_C",                       //Maximum fraction of NPP to Roots
		"PRN_C",                       //Minimum fraction of NPP to Roots



		//BIOMASS PARTITIONING CTEM
		"OMEGA_CTEM",                        //ALLOCATION PARAMETER
		"S0CTEM",                           //PARAMETER CONTROLLING ALLOCATION TO STEM
		"R0CTEM",                           //PARAMETER CONTROLLING ALLOCATION TO ROOT
		"F0CTEM",                           //PARAMETER CONTROLLING ALLOCATION TO FOLIAGE
		"RES0CTEM",                         //PARAMETER CONTROLLING ALLOCATION TO RESERVE
		"EPSILON_CTEM",                          //ALLOCATION PARAMETER IN (Kgcm^2)^-0.6
		"KAPPA",                            //ALLOCATION PARAMETER DIMENSIONLESS
		"MIN_R0CTEM",                       //MINIMUM RATE TO ROOT AT THE FIRST YEAR AFTER COPPICING
		"MAX_S0CTEM",                       //MAXIMUM RATE TO STEM AT THE FIRST YEAR AFTER COPPICING
		"YEARS_FOR_CONVERSION",        //years from coppicing to consider tree as a timber


		//FINE COARSE ROOT RATIO
		"FINE_ROOT_LEAF",	//allocation new fine root C:new leaf (ratio)
		"STEM_LEAF",		//allocation new stem C:new leaf (ratio)
		"COARSE_ROOT_STEM",	//allocation new coarse root C:new stem (ratio)

		//per specie caducifoglie LITTERFALL RATE = 1 !!!!!!!!!!!!!!
		//LITTERFALL
		//FOLLOWING BIOME-BGC
		"LEAF_FALL_FRAC_GROWING",		//proportions of the growing season of leaf fall
		"GAMMAFX",                    //Maximum Litterfall rate (month^-1)
		"GAMMAF0",                    //Litterfall rate at t = 0 (month^-1)
		"TGAMMAF",                    //Age at which litterfall rate has median value

		//ROOT TURNOVER
		"FINERTTOVER",                    //Average yearly fine root turnover rate
		"COARSERTTOVER",                  //Average yearly coarse root turnover rate
		"SAPWOODTTOVER",	              //Average yearly sapwood turnover rate
		"BRANCHTTOVER",	                  //Average yearly branch turnover rate


		//MORTALITY
		"WSX1000",                    //Max stem mass (kg) per tree at 1000 trees/hectare
		"THINPOWER",                  //Power in self-thinning rule
		"MF",                         //Fraction mean single tree foliage biomass lost per dead tree
		"MR",                         //Fraction mean single tree root biomass lost per dead tree
		"MS",                         //Fraction mean single tree stem biomass lost per dead tree

		//ALLOMETRIC RELATIONSHIPS

		//"DBHDC",                      //dbh (cm)- crown diameter (e) ratio  from cm to meter cm-->m
		//DBHDC = 20/100
		"DBHDCMAX",                   //Low Density
		"DBHDCMIN",                   //High Density
		"SAP_A",                      //a coefficient for sapwood
		"SAP_B",                      //b coefficient for sapwood
		"SAP_LEAF",                   //sapwood_max leaf area ratio in pipe model
		"SAP_WRES",					  //Sapwood-Reserve biomass ratio used if no Wres data are available
		"HMAX",                       //Max Height in m
		"DMAX",                       //Max Diameter in cm
		"HPOWER",                     //Slope of Asymptotic Height from Sortie
		"RPOWER",                     //Slope of Asymptotic Crown-Radius from Sortie
		"CHPOWER",                    //Slope of Asymptotic Crown-Height from Sortie



		//CHAPMAN-RICHARDS relationships
		"CRA",
		"CRB",
		"CRC",

		//CROWDING COMPETITION FUNCTION
		"HDMAX",                      //Height to Base diameter ratio MAX
		"HDMIN",                      //Height to Base diameter ratio MIN

		//DENSITY FUNCTION
		"DENMAX",                     //Maximum density (trees/10000 m^2)
		"DENMIN",                     //Minimum density (trees/10000 m^2)

		//ESTABLISHMENT
		"MINPAREST",                  //Minimum Monthly PAR (W/m^2 hour) for Establishment for the Dominated Layer
		"MINRAIN",                    //Minimum annual Rain Precipitation for Establishment

		//SEEDS PRODUCTION
		"ADULT_AGE",
		"MAXSEED",                    //numero massimo semi prodotti dalla pianta (da TREEMIG)
		"MASTSEED",                   //ricorrenza anni di pasciona (da TREEMIG)
		"WEIGHTSEED",                 //peso frutto in g
		"SEXAGE",
		"GERMCAPACITY",               //Geminability (Lischke H. & Loffler T. J.)
		"MINTEMP",                    //Minimum temperature for germination in °C
		"ESTMAX",                     //Potential Establishment rate in the absence of competition

		//SEEDS PRODUCTION FROM LPJ
		"FRACFRUIT",                  //Fraction of NPP to Fruit Production


		//MANAGMENT
		"ROTATION",
		"MINAGEMANAG",
		"MINDBHMANAG",                //Minimum DBH for Managment
		"AV_SHOOT",                   //Average number of shoots produced after coppicing
};
static const char species_values_delimiter[] = " \t\r\n";

/* error strings */
extern const char err_out_of_memory[];
static const char err_equal_rows[] = "equal values at row %d\n";
static const char err_unable_open_file[] = "unable to open file \"%s\".\n";

/* */
static int alloc_struct(void **t, int *count, unsigned int size)
{
	void *no_leak;

	/* alloc memory */
	no_leak = realloc(*t, ++*count*size);
	if ( !no_leak )
	{
		--*count;
		return 0;
	}

	/* assign pointer */
	*t = no_leak;

	/* ok */
	return 1;
}

/* */
static int fill_cell_from_species(AGE *const a, const ROW *const row)
{
	int i;

	// check parameter
	assert(a && row);

	if ( !alloc_struct((void **)&a->species, &a->species_count, sizeof(SPECIES)) )
	{
		return 0;
	}
	//  reset
	for ( i = 0; i < VALUES; i++ )
	{
		a->species[a->species_count-1].value[i] = INVALID_VALUE;
	}

	for ( i = 0; i < COUNTERS; i++ )
	{
		a->species[a->species_count-1].counter[i] = INVALID_VALUE;
	}
	// set values

	//a->species[a->species_count-1].phenology = row->phenology;
	a->species[a->species_count-1].management = row->management;
	a->species[a->species_count-1].name = mystrdup(row->species);
	a->species[a->species_count-1].counter[N_TREE] = row->n;
	a->species[a->species_count-1].counter[N_STUMP] = row->stump;
	a->species[a->species_count-1].value[AVDBH] = row->avdbh;
	a->species[a->species_count-1].value[BIOMASS_FOLIAGE_CTEM] = row->wf;
	a->species[a->species_count-1].value[BIOMASS_ROOTS_COARSE_CTEM] = row->wrc;
	a->species[a->species_count-1].value[BIOMASS_ROOTS_FINE_CTEM] = row->wrf;
	a->species[a->species_count-1].value[BIOMASS_STEM_CTEM] = row->ws;
	a->species[a->species_count-1].value[BIOMASS_RESERVE_CTEM] = row->wres;
	a->species[a->species_count-1].value[LAI] = row->lai;

	/* check memory */
	if ( !a->species[a->species_count-1].name )
	{
		return 0;
	}

	// ok
	return 1;
}

/* */
static int fill_cell_from_ages(HEIGHT *const h, const ROW *const row)
{
	//check parameter
	assert ( h && row);


	if ( !alloc_struct((void **)&h->ages, &h->ages_count, sizeof(AGE)) )
	{
		return 0;
	}

	//set values
	h->ages[h->ages_count-1].value = row->age;
	h->ages[h->ages_count-1].species = NULL;
	h->ages[h->ages_count-1].species_count = 0;

	//add species
	return fill_cell_from_species(&h->ages[h->ages_count-1], row);
}

/* */
int fill_cell_from_heights(CELL *const c, const ROW *const row)
{
	//check parameter
	assert(c && row);

	if (!alloc_struct((void **)&c->heights, &c->heights_count, sizeof(HEIGHT)) )
	{
		return 0;
	}

	// set values
	c->heights[c->heights_count-1].value = row->height;
	c->heights[c->heights_count-1].ages = NULL;
	c->heights[c->heights_count-1].ages_count = 0;

	// add age
	return fill_cell_from_ages(&c->heights[c->heights_count-1], row);
}

/* */
static int fill_cell(MATRIX *const m, const ROW *const row)
{
	/* check parameter */
	assert(m && row);

	if ( !alloc_struct((void **)&m->cells, &m->cells_count, sizeof(CELL)) )
	{
		return 0;
	}

	/* set values */
	m->cells[m->cells_count-1].landuse = row->landuse;
	m->cells[m->cells_count-1].x = row->x;
	m->cells[m->cells_count-1].y = row->y;
	m->cells[m->cells_count-1].heights = NULL;
	m->cells[m->cells_count-1].heights_count = 0;

	/* add species */
	return fill_cell_from_heights(&m->cells[m->cells_count-1], row);
}

/* */
MATRIX *matrix_create(ROW *const rows, const int rows_count, char* in_dir)
{
	int i;
	int y;
	int row;
	int cell;
	int species;
	int age;
	int height;
	int result;
	unsigned char equal_flag;
	MATRIX *m;
	FILE *f;
	char *token;
	char *token2;
	char *p;
	char filename[PATH_SIZE];
	char buffer[BUFFER_SIZE];
	PREC value;

	/* check parameters */
	assert(rows && rows_count);

	/* alloc memory for a matrix */
	m = malloc(sizeof*m);
	if ( !m )
	{
		return NULL;
	}

	/* init matrix */
	m->cells = NULL;
	m->cells_count = 0;

	/* loop on each rows */
	for ( row = 0; row < rows_count; row++ )
	{
		/* reset */
		equal_flag = 0;
		age = 0;
		species = 0;
		cell = 0;

		/* loop on each cell */
		for ( cell = 0; cell < m->cells_count; cell++ )
		{
			/* check cell */
			if ( ARE_FLOATS_EQUAL(rows[row].x, m->cells[cell].x) && ARE_FLOATS_EQUAL(rows[row].y, m->cells[cell].y) )
			{
				/* update flag */
				equal_flag |= EQUAL_CELL;

				for (height = 0; height < m->cells[cell].heights_count; ++height)
				{
					//equal heights?
					if ( ARE_FLOATS_EQUAL(rows[row].height, m->cells[cell].heights[height].value) )
					{
						//update flag
						equal_flag |= EQUAL_HEIGHT;

						// loop on each heights (check for ages)
						for (age = 0;  age < m->cells[cell].heights[height].ages_count; ++age )
						{
							// equal age ?
							if ( rows[row].age == m->cells[cell].heights[height].ages[age].value )
							{
								// update flag
								equal_flag |= EQUAL_AGE;

								// loop on each species
								for ( species = 0; species < m->cells[cell].heights[height].ages[age].species_count; ++species )
								{
									// equal species?
									if ( !mystricmp(rows[row].species, m->cells[cell].heights[height].ages[age].species[species].name) )
									{
										/* */
										Log((char *)err_equal_rows, row + 1);
										matrix_free(m);
										return NULL;
									}
								}
								//exit loop
								break;
							}
						}
						//exit loop
						break;
					}
				}
				// exit loop
				break;
			}
		}

		/* fill by flag */
		if ( IS_FLAG_SET(equal_flag, EQUAL_AGE) )
		{
			result = fill_cell_from_species(&m->cells[cell].heights[height].ages[age], &rows[row]);
		}
		else if ( IS_FLAG_SET(equal_flag, EQUAL_HEIGHT) )
		{
			result = fill_cell_from_ages(&m->cells[cell].heights[height], &rows[row]);
		}
		else if ( IS_FLAG_SET(equal_flag, EQUAL_CELL) )
		{
			result = fill_cell_from_heights(&m->cells[cell], &rows[row]);
		}
		else
		{
			result = fill_cell(m, &rows[row]);
		}

		/* check result */
		if ( !result )
		{
			Log(err_out_of_memory);
			matrix_free(m);
			return NULL;
		}
	}

	/* fill with species values */
	for ( cell = 0; cell < m->cells_count; cell++ )
	{
		for (height = 0; height < m->cells[cell].heights_count; ++height)
		{
			for (age = 0;  age < m->cells[cell].heights[height].ages_count; ++age )
			{
				for ( species = 0; species < m->cells[cell].heights[height].ages[age].species_count; ++species )
				{
					sprintf(filename, "%s/%s.txt", in_dir, m->cells[cell].heights[height].ages[age].species[species].name);
					f = fopen(filename, "r");
					if ( !f )
					{
						Log(err_unable_open_file, filename);
						matrix_free(m);
						return NULL;
					}

					//Log("importing %s...\n", filename);

					// RESET
					y = 0;
					while ( fgets(buffer, BUFFER_SIZE, f) )
					{
						// REMOVE \r\n
						for ( i = 0; buffer[i]; i++ )
						{
							if ( '\r' == buffer[i] )
							{
								buffer[i] = '\0';
							}

							if ( '\n' == buffer[i] ) {
								buffer[i] = '\0';
							}
						}

						// SKIP EMPTY LINE
						if ( '\0' == buffer[0] ) {
							continue;
						}

						// SKIP COMMENTS
						if ( ('\\' == buffer[0]) && ('\\' == buffer[1]) ) {
							continue;
						}

						// GET VARIABLE NAME
						token = mystrtok(buffer, species_values_delimiter, &p);
						if ( !token ) {
							Log("unable to get value token in file \"%s\", line %s.\n", filename, buffer);
							matrix_free(m);
							return NULL;
						}

						// SEARCH FOR STRING
						for ( i = 0; i < SIZE_OF_ARRAY(species_values); ++i ) {
							if ( !strcmp(species_values[i], token) ) {
								// GET VALUE TOKEN
								token2 = mystrtok(NULL, species_values_delimiter, &p);
								if ( !token2 ) {
									Log("unable to get value for \"%s\" in \"%s\".\n", token, filename);
									matrix_free(m);
									return NULL;
								}

								// CONVERT TOKEN
								value = convert_string_to_prec(token2, &result);
								if ( result ) {
									Log("unable to convert value \"%s\" for \"%s\" in \"%s\".\n", token2, token, filename);
									matrix_free(m);
									return NULL;
								}

								// ASSIGN VALUE
								m->cells[cell].heights[height].ages[age].species[species].value[i] = value;

								// KEEP TRACK OF ASSIGNED VALUES
								++y;

								// DEBUG
								//Log("imported %s = %g (row %d)\n", token, value, i);

								break;
							}
						}
					}

					// CHECK ASSIGNED SPECIES VALUES
					if ( y != SIZE_OF_ARRAY(species_values) ) {
						Log("error: assigned %d species value instead of %d\n", y, SIZE_OF_ARRAY(species_values));
						matrix_free(m);
						return NULL;
					}
				}

				/* convert SLA in m^2/Kg */
				for ( species = 0; species < m->cells[cell].heights[height].ages[age].species_count; ++species )
				{
					m->cells[cell].heights[height].ages[age].species[species].value[SLAmkg] = m->cells[cell].heights[height].ages[age].species[species].value[SLA] * SLAMKG;
				}
			}
		}
	}
	/* return pointer */
	return m;
}



/* */
void matrix_summary(const MATRIX *const m, int years, const YOS *const yos )
{
	int cell;
	int species;
	int age;
	int height;
	int resol;




	//check parameter
	assert (m);

	Log ("RUN DATASET (COMPSET)\n");

	//cell MUST be squares
	resol = sqrt (settings->sizeCell);

	Log ("Cell resolution = %d x %d = %g m^2\n", resol, resol, settings->sizeCell);
	if (settings->version == 'f')
	{
		Log ("Model version = FEM \n");
	}
	else
	{
		Log("Model version = BGC \n");
	}

	if (settings->spatial == 's')
	{
		Log ("Model spatial = spatial \n");
	}
	else
	{
		Log ("Model spatial = un-spatial \n");
	}

	if (settings->time == 'm')
	{
		Log ("Temporal scale = monthly \n");
	}
	else
	{
		Log ("Temporal scale = daily \n");
	}

	if (settings->presence == 't')
	{
		Log ("Vegetation Presence = total \n");
	}
	else
	{
		Log ("Vegetation Presence = percentage \n");
	}

	/*Site definition*/
	Log("***************************************************\n");
	Log("SITE DATASET\n");
	Log("Site Name = %s\n", site->sitename);
	Log("Latitude = %g \n", site->lat);
	Log("Longitude = %g \n", site->lon);
	Log("***************************************************\n");


	//loop on each cell
	for ( cell = 0; cell< m->cells_count; cell++)
	{
		if (m->cells[cell].landuse == F)
		{
			Log ("FOREST DATASET\n");
			Log("matrix has %d cell%s\n", m->cells_count, (m->cells_count > 1 ? "s" : ""));
			Log("****GET FOREST CHARACTERISTICS for cell  (%g, %g)****\n", m->cells[cell].x, m->cells[cell].y);
			Log("- cell n.%02d is at %g, %g and has %d height classes \n",
					cell+1,
					m->cells[cell].x,
					m->cells[cell].y,
					m->cells[cell].heights_count);

			//loop on each height
			for ( height = 0; height < m->cells[cell].heights_count; height++ )
			{
				Log("**(%d)\n", height + 1);
				Log("-- height n.%02d is %g m and has %d age classes\n",
						height + 1,
						m->cells[cell].heights[height].value,
						m->cells[cell].heights[height].ages_count);

				//loop on each age
				for ( age = 0; age < m->cells[cell].heights[height].ages_count; age++ )
				{
					Log("--- age n.%02d is %d yrs and has %d species '%s'\n\n",
							age + 1,
							m->cells[cell].heights[height].ages[age].value,
							m->cells[cell].heights[height].ages[age].species_count,
							m->cells[cell].heights[height].ages[age].species[species].name);

					// loop on each species
					for ( species = 0; species < m->cells[cell].heights[height].ages[age].species_count; species ++)
					{
						//*************FOREST INITIALIZATION DATA***********

						Get_a_Power_Function (&m->cells[cell].heights[height].ages[age], &m->cells[cell].heights[height].ages[age].species[species]);

						//IF NO BIOMASS INITIALIZATION DATA OR TREE HEIGHTS ARE AVAILABLE FOR STAND BUT JUST DENDROMETRIC VARIABLES (i.e. AVDBH, HEIGHT)
						//HEIGHT VALUES ARE MANDATORY
						if (	m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM]== 0 &&
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM]== 0 &&
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM]== 0 &&
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM] == 0)
						{
							Get_initialization_biomass_data (&m->cells[cell].heights[height].ages[age].species[species], yos, years);
						}

						Log(
								"----- height = %g\n"
								"----- age = %d\n"
								"----- species n.%02d is %s\n"
								"----- n = %d trees\n"
								"----- n stumps = %d stumps\n"
								"----- avdbh = %g cm\n"
								"----- wf = %g tDM/ha\n"
								"----- wr coarse = %g tDM/area\n"
								"----- wr fine = %g tDM/area\n"
								"----- ws = %g tDM/area\n"
								"----- wres = %g tDM/area\n"
								"----- lai = %g tDM/area\n",


								m->cells[cell].heights[height].value,
								m->cells[cell].heights[height].ages[age].value,
								species + 1,
								m->cells[cell].heights[height].ages[age].species[species].name,
								m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE],
								m->cells[cell].heights[height].ages[age].species[species].counter[N_STUMP],
								m->cells[cell].heights[height].ages[age].species[species].value[AVDBH],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE_CTEM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE_CTEM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE_CTEM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_CTEM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_RESERVE_CTEM],
								m->cells[cell].heights[height].ages[age].species[species].value[LAI]);

					}
				}
			}

			Get_initialization_site_data (&m->cells[cell]);

		}
		else
		{
			Log ("*********************\n\n\n");
			Log ("CROP DATASET\n");
			Log("*(%d)\n", cell + 1);

		}
	}

}

/*free matrix */
void matrix_free(MATRIX *m)
{
	int cell;
	int age;
	int height;


	if ( m )
	{
		if ( m->cells_count )
		{
			for ( cell = 0 ; cell < m->cells_count; cell++)
			{
				if ( m->cells[cell].heights_count )
				{
					for ( height = 0; height < m->cells[cell].heights_count; height++ )
					{
						if ( m->cells[cell].heights[height].ages_count )
						{
							for ( age = 0; age < m->cells[cell].heights[height].ages_count; age++ )
							{
								if ( m->cells[cell].heights[height].ages[age].species )
								{
									free ( m->cells[cell].heights[height].ages[age].species);
								}
							}
							free ( m->cells[cell].heights[height].ages);
						}
					}
					free ( m->cells[cell].heights);
				}
			}
			free ( m->cells);
		}
		free (m);
		m = NULL;
	}
}
