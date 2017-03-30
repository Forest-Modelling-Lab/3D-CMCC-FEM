/* matrix.h */
#ifndef MATRIX_H_
#define MATRIX_H_

#include "yos.h"
#include "soil_settings.h"

#define MAXTURNTIME 5000

typedef enum {
	T		/* timber */ //fixme change with H (high forest)
	, C		/* coppice */
	, N		/* natural (no management) */
} e_management;

enum {

	/*********************************************** PARAMETERS **********************************************/
	/*********************************************************************************************************/

	/* SPECIES-SPECIFIC CONSTANT ECO-PHYSIOLOGICAL PARAMETER VALUES */
	/* NOTE: DON'T CHANGE THEIR ORDER!! */

	LIGHT_TOL,                           /* Light Tolerance 4 = very shade intolerant (cc = 90%), 3 = shade intolerant (cc = 100%), 2 = shade tolerant (cc = 110%), 1 = very shade tolerant (cc = 120%)*/
	PHENOLOGY,                           /* PHENOLOGY 0.1 = deciduous broadleaf, 0.2 = deciduous needle leaf, 1.1 = broad leaf evergreen, 1.2 = needle leaf evergreen*/
	ALPHA,                               /* Canopy quantum efficiency (molC/molPAR) */
	EPSILONgCMJ,                         /* Light Use Efficiency  (gC/MJ)(used if ALPHA is not available) */
	K,                                   /* Extinction coefficient for absorption of PAR by canopy */
	ALBEDO,                              /* Canopy albedo */
	INT_COEFF,                           /* Precipitation interception coefficient */
	SLA_AVG,                             /* AVERAGE Specific Leaf Area m^2/KgC for sunlit/shaded leaves */
	SLA_RATIO,                           /* (DIM) ratio of shaded to sunlit projected SLA */
	LAI_RATIO,                           /* (DIM) all-sided to projected leaf area ratio */
	FRACBB0,                             /* Branch and Bark fraction at age 0 (m^2/kg) */
	FRACBB1,                             /* Branch and Bark fraction for mature stands (m^2/kg) */
	TBB,                                 /* Age at which fracBB = (FRACBB0 + FRACBB1 )/ 2 */
	RHOMIN,                              /* Minimum Basic Density for young Trees (tDM/m3) */
	RHOMAX,                              /* Maximum Basic Density for young Trees (tDM/m3) */
	TRHO,                                /* Age at which rho = (RHOMIN + RHOMAX )/2 */
	FORM_FACTOR,                         /* Stem form factor (adim) */
	COEFFCOND,                           /* Define stomatal responsee to VPD in m/sec */
	BLCOND,                              /* Canopy Boundary Layer conductance */
	MAXCOND,                             /* Maximum leaf Conductance in m/sec */
	CUTCOND,                             /* Cuticular conductance in m/sec */
	MAXAGE,                              /* Maximum tree age */
	RAGE,                                /* Relative Age to give fAGE = 0.5 */
	NAGE,                                /* Power of relative Age in function for Age */
	GROWTHTMIN,                          /* Minimum temperature for growth */
	GROWTHTMAX,                          /* Maximum temperature for growth */
	GROWTHTOPT,                          /* Optimum temperature for growth */
	GROWTHSTART,                         /* Thermic sum  value for starting growth in °C */
	MINDAYLENGTH,                        /* Minimum day length for phenology */
	SWPOPEN,                             /* Soil water potential open */
	SWPCLOSE,                            /* Soil water potential close */
	OMEGA_CTEM,                          /* ALLOCATION PARAMETER */
	S0CTEM,                              /* PARAMETER CONTROLLING ALLOCATION TO STEM */
	R0CTEM,                              /* PARAMETER CONTROLLING ALLOCATION TO ROOT */
	F0CTEM,                              /* PARAMETER CONTROLLING ALLOCATION TO FOLIAGE */
	FRUIT_PERC,                          /* percentage of npp to fruit */
	CONES_LIFE_SPAN,                     /* cone life span */
	FINE_ROOT_LEAF,                      /* allocation new fine root C:new leaf (ratio) */
	STEM_LEAF,                           /* allocation new stem C:new leaf (ratio) */
	COARSE_ROOT_STEM,                    /* allocation new coarse root C:new stem (ratio) */
	LIVE_TOTAL_WOOD,                     /* allocation new live wood C:new total wood C (ratio) */
	CN_LEAVES,                           /* CN of leaves (kgC/kgN) */
	CN_FALLING_LEAVES,                   /* CN of leaf litter (kgC/kgN) */
	CN_FINE_ROOTS,                       /* CN of fine roots (kgC/kgN) */
	CN_LIVE_WOODS,                       /* CN of live woods (kgC/kgN) */
	CN_DEAD_WOODS,                       /* CN of dead woods (kgC/kgN) */
	LEAF_LITR_LAB_FRAC,                  /* (DIM) leaf litter labile fraction */
	LEAF_LITR_CEL_FRAC,                  /* (DIM) leaf litter cellulose fraction */
	LEAF_LITR_LIGN_FRAC,                 /* (DIM) leaf litter lignin fraction */
	FROOT_LITR_LAB_FRAC,                 /* (DIM) fine root litter labile fraction */
	FROOT_LITR_CEL_FRAC,                 /* (DIM) fine root litter cellulose fraction */
	FROOT_LITR_LIGN_FRAC,                /* (DIM) fine root litter lignin fraction */
	DEAD_WOOD_CEL_FRAC,                  /* (DIM) dead wood litter cellulose fraction */
	DEAD_WOOD_LIGN_FRAC,                 /* (DIM) dead wood litter lignin fraction */
	BUD_BURST,                           /* days of bud burst at the beginning of growing season (only for deciduous) */
	LEAF_FALL_FRAC_GROWING,              /* proportions of the growing season of leaf fall */
	LEAF_FROOT_TURNOVER,                 /* Average yearly leaves and fine root turnover rate */
	LIVE_WOOD_TURNOVER,                  /* Average yearly live wood turnover rate */
	DBHDCMAX,                            /* maximum dbh crown diameter relationship when minimum density */
	DBHDCMIN,                            /* minimum dbh crown diameter relationship when maximum density */
	SAP_A,                               /* a coefficient for sapwood */
	SAP_B,                               /* b coefficient for sapwood */
	SAP_LEAF,                            /* sapwood_max leaf area ratio in pipe model */
	SAP_WRES,                            /* Sapwood-Reserve biomass ratio used if no Wres data are available */
	STEMCONST_P,                         /* constant in the stem mass vs. diameter relationship */
	STEMPOWER_P,                         /* power in the stem mass vs. diameter relationship */
	CRA,                                 /* Chapman-Richards a parameter (maximum height, meter) */
	CRB,                                 /* Chapman-Richards b parameter */
	CRC,                                 /* Chapman-Richards c parameter */
	HDMAX_A,                             /* A parameter for Height (m) to Base diameter (m) ratio MAX */
	HDMAX_B,                             /* B parameter for Height (m) to Base diameter (m) ratio MAX */
	HDMIN_A,                             /* A parameter for Height (m) to Base diameter (m) ratio MIN */
	HDMIN_B,                             /* B parameter for Height (m) to Base diameter (m) ratio MIN */
	CROWN_FORM_FACTOR,                   /* Crown form factor (0 = cylinder, 1 = cone, 2 = sphere, 3 = ellipsoid) */
	CROWN_A,                             /* Crown a parameter */
	CROWN_B,                             /* Crown b parameter */
	MAXSEED,                             /* maximum seeds number (see TREEMIG) */
	MASTSEED,                            /* Musting year (see TREEMIG) */
	WEIGHTSEED,                          /* single fruit weight in g */
	SEXAGE,                              /* Age for sexual maturity */
	GERMCAPACITY,                        /* Geminability rate */
	ROTATION,                            /* rotation for final harvest (based on tree age) */
	THINNING,                            /* thinning regime (based on year simulation) */
	THINNING_REGIME,                     /* thinning regime (0 = above, 1 = below) */
	THINNING_INTENSITY,                  /* thinning intensity (% of Basal Area/N-tree to remove) */

	/*********************************************STATE VARIABLES*********************************************/
	/*********************************************************************************************************/

	DBHDC_EFF,                           /* Crown Projected Diameter from DBH in function of density*/
	DENMAX,                              /* maximum stand density (trees/cell) */
	DENMIN,                              /* minimum stand density (trees/cell) */
	CROWN_RADIUS,                        /* Crown Projected Radius in m */
	CROWN_DIAMETER,                      /* Crown Projected Diameter in m */
	CROWN_HEIGHT,                        /* Crown height in m */
	CROWN_AREA_PROJ,                     /* Crown Projected Area in m2 (at zenith angle) */
	CROWN_AREA_EXP,                      /* Crown Exposed Area in m2 (considered as the area that can intercepts light) */
	CROWN_VOLUME,                        /* Crown Volume in m3 */
	CROWN_DENSITY,                       /* Crown density m-1 */
	CANOPY_COVER_PROJ,                   /* Canopy Projected Cover % of pixel covered (at zenith angle) */
	CANOPY_COVER_EXP,                    /* Canopy Exposed Cover % of pixel exposed (integrated all over all viewing angles) */
	DAILY_CANOPY_COVER_PROJ,             /* Daily Canopy Projected Cover % of pixel covered (at zenith angle) */
	DAILY_CANOPY_COVER_EXP,              /* Daily Canopy Exposed Cover % of pixel exposed (integrated all over all viewing angles) */
	HD_EFF,                              /* Effective Height/Diameter ratio */
	HD_MAX,                              /* Height (m) to Base diameter (m) ratio MAX */
	HD_MIN,                              /* Height (m) to Base diameter (m) ratio MIN */
	SAPWOOD_AREA,                        /* tree sapwood area cm2*/
	SAPWOOD_PERC,                        /* sapwood area to toal stem area */
	HEARTWOOD_AREA,                      /* tree heartwood area cm^2*/
	HEARTWOOD_PERC,                      /* heartwood area to total stem area */
	BASAL_AREA,                          /* Individual Basal Area (cm2/area tree) */
	BASAL_AREA_m2,                       /* Individual Basal Area (m2/area tree) */
	STAND_BASAL_AREA,                    /* Class Basal Area of overall class (cm2/area tree class) */
	STAND_BASAL_AREA_m2,                 /* Class Basal Area of overall class (m2/area tree class) */
	MASS_DENSITY,                        /* mass density (t/m3)*/
	VOLUME,                              /* Stem Volume (m3/sizeCell) */
	TREE_VOLUME,                         /* Single Tree Volume (m3/tree)*/
	CAI,                                 /* Current Annual Increment (m3/sizeCell/year) */
	MAI,                                 /* Mean Annual Volume Increment (m3/sizeCell/year) */
	STEMCONST,
	MAX_SPECIES_COVER,                   /* maximum species-specific cell cover (fraction) */

	/* PAR */
	PAR,                                /* Photosynthetically Active Radiation molPAR/m^2/day */
	PAR_SUN,                            /* Photosynthetically Active Radiation molPAR/m^2/day for sun leaves */
	PAR_SHADE,                          /* Photosynthetically Active Radiation molPAR/m^2/day for shaded leaves */
	PAR_REFL,                           /* Reflected Photosynthetically Active Radiation molPAR/m^2/day OVERALL CANOPY */
	PAR_REFL_SUN,                       /* Reflected Photosynthetically Active Radiation molPAR/m^2/day for sun leaves */
	PAR_REFL_SHADE,                     /* Reflected Photosynthetically Active Radiation molPAR/m^2/day for shaded leaves */
	APAR,                               /* Absorbed Photosynthetically Active Radiation molPAR/m^2/day */
	APAR_SUN,                           /* Absorbed Photosynthetically Active Radiation molPAR/m^2/day for sun leaves */
	APAR_SHADE,                         /* Absorbed Photosynthetically Active Radiation molPAR/m^2/day for shaded leaves */
	TRANSM_PAR,                         /* Transmitted Photosynthetically Active Radiation molPAR/m^2/day */
	TRANSM_PAR_SUN,                     /* Transmitted Photosynthetically Active Radiation molPAR/m^2/day from sun leaves */
	TRANSM_PAR_SHADE,                   /* Transmitted Photosynthetically Active Radiation molPAR/m^2/day from shaded leaves */

	YEARLY_APAR,

	/* short wave */
	SW_RAD,                             /* Short Wave Radiation in W/m2 */
	SW_RAD_SUN,                         /* Short Wave Radiation in W/m2 for sun leaves */
	SW_RAD_SHADE,                       /* Short Wave Radiation in W/m2 for shaded leaves */
	SW_RAD_REFL,                        /* Reflected Short Wave Radiation W/m2 OVERALL CANOPY */
	SW_RAD_REFL_SUN,                    /* Reflected Short Wave Radiation W/m2 for sun leaves */
	SW_RAD_REFL_SHADE,                  /* Reflected Short Wave Radiation W/m2 for shaded leaves */
	SW_RAD_ABS,                         /* Absorbed Net Short Wave radiation in W/m2 */
	SW_RAD_ABS_SUN,                     /* Absorbed Net Short Wave Radiation W/m2 for sun leaves */
	SW_RAD_ABS_SHADE,                   /* Absorbed Net Short Wave Radiation W/m2 for shaded leaves */
	SW_RAD_TRANSM,                      /* Transmitted Net Short Wave Radiation W/m2 */
	SW_RAD_TRANSM_SUN,                  /* Transmitted Net Short Wave Radiation W/m2 for sun leaves */
	SW_RAD_TRANSM_SHADE,                /* Transmitted Net Short Wave Radiation W/m2 for shaded leaves */

	/* long wave */
	NET_LW_RAD,                         /* Long Wave Radiation in W/m2 */
	LW_RAD_REFL,                        /* Reflected Long Wave Radiation W/m2 OVERALL CANOPY */
	LW_RAD_EMIT,                        /* Emitted Long Wave radiation in W/m2 */
	LW_RAD_EMIT_SUN,                    /* Emitted Long Wave Radiation W/m2 for sun leaves */
	LW_RAD_EMIT_SHADE,                  /* Emitted Long Wave Radiation W/m2 for shaded leaves */
	LW_RAD_ABS,                         /* Absorbed Long Wave radiation in W/m 2*/
	LW_RAD_ABS_SUN,                     /* Absorbed Long Wave Radiation W/m2 for sun leaves */
	LW_RAD_ABS_SHADE,                   /* Absorbed Long Wave Radiation W/m2 for shaded leaves */
	LW_RAD_TRANSM,                      /* Transmitted Long Wave Radiation W/m2 */
	LW_RAD_TRANSM_SUN,                  /* Transmitted Long Wave Radiation W/m2 for sun leaves */
	LW_RAD_TRANSM_SHADE,                /* Transmitted Long Wave Radiation W/m2 for shaded leaves */

	/* net radiation */
	NET_RAD,                            /* Short Wave Radiation in W/m2 */
	RAD_REFL,                           /* Reflected Radiation W/m2 OVERALL CANOPY */
	NET_RAD_ABS,                        /* Absorbed Net radiation in W/m2 */
	NET_RAD_ABS_SUN,                    /* Absorbed Net Radiation W/m2 for sun leaves */
	NET_RAD_ABS_SHADE,                  /* Absorbed Net Radiation W/m2 for shaded leaves */
	NET_RAD_TRANSM,                     /* Transmitted Net Radiation W/m2 */
	NET_RAD_TRANSM_SUN,                 /* Transmitted Net Radiation W/m2 for sun leaves */
	NET_TRANSM_SHADE,                   /* Transmitted Net Radiation W/m2 for shaded leaves */

	/* PPFD */
	PPFD,                               /* Photosynthetic Photon Flux Density umol/m2/sec */
	PPFD_SUN,                           /* Photosynthetic Photon Flux Density umol/m2/sec for sun leaves */
	PPFD_SHADE,                         /* Photosynthetic Photon Flux Density umol/m2/sec f0r shaded leaves */
	PPFD_REFL,                          /* Transmitted Photosynthetic Photon Flux Density umol/m2/sec */
	PPFD_REFL_SUN,                      /* Transmitted Photosynthetic Photon Flux Density umol/m2/sec for sun leaves */
	PPFD_REFL_SHADE,                    /* Transmitted Photosynthetic Photon Flux Density umol/m2/sec for shaded leaves */
	PPFD_ABS,                           /* Absorbed Photosynthetic Photon Flux Density umol/m2/sec */
	PPFD_ABS_SUN,                       /* Absorbed Photosynthetic Photon Flux Density umol/m2/sec for sun leaves */
	PPFD_ABS_SHADE,                     /* Absorbed Photosynthetic Photon Flux Density umol/m2/sec for shaded leaves */
	PPFD_TRANSM,                        /* Transmitted Photosynthetic Photon Flux Density umol/m2/sec */
	PPFD_TRANSM_SUN,                    /* Transmitted Photosynthetic Photon Flux Density umol/m2/sec from sun leaves */
	PPFD_TRANSM_SHADE,                  /* Transmitted Photosynthetic Photon Flux Density umol/m2/sec  from shaded leaves */

	/* modifiers */
	F_VPD,                              /* VPD modifier */
	F_LIGHT,                            /* LIGHT modifier */
	F_LIGHT_SUN,                        /* LIGHT modifier for Sun leaves */
	F_LIGHT_SHADE,                      /* LIGHT modifier for Shaded leaves */
	F_AGE,                              /* AGE modifier */
	FR,                                 /* Nitrogen fertility rate */
	F_NUTR,                             /* SOIL NUTRIENT Modifier */
	F_T,                                /* TEMPERATURE modifier */
	F_SW,                               /* SOIL WATER modifier */
	F_DROUGHT,                          /* SOIL DROUGHT modifier (see Duursma et al., 2008) */
	F_PSI,                              /* SOIL WATER modifier using PSI, see Biome */
	F_CO2,                              /* CO2 soil fertilization effect */
	F_CO2_TR,                           /* CO2 reduction effect (for stomatal conductance) */
	PHYS_MOD,                           /* PHYSIOLOGICAL modifier */

	/* water */
	CANOPY_CONDUCTANCE,
	CANOPY_BLCOND,                      /* Canopy Boundary Layer conductance (m s-1) */
	FRAC_DAYTIME_TRANSP,                /* fraction of daytime for transpiration (and photosynthesis) */
	CANOPY_INT,                         /* canopy interception of rainfall mm day-1 */
	CANOPY_EVAPO,                       /* canopy evaporation mm day-1 */
	CANOPY_WET,                         /* it should'nt be reset every day */
	CANOPY_TRANSP,                      /* canopy transpiration mm day -1 */
	CANOPY_EVAPO_TRANSP,                /* canopy evapotranspiration mm day-1 */
	CANOPY_INT_SNOW,                    /* canopy interception of snow mm day-1 */
	CANOPY_WATER,                       /* it should'nt be reset every day */
	OLD_CANOPY_WATER,                   /* it should'nt be reset every day */
	CANOPY_SNOW,                        /* it should'nt be reset every day */
	OLD_CANOPY_SNOW,                    /* it should'nt be reset every day */
	CANOPY_FRAC_DAY_TRANSP,             /* Fraction of daily canopy transpiration (DIM) */
	CANOPY_LATENT_HEAT,                 /* canopy latent heat W m-2 */
	CANOPY_SENSIBLE_HEAT,               /* canopy sensible heat W m-2 */

	MONTHLY_CANOPY_TRANSP,              /* monthly canopy transpiration mm month-1 */
	MONTHLY_CANOPY_INT,                 /* monthly canopy interception mm month-1 */
	MONTHLY_CANOPY_EVAPO_TRANSP,        /* monthly canopy evapotranspiration mm month-1 */
	YEARLY_CANOPY_TRANSP,               /* annual canopy transpiration mm year-1 */
	YEARLY_CANOPY_INT,                  /* annual canopy interception mm year-1 */
	YEARLY_CANOPY_EVAPO_TRANSP,         /* annual canopy evapotranspiration mm year-1 */
	MONTHLY_CANOPY_LATENT_HEAT,         /* monthly canopy latent heat W m-2 */
	YEARLY_CANOPY_LATENT_HEAT,          /* annual canopy latent heat W m-2 */

	/* carbon use efficiency */
	CUE,                                /* daily Carbon Use Efficiency */
	MONTHLY_CUE,                        /* monthly Carbon Use Efficiency */
	YEARLY_CUE,                         /* annual Carbon Use Efficiency */

	/* water use efficiency */
	WUE,                                /* daily Water Use Efficiency */
	MONTHLY_WUE,                        /* monthly Water Use Efficiency */
	YEARLY_WUE,                         /* annual Water Use Efficiency */
	iWUE,                               /* daily intrinsic Water Use Efficiency */
	MONTHLY_iWUE,                       /* monthly intrinsic Water Use Efficiency */
	YEARLY_iWUE,                        /* annual intrinsic Water Use Efficiency */

	/* LAI */
	LAI_PROJ,                           /* LAI for Projected Area covered (at zenith angle)(m^2/m2) */
	LAI_SUN_PROJ,                       /* LAI for sun leaves for Projected Area covered (at zenith angle) (m2 m-2) */
	LAI_SHADE_PROJ,                     /* LAI for shaded leaves for Projected Area covered (at zenith angle) (m2 m-2) */
	MAX_LAI_PROJ,                       /* LAI Projected at the very first day of senescence: parameter of the sigmoid function (m2 m-2) */
	PEAK_LAI_PROJ,                      /* Peak Projected LAI (m2 m-2) */
	ALL_LAI_PROJ,                       /* All side Projected LAI (m2) */
	LAI_EXP,                            /* LAI for Exposed Area covered (all over all viewing angles) (m2 m-2) */
	LAI_SUN_EXP,                        /* LAI for sun leaves for Exposed Area covered (all over all viewing angles) (m2 m-2) */
	LAI_SHADE_EXP,                      /* LAI for shaded leaves for Exposed Area covered (all over all viewing angles) (m2 m-2) */
	LAD,                                /* Leaf Area Density (m2/m2) */
	LEAF_TEMP_K,                        /* leaf temperature (K) */
	CANOPY_TEMP_K,                      /* canopy temperature (K) */
	CANOPY_TEMP_K_OLD,                  /* previous time canopy temperature (K) */

	/* carbon variable */
	C_FLUX,
	GPP,                                /* Daily Gross Primary Production gC/m2/day */
	MONTHLY_GPP,                        /* Monthly Gross Primary Production gC/m2/month */
	YEARLY_GPP,                         /* Yearly Gross Primary Production gC/m2/day */
	GPP_tC,                             /* Daily Gross Primary Production tC/sizeCell/day */
	NPP,                                /* Daily Net Primary Production gC/m2/day */
	MONTHLY_NPP,                        /* Monthly Net Primary Production gC/m2/day */
	YEARLY_NPP,                         /* Yearly Net Primary Production gC/m2/day */
	NPP_tC,                             /* Daily Net Primary Production tC/sizeCell/day */
	MONTHLY_NPP_tC,                     /* Monthly Net Primary Production tC/sizeCell/day */
	YEARLY_NPP_tC,                      /* Yearly Net Primary Production tC/sizeCell/day */

	/* nitrogen variable */
	NPP_gN,                             /* Daily Net Primary Production demand gN/m^2 day */
	MONTHLY_NPP_gN,                     /* Monthly Net Primary Production demand gN/m^2 day */
	YEARLY_NPP_gN,                      /* Yearly Net Primary Production demand gN/m^2 day */
	NPP_tN,                             /* Daily Net Primary Production demand tNC/sizeCell day */
	MONTHLY_NPP_tN,                     /* Monthly Net Primary Production demand tN/sizeCell day */
	YEARLY_NPP_tN,                      /* Yearly Net Primary Production demand tN/sizeCell day */

	/* biomass */
	/* transfer pools carbon to carbon pools */
	C_TO_LEAF,                          /* Daily Net Primary Production to Leaf pool (tC/sizeCell day) */
	C_TO_ROOT,                          /* Daily Net Primary Production to Root pool (tC/sizeCell day) */
	C_TO_FROOT,                         /* Daily Net Primary Production to Fine root pool (tC/sizeCell day) */
	C_TO_CROOT,                         /* Daily Net Primary Production to Coarse root pool (tC/sizeCell day) */
	C_TO_TOT_STEM,                      /* Daily Net Primary Production to Stem and Branch pool (tC/sizeCell day) */
	C_TO_STEM,                          /* Daily Net Primary Production to Stem pool (tC/sizeCell day) */
	C_TO_BRANCH,                        /* Daily Net Primary Production to Branch pool (tC/sizeCell day) */
	C_TO_RESERVE,                       /* Daily Net Primary Production to Reserve pool (tC/sizeCell day) */
	C_TO_FRUIT,                         /* Daily Net Primary Production to Fruit pool (tC/sizeCell day) */
	LEAF_TO_REMOVE,                     /* Daily Leaf to remove tC/sizeCell */
	FROOT_TO_REMOVE,                    /* Daily Fine Root to remove tC/sizeCell */
	FRUIT_TO_REMOVE,                    /* Daily Fruit to remove tC/sizeCell */
	C_LEAF_TO_RESERVE,                  /* Daily retranslocated C from Leaf pool to Reserve (tC/sizeCell day) */
	C_FROOT_TO_RESERVE,                 /* Daily retranslocated C from Fine root pool to Reserve (tC/sizeCell day) */
	C_LEAF_TO_LITR,                     /* Daily tC from Leaf pool to Litter (tC/sizeCell day) */
	C_FROOT_TO_LITR,                    /* Daily tC from Fine root pool to Soil (tC/sizeCell day) */
	C_FRUIT_TO_LITR,                    /* Daily tC from Fruit pool to Litter (tC/sizeCell day) */
	C_TO_LITR,                          /* Daily Total Carbon to Litter pool (tC/sizeCell day) */
	C_STEM_LIVEWOOD_TO_DEADWOOD,        /* Daily tC from Stem live wood pool to Stem dead wood (tC/sizeCell day) */
	C_CROOT_LIVE_WOOD_TO_DEADWOOD,      /* Daily tC from Coarse live wood pool to Coarse dead wood (tC/sizeCell day) */
	C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD,    /* Daily tC from Branch live wood pool to Branch dead wood (tC/sizeCell day) */
	C_LEAF_TO_LITR1C,                   /* Daily Leaf Litter Carbon to Litter labile carbon pool (tC/sizeCell day) */
	C_LEAF_TO_LITR2C,                   /* Daily Leaf Litter Carbon to Litter unshielded cellulose carbon pool (tC/sizeCell day) */
	C_LEAF_TO_LITR3C,                   /* Daily Leaf Litter Carbon to Litter shielded cellulose carbon pool (tC/sizeCell day) */
	C_LEAF_TO_LITR4C,                   /* Daily Leaf Litter Carbon to Litter lignin carbon pool (tC/sizeCell day) */
	C_FROOT_TO_LITR1C,                  /* Daily Fine Root Litter Carbon to Litter labile carbon pool (tC/sizeCell day) */
	C_FROOT_TO_LITR2C,                  /* Daily Fine Root Litter Carbon to Litter unshielded cellulose carbon pool (tC/sizeCell day) */
	C_FROOT_TO_LITR3C,                  /* Daily Fine Root Litter Carbon to Litter shielded cellulose carbon pool (tC/sizeCell day) */
	C_FROOT_TO_LITR4C,                  /* Daily Fine Root Litter Carbon to Litter lignin carbon pool (tC/sizeCell day) */
	M_C_TO_TOT_STEM,                    /* Monthly cumulated Net Primary Production to total stem biomass  (tC/month/sizeCell) */
	M_C_TO_STEM,                        /* Monthly cumulated Net Primary Production to stem biomass (tC/month/sizeCell) */
	M_C_TO_LEAF,                        /* Monthly cumulated Net Primary Production to leaf biomass (tC/month/sizeCell) */
	M_C_TO_ROOT,                        /* Monthly cumulated Net Primary Production to root biomass (tC/month/sizeCell) */
	M_C_TO_FROOT,                       /* Monthly cumulated Net Primary Production to fine root biomass (tC/month/sizeCell) */
	M_C_TO_CROOT,                       /* Monthly cumulated Net Primary Production to coarse root biomass (tC/month/sizeCell) */
	M_C_TO_RESERVE,                     /* Monthly cumulated Net Primary Production to reserve biomass (tC/month/sizeCell) */
	M_C_TO_BRANCH,                      /* Monthly cumulated Net Primary Production to branch and bark biomass (tC/month/sizeCell) */
	M_C_TO_FRUIT,                       /* Monthly cumulated Net Primary Production to fruit biomass (tC/month/sizeCell) */
	M_C_TO_STEM_SAPWOOD,                /* Monthly cumulated Net Primary Production to stem sapwood biomass (tC/month/sizeCell) */
	M_C_TO_CROOT_SAPWOOD,               /* Monthly cumulated Net Primary Production to coarse root biomass (tC/month/sizeCell) */
	M_C_TO_BRANCH_SAPWOOD,              /* Monthly cumulated Net Primary Production to branch sapwood biomass (tC/month/sizeCell) */
	Y_C_TO_TOT_STEM,                    /* Yearly cumulated Net Primary Production to total stem biomass (tC/year/sizeCell) */
	Y_C_TO_STEM,                        /* Yearly cumulated Net Primary Production to stem biomass (tC/year/sizeCell) */
	Y_C_TO_LEAF,                        /* Yearly cumulated Net Primary Production to leaf biomass (tC/year/sizeCell) */
	Y_C_TO_ROOT,                        /* Yearly cumulated Net Primary Production to root biomass (tC/year/sizeCell) */
	Y_C_TO_FROOT,                       /* Yearly cumulated Net Primary Production to fine root biomass (tC/year/sizeCell)*/
	Y_C_TO_CROOT,                       /* Yearly cumulated Net Primary Production to fine root biomass (tC/year/sizeCell)*/
	Y_C_TO_RESERVE,                     /* Yearly cumulated Net Primary Production to reserve biomass (tC/year/sizeCell) */
	Y_C_TO_BRANCH,                      /* Yearly cumulated Net Primary Production to branch and bark biomass (tC/year/sizeCell) */
	Y_C_TO_FRUIT,                       /* Yearly cumulated Net Primary Production to fruit biomass (tC/year/sizeCell) */
	Y_C_TO_STEM_SAPWOOD,                /* Yearly cumulated Net Primary Production to stem sapwood biomass (tC/year/sizeCell) */
	Y_C_TO_CROOT_SAPWOOD,               /* Yearly cumulated Net Primary Production to coarse root biomass (tC/year/sizeCell) */
	Y_C_TO_BRANCH_SAPWOOD,              /* Yearly cumulated Net Primary Production to branch sapwood biomass (tC/year/sizeCell) */

	/* carbon pools in tons of tC/sizeCell */
	LEAF_C,                             /* Current Leaf carbon pool tC/sizeCell */
	MAX_LEAF_C,                         /* Maximum Current Leaf carbon pool tC/sizeCell */
	TOT_ROOT_C,                         /* Current Total Coarse carbon pool tC/sizeCell */
	CROOT_C,                            /* Current Coarse root carbon pool tC/sizeCell */
	FROOT_C,                            /* Current Fine root carbon pool tC/sizeCell */
	MAX_FROOT_C,                        /* Maximum Current Fine root carbon pool tC/sizeCell */
	MAX_BUD_BURST_C,                    /* Maximum Current Leaf carbon pool for BudBurst tC/sizeCell */
	STEM_C,                             /* Current Stem carbon pool tC/sizeCell */
	BRANCH_C,                           /* Current Branch carbon pool tC/sizeCell */
	TOT_STEM_C,                         /* Current Stem + Branch carbon pool tC/sizeCell */
	MIN_RESERVE_C,                      /* Current Minimum reserve carbon pool tC/sizeCell */
	RESERVE_C,                          /* Current Reserve carbon pool tC/sizeCell */
	FRUIT_C,                            /* Current Fruit carbon pool tC/sizeCell */
	LITR_C,                             /* Current Litter carbon pool tC/sizeCell */
	SOIL_C,                             /* Current Soil carbon pool tC/sizeCell */
	LIVE_WOOD_C,                        /* Current Live wood carbon pool tC/sizeCell */
	DEAD_WOOD_C,                        /* Current Dead wood carbon pool tC/sizeCell */
	TOT_WOOD_C,                         /* Current Total wood carbon pool tC/sizeCell */
	TOTAL_C,                            /* Current Total carbon pool tC/sizeCell */
	STEM_LIVE_WOOD_C,                   /* Current Stem live wood carbon pool tC/sizeCell */
	STEM_DEAD_WOOD_C,                   /* Current Stem dead wood carbon pool tC/sizeCell */
	CROOT_LIVE_WOOD_C,                  /* Current Coarse root live wood carbon pool tC/sizeCell */
	CROOT_DEAD_WOOD_C,                  /* Current Coarse root dead wood carbon pool tC/sizeCell */
	BRANCH_LIVE_WOOD_C,                 /* Current Branch live wood carbon pool tC/sizeCell */
	BRANCH_DEAD_WOOD_C,                 /* Current Branch dead wood carbon pool tC/sizeCell */
	STEM_SAPWOOD_C,                     /* Current Stem sapwood carbon pool tC/sizeCell */
	STEM_HEARTWOOD_C,                   /* Current Stem heartwood carbon pool tC/sizeCell */
	CROOT_SAPWOOD_C,                    /* Current Coarse root sapwood carbon pool tC/sizeCell */
	CROOT_HEARTWOOD_C,                  /* Current Coarse root heartwood carbon pool tC/sizeCell */
	BRANCH_SAPWOOD_C,                   /* Current Branch sapwood carbon pool tC/sizeCell */
	BRANCH_HEARTWOOD_C,                 /* Current Branch heartwood carbon pool tC/sizeCell */
	TOT_SAPWOOD_C,                      /* Current total sapwood carbon pool tC/sizeCell */
	EFF_LIVE_TOTAL_WOOD_FRAC,           /* Age-related fraction of Live biomass per Total biomass */
	DAILY_LIVE_WOOD_TURNOVER,           /* Daily live wood turnover rate */


	/* per tree in KgC */
	AV_LEAF_MASS_KgC,                   /* Average Leaf carbon pool kgC/tree */
	AV_STEM_MASS_KgC,                   /* Average Stem carbon pool kgC/tree */
	AV_TOT_STEM_MASS_KgC,               /* Average Stem + Branch carbon pool kgC/tree */
	AV_ROOT_MASS_KgC,                   /* Average Total root carbon pool kgC/tree */
	AV_FROOT_MASS_KgC,                  /* Average Fine root carbon pool kgC/tree */
	AV_CROOT_MASS_KgC,                  /* Average Coarse carbon pool kgC/tree */
	AV_RESERVE_MASS_KgC,                /* Average Reserve carbon pool kgC/tree */
	AV_MIN_RESERVE_KgC,                 /* Average Minimum Reserve carbon pool kgC/tree */
	AV_BRANCH_MASS_KgC,                 /* Average Branch carbon pool kgC/tree */
	AV_LIVE_STEM_MASS_KgC,              /* Average Live Stem carbon pool kgC/tree */
	AV_DEAD_STEM_MASS_KgC,              /* Average Dead Stem carbon pool kgC/tree */
	AV_LIVE_CROOT_MASS_KgC,             /* Average Live Coarse root carbon pool kgC/tree */
	AV_DEAD_CROOT_MASS_KgC,             /* Average Dead Coarse root carbon pool kgC/tree */
	AV_LIVE_BRANCH_MASS_KgC,            /* Average Live Branch carbon pool kgC/tree */
	AV_DEAD_BRANCH_MASS_KgC,            /* Average Dead Stem carbon pool kgC/tree */
	AV_LIVE_WOOD_MASS_KgC,              /* Average Live Wood carbon pool kgC/tree */
	AV_DEAD_WOOD_MASS_KgC,              /* Average Dead Wood carbon pool kgC/tree */
	AV_TOT_WOOD_MASS_KgC,               /* Average Total Wood carbon pool kgC/tree */

	/* carbon biomass pools in tDM/sizeCell */
	BIOMASS_LEAF_tDM,                   /* Current Leaf carbon pool tDM/sizeCell */
	BIOMASS_ROOTS_TOT_tDM,              /* Current Root carbon pool tDM/sizeCell */
	BIOMASS_CROOT_tDM,                  /* Current Coarse root carbon pool tDM/sizeCell */
	BIOMASS_FROOT_tDM,                  /* Current Fine root carbon pool tDM/sizeCell */
	BIOMASS_STEM_tDM,                   /* Current Stem carbon pool tDM/sizeCell */
	BIOMASS_BRANCH_tDM,                 /* Current Branch carbon pool tDM/sizeCell */
	BIOMASS_TOT_STEM_tDM,               /* Current Total Stem carbon pool tDM/sizeCell */
	RESERVE_tDM,                        /* Current Reserve carbon pool tDM/sizeCell */
	BIOMASS_FRUIT_tDM,                  /* Current Fruit carbon pool tDM/sizeCell */
	BIOMASS_LIVE_WOOD_tDM,              /* Current Live Wood carbon pool tDM/sizeCell */
	BIOMASS_DEAD_WOOD_tDM,              /* Current Dead Wood carbon pool tDM/sizeCell */
	BIOMASS_STEM_LIVE_WOOD_tDM,         /* Current Live Stem carbon pool tDM/sizeCell */
	BIOMASS_STEM_DEAD_WOOD_tDM,         /* Current Dead Stem carbon pool tDM/sizeCell */
	BIOMASS_CROOT_LIVE_WOOD_tDM,        /* Current Live Coarse root carbon pool tDM/sizeCell */
	BIOMASS_CROOT_DEAD_WOOD_tDM,        /* Current Dead Coarse root carbon pool tDM/sizeCell */
	BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM,  /* Current Live Stem carbon pool tDM/sizeCell */
	BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM,  /* Current Dead Stem carbon pool tDM/sizeCell */

	/* per tree in KgDM */
	AV_LEAF_MASS_KgDM,                  /* Average Leaf carbon pool kgDM/tree */
	AV_TOT_STEM_MASS_KgDM,              /* Average Stem + Branch carbon pool kgDM/tree */
	AV_STEM_MASS_KgDM,                  /* Average Leaf carbon pool kgDM/tree */
	AV_ROOT_MASS_KgDM,                  /* Average Root carbon pool kgDM/tree */
	AV_FROOT_MASS_KgDM,                 /* Average Fine root carbon pool kgDM/tree */
	AV_CROOT_MASS_KgDM,                 /* Average Coarse root carbon pool kgDM/tree */
	AV_RESERVE_MASS_KgDM,               /* Average Reserve carbon pool kgDM/tree */
	AV_BRANCH_MASS_KgDM,                /* Average Branch carbon pool kgDM/tree */
	AV_LIVE_STEM_MASS_KgDM,             /* Average Live Stem carbon pool kgDM/tree */
	AV_DEAD_STEM_MASS_KgDM,             /* Average Dead Stem carbon pool kgDM/tree */
	AV_LIVE_CROOT_MASS_KgDM,      /* Average Live Coarse root carbon pool kgDM/tree */
	AV_DEAD_CROOT_MASS_KgDM,      /* Average Dead Coarse root carbon pool kgDM/tree */
	AV_LIVE_WOOD_MASS_KgDM,             /* Average Live Wood carbon pool kgDM/tree */
	AV_DEAD_WOOD_MASS_KgDM,             /* Average Dead Wood carbon pool kgDM/tree */
	AV_LIVE_BRANCH_MASS_KgDM,           /* Average Live Branch carbon pool kgDM/tree */
	AV_DEAD_BRANCH_MASS_KgDM,           /* Average Dead Branch carbon pool kgDM/tree */

	/* Above and Below Ground Biomass */
	CLASS_AGB,                          /* Above Ground Biomass pool tC/sizeCell */
	CLASS_BGB,                          /* Below Ground Biomass pool tC/sizeCell */

	/* converted fraction for turnover */
	WS_sap_tDM,                         /* Fraction Sapwood in Stem biomass */
	WS_heart_tDM,                       /* Fraction Heartwood in Stem biomass */
	WRC_sap_tDM,                        /* Fraction Sapwood in Coarse root biomass */
	WRC_heart_tDM,                      /* Fraction Heartwood in Coarse root biomass */
	WBB_sap_tDM,                        /* Fraction Sapwood in Branch biomass */
	WBB_heart_tDM,                      /* Fraction Heartwood in Branch biomass */
	WTOT_sap_tDM,                       /* Fraction Sapwood in Total biomass */

	/* converted fraction for allocation */
	FROOT_LEAF_FRAC,                    /* Fraction fine root to leaf */
	STEM_LEAF_FRAC,                     /* Fraction stem to leaf */
	CROOT_STEM_FRAC,                    /* Fraction coarse root to stem */
	LIVE_TOTAL_WOOD_FRAC,               /* Fraction live to total stem wood */
	FINE_COARSE_ROOT,                   /* Fraction fine to coarse root */
	FRACBB,                             /* Fraction branch and bark to stem */

	/* Maintenance respiration */
	DAILY_LEAF_MAINT_RESP,              /* Daytime leaf maintenance respiration (gC/m2/day) */
	NIGHTLY_LEAF_MAINT_RESP,            /* Night time leaf maintenance respiration (gC/m2/day) */
	TOT_DAY_LEAF_MAINT_RESP,            /* Leaf maintenance respiration (gC/m2/day) */
	FROOT_MAINT_RESP,                   /* Fine root maintenance respiration (gC/m2/day) */
	STEM_MAINT_RESP,                    /* Stem maintenance respiration (gC/m2/day) */
	CROOT_MAINT_RESP,                   /* Coarse root maintenance respiration (gC/m2/day) */
	BRANCH_MAINT_RESP,                  /* Branch maintenance respiration (gC/m2/day) */
	TOTAL_MAINT_RESP,                   /* Total maintenance respiration (gC/m2/day) */
	MONTHLY_TOTAL_MAINT_RESP,           /* Monthly Total maintenance respiration (gC/m2/month) */
	YEARLY_TOTAL_MAINT_RESP,            /* Yearly Total maintenance respiration (gC/m2/year) */
	TOTAL_MAINT_RESP_tC,                /* Total maintenance respiration (tC/sizecell/day) */

	/* Growth respiration */
	EFF_GRPERC,                         /* (DIM) Growth respiration ratio based on age */
	LEAF_GROWTH_RESP,                   /* Leaf growth respiration (gC/m2/day) */
	FROOT_GROWTH_RESP,                  /* Fine root growth respiration (gC/m2/day) */
	STEM_GROWTH_RESP,                   /* Stem growth respiration (gC/m2/day) */
	BRANCH_GROWTH_RESP,                 /* Branch growth respiration (gC/m2/day) */
	CROOT_GROWTH_RESP,                  /* Coarse root growth respiration (gC/m2/day) */
	TOTAL_GROWTH_RESP,                  /* Total growth respiration (gC/m2/day) */
	MONTHLY_TOTAL_GROWTH_RESP,          /* Monthly Total growth respiration (gC/m2/month) */
	YEARLY_TOTAL_GROWTH_RESP,           /* Yearly Total growth respiration (gC/m2/year) */
	TOTAL_GROWTH_RESP_tC,               /* Total growth respiration (tC/sizecell/day) */

	/* Autotrophic respiration */
	LEAF_AUT_RESP,                      /* Leaf autotrophic respiration (gC/m2/day) */
	FROOT_AUT_RESP,                     /* Fine root autotrophic respiration (gC/m2/day) */
	STEM_AUT_RESP,                      /* Stem autotrophic respiration (gC/m2/day) */
	CROOT_AUT_RESP,                     /* Coarse root autotrophic respiration (gC/m2/day) */
	BRANCH_AUT_RESP,                    /* Branch autotrophic respiration (gC/m2/day) */
	TOTAL_AUT_RESP,                     /* Total autotrophic respiration (gC/m2/day) */
	MONTHLY_LEAF_AUT_RESP,              /* Monthly Leaf autotrophic respiration (gC/m2/month) */
	MONTHLY_FROOT_AUT_RESP,             /* Monthly Fine root autotrophic respiration (gC/m2/month) */
	MONTHLY_STEM_AUT_RESP,              /* Monthly Stem autotrophic respiration (gC/m2/month) */
	MONTHLY_CROOT_AUT_RESP,             /* Monthly Coarse root autotrophic respiration (gC/m2/month) */
	MONTHLY_BRANCH_AUT_RESP,            /* Monthly Branch autotrophic respiration (gC/m2/month) */
	MONTHLY_TOTAL_AUT_RESP,             /* Monthly Total autotrophic respiration (gC/m2/month) */
	YEARLY_LEAF_AUT_RESP,               /* Annual Leaf autotrophic respiration (gC/m2/year) */
	YEARLY_FROOT_AUT_RESP,              /* Annual Fine root autotrophic respiration (gC/m2/year) */
	YEARLY_STEM_AUT_RESP,               /* Annual Stem autotrophic respiration (gC/m2/year) */
	YEARLY_CROOT_AUT_RESP,              /* Annual Coarse root autotrophic respiration (gC/m2/year) */
	YEARLY_BRANCH_AUT_RESP,             /* Annual Branch autotrophic respiration (gC/m2/year) */
	YEARLY_TOTAL_AUT_RESP,              /* Annual Total autotrophic respiration (gC/m2/year) */
	TOTAL_AUT_RESP_tC,                  /* Total autotrophic respiration (tC/sizecell/day) */

	/* nitrogen to nitrogen pools */
	N_TO_LEAF,                          /* Daily Net Primary Production to Leaf pool (tN/sizeCell day) */
	N_TO_ROOT,                          /* Daily Net Primary Production to Root pool (tN/sizeCell day) */
	N_TO_FROOT,                         /* Daily Net Primary Production to Fine root pool (tN/sizeCell day) */
	N_TO_CROOT,                         /* Daily Net Primary Production to Coarse root pool (tN/sizeCell day) */
	N_TO_TOT_STEM,                      /* Daily Net Primary Production to Stem and Branch pool (tN/sizeCell day) */
	N_TO_STEM,                          /* Daily Net Primary Production to Stem pool (tN/sizeCell day) */
	N_TO_BRANCH,                        /* Daily Net Primary Production to Branch pool (tN/sizeCell day) */
	N_TO_RESERVE,                       /* Daily Net Primary Production to Reserve pool (tN/sizeCell day) */
	N_TO_FRUIT,                         /* Daily Net Primary Production to Fruit pool (tN/sizeCell day) */
	N_TO_LITTER,                        /* Daily Net Primary Production to Litter pool (tN/sizeCell day) */
	N_TO_LITR,                          /* Daily Litter to Litter nitrogen pool (tN/sizeNell day) */
	N_LEAF_TO_LITR,                     /* Daily Leaf Litter to Litter labile nitrogen pool (tN/sizeNell day) */
	N_FROOT_TO_LITR,                    /* Daily Leaf Litter to Litter labile nitrogen pool (tN/sizeNell day) */
	N_LEAF_TO_LITR1N,                   /* Daily Leaf Litter to Litter labile nitrogen pool (tN/sizeNell day) */
	N_LEAF_TO_LITR2N,                   /* Daily Leaf Litter to Litter unshielded cellulose nitrogen pool (tN/sizeNell day) */
	N_LEAF_TO_LITR3N,                   /* Daily Leaf Litter to Litter shielded cellulose nitrogen pool (tN/sizeNell day) */
	N_LEAF_TO_LITR4N,                   /* Daily Leaf Litter to Litter lignin nitrogen pool (tN/sizeNell day) */
	N_FROOT_TO_LITR1N,                  /* Daily Fine Root Litter to Litter labile nitrogen pool (tN/sizeNell day) */
	N_FROOT_TO_LITR2N,                  /* Daily Fine Root Litter to Litter unshielded cellulose nitrogen pool (tN/sizeNell day) */
	N_FROOT_TO_LITR3N,                  /* Daily Fine Root Litter to Litter shielded cellulose nitrogen pool (tN/sizeNell day) */
	N_FROOT_TO_LITR4N,                  /* Daily Fine Root Litter to Litter lignin nitrogen pool (tN/sizeNell day) */
	N_LEAF_TO_RESERVE,                  /* Daily retranslocated C from Leaf pool to Reserve (tN/sizeCell day) */
	N_FROOT_TO_RESERVE,                 /* Daily retranslocated C from Fine root pool to Reserve (tN/sizeCell day) */
	N_STEM_LIVEWOOD_TO_DEADWOOD,        /* Daily tN from Stem live wood pool to Stem dead wood (tN/sizeCell day) */
	N_CROOT_LIVE_WOOD_TO_DEADWOOD,      /* Daily tN from Coarse live wood pool to Coarse dead wood (tN/sizeCell day) */
	N_BRANCH_LIVE_WOOD_TO_DEAD_WOOD,    /* Daily tN from Branch live wood pool to Branch dead wood (tN/sizeCell day) */
	M_N_TO_TOT_STEM,                    /* Monthly cumulated Net Primary Production to total stem  tN/month/sizeCell) */
	M_N_TO_STEM,                        /* Monthly cumulated Net Primary Production to stem (tN/month/sizeCell) */
	M_N_TO_LEAF,                        /* Monthly cumulated Net Primary Production to leaf (tN/month/sizeCell) */
	M_N_TO_ROOT,                        /* Monthly cumulated Net Primary Production to root (tN/month/sizeCell) */
	M_N_TO_FROOT,                       /* Monthly cumulated Net Primary Production to fine root (tN/month/sizeCell) */
	M_N_TO_CROOT,                       /* Monthly cumulated Net Primary Production to coarse root (tN/month/sizeCell) */
	M_N_TO_RESERVE,                     /* Monthly cumulated Net Primary Production to reserve (tN/month/sizeCell) */
	M_N_TO_BRANCH,                      /* Monthly cumulated Net Primary Production to branch and bark (tN/month/sizeCell) */
	M_N_TO_FRUIT,                       /* Monthly cumulated Net Primary Production to fruit (tN/month/sizeCell) */
	Y_N_TO_TOT_STEM,                    /* Yearly cumulated Net Primary Production to total stem (tN/year/sizeCell) */
	Y_N_TO_STEM,                        /* Yearly cumulated Net Primary Production to stem (tN/year/sizeCell) */
	Y_N_TO_LEAF,                        /* Yearly cumulated Net Primary Production to leaf (tN/year/sizeCell) */
	Y_N_TO_ROOT,                        /* Yearly cumulated Net Primary Production to root (tN/year/sizeCell) */
	Y_N_TO_FROOT,                       /* Yearly cumulated Net Primary Production to fine root (tN/year/sizeCell)*/
	Y_N_TO_CROOT,                       /* Yearly cumulated Net Primary Production to fine root tN/year/sizeCell)*/
	Y_N_TO_RESERVE,                     /* Yearly cumulated Net Primary Production to reserve (tN/year/sizeCell) */
	Y_N_TO_BRANCH,                      /* Yearly cumulated Net Primary Production to branch and bark (tN/year/sizeCell) */
	Y_N_TO_FRUIT,                       /* Yearly cumulated Net Primary Production to fruit (tN/year/sizeCell) */

	/* nitrogen pools (tN/area) */
	LEAF_N,                             /* Current Leaf nitrogen pool tN/sizeCell */
	LEAF_FALLING_N,                     /* Current Leaf falling nitrogen pool tN/sizeCell */
	FROOT_N,                            /* Current Fine root nitrogen pool tN/sizeCell */
	CROOT_N,                            /* Current Coarse root nitrogen pool tN/sizeCell */
	CROOT_LIVE_WOOD_N,                  /* Current Live Coarse root nitrogen pool tN/sizeCell */
	CROOT_DEAD_WOOD_N,                  /* Current Dead Coarse root nitrogen pool tN/sizeCell */
	STEM_N,                             /* Current Stem nitrogen pool tN/sizeCell */
	STEM_LIVE_WOOD_N,                   /* Current Live Stem nitrogen pool tN/sizeCell */
	STEM_DEAD_WOOD_N,                   /* Current Dead Stem nitrogen pool tN/sizeCell */
	BRANCH_N,                           /* Current Branch nitrogen pool tN/sizeCell */
	BRANCH_LIVE_WOOD_N,                 /* Current Live Branch nitrogen pool tN/sizeCell */
	BRANCH_DEAD_WOOD_N,                 /* Current Dead Branch nitrogen pool tN/sizeCell */
	RESERVE_N,                          /* Current Reserve nitrogen pool tN/sizeCell */
	FRUIT_N,                            /* Current Friut nitrogen pool tN/sizeCell */
	TREE_N_DEMAND,                      /* Current Nitrogen demand for new plant tissues tN/sizeCell */
	LITR_N,                             /* Current Litter nitrogen pool tN/sizeCell */
	SOIL_N,                             /* Current soil nitrogen pool tN/sizeCell */
	TOT_STEM_N,                         /* Current Total Stem nitrogen pool tN/sizeCell */
	TOT_ROOT_N,                         /*Current Total root nitrogen pool tN/sizeCell */

	/* per tree in kgN */
	AV_LEAF_MASS_kgN,                   /* Average Leaf carbon pool kgN/tree */
	AV_STEM_MASS_kgN,                   /* Average Stem carbon pool kgN/tree */
	AV_TOT_STEM_MASS_kgN,               /* Average Stem + Branch carbon pool kgN/tree */
	AV_ROOT_MASS_kgN,                   /* Average Total root carbon pool kgN/tree */
	AV_FROOT_MASS_kgN,                  /* Average Fine root carbon pool kgN/tree */
	AV_CROOT_MASS_kgN,                  /* Average Coarse carbon pool kgN/tree */
	AV_BRANCH_MASS_kgN,                 /* Average Branch carbon pool kgN/tree */

	/* litter and soil pools */
	CWDC,                               /* (tC/sizecell) coarse woody debris C */
	LITR1C,                             /* (tC/sizecell) litter labile C */
	LITR2C,                             /* (tC/sizecell) litter unshielded cellulose C */
	LITR3C,                             /* (tC/sizecell) litter shielded cellulose C */
	LITR4C,                             /* (tC/sizecell) litter lignin C */
	LITR1N,                             /* (tN/sizecell) litter labile N */
	LITR2N,                             /* (tN/sizecell) litter unshielded Cellulose N */
	LITR3N,                             /* (tN/sizecell) litter shielded Cellulose N */
	LITR4N,                             /* (tN/sizecell) litter lignin N */
	SOIL1C,                             /* (tC/sizecell) microbial recycling pool C (fast) */
	SOIL2C,                             /* (tC/sizecell) microbial recycling pool C (medium) */
	SOIL3C,                             /* (tC/sizecell) microbial recycling pool C (slow) */
	SOIL4C,                             /* (tC/sizecell) recalcitrant SOM C (humus, slowest) */
	SOIL1N,                             /* (tN/sizecell) microbial recycling pool N (fast) */
	SOIL2N,                             /* (tN/sizecell) microbial recycling pool N (medium) */
	SOIL3N,                             /* (tN/sizecell) microbial recycling pool N (slow) */
	SOIL4N,                             /* (tN/sizecell) recalcitrant SOM N (humus, slowest) */
	/* fractions */
	LEAF_LITR_SCEL_FRAC,                /* (DIM) leaf litter shielded cellulose fraction */
	LEAF_LITR_USCEL_FRAC,               /* (DIM) leaf litter unshielded cellulose fraction */
	FROOT_LITR_SCEL_FRAC,               /* (DIM) fine root litter shielded cellulose fraction */
	FROOT_LITR_USCEL_FRAC,              /* (DIM) fine root litter unshielded fraction */
	DEAD_WOOD_SCEL_FRAC,                /* (DIM) dead wood litter shielded cellulose fraction */
	DEAD_WOOD_USCEL_FRAC,               /* (DIM) dead wood litter unshielded fraction */

	/* heterotrophic soil pools */
	LITR1_RH,                           /* (kgC/m2/d) heterotrophic respiration for litter labile */
	LITR2_RH,                           /* (kgC/m2/d) heterotrophic respiration for litter unshielded cellulose */
	LITR3_RH,                           /* (kgC/m2/d) heterotrophic respiration for litter shielded cellulose */
	LITR4_RH,                           /* (kgC/m2/d) heterotrophic respiration for litter lignin */
	SOIL1_RH,                           /* (kgC/m2/d) heterotrophic respiration for microbial recycling pool (fast) */
	SOIL2_RH,                           /* (kgC/m2/d) heterotrophic respiration for microbial recycling pool (medium) */
	SOIL3_RH,                           /* (kgC/m2/d) heterotrophic respiration for microbial recycling pool (slow) */
	SOIL4_RH,                           /* (kgC/m2/d) heterotrophic respiration for recalcitrant SOM (humus, slowest) */



	/* LPJ MORTALITY FUNCTION */
	AGEMORT,                            /* Age probability mortality function */

	VALUES
};

enum {

	N_TREE,                             /* Numbers of Adult Tree per class */
	N_STUMP,                            /* Number of stumps for coppice per class */
	N_SEED,                             /* Seeds number per class */
	DEAD_STEMS,                         /* Dead Trees per class */
	N_TREE_SAP,                         /* Numbers of Saplings per class */
	VEG_UNVEG,                          /* vegetative-un vegetative period per class */
	VEG_DAYS,                           /* day of vegetative period for class if != 0 is in veg period */
	FIRST_VEG_DAYS,                     /* first annual day of veg period */
	YEARLY_VEG_DAYS,                    /* annual number of vegetative days */
	BUD_BURST_COUNTER,                  /* days of budburst per class */
	DAY_FRAC_FOLIAGE_REMOVE,            /* days of leaf fall per class */
	DAY_FRAC_FINE_ROOT_REMOVE,          /* days of "fine root fall" per class */
	DAY_VEG_FOR_LEAF_FALL,              /* days for leaf fall */
	LEAF_FALL_COUNTER,                  /* counter for leaf fall */
	SENESCENCE_DAY_ONE,                 /* doy at first day of the senescence */

	COUNTERS
};

typedef struct {
	e_management management;

	char *name;                         /** species name **/
	int phenology_phase;                /* species-specific phenological phase */
	int counter[COUNTERS];
	double value[VALUES];
} species_t;

typedef struct
{
	species_t *species;

	int value;                          /** age value (year) **/
	int species_count;                  /* number of different species */
	int species_avail;

	/* for logger */
	int initial_species_count;
} age_t;

typedef struct
{
	age_t* ages;

	double value;                       /** dbh value (cm) **/
	int ages_count;                     /* number of different cohorts (ages) */
	int ages_avail;

	/* for logger */
	int initial_ages_count;

	int dbh_n_trees;                    /* number of trees per dbh class */
	double dbh_density;                 /* density of trees per dbh class */
} dbh_t;

typedef struct
{
	int layer_z;                        //fixme remove
	int layer_n_height_class;           /* number of height class per layer */
	int layer_n_trees;                  /* number of trees per layer */
	double layer_density;               /* tree density per layer (n_tree/sizecell) */
	double layer_cover;                 /* layer canopy cover per layer */
	double layer_avg_tree_height;       /* average tree layer height */
	double layer_tree_height_modifier;  /* layer level tree height modifier (see Wallace et al., 1991; Cannell & Grace 1993) */

} tree_layer_t;

typedef struct
{
	dbh_t *dbhs;

	double value;                       /** height value (m) **/
	int dbhs_count;                     /* number of different dbh classes */
	int dbhs_avail;

	/* for logger */
	int initial_dbhs_count;
	int height_z;
	double height_density;              /* tree density per height class (n_tree/sizecell) */
	double height_cover;                /* height class cover per height class (n_tree/sizecell) */
} height_t;

enum
{
	SOIL_CARBON,                    /* SOIL CARBON (gC/m2 soil) */
	PREVIOUS_ASW,
	WATER_BALANCE,
	OLD_WATER_BALANCE,
	MOIST_RATIO,
	VSWC,                           /* VOLUMETRIC SOIL WATER CONTENT (%VOL) */
	PSI_SAT,                        /* SOIL SATURATED MATRIC POTENTIAL */
	VWC,
	VWC_FC,                         /* SOIL FIELD CAPACITY VOLUMETRIC WATER CONTENT */
	VWC_SAT,                        /* SOIL SATURATED VOLUMETRIC WATER CONTENT */
	SOIL_B,                         /* SOIL MOISTURE PARAMETER */
	SOIL_W_SAT,                     /* (KGH2O/M2) SOILWATER AT SATURATION */
	SOIL_W_FC,                      /* (KGH2O/M2) SOILWATER AT FIELD CAPACITY */
	DAILYSOILEVAPORATIONWATT,
	DAILYSOILLATENTHEATFLUX,
	DAILYSOILSENSIBLEHEATFLUX,

	SOIL_COUNT
};

typedef struct {

	double value[SOIL_COUNT];


} soil_layer_s;

typedef struct
{
	/* tree height struct */
	height_t* heights;
	int heights_count;
	int heights_avail;

	/* tree layer struct */
	tree_layer_t* tree_layers;
	int tree_layers_count;
	int t_layers_avail;

	/* soil layer struct */
	soil_layer_s* soil_layers;
	int soil_layers_count;
	int s_layers_avail;

	int years_count;
	yos_t *years;

	int doy;

	/* for logger */
	int initial_tree_layers_count;
	int initial_heights_count;

	/* overall cell counter */
	int cell_tree_layers_count;
	int cell_heights_count;
	int cell_dbhs_count;
	int cell_ages_count;
	int cell_species_count;

	int year_stand;                                     /* input stand.txt row stand year */

	int x;                                              /* cell index within the matrix */
	int y;                                              /* cell index within the matrix */

	/* general variables */
	double abscission_daylength;                        /* day length for starting abscission (not used) */
	double av_yearly_daylength;                         /* average yearly daylength for regeneration function */
	double av_yearly_par_soil;                          /* average yearly par for soil for regeneration function */
	int north;                                          /* northern hemisphere north = 0, south hemisphere south = 1 */

	/* annual met values */
	double annual_tavg;                                 /* annual avg daily avg temperature (°C) */
	double annual_tmin;                                 /* annual avg daily min temperature (°C) */
	double annual_tmax;                                 /* annual avg daily max temperature (°C) */
	double annual_tday;                                 /* annual avg daily daytime temperature (°C) */
	double annual_tnight;                               /* annual avg daily nighttime temperature (°C) */
	double annual_tsoil;                                /* annual avg daily soil temperature (°C) */
	double annual_solar_rad;                            /* annual avg daily solar radiation (short wave) (MJ/m2/day) */
	double annual_precip;                               /* annual cumulate daily precipitation (mm) */
	double annual_vpd;                                  /* annual avg daily vpd (hPa) */

	/*forest structure variables*/	
	int Veg_Counter;                                    /* number of class in vegetative period  in the cell */
	double cell_cover;                                  /* canopy cover (ratio) */
	int cell_n_trees;                                   /* number of trees */
	int daily_dead_tree;                                /* daily number of dead tree */
	int monthly_dead_tree;                              /* monthly number of dead tree */
	int annual_dead_tree ;                              /* annual number of dead tree */
	int saplings_counter;                               /* number of class as sapling */
	double basal_area;                                  /* cumulated basal area at cell level */
	double agb;                                         /* current above ground biomass at cell level (tC/cell) */
	double bgb;                                         /* current below ground biomass at cell level (tC/cell) */

	/* radiation variables */
	/* short wave radiation */
	double sw_rad_down_MJ;                              /* Downward short wave radiation flux (MJ/m2/day) */
	double short_wave_radiation_upward_MJ;              /* Upward short wave radiation flux (MJ/m2/day) */
	double short_wave_radiation_upward_W;               /* Upward short wave radiation flux (W/m2) */
	double sw_rad_refl;                                 /* Short Wave radiation reflected (W/m2) */
	double sw_rad_for_soil_refl;                        /* Short Wave radiation reflected from soil level (W/m2) */
	double sw_rad_for_snow_refl;                        /* Short Wave radiation reflected from snow level (W/m2) */
	double sw_rad_abs;                                  /* Short Wave radiation absorbed tree level (W/m2) */
	double sw_rad_abs_soil;                             /* Short Wave radiation absorbed soil level (W/m2) */
	double sw_rad_abs_snow;                             /* Short Wave radiation absorbed snow level (W/m2) */
	double sw_rad_transm;                               /* Short Wave radiation transmitted (W/m2) */

	/* long wave radiation */
	double long_wave_radiation_upward_MJ;               /* Upward long wave radiation flux (MJ/m2/day) */
	double long_wave_radiation_upward_W;                /* Upward long wave radiation flux (W/m2) */
	double net_long_wave_radiation_MJ;                  /* Net long wave radiation flux (MJ/m2/day) */
	double net_long_wave_radiation_W;                   /* Net long wave radiation flux (W/m2) */
	double long_wave_absorbed;                          /* Total absorbed long wave wave radiation flux (W/m2) */
	double long_wave_reflected;                         /* Total reflected long wave radiation flux (W/m2) */
	double long_wave_emitted;                           /* Total emitted long wave radiation flux (W/m2) */
	double soil_long_wave_emitted;                      /* Soil emitted long wave radiation flux (W/m2) */
	double net_lw_rad_for_soil;                         /* Net Long Wave radiation to soil level (W/m2) */
	double net_rad_for_soil;                            /* Net radiation to soil level (W/m2) */

	/* PAR radiation */
	double apar;                                        /* cumulated absorbed PAR at tree level (molPAR/m2/day) */
	double apar_soil;                                   /* cumulated absorbed PAR at soil level (molPAR/m2/day) */
	double apar_snow;                                   /* cumulated absorbed PAR at snow level (molPAR/m2/day) */
	double par_transm;                                  /* cumulated transmitted PAR at cell level (molPAR/m2/day) */
	double par_refl;                                    /* cumulated reflected PAR at cell level (molPAR/m2/day) */
	double par_refl_soil;                               /* reflected PAR by the soil (molPAR/m2/day) */
	double par_refl_snow;                               /* reflected PAR by the snow (molPAR/m2/day) */

	/* PPFD radiation */
	double ppfd_abs;                                    /* Absorbed Photosynthetic photon flux density tree level (umol/m2/sec) */
	double ppfd_abs_soil;                               /* Absorbed Photosynthetic photon flux density soil level (umol/m2/sec) */
	double ppfd_abs_snow;                               /* Absorbed Photosynthetic photon flux density snow level (umol/m2/sec) */
	double ppfd_transm;                                 /* Transmitted Photosynthetic photon flux density (umol/m2/sec) */
	double ppfd_refl;                                   /* Reflected Photosynthetic photon flux density (umol/m2/sec) */
	double ppfd_refl_soil;                              /* PPFD reflected by the soil (umol/m2/day) */
	double ppfd_refl_snow;                              /* PPFD reflected by the snow (umol/m2/day) */

	double canopy_temp;                                 /* daily canopy temperature (°C) */
	double canopy_temp_k;                               /* daily canopy temperature (K) */
	double canopy_temp_diff;                            /* daily canopy temperature (K) */

	/*carbon variables*/
	double daily_gpp, monthly_gpp, annual_gpp;                            /* daily, monthly and annual GPP at cell level (gC/m2/ ) */
	double daily_npp, monthly_npp, annual_npp;                            /* daily, monthly and annual NPP at cell level (gC/m2/ ) */
	double daily_npp_tC, monthly_npp_tC, annual_npp_tC;                   /* daily, monthly and annual NPP at cell level (tC/cell/ ) */
	double daily_npp_tDM, monthly_npp_tDM, annual_npp_tDM;                /* daily, monthly and annual NPP at cell level (tDM/cell/ ) */
	double daily_aut_resp, monthly_aut_resp, annual_aut_resp;             /* daily, monthly and annual aut resp at cell level (gC/m2/ ) */
	double daily_aut_resp_tC, monthly_aut_resp_tC, annual_aut_resp_tC;    /* daily, monthly and annual aut resp at cell level (tC/cell/ ) */
	double daily_maint_resp, monthly_maint_resp, annual_maint_resp;       /* daily, monthly and annual maint resp at cell level (gC/m2/ ) */
	double daily_growth_resp, monthly_growth_resp, annual_growth_resp;    /* daily, monthly and annual growth resp at cell level (gC/m2/ ) */
	double daily_r_eco, monthly_r_eco, annual_r_eco;                      /* daily, monthly and annual ecosystem resp at cell level (gC/m2/ ) */
	double daily_het_resp, monthly_het_resp, annual_het_resp;             /* daily, monthly and annual heterotrophic resp at cell level (gC/m2/ ) */
	double daily_C_flux, monthly_C_flux, annual_C_flux;                   /* daily, monthly and annual carbon fluxes at cell level (gC/m2/ ) */
	double daily_nee, monthly_nee, annual_nee;                            /* daily, monthly and annual NEE at cell level (gC/m2/ ) */
	double daily_leaf_carbon;                                             /* daily carbon assimilated to c pool at cell level (gC/m2/day) */
	double daily_stem_carbon;                                             /* daily carbon assimilated to c pool at cell level (gC/m2/day)  */
	double daily_froot_carbon;                                            /* daily carbon assimilated to c pool at cell level (gC/m2/day) */
	double daily_croot_carbon;                                            /* daily carbon assimilated to c pool at cell level (gC/m2/day) */
	double daily_root_carbon;                                             /* daily carbon assimilated to c pool at cell level (gC/m2/day) */
	double daily_branch_carbon;                                           /* daily carbon assimilated to c pool at cell level (gC/m2/day) */
	double daily_reserve_carbon;                                          /* daily carbon assimilated to c pool at cell level (gC/m2/day) */
	double daily_litr_carbon;                                             /* daily carbon to litter c pool at cell level (gC/m2/day) */
	double daily_soil_carbon;                                             /* daily carbon to soil c pool at cell level (gC/m2/day) */
	double daily_fruit_carbon;                                            /* daily carbon assimilated to fruit c pool at cell level (gC/m2/day) */
	double daily_leaf_carbon_tC;                                          /* daily carbon assimilated to c pool at cell level (tC/cell/day) */
	double daily_stem_carbon_tC;                                          /* daily carbon assimilated to c pool at cell level (tC/cell/day) */
	double daily_froot_carbon_tC;                                         /* daily carbon assimilated to c pool at cell level (tC/cell/day) */
	double daily_croot_carbon_tC;                                         /* daily carbon assimilated to c pool at cell level (tC/cell/day) */
	double daily_branch_carbon_tC;                                        /* daily carbon assimilated to c pool at cell level (tC/cell/day) */
	double daily_reserve_carbon_tC;                                       /* daily carbon assimilated to c pool at cell level (tC/cell/day) */
	double daily_root_carbon_tC;                                          /* daily carbon assimilated to c pool at cell level (tC/cell/day) */
	double daily_litter_carbon_tC;                                        /* daily carbon leaves to litter c pool at cell level (tC/cell/day) */
	double daily_soil_carbon_tC;                                          /* daily carbon fine root to soil c pool at cell level (tC/cell/day) */
	double daily_fruit_carbon_tC;                                         /* daily carbon assimilated to c pool at cell level (tC/cell/day) */
	double daily_leaf_maint_resp;                                         /* daily leaf maint resp at cell level (gC/m2/day) */
	double daily_stem_maint_resp;                                         /* daily stem maint resp at cell level (gC/m2/day) */
	double daily_froot_maint_resp;                                        /* daily fine root maint resp at cell level (gC/m2/day) */
	double daily_branch_maint_resp;                                       /* daily branch and bark maint resp at cell level (gC/m2/day) */
	double daily_croot_maint_resp;                                        /* daily coarse root maint resp at cell level (gC/m2/day) */
	double daily_leaf_growth_resp;                                        /* daily leaf growth resp at cell level (gC/m2/day) */
	double daily_stem_growth_resp;                                        /* daily stem growth resp at cell level (gC/m2/day) */
	double daily_froot_growth_resp;                                       /* daily fine root growth resp at cell level (gC/m2/day) */
	double daily_branch_growth_resp;                                      /* daily branch and bark growth resp at cell level (gC/m2/day) */
	double daily_croot_growth_resp;                                       /* daily coarse root growth resp at cell level (gC/m2/day) */
	double daily_leaf_aut_resp;                                           /* daily leaf aut resp at cell level (gC/m2/day) */
	double daily_stem_aut_resp;                                           /* daily stem aut resp at cell level (gC/m2/day) */
	double daily_branch_aut_resp;                                         /* daily branch and bark aut resp at cell level (gC/m2/day) */
	double daily_froot_aut_resp;                                          /* daily fine root aut resp at cell level (gC/m2/day) */
	double daily_croot_aut_resp;                                          /* daily coarse root aut resp at cell level (gC/m2/day) */

	/* tree carbon pools */
	double leaf_carbon;                                                   /* leaf carbon at cell level (gC/m2) */
	double froot_carbon;                                                  /* fine root carbon at cell level (gC/m2) */
	double stem_carbon;                                                   /* stem carbon at cell level (gC/m2) */
	double stem_live_wood_carbon;                                         /* stem live wood carbon at cell level (gC/m2) */
	double stem_dead_wood_carbon;                                         /* stem dead wood carbon at cell level (gC/m2) */
	double croot_carbon;                                                  /* coarse root carbon at cell level (gC/m2) */
	double croot_live_wood_carbon;                                        /* coarse root live wood carbon at cell level (gC/m2) */
	double croot_dead_wood_carbon;                                        /* coarse root dead wood carbon at cell level (gC/m2) */
	double branch_carbon;                                                 /* branch carbon at cell level (gC/m2) */
	double branch_live_wood_carbon;                                       /* branch live wood carbon at cell level (gC/m2) */
	double branch_dead_wood_carbon;                                       /* branch dead wood carbon at cell level (gC/m2) */
	double reserve;                                                       /* reserve at cell level (gC/m2) */
	double leaf_tC;                                                       /* leaf carbon at cell level (tC/cell) */
	double froot_tC;                                                      /* fine root carbon at cell level (tC/cell) */
	double stem_tC;                                                       /* stem carbon at cell level (tC/cell) */
	double stem_live_wood_tC;                                             /* stem live wood carbon at cell level (tC/cell) */
	double stem_dead_wood_tC;                                             /* stem dead wood carbon at cell level (tC/cell) */
	double croot_tC;                                                      /* coarse root carbon at cell level (tC/cell) */
	double croot_live_wood_tC;                                            /* coarse root live wood carbon at cell level (tC/cell) */
	double croot_dead_wood_tC;                                            /* coarse root dead wood carbon at cell level (tC/cell) */
	double branch_tC;                                                     /* branch carbon at cell level (tC/cell) */
	double branch_live_wood_tC;                                           /* branch live wood carbon at cell level (tC/cell) */
	double branch_dead_wood_tC;                                           /* branch dead wood carbon at cell level (tC/cell) */
	double reserve_tC;                                                    /* reserve at cell level (tC/cell) */

	/* water use efficiency */
	double daily_wue;                                                     /* daily water use efficiency */
	double daily_iwue;                                                    /* daily intrinsic water use efficiency */
	double monthly_wue;                                                   /* monthly water use efficiency */
	double monthly_iwue;                                                  /* monthly intrinsic water use efficiency */
	double annual_wue;                                                    /* annual water use efficiency */
	double annual_iwue;                                                   /* annual intrinsic water use efficiency */

	/* soil */
	//ALESSIOR TO MOVE INTO SOIL LAYER STRUCTURE
	/* soil water */
	double bulk_density;                                                  /* soil bulk density (g/cm3) */
	double wilting_point;                                                 /* volumetric water content at wilting point (mm/m2) */
	double field_capacity;                                                /* volumetric water content at field capacity (mm/m2) */
	double sat_hydr_conduct;                                              /* saturated hydraulic conductivity (mm/m2) */
	double asw;                                                           /* current available soil water (mm/volume) */
	double old_asw;                                                       /* old available soil water (mm/volume) */
	double max_asw_fc;                                                    /* max available soil water at field capacity mmKgH2O/m3*/
	double max_asw_sat;                                                   /* max available soil water at field capacity mmKgH2O/m3*/
	double psi;                                                           /* water potential of soil and leaves (MPa) */
	double soil_moist_ratio;                                              /* soil moisture ratio */
	double vwc;                                                           /* soil volumetric water content (mm/m2) */
	double vwc_fc;                                                        /* soil volumetric water content at field capacity (mm/m2) */
	double vwc_sat;                                                       /* soil volumetric water content at saturation (mm/m2) */
	double psi_sat;                                                       /* soil matric potential (MPa) */
	double soil_b;                                                        /* slope of log(psi) vs log(rwc) (DIM) */
	double soilw_fc;                                                      /* soil water at field capacity (kgH2O/m2) */
	double soilw_sat;                                                     /* soil water at saturation (kgH2O/m2) */
	double swc;                                                           /* soil Water content (kgH2O/m2) */

	/* water */
	double rain;
	double snow;
	int days_since_rain;                                                             /* consecutive days without rain */
	double rain_intercepted;                                                         /* current amount of intercepted rain (mm/m2) */
	double vapour_to_atmosphere;                                                     /* current water flux water to atmosphere (mm/m2) */
	double snow_pack;                                                                /* current amount of snow (Kg/m2) */
	double snow_melt;                                                                /* current amount of melted snow (mm/m2) */
	double snow_subl;                                                                /* current amount of sublimated snow (mm/m2) */
	double old_snow_pack;                                                            /* old amount of snow (Kg/m2) */
	double out_flow;                                                                 /* current amount of water outflow (mm/m2) */
	double daily_c_rain_int, monthly_c_rain_int, annual_c_rain_int;                  /* daily, monthly and canopy rain interception at cell level (mm/m2/ ) */
	double daily_c_snow_int, monthly_c_snow_int, annual_c_snow_int;                  /* daily, monthly and canopy snow interception at cell level (mm/m2/ ) */
	double daily_c_transp, monthly_c_transp, annual_c_transp;                        /* daily, monthly and canopy transpiration at cell level (mm/m2/ ) */
	double daily_c_evapo, monthly_c_evapo, annual_c_evapo;                           /* daily, monthly and canopy evaporation at cell level (mm/m2/ ) */
	double daily_c_water_stored, monthly_c_water_stored, annual_c_water_stored;      /* daily, monthly and canopy water stored at cell level (mm/m2/ ) */
	double daily_c_evapotransp, monthly_c_evapotransp, annual_c_evapotransp;         /* daily, monthly and canopy evapotranspiration at cell level (mm/m2/ ) */
	double daily_soil_evapo, monthly_soil_evapo, annual_soil_evapo;                  /* daily, monthly and soil evaporation at cell level (mm/m2/ ) */
	double daily_et, monthly_et, annual_et;                                          /* daily, monthly and evapotranspiration at cell level (mm/m2/ ) */
	double daily_tot_w_flux, monthly_tot_w_flux, annual_tot_w_flux;                  /* daily, monthly and annual water fluxes at cell level (mm/m2/ ) */
	double old_daily_c_water_stored;
	double daily_soil_evaporation_watt;                                              /* current daily soil evaporation in watt at cell level (W/m2) */
	double daily_soil_latent_heat_flux;                                              /* current daily soil latent heat flux  at cell level (W/m2) */
	double daily_soil_sensible_heat_flux;                                            /* current daily soil sensible heat flux at cell level (W/m2) */

	/* energy balance */
	double daily_c_transp_watt;                                                      /* daily canopy transpiration at cell level (W/m2) */
	double daily_c_evapo_watt;                                                       /* daily canopy evaporation at cell level (W/m2) */
	double daily_c_evapotransp_watt;                                                 /* daily canopy evapotranspiration at cell level (W/m2) */
	double daily_c_latent_heat_flux;                                                 /* daily canopy latent heat flux at cell level  (W/m2) */
	double daily_latent_heat_flux, monthly_latent_heat_flux, annual_latent_heat_flux;/* daily, monthly and annual latent heat flux at cell level (W/m2) */
	double daily_c_sensible_heat_flux;                                               /* current daily canopy sensible heat flux (W/m2) */
	double daily_sensible_heat_flux, monthly_sensible_heat_flux, annual_sensible_heat_flux;/* daily, monthly and annual sensible heat flux at cell level (W/m2) */

	/* soil scalars */
	double tsoil_scalar;                                                  /* soil temperature scalar for decomposition */
	double wsoil_scalar;                                                  /* soil water scalar  for decomposition */
	double rate_scalar;                                                   /* soil (temperature * water) scalar for decomposition */
	double daily_gross_nmin;                                              /* (tN/sizecell/d) daily gross N mineralization */
	double daily_gross_nimmob;                                            /* (tNN/sizecell/d) daily gross N immobilization */
	double daily_net_nmin;                                                /* (tN/sizecell/d) daily net N mineralization */
	double fpi;                                                           /* (DIM) fraction of potential immobilization */

	/* daily leaf and fine root litter transfer pools */
	//FIXME ADD COARSE WOODY DEBRIS
	/* carbon */
	double daily_litrC;                                                   /* daily leaf + fine root to litter carbon pool at cell level (tC/cell/day) */
	double daily_leaf_litrC;                                              /* daily leaf to litter carbon pool at cell level (tC/cell/day) */
	double daily_leaf_litr1C;                                             /* daily leaf to litter labile carbon pool at cell level (tC/cell/day) */
	double daily_leaf_litr2C;                                             /* daily leaf to litter unshelded cellulose carbon pool at cell level (tC/cell/day) */
	double daily_leaf_litr3C;                                             /* daily leaf to litter  shelded cellulose carbon pool at cell level (tC/cell/day) */
	double daily_leaf_litr4C;                                             /* daily leaf to litter lignin carbon pool at cell level (tC/cell/day) */
	double daily_froot_litrC;                                             /* daily fine root to litter carbon pool at cell level (tC/cell/day) */
	double daily_froot_litr1C;                                            /* daily fine root to litter labile carbon pool at cell level (tC/cell/day) */
	double daily_froot_litr2C;                                            /* daily fine root to litter unshelded cellulose carbon pool at cell level (tC/cell/day) */
	double daily_froot_litr3C;                                            /* daily fine root to litter shelded cellulose carbon pool at cell level (tC/cell/day) */
	double daily_froot_litr4C;                                            /* daily fine root to litter lignin carbon pool at cell level (tC/cell/day) */
	/* nitrogen */
	double daily_litrN;                                                   /* daily leaf + fine root to litter nitrogen pool at cell level (tN/cell/day) */
	double daily_leaf_litrN;                                              /* daily leaf to litter nitrogen pool at cell level (tN/cell/day) */
	double daily_leaf_litr1N;                                             /* daily leaf to litter labile nitrogen pool at cell level (tN/cell/day) */
	double daily_leaf_litr2N;                                             /* daily leaf to litter unshelded cellulose nitrogen pool at cell level (tN/cell/day) */
	double daily_leaf_litr3N;                                             /* daily leaf to litter  shelded cellulose nitrogen pool at cell level (tN/cell/day) */
	double daily_leaf_litr4N;                                             /* daily leaf to litter lignin nitrogen pool at cell level (tN/cell/day) */
	double daily_froot_litrN;                                             /* daily fine root to litter nitrogen pool at cell level (tN/cell/day) */
	double daily_froot_litr1N;                                            /* daily fine root to litter labile nitrogen pool at cell level (tN/cell/day) */
	double daily_froot_litr2N;                                            /* daily fine root to litter unshelded cellulose nitrogen pool at cell level (tN/cell/day) */
	double daily_froot_litr3N;                                            /* daily fine root to litter shelded cellulose nitrogen pool at cell level (tN/cell/day) */
	double daily_froot_litr4N;                                            /* daily fine root to litter lignin nitrogen pool at cell level (tN/cell/day) */

	/* litter and soil carbon pools */
	double leaf_litrC;                                                    /* (tC/sizecell) leaf total litter carbon */
	double leaf_litr1C;                                                   /* (tC/sizecell) leaf litter labile carbon */
	double leaf_litr2C;                                                   /* (tC/sizecell) leaf litter unshielded cellulose carbon */
	double leaf_litr3C;                                                   /* (tC/sizecell) leaf litter shielded cellulose carbon */
	double leaf_litr4C;                                                   /* (tC/sizecell) leaf litter lignin carbon */
	double froot_litrC;                                                   /* (tC/sizecell) fine root total litter carbon */
	double froot_litr1C;                                                  /* (tC/sizecell) fine root litter labile carbon */
	double froot_litr2C;                                                  /* (tC/sizecell) fine root litter unshielded cellulose carbon */
	double froot_litr3C;                                                  /* (tC/sizecell) fine root litter shielded cellulose carbon */
	double froot_litr4C;                                                  /* (tC/sizecell) fine root litter lignin carbon */
	double deadwood_litr1C;                                               /* (tC/sizecell) deadwood litter labile carbon */
	double deadwood_litr2C;                                               /* (tC/sizecell) deadwood litter unshielded cellulose carbon */
	double deadwood_litr3C;                                               /* (tC/sizecell) deadwood litter shielded cellulose carbon */
	double deadwood_litr4C;                                               /* (tC/sizecell) deadwood litter lignin carbon */

	double cwdC;                                                          /* (tC/sizecell) coarse woody debris carbon */
	double litrC;                                                         /* (tC/sizecell) litter + cwd total carbon */
	double litr1C;                                                        /* (tC/sizecell) litter labile carbon */
	double litr2C;                                                        /* (tC/sizecell) litter unshielded cellulose carbon */
	double litr3C;                                                        /* (tC/sizecell) litter shielded cellulose carbon */
	double litr4C;                                                        /* (tC/sizecell) litter lignin carbon */
	double soilC;                                                         /* (tC/sizecell) microbial recycling pool carbon (total) */
	double soil1C;                                                        /* (tC/sizecell) microbial recycling pool carbon (fast) */
	double soil2C;                                                        /* (tC/sizecell) microbial recycling pool carbon (medium) */
	double soil3C;                                                        /* (tC/sizecell) microbial recycling pool carbon (slow) */
	double soil4C;                                                        /* (tC/sizecell) recalcitrant SOM carbon (humus, slowest) */

	/* litter and soil nitrogen pools*/
	double leaf_litrN;                                                    /* (tN/sizecell) leaf total litter nitrogen */
	double leaf_litr1N;                                                   /* (tN/sizecell) leaf litter labile nitrogen */
	double leaf_litr2N;                                                   /* (tN/sizecell) leaf litter unshielded cellulose nitrogen */
	double leaf_litr3N;                                                   /* (tN/sizecell) leaf litter shielded cellulose nitrogen */
	double leaf_litr4N;                                                   /* (tN/sizecell) leaf litter lignin nitrogen */
	double froot_litrN;                                                   /* (tN/sizecell) fine root total litter nitrogen */
	double froot_litr1N;                                                  /* (tN/sizecell) fine root litter labile nitrogen */
	double froot_litr2N;                                                  /* (tN/sizecell) fine root litter unshielded cellulose nitrogen */
	double froot_litr3N;                                                  /* (tN/sizecell) fine root litter shielded cellulonitrogenN */
	double froot_litr4N;                                                  /* (tN/sizecell) fine root litter lignin nitrogen */
	double deadwood_litr1N;                                               /* (tN/sizecell) deadwood litter labile nitrogen */
	double deadwood_litr2N;                                               /* (tN/sizecell) deadwood litter unshielded cellulose nitrogen */
	double deadwood_litr3N;                                               /* (tN/sizecell) deadwood litter shielded cellulose nitrogen */
	double deadwood_litr4N;                                               /* (tN/sizecell) deadwood litter lignin nitrogen */

	double cwdN;                                                          /* (tN/sizecell) coarse woody debris nitrogen */
	double litrN;                                                         /* (tN/sizecell) litter litter + cwd total nitrogen */
	double litr1N;                                                        /* (tN/sizecell) litter labile nitrogen */
	double litr2N;                                                        /* (tN/sizecell) litter unshielded cellulose nitrogen */
	double litr3N;                                                        /* (tN/sizecell) litter shielded cellulose nitrogen */
	double litr4N;                                                        /* (tN/sizecell) litter lignin nitrogen */
	double soilN;                                                         /* (tN/sizecell) microbial recycling pool nitrogen (total) */
	double soil1N;                                                        /* (tN/sizecell) microbial recycling pool nitrogen (fast) */
	double soil2N;                                                        /* (tN/sizecell) microbial recycling pool nitrogen (medium) */
	double soil3N;                                                        /* (tN/sizecell) microbial recycling pool nitrogen (slow) */
	double soil4N;                                                        /* (tN/sizecell) recalcitrant SOM nitrogen (humus, slowest) */
	double sminN;                                                         /* (tN/sizecell) soil mineral nitrogen */
	double retransN;                                                      /* (tN/sizecell) plant pool of retranslocated nitrogen */
	double Npool;                                                         /* (tN/sizecell) temporary plant nitrogen pool */
	double Nfix_src;                                                      /* (tN/sizecell) SUM of biological nitrogen fixation */
	double Ndep_src;                                                      /* (tN/sizecell) SUM of nitrogen deposition inputs */
	double Nleached_snk;                                                  /* (tN/sizecell) SUM of nitrogen leached */

	/* temporary nitrogen variables for reconciliation of decomposition
	immobilization fluxes and plant growth N demands */
	double mineralized;
	double potential_immob;
	double plitr1c_loss;                                                  /* litter carbon labile decomposition */
	double pmnf_l1s1;
	double plitr2c_loss;                                                  /* litter carbon cellulose decomposition */
	double pmnf_l2s2;
	double plitr4c_loss;                                                  /* litter carbon ligning decomposition */
	double pmnf_l4s3;
	double psoil1c_loss;                                                  /* soil carbon labile decomposition */
	double pmnf_s1s2;
	double psoil2c_loss;                                                  /* soil carbon unshielded cellulose decomposition */
	double pmnf_s2s3;
	double psoil3c_loss;                                                  /* soil carbon shielded cellulose decomposition */
	double pmnf_s3s4;
	double psoil4c_loss;                                                  /* soil carbon lignin decomposition */
	double kl4;                                                           /* decay rate of lignin litter */

	/* heterotrophic soil */
	double litr1_rh;                                                      /* (kgC/m2/d) heterotrophic respiration for litter labile carbon */
	double litr2_rh;                                                      /* (kgC/m2/d) heterotrophic respiration for litter unshielded cellulose carbon */
	double litr3_rh;                                                      /* (kgC/m2/d) heterotrophic respiration for litter shielded cellulose carbon */
	double litr4_rh;                                                      /* (kgC/m2/d) heterotrophic respiration for litter lignin carbon */
	double soil1_rh;                                                      /* (kgC/m2/d) heterotrophic respiration for microbial recycling pool carbon (fast) */
	double soil2_rh;                                                      /* (kgC/m2/d) heterotrophic respiration for microbial recycling pool carbon (medium) */
	double soil3_rh;                                                      /* (kgC/m2/d) heterotrophic respiration for microbial recycling pool carbon (slow) */
	double soil4_rh;                                                      /* (kgC/m2/d) heterotrophic respiration for recalcitrant SOM carbon (humus, slowest) */

	/* soil fraction */
	double leaf_litt_scel_frac;                                           /* (dim) leaf litter shielded cellulose fraction */
	double leaf_litt_uscel_frac;                                          /* (dim) leaf litter unshielded cellulose fraction */
	double froot_litt_scel_frac;                                          /* (dim) fine root litter shielded cellulose fraction */
	double froot_litt_uscel_frac;                                         /* (dim) fine root litter unshielded fraction */
	double dead_wood_scel_frac;                                           /* (dim) dead wood litter shielded cellulose fraction */
	double dead_wood_uscel_frac;                                          /* (dim) dead wood litter unshielded fraction */

	double elev;

} cell_t;

typedef struct {
	cell_t *cells;
	int cells_count;
	int cells_avail;
	int x_cells_count;
	int y_cells_count;
} matrix_t;

matrix_t* matrix_create(const soil_settings_t*const s, const int count, const char* const filename);
void matrix_free(matrix_t *m);
void simulation_summary(const matrix_t* const m);
void site_summary(const matrix_t* const m);
void topo_summary(const matrix_t* const m);
void soil_summary(const matrix_t* const m, const cell_t* const cell);
void forest_summary(const matrix_t* const m, const int day, const int month, const int year);
int layer_add(cell_t* const c);
int fill_species_from_file(species_t *const s);
#endif /* MATRIX_H_ */
