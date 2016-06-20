/* types.h */
#ifndef TYPES_H
#define TYPES_H

/* precision */
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "constants.h"


#define NO_DATA -9999

/* enums */
typedef enum {
	F = 0,      //forest
	Z          //crop
} eLanduse;

/* */
typedef enum {
	T = 0,      //Timber
	C          //Coppice
} eManagement;

/* */
enum {
	JANUARY,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER,

	MONTHS
};

/* */
typedef struct {
	int n_days;
	PREC solar_rad;
	PREC tavg;          /* (deg C) daily average air temperature */
	PREC tmax;          /* (deg C) daily maximum air temperature */
	PREC tmin;          /* (deg C) daily minimum air temperature */
	PREC tday;          /* (deg C) daylight  daily air temperature */
	PREC tnight;        /* (deg C) nightime average daily air temperature */
	PREC tdew;          /* (deg C) dew average daily air temperature */
	PREC vpd;
	PREC rh_f;
	PREC ts_f;
	PREC prcp;
	PREC swc;
	PREC ndvi_lai;
	PREC daylength;
	PREC thermic_sum;   /* daily thermic sum */
	PREC rho_air;
	PREC tsoil;
	PREC et;
	PREC windspeed;
	PREC lh_vap;
	PREC lh_vap_soil;
	PREC lh_fus;
	PREC lh_sub;
	PREC air_pressure;
	PREC co2_conc;
	PREC es;            /* (KPa) weighted mean saturation vapour pressure at the air temperature */
	PREC ea;            /* (KPa) actual vapour pressure derived from relative humidity data */
	PREC psych;         /* (KPa/°C) psychrometric constant */

} MET_DAILY_DATA;

/* */
typedef struct {
	MET_DAILY_DATA d[31];
} MET_DATA;

/* */
typedef struct {
	int year;
	MET_DATA m[MONTHS];
} YOS; /* YEARS OF SIMULATION */

// Struct representing settings.txt content
#define SETTINGS_REPLANTED_SPECIES_MAX_SIZE		(32+1)
typedef struct
{
	char version,
	spatial, // must be 's' or 'u' (spatial or unspatial)
	time,  // must be 'm' or 'd' (monthly or daily)
	symmetric_water_competition; // must be 'y' or 'n' (y = yes for symmetric competition)

	char spin_up[4];
	char CO2_fixed[4];
	char Ndep_fixed[4];
	char management[4];
	char Prog_Aut_Resp[4]; //Prognostic autotrophic respiration
	char dndc[4];
	char replanted_species[SETTINGS_REPLANTED_SPECIES_MAX_SIZE]; /* species name of replanted species */

	/* DO NOT MODIFY BELOW, PLEASE INSERT STUFF IN SPACE ABOVE */

	double sizeCell;

	double Fixed_Aut_Resp_rate; //It is the "Y"values for fixed autotrophic respiration

	double co2Conc, co2_incr; // Co2 concentration and annual co2 increment

	double init_frac_maxasw; //minimum fraction of available soil water at the beginning of simulation

	double tree_layer_limit,
	soil_layer,
	min_layer_cover,
	max_layer_cover;

	/* management/renovation (human or natural) input */
	double removing_basal_area; /* percentage of basal area to remove per sizecell */
	double replanted_tree, /* number of replanted trees per sizecell */
	age_sapling;
	double avdbh_sapling,
	lai_sapling,
	height_sapling,
	ws_sapling, /* probably no need to be used */
	wr_sapling, /* probably no need to be used */
	wf_sapling, /* probably no need to be used */
	light_estab_very_tolerant,
	light_estab_tolerant,
	light_estab_intermediate,
	light_estab_intolerant;
	/* control check */
	double maxlai,
	defaultlai;
	double 	switchtounspatial;

} settings_t;

/* */
enum
{
	/*valori relativi alla specie*/
	/* !!!!! NON SPOSTARE !!!!!!*/
	/* serve questo ordine per l'importazione, vedere species_values dentro matrix.c */
	LIGHT_TOL = 0,
	PHENOLOGY,                  //PHENOLOGY 0=deciduous, 1=evergreen
	ALPHA,                      // Canopy quantum efficiency (molC/molPAR)
	EPSILONgCMJ,                // Light Use Efficiency  (gC/MJ)(used if ALPHA is not available)
	K,                          //Extinction coefficient for absorption of PAR by canopy for Quercus cerris L. (A. Cutini, Ann Sci For, 1996)
	ALBEDO,
	GAMMA_LIGHT,
	INT_COEFF,                  //precip interception coefficient LAI-1day-1
	LAIGCX,                     //LAI for maximum canopy conductance
	LAIMAXINTCPTN,              //LAI for maximum rainfall interception
	MAXINTCPTN,                 //Maximum proportion of rainfall interception evaporated from canopy for Quercus spp Breuer et al 2003
	SLA_AVG,                    //AVERAGE Specific Leaf Area m^2/KgC for sunlit/shaded leaves
	SLA_RATIO,                  //(DIM) ratio of shaded to sunlit projected SLA
	LAI_RATIO,					//(DIM) all-sided to projected leaf area ratio
	FRACBB0,                    //Branch and Bark fraction at age 0 (m^2/kg)
	FRACBB1,                    //Branch and Bark fraction for mature stands (m^2/kg)
	TBB,                        //Age at which fracBB = (FRACBB0 + FRACBB1 )/ 2
	RHOMIN,                     //Minimum Basic Density for young Trees
	RHOMAX,                     //Maximum Basic Density for young Trees (Ferrara-Nolè)
	TRHO,                       //Age at which rho = (RHOMIN + RHOMAX )/2
	COEFFCOND,                  //Define stomatal responsee to VPD in m/sec TO REMOVE
	BLCOND,                     //Canopy Boundary Layer conductance
	MAXCOND,                    //Maximum leaf Conductance in m/sec
	CUTCOND,                    //cuticular conductance in m/sec
	MAXAGE,                     //Determines rate of "physiological decline" of forest
	RAGE,                       //Relative Age to give fAGE = 0.5
	NAGE,                       //Power of relative Age in function for Age
	MAXAGE_S,
	RAGE_S,                       //Relative Age to give fAGE = 0.5
	NAGE_S,                       //Power of relative Age in function for Age
	GROWTHTMIN,                 //Minimum temperature for growth
	GROWTHTMAX,                 //Maximum temperature for growth
	GROWTHTOPT,                 //Optimum temperature fro growth
	GROWTHSTART,                //Thermic sum  value for starting growth in °C
	GROWTHEND,                  //Thermic sum  value for ending growth in °C
	MINDAYLENGTH,               //minimum day length for phenology
	SWPOPEN,
	SWPCLOSE,
	SWCONST,                    //Costant in Soil Water modifier vs Moist Ratio
	SWPOWER,                    //Power in Soil Water modifier vs Moist Ratio
	OMEGA_CTEM,                        //ALLOCATION PARAMETER
	S0CTEM,                           //PARAMETER CONTROLLING ALLOCATION TO STEM
	R0CTEM,                           //PARAMETER CONTROLLING ALLOCATION TO ROOT
	F0CTEM,                           //PARAMETER CONTROLLING ALLOCATION TO FOLIAGE
	RES0CTEM,                       //PARAMETER CONTROLLING ALLOCATION TO RESERVE
	EPSILON_CTEM,                     //ALLOCATION PARAMETER IN (Kgcm^2)^-0.6
	KAPPA,                            //ALLOCATION PARAMETER DIMENSIONLESS
	MIN_R0CTEM,                       //MINIMUM RATE TO ROOT AT THE FIRST YEAR AFTER COPPICING
	MAX_S0CTEM,                       //MAXIMUM RATE TO STEM AT THE FIRST YEAR AFTER COPPICING
	YEARS_FOR_CONVERSION,        //years from coppicing to consider tree as a timber
	FRUIT_PERC,
	CONES_LIFE_SPAN,
	FINE_ROOT_LEAF,	//allocation new fine root C:new leaf (ratio)
	STEM_LEAF,		//allocation new stem C:new leaf (ratio)
	COARSE_ROOT_STEM,	//allocation new coarse root C:new stem (ratio)
	LIVE_TOTAL_WOOD,	//new live C:new total wood (ratio)
	CN_LEAVES,  //CN of leaves (kgC/kgN)
	CN_LITTER,  //CN of leaf litter (kgC/kgN)
	CN_FINE_ROOTS,  // CN of fine roots (kgC/kgN)
	CN_LIVE_WOODS,  //CN of live woods (kgC/kgN)
	CN_DEAD_WOODS,  //CN of dead woods (kgC/kgN)
	BUD_BURST,					//days of bud burst at the beginning of growing season (only for deciduous)
	LEAF_FALL_FRAC_GROWING,		//proportions of the growing season of leaf fall
	LEAF_LIFE_SPAN,				//Leaf life span
	LEAVES_FINERTTOVER,                    //Average daily fine root turnover rate
	COARSERTTOVER,                  //Average daily coarse root turnover rate
	SAPWOODTTOVER,	                //Average daily sapwood turnover rate
	BRANCHTTOVER,	                //Average daily branch turnover rate
	LIVE_WOOD_TURNOVER,             //Average daily live wood turnover rate
	WSX1000,                    //Max stem mass (kg) per tree at 1000 trees/hectare
	THINPOWER,                  //Power in self-thinning rule
	MF,                         //Fraction mean single tree foliage biomass lost per dead tree
	MR,                         //Fraction mean single tree root biomass lost per dead tree
	MS,                         //Fraction mean single tree stem biomass lost per dead tree
	DBHDCMAX,                   //Low Density
	DBHDCMIN,                   //High Density
	SAP_A,                      //a coefficient for sapwood
	SAP_B,                      //b coefficient for sapwood
	SAP_LEAF,                   //sapwood_max leaf area ratio in pipe model
	SAP_WRES,					  //Sapwood-Reserve biomass ratio used if no Wres data are available
	HMAX,                       //Max Height in m
	DMAX,                       //Max Diameter in cm
	HPOWER,                     //Slope of Asymptotic Height from Sortie
	RPOWER,                     //Slope of Asymptotic Crown-Radius from Sortie
	b_RPOWER,
	CHPOWER,                    //Slope of Asymptotic Crown-Height from Sortie
	b_CHPOWER,
	STEMCONST_P,
	STEMPOWER_P,
	CRA,
	CRB,
	CRC,
	HDMAX,                      //Height to Base diameter ratio MAX
	HDMIN,                      //Height to Base diameter ratio MIN
	DENMAX,                     //Maximum density (trees/10000 m^2)
	DENMIN,                     //Minimum density (trees/10000 m^2)
	MINPAREST,           //Minimum Monthly PAR (W/m^2 hour) for Establishment for the Dominated Layer
	MINRAIN,                    //Minimum annual Rain Precipitation for Establishment
	ADULT_AGE,
	MAXSEED,                    //numero massimo semi prodotti dalla pianta (da TREEMIG)
	MASTSEED,                   //ricorrenza anni di pasciona (da TREEMIG)
	WEIGHTSEED,                 //peso frutto in g
	SEXAGE,                         //Age at Sexual Maturity
	GERMCAPACITY,               //Geminability (Lischke H. & Loffler T. J.)
	MINTEMP,                    //Minimum temperature for germination in °C
	ESTMAX,                     //Potential Establishment rate in the absence of competition
	FRACFRUIT,                  //Fraction of NPP to Fruit Production
	ROTATION,
	MINAGEMANAG,                    //Minimum age for Managment
	MINDBHMANAG,                //Minimum DBH for Managment
	AV_SHOOT,                   //Average number of shoots produced after coppicing

	/************************************************* VARIABLES *****************************************************/



	//VEG_PERIOD,
	//STRUCTURE

	/* ALESSIOR:
	
		AVDBH deve essere sempre il primo perchè prima di lui ci sono i
		parametri con i valori CHE NON CAMBIANO MAI (COSTANTI!!!!!)

		oltretutto l'indice AVDBH viene usato in new_class.c
		e perdio non lo cambiate!
	*/


	AVDBH,                          //Average DBH in cm
	CROWN_DIAMETER,                 //Crown Diameter in m
	CROWN_AREA,                     //Crown Area in m^2
	CROWN_RADIUS_SORTIE,            //Crown Radius in m from Sortie Standard
	CROWN_DIAMETER_SORTIE,          //Crown Diameter in m from Sortie Standard
	CROWN_AREA_SORTIE,              //Crown Area from Sortie diameter
	CROWN_HEIGHT_SORTIE,            //Crown Height in m from Sortie Standard
	TREE_HEIGHT_SORTIE,             //Tree Height in m from Sortie function
	TREE_HEIGHT_LPJ,                //Tree Height in m from LPJ
	CROWN_AREA_LPJ,                 //Crown Area in m^2 from LPJ
	CROWN_DIAMETER_LPJ,             //Crown Diameter in m from LPJ
	MCA,                            //Maximum Crown Area in m^2
	MCD,                            //Maximum Crown Diameter in m^2
	DBHDC_EFF,                      //Crown Diameter from DBH in function of density
	PREVIOUS_DBHDC_EFF,				//previous dbhdc value
	CROWN_DIAMETER_DBHDC_FUNC,      //Crown Diameter in m from DBHDC function
	CROWN_AREA_DBHDC_FUNC,          //Crown Area in m^2 from DBHDC function
	CANOPY_COVER_DBHDC_FUNC,        //Canopy Cover % of pixel covered from DBHDC function
	CANOPY_COVER_DBHDC,
	CANOPY_COVER_P,                 //Canopy Cover from Portoghesi Function % of pixel covered
	FREE_CANOPY_COVER_P,
	CANOPY_COVER_LPJ,               //Canopy Cover % of pixel covered from LPJ
	TREE_HEIGHT_CR,                 //Tree Height in m from CHAPMAN-RICHARDS FUNCTION
	HD_EFF,                         //Effective HD ratio to give to Crowding Competition Function
	CC_TREE_HEIGHT,                 //Tree Height in m from Crowding Competition Function in m
	CC_AVDBH,                       //Average DBH from Crowding Competition Function in cm
	NUMBER_DENSITY,                 //Numbers of Tree per SIZECELL
	DENSITY,                        //Numbers of Tree per m^2
	TREE_AREA,                      //Average Available Area per Tree in m^2
	SAPWOOD_AREA,
	SAPWOOD_PERC,
	HEARTWOOD_AREA,
	HEARTWOOD_PERC,
	BASAL_AREA,                     //Basal Area (cm^2/area tree)
	STAND_BASAL_AREA,
	BASAL_AREA_m2,                     //Basal Area (cm^2/area tree)
	STAND_BASAL_AREA_m2,
	CROWN_HEIGHT,                   //Crown Height (m)
	PREVIOUS_VOLUME,				//previous year volume for CAI
	VOLUME,                   //Stem Volume
	TOTAL_VOLUME,
	TREE_VOLUME,                    //Single Tree Volume (m^3/area)
	IND_STEM_VOLUME,                //Individual Stem Volume for Crowding Competition Function
	CAI,                            //Current Annual Increment
	IND_CAI,                        //Individual Current Annual Increment
	MAI,                            //Mean Annual Volume Increment (m^3/area year)
	STEMCONST,

	/*radiation*/
	//PAR,                            //Photosyntheticallyl Active Radiation molPAR/m^2/day
	APAR,                           //Available Physiological Active Radiation molPAR/m^2/day  for DOMINANT LAYER
	APAR_SUN,
	APAR_SHADE,
	TRANSM_PAR,
	TRANSM_PAR_SUN,
	TRANSM_PAR_SHADE,
	APARU,                          //"Utilisable PAR" 'mol/m^2'
	//NET_RAD,                        //Daily Net Solar Radiation in W/m2
	NET_RAD_ABS,                    //Daily Net Solar Radiation in W/m2
	NET_RAD_ABS_SUN,
	NET_RAD_ABS_SHADE,
	NET_RAD_TRANSM,
	NET_RAD_TRANSM_SUN,
	NET_RAD_TRANSM_SHADE,
	//PPFD,
	PPFD_ABS,
	PPFD_TRANSM,
	PPFD_ABS_SUN,
	PPFD_TRANSM_SUN,
	PPFD_ABS_SHADE,
	PPFD_TRANSM_SHADE,
	//PPFD_SUN,
	//PPFD_SHADE,

	/*modifiers variables*/
	F_VPD,                          //VPD modifier
	//ALTER_VPD,                    //Alternative VPD
	//ALTER_F_VPD,                  //Alternative VPD Modifier
	F_LIGHT,                        //LIGHT modifier
	F_LIGHT_SUN,                    //LIGHT modifier for Sun leaves
	F_LIGHT_SHADE,                  //LIGHT modifier for Shaded leaves
	F_AGE,                          //AGE modifier
	F_NUTR,                         //SOIL NUTRIENT Modifer
	F_T,                            //TEMPERATURE modifier
	F_FROST,                        //FROST modifer
	F_SW,                           //SOIL WATER modifier
	F_DROUGHT,                      //SOIL DROUGHT modifier (see Duursma et al., 2008)
	F_PSI,							//SOIL WATER modifier using PSI, see Biome
	F_CO2,							//CO2 fert effect
	F_EVAPO,                        //Evapotranspiration modifier 5 oct 2012
	PHYS_MOD,                       //Physmod
	YEARLY_PHYS_MOD,
	AVERAGE_PHYS_MOD,
	AVERAGE_F_VPD,
	AVERAGE_F_NUTR,
	AVERAGE_F_T,
	AVERAGE_F_SW,

	/*water variables*/
	CANOPY_CONDUCTANCE,
	CANOPY_BLCOND,                  //Canopy Boundary Layer conductance (m s-1)
	MONTH_TRANSP,
	DAILY_TRANSP,
	DAILY_TRANSP_W,
	FRAC_RAIN_INTERC,				//FRACTION OF RAIN INTERCEPTED
	FRAC_DAYTIME_EVAPO,             //fraction of daytime for evaporation
	FRAC_DAYTIME_TRANSP,            //fraction of daytime for transpiration (and photosynthesis)
	RAIN_INTERCEPTED,
	CANOPY_WATER_STORED,            //residual of canopy water intercepted and not evaporated
	CANOPY_EVAPOTRANSPIRATION,
	CANOPY_EVAPORATION,             //Evaporation (mm)
	MONTHLY_EVAPOTRANSPIRATION,     //for WUE
	ASW,                            //available soil water per mm/ha
	WUE,                            //Water use efficiency (gDM/mm)

	CANOPY_INT,
	CANOPY_EVAPO,
	CANOPY_WET,                             //it should'nt be reset every day
	CANOPY_TRANSP,
	CANOPY_EVAPO_TRANSP,
	OLD_CANOPY_WATER,                       //it should'nt be reset every day
	CANOPY_WATER,                           //it should'nt be reset every day
	CANOPY_FRAC_DAY_TRANSP,                 //Fraction of daily canopy transpiration (DIM)


	/*LAI*/
	LAI,                            //LAI (m^2/m2)
	LAI_SUN,
	LAI_SHADE,
	ALL_LAI,
	MAX_LAI,						//lai at the very first day of senescence: parameter of the sigmoid function
	PEAK_LAI,                      //PEAK LAI
	SLA_SUN,
	SLA_SHADE,

	/*carbon variables*/
	DAILY_GPP_gC,                  //Daily Gross Primary Production gC/m^2 day
	DAILY_POINT_GPP_gC,
	MONTHLY_GPP_gC,				//Class Monthly Gross Primary Production gC/m^2 month
	NPP_tDM,                            //Net Primary Production  tDM/area
	NPP_gC, 						//Net Primary Production in grams of C
	NPP_tC,
	C_FLUX,
	YEARLY_POINT_GPP_gC,
	YEARLY_GPP_gC,                 //Yearly GPP
	YEARLY_NPP_tDM,                     //Yearly NPP

	YEARLY_RAIN,                    //Yearly Rain

	/*biomass variables*/
	//carbon to carbon pools in gC/m2/day
	C_TO_LEAF,
	C_TO_ROOT,
	C_TO_FINEROOT,
	C_TO_COARSEROOT,
	C_TO_TOT_STEM,
	C_TO_STEM,
	C_TO_BRANCH,
	C_TO_RESERVE,
	C_TO_FRUIT,
	C_TO_LITTER,
	RETRANSL_C_LEAF_TO_RESERVE,
	RETRANSL_C_FINEROOT_TO_RESERVE,
	C_BRANCH_TO_LITTER,
	C_COARSE_ROOT_TO_LITTER,
	C_STEM_TO_LITTER,

	//carbon biomass pools in tons of C/area
	LEAF_C,
	MAX_LEAF_C,
	TOT_ROOT_C,
	COARSE_ROOT_C,
	FINE_ROOT_C,
	MAX_FINE_ROOT_C,
	MAX_BUD_BURST_C,
	STEM_C,
	BRANCH_C,
	TOT_STEM_C,//STEM + BRANCH AND BARK
	MIN_RESERVE_C,
	RESERVE_C,
	FRUIT_C,
	LIVE_WOOD_C,
	DEAD_WOOD_C,
	TOTAL_WOOD_C,
	TOTAL_C,
	STEM_LIVE_WOOD_C,
	STEM_DEAD_WOOD_C,
	COARSE_ROOT_LIVE_WOOD_C,
	COARSE_ROOT_DEAD_WOOD_C,
	BRANCH_LIVE_WOOD_C,
	BRANCH_DEAD_WOOD_C,
	RESERVE_FOLIAGE_TO_RETRANSL_C,
	RESERVE_FINEROOT_TO_RETRANSL_C,
	STEM_SAPWOOD_C,
	STEM_HEARTWOOD_C,
	COARSE_ROOT_SAPWOOD_C,
	COARSE_ROOT_HEARTWOOD_C,
	BRANCH_SAPWOOD_C,
	BRANCH_HEARTWOOD_C,
	TOT_SAPWOOD_C,


	EFF_LIVE_TOTAL_WOOD_FRAC,

	/* per tree  in KgC */
	AV_LEAF_MASS_KgC,
	AV_STEM_MASS_KgC,
	AV_TOT_STEM_MASS_KgC,
	AV_ROOT_MASS_KgC,
	AV_FINE_ROOT_MASS_KgC,
	AV_COARSE_ROOT_MASS_KgC,
	AV_RESERVE_MASS_KgC,
	AV_MIN_RESERVE_KgC,
	AV_BRANCH_MASS_KgC,
	AV_LIVE_STEM_MASS_KgC,
	AV_DEAD_STEM_MASS_KgC,
	AV_LIVE_COARSE_ROOT_MASS_KgC,
	AV_DEAD_COARSE_ROOT_MASS_KgC,
	AV_LIVE_BRANCH_MASS_KgC,
	AV_DEAD_BRANCH_MASS_KgC,
	AV_LIVE_WOOD_MASS_KgC,
	AV_DEAD_WOOD_MASS_KgC,
	AV_TOT_WOOD_MASS_KgC,

	MASS_DENSITY,

	//CTEM CARBON
	//carbon biomass increment
	DEL_ROOTS_TOT,
	DEL_STEMS,
	DEL_FOLIAGE,
	DEL_RESERVE,
	DEL_FRUIT,
	DEL_BB,
	DEL_TOT_STEM,
	DEL_ROOTS_FINE,
	DEL_ROOTS_COARSE,
	DAILY_DEL_LITTER,

	//total cell carbon biomass pools in MgDM
	BIOMASS_FOLIAGE_tDM,
	BIOMASS_ROOTS_TOT_tDM,
	BIOMASS_COARSE_ROOT_tDM,
	BIOMASS_FINE_ROOT_tDM,
	BIOMASS_STEM_tDM,
	BIOMASS_BRANCH_tDM,
	BIOMASS_TOT_STEM_tDM, //STEM + BRANCH AND BARK
	RESERVE_tDM,
	BIOMASS_FRUIT_tDM,   //only for coniferous
	BIOMASS_LIVE_WOOD_tDM,
	BIOMASS_DEAD_WOOD_tDM,
	BIOMASS_STEM_LIVE_WOOD_tDM,
	BIOMASS_STEM_DEAD_WOOD_tDM,
	BIOMASS_COARSE_ROOT_LIVE_WOOD_tDM,
	BIOMASS_COARSE_ROOT_DEAD_WOOD_tDM,
	BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM,
	BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM,
	RESERVE_FOLIAGE_TO_RETRANSL_tDM,
	RESERVE_FINEROOT_TO_RETRANSL_tDM,
	MIN_RESERVE_tDM,

	/* per tree  in KgDM */
	AV_FOLIAGE_MASS_KgDM,
	AV_TOT_STEM_MASS_KgDM,
	AV_STEM_MASS_KgDM,
	AV_ROOT_MASS_KgDM,
	AV_FINE_ROOT_MASS_KgDM,
	AV_COARSE_ROOT_MASS_KgDM,
	AV_RESERVE_MASS_KgDM,
	AV_MIN_RESERVE_KgDM,
	AV_BRANCH_MASS_KgDM,
	AV_LIVE_STEM_MASS_KgDM,
	AV_DEAD_STEM_MASS_KgDM,
	AV_LIVE_COARSE_ROOT_MASS_KgDM,
	AV_DEAD_COARSE_ROOT_MASS_KgDM,
	AV_LIVE_WOOD_MASS_KgDM,
	AV_DEAD_WOOD_MASS_KgDM,
	AV_LIVE_BRANCH_MASS_KgDM,
	AV_DEAD_BRANCH_MASS_KgDM,
	CLASS_AGB,
	CLASS_BGB,
	//converted biome fraction for turnover
	LIVE_TOTAL_WOOD_TURNOVER_FRAC,
	MAX_BIOMASS_BUDBURST_tDM,
	MAX_BIOMASS_FOLIAGE_tDM,
	MAX_BIOMASS_FINE_ROOTS_tDM,
	WS_sap_tDM,
	WS_heart_tDM,
	WRC_sap_tDM,
	WRC_heart_tDM,
	WBB_sap_tDM,
	WBB_heart_tDM,
	WTOT_sap_tDM,
	//Branch and bark fraction --- currently the model doesn't accumulate it!!!!!!!
	W_SEED,                        //Seed Biomass
	DEL_TOTAL_W,                   //Total Biomass Increment
	TOTAL_W,
	DEL_Y_WTS,
	DEL_Y_WS,                       //Yearly cumulated stem biomass increment
	DEL_Y_WF,                       //Yearly cumulated foliage biomass increment
	DEL_Y_WR,                       //Yearly cumulated root biomass increment
	DEL_Y_WFR,
	DEL_Y_WCR,
	DEL_Y_WRES,                      //Yearly cumulated reserve biomass increment
	DEL_Y_BB,


	//converted biome fraction for allocation
	FINE_ROOT_LEAF_FRAC,
	STEM_LEAF_FRAC,
	COARSE_ROOT_STEM_FRAC,
	LIVE_TOTAL_WOOD_FRAC,
	//FINE/COARSE RATIO
	FR_CR,                       //FINE-COARSE ROOT RATIO
	LITTERFALL_RATE,				//daily/monthly litterfall rate from CTEM
	FRACBB,



	//MONTHLY PHENOLOGY
	FRAC_MONTH_FOLIAGE_REMOVE,
	MONTH_FRAC_FOLIAGE_REMOVE,
	//DAILY PHENOLOGY
	FRAC_DAY_FOLIAGE_REMOVE,	//number of leaf fall days/tot number of veg days
	FOLIAGE_REDUCTION_RATE,
	DAILY_FOLIAGE_BIOMASS_TO_REMOVE,
	//Marconi
	DAILY_FINEROOT_BIOMASS_TO_REMOVE,
	DAILY_LEAVES_BIOMASS_TO_REMOVE,
	OLD_BIOMASS_ROOTS_COARSE,
	OLD_BIOMASS_FINE_ROOT_tDM,
	OLD_BIOMASS_STEM,
	OLD_BIOMASS_BRANCH,
	OLD_BIOMASS_LEAVES,

	OLD_BIOMASS_STEM_LIVE_WOOD,
	OLD_BIOMASS_COARSE_ROOT_LIVE_WOOD,
	OLD_BIOMASS_STEM_BRANCH_LIVE_WOOD,
	FIRST_DAY_LAI,


	/*Maintenance respiration*/
	DAILY_LEAF_MAINT_RESP,
	NIGHTLY_LEAF_MAINT_RESP,
	TOT_DAY_LEAF_MAINT_RESP,
	FINE_ROOT_MAINT_RESP,
	STEM_MAINT_RESP,
	COARSE_ROOT_MAINT_RESP,
	BRANCH_MAINT_RESP,
	LIVE_STEM_MAINT_RESP,
	LIVE_COARSE_ROOT_MAINT_RESP,
	TOTAL_MAINT_RESP,

	/*Growth respiration*/
	LEAF_GROWTH_RESP,
	FINE_ROOT_GROWTH_RESP,
	STEM_GROWTH_RESP,
	BRANCH_GROWTH_RESP,
	COARSE_ROOT_GROWTH_RESP,
	TOTAL_GROWTH_RESP,

	//Autotrophic respiration
	LEAF_AUT_RESP,
	FINE_ROOT_AUT_RESP,
	STEM_AUT_RESP,
	COARSE_ROOT_AUT_RESP,
	BRANCH_AUT_RESP,
	TOTAL_AUT_RESP,

	//NITROGEN
	LEAF_NITROGEN,
	FINE_ROOT_NITROGEN,
	COARSE_ROOT_NITROGEN,
	STEM_NITROGEN,
	BRANCH_NITROGEN,

	//3PG MORTALITY FUNCTION
	WS_MAX,                         //Maximum stem mass per tree at 1000 trees/ha
	//LPJ MORTALITY FUNCTION
	AGEMORT,                        //Age probability mortality function

	THERMIC_SUM_FOR_END_VEG, //thermic sum at the end of leaf fall period

	//NATURAL REGENERATION INDIVIDUAL BIOMASS VALUES FOR SAPLING PASSING TO ADULT
	AVDBH_SAPLING,
	LAI_SAPLING,
	TREE_HEIGHT_SAPLING,
	LEAF_C_SAPLING,
	COARSE_ROOT_C_SAPLING,
	FINE_ROOT_C_SAPLING,
	STEM_C_SAPLING,
	PERC,

	VALUES
};

/* enum for counters */
enum {

	N_TREE,                         //Numbers of Tree per cell
	N_STUMP,                        //Number of stumps for coppice
	N_SEED,                        //Seeds number for cell
	DEAD_STEMS,                      //Dead Trees
	REM_TREES,                      //Yearly Total dead-removed Trees
	CUT_TREES,                      //Cut trees for Management
	TOT_REM_TREES,                  //Total dead-removed Trees from the beginning of simulation

	N_SAPLINGS,
	TREE_AGE_SAP,
	N_TREE_SAP,

	VEG_UNVEG,
	VEG_MONTHS,
	VEG_DAYS,
	BUD_BURST_COUNTER,
	DAY_FRAC_FOLIAGE_REMOVE, 	//number of leaf fall days
	DAY_FRAC_FINE_ROOT_REMOVE, 	//number of leaf fall days

	DAY_VEG_FOR_LITTERFALL_RATE,
	MONTH_VEG_FOR_LITTERFALL_RATE,
	LEAF_FALL_COUNTER,
	//included by Marconi
	//FROM_SEN_TO_EOY,
	//SENESCENCE_PERIOD,
	SENESCENCE_DAYONE,

	COUNTERS
};

/* structures */
// A row inside input.txt file
typedef struct {
	int x;
	int y;
	eLanduse landuse;
	int age;
	char *species;
	//ePhenology phenology;
	eManagement management;
	int n;
	int stump;
	PREC avdbh;
	PREC height;
	PREC wf;
	PREC wrc;
	PREC wrf;
	PREC ws;
	PREC wbb;
	PREC wres;
	PREC lai;

} ROW;

// variables related to plant pools turnover; it can't be placed int he ordinary species structure, since there are up to 5 arrays
typedef struct {
	double fineroot[MAXTURNTIME];
	double leaves[MAXTURNTIME];
	double coarseRoots[MAXTURNTIME];
	double branch[MAXTURNTIME];
	double stem[MAXTURNTIME];

	int FINERTOVER;
	int COARSERTOVER;
	int STEMTOVER;
	int BRANCHTOVER;
} TURNOVER;
/* */
//all variables related to the species class
typedef struct {
	char *name;
	//ePhenology phenology;
	eManagement management;
	int period;            // period = 0 for adult tree ; 1 for very young tree

	PREC value[VALUES];

	int counter[COUNTERS];
	int phenology_phase;
	TURNOVER *turnover;
} SPECIES;

/* */
//all variables related to the age class
typedef struct {
	int value;
	SPECIES *species;
	int species_count;
} AGE;

//todo
//all variables related to layers
typedef struct {

	double Abs_par;                 /* overall gridcell weighted average absorbed par (MJ/m2/day) */
	double Transm_par;              /* overall gridcell weighted average transmitted par (MJ/m2/day) */
	double Abs_net_rad;             /* overall gridcell weighted average absorbed net radiation (W/m2) */
	double Transm_net_rad;          /* overall gridcell weighted average transmitted net radiation (W/m2) */
	double Abs_ppfd;                /* overall gridcell weighted average absorbed ppfd (umol/m2/sec) */
	double Transm_ppfd;             /* overall gridcell weighted average transmitted ppfd (umol/m2/sec) */

} LAYER;

/* */
//all variables related to the height class
typedef struct {
	PREC value;
	int layer_coverage;
	int layer;
	int dominance;         //dominance = -1 no trees in veg period, dominance = 1 trees in veg period
	AGE *ages;
	int ages_count;

	LAYER *layers;
	int layer_count;


	int z;

	//int top_layer;

} HEIGHT;

/* */
//all variables related to the soil site
typedef struct
{
	double variabile;

	double bulk_density,
	field_capacity,
	wilting_point;
	/**************************************************************************************************
	 * 	SOIL PART; DEVELOPMENT OF DAILY SOIL CARBON NITROGEN AND PHOSPHORUS BALANCE
	 * 			for any issue contact Sergio Marconi (sergio.marconi@cmcc.it)
	 *
	 **************************************************************************************************/
	//3d-cmcc-sgm
	double sph; //check if removable (for sure in a single layer model)
	double soilMoisture;
	double soilTemp;
	double ice;
	double soc;
	double doc;
	double rcvl;
	double rcr;
	double rcl;
	double CRB1;
	double CRB2;
	double crhl;
	double crhr;
	double dphum;
	double no3;
	double no2;
	double nh4;
	double nh3;
	//double Eh;
	double co2;
	double day_O2;
	double dcbavai;
	double drcvl;
	double DRCB1, DRCB2;
	double soilDepth;
	double clay_nh4;
	double inert_C;
	double LabP;
	double AdsP;
	//unknown
	double sts,mmm;
	double no, n2o;
	double litco22, litco23;

	double initialOrganicC;
	double waterContent;
	double CEC;
} SOIL;


//fixme
typedef struct {
	int class;

	//cumulative variables class related used in annual-monthly-daily Log
	/*
	//test set as class level result e.g. class_daily_gpp[]
	double class_daily_gpp[3], class_daily_tot_gpp, monthly_gpp[3], monthly_tot_gpp, annual_gpp[3], annual_tot_gpp;
	double class_daily_npp[3], class_daily_tot_npp, monthly_npp[3], class_monthly_tot_npp, annual_npp[3], class_annual_tot_npp;
	double class_daily_et[3], daily_tot_et, class_monthly_et[3], monthly_tot_et, class_annual_et[3], annual_tot_et;
	double class_daily_maint_resp[3], class_daily_tot_maint_resp, class_monthly_maint_resp[3], class_monthly_tot_maint_resp, class_annual_maint_resp[3], class_annual_tot_maint_resp;
	double class_daily_growth_resp[3], class_daily_tot_growth_resp, monthly_gowth_resp[3], class_monthly_tot_growth_resp, class_annual_growth_resp[3], class_annual_tot_growth_resp;
	double class_daily_aut_resp[3], daily_tot_aut_resp, daily_tot_het_resp,  monthly_aut_resp[3], monthly_tot_aut_resp, annual_aut_resp[3], annual_tot_aut_resp;
	double daily_aut_resp_tDM[3];
	double daily_cc[3], monthly_cc[3], annual_cc[3];
	double daily_lai[3];
	double annual_peak_lai[10];
	int daily_dead_tree[3], daily_tot_dead_tree, monthly_dead_tree[3], monthly_tot_dead_tree, annual_dead_tree[3], annual_tot_dead_tree;
	double daily_f_sw, daily_f_psi, daily_f_t, daily_f_vpd;
	 */
} CLASS;

/* */
//all variables related to the cell (stand) are here defined
typedef struct {
	int x;
	int y;

	eLanduse landuse;

	HEIGHT *heights;
	SOIL *soils;
	CLASS *class;
	int heights_count; //number of heights
	int soils_count;

	/* general variables */
	int yearday;
	int cum_dayOfyear;
	double abscission_daylength;
	double av_yearly_daylength;
	int north;                                                   //northern hemisphere north = 0, south hemisphere south = 1
	double ni;                                                   //proportion of day length (frac)

	/* annual met values */
	double annual_tavg;
	double annual_tmin;
	double annual_tmax;
	double annual_tday;
	double annual_tnight;
	double annual_tsoil;
	double annual_solar_rad;
	double annual_precip;
	double annual_vpd;

	/*forest structure variables*/
	int height_class_in_layer_dominant_counter;
	int height_class_in_layer_dominated_counter;
	int height_class_in_layer_subdominated_counter;
	int dominant_veg_counter;
	int dominated_veg_counter;
	int subdominated_veg_counter;
	int Veg_Counter;
	int tree_number_dominant;
	int tree_number_dominated;
	int tree_number_subdominated;
	int daily_layer_number;
	int monthly_layer_number;
	int annual_layer_number;
	int top_layer;
	int saplings_counter;
	double cell_cover;
	double density_dominant;
	double density_dominated;
	double density_subdominated;
	double canopy_cover_dominant;
	double canopy_cover_dominated;
	double canopy_cover_subdominated;
	double layer_cover_dominant;
	double layer_cover_dominated;
	double layer_cover_subdominated;

	int n_tree;
	int daily_dead_tree, monthly_dead_tree, annual_dead_tree ;
	double basal_area;

	/*radiation variables*/
	double extra_terr_radiation_MJ;                     //Extraterrestrial radiation (MJ/m2/day)
	double extra_terr_radiation_W;                      //Extraterrestrial radiation (W/m2)
	double short_wave_clear_sky_radiation_MJ;           //Short wave clear sky radiation (MJ/m2/day)
	double short_wave_clear_sky_radiation_W;            //Short wave clear sky radiation (W/m2)
	double short_wave_radiation_DW_MJ;                  //Downward short wave radiation flux (MJ/m2/day)
	double short_wave_radiation_DW_W;                   //Downward short wave radiation flux (W/m2)
	double short_wave_radiation_UW_MJ;                  //Upward short wave radiation flux (MJ/m2/day)
	double short_wave_radiation_UW_W;                   //Upward short wave radiation flux (W/m2)
	double net_short_wave_radiation_MJ;                 //Net short wave radiation flux (MJ/m2/day)
	double net_short_wave_radiation_W;                  //Net short wave radiation flux (W/m2)

	double long_wave_radiation_DW_MJ;                   //Downward long wave radiation flux (MJ/m2/day)
	double long_wave_radiation_DW_W;                    //Downward long wave radiation flux (W/m2)
	double long_wave_radiation_UW_MJ;                   //Upward long wave radiation flux (MJ/m2/day)
	double long_wave_radiation_UW_W;                    //Upward long wave radiation flux (W/m2)
	double net_long_wave_radiation_MJ;                  //Net long wave radiation flux (MJ/m2/day)
	double net_long_wave_radiation_W;                   //Net long wave radiation flux (W/m2)

	double net_radiation;                               //Net radiation flux ('terrestrial radiation') (W/m2)
	double net_radiation_transm;
	double net_radiation_for_dominated;
	double net_radiation_for_subdominated;
	double net_radiation_for_soil;

	double par;
	double par_transm;
	double par_for_dominated;
	double par_for_subdominated;
	double par_for_soil;
	double par_for_establishment;

	double ppfd;                                        //Photosynthetic phton flux density (umol/m2/sec)
	double ppfd_transm;

	double cloud_cover_frac;                            //cloud cover fraction




	double av_yearly_par_soil;

	/*carbon variables*/
	double daily_gpp, monthly_gpp, annual_gpp;     //in g of C m^2
	double daily_npp_gC, monthly_npp_gC, annual_npp_gC;	//in g of C m^2
	double daily_npp_tDM, monthly_npp_tDM, annual_npp_tDM;     //in tonnes of DM per hectare
	double daily_aut_resp, monthly_aut_resp, annual_aut_resp;
	double daily_aut_resp_tC, monthly_aut_resp_tC, annual_aut_resp_tC;
	double daily_maint_resp, monthly_maint_resp, annual_maint_resp;
	double daily_growth_resp, monthly_growth_resp, annual_growth_resp;
	double daily_r_eco, monthly_r_eco, annual_r_eco;
	double daily_het_resp, monthly_het_resp, annual_het_resp;
	double daily_C_flux, monthly_C_flux, annual_C_flux;
	double litter;
	double ter;  //total ecosystem respiration
	double carbon_balance, old_carbon_balance;
	double daily_nee, monthly_nee, annual_nee;
	double daily_leaf_carbon;/* daily carbon assimilated to c pool in gC/m2/day */
	double daily_stem_carbon;/* daily carbon assimilated to c pool in gC/m2/day  */
	double daily_fine_root_carbon;/* daily carbon assimilated to c pool in gC/m2/day  */
	double daily_coarse_root_carbon;/* daily carbon assimilated to c pool in gC/m2/day  */
	double daily_root_carbon;/* daily carbon assimilated to c pool in gC/m2/day  */
	double daily_branch_carbon;/* daily carbon assimilated to c pool in gC/m2/day  */
	double daily_reserve_carbon;/* daily carbon assimilated to c pool in gC/m2/day  */
	double daily_litter_carbon;/* daily carbon to litter  c pool in gC/m2/day  */
	double daily_leaf_carbon_tC;/* daily carbon assimilated to c pool in tC/cell/day */
	double daily_stem_carbon_tC;/* daily carbon assimilated to c pool in tC/cell/day */
	double daily_fine_root_carbon_tC;/* daily carbon assimilated to c pool in tC/cell/day */
	double daily_coarse_root_carbon_tC;/* daily carbon assimilated to c pool in tC/cell/day */
	double daily_branch_carbon_tC;/* daily carbon assimilated to c pool in tC/cell/day */
	double daily_reserve_carbon_tC;/* daily carbon assimilated to c pool in tC/cell/day */
	double daily_root_carbon_tC;/* daily carbon assimilated to c pool in tC/cell/day */
	double daily_litter_carbon_tC;/* daily carbon assimilated to c pool in tC/cell/day */
	double daily_leaf_drymatter;
	double daily_stem_drymatter;
	double daily_fine_root_drymatter;
	double daily_coarse_root_drymatter;
	double daily_branch_drymatter;
	double daily_reserve_drymatter;
	double daily_leaf_maint_resp;
	double daily_stem_maint_resp;
	double daily_fine_root_maint_resp;
	double daily_branch_maint_resp;
	double daily_coarse_root_maint_resp;
	double daily_leaf_growth_resp;
	double daily_stem_growth_resp;
	double daily_fine_root_growth_resp;
	double daily_branch_growth_resp;
	double daily_coarse_root_growth_resp;
	double daily_leaf_aut_resp;
	double daily_stem_aut_resp;
	double daily_branch_aut_resp;
	double daily_fine_root_aut_resp;
	double daily_coarse_root_aut_resp;
	double daily_f_sw, daily_f_psi, daily_f_t, daily_f_vpd;
	double daily_litterfall, monthly_litterfall, annual_litterfall;
	double av_gpp;
	double av_npp;
	double stand_agb;
	double stand_bgb;

	/* flux balance */
	double flux_C_balance, old_flux_C_balance;

	/*water variables*/
	//todo move these variables into soil struct

	double wilting_point;
	double field_capacity;
	double sat_hydr_conduct;
	double bulk_density;

	double daily_tot_w_flux, monthly_tot_w_flux, annual_tot_w_flux;
	double asw;
	double old_asw;
	double max_asw_fc;              /* max available soil water at field capacity mmKgH2O/m3*/
	double psi;

	double soil_pool_water_balance, old_soil_pool_water_balance;
	double canopy_pool_water_balance, old_canopy_pool_water_balance;
	double soil_moist_ratio;
	double av_soil_moist_ratio;
	double swc;//volumetric soil water content (%vol)
	double psi_sat;//soil saturated matric potential
	double vwc;
	double vwc_fc;
	double vwc_sat;//soil saturated Volumetric water content
	double soil_b; //soil moisture parameter
	double soilw_sat; //(kgH2O/m2) soilwater at saturation
	double soilw_fc; //(kgH2O/m2) soilwater at field capacity
	double prcp_rain;
	double prcp_snow;
	double days_since_rain;
	double rain_intercepted;
	double water_to_soil;
	double water_to_atmosphere;
	double precip_sources;
	double precip_canopy;
	double precip_soil;
	double snow_pack;  //amount of snow in Kg H2O
	double snow_melt; //melted snow
	double snow_subl; //sublimated snow
	double snow_to_soil;
	double out_flow;
	double daily_c_int, monthly_c_int, annual_c_int;
	double daily_c_transp, monthly_c_transp, annual_c_transp;
	double daily_c_evapo, monthly_c_evapo, annual_c_evapo;
	double old_daily_c_water_stored;
	double daily_c_water_stored, monthly_c_water_stored, annual_c_water_stored; //not used
	double daily_c_evapotransp, monthly_c_evapotransp, annual_c_evapotransp;
	double daily_c_bl_cond, monthly_c_bl_cond, annual_c_bl_cond;
	double daily_soil_evapo, monthly_soil_evapo, annual_soil_evapo;
	double daily_soil_bl_cond, monthly_soil_bl_cond, annual_soil_bl_cond;
	double daily_et, monthly_et, annual_et;
	double daily_canopy_sensible_heat_flux;
	double daily_soil_sensible_heat_flux;

	/*energy balance*/
	double daily_c_int_watt;
	double daily_c_transp_watt;
	double daily_c_evapo_watt;
	double daily_c_evapotransp_watt;
	double daily_soil_evaporation_watt;
	double daily_latent_heat_flux, monthly_latent_heat_flux, annual_latent_heat_flux;
	double daily_sensible_heat_flux, monthly_sensible_heat_flux, annual_sensible_heat_flux;


	//cumulative variables layer related used in annual-monthly-daily Log
	double layer_daily_gpp[3], layer_monthly_gpp[3], layer_annual_gpp[3];
	double layer_daily_npp_tDM[3], layer_monthly_npp_tDM[3], layer_annual_npp_tDM[3];
	double layer_daily_npp_gC[3], layer_monthly_npp_gC[3], layer_annual_npp_gC[3];
	double layer_daily_maint_resp[3], layer_monthly_maint_resp[3], layer_annual_maint_resp[3];
	double layer_daily_growth_resp[3], layer_monthly_gowth_resp[3], layer_annual_growth_resp[3];
	/* among pools */
	double layer_daily_aut_resp[3], layer_monthly_aut_resp[3], layer_annual_aut_resp[3];
	double layer_daily_leaf_aut_resp[3], layer_monthly_leaf_aut_resp[3], layer_annual_leaf_aut_resp[3];
	double layer_daily_stem_aut_resp[3], layer_monthlyl_stem_aut_resp[3], layer_annual_stem_aut_resp[3];
	double layer_daily_branch_aut_resp[3], layer_monthly_branch_aut_resp[3], layer_annual_branch_aut_resp[3];
	double layer_daily_fine_root_aut_resp[3], layer_monthly_fine_root_aut_resp[3], layer_annual_fine_root_aut_resp[3];
	double layer_daily_coarse_root_aut_resp[3], layer_monthly_coarse_root_aut_resp[3], layer_annual_coarse_root_aut_resp[3];

	double layer_daily_c_flux[3], layer_monthly_c_flux[3], layer_annual_c_flux[3];
	double layer_daily_aut_resp_tC[3];
	double layer_daily_c_flux_tDM[3];


	double daily_delta_wf[3], monthly_delta_wf[3], annual_delta_wf[3];
	double daily_delta_wts[3], monthly_delta_wts[3], annual_delta_wts[3];
	double daily_delta_ws[3], monthly_delta_ws[3],  annual_delta_ws[3];
	double daily_delta_wbb[3], monthly_delta_wbb[3], annual_delta_wbb[3];
	double daily_delta_wfr[3], monthly_delta_wfr[3], annual_delta_wfr[3];
	double daily_delta_wcr[3], monthly_delta_wcr[3], annual_delta_wcr[3];
	double daily_delta_wres[3], monthly_delta_wres[3], annual_delta_wres[3];

	double daily_layer_avDBH[3], monthly_layer_avDBH[3], annual_layer_avDBH[3];
	double daily_layer_stem_c[3], monthly_layer_stem_c[3], annual_layer_stem_c[3];
	double daily_layer_live_stem_c[3], monthly_layer_live_stem_c[3], annual_layer_live_stem_c[3];
	double daily_layer_stem_sapwood_c[3], monthly_layer_stem_sapwood_c[3], annual_layer_stem_sapwood_c[3];
	double daily_layer_leaf_c[3], monthly_layer_leaf_c[3], annual_layer_leaf_c[3];
	double daily_layer_tot_stem_c[3], monthly_layer_tot_stem_c[3], annual_layer_tot_stem_c[3];
	double daily_layer_branch_c[3], monthly_layer_branch_c[3], annual_layer_branch_c[3];
	double daily_layer_live_branch_c[3], monthly_layer_live_branch_c[3], annual_layer_live_branch_c[3];
	double daily_layer_branch_sapwood_c[3], monthly_layer_branch_sapwood_c[3], annual_layer_branch_sapwood_c[3];
	double daily_layer_fineroot_c[3], monthly_layer_fineroot_c[3], annual_layer_fineroot_c[3];
	double daily_layer_coarseroot_c[3], monthly_layer_coarseroot_c[3], annual_layer_coarseroot_c[3];
	double daily_layer_coarse_root_sapwood_c[3], monthly_layer_coarse_root_sapwood_c[3], annual_layer_coarse_root_sapwood_c[3];
	double daily_layer_live_coarseroot_c[3], monthly_layer_live_coarseroot_c[3], annual_layer_live_coarseroot_c[3];
	double daily_layer_sapwood_c[3], monthly_layer_sapwood_c[3], annual_layer_sapwood_c[3];
	double daily_layer_reserve_c[3], monthly_layer_reserve_c[3], annual_layer_reserve_c[3];


	double layer_daily_c_int[3], layer_monthly_c_int[3], layer_annual_c_int[3];
	double layer_daily_c_transp[3], layer_monthly_c_transp[3], layer_annual_c_transp[3];
	double layer_daily_c_evapo[3], layer_monthly_c_evapo[3], layer_annual_c_evapo[3];
	double layer_daily_c_water_stored[3], layer_monthly_c_water_stored[3], layer_annual_c_water_stored[3];
	double layer_daily_c_evapotransp[3], layer_monthly_c_evapotransp[3], layer_annual_c_evapotransp[3];
	double layer_daily_et[3], layer_monthly_et[3], layer_annual_et[3];


	double layer_daily_cc[3], layer_monthly_cc[3], layer_annual_cc[3];
	double daily_lai[3];
	double annual_peak_lai[10];
	int layer_daily_dead_tree[3], layer_monthly_dead_tree[3], layer_annual_dead_tree[3];

	double annual_dbh[3];


	/**************************************************************************************************
	 * 	SOIL PART; DEVELOPMENT OF DAILY SOIL CARBON NITROGEN AND PHOSPHORUS BALANCE
	 * 			for any issue contact Sergio Marconi (sergio.marconi@cmcc.it)
	 *
	 **************************************************************************************************/
	//daily soil variables; 3D-CMCC-SGM
	double soil_ph;
	//sergio; not clear till concepts and dynamics; what's that and how to replace&introcude in 3d-cmcc
	double till_fact, tilq;
	//dC
	double leafLittering, fineRootLittering,stemBrancLittering,stemLittering, coarseRootLittering;
	double leaflitN, fineRootlitN,stemBranclitN,stemlitN, coarseRootlitN;
	double day_C_mine;
	double day_N_mine;
	double day_N_assim;
	double day_soil_nh3;
	double day_clay_N, day_clayn;
	double wrcvl, wrcl, wrcr, wcrb, wcrh,whumus;
	double wtcavai,w_no2,w_no3,w_n2o, w_nh4, w_nh3, wpool_no;
	double  wsoc;
	double leach_u, day_no2, flux_no2;
	//double wFreezedoc;
	double End_SON;
	double runoff_N;
	double previousSoilT;
	double soilSurfaceT;
	double temp_avet;
	double yr_avet;
	double base_clay_N, max_clay_N;
	double AddC, AddCN, AddC1, AddC2, AddC3;

	//potentially already existant
	int doy, dos;

	//todo to be remopved used just to evaluate total biomass flutctations in the several different compartments
	double leafBiomass, stemBiomass, fineRootBiomass, coarseRootBiomass,stemBranchBiomass;
	double vpSat[365], maxVpSat;

	/* ALESSIOR */
	int years_count;
	YOS *years;
} CELL;

/* */
typedef struct {
	CELL *cells;
	int cells_count;
} MATRIX;

typedef struct {
	int *daily_vars;
	int daily_vars_count;
	int *monthly_vars;
	int monthly_vars_count;
	int *yearly_vars;
	int yearly_vars_count;
	double *daily_vars_value;
	double *monthly_vars_value;
	double *yearly_vars_value;
} OUTPUT_VARS;

/* constants */
#define MET_FILENAME_LEN                1024

/* defines */
#define Minimum(a, b)  (((a) < (b)) ? (a) : (b))
#define Maximum(a, b)  (((a) > (b)) ? (a) : (b))



//-----------------------------DEAFULT PARAMETERS------------------------------------- This was run_model.h

//#define LOGFILE		"output.txt"
/* #define BUFFER_SIZE	4096 */
#define BUFFER_SIZE	585600
#define BUFFER_SIZE_LOG 67500



// Store site.txt and settings.txt data
settings_t *settings;


// External functions
YOS *ImportYosFiles(char *, int *const, const int, const int);
int Tree_model_daily (MATRIX *const, const YOS *const, const int, const int, const int, const int, const int);
//if putted into main.c
//int soil_model (MATRIX *const, const YOS *const, const int, const int, const int);
void soil_model (MATRIX *const, const YOS *const, const int, const int, const int, const int);
void Phenology_phase (SPECIES *, const MET_DATA *const, const int , const int , const int);
void met_summary(MET_DATA *);
int is_valid_met(const char *const);
void Avg_temperature (CELL *, int, int, int);
void Daylight_avg_temperature (CELL *, int, int, int, YOS *);
void Nightime_avg_temperature (CELL *, int, int, int, YOS *);
void Dew_temperature (CELL *, int , int , int , YOS *);
void Soil_temperature (CELL *, int, int, int, YOS *);
void Air_density (CELL *, int, int, int, YOS *);
void Latent_heat (CELL *, int, int, int, YOS *);
void Thermic_sum (CELL *, int, int, int, YOS *);
void Air_pressure (CELL *c, int, int, int, YOS *);
void Psychrometric (CELL *c, int, int, int, YOS *);
void Veg_Days (CELL *const, const YOS *const, const int, const int, const int);
int sort_by_years(const void *, const void *);
int sort_by_heights_asc(const void * , const void * );
int sort_by_heights_desc(const void * , const void * );
double Canopy_cover (SPECIES *const, int, int, int);
void Crowding_competition (SPECIES *const, HEIGHT *, int, int , int);
ROW *import_dataset(const char *const, int *const);
int importSettingsFile(char *);

void Sat_vapour_pressure (CELL *, int, int, int, YOS *);

void Day_Length (CELL *, int, int, int, YOS *);
void DayLength_3PG (CELL *, int, int, int, int, YOS *);
void Annual_met_values (CELL *, int, int, int, YOS *);
void Annual_CO2_concentration (CELL *, int, int, int, YOS *);
void Abscission_DayLength (CELL *);
int Establishment_LPJ (CELL *const, SPECIES *const);
void Age_Mortality (SPECIES *const, AGE *const);
void Greff_Mortality (SPECIES *const);
void Mortality (SPECIES *const, int);
void Daily_modifiers (SPECIES *const, AGE *const, CELL *const, const MET_DATA *const, int, int, int, int, int);
void Management (SPECIES *const, AGE *const, int);
void Clearcut_Timber_upon_request (SPECIES *const, int, int, int);
void Clearcut_Timber_without_request (SPECIES *, CELL*, int);
void Clearcut_Coppice (SPECIES *const, int, int, int);
MATRIX *matrix_create(ROW *const, const int);
void matrix_free(MATRIX *);
void matrix_summary(const MATRIX *const);
void Dominant_Light(HEIGHT *, CELL *, const int, const MET_DATA *const, const int, const int);
//yearly allocation
void Fruit_Allocation_LPJ (SPECIES *const, int, int, double, double);
int Fruit_Allocation_Logistic_Equation (SPECIES *const, AGE *const);
int Fruit_Allocation_TREEMIG (SPECIES *const, AGE *const);

//monthly allocation
void M_Fruit_Allocation_LPJ (SPECIES *const, int, int, double, double);
int M_Fruit_Allocation_Logistic_Equation (SPECIES *const, AGE *const);
int M_Fruit_Allocation_TREEMIG (SPECIES *const, AGE *const);


//deciduous routine for carbon allocation
void Deciduous_Partitioning_Allocation (SPECIES *const, CELL *, const MET_DATA *const, int, int, int, int, int, int, int);
void simple_Deciduous_Partitioning_Allocation (SPECIES *const, CELL *, const MET_DATA *const, int, int, int, int, int, int, int);


void Daily_C_Evergreen_Partitioning_Allocation (SPECIES *const, CELL *, const MET_DATA *const, int, int, int, int, int, int, int);
void Daily_C_Deciduous_Partitioning_Allocation (SPECIES *const, CELL *const, const MET_DATA *const, int, int, int, int, int, int);


void Nitrogen_stock (SPECIES *);

void Maintenance_respiration (SPECIES *const, CELL *const, const MET_DATA *const, int, int, int);
void Growth_respiration (SPECIES *const, CELL *, int, int, int, int);
void Autotrophic_respiration (SPECIES *const, CELL *, int);
void Carbon_assimilation (SPECIES *const , CELL *const , int, int, int, int);
void Soil_respiration (CELL *);
void Carbon_fluxes (SPECIES *const, CELL *const, int, int, int);
void Water_fluxes (CELL *const);
void Soil_evaporation (CELL *, const MET_DATA *const, int, int);
void Daily_lai (SPECIES *const);
void Peak_lai(SPECIES *const , CELL *const, int, int, int, int, int );
void Turnover(SPECIES *, CELL *);
void EOY_Turnover(SPECIES *);
void Leaf_fall(SPECIES *, int *);
void Light_Recruitment (SPECIES *const, double, double);
void Solar_radiation (CELL *, int, int, int);
void Radiation (SPECIES *const, CELL *, const MET_DATA *const, int, int, int, int, int, int, int);
void Rad_abs_transm (CELL *const, SPECIES *const, double, double);
void Phosynthesis(SPECIES *const , CELL *, int , int, int, int, int, int);
void Biomass_increment_BOY ( CELL *const, SPECIES *const, int, int, int);
void Biomass_increment_EOY ( CELL *const, SPECIES *const, int, int, int, int);
void AGB_BGB_biomass (CELL *const , int, int, int);
void Dendrometry (CELL *const, SPECIES *const, HEIGHT *, int);
void Daily_layer_cover (CELL *, const MET_DATA *const, int, int);
void Daily_Forest_structure (CELL *, const int,const int,const int);
void Print_met_daily_data (const YOS *const , int , int , int );
void Print_met_data (const MET_DATA *const, int, int);
void Print_init_month_stand_data (CELL *, const MET_DATA *const, const int, const int, int, int, int);
void Print_end_month_stand_data (CELL *, const YOS *const, const MET_DATA *const, const int, const int, int, int, int);
void Print_parameters (SPECIES *const, int, int, int);
void Daily_vegetative_period (CELL *, const MET_DATA *const, int, int);
int  Number_of_layers (CELL *);
void Annual_numbers_of_layers (CELL *);
void Daily_numbers_of_layers (CELL *);
void Layer_cover_mortality (CELL *, int, int, int, double, int);
void water_downward_balance (CELL *, const MET_DATA *const, int, int);
void water_upward_balance (CELL *, const MET_DATA *const, int, int);
void Soil_water_balance (CELL *const, const MET_DATA *const, int, int);
void Annual_average_values_modifiers (SPECIES *);
void Annual_average_values_met_data (CELL *, double, double, double, double);
void Average_tree_biomass (SPECIES *);
void Total_class_level_biomass (SPECIES *);
void Renovation (CELL *, HEIGHT *, SPECIES *);
void Water_Use_Efficiency (SPECIES *);
void Tree_period (SPECIES *, AGE *, CELL *);
void Daily_veg_counter (CELL *, SPECIES * , int);
void First_day (CELL *const, const int);
void Reset_daily_variables (CELL *const, const int);
void Reset_monthly_variables (CELL *const, const int);
void Reset_annual_variables (CELL *const, const int);
void Initialization_biomass_data (SPECIES *, HEIGHT *);
void Initialization_site_data (CELL *);
void Choose_management (CELL *, SPECIES *, int , int );
void Tree_Branch_Bark (SPECIES *, AGE *, int, int, int);
void Allometry_Power_Function (AGE *, SPECIES *);
void Check_prcp (CELL *c, MET_DATA *, int, int);
void Pool_fraction (SPECIES *);
void Canopy_transpiration (SPECIES *, CELL *, const MET_DATA *const, int, int, int, int, int);
void Canopy_transpiration_biome (SPECIES *, CELL *, const MET_DATA *const, int, int, int, int, int);
void Canopy_interception (SPECIES *const, CELL *const, const MET_DATA *const, int, int, int);
void Canopy_evapotranspiration (SPECIES *, CELL *, int);
void Evapotranspiration (CELL *);
void Latent_heat_flux (CELL *, const MET_DATA *const, int, int);
void Check_class_carbon_balance (SPECIES *);
void Check_carbon_balance (CELL *);
void Check_class_water_balance (SPECIES *);
void Check_soil_water_balance (CELL *);
void Check_C_flux_balance (CELL *);

void live_total_wood_age(AGE *, SPECIES *);

//sergio's functions
int crop_model_M (MATRIX *const, const YOS *const, const int, const int, const int);
int crop_model_D (MATRIX *const, const YOS *const, const int, const int, const int, const int);
void Get_EOD_soil_balance_cell_level (CELL *, const YOS *const , int, int, int);
void get_av_year_temperature(CELL * const, int, int, int, const MET_DATA *const);
void soil_temperature(CELL * const, int, int, int, const MET_DATA *const);
void soil_dndc_sgm(MATRIX *const, const YOS *const, const int, const int, const int, const int);
void soil_initialization(CELL *);
void tree_leaves_fall(MATRIX *const, int const);
void soilCEC(CELL *const);
void leaffall(SPECIES *, const MET_DATA *const, int*, int*, int );
void get_vpsat(CELL *, int , int , int , YOS *, int);
void Get_turnover_Marconi (SPECIES *, CELL *, int, int);
void get_net_ecosystem_exchange(CELL *);
int endOfYellowing(const MET_DATA *const, SPECIES *);
void senescenceDayOne(SPECIES *, const MET_DATA *const, CELL *const);
void Stool_mortality (SPECIES *const, int);

//test
void simple_phenology_phase (SPECIES *, const MET_DATA *const, const int, const int, const int);
void Canopy_evapo_transpiration (SPECIES *const, CELL *const, const MET_DATA *const, int, int, int, int, int);
void soil_evaporation_biome (CELL *const c, const MET_DATA *const, int, int);
double Penman_Monteith (const MET_DATA *const, int, int, int, int, double);
void Annual_minimum_reserve (SPECIES *);
int alloc_struct(void **t, int *count, unsigned int size);
int Create_new_class(CELL *const c, const int height, const int age, const int species);
int fill_species_from_file(SPECIES *const s);
OUTPUT_VARS *ImportOutputVarsFile(const char *const filename);
void FreeOutputVars(OUTPUT_VARS *ov);
int WriteNetCDFOutput(const char *const path, const OUTPUT_VARS *const vars, const int year_start, const int years_count, const int x_cells_count, const int y_cells_count, const int type);
const char* get_netcdf_version(void);
#define XSTR(a)		STR(a)
#define STR(a)		#a
#define CHECK_CONDITION(x,c) {																																\
		if ( (x)c )		{																																	\
			logger(g_log, "\nerror: condition (%s %s) is true, value of %s is %g in %s on line %d\n", XSTR(x), XSTR(c), XSTR(x), (double)(x),  __FILE__, __LINE__);	\
			exit(1);																																		\
		}																																					\
}

/* DO NOT CHANGE THIS ORDER */
enum {
	AR_DAILY_OUT
	, AR_MONTHLY_OUT
	, AR_YEARLY_OUT

	, GPP_DAILY_OUT
	, GPP_MONTHLY_OUT
	, GPP_YEARLY_OUT

	, NPP_DAILY_OUT
	, NPP_MONTHLY_OUT
	, NPP_YEARLY_OUT

	, OUTPUT_VARS_COUNT
};

/* */
#endif /* TYPES_H */
