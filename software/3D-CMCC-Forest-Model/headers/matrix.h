/* matrix.h */
#ifndef MATRIX_H_
#define MATRIX_H_

#include "meteo.h"
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
	DENMAX,                              /* (trees/cell) maximum stand density  */
	DENMIN,                              /* (trees/cell) minimum stand density */
	CROWN_RADIUS,                        /* (m) Crown Projected Radius */
	CROWN_DIAMETER,                      /* (m) Crown Projected Diameter */
	CROWN_HEIGHT,                        /* (m) Crown height */
	CROWN_AREA_PROJ,                     /* (m2) Crown Projected Area (at zenith angle) */
	CROWN_AREA_EXP,                      /* (m2) Crown Exposed Area (considered as the area that can intercepts light) */
	CROWN_VOLUME,                        /* (m3) Crown Volume */
	CROWN_DENSITY,                       /* (m) Crown density */
	CANOPY_COVER_PROJ,                   /* (DIM) Canopy Projected Cover % of pixel covered (at zenith angle) */
	CANOPY_COVER_EXP,                    /* (DIM) Canopy Exposed Cover % of pixel exposed (integrated all over all viewing angles) */
	DAILY_CANOPY_COVER_PROJ,             /* (DIM) Daily Canopy Projected Cover % of pixel covered (at zenith angle) */
	DAILY_CANOPY_COVER_EXP,              /* (DIM) Daily Canopy Exposed Cover % of pixel exposed (integrated all over the daylength) */
	HD_EFF,                              /* (DIM) Effective Height/Diameter ratio */
	HD_MAX,                              /* (DIM) Height (m) to Base diameter (m) ratio MAX */
	HD_MIN,                              /* (DIM) Height (m) to Base diameter (m) ratio MIN */
	SAPWOOD_AREA,                        /* (cm2) tree sapwood area */
	SAPWOOD_PERC,                        /* sapwood area to total stem area */
	HEARTWOOD_AREA,                      /* (cm2) tree heartwood area */
	HEARTWOOD_PERC,                      /* (DIM) heartwood area to total stem area */
	BASAL_AREA,                          /* (cm2/tree) Individual Basal Area */
	BASAL_AREA_m2,                       /* (m2/tree) Individual Basal Area */
	STAND_BASAL_AREA,                    /* (cm2/class) Class Basal Area of overall class */
	STAND_BASAL_AREA_m2,                 /* (m2/class) Class Basal Area of overall class */
	MASS_DENSITY,                        /* (tDM/m3) mass density */
	VOLUME,                              /* (m3/class) Stem Volume */
	TREE_VOLUME,                         /* (m3/tree) Single Tree Volume */
	CAI,                                 /* (m3/class/year) Current Annual Volume Increment */
	TREE_CAI,                            /* (m3/tree/year) Single Tree Current Annual Volume Increment */
	MAI,                                 /* (m3/class/year) Mean Annual Volume Increment */
	TREE_MAI,                            /* (m3/tree/year) Single Tree Mean Annual Volume Increment */
	STEMCONST,                           /* constant in the stem mass vs. diameter relationship */
	MAX_SPECIES_COVER,                   /* (DIM) maximum species-specific cell cover */

	/* PAR */
	PAR,                                /* (molPAR/m2/day) Photosynthetically Active Radiation  */
	PAR_SUN,                            /* (molPAR/m2/day) Photosynthetically Active Radiation for sun leaves */
	PAR_SHADE,                          /* (molPAR/m2/day) Photosynthetically Active Radiation for shaded leaves */
	PAR_REFL,                           /* (molPAR/m2/day) Reflected Photosynthetically Active Radiation OVERALL CANOPY */
	PAR_REFL_SUN,                       /* (molPAR/m2/day) Reflected Photosynthetically Active Radiation for sun leaves */
	PAR_REFL_SHADE,                     /* (molPAR/m2/day) Reflected Photosynthetically Active Radiation for shaded leaves */
	APAR,                               /* (molPAR/m2/day) Absorbed Photosynthetically Active Radiation */
	APAR_SUN,                           /* (molPAR/m2/day) Absorbed Photosynthetically Active Radiation for sun leaves */
	APAR_SHADE,                         /* (molPAR/m2/day) Absorbed Photosynthetically Active Radiation for shaded leaves */
	TRANSM_PAR,                         /* (molPAR/m2/day) Transmitted Photosynthetically Active Radiation */
	TRANSM_PAR_SUN,                     /* (molPAR/m2/day) Transmitted Photosynthetically Active Radiation from sun leaves */
	TRANSM_PAR_SHADE,                   /* (molPAR/m2/day) Transmitted Photosynthetically Active Radiation from shaded leaves */

	YEARLY_APAR,

	/* short wave */
	SW_RAD,                             /* (W/m2) Short Wave Radiation  */
	SW_RAD_SUN,                         /* (W/m2) Short Wave Radiation for sun leaves */
	SW_RAD_SHADE,                       /* (W/m2) Short Wave Radiation for shaded leaves */
	SW_RAD_REFL,                        /* (W/m2) Reflected Short Wave Radiation OVERALL CANOPY */
	SW_RAD_REFL_SUN,                    /* (W/m2) Reflected Short Wave Radiation for sun leaves */
	SW_RAD_REFL_SHADE,                  /* (W/m2) Reflected Short Wave Radiation for shaded leaves */
	SW_RAD_ABS,                         /* (W/m2) Absorbed Net Short Wave radiation */
	SW_RAD_ABS_SUN,                     /* (W/m2) Absorbed Net Short Wave Radiation for sun leaves */
	SW_RAD_ABS_SHADE,                   /* (W/m2) Absorbed Net Short Wave Radiation for shaded leaves */
	SW_RAD_TRANSM,                      /* (W/m2) Transmitted Net Short Wave Radiation */
	SW_RAD_TRANSM_SUN,                  /* (W/m2) Transmitted Net Short Wave Radiation for sun leaves */
	SW_RAD_TRANSM_SHADE,                /* (W/m2) Transmitted Net Short Wave Radiation for shaded leaves */

	/* long wave */
	NET_LW_RAD,                         /* (W/m2) Long Wave Radiation */
	LW_RAD_REFL,                        /* (W/m2) Reflected Long Wave Radiation OVERALL CANOPY */
	LW_RAD_EMIT,                        /* (W/m2) Emitted Long Wave radiation */
	LW_RAD_EMIT_SUN,                    /* (W/m2) Emitted Long Wave Radiation for sun leaves */
	LW_RAD_EMIT_SHADE,                  /* (W/m2) Emitted Long Wave Radiation for shaded leaves */
	LW_RAD_ABS,                         /* (W/m2) Absorbed Long Wave radiation */
	LW_RAD_ABS_SUN,                     /* (W/m2) Absorbed Long Wave Radiation for sun leaves */
	LW_RAD_ABS_SHADE,                   /* (W/m2) Absorbed Long Wave Radiation for shaded leaves */
	LW_RAD_TRANSM,                      /* (W/m2) Transmitted Long Wave Radiation */
	LW_RAD_TRANSM_SUN,                  /* (W/m2) Transmitted Long Wave Radiation for sun leaves */
	LW_RAD_TRANSM_SHADE,                /* (W/m2) Transmitted Long Wave Radiation for shaded leaves */

	/* net radiation */
	NET_RAD,                            /* (W/m2) Short Wave Radiation */
	RAD_REFL,                           /* (W/m2) Reflected Radiation OVERALL CANOPY */
	NET_RAD_ABS,                        /* (W/m2) Absorbed Net radiation */
	NET_RAD_ABS_SUN,                    /* (W/m2) Absorbed Net Radiation for sun leaves */
	NET_RAD_ABS_SHADE,                  /* (W/m2) Absorbed Net Radiation for shaded leaves */
	NET_RAD_TRANSM,                     /* (W/m2) Transmitted Net Radiation */
	NET_RAD_TRANSM_SUN,                 /* (W/m2) Transmitted Net Radiation for sun leaves */
	NET_TRANSM_SHADE,                   /* (W/m2) Transmitted Net Radiation for shaded leaves */

	/* PPFD */
	PPFD,                               /* (umol/m2/sec) Photosynthetic Photon Flux Density  */
	PPFD_SUN,                           /* (umol/m2/sec) Photosynthetic Photon Flux Density for sun leaves */
	PPFD_SHADE,                         /* (umol/m2/sec) Photosynthetic Photon Flux Density for shaded leaves */
	PPFD_REFL,                          /* (umol/m2/sec) Transmitted Photosynthetic Photon Flux Density */
	PPFD_REFL_SUN,                      /* (umol/m2/sec) Transmitted Photosynthetic Photon Flux Density for sun leaves */
	PPFD_REFL_SHADE,                    /* (umol/m2/sec) Transmitted Photosynthetic Photon Flux Density for shaded leaves */
	PPFD_ABS,                           /* (umol/m2/sec) Absorbed Photosynthetic Photon Flux Density */
	PPFD_ABS_SUN,                       /* (umol/m2/sec) Absorbed Photosynthetic Photon Flux Density for sun leaves */
	PPFD_ABS_SHADE,                     /* (umol/m2/sec) Absorbed Photosynthetic Photon Flux Density for shaded leaves */
	PPFD_TRANSM,                        /* (umol/m2/sec) Transmitted Photosynthetic Photon Flux Density */
	PPFD_TRANSM_SUN,                    /* (umol/m2/sec) Transmitted Photosynthetic Photon Flux Density from sun leaves */
	PPFD_TRANSM_SHADE,                  /* (umol/m2/sec) Transmitted Photosynthetic Photon Flux Density from shaded leaves */

	/* modifiers */
	F_VPD,                              /* (DIM) VPD modifier */
	F_LIGHT,                            /* (DIM) LIGHT modifier */
	F_LIGHT_SUN,                        /* (DIM) LIGHT modifier for Sun leaves */
	F_LIGHT_SHADE,                      /* (DIM) LIGHT modifier for Shaded leaves */
	F_AGE,                              /* (DIM) AGE modifier */
	FR,                                 /* (DIM) Nitrogen fertility rate */
	F_NUTR,                             /* (DIM) SOIL NUTRIENT Modifier */
	F_T,                                /* (DIM) TEMPERATURE modifier */
	F_SW,                               /* (DIM) SOIL WATER modifier */
	F_DROUGHT,                          /* (DIM) SOIL DROUGHT modifier (see Duursma et al., 2008) */
	F_PSI,                              /* (DIM) SOIL WATER modifier using PSI, see Biome */
	F_CO2,                              /* (DIM) CO2 soil fertilization effect */
	F_CO2_TR,                           /* (DIM) CO2 reduction effect (for stomatal conductance) */
	F_A_SUN,                            /* (DIM) ASSIMILATION modifier for Sun leaves (test) */
	F_A_SHADE,                          /* (DIM) ASSIMILATION modifier for Shaded leaves (test) */
	PHYS_MOD,                           /* (DIM) PHYSIOLOGICAL modifier */

	/* water */
	/* pools */
	CANOPY_WATER,                       /* (mm/m2) canopy water pool */
	CANOPY_SNOW,                        /* (mm/m2) canopy snow pool  */

	/* fluxes */
	STOMATAL_CONDUCTANCE,               /* (m/sec) stomatal conductance */
	STOMATAL_SUN_CONDUCTANCE,           /* (m/sec) stomatal conductance for sun leaves */
	STOMATAL_SHADE_CONDUCTANCE,         /* (m/sec) stomatal conductance for shade leaves */
	LEAF_CONDUCTANCE,                   /* (m/sec) leaf conductance */
	LEAF_SUN_CONDUCTANCE,               /* (m/sec) leaf conductance for sun leaves */
	LEAF_SHADE_CONDUCTANCE,             /* (m/sec) leaf conductance for shade leaves */
	CANOPY_BLCOND,                      /* (m/sec) Canopy Boundary Layer conductance */
	FRAC_DAYTIME_TRANSP,                /* (DIM) fraction of daytime for transpiration (and photosynthesis) */
	CANOPY_INT,                         /* (mm/day) canopy interception of rainfall */
	CANOPY_INT_SNOW,                    /* (mm/day) canopy interception of snow */
	CANOPY_EVAPO,                       /* (mm/day) canopy evaporation */
	CANOPY_TRANSP,                      /* (mm/day) canopy transpiration */
	CANOPY_TRANSP_SUN,                  /* (mm/day) canopy transpiration for sun leaves */
	CANOPY_TRANSP_SHADE,                /* (mm/day) canopy transpiration for shade leaves */
	CANOPY_EVAPO_TRANSP,                /* (mm/day) canopy evapotranspiration */
	CANOPY_FRAC_DAY_TRANSP,             /* (DIM) Fraction of daily canopy transpiration */
	CANOPY_LATENT_HEAT,                 /* (W/m2) canopy latent heat */
	CANOPY_SENSIBLE_HEAT,               /* (W/m2) canopy sensible heat */

	MONTHLY_CANOPY_TRANSP,              /* (mm/month) monthly canopy transpiration */
	MONTHLY_CANOPY_TRANSP_SUN,          /* (mm/month) monthly canopy transpiration for sun leaves */
	MONTHLY_CANOPY_TRANSP_SHADE,        /* (mm/month) monthly canopy transpiration for shade leaves */
	MONTHLY_CANOPY_INT,                 /* (mm/month) monthly canopy interception */
	MONTHLY_CANOPY_EVAPO_TRANSP,        /* (mm/month) monthly canopy evapotranspiration */
	YEARLY_CANOPY_TRANSP,               /* (mm/year) annual canopy transpiration */
	YEARLY_CANOPY_TRANSP_SUN,           /* (mm/year) annual canopy transpiration for sun leaves */
	YEARLY_CANOPY_TRANSP_SHADE,         /* (mm/year) annual canopy transpiration for shade leaves */
	YEARLY_CANOPY_INT,                  /* (mm/year) annual canopy interception */
	YEARLY_CANOPY_EVAPO_TRANSP,         /* (mm/year) annual canopy evapotranspiration */
	MONTHLY_CANOPY_LATENT_HEAT,         /* (W/m2) monthly canopy latent heat */
	YEARLY_CANOPY_LATENT_HEAT,          /* (W/m2) annual canopy latent heat */

	/* carbon use efficiency */
	CUE,                                /* (gCNPP/gCGPP) daily Carbon Use Efficiency */
	MONTHLY_CUE,                        /* (gCNPP/gCGPP) monthly Carbon Use Efficiency */
	YEARLY_CUE,                         /* (gCNPP/gCGPP) annual Carbon Use Efficiency */

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
	LAI_PROJ,                           /* (m2/m2) LAI for Projected Area covered (at zenith angle) */
	LAI_SUN_PROJ,                       /* (m2/m2) LAI for sun leaves for Projected Area covered (at zenith angle) */
	LAI_SHADE_PROJ,                     /* (m2/m2) LAI for shaded leaves for Projected Area covered (at zenith angle) */
	MAX_LAI_PROJ,                       /* (m2/m2) LAI Projected at the very first day of senescence: parameter of the sigmoid function */
	PEAK_LAI_PROJ,                      /* (m2/m2) Peak Projected LAI */
	ALL_LAI_PROJ,                       /* (m2) All side Projected LAI */
//	LAI_EXP,                            /* (m2/m2) LAI for Exposed Area covered (all over all viewing angles) */
//	LAI_SUN_EXP,                        /* (m2/m2) LAI for sun leaves for Exposed Area covered (all over all viewing angles) */
//	LAI_SHADE_EXP,                      /* (m2/m2) LAI for shaded leaves for Exposed Area covered (all over all viewing angles) */
	LAD,                                /* (m2/m2) Leaf Area Density */
	LEAF_TEMP_K,                        /* (K) leaf temperature */
	CANOPY_TEMP_K,                      /* (K) canopy temperature */
	CANOPY_TEMP_K_OLD,                  /* (K) previous time canopy temperature */

	/* carbon fluxes */
	C_FLUX,
	GPP,                                /* (gC/m2/day) Daily Gross Primary Production */
	GPP_SUN,                            /* (gC/m2/day) Daily Sun Leaves Gross Primary Production */
	GPP_SHADE,                          /* (gC/m2/day) Daily Shade Leaves Gross Primary Production */
	MONTHLY_GPP,                        /* (gC/m2/month) Monthly Gross Primary Production */
	MONTHLY_GPP_SUN,                    /* (gC/m2/month) Monthly Gross Primary Production for sun leaves */
	MONTHLY_GPP_SHADE,                  /* (gC/m2/month) Monthly Gross Primary Production for shaded leaves */
	YEARLY_GPP,                         /* (gC/m2/year) Yearly Gross Primary Production */
	YEARLY_GPP_SUN,                     /* (gC/m2/year) Yearly Gross Primary Production for sun leaves */
	YEARLY_GPP_SHADE,                   /* (gC/m2/year) Yearly Gross Primary Production for shaded leaves */
	GPP_tC,                             /* (tC/cell/day) Daily Gross Primary Production  */
	NPP,                                /* (gC/m2/day) Daily Net Primary Production */
	MONTHLY_NPP,                        /* (gC/m2/monthly) Monthly Net Primary Production */
	YEARLY_NPP,                         /* (gC/m2/year) Yearly Net Primary Production */
	NPP_tC,                             /* (tC/cell/day) Daily Net Primary Production */
	MONTHLY_NPP_tC,                     /* (tC/cell/month) Monthly Net Primary Production */
	YEARLY_NPP_tC,                      /* (tC/cell/day) Yearly Net Primary Production */

	/* nitrogen fluxes */
	NPP_gN,                             /* (gN/m2/day) Daily Net Primary Production demand */
	MONTHLY_NPP_gN,                     /* (gN/m2/month) Monthly Net Primary Production demand */
	YEARLY_NPP_gN,                      /* (gN/m2/year) Yearly Net Primary Production demand */
	NPP_tN,                             /* (tNC/cell/day) Daily Net Primary Production demand */
	MONTHLY_NPP_tN,                     /* (tNC/cell/month) Monthly Net Primary Production demand */
	YEARLY_NPP_tN,                      /* (tNC/cell/year) Yearly Net Primary Production demand */

	/* transfer pools carbon to carbon pools */
	C_TO_LEAF,                          /* (tC/cell/day) Daily Net Primary Production to Leaf pool */
	C_TO_FROOT,                         /* (tC/cell/day) Daily Net Primary Production to Fine root pool */
	C_TO_CROOT,                         /* (tC/cell/day) Daily Net Primary Production to Coarse root pool */
	C_TO_STEM,                          /* (tC/cell/day) Daily Net Primary Production to Stem pool */
	C_TO_BRANCH,                        /* (tC/cell/day) Daily Net Primary Production to Branch pool */
	C_TO_RESERVE,                       /* (tC/cell/day) Daily Net Primary Production to Reserve pool */
	C_TO_FRUIT,                         /* (tC/cell/day) Daily Net Primary Production to Fruit pool */
	LEAF_C_TO_REMOVE,                   /* (tC/cell/day) Daily Leaf to remove */
	FROOT_C_TO_REMOVE,                  /* (tC/cell/day) Daily Fine Root to remove tC/sizeCell */
	FRUIT_C_TO_REMOVE,                  /* (tC/cell/day) Daily Fruit to remove */
	BRANCH_C_TO_REMOVE,                 /* (tC/cell/day) Annual Branch to remove */
	C_LEAF_TO_RESERVE,                  /* (tC/cell/day) Daily retranslocated C from Leaf pool to Reserve */
	C_FROOT_TO_RESERVE,                 /* (tC/cell/day) Daily retranslocated C from Fine root pool to Reserve */
	C_LEAF_TO_LITR,                     /* (tC/cell/day) Daily tC from Leaf pool to Litter */
	C_FROOT_TO_LITR,                    /* (tC/cell/day) Daily tC from Fine root pool to Soil */
	C_BRANCH_TO_RESERVE,                /* (tC/cell/day) Daily tC from Branch pool to Litter */
	C_STEM_TO_CWD,                      /* (tC/cell/day) Daily tC from Stem pool to Coarse Woody Debris */
	C_CROOT_TO_CWD,                     /* (tC/cell/day) Daily tC from Coarse root pool to Coarse Woody Debris */
	C_BRANCH_TO_CWD,                    /* (tC/cell/day) Daily tC from Branch pool to Coarse Woody Debris */
	C_RESERVE_TO_CWD,                   /* (tC/cell/day) Daily tC from Reserve pool to Coarse Woody Debris */
	C_FRUIT_TO_CWD,                     /* (tC/cell/day) Daily tC from Fruit pool to Coarse Woody Debris */
	C_TO_LITR,                          /* (tC/cell/day) Daily Leaf and Fine root Carbon to Litter pool */
	C_TO_CWD,                           /* (tC/cell/day) Daily Wood Carbon to Coarse Woody Debirs */
	C_STEM_LIVEWOOD_TO_DEADWOOD,        /* (tC/cell/day) Daily tC from Stem live wood pool to Stem dead wood */
	C_CROOT_LIVE_WOOD_TO_DEADWOOD,      /* (tC/cell/day) Daily tC from Coarse live wood pool to Coarse dead wood */
	C_BRANCH_LIVE_WOOD_TO_DEAD_WOOD,    /* (tC/cell/day) Daily tC from Branch live wood pool to Branch dead wood */
	C_LEAF_TO_LITR1C,                   /* (tC/cell/day) Daily Leaf Litter Carbon to Litter labile carbon pool */
	C_LEAF_TO_LITR2C,                   /* (tC/cell/day) Daily Leaf Litter Carbon to Litter unshielded cellulose carbon pool */
	C_LEAF_TO_LITR3C,                   /* (tC/cell/day) Daily Leaf Litter Carbon to Litter shielded cellulose carbon pool */
	C_LEAF_TO_LITR4C,                   /* (tC/cell/day) Daily Leaf Litter Carbon to Litter lignin carbon pool */
	C_FROOT_TO_LITR1C,                  /* (tC/cell/day) Daily Fine Root Litter Carbon to Litter labile carbon pool */
	C_FROOT_TO_LITR2C,                  /* (tC/cell/day) Daily Fine Root Litter Carbon to Litter unshielded cellulose carbon pool */
	C_FROOT_TO_LITR3C,                  /* (tC/cell/day) Daily Fine Root Litter Carbon to Litter shielded cellulose carbon pool */
	C_FROOT_TO_LITR4C,                  /* (tC/cell/day) Daily Fine Root Litter Carbon to Litter lignin carbon pool */
	C_CWDC_TO_LITR2C,                   /* (tC/cell/day) Daily Coarse Woody Debris Carbon to Litter unshielded cellulose carbon pool */
	C_CWDC_TO_LITR3C,                   /* (tC/cell/day) Daily Coarse Woody Debris Carbon to Litter shielded cellulose carbon pool */
	C_CWDC_TO_LITR4C,                   /* (tC/cell/day) Daily Coarse Woody Debris Carbon to Litter lignin carbon pool */


	/* carbon pools in tons of tC/sizeCell */
	LEAF_C,                             /* (tC/cell) Current Leaf carbon pool */
	MAX_LEAF_C,                         /* (tC/cell) Maximum Current Leaf carbon pool */
	CROOT_C,                            /* (tC/cell) Current Coarse root carbon pool */
	FROOT_C,                            /* (tC/cell) Current Fine root carbon pool */
	MAX_FROOT_C,                        /* (tC/cell) Maximum Current Fine root carbon pool */
	MAX_BUD_BURST_C,                    /* (tC/cell) Maximum Current Leaf carbon pool for BudBurst */
	STEM_C,                             /* (tC/cell) Current Stem carbon pool */
	BRANCH_C,                           /* (tC/cell) Current Branch carbon pool */
	MIN_RESERVE_C,                      /* (tC/cell) Current Minimum reserve carbon pool */
	RESERVE_C,                          /* (tC/cell) Current Reserve carbon pool */
	FRUIT_C,                            /* (tC/cell) Current Fruit carbon pool */
	LITR_C,                             /* (tC/cell) Current Litter carbon pool */
	SOIL_C,                             /* (tC/cell) Current Soil carbon pool */
	CWD_C,                              /* (tC/cell) Current Coarse Woody Debris carbon pool */
	LIVE_WOOD_C,                        /* (tC/cell) Current Live wood carbon pool */
	DEAD_WOOD_C,                        /* (tC/cell) Current Dead wood carbon pool */
	TOT_WOOD_C,                         /* (tC/cell) Current Total wood carbon pool */
	TOTAL_C,                            /* (tC/cell) Current Total carbon pool */
	STEM_LIVE_WOOD_C,                   /* (tC/cell) Current Stem live wood carbon pool */
	STEM_DEAD_WOOD_C,                   /* (tC/cell) Current Stem dead wood carbon pool */
	CROOT_LIVE_WOOD_C,                  /* (tC/cell) Current Coarse root live wood carbon pool */
	CROOT_DEAD_WOOD_C,                  /* (tC/cell) Current Coarse root dead wood carbon pool */
	BRANCH_LIVE_WOOD_C,                 /* (tC/cell) Current Branch live wood carbon pool */
	BRANCH_DEAD_WOOD_C,                 /* (tC/cell) Current Branch dead wood carbon pool */
	STEM_SAPWOOD_C,                     /* (tC/cell) Current Stem sapwood carbon pool */
	STEM_HEARTWOOD_C,                   /* (tC/cell) Current Stem heartwood carbon pool */
	CROOT_SAPWOOD_C,                    /* (tC/cell) Current Coarse root sapwood carbon pool */
	CROOT_HEARTWOOD_C,                  /* (tC/cell) Current Coarse root heartwood carbon pool */
	BRANCH_SAPWOOD_C,                   /* (tC/cell) Current Branch sapwood carbon pool */
	BRANCH_HEARTWOOD_C,                 /* (tC/cell) Current Branch heartwood carbon pool */
	TOT_SAPWOOD_C,                      /* (tC/cell) Current total sapwood carbon pool */
	TOT_HEARTWOOD_C,                    /* (tC/cell) Current total heartwood carbon pool */
	EFF_LIVE_TOTAL_WOOD_FRAC,           /* (DIM) Age-related fraction of Live biomass per Total biomass */
	DAILY_LIVE_WOOD_TURNOVER,           /* (tC/cell/day) Daily live wood turnover rate */

	/* per tree in tC */
	AV_LEAF_MASS_C,                     /* (tC/tree) Average Leaf carbon pool */
	AV_STEM_MASS_C,                     /* (tC/tree) Average Stem carbon pool */
	AV_FROOT_MASS_C,                    /* (tC/tree) Average Fine root carbon pool */
	AV_CROOT_MASS_C,                    /* (tC/tree) Average Coarse carbon pool */
	AV_RESERVE_MASS_C,                  /* (tC/tree) Average Reserve carbon pool */
	AV_MIN_RESERVE_C,                   /* (tC/tree) Average Minimum Reserve carbon pool */
	AV_FRUIT_MASS_C,                    /* (tC/tree) Average Fruit carbon pool */
	AV_BRANCH_MASS_C,                   /* (tC/tree) Average Branch carbon pool */
	AV_STEM_SAPWOOD_MASS_C,             /* (tC/tree) Average Sapwood Stem carbon pool */
	AV_STEM_HEARTWOOD_MASS_C,           /* (tC/tree) Average Heartwood Stem carbon pool */
	AV_LIVE_STEM_MASS_C,                /* (tC/tree) Average Live Stem carbon pool */
	AV_DEAD_STEM_MASS_C,                /* (tC/tree) Average Dead Stem carbon pool */
	AV_CROOT_SAPWOOD_MASS_C,            /* (tC/tree) Average Sapwood Coarse root carbon pool */
	AV_CROOT_HEARTWOOD_MASS_C,          /* (tC/tree) Average Heartwood Coarse root carbon pool */
	AV_LIVE_CROOT_MASS_C,               /* (tC/tree) Average Live Coarse root carbon pool */
	AV_DEAD_CROOT_MASS_C,               /* (tC/tree) Average Dead Coarse root carbon pool */
	AV_BRANCH_SAPWOOD_MASS_C,           /* (tC/tree) Average Sapwood Branch carbon pool */
	AV_BRANCH_HEARTWOOD_MASS_C,         /* (tC/tree) Average Heartwood Branch carbon pool */
	AV_LIVE_BRANCH_MASS_C,              /* (tC/tree) Average Live Branch carbon pool */
	AV_DEAD_BRANCH_MASS_C,              /* (tC/tree) Average Dead Stem carbon pool */
	AV_LIVE_WOOD_MASS_C,                /* (tC/tree) Average Live Wood carbon pool */
	AV_DEAD_WOOD_MASS_C,                /* (tC/tree) Average Dead Wood carbon pool */
	AV_TOT_WOOD_MASS_C,                 /* (tC/tree) Average Total Wood carbon pool */

	/* carbon biomass pools in tDM/sizeCell */
	BIOMASS_LEAF_tDM,                   /* (tDM/cell) Current Leaf carbon pool */
	BIOMASS_CROOT_tDM,                  /* (tDM/cell) Current Coarse root carbon pool */
	BIOMASS_FROOT_tDM,                  /* (tDM/cell) Current Fine root carbon pool */
	BIOMASS_STEM_tDM,                   /* (tDM/cell) Current Stem carbon pool */
	BIOMASS_BRANCH_tDM,                 /* (tDM/cell) Current Branch carbon pool */
	RESERVE_tDM,                        /* (tDM/cell) Current Reserve carbon pool */
	BIOMASS_FRUIT_tDM,                  /* (tDM/cell) Current Fruit carbon pool */
	BIOMASS_LIVE_WOOD_tDM,              /* (tDM/cell) Current Live Wood carbon pool */
	BIOMASS_DEAD_WOOD_tDM,              /* (tDM/cell) Current Dead Wood carbon pool */
	BIOMASS_STEM_LIVE_WOOD_tDM,         /* (tDM/cell) Current Live Stem carbon pool */
	BIOMASS_STEM_DEAD_WOOD_tDM,         /* (tDM/cell) Current Dead Stem carbon pool */
	BIOMASS_CROOT_LIVE_WOOD_tDM,        /* (tDM/cell) Current Live Coarse root carbon pool */
	BIOMASS_CROOT_DEAD_WOOD_tDM,        /* (tDM/cell) Current Dead Coarse root carbon pool */
	BIOMASS_STEM_BRANCH_LIVE_WOOD_tDM,  /* (tDM/cell) Current Live Stem carbon pool */
	BIOMASS_STEM_BRANCH_DEAD_WOOD_tDM,  /* (tDM/cell) Current Dead Stem carbon pool */

	/* (kgDM/tree) per tree in KgDM */
	AV_LEAF_MASS_KgDM,                  /* (kgDM/tree) Average Leaf carbon pool */
	AV_STEM_MASS_KgDM,                  /* (kgDM/tree) Average Leaf carbon pool */
	AV_FROOT_MASS_KgDM,                 /* (kgDM/tree) Average Fine root carbon pool */
	AV_CROOT_MASS_KgDM,                 /* (kgDM/tree) Average Coarse root carbon pool */
	AV_RESERVE_MASS_KgDM,               /* (kgDM/tree) Average Reserve carbon pool */
	AV_BRANCH_MASS_KgDM,                /* (kgDM/tree) Average Branch carbon pool */
	AV_LIVE_STEM_MASS_KgDM,             /* (kgDM/tree) Average Live Stem carbon pool */
	AV_DEAD_STEM_MASS_KgDM,             /* (kgDM/tree) Average Dead Stem carbon pool */
	AV_LIVE_CROOT_MASS_KgDM,            /* (kgDM/tree) Average Live Coarse root carbon pool */
	AV_DEAD_CROOT_MASS_KgDM,            /* (kgDM/tree) Average Dead Coarse root carbon pool */
	AV_LIVE_WOOD_MASS_KgDM,             /* (kgDM/tree) Average Live Wood carbon pool */
	AV_DEAD_WOOD_MASS_KgDM,             /* (kgDM/tree) Average Dead Wood carbon pool */
	AV_LIVE_BRANCH_MASS_KgDM,           /* (kgDM/tree) Average Live Branch carbon pool */
	AV_DEAD_BRANCH_MASS_KgDM,           /* (kgDM/tree) Average Dead Branch carbon pool */

	/* Above and Below Ground Biomass */
	AGB,                                /* (tC/cell) Above Ground Biomass pool */
	BGB,                                /* (tC/cell) Below Ground Biomass pool */
	DELTA_AGB,                          /* (tC/cell/year) Above Ground Biomass increment */
	DELTA_BGB,                          /* (tC/cell/year) Below Ground Biomass increment */
	TREE_AGB,                           /* (tC/tree) Above Ground Biomass pool */
	TREE_BGB,                           /* (tC/tree) Below Ground Biomass pool */
	DELTA_TREE_AGB,                     /* (tC/tree/year) Above Ground Biomass increment */
	DELTA_TREE_BGB,                     /* (tC/tree/year) Below Ground Biomass increment */

	/* converted fraction for turnover */
	WS_sap_tDM,                         /* (tDM/cell) Fraction Sapwood in Stem biomass */
	WS_heart_tDM,                       /* (tDM/cell) Fraction Heartwood in Stem biomass */
	WRC_sap_tDM,                        /* (tDM/cell) Fraction Sapwood in Coarse root biomass */
	WRC_heart_tDM,                      /* (tDM/cell) Fraction Heartwood in Coarse root biomass */
	WBB_sap_tDM,                        /* (tDM/cell) Fraction Sapwood in Branch biomass */
	WBB_heart_tDM,                      /* (tDM/cell) Fraction Heartwood in Branch biomass */
	WTOT_sap_tDM,                       /* (tDM/cell) Fraction Sapwood in Total biomass */

	/* converted fraction for allocation */
	FROOT_LEAF_FRAC,                    /* (DIM) Fraction fine root to leaf */
	STEM_LEAF_FRAC,                     /* (DIM) Fraction stem to leaf */
	CROOT_STEM_FRAC,                    /* (DIM) Fraction coarse root to stem */
	LIVE_TOTAL_WOOD_FRAC,               /* (DIM) Fraction live to total stem wood */
	FINE_COARSE_ROOT,                   /* (DIM) Fraction fine to coarse root */
	FRACBB,                             /* (DIM) Fraction branch and bark to stem */

	/* Maintenance respiration */
	DAILY_LEAF_MAINT_RESP,              /* (gC/m2/day) Daytime leaf maintenance respiration */
	NIGHTLY_LEAF_MAINT_RESP,            /* (gC/m2/day) Night time leaf maintenance respiration */
	TOT_DAY_LEAF_MAINT_RESP,            /* (gC/m2/day) Leaf maintenance respiration */
	FROOT_MAINT_RESP,                   /* (gC/m2/day) Fine root maintenance respiration */
	STEM_MAINT_RESP,                    /* (gC/m2/day) Stem maintenance respiration */
	CROOT_MAINT_RESP,                   /* (gC/m2/day) Coarse root maintenance respiration */
	BRANCH_MAINT_RESP,                  /* (gC/m2/day) Branch maintenance respiration */
	TOTAL_MAINT_RESP,                   /* (gC/m2/day) Total maintenance respiration */
	MONTHLY_TOTAL_MAINT_RESP,           /* (gC/m2/month) Monthly Total maintenance respiration */
	YEARLY_TOTAL_MAINT_RESP,            /* (gC/m2/year) Yearly Total maintenance respiration */
	TOTAL_MAINT_RESP_tC,                /* (tC/cell/day) Total maintenance respiration */

	/* Growth respiration */
	EFF_GRPERC,                         /* (DIM) Growth respiration ratio based on age */
	LEAF_GROWTH_RESP,                   /* (gC/m2/day) Leaf growth respiration */
	FROOT_GROWTH_RESP,                  /* (gC/m2/day) Fine root growth respiration */
	STEM_GROWTH_RESP,                   /* (gC/m2/day) Stem growth respiration */
	BRANCH_GROWTH_RESP,                 /* (gC/m2/day) Branch growth respiration */
	CROOT_GROWTH_RESP,                  /* (gC/m2/day) Coarse root growth respiration */
	FRUIT_GROWTH_RESP,                  /* (gC/m2/day) Fruit growth respiration */
	TOTAL_GROWTH_RESP,                  /* (gC/m2/day) Total growth respiration */
	MONTHLY_TOTAL_GROWTH_RESP,          /* (gC/m2/monthly) Monthly Total growth respiration */
	YEARLY_TOTAL_GROWTH_RESP,           /* (gC/m2/year) Yearly Total growth respiration */
	TOTAL_GROWTH_RESP_tC,               /* (tC/cell/day) Total growth respiration */

	/* Autotrophic respiration */
	LEAF_AUT_RESP,                      /* (gC/m2/day) Leaf autotrophic respiration */
	FROOT_AUT_RESP,                     /* (gC/m2/day) Fine root autotrophic respiration */
	STEM_AUT_RESP,                      /* (gC/m2/day) Stem autotrophic respiration */
	CROOT_AUT_RESP,                     /* (gC/m2/day) Coarse root autotrophic respiration */
	BRANCH_AUT_RESP,                    /* (gC/m2/day) Branch autotrophic respiration */
	FRUIT_AUT_RESP,                     /* (gC/m2/day) Fruit autotrophic respiration */
	TOTAL_AUT_RESP,                     /* (gC/m2/day) Total autotrophic respiration */
	MONTHLY_LEAF_AUT_RESP,              /* (gC/m2/month) Monthly Leaf autotrophic respiration */
	MONTHLY_FROOT_AUT_RESP,             /* (gC/m2/month) Monthly Fine root autotrophic respiration */
	MONTHLY_STEM_AUT_RESP,              /* (gC/m2/month) Monthly Stem autotrophic respiration */
	MONTHLY_CROOT_AUT_RESP,             /* (gC/m2/month) Monthly Coarse root autotrophic respiration */
	MONTHLY_FRUIT_AUT_RESP,             /* (gC/m2/month) Monthly Fruit autotrophic respiration */
	MONTHLY_BRANCH_AUT_RESP,            /* (gC/m2/month) Monthly Branch autotrophic respiration */
	MONTHLY_TOTAL_AUT_RESP,             /* (gC/m2/month) Monthly Total autotrophic respiration */
	YEARLY_LEAF_AUT_RESP,               /* (gC/m2/year) Annual Leaf autotrophic respiration */
	YEARLY_FROOT_AUT_RESP,              /* (gC/m2/year) Annual Fine root autotrophic respiration */
	YEARLY_STEM_AUT_RESP,               /* (gC/m2/year) Annual Stem autotrophic respiration */
	YEARLY_CROOT_AUT_RESP,              /* (gC/m2/year) Annual Coarse root autotrophic respiration */
	YEARLY_BRANCH_AUT_RESP,             /* (gC/m2/year) Annual Branch autotrophic respiration */
	YEARLY_FRUIT_AUT_RESP,              /* (gC/m2/year) Annual fRUIT autotrophic respiration */
	YEARLY_TOTAL_AUT_RESP,              /* (gC/m2/year) Annual Total autotrophic respiration */
	TOTAL_AUT_RESP_tC,                  /* (tC/cell/day) Total autotrophic respiration */

	/* nitrogen to nitrogen pools */
	N_TO_LEAF,                          /* (tN/cell/day) Daily Net Primary Production to Leaf pool */
	N_TO_ROOT,                          /* (tN/cell/day) Daily Net Primary Production to Root pool */
	N_TO_FROOT,                         /* (tN/cell/day) Daily Net Primary Production to Fine root pool */
	N_TO_CROOT,                         /* (tN/cell/day) Daily Net Primary Production to Coarse root pool */
	N_TO_TOT_STEM,                      /* (tN/cell/day) Daily Net Primary Production to Stem and Branch pool */
	N_TO_STEM,                          /* (tN/cell/day) Daily Net Primary Production to Stem pool */
	N_TO_BRANCH,                        /* (tN/cell/day) Daily Net Primary Production to Branch pool */
	N_TO_RESERVE,                       /* (tN/cell/day) Daily Net Primary Production to Reserve pool */
	N_TO_FRUIT,                         /* (tN/cell/day) Daily Net Primary Production to Fruit pool */
	N_TO_LITTER,                        /* (tN/cell/day) Daily Net Primary Production to Litter pool */
	N_TO_LITR,                          /* (tN/cell/day) Daily Litter to Litter nitrogen pool */
	LEAF_N_TO_REMOVE,                   /* (tN/cell/day) Daily Leaf to remove */
	FROOT_N_TO_REMOVE,                  /* (tN/cell/day) Daily Fine Root to remove */
	FRUIT_N_TO_REMOVE,                  /* (tN/cell/day) Daily Fruit to remove */
	BRANCH_N_TO_REMOVE,                 /* (tN/cell/day) Annual Branch to remove */
	N_TO_CWD,                           /* (tN/cell/day) Daily Woody Nitrogen to Coarse Woody Debris */
	N_LEAF_TO_LITR,                     /* (tN/cell/day) Daily Leaf Litter to Litter labile nitrogen pool */
	N_FROOT_TO_LITR,                    /* (tN/cell/day) Daily Leaf Litter to Litter labile nitrogen pool */
	//N_FRUIT_TO_LITR,                    /* (tN/cell/day) Daily Fruitr to Litter labile nitrogen pool */
	N_LEAF_TO_LITR1N,                   /* (tN/cell/day) Daily Leaf Litter to Litter labile nitrogen pool */
	N_LEAF_TO_LITR2N,                   /* (tN/cell/day) Daily Leaf Litter to Litter unshielded cellulose nitrogen pool */
	N_LEAF_TO_LITR3N,                   /* (tN/cell/day) Daily Leaf Litter to Litter shielded cellulose nitrogen pool */
	N_LEAF_TO_LITR4N,                   /* (tN/cell/day) Daily Leaf Litter to Litter lignin nitrogen pool */
	N_FROOT_TO_LITR1N,                  /* (tN/cell/day) Daily Fine Root Litter to Litter labile nitrogen pool */
	N_FROOT_TO_LITR2N,                  /* (tN/cell/day) Daily Fine Root Litter to Litter unshielded cellulose nitrogen pool */
	N_FROOT_TO_LITR3N,                  /* (tN/cell/day) Daily Fine Root Litter to Litter shielded cellulose nitrogen pool */
	N_FROOT_TO_LITR4N,                  /* (tN/cell/day) Daily Fine Root Litter to Litter lignin nitrogen pool */
	N_STEM_TO_CWD,                      /* (tN/cell/day) tN from Stem pool to Coarse Woody Debris */
	N_CROOT_TO_CWD,                     /* (tN/cell/day) tN from Coarse root pool to Coarse Woody Debris */
	N_BRANCH_TO_CWD,                    /* (tN/cell/day) tN from Branch pool to Coarse Woody Debris */
	N_RESERVE_TO_CWD,                   /* (tN/cell/day) tN from Reserve pool to Coarse Woody Debris */
	N_FRUIT_TO_CWD,                     /* (tN/cell/day) tN from Fruit pool to Coarse Woody Debris */
	N_LEAF_TO_RESERVE,                  /* (tN/cell/day) Daily retranslocated C from Leaf pool to Reserve */
	N_FROOT_TO_RESERVE,                 /* (tN/cell/day) Daily retranslocated C from Fine root pool to Reserve */
	N_BRANCH_TO_RESERVE,                /* (tN/cell/day) Annual retranslocated C from Branch pool to Reserve */
	N_STEM_LIVEWOOD_TO_DEADWOOD,        /* (tN/cell/day) Daily tN from Stem live wood pool to Stem dead wood */
	N_CROOT_LIVE_WOOD_TO_DEADWOOD,      /* (tN/cell/day) Daily tN from Coarse live wood pool to Coarse dead wood */
	N_BRANCH_LIVE_WOOD_TO_DEAD_WOOD,    /* (tN/cell/day) Daily tN from Branch live wood pool to Branch dead wood */


	/* nitrogen pools (tN/area) */
	LEAF_N,                             /* (tN/cell) Current Leaf nitrogen pool */
	LEAF_FALLING_N,                     /* (tN/cell) Current Leaf falling nitrogen pool */
	FROOT_N,                            /* (tN/cell) Current Fine root nitrogen pool */
	CROOT_N,                            /* (tN/cell) Current Coarse root nitrogen pool */
	CROOT_LIVE_WOOD_N,                  /* (tN/cell) Current Live Coarse root nitrogen pool */
	CROOT_DEAD_WOOD_N,                  /* (tN/cell) Current Dead Coarse root nitrogen pool */
	STEM_N,                             /* (tN/cell) Current Stem nitrogen pool */
	STEM_LIVE_WOOD_N,                   /* (tN/cell) Current Live Stem nitrogen pool */
	STEM_DEAD_WOOD_N,                   /* (tN/cell) Current Dead Stem nitrogen pool */
	BRANCH_N,                           /* (tN/cell) Current Branch nitrogen pool */
	BRANCH_LIVE_WOOD_N,                 /* (tN/cell) Current Live Branch nitrogen pool */
	BRANCH_DEAD_WOOD_N,                 /* (tN/cell) Current Dead Branch nitrogen pool */
	RESERVE_N,                          /* (tN/cell) Current Reserve nitrogen pool */
	FRUIT_N,                            /* (tN/cell) Current Friut nitrogen pool */
	TREE_N_DEMAND,                      /* (tN/cell) Current Nitrogen demand for new plant tissues */
	LITR_N,                             /* (tN/cell) Current Litter nitrogen pool */
	SOIL_N,                             /* (tN/cell) Current soil nitrogen pool */
	CWD_N,                              /* (tN/cell) Current Coarse Woody Debris nitrogen pool */
	TOT_STEM_N,                         /* (tN/cell) Current Total Stem nitrogen pool */
	TOT_ROOT_N,                         /* (tN/cell) Current Total root nitrogen pool */

	/* dead pools */
	DEAD_LEAF_N,                        /* (tN/cell) dead leaf flux/pool */
	DEAD_FROOT_N,                       /* (tN/cell) dead froot flux/pool */
	DEAD_STEM_N,                        /* (tN/cell) dead stem flux/pool */
	DEAD_CROOT_N,                       /* (tN/cell) dead croot flux/pool */
	DEAD_BRANCH_N,                      /* (tN/cell) dead branch flux/pool */
	DEAD_RESERVE_N,                     /* (tN/cell) dead reserve flux/pool */
	DEAD_FRUIT_N,                       /* (tN/cell) dead fruit flux/pool */
	DEAD_TO_LITRN,                      /* (tN/cell) dead pool to litter pool flux */
	DEAD_TO_CWDN,                       /* (tN/cell) dead pool to cwd pool flux */

	/* per tree in tN */
	AV_LEAF_MASS_N,                     /* (tN/tree) Average Leaf nitrogen pool */
	AV_STEM_MASS_N,                     /* (tN/tree) Average Stem nitrogen pool */
	AV_TOT_STEM_MASS_N,                 /* (tN/tree) Average Stem + Branch nitrogen pool */
	AV_ROOT_MASS_N,                     /* (tN/tree) Average Total root nitrogen pool */
	AV_FROOT_MASS_N,                    /* (tN/tree) Average Fine root nitrogen pool */
	AV_CROOT_MASS_N,                    /* (tN/tree) Average Coarse nitrogen pool */
	AV_BRANCH_MASS_N,                   /* (tN/tree) Average Branch nitrogen pool */
	AV_RESERVE_MASS_N,                  /* (tN/tree) Average Reserve nitrogen pool */
	AV_FRUIT_MASS_N,                    /* (tN/tree) Average Fruit nitrogen pool */

	/* litter and soil pools */
	CWDC,                               /* (tC/cell) coarse woody debris C */
	LITR1C,                             /* (tC/cell) litter labile C */
	LITR2C,                             /* (tC/cell) litter unshielded cellulose C */
	LITR3C,                             /* (tC/cell) litter shielded cellulose C */
	LITR4C,                             /* (tC/cell) litter lignin C */
	LITR1N,                             /* (tN/cell) litter labile N */
	LITR2N,                             /* (tN/cell) litter unshielded Cellulose N */
	LITR3N,                             /* (tN/cell) litter shielded Cellulose N */
	LITR4N,                             /* (tN/cell) litter lignin N */
	SOIL1C,                             /* (tC/cell) microbial recycling pool C (fast) */
	SOIL2C,                             /* (tC/cell) microbial recycling pool C (medium) */
	SOIL3C,                             /* (tC/cell) microbial recycling pool C (slow) */
	SOIL4C,                             /* (tC/cell) recalcitrant SOM C (humus, slowest) */
	SOIL1N,                             /* (tN/cell) microbial recycling pool N (fast) */
	SOIL2N,                             /* (tN/cell) microbial recycling pool N (medium) */
	SOIL3N,                             /* (tN/cell) microbial recycling pool N (slow) */
	SOIL4N,                             /* (tN/cell) recalcitrant SOM N (humus, slowest) */

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

	N_TREE,                             /* (tree/cell) Live Tree per class */
	DEAD_TREE,                          /* (tree/cell) Dead Tree per class */
	N_STUMP,                            /* (stump/cell) Number of stumps for coppice per class */
	N_SEED,                             /* (seeds/cell) Seeds number per class */
	N_TREE_SAP,                         /* (saplings/cell) Numbers of Saplings per class */
	VEG_UNVEG,                          /* (DIM) vegetative-un vegetative period per class */
	VEG_DAYS,                           /* (days/year) day of vegetative period for class if != 0 is in veg period */
	FIRST_VEG_DAYS,                     /* (DIM) first annual day of veg period */
	YEARLY_VEG_DAYS,                    /* (days/year) annual number of vegetative days */
	BUD_BURST_COUNTER,                  /* (days/year) days of budburst per class */
	DAYS_LEAFFALL,                      /* (days/year) days of leaf fall per class */
	DAY_VEG_FOR_LEAF_FALL,              /* (days/year) days for leaf fall */
	LEAF_FALL_COUNTER,                  /* (DIM) counter for leaf fall */
	SENESCENCE_DAY_ONE,                 /* (DIM) doy at first day of the senescence */
	DOS,                                /* (days) day of simulation */
	MOS,                                /* (months) month of simulation */
	YOS,                                /* (years) year of simulation */

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
	meteo_annual_t *years;

	int doy;                                                              /* day of the year */
	int dos;                                                              /* day of simulation */

	/* for logger */
	int initial_tree_layers_count;
	int initial_heights_count;

	/* overall cell counter */
	int cell_tree_layers_count;
	int cell_heights_count;
	int cell_dbhs_count;                                                  /* (dbh/cell) number of different dbh */
	int cell_ages_count;                                                  /* (ages/cell) number of different ages */
	int cell_species_count;                                               /* (species/cell) number of different species */

	int year_stand;                                                       /* input stand.txt row stand year */

	int x;                                                                /* cell index within the matrix */
	int y;                                                                /* cell index within the matrix */
	double elev;                                                          /* cell elevation (m) */
	int north;                                                            /* northern hemisphere north = 0, south hemisphere south = 1 */

	/* annual met values */
	double abscission_daylength;                                          /* (hrs/day) day length for starting abscission (not used) */

	/*forest structure variables*/	
	int Veg_Counter;                                                      /* number of class in vegetative period  in the cell */
	double cell_cover;                                                    /* (ratio) Canopy cover  */
	double bare_soil_cover;                                               /* (ratio) Bare soil cover */
	int cell_n_trees;                                                     /* (trees/cell/day) number of trees */
	int daily_dead_tree;                                                  /* (trees/cell/day) daily number of dead tree */
	int monthly_dead_tree;                                                /* (trees/cell/month) monthly number of dead tree */
	int annual_dead_tree ;                                                /* (trees/cell/year) annual number of dead tree */
	int saplings_counter;                                                 /* (trees/cell) number of class as sapling */
	double basal_area;                                                    /* (m2/cell) cumulated basal area at cell level */
	double agb;                                                           /* (tC/cell) current above ground biomass at cell level */
	double bgb;                                                           /* (tC/cell) current below ground biomass at cell level */
	double volume;                                                        /* (m3/cell) current volume */
	double cum_volume;                                                    /* (m3/cell) cumulated volume */

	/* radiation variables */
	/* short wave radiation */
	double sw_rad_down_MJ;                                                /* (MJ/m2/day) Downward short wave radiation flux */
	double short_wave_radiation_upward_MJ;                                /* (MJ/m2/day) Upward short wave radiation flux */
	double short_wave_radiation_upward_W;                                 /* (W/m2) Upward short wave radiation flux */
	double sw_rad_refl;                                                   /* (W/m2) Short Wave radiation reflected */
	double sw_rad_refl_soil;                                              /* (W/m2) Short Wave radiation reflected from soil level */
	double sw_rad_refl_snow;                                              /* (W/m2) (W/day)  Wave radiation reflected from snow level */
	double sw_rad_abs;                                                    /* (W/m2) Short Wave radiation absorbed tree level */
	double sw_rad_abs_soil;                                               /* (W/m2) Short Wave radiation absorbed soil level */
	double sw_rad_abs_snow;                                               /* (W/m2) Short Wave radiation absorbed snow level */
	double sw_rad_transm;                                                 /* (W/m2) Short Wave radiation transmitted */

	/* long wave radiation */
	double long_wave_radiation_upward_MJ;                                 /* (MJ/m2/day) Upward long wave radiation flux */
	double long_wave_radiation_upward_W;                                  /* (W/m2) Upward long wave radiation flux */
	double net_long_wave_radiation_MJ;                                    /* (MJ/m2/day) Net long wave radiation flux  */
	double net_long_wave_radiation_W;                                     /* (W/m2) Net long wave radiation flux */
	double long_wave_absorbed;                                            /* (W/m2) Total absorbed long wave wave radiation flux */
	double long_wave_reflected;                                           /* (W/m2) Total reflected long wave radiation flux */
	double long_wave_emitted;                                             /* (W/m2) Total emitted long wave radiation flux */
	double soil_long_wave_emitted;                                        /* (W/m2) Soil emitted long wave radiation flux */
	double net_lw_rad_for_soil;                                           /* (W/m2) Net Long Wave radiation to soil level */
	double net_rad_for_soil;                                              /* (W/m2) Net radiation to soil level */

	/* PAR radiation */
	double apar;                                                          /* (molPAR/m2/day) cumulated absorbed PAR at tree level */
	double apar_soil;                                                     /* (molPAR/m2/day) cumulated absorbed PAR at soil level */
	double apar_snow;                                                     /* (molPAR/m2/day) cumulated absorbed PAR at snow level */
	double par_transm;                                                    /* (molPAR/m2/day) cumulated transmitted PAR at cell level */
	double par_refl;                                                      /* (molPAR/m2/day) cumulated reflected PAR at cell level */
	double par_refl_soil;                                                 /* (molPAR/m2/day) reflected PAR by the soil */
	double par_refl_snow;                                                 /* (molPAR/m2/day) reflected PAR by the snow */

	/* PPFD radiation */
	double ppfd_abs;                                                      /* (umol/m2/sec) Absorbed Photosynthetic photon flux density tree level  */
	double ppfd_abs_soil;                                                 /* (umol/m2/sec) Absorbed Photosynthetic photon flux density soil level */
	double ppfd_abs_snow;                                                 /* (umol/m2/sec) Absorbed Photosynthetic photon flux density snow level */
	double ppfd_transm;                                                   /* (umol/m2/sec) Transmitted Photosynthetic photon flux density */
	double ppfd_refl;                                                     /* (umol/m2/sec) Reflected Photosynthetic photon flux density */
	double ppfd_refl_soil;                                                /* (umol/m2/sec) PPFD reflected by the soil */
	double ppfd_refl_snow;                                                /* (umol/m2/sec) PPFD reflected by the snow */

	/************************************************** forest trees **************************************************/

	double canopy_temp;                                                   /* (°C) daily canopy temperature */
	double canopy_temp_k;                                                 /* (K) daily canopy temperature */
	double canopy_temp_diff;                                              /* (K) daily canopy temperature */

	/* carbon fluxes */
	double daily_gpp, monthly_gpp, annual_gpp;                            /* (gC/m2/time) daily, monthly and annual GPP at cell level */
	double daily_gpp_tC, monthly_gpp_tC, annual_gpp_tC;                   /* (tC/m2/time) daily, monthly and annual GPP at cell level */
	double daily_npp, monthly_npp, annual_npp;                            /* (gC/m2/time) daily, monthly and annual NPP at cell level */
	double daily_npp_tC, monthly_npp_tC, annual_npp_tC;                   /* (tC/m2/time) daily, monthly and annual NPP at cell level */
	double daily_npp_tDM, monthly_npp_tDM, annual_npp_tDM;                /* (tDM/m2/time) daily, monthly and annual NPP at cell level */
	double daily_aut_resp, monthly_aut_resp, annual_aut_resp;             /* (gC/m2/time) daily, monthly and annual aut resp at cell level */
	double daily_aut_resp_tC, monthly_aut_resp_tC, annual_aut_resp_tC;    /* (tC/m2/time) daily, monthly and annual aut resp at cell level */
	double daily_maint_resp, monthly_maint_resp, annual_maint_resp;       /* (gC/m2/time) daily, monthly and annual maint resp at cell level */
	double daily_growth_resp, monthly_growth_resp, annual_growth_resp;    /* (gC/m2/time) daily, monthly and annual growth resp at cell level */
	double daily_r_eco, monthly_r_eco, annual_r_eco;                      /* (gC/m2/time) daily, monthly and annual ecosystem resp at cell level */
	double daily_het_resp, monthly_het_resp, annual_het_resp;             /* (gC/m2/time) daily, monthly and annual heterotrophic resp at cell level */
	double daily_C_flux, monthly_C_flux, annual_C_flux;                   /* (gC/m2/time) daily, monthly and annual carbon fluxes at cell level */
	double daily_nee, monthly_nee, annual_nee;                            /* (gC/m2/time) daily, monthly and annual NEE at cell level */
	double daily_leaf_carbon;                                             /* (gC/m2/day) daily carbon assimilated to c pool at cell level */
	double daily_stem_carbon;                                             /* (gC/m2/day) daily carbon assimilated to c pool at cell level */
	double daily_froot_carbon;                                            /* (gC/m2/day) daily carbon assimilated to c pool at cell level */
	double daily_croot_carbon;                                            /* (gC/m2/day) daily carbon assimilated to c pool at cell level */
	double daily_root_carbon;                                             /* (gC/m2/day) daily carbon assimilated to c pool at cell level */
	double daily_branch_carbon;                                           /* (gC/m2/day) daily carbon assimilated to c pool at cell level */
	double daily_reserve_carbon;                                          /* (gC/m2/day) daily carbon assimilated to c pool at cell level */
	double daily_fruit_carbon;                                            /* (gC/m2/day) daily carbon assimilated to fruit c pool at cell level */
	double daily_leaf_maint_resp;                                         /* (gC/m2/day) daily leaf maint resp at cell level */
	double daily_stem_maint_resp;                                         /* (gC/m2/day) daily stem maint resp at cell level */
	double daily_froot_maint_resp;                                        /* (gC/m2/day) daily fine root maint resp at cell level */
	double daily_branch_maint_resp;                                       /* (gC/m2/day) daily branch and bark maint resp at cell level */
	double daily_croot_maint_resp;                                        /* (gC/m2/day) daily coarse root maint resp at cell level */
	double daily_leaf_growth_resp;                                        /* (gC/m2/day) daily leaf growth resp at cell level */
	double daily_stem_growth_resp;                                        /* (gC/m2/day) daily stem growth resp at cell level */
	double daily_froot_growth_resp;                                       /* (gC/m2/day) daily fine root growth resp at cell level */
	double daily_branch_growth_resp;                                      /* (gC/m2/day) daily branch and bark growth resp at cell level */
	double daily_croot_growth_resp;                                       /* (gC/m2/day) daily coarse root growth resp at cell level */
	double daily_fruit_growth_resp;                                       /* (gC/m2/day) daily fruit growth resp at cell level */
	double daily_leaf_aut_resp;                                           /* (gC/m2/day) daily leaf aut resp at cell level */
	double daily_stem_aut_resp;                                           /* (gC/m2/day) daily stem aut resp at cell level */
	double daily_branch_aut_resp;                                         /* (gC/m2/day) daily branch and bark aut resp at cell level */
	double daily_froot_aut_resp;                                          /* (gC/m2/day) daily fine root aut resp at cell level */
	double daily_croot_aut_resp;                                          /* (gC/m2/day) daily coarse root aut resp at cell level */
	double daily_fruit_aut_resp;                                          /* (gC/m2/day) daily fruit aut resp at cell level */

	/* tree carbon pools */
	double leaf_carbon;                                                   /* (gC/m2) leaf carbon at cell level */
	double froot_carbon;                                                  /* (gC/m2) fine root carbon at cell level */
	double stem_carbon;                                                   /* (gC/m2) stem carbon at cell level */
	double stem_live_wood_carbon;                                         /* (gC/m2) stem live wood carbon at cell level */
	double stem_dead_wood_carbon;                                         /* (gC/m2) stem dead wood carbon at cell level */
	double tot_stem_carbon;                                               /* (gC/m2) total stem carbon at cell level */
	double croot_carbon;                                                  /* (gC/m2) coarse root carbon at cell level */
	double croot_live_wood_carbon;                                        /* (gC/m2) coarse root live wood carbon at cell level */
	double croot_dead_wood_carbon;                                        /* (gC/m2) coarse root dead wood carbon at cell level */
	double tot_root_carbon;                                               /* (gC/m2) total root carbon at cell level */
	double branch_carbon;                                                 /* (gC/m2) branch carbon at cell level */
	double branch_live_wood_carbon;                                       /* (gC/m2) branch live wood carbon at cell level */
	double branch_dead_wood_carbon;                                       /* (gC/m2) branch dead wood carbon at cell level */
	double reserve_carbon;                                                /* (gC/m2) reserve at cell level */
	double fruit_carbon;                                                  /* (gC/m2) fruit at cell level */

	/* tree nitrogen pools */
	double leaf_nitrogen;                                                 /* (gN/m2) leaf carbon at cell level */
	double froot_nitrogen;                                                /* (gN/m2) fine root carbon at cell level */
	double stem_nitrogen;                                                 /* (gN/m2) stem carbon at cell level */
	double stem_live_wood_nitrogen;                                       /* (gN/m2) stem live wood carbon at cell level */
	double stem_dead_wood_nitrogen;                                       /* (gN/m2) stem dead wood carbon at cell level */
	double tot_stem_nitrogen;                                             /* (gN/m2) total stem carbon at cell level */
	double croot_nitrogen;                                                /* (gN/m2) coarse root carbon at cell level */
	double croot_live_wood_nitrogen;                                      /* (gN/m2) coarse root live wood carbon at cell level */
	double croot_dead_wood_nitrogen;                                      /* (gN/m2) coarse root dead wood carbon at cell level */
	double tot_root_nitrogen;                                             /* (gN/m2) total root carbon at cell level */
	double branch_nitrogen;                                               /* (gN/m2) branch carbon at cell level */
	double branch_live_wood_nitrogen;                                     /* (gN/m2) branch live wood carbon at cell level */
	double branch_dead_wood_nitrogen;                                     /* (gN/m2) branch dead wood carbon at cell level */
	double reserve_nitrogen;                                              /* (gN/m2) reserve at cell level */
	double fruit_nitrogen;                                                /* (gN/m2) fruit at cell level */

	/* carbon use efficiency */
	double daily_cue;                                                     /* (DIM) daily carbon use efficiency */
	double monthly_cue;                                                   /* (DIM) monthly carbon use efficiency */
	double annual_cue;                                                    /* (DIM) annual carbon use efficiency */

	/* water use efficiency */
	double daily_wue;                                                     /* (DIM) daily water use efficiency */
	double daily_iwue;                                                    /* (DIM) daily intrinsic water use efficiency */
	double monthly_wue;                                                   /* (DIM) monthly water use efficiency */
	double monthly_iwue;                                                  /* (DIM) monthly intrinsic water use efficiency */
	double annual_wue;                                                    /* (DIM) annual water use efficiency */
	double annual_iwue;                                                   /* (DIM) annual intrinsic water use efficiency */

	/* energy fluxes */
	double daily_canopy_transp_watt;                                      /* (W/m2) daily canopy transpiration at cell level */
	double daily_canopy_evapo_watt;                                       /* (W/m2) daily canopy evaporation at cell level */
	double daily_canopy_et_watt;                                          /* (W/m2) daily canopy evapotranspiration at cell level */
	double daily_canopy_lh_flux;                                          /* (W/m2) daily canopy latent heat flux at cell level  */
	double daily_canopy_sh_flux;                                          /* (W/m2) current daily canopy sensible heat flux */
	double daily_lh_flux, monthly_lh_flux, annual_lh_flux;                /* (W/m2) daily, monthly and annual latent heat flux at cell level */
	double daily_sh_flux, monthly_sh_flux, annual_sh_flux;                /* (W/m2) daily, monthly and annual sensible heat flux at cell level */

	/************************************************** litter and soil **************************************************/

	/* soil scalars */
	double tsoil_scalar;                                                  /* (DIM) soil temperature scalar for decomposition */
	double wsoil_scalar;                                                  /* (DIM) soil water scalar  for decomposition */
	double rate_scalar;                                                   /* (DIM) soil (temperature * water) scalar for decomposition */
	double daily_gross_nmin;                                              /* (tN/cell/m2) daily gross N mineralization */
	double daily_gross_nimmob;                                            /* (tN/cell/m2) daily gross N immobilization */
	double daily_net_nmin;                                                /* (tN/cell/m2) daily net N mineralization */
	double fpi;                                                           /* (DIM) fraction of potential immobilization */

	/* litter and soil decomposition carbon fluxes */
	/* litter */
	double daily_litrC;                                                   /* (gC/m2/day) daily litter carbon pool at cell level */
	double daily_leaf_litrC;                                              /* (gC/m2/day) daily leaf to litter carbon pool at cell level */
	double daily_leaf_litr1C;                                             /* (gC/m2/day) daily leaf to litter labile carbon pool at cell level */
	double daily_leaf_litr2C;                                             /* (gC/m2/day) daily leaf to litter unshelded cellulose carbon pool at cell level */
	double daily_leaf_litr3C;                                             /* (gC/m2/day) daily leaf to litter  shelded cellulose carbon pool at cell level */
	double daily_leaf_litr4C;                                             /* (gC/m2/day) daily leaf to litter lignin carbon pool at cell level */
	double daily_froot_litrC;                                             /* (gC/m2/day) daily fine root to litter carbon pool at cell level */
	double daily_froot_litr1C;                                            /* (gC/m2/day) daily fine root to litter labile carbon pool at cell level */
	double daily_froot_litr2C;                                            /* (gC/m2/day) daily fine root to litter unshelded cellulose carbon pool at cell level */
	double daily_froot_litr3C;                                            /* (gC/m2/day) daily fine root to litter shelded cellulose carbon pool at cell level */
	double daily_froot_litr4C;                                            /* (gC/m2/day) daily fine root to litter lignin carbon pool at cell level */
	double daily_litr1C_to_soil1C;                                        /* (gC/m2/day) daily decomposition fluxes litter to soil */
	double daily_litr2C_to_soil2C;                                        /* (gC/m2/day) daily decomposition fluxes litter to soil */
	double daily_litr3C_to_litr2C;                                        /* (gC/m2/day) daily decomposition fluxes litter to soil */
	double daily_litr4C_to_soil3C;                                        /* (gC/m2/day) daily decomposition fluxes litter to soil */
	/* cwd */
	double daily_cwdC;                                                    /* (gC/m2/day) daily cwd carbon pool at cell level */
	double daily_stem_cwdC;                                               /* (gC/m2/day) daily stem carbon to Coarse Woody Debris */
	double daily_croot_cwdC;                                              /* (gC/m2/day) daily croot carbon to Coarse Woody Debris */
	double daily_reserve_cwdC;                                            /* (gC/m2/day) daily reserve carbon to Coarse Woody Debris */
	double daily_branch_cwdC;                                             /* (gC/m2/day) daily branch carbon to Coarse Woody Debris */
	double daily_fruit_cwdC;                                              /* (gC/m2/day) daily fruit to CWD lignin carbon pool at cell level */
	double daily_cwdc_to_litr2C;                                          /* (gC/m2/day) daily decomposition fluxes cwd to litter */
	double daily_cwdc_to_litr3C;                                          /* (gC/m2/day) daily decomposition fluxes cwd to litter */
	double daily_cwdc_to_litr4C;                                          /* (gC/m2/day) daily decomposition fluxes cwd to litter */
	/* soil */
	double daily_soilC;                                                   /* (gC/m2/day) daily litter carbon to soil */
	double daily_soil1C_to_soil2C;                                        /* (gC/m2/day) daily decomposition fluxes soil to soil */
	double daily_soil2C_to_soil3C;                                        /* (gC/m2/day) daily decomposition fluxes soil to soil */
	double daily_soil3C_to_soil4C;                                        /* (gC/m2/day) daily decomposition fluxes soil to soil */

	/* litter and soil nitrogen fluxes */
	/* litter */
	double daily_litrN;                                                   /* (gN/m2/day) daily leaf + fine root to litter nitrogen pool at cell level */
	double daily_leaf_litrN;                                              /* (gN/m2/day) daily leaf to litter nitrogen pool at cell level */
	double daily_leaf_litr1N;                                             /* (gN/m2/day) daily leaf to litter labile nitrogen pool at cell level */
	double daily_leaf_litr2N;                                             /* (gN/m2/day) daily leaf to litter unshelded cellulose nitrogen pool at cell level */
	double daily_leaf_litr3N;                                             /* (gN/m2/day) daily leaf to litter  shelded cellulose nitrogen pool at cell level */
	double daily_leaf_litr4N;                                             /* (gN/m2/day) daily leaf to litter lignin nitrogen pool at cell level */
	double daily_froot_litrN;                                             /* (gN/m2/day) daily fine root to litter nitrogen pool at cell level */
	double daily_froot_litr1N;                                            /* (gN/m2/day) daily fine root to litter labile nitrogen pool at cell level */
	double daily_froot_litr2N;                                            /* (gN/m2/day) daily fine root to litter unshelded cellulose nitrogen pool at cell level */
	double daily_froot_litr3N;                                            /* (gN/m2/day) daily fine root to litter shelded cellulose nitrogen pool at cell level */
	double daily_froot_litr4N;                                            /* (gN/m2/day) daily fine root to litter lignin nitrogen pool at cell level */
	double daily_litr1N_to_soil1N;                                        /* (gN/m2/day) daily decomposition fluxes litter to soil */
	double daily_litr2N_to_soil2N;                                        /* (gN/m2/day) daily decomposition fluxes litter to soil */
	double daily_litr3N_to_litr2N;                                        /* (gN/m2/day) daily decomposition fluxes litter to soil */
	double daily_litr4N_to_soil3N;                                        /* (gN/m2/day) daily decomposition fluxes litter to soil */
	/* cwd */
	double daily_cwdN;                                                    /* (gN/m2/day) daily cwd nitrogen pool at cell level */
	double daily_stem_cwdN;                                               /* (gN/m2/day) daily stem nitrogen to Coarse Woody Debris */
	double daily_croot_cwdN;                                              /* (gN/m2/day) daily croot nitrogen to Coarse Woody Debris */
	double daily_reserve_cwdN;                                            /* (gN/m2/day) daily reserve nitrogen to Coarse Woody Debris */
	double daily_branch_cwdN;                                             /* (gN/m2/day) daily branch nitrogen to Coarse Woody Debris */
	double daily_fruit_cwdN;                                              /* (gN/m2/day) daily fruit to cwd nitrogen pool at cell level */
	double daily_cwdc_to_litr2N;                                          /* (gN/m2/day) daily decomposition fluxes cwd to litter */
	double daily_cwdc_to_litr3N;                                          /* (gN/m2/day) daily decomposition fluxes cwd to litter */
	double daily_cwdc_to_litr4N;                                          /* (gN/m2/day) daily decomposition fluxes cwd to litter */
	/* soil */
	double daily_soilN;                                                   /* (gN/m2/day) daily litter nitrogen to soil */
	double daily_soil1N_to_soil2N;                                        /* (gN/m2/day) daily decomposition fluxes soil to soil */
	double daily_soil2N_to_soil3N;                                        /* (gN/m2/day) daily decomposition fluxes soil to soil */
	double soil3N_to_soil4N;                                              /* (gN/m2/day) daily decomposition fluxes soil to soil */

	/* litter and soil carbon pools */
	double litrC;                                                         /* (gC/m2) litter carbon */
	double litr1C;                                                        /* (gC/m2) litter labile carbon */
	double litr2C;                                                        /* (gC/m2) litter unshielded cellulose carbon */
	double litr3C;                                                        /* (gC/m2) litter shielded cellulose carbon */
	double litr4C;                                                        /* (gC/m2) litter lignin carbon */
	double leaf_litrC;                                                    /* (gC/m2) leaf total litter carbon */
	double leaf_litr1C;                                                   /* (gC/m2) leaf litter labile carbon */
	double leaf_litr2C;                                                   /* (gC/m2) leaf litter unshielded cellulose carbon */
	double leaf_litr3C;                                                   /* (gC/m2) leaf litter shielded cellulose carbon */
	double leaf_litr4C;                                                   /* (gC/m2) leaf litter lignin carbon */
	double froot_litrC;                                                   /* (gC/m2) fine root total litter carbon */
	double froot_litr1C;                                                  /* (gC/m2) fine root litter labile carbon */
	double froot_litr2C;                                                  /* (gC/m2) fine root litter unshielded cellulose carbon */
	double froot_litr3C;                                                  /* (gC/m2) fine root litter shielded cellulose carbon */
	double froot_litr4C;                                                  /* (gC/m2) fine root litter lignin carbon */
	double cwdC;                                                          /* (gC/m2) cwd carbon */
	double stem_cwdC;                                                     /* (gC/m2) stem total cwd carbon */
	double croot_cwdC;                                                    /* (gC/m2) croot total cwd carbon */
	double reserve_cwdC;                                                  /* (gC/m2) reserve total cwd carbon */
	double branch_cwdC;                                                   /* (gC/m2) branch total cwd carbon */
	double fruit_cwdC;                                                    /* (gC/m2) fruit total cwd carbon */
	double deadwood_litr1C;                                               /* (gC/m2) deadwood litter labile carbon */
	double deadwood_litr2C;                                               /* (gC/m2) deadwood litter unshielded cellulose carbon */
	double deadwood_litr3C;                                               /* (gC/m2) deadwood litter shielded cellulose carbon */
	double deadwood_litr4C;                                               /* (gC/m2) deadwood litter lignin carbon */
	double soilC;                                                         /* (gC/m2) Soil at cell level */
	double soil1C;                                                        /* (gC/m2) microbial recycling pool carbon (fast) */
	double soil2C;                                                        /* (gC/m2) microbial recycling pool carbon (medium) */
	double soil3C;                                                        /* (gC/m2) microbial recycling pool carbon (slow) */
	double soil4C;                                                        /* (gC/m2) recalcitrant SOM carbon (humus, slowest) */

	/* litter and soil nitrogen pools */
	double litrN;                                                         /* (gN/m2) litter nitrogen */
	double litr1N;                                                        /* (gN/m2) litter labile nitrogen */
	double litr2N;                                                        /* (gN/m2) litter unshielded cellulose nitrogen */
	double litr3N;                                                        /* (gN/m2) litter shielded cellulose nitrogen */
	double litr4N;                                                        /* (gN/m2) litter lignin nitrogen */
	double leaf_litrN;                                                    /* (gN/m2) leaf total litter nitrogen */
	double leaf_litr1N;                                                   /* (gN/m2) leaf litter labile nitrogen */
	double leaf_litr2N;                                                   /* (gN/m2) leaf litter unshielded cellulose nitrogen */
	double leaf_litr3N;                                                   /* (gN/m2) leaf litter shielded cellulose nitrogen */
	double leaf_litr4N;                                                   /* (gN/m2) leaf litter lignin nitrogen */
	double froot_litrN;                                                   /* (gN/m2) fine root total litter nitrogen */
	double froot_litr1N;                                                  /* (gN/m2) fine root litter labile nitrogen */
	double froot_litr2N;                                                  /* (gN/m2) fine root litter unshielded cellulose nitrogen */
	double froot_litr3N;                                                  /* (gN/m2) fine root litter shielded cellulonitrogenN */
	double froot_litr4N;                                                  /* (gN/m2) fine root litter lignin nitrogen */
	double cwdN;                                                          /* (gN/m2) coarse woody debris nitrogen */
	double stem_cwdN;                                                     /* (gN/m2) stem total cwd nitrogen */
	double croot_cwdN;                                                    /* (gN/m2) croot total cwd nitrogen */
	double reserve_cwdN;                                                  /* (gN/m2) reserve total cwd nitrogen */
	double branch_cwdN;                                                   /* (gN/m2) branch total cwd nitrogen */
	double fruit_cwdN;                                                    /* (gN/m2) fruit total cwd nitrogen */
	double deadwood_litr1N;                                               /* (gN/m2) deadwood litter labile nitrogen */
	double deadwood_litr2N;                                               /* (gN/m2) deadwood litter unshielded cellulose nitrogen */
	double deadwood_litr3N;                                               /* (gN/m2) deadwood litter shielded cellulose nitrogen */
	double deadwood_litr4N;                                               /* (gN/m2) deadwood litter lignin nitrogen */
	double soilN;                                                         /* (gN/m2) soil nitrogen */
	double soil1N;                                                        /* (gN/m2) microbial recycling pool nitrogen (fast) */
	double soil2N;                                                        /* (gN/m2) microbial recycling pool nitrogen (medium) */
	double soil3N;                                                        /* (gN/m2) microbial recycling pool nitrogen (slow) */
	double soil4N;                                                        /* (gN/m2) recalcitrant SOM nitrogen (humus, slowest) */
	double SoilMinN;                                                      /* (gN/m2) Soil mineral nitrogen */
	double Npool;                                                         /* (gN/m2) temporary plant nitrogen pool */
	double Nfix;                                                          /* (gN/m2) SUM of biological nitrogen fixation */
	double Nleach;                                                        /* (gN/m2) SUM of nitrogen lost for leacheing */
	double Nvol;                                                          /* (gN/m2) SUM of nitrogen lost for volatilization  */

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
	double litr1_rh;                                                      /* (kgC/m2/day) heterotrophic respiration for litter labile carbon */
	double litr2_rh;                                                      /* (kgC/m2/day) heterotrophic respiration for litter unshielded cellulose carbon */
	double litr3_rh;                                                      /* (kgC/m2/day) heterotrophic respiration for litter shielded cellulose carbon */
	double litr4_rh;                                                      /* (kgC/m2/day) heterotrophic respiration for litter lignin carbon */
	double soil1_rh;                                                      /* (kgC/m2/day) heterotrophic respiration for microbial recycling pool carbon (fast) */
	double soil2_rh;                                                      /* (kgC/m2/day) heterotrophic respiration for microbial recycling pool carbon (medium) */
	double soil3_rh;                                                      /* (kgC/m2/day) heterotrophic respiration for microbial recycling pool carbon (slow) */
	double soil4_rh;                                                      /* (kgC/m2/day) heterotrophic respiration for recalcitrant SOM carbon (humus, slowest) */

	/* soil fraction */
	double leaf_litt_scel_frac;                                           /* (DIM) leaf litter shielded cellulose fraction */
	double leaf_litt_uscel_frac;                                          /* (DIM) leaf litter unshielded cellulose fraction */
	double froot_litt_scel_frac;                                          /* (DIM) fine root litter shielded cellulose fraction */
	double froot_litt_uscel_frac;                                         /* (DIM) fine root litter unshielded fraction */
	double dead_wood_scel_frac;                                           /* (DIM) dead wood litter shielded cellulose fraction */
	double dead_wood_uscel_frac;                                          /* (DIM) dead wood litter unshielded fraction */

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
	int days_since_rain;                                                  /* (days) consecutive days without rain */

	/* pools */
	double asw;                                                           /* (mm/volume) current available soil water  */
	double max_asw_fc;                                                    /* (mmKgH2O/m3) max available soil water at field capacity */
	double max_asw_sat;                                                   /* (mmKgH2O/m3) max available soil water at field capacity */
	double snow_pack;                                                     /* (Kg/m2)current amount of snow */
	double canopy_water_stored;                                           /* (mm/m2) canopy water stored at cell level */

	/* fluxes */
	double daily_snow_melt;                                               /* (mm/m2/time) current amount of melted snow  */
	double daily_snow_subl;                                               /* (mm/m2/time) current amount of sublimated snow */
	double daily_out_flow;                                                /* (mm/m2/time) current amount of water outflow */
	double daily_canopy_rain_int, monthly_canopy_rain_int, annual_canopy_rain_int; /* (mm/m2/time) daily, monthly and canopy rain interception */
	double daily_canopy_snow_int, monthly_canopy_snow_int, annual_canopy_snow_int; /* (mm/m2/time) daily, monthly and canopy snow interception */
	double daily_canopy_transp, monthly_canopy_transp, annual_canopy_transp; /* (mm/m2/time) daily, monthly and canopy transpiration */
	double daily_canopy_evapo, monthly_canopy_evapo, annual_canopy_evapo; /* (mm/m2/time) daily, monthly and canopy evaporation */
	double daily_canopy_et, monthly_canopy_et, annual_canopy_et;          /* (mm/m2/time) daily, monthly and canopy evapotranspiration */
	double daily_soil_evapo, monthly_soil_evapo, annual_soil_evapo;       /* (mm/m2/time) daily, monthly and soil evaporation */
	double daily_et, monthly_et, annual_et;                               /* (mm/m2/time) daily, monthly and evapotranspiration */
	double daily_tot_w_flux, monthly_tot_w_flux, annual_tot_w_flux;       /* (mm/m2/time) daily, monthly and annual water fluxes */
	double daily_soil_evapo_watt;                                         /* (W/m2) current daily soil evaporation */
	double daily_soil_lh_flux;                                            /* (W/m2) current daily soil latent heat flux */
	double daily_soil_sh_flux;                                            /* (W/m2) current daily soil sensible heat flux */

	/************************************************** BALANCES VARIABLES **************************************************/
	/* cell */
	double cell_carbon_in;                                                /* (gC/day/day) cell carbon balances in */
	double cell_carbon_out;                                               /* (gC/day/day) cell carbon balances out */
	double cell_carbon_store;                                             /* (gC/day) cell carbon balances store */
	double cell_carbon_old_store;                                         /* (gC/day) cell carbon balances old_store */
	double cell_carbon_balance;                                           /* (gC/day/day) cell carbon balances balance */

	double cell_carbontc_in;                                              /* (tC/cell/day) cell carbon balances in */
	double cell_carbontc_out;                                             /* (tC/cell/day) cell carbon balances out */
	double cell_carbontc_store;                                           /* (tC/cell) cell carbon balances store */
	double cell_carbontc_old_store;                                       /* (tC/cell) cell carbon balances old_store */
	double cell_carbontc_balance;                                         /* (tC/cell/day) cell carbon balances balance */

	double cell_water_in;                                                 /* (mm/day/day) cell water balances in */
	double cell_water_out;                                                /* (mm/day/day) cell water balances out */
	double cell_water_store;                                              /* (mm/day) cell water balances store */
	double cell_water_old_store;                                          /* (mm/day) cell water balances old_store */
	double cell_water_balance;                                            /* (mm/day/day) cell water balances balance */

	double cell_snow_in;                                                  /* (mm/day/day) cell snow balances in */
	double cell_snow_out;                                                 /* (mm/day/day) cell snow balances out */
	double cell_snow_store;                                               /* (mm/day) cell snow balances store */
	double cell_snow_old_store;                                           /* (mm/day) cell snow balances old_store */
	double cell_snow_balance;                                             /* (mm/day/day) cell snow balances balance */

	double cell_nitrogen_in;                                              /* (gN/day/day) cell nitrogen balances in */
	double cell_nitrogen_out;                                             /* (gN/day/day) cell nitrogen balances out */
	double cell_nitrogen_store;                                           /* (gN/day) cell nitrogen balances store */
	double cell_nitrogen_old_store;                                       /* (gN/day) cell nitrogen balances old_store */
	double cell_nitrogen_balance;                                         /* (gN/day/day) cell nitrogen balances balance */

	double cell_nitrogentc_in;                                            /* (tN/cell/day) cell nitrogen balances in */
	double cell_nitrogentc_out;                                           /* (tN/cell/day) cell nitrogen balances out */
	double cell_nitrogentc_store;                                         /* (tN/cell) cell nitrogen balances store */
	double cell_nitrogentc_old_store;                                     /* (tN/cell/day) cell nitrogen balances old_store */
	double cell_nitrogentc_balance;                                       /* (tN/cell/day) cell nitrogen balances balance */

	/* litter */
	double litr_carbon_in;                                                /* (gC/day/day) cell carbon balances in */
	double litr_carbon_out;                                               /* (gC/day/day) cell carbon balances out */
	double litr_carbon_store;                                             /* (gC/day) cell carbon balances store */
	double litr_carbon_old_store;                                         /* (gC/day) cell carbon balances old_store */
	double litr_carbon_balance;                                           /* (gC/day/day) cell carbon balances balance */

	double litr_nitrogen_in;                                              /* (gN/day/day) cell nitrogen balances in */
	double litr_nitrogen_out;                                             /* (gN/day/day) cell nitrogen balances out */
	double litr_nitrogen_store;                                           /* (gN/day) cell nitrogen balances store */
	double litr_nitrogen_old_store;                                       /* (gN/day) cell nitrogen balances old_store */
	double litr_nitrogen_balance;                                         /* (gN/day/day) cell nitrogen balances balance */

	/* soil */
	double soil_carbon_in;                                                /* (gC/day/day) cell carbon balances in */
	double soil_carbon_out;                                               /* (gC/day/day) cell carbon balances out */
	double soil_carbon_store;                                             /* (gC/day) cell carbon balances store */
	double soil_carbon_old_store;                                         /* (gC/day) cell carbon balances old_store */
	double soil_carbon_balance;                                           /* (gC/day/day) cell carbon balances balance */

	double soil_nitrogen_in;                                              /* (gN/day/day) cell nitrogen balances in */
	double soil_nitrogen_out;                                             /* (gN/day/day) cell nitrogen balances out */
	double soil_nitrogen_store;                                           /* (gN/day) cell nitrogen balances store */
	double soil_nitrogen_old_store;                                       /* (gN/day) cell nitrogen balances old_store */
	double soil_nitrogen_balance;                                         /* (gN/day/day) cell nitrogen balances balance */

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
