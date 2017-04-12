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
	F_A_SUN,                            /* ASSIMILATION modifier for Sun leaves (test) */
	F_A_SHADE,                          /* ASSIMILATION modifier for Shaded leaves (test) */
	PHYS_MOD,                           /* PHYSIOLOGICAL modifier */

	/* water */
	/* pools */
	CANOPY_WATER,                       /* canopy water pool */
	CANOPY_SNOW,                        /* canopy snow pool  */
	/* fluxes */
	STOMATAL_CONDUCTANCE,               /* stomatal conductance (m/sec) */
	STOMATAL_SUN_CONDUCTANCE,           /* stomatal conductance for sun leaves (m/sec) */
	STOMATAL_SHADE_CONDUCTANCE,         /* stomatal conductance for shade leaves (m/sec) */
	LEAF_CONDUCTANCE,                   /* leaf conductance (m/sec) */
	LEAF_SUN_CONDUCTANCE,               /* leaf conductance for sun leaves (m/sec) */
	LEAF_SHADE_CONDUCTANCE,             /* leaf conductance for shade leaves (m/sec) */
	CANOPY_BLCOND,                      /* Canopy Boundary Layer conductance (m/sec) */
	FRAC_DAYTIME_TRANSP,                /* fraction of daytime for transpiration (and photosynthesis) */
	CANOPY_INT,                         /* canopy interception of rainfall (mm/day) */
	CANOPY_INT_SNOW,                    /* canopy interception of snow (mm/day) */
	CANOPY_EVAPO,                       /* canopy evaporation (mm/day) */
	CANOPY_WET,                         /* it should'nt be reset every day */
	CANOPY_TRANSP,                      /* canopy transpiration (mm/day) */
	CANOPY_TRANSP_SUN,                  /* canopy transpiration (mm/day) for sun leaves */
	CANOPY_TRANSP_SHADE,                /* canopy transpiration (mm/day) for shade leaves */
	CANOPY_EVAPO_TRANSP,                /* canopy evapotranspiration (mm/day) */
	CANOPY_FRAC_DAY_TRANSP,             /* Fraction of daily canopy transpiration (DIM) */
	CANOPY_LATENT_HEAT,                 /* canopy latent heat (W/m2) */
	CANOPY_SENSIBLE_HEAT,               /* canopy sensible heat (W/m2) */

	MONTHLY_CANOPY_TRANSP,              /* monthly canopy transpiration (mm/month) */
	MONTHLY_CANOPY_TRANSP_SUN,          /* monthly canopy transpiration (mm/month) for sun leaves */
	MONTHLY_CANOPY_TRANSP_SHADE,        /* monthly canopy transpiration (mm/month) for shade leaves */
	MONTHLY_CANOPY_INT,                 /* monthly canopy interception (mm/month) */
	MONTHLY_CANOPY_EVAPO_TRANSP,        /* monthly canopy evapotranspiration (mm/month) */
	YEARLY_CANOPY_TRANSP,               /* annual canopy transpiration (mm/year) */
	YEARLY_CANOPY_TRANSP_SUN,           /* annual canopy transpiration (mm/year) for sun leaves */
	YEARLY_CANOPY_TRANSP_SHADE,         /* annual canopy transpiration (mm/year) for shade leaves */
	YEARLY_CANOPY_INT,                  /* annual canopy interception (mm/year) */
	YEARLY_CANOPY_EVAPO_TRANSP,         /* annual canopy evapotranspiration (mm/year) */
	MONTHLY_CANOPY_LATENT_HEAT,         /* monthly canopy latent heat (W/m2) */
	YEARLY_CANOPY_LATENT_HEAT,          /* annual canopy latent heat (W/m2) */

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
	iWUE_SUN,                           /* daily intrinsic Water Use Efficiency for sun leaves */
	MONTHLY_iWUE_SUN,                   /* monthly intrinsic Water Use Efficiency for sun leaves */
	YEARLY_iWUE_SUN,                    /* annual intrinsic Water Use Efficiency for sun leaves */
	iWUE_SHADE,                         /* daily intrinsic Water Use Efficiency for shade leaves */
	MONTHLY_iWUE_SHADE,                 /* monthly intrinsic Water Use Efficiency for shade leaves */
	YEARLY_iWUE_SHADE,                  /* annual intrinsic Water Use Efficiency for shade leaves */
	gsWUE,                              /* daily intrinsic Water Use Efficiency (stomatal) */
	gsWUE_SUN,                          /* daily intrinsic Water Use Efficiency (stomatal) for sun leaves */
	gsWUE_SHADE,                        /* daily intrinsic Water Use Efficiency (stomatal) for shade leaves */

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
	GPP_SUN,                            /* Daily Sun Leaves Gross Primary Production gC/m2/day */
	GPP_SHADE,                          /* Daily Shade Leaves Gross Primary Production gC/m2/day */
	MONTHLY_GPP,                        /* Monthly Gross Primary Production gC/m2/month */
	MONTHLY_GPP_SUN,                    /* Monthly Gross Primary Production gC/m2/month for sun leaves */
	MONTHLY_GPP_SHADE,                  /* Monthly Gross Primary Production gC/m2/month for shaded leaves */
	YEARLY_GPP,                         /* Yearly Gross Primary Production gC/m2/year */
	YEARLY_GPP_SUN,                     /* Yearly Gross Primary Production gC/m2/year for sun leaves */
	YEARLY_GPP_SHADE,                   /* Yearly Gross Primary Production gC/m2/year for shaded leaves */
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
	LEAF_C_TO_REMOVE,                   /* Daily Leaf to remove tC/sizeCell */
	FROOT_C_TO_REMOVE,                  /* Daily Fine Root to remove tC/sizeCell */
	FRUIT_C_TO_REMOVE,                  /* Daily Fruit to remove tC/sizeCell */
	BRANCH_C_TO_REMOVE,                 /* Annual Branch to remove tC/sizeCell */
	C_LEAF_TO_RESERVE,                  /* Daily retranslocated C from Leaf pool to Reserve (tC/sizeCell day) */
	C_FROOT_TO_RESERVE,                 /* Daily retranslocated C from Fine root pool to Reserve (tC/sizeCell day) */
	C_LEAF_TO_LITR,                     /* Daily tC from Leaf pool to Litter (tC/sizeCell day) */
	C_FROOT_TO_LITR,                    /* Daily tC from Fine root pool to Soil (tC/sizeCell day) */
	C_FRUIT_TO_LITR,                    /* Daily tC from Fruit pool to Litter (tC/sizeCell day) */
	C_BRANCH_TO_RESERVE,                /* Annual tC from Branch pool to Litter (tC/sizeCell day) */
	C_BRANCH_TO_CWD,                    /* Annual tC from Branch pool to Coarse Woody Debris (tC/sizeCell day) */
	C_TO_LITR,                          /* Daily Leaf and Fine root Carbon to Litter pool (tC/sizeCell day) */
	C_TO_CWD,                           /* Daily Wood Carbon to Coarse Woody Debirs (tC/sizeCell day) */
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
	C_CWDC_TO_LITR2C,                   /* Daily Coarse Woody Debris Carbon to Litter unshielded cellulose carbon pool (tC/sizeCell day) */
	C_CWDC_TO_LITR3C,                   /* Daily Coarse Woody Debris Carbon to Litter shielded cellulose carbon pool (tC/sizeCell day) */
	C_CWDC_TO_LITR4C,                   /* Daily Coarse Woody Debris Carbon to Litter lignin carbon pool (tC/sizeCell day) */
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
	CWD_C,                              /* Current Coarse Woody Debris carbon pool tC/sizeCell */
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
	TOT_HEARTWOOD_C,                    /* Current total heartwood carbon pool tC/sizeCell */
	EFF_LIVE_TOTAL_WOOD_FRAC,           /* Age-related fraction of Live biomass per Total biomass */
	DAILY_LIVE_WOOD_TURNOVER,           /* Daily live wood turnover rate */


	/* per tree in tC */
	AV_LEAF_MASS_C,                     /* Average Leaf carbon pool tC/tree */
	AV_STEM_MASS_C,                     /* Average Stem carbon pool tC/tree */
	AV_TOT_STEM_MASS_C,                 /* Average Stem + Branch carbon pool tC/tree */
	AV_ROOT_MASS_C,                     /* Average Total root carbon pool tC/tree */
	AV_FROOT_MASS_C,                    /* Average Fine root carbon pool tC/tree */
	AV_CROOT_MASS_C,                    /* Average Coarse carbon pool tC/tree */
	AV_RESERVE_MASS_C,                  /* Average Reserve carbon pool tC/tree */
	AV_MIN_RESERVE_C,                   /* Average Minimum Reserve carbon pool tC/tree */
	AV_FRUIT_MASS_C,                    /* Average Fruit carbon pool tC/tree */
	AV_BRANCH_MASS_C,                   /* Average Branch carbon pool tC/tree */
	AV_STEM_SAPWOOD_MASS_C,             /* Average Sapwood Stem carbon pool tC/tree */
	AV_STEM_HEARTWOOD_MASS_C,           /* Average Heartwood Stem carbon pool tC/tree */
	AV_LIVE_STEM_MASS_C,                /* Average Live Stem carbon pool tC/tree */
	AV_DEAD_STEM_MASS_C,                /* Average Dead Stem carbon pool tC/tree */
	AV_CROOT_SAPWOOD_MASS_C,            /* Average Sapwood Coarse root carbon pool tC/tree */
	AV_CROOT_HEARTWOOD_MASS_C,          /* Average Heartwood Coarse root carbon pool tC/tree */
	AV_LIVE_CROOT_MASS_C,               /* Average Live Coarse root carbon pool tC/tree */
	AV_DEAD_CROOT_MASS_C,               /* Average Dead Coarse root carbon pool tC/tree */
	AV_BRANCH_SAPWOOD_MASS_C,           /* Average Sapwood Branch carbon pool tC/tree */
	AV_BRANCH_HEARTWOOD_MASS_C,         /* Average Heartwood Branch carbon pool tC/tree */
	AV_LIVE_BRANCH_MASS_C,              /* Average Live Branch carbon pool tC/tree */
	AV_DEAD_BRANCH_MASS_C,              /* Average Dead Stem carbon pool tC/tree */
	AV_LIVE_WOOD_MASS_C,                /* Average Live Wood carbon pool tC/tree */
	AV_DEAD_WOOD_MASS_C,                /* Average Dead Wood carbon pool tC/tree */
	AV_TOT_WOOD_MASS_C,                 /* Average Total Wood carbon pool tC/tree */

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
	AV_LIVE_CROOT_MASS_KgDM,            /* Average Live Coarse root carbon pool kgDM/tree */
	AV_DEAD_CROOT_MASS_KgDM,            /* Average Dead Coarse root carbon pool kgDM/tree */
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
	FRUIT_GROWTH_RESP,                  /* Fruit growth respiration (gC/m2/day) */
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
	FRUIT_AUT_RESP,                     /* Fruit autotrophic respiration (gC/m2/day) */
	TOTAL_AUT_RESP,                     /* Total autotrophic respiration (gC/m2/day) */
	MONTHLY_LEAF_AUT_RESP,              /* Monthly Leaf autotrophic respiration (gC/m2/month) */
	MONTHLY_FROOT_AUT_RESP,             /* Monthly Fine root autotrophic respiration (gC/m2/month) */
	MONTHLY_STEM_AUT_RESP,              /* Monthly Stem autotrophic respiration (gC/m2/month) */
	MONTHLY_CROOT_AUT_RESP,             /* Monthly Coarse root autotrophic respiration (gC/m2/month) */
	MONTHLY_FRUIT_AUT_RESP,             /* Monthly Fruit autotrophic respiration (gC/m2/month) */
	MONTHLY_BRANCH_AUT_RESP,            /* Monthly Branch autotrophic respiration (gC/m2/month) */
	MONTHLY_TOTAL_AUT_RESP,             /* Monthly Total autotrophic respiration (gC/m2/month) */
	YEARLY_LEAF_AUT_RESP,               /* Annual Leaf autotrophic respiration (gC/m2/year) */
	YEARLY_FROOT_AUT_RESP,              /* Annual Fine root autotrophic respiration (gC/m2/year) */
	YEARLY_STEM_AUT_RESP,               /* Annual Stem autotrophic respiration (gC/m2/year) */
	YEARLY_CROOT_AUT_RESP,              /* Annual Coarse root autotrophic respiration (gC/m2/year) */
	YEARLY_BRANCH_AUT_RESP,             /* Annual Branch autotrophic respiration (gC/m2/year) */
	YEARLY_FRUIT_AUT_RESP,              /* Annual fRUIT autotrophic respiration (gC/m2/year) */
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
	LEAF_N_TO_REMOVE,                   /* Daily Leaf to remove tN/sizeCell */
	FROOT_N_TO_REMOVE,                  /* Daily Fine Root to remove tN/sizeCell */
	FRUIT_N_TO_REMOVE,                  /* Daily Fruit to remove tN/sizeCell */
	BRANCH_N_TO_REMOVE,                 /* Annual Branch to remove tN/sizeCell */
	N_TO_CWD,                           /* Daily Woody Nitrogen to Coarse Woody Debris tN/sizeCell */
	N_LEAF_TO_LITR,                     /* Daily Leaf Litter to Litter labile nitrogen pool (tN/sizeNell day) */
	N_FROOT_TO_LITR,                    /* Daily Leaf Litter to Litter labile nitrogen pool (tN/sizeNell day) */
	N_FRUIT_TO_LITR,                    /* Daily Fruitr to Litter labile nitrogen pool (tN/sizeNell day) */
	N_LEAF_TO_LITR1N,                   /* Daily Leaf Litter to Litter labile nitrogen pool (tN/sizeNell day) */
	N_LEAF_TO_LITR2N,                   /* Daily Leaf Litter to Litter unshielded cellulose nitrogen pool (tN/sizeNell day) */
	N_LEAF_TO_LITR3N,                   /* Daily Leaf Litter to Litter shielded cellulose nitrogen pool (tN/sizeNell day) */
	N_LEAF_TO_LITR4N,                   /* Daily Leaf Litter to Litter lignin nitrogen pool (tN/sizeNell day) */
	N_FROOT_TO_LITR1N,                  /* Daily Fine Root Litter to Litter labile nitrogen pool (tN/sizeNell day) */
	N_FROOT_TO_LITR2N,                  /* Daily Fine Root Litter to Litter unshielded cellulose nitrogen pool (tN/sizeNell day) */
	N_FROOT_TO_LITR3N,                  /* Daily Fine Root Litter to Litter shielded cellulose nitrogen pool (tN/sizeNell day) */
	N_FROOT_TO_LITR4N,                  /* Daily Fine Root Litter to Litter lignin nitrogen pool (tN/sizeNell day) */
	N_BRANCH_TO_CWD,                    /* Annual Branch to Coarse Woody Debris nitrogen pool (tN/sizeNell day) */
	N_LEAF_TO_RESERVE,                  /* Daily retranslocated C from Leaf pool to Reserve (tN/sizeCell day) */
	N_FROOT_TO_RESERVE,                 /* Daily retranslocated C from Fine root pool to Reserve (tN/sizeCell day) */
	N_BRANCH_TO_RESERVE,                /* Annual retranslocated C from Branch pool to Reserve (tN/sizeCell day) */
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
	CWD_N,                              /* Current Coarse Woody Debris nitrogen pool tN/sizeCell */
	TOT_STEM_N,                         /* Current Total Stem nitrogen pool tN/sizeCell */
	TOT_ROOT_N,                         /* Current Total root nitrogen pool tN/sizeCell */

	/* per tree in tN */
	AV_LEAF_MASS_N,                    /* Average Leaf nitrogen pool tN/tree */
	AV_STEM_MASS_N,                    /* Average Stem nitrogen pool tN/tree */
	AV_TOT_STEM_MASS_N,                /* Average Stem + Branch nitrogen pool tN/tree */
	AV_ROOT_MASS_N,                    /* Average Total root nitrogen pool tN/tree */
	AV_FROOT_MASS_N,                   /* Average Fine root nitrogen pool tN/tree */
	AV_CROOT_MASS_N,                   /* Average Coarse nitrogen pool tN/tree */
	AV_BRANCH_MASS_N,                  /* Average Branch nitrogen pool tN/tree */
	AV_RESERVE_MASS_N,                 /* Average Reserve nitrogen pool tN/tree */
	AV_FRUIT_MASS_N,                   /* Average Fruit nitrogen pool tN/tree */

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

	/*** BALANCES VALUES ***/
	TREEC_IN,                           /* tree carbon balance in */
	TREEC_OUT,                          /* tree carbon balance out */
	TREEC_STORE,                        /* tree carbon balance store */
	TREEC_OLDSTORE,                     /* tree carbon balance old_store */
	TREEC_BALANCE,                      /* tree carbon balance balance */

	TREEW_IN,                           /* tree water balance in */
	TREEW_OUT,                          /* tree water balance out */
	TREEW_STORE,                        /* tree water balance store */
	TREEW_OLDSTORE,                     /* tree water balance old_store */
	TREEW_BALANCE,                      /* tree water balance balance */

	TREEN_IN,                           /* tree nitrogen balance in */
	TREEN_OUT,                          /* tree nitrogen balance out */
	TREEN_STORE,                        /* tree nitrogen balance store */
	TREEN_OLDSTORE,                     /* tree nitrogen balance old_store */
	TREEN_BALANCE,                      /* tree nitrogen balance balance */

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
	DOS,                                /* day of simulation */

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

	int doy;                                            /* day of the year */
	int dos;                                            /* day of simulation */

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
	double elev;                                        /* cell elevation (m) */
	int north;                                          /* northern hemisphere north = 0, south hemisphere south = 1 */

	/* annual met values */
	double abscission_daylength;                        /* (hrs/day) day length for starting abscission (not used) */
	double annual_tavg;                                 /* (°C) annual avg daily avg temperature */
	double annual_tmin;                                 /* (°C) annual avg daily min temperature */
	double annual_tmax;                                 /* (°C) annual avg daily max temperature */
	double annual_tday;                                 /* (°C) annual avg daily daytime temperature */
	double annual_tnight;                               /* (°C) annual avg daily nighttime temperature */
	double annual_tsoil;                                /* (°C) (°C)  avg daily soil temperature */
	double annual_solar_rad;                            /* (MJ/day/yr) annual avg daily solar radiation (short wave)  */
	double annual_precip;                               /* (mm) annual cumulate daily precipitation */
	double annual_vpd;                                  /* (hPa) annual avg daily vpd */

	/*forest structure variables*/	
	int Veg_Counter;                                    /* number of class in vegetative period  in the cell */
	double cell_cover;                                  /* (ratio) canopy cover  */
	int cell_n_trees;                                   /* (trees/cell/day) number of trees */
	int daily_dead_tree;                                /* (trees/cell/day) daily number of dead tree */
	int monthly_dead_tree;                              /* (trees/cell/month) monthly number of dead tree */
	int annual_dead_tree ;                              /* (trees/cell/year ) annual number of dead tree */
	int saplings_counter;                               /* (trees/cell) number of class as sapling */
	double basal_area;                                  /* (day/cell) cumulated basal area at cell level */
	double agb;                                         /* (tC/cell) current above ground biomass at cell level */
	double bgb;                                         /* (tC/cell) current below ground biomass at cell level */

	/* radiation variables */
	/* short wave radiation */
	double sw_rad_down_MJ;                              /* (MJ/day/day) Downward short wave radiation flux */
	double short_wave_radiation_upward_MJ;              /* (MJ/day/day) Upward short wave radiation flux */
	double short_wave_radiation_upward_W;               /* (W/day) Upward short wave radiation flux */
	double sw_rad_refl;                                 /* (W/day) Short Wave radiation reflected */
	double sw_rad_refl_soil;                            /* (W/day) Short Wave radiation reflected from soil level */
	double sw_rad_refl_snow;                            /* (W/day) (W/day)  Wave radiation reflected from snow level */
	double sw_rad_abs;                                  /* (W/day) Short Wave radiation absorbed tree level */
	double sw_rad_abs_soil;                             /* (W/day) Short Wave radiation absorbed soil level */
	double sw_rad_abs_snow;                             /* (W/day) Short Wave radiation absorbed snow level */
	double sw_rad_transm;                               /* (W/day) Short Wave radiation transmitted */

	/* long wave radiation */
	double long_wave_radiation_upward_MJ;               /* (MJ/day/day) Upward long wave radiation flux */
	double long_wave_radiation_upward_W;                /* (W/day) Upward long wave radiation flux */
	double net_long_wave_radiation_MJ;                  /* (MJ/day/day) Net long wave radiation flux  */
	double net_long_wave_radiation_W;                   /* (W/day) Net long wave radiation flux */
	double long_wave_absorbed;                          /* (W/day) Total absorbed long wave wave radiation flux */
	double long_wave_reflected;                         /* (W/day) Total reflected long wave radiation flux */
	double long_wave_emitted;                           /* (W/day) Total emitted long wave radiation flux */
	double soil_long_wave_emitted;                      /* (W/day) Soil emitted long wave radiation flux */
	double net_lw_rad_for_soil;                         /* (W/day) Net Long Wave radiation to soil level */
	double net_rad_for_soil;                            /* (W/day) Net radiation to soil level */

	/* PAR radiation */
	double apar;                                        /* (molPAR/day/day) cumulated absorbed PAR at tree level */
	double apar_soil;                                   /* (molPAR/day/day) cumulated absorbed PAR at soil level */
	double apar_snow;                                   /* (molPAR/day/day) cumulated absorbed PAR at snow level */
	double par_transm;                                  /* (molPAR/day/day) cumulated transmitted PAR at cell level */
	double par_refl;                                    /* (molPAR/day/day) cumulated reflected PAR at cell level */
	double par_refl_soil;                               /* (molPAR/day/day) reflected PAR by the soil */
	double par_refl_snow;                               /* (molPAR/day/day) reflected PAR by the snow */

	/* PPFD radiation */
	double ppfd_abs;                                    /* (umol/day/sec) Absorbed Photosynthetic photon flux density tree level  */
	double ppfd_abs_soil;                               /* (umol/day/sec) Absorbed Photosynthetic photon flux density soil level */
	double ppfd_abs_snow;                               /* (umol/day/sec) Absorbed Photosynthetic photon flux density snow level */
	double ppfd_transm;                                 /* (umol/day/sec) Transmitted Photosynthetic photon flux density */
	double ppfd_refl;                                   /* (umol/day/sec) Reflected Photosynthetic photon flux density */
	double ppfd_refl_soil;                              /* (umol/day/sec) PPFD reflected by the soil */
	double ppfd_refl_snow;                              /* (umol/day/sec) PPFD reflected by the snow */

	double canopy_temp;                                 /* (°C) daily canopy temperature */
	double canopy_temp_k;                               /* (K) daily canopy temperature */
	double canopy_temp_diff;                            /* (K) daily canopy temperature */

	/*carbon variables*/
	double daily_gpp, monthly_gpp, annual_gpp;                            /* (gC/day/time) daily, monthly and annual GPP at cell level */
	double daily_gpp_tC, monthly_gpp_tC, annual_gpp_tC;                   /* (tC/day/time) daily, monthly and annual GPP at cell level */
	double daily_npp, monthly_npp, annual_npp;                            /* (gC/day/time) daily, monthly and annual NPP at cell level */
	double daily_npp_tC, monthly_npp_tC, annual_npp_tC;                   /* (tC/day/time) daily, monthly and annual NPP at cell level */
	double daily_npp_tDM, monthly_npp_tDM, annual_npp_tDM;                /* (tDM/day/time) daily, monthly and annual NPP at cell level */
	double daily_aut_resp, monthly_aut_resp, annual_aut_resp;             /* (gC/day/time) daily, monthly and annual aut resp at cell level */
	double daily_aut_resp_tC, monthly_aut_resp_tC, annual_aut_resp_tC;    /* (tC/day/time) daily, monthly and annual aut resp at cell level */
	double daily_maint_resp, monthly_maint_resp, annual_maint_resp;       /* (gC/day/time) daily, monthly and annual maint resp at cell level */
	double daily_growth_resp, monthly_growth_resp, annual_growth_resp;    /* (gC/day/time) daily, monthly and annual growth resp at cell level */
	double daily_r_eco, monthly_r_eco, annual_r_eco;                      /* (gC/day/time) daily, monthly and annual ecosystem resp at cell level */
	double daily_het_resp, monthly_het_resp, annual_het_resp;             /* (gC/day/time) daily, monthly and annual heterotrophic resp at cell level */
	double daily_C_flux, monthly_C_flux, annual_C_flux;                   /* (gC/day/time) daily, monthly and annual carbon fluxes at cell level */
	double daily_nee, monthly_nee, annual_nee;                            /* (gC/day/time) daily, monthly and annual NEE at cell level */
	double daily_leaf_carbon;                                             /* (gC/day/day) daily carbon assimilated to c pool at cell level */
	double daily_stem_carbon;                                             /* (gC/day/day) daily carbon assimilated to c pool at cell level */
	double daily_froot_carbon;                                            /* (gC/day/day) daily carbon assimilated to c pool at cell level */
	double daily_croot_carbon;                                            /* (gC/day/day) daily carbon assimilated to c pool at cell level */
	double daily_root_carbon;                                             /* (gC/day/day) daily carbon assimilated to c pool at cell level */
	double daily_branch_carbon;                                           /* (gC/day/day) daily carbon assimilated to c pool at cell level */
	double daily_reserve_carbon;                                          /* (gC/day/day) daily carbon assimilated to c pool at cell level */
	double daily_litr_carbon;                                             /* (gC/day/day) daily carbon to litter c pool at cell level */
	double daily_soil_carbon;                                             /* (gC/day/day) daily carbon to soil c pool at cell level */
	double daily_cwd_carbon;                                              /* (gC/day/day) daily carbon to cwd c pool at cell level */
	double daily_fruit_carbon;                                            /* (gC/day/day) daily carbon assimilated to fruit c pool at cell level */
	double daily_leaf_tC;                                                 /* (tC/cell/day) daily carbon assimilated to c pool at cell level */
	double daily_stem_tC;                                                 /* (tC/cell/day) daily carbon assimilated to c pool at cell level */
	double daily_froot_tC;                                                /* (tC/cell/day) daily carbon assimilated to c pool at cell level */
	double daily_croot_tC;                                                /* (tC/cell/day) daily carbon assimilated to c pool at cell level */
	double daily_branch_tC;                                               /* (tC/cell/day) daily carbon assimilated to c pool at cell level */
	double daily_reserve_tC;                                              /* (tC/cell/day) daily carbon assimilated to c pool at cell level */
	double daily_root_tC;                                                 /* (tC/cell/day) daily carbon assimilated to c pool at cell level */
	double daily_litr_tC;                                                 /* (tC/cell/day) daily carbon leaves to litter c pool at cell level */
	double daily_soil_tC;                                                 /* (tC/cell/day) daily carbon fine root to soil c pool at cell level */
	double daily_fruit_tC;                                                /* (tC/cell/day) daily carbon assimilated to c pool at cell level */
	double daily_leaf_maint_resp;                                         /* (gC/day/day) daily leaf maint resp at cell level */
	double daily_stem_maint_resp;                                         /* (gC/day/day) daily stem maint resp at cell level */
	double daily_froot_maint_resp;                                        /* (gC/day/day) daily fine root maint resp at cell level */
	double daily_branch_maint_resp;                                       /* (gC/day/day) daily branch and bark maint resp at cell level */
	double daily_croot_maint_resp;                                        /* (gC/day/day) daily coarse root maint resp at cell level */
	double daily_leaf_growth_resp;                                        /* (gC/day/day) daily leaf growth resp at cell level */
	double daily_stem_growth_resp;                                        /* (gC/day/day) daily stem growth resp at cell level */
	double daily_froot_growth_resp;                                       /* (gC/day/day) daily fine root growth resp at cell level */
	double daily_branch_growth_resp;                                      /* (gC/day/day) daily branch and bark growth resp at cell level */
	double daily_croot_growth_resp;                                       /* (gC/day/day) daily coarse root growth resp at cell level */
	double daily_fruit_growth_resp;                                       /* (gC/day/day) daily fruit growth resp at cell level */
	double daily_leaf_aut_resp;                                           /* (gC/day/day) daily leaf aut resp at cell level */
	double daily_stem_aut_resp;                                           /* (gC/day/day) daily stem aut resp at cell level */
	double daily_branch_aut_resp;                                         /* (gC/day/day) daily branch and bark aut resp at cell level */
	double daily_froot_aut_resp;                                          /* (gC/day/day) daily fine root aut resp at cell level */
	double daily_croot_aut_resp;                                          /* (gC/day/day) daily coarse root aut resp at cell level */
	double daily_fruit_aut_resp;                                          /* (gC/day/day) daily fruit aut resp at cell level */

	/* tree carbon pools */
	double leaf_carbon;                                                   /* (gC/day) leaf carbon at cell level */
	double froot_carbon;                                                  /* (gC/day) fine root carbon at cell level */
	double stem_carbon;                                                   /* (gC/day) stem carbon at cell level */
	double stem_live_wood_carbon;                                         /* (gC/day) stem live wood carbon at cell level */
	double stem_dead_wood_carbon;                                         /* (gC/day) stem dead wood carbon at cell level */
	double tot_stem_carbon;                                               /* (gC/day) total stem carbon at cell level */
	double croot_carbon;                                                  /* (gC/day) coarse root carbon at cell level */
	double croot_live_wood_carbon;                                        /* (gC/day) coarse root live wood carbon at cell level */
	double croot_dead_wood_carbon;                                        /* (gC/day) coarse root dead wood carbon at cell level */
	double tot_root_carbon;                                               /* (gC/day) total root carbon at cell level */
	double branch_carbon;                                                 /* (gC/day) branch carbon at cell level */
	double branch_live_wood_carbon;                                       /* (gC/day) branch live wood carbon at cell level */
	double branch_dead_wood_carbon;                                       /* (gC/day) branch dead wood carbon at cell level */
	double reserve_carbon;                                                /* (gC/day) reserve at cell level */
	double fruit_carbon;                                                  /* (gC/day) fruit at cell level */
	double litr_carbon;                                                   /* (gC/day) litter at cell level */
	double cwd_carbon;                                                    /* (gC/day) coarse woody debris at cell level */
	double soil_carbon;                                                   /* (gC/day) microbial recycling pool carbon (total) at cell level */
	double leaf_tC;                                                       /* (tC/cell) leaf carbon at cell level */
	double froot_tC;                                                      /* (tC/cell) fine root carbon at cell level */
	double stem_tC;                                                       /* (tC/cell) stem carbon at cell level */
	double stem_live_wood_tC;                                             /* (tC/cell) stem live wood carbon at cell level */
	double stem_dead_wood_tC;                                             /* (tC/cell) stem dead wood carbon at cell level */
	double croot_tC;                                                      /* (tC/cell) coarse root carbon at cell level */
	double croot_live_wood_tC;                                            /* (tC/cell) coarse root live wood carbon at cell level */
	double croot_dead_wood_tC;                                            /* (tC/cell) coarse root dead wood carbon at cell level */
	double branch_tC;                                                     /* (tC/cell) branch carbon at cell level */
	double branch_live_wood_tC;                                           /* (tC/cell) branch live wood carbon at cell level */
	double branch_dead_wood_tC;                                           /* (tC/cell) branch dead wood carbon at cell level */
	double reserve_tC;                                                    /* (tC/cell) reserve at cell level */
	double fruit_tC;                                                      /* (tC/cell) fruit at cell level */
	double litr_tC;                                                       /* (tC/cell) litter at cell level */
	double cwd_tC;                                                        /* (tC/cell) coarse woody debris at cell level */

	/* tree nitrogen pools */
	double leaf_nitrogen;                                                 /* (gN/day) leaf carbon at cell level */
	double froot_nitrogen;                                                /* (gN/day) fine root carbon at cell level */
	double stem_nitrogen;                                                 /* (gN/day) stem carbon at cell level */
	double stem_live_wood_nitrogen;                                       /* (gN/day) stem live wood carbon at cell level */
	double stem_dead_wood_nitrogen;                                       /* (gN/day) stem dead wood carbon at cell level */
	double tot_stem_nitrogen;                                             /* (gN/day) total stem carbon at cell level */
	double croot_nitrogen;                                                /* (gN/day) coarse root carbon at cell level */
	double croot_live_wood_nitrogen;                                      /* (gN/day) coarse root live wood carbon at cell level */
	double croot_dead_wood_nitrogen;                                      /* (gN/day) coarse root dead wood carbon at cell level */
	double tot_root_nitrogen;                                             /* (gN/day) total root carbon at cell level */
	double branch_nitrogen;                                               /* (gN/day) branch carbon at cell level */
	double branch_live_wood_nitrogen;                                     /* (gN/day) branch live wood carbon at cell level */
	double branch_dead_wood_nitrogen;                                     /* (gN/day) branch dead wood carbon at cell level */
	double reserve_nitrogen;                                              /* (gN/day) reserve at cell level */
	double fruit_nitrogen;                                                /* (gN/day) fruit at cell level */
	double litr_nitrogen;                                                 /* (gN/day) litter at cell level */
	double cwd_nitrogen;                                                  /* (gN/day) coarse woody debris at cell level */
	double soil_nitrogen;                                                 /* (gN/day) soil at cell level */
	double leaf_tN;                                                       /* (tN/cell) leaf carbon at cell level */
	double froot_tN;                                                      /* (tN/cell) fine root carbon at cell level */
	double stem_tN;                                                       /* (tN/cell) stem carbon at cell level */
	double stem_live_wood_tN;                                             /* (tN/cell) stem live wood carbon at cell level */
	double stem_dead_wood_tN;                                             /* (tN/cell) stem dead wood carbon at cell level */
	double croot_tN;                                                      /* (tN/cell) coarse root carbon at cell level */
	double croot_live_wood_tN;                                            /* (tN/cell) coarse root live wood carbon at cell level */
	double croot_dead_wood_tN;                                            /* (tN/cell) coarse root dead wood carbon at cell level */
	double branch_tN;                                                     /* (tN/cell) branch carbon at cell level */
	double branch_live_wood_tN;                                           /* (tN/cell) branch live wood carbon at cell level */
	double branch_dead_wood_tN;                                           /* (tN/cell) branch dead wood carbon at cell level */
	double reserve_tN;                                                    /* (tN/cell) reserve at cell level */
	double fruit_tN;                                                      /* (tN/cell) fruit at cell level */
	double litr_tN;                                                       /* (tN/cell) Litter at cell level */
	double cwd_tN;                                                        /* (tN/cell) coarse woody debris at cell level */

	/* water use efficiency */
	double daily_wue;                                                     /* (DIM) daily water use efficiency */
	double daily_iwue;                                                    /* (DIM) daily intrinsic water use efficiency */
	double monthly_wue;                                                   /* (DIM) monthly water use efficiency */
	double monthly_iwue;                                                  /* (DIM) monthly intrinsic water use efficiency */
	double annual_wue;                                                    /* (DIM) annual water use efficiency */
	double annual_iwue;                                                   /* (DIM) annual intrinsic water use efficiency */

	/* soil */
	//ALESSIOR TO MOVE INTO SOIL LAYER STRUCTURE
	/* soil water */
	double bulk_density;                                                  /* (g/cm3) soil bulk density */
	double wilting_point;                                                 /* (mm/day) volumetric water content at wilting point */
	double field_capacity;                                                /* volumetric water content at field capacity */
	double sat_hydr_conduct;                                              /* (mm/day) saturated hydraulic conductivity (mm/day) */
	double psi;                                                           /* (MPa) water potential of soil and leaves */
	double soil_moist_ratio;                                              /* (DIM) soil moisture ratio */
	double vwc;                                                           /* (mm/day) soil volumetric water content */
	double vwc_fc;                                                        /* (mm/day) soil volumetric water content at field capacity */
	double vwc_sat;                                                       /* (mm/day) soil volumetric water content at saturation */
	double psi_sat;                                                       /* (MPa) soil matric potential */
	double soil_b;                                                        /* (DIM) slope of log(psi) vs log(rwc) */
	double soilw_fc;                                                      /* (kgH2O/day) soil water at field capacity */
	double soilw_sat;                                                     /* (kgH2O/day) soil water at saturation */
	double swc;                                                           /* (kgH2O/day) soil Water content */

	/* water */
	int days_since_rain;                                                             /* (days) consecutive days without rain */
	/* pools */
	double asw;                                                                      /* (mm/volume) current available soil water  */
	double max_asw_fc;                                                               /* (mmKgH2O/m3) max available soil water at field capacity */
	double max_asw_sat;                                                              /* (mmKgH2O/m3) max available soil water at field capacity */
	double snow_pack;                                                                /* (Kg/day)current amount of snow */
	double canopy_water_stored;                                                      /* (mm/day) canopy water stored at cell level */
	/* fluxes */
	double daily_snow_melt;                                                          /* (mm/day/time) current amount of melted snow  */
	double daily_snow_subl;                                                          /* (mm/day/time) current amount of sublimated snow */
	double daily_out_flow;                                                           /* (mm/day/time) current amount of water outflow */
	double daily_c_rain_int, monthly_c_rain_int, annual_c_rain_int;                  /* (mm/day/time) daily, monthly and canopy rain interception at cell level */
	double daily_c_snow_int, monthly_c_snow_int, annual_c_snow_int;                  /* (mm/day/time) daily, monthly and canopy snow interception at cell level */
	double daily_c_transp, monthly_c_transp, annual_c_transp;                        /* (mm/day/time) daily, monthly and canopy transpiration at cell level */
	double daily_c_evapo, monthly_c_evapo, annual_c_evapo;                           /* (mm/day/time) daily, monthly and canopy evaporation at cell level */
	double daily_c_evapotransp, monthly_c_evapotransp, annual_c_evapotransp;         /* (mm/day/time) daily, monthly and canopy evapotranspiration at cell level */
	double daily_soil_evapo, monthly_soil_evapo, annual_soil_evapo;                  /* (mm/day/time) daily, monthly and soil evaporation at cell level */
	double daily_et, monthly_et, annual_et;                                          /* (mm/day/time) daily, monthly and evapotranspiration at cell level */
	double daily_tot_w_flux, monthly_tot_w_flux, annual_tot_w_flux;                  /* (mm/day/time) daily, monthly and annual water fluxes at cell level */
	double daily_soil_evaporation_watt;                                              /* (W/day) current daily soil evaporation in watt at cell level */
	double daily_soil_latent_heat_flux;                                              /* (W/day) current daily soil latent heat flux  at cell level */
	double daily_soil_sensible_heat_flux;                                            /* (W/day) current daily soil sensible heat flux at cell level */

	/* energy balance */
	double daily_c_transp_watt;                                                      /* (W/day) daily canopy transpiration at cell level */
	double daily_c_evapo_watt;                                                       /* (W/day) daily canopy evaporation at cell level */
	double daily_c_evapotransp_watt;                                                 /* (W/day) daily canopy evapotranspiration at cell level */
	double daily_c_latent_heat_flux;                                                 /* (W/day) daily canopy latent heat flux at cell level  */
	double daily_latent_heat_flux, monthly_latent_heat_flux, annual_latent_heat_flux;/* (W/day) daily, monthly and annual latent heat flux at cell level */
	double daily_c_sensible_heat_flux;                                               /* (W/day) current daily canopy sensible heat flux */
	double daily_sensible_heat_flux, monthly_sensible_heat_flux, annual_sensible_heat_flux;/* (W/day) daily, monthly and annual sensible heat flux at cell level */

	/* soil scalars */
	double tsoil_scalar;                                                  /* (DIM) soil temperature scalar for decomposition */
	double wsoil_scalar;                                                  /* (DIM) soil water scalar  for decomposition */
	double rate_scalar;                                                   /* (DIM) soil (temperature * water) scalar for decomposition */
	double daily_gross_nmin;                                              /* (tN/cell/day) daily gross N mineralization */
	double daily_gross_nimmob;                                            /* (tNN/cell/day) daily gross N immobilization */
	double daily_net_nmin;                                                /* (tN/cell/day) daily net N mineralization */
	double fpi;                                                           /* (DIM) fraction of potential immobilization */

	/* daily leaf and fine root litter transfer pools */
	//FIXME ADD COARSE WOODY DEBRIS
	/* carbon */
	double daily_litrC;                                                   /* (tC/cell/day) daily leaf + fine root to litter carbon pool at cell level */
	double daily_leaf_litrC;                                              /* (tC/cell/day) daily leaf to litter carbon pool at cell level */
	double daily_leaf_litr1C;                                             /* (tC/cell/day) daily leaf to litter labile carbon pool at cell level */
	double daily_leaf_litr2C;                                             /* (tC/cell/day) daily leaf to litter unshelded cellulose carbon pool at cell level */
	double daily_leaf_litr3C;                                             /* (tC/cell/day) daily leaf to litter  shelded cellulose carbon pool at cell level */
	double daily_leaf_litr4C;                                             /* (tC/cell/day) daily leaf to litter lignin carbon pool at cell level */
	double daily_froot_litrC;                                             /* (tC/cell/day) daily fine root to litter carbon pool at cell level */
	double daily_froot_litr1C;                                            /* (tC/cell/day) daily fine root to litter labile carbon pool at cell level */
	double daily_froot_litr2C;                                            /* (tC/cell/day) daily fine root to litter unshelded cellulose carbon pool at cell level */
	double daily_froot_litr3C;                                            /* (tC/cell/day) daily fine root to litter shelded cellulose carbon pool at cell level */
	double daily_froot_litr4C;                                            /* (tC/cell/day) daily fine root to litter lignin carbon pool at cell level */
	double daily_fruit_litrC;                                             /* (tC/cell/day) daily fruit to litter lignin carbon pool at cell level */
	double daily_soilC;                                                   /* (tC/cell/day) daily litter carbon to soil */

	/* nitrogen */
	double daily_litrN;                                                   /* (tN/cell/day) daily leaf + fine root to litter nitrogen pool at cell level */
	double daily_leaf_litrN;                                              /* (tN/cell/day) daily leaf to litter nitrogen pool at cell level */
	double daily_leaf_litr1N;                                             /* (tN/cell/day) daily leaf to litter labile nitrogen pool at cell level */
	double daily_leaf_litr2N;                                             /* (tN/cell/day) daily leaf to litter unshelded cellulose nitrogen pool at cell level */
	double daily_leaf_litr3N;                                             /* (tN/cell/day) daily leaf to litter  shelded cellulose nitrogen pool at cell level */
	double daily_leaf_litr4N;                                             /* (tN/cell/day) daily leaf to litter lignin nitrogen pool at cell level */
	double daily_froot_litrN;                                             /* (tN/cell/day) daily fine root to litter nitrogen pool at cell level */
	double daily_froot_litr1N;                                            /* (tN/cell/day) daily fine root to litter labile nitrogen pool at cell level */
	double daily_froot_litr2N;                                            /* (tN/cell/day) daily fine root to litter unshelded cellulose nitrogen pool at cell level */
	double daily_froot_litr3N;                                            /* (tN/cell/day) daily fine root to litter shelded cellulose nitrogen pool at cell level */
	double daily_froot_litr4N;                                            /* (tN/cell/day) daily fine root to litter lignin nitrogen pool at cell level */
	double daily_fruit_litrN;                                             /* (tN/cell/day) daily fruit to litter nitrogen pool at cell level */
	double daily_soilN;                                                   /* (tN/cell/day) daily litter nitrogen to soil */

	/* litter and soil carbon pools */
	double leaf_litrC;                                                    /* (tC/cell) leaf total litter carbon */
	double leaf_litr1C;                                                   /* (tC/cell) leaf litter labile carbon */
	double leaf_litr2C;                                                   /* (tC/cell) leaf litter unshielded cellulose carbon */
	double leaf_litr3C;                                                   /* (tC/cell) leaf litter shielded cellulose carbon */
	double leaf_litr4C;                                                   /* (tC/cell) leaf litter lignin carbon */
	double froot_litrC;                                                   /* (tC/cell) fine root total litter carbon */
	double froot_litr1C;                                                  /* (tC/cell) fine root litter labile carbon */
	double froot_litr2C;                                                  /* (tC/cell) fine root litter unshielded cellulose carbon */
	double froot_litr3C;                                                  /* (tC/cell) fine root litter shielded cellulose carbon */
	double froot_litr4C;                                                  /* (tC/cell) fine root litter lignin carbon */
	double fruit_litrC;                                                   /* (tC/cell) fruit total litter carbon */
	double deadwood_litr1C;                                               /* (tC/cell) deadwood litter labile carbon */
	double deadwood_litr2C;                                               /* (tC/cell) deadwood litter unshielded cellulose carbon */
	double deadwood_litr3C;                                               /* (tC/cell) deadwood litter shielded cellulose carbon */
	double deadwood_litr4C;                                               /* (tC/cell) deadwood litter lignin carbon */

	double litr1C;                                                        /* (tC/cell) litter labile carbon */
	double litr2C;                                                        /* (tC/cell) litter unshielded cellulose carbon */
	double litr3C;                                                        /* (tC/cell) litter shielded cellulose carbon */
	double litr4C;                                                        /* (tC/cell) litter lignin carbon */
	double soil1C;                                                        /* (tC/cell) microbial recycling pool carbon (fast) */
	double soil2C;                                                        /* (tC/cell) microbial recycling pool carbon (medium) */
	double soil3C;                                                        /* (tC/cell) microbial recycling pool carbon (slow) */
	double soil4C;                                                        /* (tC/cell) recalcitrant SOM carbon (humus, slowest) */
	double soil_tC;                                                       /* (tC/cell) Soil at cell level */

	/* litter and soil nitrogen pools*/
	double leaf_litrN;                                                    /* (tN/cell) leaf total litter nitrogen */
	double leaf_litr1N;                                                   /* (tN/cell) leaf litter labile nitrogen */
	double leaf_litr2N;                                                   /* (tN/cell) leaf litter unshielded cellulose nitrogen */
	double leaf_litr3N;                                                   /* (tN/cell) leaf litter shielded cellulose nitrogen */
	double leaf_litr4N;                                                   /* (tN/cell) leaf litter lignin nitrogen */
	double froot_litrN;                                                   /* (tN/cell) fine root total litter nitrogen */
	double froot_litr1N;                                                  /* (tN/cell) fine root litter labile nitrogen */
	double froot_litr2N;                                                  /* (tN/cell) fine root litter unshielded cellulose nitrogen */
	double froot_litr3N;                                                  /* (tN/cell) fine root litter shielded cellulonitrogenN */
	double froot_litr4N;                                                  /* (tN/cell) fine root litter lignin nitrogen */
	double fruit_litrN;                                                   /* (tN/cell) fruit total litter nitrogen */
	double deadwood_litr1N;                                               /* (tN/cell) deadwood litter labile nitrogen */
	double deadwood_litr2N;                                               /* (tN/cell) deadwood litter unshielded cellulose nitrogen */
	double deadwood_litr3N;                                               /* (tN/cell) deadwood litter shielded cellulose nitrogen */
	double deadwood_litr4N;                                               /* (tN/cell) deadwood litter lignin nitrogen */


	double cwdN;                                                          /* (tN/cell) coarse woody debris nitrogen */
	double litr1N;                                                        /* (tN/cell) litter labile nitrogen */
	double litr2N;                                                        /* (tN/cell) litter unshielded cellulose nitrogen */
	double litr3N;                                                        /* (tN/cell) litter shielded cellulose nitrogen */
	double litr4N;                                                        /* (tN/cell) litter lignin nitrogen */
	double soil1N;                                                        /* (tN/cell) microbial recycling pool nitrogen (fast) */
	double soil2N;                                                        /* (tN/cell) microbial recycling pool nitrogen (medium) */
	double soil3N;                                                        /* (tN/cell) microbial recycling pool nitrogen (slow) */
	double soil4N;                                                        /* (tN/cell) recalcitrant SOM nitrogen (humus, slowest) */
	double sminN;                                                         /* (tN/cell) soil mineral nitrogen */
	double retransN;                                                      /* (tN/cell) plant pool of retranslocated nitrogen */
	double Npool;                                                         /* (tN/cell) temporary plant nitrogen pool */
	double Nfix_src;                                                      /* (tN/cell) SUM of biological nitrogen fixation */
	double Ndep_src;                                                      /* (tN/cell) SUM of nitrogen deposition inputs */
	double Nleached;                                                      /* (tN/cell) SUM of nitrogen leached */

	/* litter and soil decomposition fluxes */
	double cwdc_to_litr2C;                                                /* (kgC/day/day) daily decomposition fluxes cwd to litter */
	double cwdc_to_litr3C;                                                /* (kgC/day/day) daily decomposition fluxes cwd to litter */
	double cwdc_to_litr4C;                                                /* (kgC/day/day) daily decomposition fluxes cwd to litter */
	double litr1C_to_soil1C;                                              /* (kgC/day/day) daily decomposition fluxes litter to soil */
	double litr2C_to_soil2C;                                              /* (kgC/day/day) daily decomposition fluxes litter to soil */
	double litr3C_to_litr2C;                                              /* (kgC/day/day) daily decomposition fluxes litter to soil */
	double litr4C_to_soil3C;                                              /* (kgC/day/day) daily decomposition fluxes litter to soil */
	double soil1C_to_soil2C;                                              /* (kgC/day/day) daily decomposition fluxes soil to soil */
	double soil2C_to_soil3C;                                              /* (kgC/day/day) daily decomposition fluxes soil to soil */
	double soil3C_to_soil4C;                                              /* (kgC/day/day) daily decomposition fluxes soil to soil */

	/* litter and soil decomposition fluxes */
	double cwdc_to_litr2N;                                                /* (kgN/day/day) daily decomposition fluxes cwd to litter */
	double cwdc_to_litr3N;                                                /* (kgN/day/day) daily decomposition fluxes cwd to litter */
	double cwdc_to_litr4N;                                                /* (kgN/day/day) daily decomposition fluxes cwd to litter */
	double litr1N_to_soil1N;                                              /* (kgN/day/day) daily decomposition fluxes litter to soil */
	double litr2N_to_soil2N;                                              /* (kgN/day/day) daily decomposition fluxes litter to soil */
	double litr3N_to_litr2N;                                              /* (kgN/day/day) daily decomposition fluxes litter to soil */
	double litr4N_to_soil3N;                                              /* (kgN/day/day) daily decomposition fluxes litter to soil */
	double soil1N_to_soil2N;                                              /* (kgN/day/day) daily decomposition fluxes soil to soil */
	double soil2N_to_soil3N;                                              /* (kgN/day/day) daily decomposition fluxes soil to soil */
	double soil3N_to_soil4N;                                              /* (kgN/day/day) daily decomposition fluxes soil to soil */


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
	double litr1_rh;                                                      /* (kgC/day/day) heterotrophic respiration for litter labile carbon */
	double litr2_rh;                                                      /* (kgC/day/day) heterotrophic respiration for litter unshielded cellulose carbon */
	double litr3_rh;                                                      /* (kgC/day/day) heterotrophic respiration for litter shielded cellulose carbon */
	double litr4_rh;                                                      /* (kgC/day/day) heterotrophic respiration for litter lignin carbon */
	double soil1_rh;                                                      /* (kgC/day/day) heterotrophic respiration for microbial recycling pool carbon (fast) */
	double soil2_rh;                                                      /* (kgC/day/day) heterotrophic respiration for microbial recycling pool carbon (medium) */
	double soil3_rh;                                                      /* (kgC/day/day) heterotrophic respiration for microbial recycling pool carbon (slow) */
	double soil4_rh;                                                      /* (kgC/day/day) heterotrophic respiration for recalcitrant SOM carbon (humus, slowest) */

	/* soil fraction */
	double leaf_litt_scel_frac;                                           /* (DIM) leaf litter shielded cellulose fraction */
	double leaf_litt_uscel_frac;                                          /* (DIM) leaf litter unshielded cellulose fraction */
	double froot_litt_scel_frac;                                          /* (DIM) fine root litter shielded cellulose fraction */
	double froot_litt_uscel_frac;                                         /* (DIM) fine root litter unshielded fraction */
	double dead_wood_scel_frac;                                           /* (DIM) dead wood litter shielded cellulose fraction */
	double dead_wood_uscel_frac;                                          /* (DIM) dead wood litter unshielded fraction */

	/*** BALANCES VARIABLES ***/
	double carbon_in;                                                     /* (gC/day/day) cell carbon balances in */
	double carbon_out;                                                    /* (gC/day/day) cell carbon balances out */
	double carbon_store;                                                  /* (gC/day) cell carbon balances store */
	double carbon_old_store;                                              /* (gC/day) cell carbon balances old_store */
	double carbon_balance;                                                /* (gC/day/day) cell carbon balances balance */

	double carbontc_in;                                                   /* (tC/cell/day) cell carbon balances in */
	double carbontc_out;                                                  /* (tC/cell/day) cell carbon balances out */
	double carbontc_store;                                                /* (tC/cell) cell carbon balances store */
	double carbontc_old_store;                                            /* (tC/cell) cell carbon balances old_store */
	double carbontc_balance;                                              /* (tC/cell/day) cell carbon balances balance */

	double water_in;                                                      /* (mm/day/day) cell water balances in */
	double water_out;                                                     /* (mm/day/day) cell water balances out */
	double water_store;                                                   /* (mm/day) cell water balances store */
	double water_old_store;                                               /* (mm/day) cell water balances old_store */
	double water_balance;                                                 /* (mm/day/day) cell water balances balance */

	double snow_in;                                                       /* (mm/day/day) cell snow balances in */
	double snow_out;                                                      /* (mm/day/day) cell snow balances out */
	double snow_store;                                                    /* (mm/day) cell snow balances store */
	double snow_old_store;                                                /* (mm/day) cell snow balances old_store */
	double snow_balance;                                                  /* (mm/day/day) cell snow balances balance */

	double nitrogen_in;                                                   /* (gN/day/day) cell nitrogen balances in */
	double nitrogen_out;                                                  /* (gN/day/day) cell nitrogen balances out */
	double nitrogen_store;                                                /* (gN/day) cell nitrogen balances store */
	double nitrogen_old_store;                                            /* (gN/day) cell nitrogen balances old_store */
	double nitrogen_balance;                                              /* (gN/day/day) cell nitrogen balances balance */

	double nitrogentc_in;                                                 /* (tN/cell/day) cell nitrogen balances in */
	double nitrogentc_out;                                                /* (tN/cell/day) cell nitrogen balances out */
	double nitrogentc_store;                                              /* (tN/cell) cell nitrogen balances store */
	double nitrogentc_old_store;                                          /* (tN/cell/day) cell nitrogen balances old_store */
	double nitrogentc_balance;                                            /* (tN/cell/day) cell nitrogen balances balance */

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
