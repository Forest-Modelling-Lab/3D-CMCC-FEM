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

		"PHENOLOGY",				//PHENOLOGY 0.1 = deciduous broadleaf, 0.2 = deciduous needle leaf, 1.1 = broad leaf evergreen, 1.2 = needle leaf evergreen

		"ALPHA",                      // Canopy quantum efficiency (molC/molPAR)
		"EPSILONgCMJ",               // Light Use Efficiency  (gC/MJ)(used if ALPHA is not available)
		//"Y",                          // Assimilate use efficiency-Respiration rate-GPP/NP
		//"EPSILONgCMJ",                // = ALPHA * GC_MOL / MOLPAR_MJ = gC/MJ


		"K",                          //Extinction coefficient for absorption of PAR by canopy for Quercus cerris L. (A. Cutini, Ann Sci For, 1996)
		"ALBEDO",
		"GAMMA_LIGHT",



		//LEAF AREA INDEX
		"LAIGCX",                     //LAI for maximum canopy conductance
		"LAIMAXINTCPTN",              //LAI for maximum rainfall interception
		"MAXINTCPTN",                 //Maximum proportion of rainfall interception evaporated from canopy for Quercus spp Breuer et al 2003

		//SPECIFIC LEAF AREA
		"SLA",                        //Specific Leaf Area cm^2/g for sunlit leaves
		"SLA_AVG",                    //AVERAGE Specific Leaf Area cm^2/g for sunlit/shaded leaves
		"SLA_RATIO",                  //(DIM) ratio of shaded to sunlit projected SLA
		"LAI_RATIO",                  //(DIM) all-sided to projected leaf area ratio

		//FRACTION BRANCH-BARK
		"FRACBB0",                    //Branch and Bark fraction at age 0 (m^2/kg)
		"FRACBB1",                    //Branch and Bark fraction for mature stands (m^2/kg)
		"TBB",                        //Age at which fracBB = (FRACBB0 + FRACBB1 )/ 2

		//MASS DENSITY
		"RHOMIN",                     //Minimum Basic Density for young Trees
		"RHOMAX",                     //Maximum Basic Density for young Trees (Ferrara-Nolè)
		"TRHO",                       //Age at which rho = (RHOMIN + RHOMAX )/2

		//VPD
		"COEFFCOND",                  //Define stomatal responsee to VPD in m/sec
		"BLCOND",                     //Canopy Boundary Layer conductance
		"MAXCOND",                    //Maximum leaf Conductance in m/sec

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

		"FRUIT_PERC",
		"CONES_LIFE_SPAN",



		//BIOME ALLOCATION RATIOS
		"FINE_ROOT_LEAF",	//allocation new fine root C:new leaf (ratio)
		"STEM_LEAF",		//allocation new stem C:new leaf (ratio)
		"COARSE_ROOT_STEM",	//allocation new coarse root C:new stem (ratio)
		"LIVE_TOTAL_WOOD",	//allocation new live wood C:new total wood C (ratio)

		//BIOME C:N RATIOS
		"CN_LEAVES",  //CN of leaves (kgC/kgN)
		"CN_LITTER",  //CN of leaf litter (kgC/kgN)
		"CN_FINE_ROOTS",  // CN of fine roots (kgC/kgN)
		"CN_LIVE_WOODS",  //CN of live woods (kgC/kgN)
		"CN_DEAD_WOODS",  //CN of dead woods (kgC/kgN)

		//per specie caducifoglie LITTERFALL RATE = 1 !!!!!!!!!!!!!!
		//LITTERFALL
		"BUD_BURST",					//days of bud burst at the beginning of growing season (only for deciduous)
		//FOLLOWING BIOME-BGC
		"LEAF_FALL_FRAC_GROWING",		//proportions of the growing season of leaf fall
		"LEAF_LIFE_SPAN",				//Leaf life span

		//ROOT TURNOVER
		"LEAVES_FINERTTOVER",             //Average yearly leaves and fine root turnover rate
		"COARSERTTOVER",                  //Average yearly coarse root turnover rate
		"SAPWOODTTOVER",	              //Average yearly sapwood turnover rate
		"BRANCHTTOVER",	                  //Average yearly branch turnover rate
		"LIVE_WOOD_TURNOVER",             //Average yearly live wood turnover rate


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
		"b_RPOWER",
		"CHPOWER",                    //Slope of Asymptotic Crown-Height from Sortie
		"b_CHPOWER",

		"STEMCONST_P",
		"STEMPOWER_P",



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
	static SPECIES species = { 0 };

	// check parameter
	assert(a && row);

	if ( !alloc_struct((void **)&a->species, &a->species_count, sizeof(SPECIES)) )
	{
		return 0;
	}

	a->species[a->species_count-1] = species;

	// set values
	//a->species[a->species_count-1].phenology = row->phenology;
	a->species[a->species_count-1].management = row->management;
	a->species[a->species_count-1].name = mystrdup(row->species);
	a->species[a->species_count-1].counter[N_TREE] = row->n;
	a->species[a->species_count-1].counter[N_STUMP] = row->stump;
	a->species[a->species_count-1].value[AVDBH] = row->avdbh;
	a->species[a->species_count-1].value[BIOMASS_FOLIAGE] = row->wf;
	a->species[a->species_count-1].value[BIOMASS_ROOTS_COARSE] = row->wrc;
	a->species[a->species_count-1].value[BIOMASS_ROOTS_FINE] = row->wrf;
	a->species[a->species_count-1].value[BIOMASS_STEM] = row->ws;
	a->species[a->species_count-1].value[BIOMASS_BRANCH] = row->wbb;
	a->species[a->species_count-1].value[BIOMASS_RESERVE] = row->wres;
	a->species[a->species_count-1].value[LAI] = row->lai;

	/* check memory */
	if ( ! a->species[a->species_count-1].name )
	{
		return 0;
	}

	a->species[a->species_count-1].turnover = malloc(a->species_count*sizeof*a->species[a->species_count-1].turnover);
	if ( ! a->species[a->species_count-1].turnover ) {
		return 0;
	}

	return 1;
}

/* */
static int fill_cell_from_ages(HEIGHT *const h, const ROW *const row)
{
	static AGE age = { 0 };
	//check parameter
	assert ( h && row);


	if ( !alloc_struct((void **)&h->ages, &h->ages_count, sizeof(AGE)) )
	{
		return 0;
	}

	//set values
	h->ages[h->ages_count-1] = age;
	h->ages[h->ages_count-1].value = row->age;
	
	//add species
	return fill_cell_from_species(&h->ages[h->ages_count-1], row);
}

/* */
int fill_cell_from_heights_and_soils(CELL *const c, const ROW *const row)
{
	static HEIGHT height = { 0 };
	static SOIL soil = { 0 };

	//check parameter
	assert(c && row);

	//alloc memory for heights
	if (!alloc_struct((void **)&c->heights, &c->heights_count, sizeof(HEIGHT)) )
	{
		return 0;
	}
	c->heights[c->heights_count-1] = height;

	//alloc memory for soils
	if (!alloc_struct((void **)&c->soils, &c->soils_count, sizeof(SOIL)) )
	{
		return 0;
	}
	c->soils[c->soils_count-1] = soil;

	// set values
	c->heights[c->heights_count-1].value = row->height;

	// add age
	return fill_cell_from_ages(&c->heights[c->heights_count-1], row);
}


/* */
static int fill_cell(MATRIX *const m, const ROW *const row)
{
	static CELL cell = { 0 };

	/* check parameter */
	assert(m && row);

	if ( !alloc_struct((void **)&m->cells, &m->cells_count, sizeof(CELL)) )
	{
		return 0;
	}
	m->cells[m->cells_count-1] = cell;

	/* set values */
	m->cells[m->cells_count-1].landuse = row->landuse;
	m->cells[m->cells_count-1].x = row->x;
	m->cells[m->cells_count-1].y = row->y;
	//fixme without -1 the model gets 1 more!!
	m->cells[m->cells_count-1].soils_count = (int)settings->soil_layer -1;

	/* add species */
	return fill_cell_from_heights_and_soils(&m->cells[m->cells_count-1], row);
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

	Log("Creating matrix....\n");

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
			if ( ARE_DOUBLES_EQUAL(rows[row].x, m->cells[cell].x) && ARE_DOUBLES_EQUAL(rows[row].y, m->cells[cell].y) )
			{
				/* update flag */
				equal_flag |= EQUAL_CELL;

				for (height = 0; height < m->cells[cell].heights_count; ++height)
				{
					//equal heights?
					if ( ARE_DOUBLES_EQUAL(rows[row].height, m->cells[cell].heights[height].value) )
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
			result = fill_cell_from_heights_and_soils(&m->cells[cell], &rows[row]);
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
							fclose(f);
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
									fclose(f);
									return NULL;
								}

								// CONVERT TOKEN
								value = convert_string_to_prec(token2, &result);
								if ( result ) {
									Log("unable to convert value \"%s\" for \"%s\" in \"%s\".\n", token2, token, filename);
									matrix_free(m);
									fclose(f);
									return NULL;
								}

								// ASSIGN VALUE
								m->cells[cell].heights[height].ages[age].species[species].value[i] = value;

								// KEEP TRACK OF ASSIGNED VALUES
								++y;

								// DEBUG
								//Log("imported %s = %f (row %d)\n", token, value, i);

								break;
							}
						}
					}

					// CHECK ASSIGNED SPECIES VALUES
					if ( y != SIZE_OF_ARRAY(species_values) ) {
						Log("error: assigned %d species value instead of %d\n", y, SIZE_OF_ARRAY(species_values));
						matrix_free(m);
						fclose(f);
						return NULL;
					}
					fclose(f);
				}

				/* convert SLA in m^2/Kg */
				for ( species = 0; species < m->cells[cell].heights[height].ages[age].species_count; ++species )
				{
					m->cells[cell].heights[height].ages[age].species[species].value[SLAmkg] = m->cells[cell].heights[height].ages[age].species[species].value[SLA] * SLAMKG;
					m->cells[cell].heights[height].ages[age].species[species].value[SLA_AVG] *= SLAMKG;
				}
			}
		}
	}
	/* return pointer */
	return m;
}



/* */
void matrix_summary(const MATRIX *const m)
{
	int cell;
	int species;
	int age;
	int height;
	int resol;




	//check parameter
	assert (m);

	Log ("RUN COMPSET\n");

	//cell MUST be squares
	resol = (int)sqrt(settings->sizeCell);

	Log ("Cell resolution = %d x %d = %f m^2\n", resol, resol, settings->sizeCell);
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

	/*if (settings->time == 'm')
	{
		Log ("Temporal scale = monthly \n");
	}
	else*/
	{
		Log ("Temporal scale = daily \n");
	}

	if (settings->symmetric_water_competition == 'y')
	{
		Log ("Symmetric water competition\n");
	}
	else
	{
		Log ("Asymmetric water competition\n");
	}

	/*Site definition*/
	Log("***************************************************\n");
	Log("SITE DATASET\n");
	Log("Site Name = %s\n", site->sitename);
	Log("Latitude = %f° \n", site->lat);
	Log("Longitude = %f° \n", site->lon);
	Log("Elevation = %f m\n", site->elev);
	if (site->lat > 0) Log("North hemisphere\n");
	else Log("South hemisphere\n");
	Log("***************************************************\n");


	//loop on each cell
	for ( cell = 0; cell< m->cells_count; cell++)
	{
		if (m->cells[cell].landuse == F)
		{
			Log ("FOREST DATASET\n");
			Log("matrix has %d cell%s\n", m->cells_count, (m->cells_count > 1 ? "s" : ""));
			Log("****GET FOREST CHARACTERISTICS for cell  (%d, %d)****\n", m->cells[cell].x, m->cells[cell].y);
			Log("- cell n.%02d is at %d, %d and has %d height classes \n",
					cell+1,
					m->cells[cell].x,
					m->cells[cell].y,
					m->cells[cell].heights_count);

			//loop on each height
			for ( height = 0; height < m->cells[cell].heights_count; height++ )
			{
				Log("**(%d)\n", height + 1);
				Log("-- height n.%02d is %f m and has %d age classes\n",
						height + 1,
						m->cells[cell].heights[height].value,
						m->cells[cell].heights[height].ages_count);

				//loop on each age
				for ( age = 0; age < m->cells[cell].heights[height].ages_count; age++ )
				{
					Log("--- age n.%02d is %d yrs and has %d species \n\n",
							age + 1,
							m->cells[cell].heights[height].ages[age].value,
							m->cells[cell].heights[height].ages[age].species_count
					/*m->cells[cell].heights[height].ages[age].species[species].name*/);

					// loop on each species
					for ( species = 0; species < m->cells[cell].heights[height].ages[age].species_count; species ++)
					{

						Get_biome_fraction (&m->cells[cell].heights[height].ages[age].species[species]);


						//*************FOREST INITIALIZATION DATA***********

						Get_a_Power_Function (&m->cells[cell].heights[height].ages[age], &m->cells[cell].heights[height].ages[age].species[species]);

						//IF NO BIOMASS INITIALIZATION DATA OR TREE HEIGHTS ARE AVAILABLE FOR STAND BUT JUST DENDROMETRIC VARIABLES (i.e. AVDBH, HEIGHT)
						//HEIGHT VALUES ARE MANDATORY

						Get_initialization_biomass_data (&m->cells[cell].heights[height].ages[age].species[species], &m->cells[cell].heights[height]);


						Log(
								"\n\n----- CLASS DATASET-----\n"
								"----- height = %f\n"
								"----- age = %d\n"
								"----- species n.%02d is %s\n"
								"----- n = %d trees\n"
								"----- n stumps = %d stumps\n"
								"----- avdbh = %f cm\n"
								"----- wf = %f tDM/ha\n"
								"----- wr coarse = %f tDM/area\n"
								"----- wr fine = %f tDM/area\n"
								"----- wr tot = %f tDM/area\n"
								"----- ws = %f tDM/area\n"
								"----- wbb = %f tDM/area\n"
								"----- wres = %f tDM/area\n"
								"----- ws live = %f tDM/area\n"
								"----- wrc live = %f tDM/area\n"
								"----- wbb live = %f tDM/area\n"
								"----- w tot live = %f tDM/area\n"
								"----- w tot dead = %f tDM/area\n"
								"----- lai = %f tDM/area\n",

								m->cells[cell].heights[height].value,
								m->cells[cell].heights[height].ages[age].value,
								species + 1,
								m->cells[cell].heights[height].ages[age].species[species].name,
								m->cells[cell].heights[height].ages[age].species[species].counter[N_TREE],
								m->cells[cell].heights[height].ages[age].species[species].counter[N_STUMP],
								m->cells[cell].heights[height].ages[age].species[species].value[AVDBH],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_FOLIAGE],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_COARSE],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_FINE],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_ROOTS_TOT],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_BRANCH],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_RESERVE],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_LIVE_WOOD],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_COARSE_ROOT_LIVE_WOOD],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_STEM_BRANCH_LIVE_WOOD],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_LIVE_WOOD],
								m->cells[cell].heights[height].ages[age].species[species].value[BIOMASS_DEAD_WOOD],
								m->cells[cell].heights[height].ages[age].species[species].value[LAI]);

					}
				}
			}

			/*Soil definition*/
			Log("***************************************************\n");
			Log("SOIL DATASET\n");
			Log("Number of soil layers = %f\n", settings->soil_layer);
			Log("***************************************************\n");

			Get_initialization_site_data (&m->cells[cell]);

		}
		else if (m->cells[cell].landuse == Z)
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
	int i;
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
									for ( i = 0; i < m->cells[cell].heights[height].ages[age].species_count; ++i ) {
										if ( m->cells[cell].heights[height].ages[age].species[i].name ) {
											free(m->cells[cell].heights[height].ages[age].species[i].name);
										}
										if ( m->cells[cell].heights[height].ages[age].species[i].turnover ) {
											free(m->cells[cell].heights[height].ages[age].species[i].turnover);
										}
									}

									free ( m->cells[cell].heights[height].ages[age].species);
								}
							}
							free ( m->cells[cell].heights[height].ages);
						}
					}
					free ( m->cells[cell].heights);
					free ( m->cells[cell].soils);
				}
				free (m->cells[cell].years);
			}
			free (m->cells);
		}
		free (m);
	}
}



