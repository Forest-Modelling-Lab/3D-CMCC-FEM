/* settings.h */
#ifndef SETTINGS_H_
#define SETTINGS_H_

#define SETTINGS_REPLANTED_SPECIES_MAX_SIZE		(32+1)
#define SETTINGS_REGENERATION_SPECIES_MAX_SIZE		(32+1)

typedef struct {
	char version;
	char spatial;	                  /* must be 's' or 'u' (spatial or unspatial) */
	char time;		                  /* must be 'm' or 'd' (monthly or daily) */
	char symmetric_water_competition; /* must be 'y' or 'n' (y = yes for symmetric competition) */

	// ALESSIOR TODO 
	// REPLACE CHAR FOR ON and OFF
	// WITH 1 and 0
	char CO2_mod[4];
	char CO2_fixed[4];
	char Ndep_fixed[4];
	char Q10_fixed[4];
	char regeneration[4];
	char management[4];
	char Prog_Aut_Resp[4]; /* Prognostic autotrophic respiration */
	char dndc[4];
	char replanted_species[SETTINGS_REPLANTED_SPECIES_MAX_SIZE]; /* species name of replanted species */
	char regeneration_species[SETTINGS_REGENERATION_SPECIES_MAX_SIZE]; /* species name of regeneration species */
	// ALESSIOR: use e_management from matrix.h not char
	char replanted_management;    /*this is a mandatory variables that each class has to have AND DETERMINES HABITUS OF PLANT*/
	char regeneration_management;    /*this is a mandatory variables that each class has to have AND DETERMINES HABITUS OF PLANT*/


	/* DO NOT MODIFY BELOW, PLEASE INSERT STUFF IN SPACE ABOVE */

	double sizeCell;

	double Fixed_Aut_Resp_rate; /* It is the "Y"values for fixed autotrophic respiration */

	double co2Conc, co2_incr; /* Co2 concentration and annual co2 increment */

	double init_frac_maxasw; /* minimum fraction of available soil water at the beginning of simulation */

	/* layer settings */
	double tree_layer_limit;
	double number_of_soil_layer;
	double min_layer_cover;
	double max_layer_cover;

	/* replanted input (Management options)*/
	//ALESSIOC TO ALESSIOR move remainig_basal_area to species.txt
	double remaining_basal_area;          /* percentage of basal area to remove per sizecell (mandatory) */
	double replanted_n_tree;             /* number of replanted trees per sizecell (mandatory) */
	double replanted_age;                /* age of replanted trees (mandatory) */
	double replanted_avdbh;              /* average dbh of replanted trees (mandatory) */
	double replanted_lai;                /* LAI of replanted trees (mandatory for evergreen) */
	double replanted_height;             /* height of replanted trees (mandatory) */
	/* biomass */
	double replanted_ws;                 /* stem biomass of replanted trees (probably no need to be used) (optional)*/
	double replanted_wcr;                /* coarse root of replanted trees (probably no need to be used) (optional) */
	double replanted_wfr;                /* fine root biomass of replanted trees (probably no need to be used) (optional) */
	double replanted_wl;                 /* leaf biomass of replanted trees (probably no need to be used) (optional) */
	double replanted_wbb;                /* branch biomass of replanted trees (probably no need to be used) (optional) */

	/* regeneration input (Regeneration options)*/
	double regeneration_n_tree;
	double regeneration_age;             /* age of regeneration (1 year) */
	double regeneration_avdbh;           /* dbh for regeneration */
	double regeneration_lai;             /* regeneration LAI (mandatory for evergreen) */
	double regeneration_height;          /* regeneration height */
	/* biomass */
	double regeneration_ws;              /* stem biomass of regeneration trees (probably no need to be used) (optional)*/
	double regeneration_wcr;             /* coarse root of regeneration trees (probably no need to be used) (optional) */
	double regeneration_wfr;             /* fine root biomass of regeneration trees (probably no need to be used) (optional) */
	double regeneration_wl;              /* leaf biomass of regeneration trees (probably no need to be used) (optional) */
	double regeneration_wbb;             /* branch biomass of regeneration trees (probably no need to be used) (optional) */

} settings_t;

settings_t* settings_import(const char *const filename);
void settings_free(settings_t* s);

#endif /* SETTINGS_H_ */
