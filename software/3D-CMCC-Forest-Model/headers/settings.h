/* settings.h */
#ifndef SETTINGS_H_
#define SETTINGS_H_

enum { CO2_TRANS_OFF, CO2_TRANS_ON, CO2_TRANS_VAR };

#define SETTINGS_SITENAME_MAX_SIZE					(255+1)
#define SETTINGS_REPLANTED_SPECIES_MAX_SIZE			(32+1)
#define SETTINGS_REGENERATION_SPECIES_MAX_SIZE		(32+1)

enum { THINNING_REGIME_ABOVE, THINNING_REGIME_BELOW };

typedef struct {
	char sitename[SETTINGS_SITENAME_MAX_SIZE];
	char version;                        /* fixme to remove */
	char spatial;	                     /* spatial or unspatial must be 's' or 'u' (not longer used) fixme to remove */
	char time;		                     /* time scale simulation must be 'm' or 'd' (monthly or daily) */
	char screen_output;                  /* screen output must be 'on' or 'off' */
	char debug_output;                   /* debug output must be 'on' or 'off' */
	char daily_output;                   /* daily output must be 'on' or 'off' */
	char monthly_output;                 /* monthly output must be 'on' or 'off' */
	char yearly_output;                  /* annual output must be 'on' or 'off' */
	char soil_output;                    /* soil output must be 'on' or 'off' */
	int year_start;                      /* starting year of simulation */
	int year_end;                        /* ending year of simulation */
	int year_restart;                    /* year to restart simulation */
	char CO2_mod;                        /* CO2 modifier for photosynthesis and conductance must be 'on' or 'off' */
	char CO2_trans;                      /* CO2 transient must be 'on' or 'off' or 'var' */
	int year_start_co2_fixed;            /* for CO2_trans = var, year at which CO2 is fixed */
	char Ndep_fixed;                     /* fixed Nitrogen deposition must be 'on' or 'off' */
	//ALESSIOR double Tbase_resp;                   /* T base temperature for respiration (°C) */
	char Resp_accl;                      /* acclimation of repiration must be 'on' or 'off' */
	char regeneration;                   /* regeneration must be 'on' or 'off' */
	char management;                     /* management must be 'on' of 'off' */
	int year_start_management;           /* start year management (if management is on) */
	char Prog_Aut_Resp;                  /* Prognostic autotrophic respiration */
	char thinning_regime;                /* thinning regime above = A or below = B */
	char replanted_species[SETTINGS_REPLANTED_SPECIES_MAX_SIZE];       /* species name of replanted species */
	char regeneration_species[SETTINGS_REGENERATION_SPECIES_MAX_SIZE]; /* species name of regeneration species */
	// ALESSIOR: use e_management from matrix.h not char
	char replanted_management;           /*this is a mandatory variables that each class has to have AND DETERMINES HABITUS OF PLANT*/
	char regeneration_management;        /*this is a mandatory variables that each class has to have AND DETERMINES HABITUS OF PLANT*/


	/* DO NOT MODIFY BELOW, PLEASE INSERT STUFF IN SPACE ABOVE */

	double sizeCell;                     /* side length of the cell (meters) */

	double Fixed_Aut_Resp_rate;          /* It is the "Y"values for fixed autotrophic respiration */

	double co2Conc;                      /* reference CO2 concentration (ppmv) */
	double co2_incr;                     /* annual CO2 increment fixme to remove */

	double init_frac_maxasw;             /* minimum fraction of available soil water at the beginning of simulation */

	/* layer settings */
	double tree_layer_limit;             /* differences among tree heights before to consder different canopy layers (meters) */
	double number_of_soil_layer;         /* number of soil layers to simulate */
	double max_layer_cover;              /* maximum layer cover */

	/* replanted input (Management options)*/
	double replanted_n_tree;             /* number of replanted trees per sizecell (mandatory) */
	double replanted_age;                /* age of replanted trees (year)(mandatory) */
	double replanted_avdbh;              /* average dbh of replanted trees (cm) (mandatory) */
	double replanted_lai;                /* LAI of replanted trees (m2/m2) (mandatory for evergreen) */
	double replanted_height;             /* height of replanted trees (meters) (mandatory) */
	/* biomass */
	double replanted_ws;                 /* stem biomass of replanted trees (tDM/cell) (probably no need to be used) (optional)*/
	double replanted_wcr;                /* coarse root of replanted trees (tDM/cell) (probably no need to be used) (optional) */
	double replanted_wfr;                /* fine root biomass of replanted trees (tDM/cell) (probably no need to be used) (optional) */
	double replanted_wl;                 /* leaf biomass of replanted trees (tDM/cell) (probably no need to be used) (optional) */
	double replanted_wbb;                /* branch biomass of replanted trees (tDM/cell)  (probably no need to be used) (optional) */

	/* regeneration input (Regeneration options)*/
	double regeneration_n_tree;
	double regeneration_age;             /* age of regeneration (1 year) */
	double regeneration_avdbh;           /* dbh for regeneration (cm) */
	double regeneration_lai;             /* regeneration LAI (m2/m2) (mandatory for evergreen) */
	double regeneration_height;          /* regeneration height (meter) */
	/* biomass */
	double regeneration_ws;              /* stem biomass of regeneration trees (tDM/cell) (probably no need to be used) (optional)*/
	double regeneration_wcr;             /* coarse root of regeneration trees (tDM/cell) (probably no need to be used) (optional) */
	double regeneration_wfr;             /* fine root biomass of regeneration trees (tDM/cell) (probably no need to be used) (optional) */
	double regeneration_wl;              /* leaf biomass of regeneration trees (tDM/cell) (probably no need to be used) (optional) */
	double regeneration_wbb;             /* branch biomass of regeneration trees (tDM/cell) (probably no need to be used) (optional) */
} settings_t;

settings_t* settings_import(const char *const filename);
void settings_free(settings_t* s);

#endif /* SETTINGS_H_ */
