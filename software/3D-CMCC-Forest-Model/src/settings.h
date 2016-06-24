/* settings.h */
#ifndef SETTINGS_H_
#define SETTINGS_H_

#define SETTINGS_REPLANTED_SPECIES_MAX_SIZE		(32+1)

typedef struct {
	char version;
	char spatial;	/* must be 's' or 'u' (spatial or unspatial) */
	char time;		/* must be 'm' or 'd' (monthly or daily) */
	char symmetric_water_competition; /* must be 'y' or 'n' (y = yes for symmetric competition) */

	char spin_up[4];
	char CO2_fixed[4];
	char Ndep_fixed[4];
	char management[4];
	char Prog_Aut_Resp[4]; /* Prognostic autotrophic respiration */
	char dndc[4];
	char replanted_species[SETTINGS_REPLANTED_SPECIES_MAX_SIZE]; /* species name of replanted species */

	/* DO NOT MODIFY BELOW, PLEASE INSERT STUFF IN SPACE ABOVE */

	double sizeCell;

	double Fixed_Aut_Resp_rate; /* It is the "Y"values for fixed autotrophic respiration */

	double co2Conc, co2_incr; /* Co2 concentration and annual co2 increment */

	double init_frac_maxasw; /* minimum fraction of available soil water at the beginning of simulation */

	double tree_layer_limit;
	double soil_layer;
	double min_layer_cover;
	double max_layer_cover;

	/* management / renovation (human or natural) input */
	double removing_basal_area; /* percentage of basal area to remove per sizecell */
	double replanted_tree;		/* number of replanted trees per sizecell */
	double age_sapling;
	double avdbh_sapling;
	double lai_sapling;
	double height_sapling;
	double ws_sapling;			/* probably no need to be used */
	double wr_sapling;			/* probably no need to be used */
	double wf_sapling;			/* probably no need to be used */
	double light_estab_very_tolerant;
	double light_estab_tolerant;
	double light_estab_intermediate;
	double light_estab_intolerant;

	/* control check */
	double maxlai;
	double defaultlai;
	double switchtounspatial;
} settings_t;

settings_t* settings_import(const char *const filename);
void settings_free(settings_t* s);

#endif /* SETTINGS_H_ */
