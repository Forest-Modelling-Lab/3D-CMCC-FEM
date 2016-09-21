/* constants.h */
#ifndef CONSTANTS_H_
#define CONSTANTS_H_

enum {
	JANUARY
	, FEBRUARY
	, MARCH
	, APRIL
	, MAY
	, JUNE
	, JULY
	, AUGUST
	, SEPTEMBER
	, OCTOBER
	, NOVEMBER
	, DECEMBER

	, MONTHS_COUNT
};

#define NO_DATA                -9999
/* constants */
#define Q0                    1376.1      /* Solar constant (W/m^2) */
#define Q0_MJ                  0.082      /* Solar constant (MJ/m^2/day) */
#define QA                       -90      /* Intercept of Net vs Solar Radiation (W/m^2) */
#define QB                       0.8      /* Slope of Net vs Solar Radiation */
#define RAD2PAR                 0.45      /* (DIM) ratio PAR / SWtotal */
#define RAD2PPFD                0.45      /* (DIM) ratio PPFD / SWtotal */
#define GC_GDM                     2      /* convert grams C to grams DM (Campioli et al., 2013) */
#define GLUCOSE_C                0.4      /* fraction of Carbon in glucose reserve (Barbaroux et al., 2002) */
#define MOLPAR_MJ                2.3      /* Conversion of Solar Radiation to PAR - CONVERT MJ TO MOLPAR -- 1 MJ = 2.3 molPAR */
#define GDM_MOL                   24      /* Molecular weight of dry matter - CONVERT molC TO gDM (12 Molecular weight of C * 2 to have DM)(IPCC guidelines 1996) */
#define GC_MOL                    12      /* Convert molC to grams of C */
#define MJ_TO_W         11.574074074      /* Convert MJ/m2/day to W/m2 */
#define W_TO_MJ               0.0864      /* Convert W/m2 to MJ/m2/day */
#define W_MJ                 1000000      /* Convert Watt to MegaJoule/sec m^2 */
#define SBC_W              5.6696e-8      /* (W/(m^2 K^4)) Stefan-Boltzmann constant */
#define SBC_MJ              4.903e-9      /* (MJ/K^4/m^2/day) Stefan-Boltzmann constant in MJ day */
#define CP                    1010.0      /* (J/kg K) specific heat of air */
#define G_STD                9.80665      /* (m/s2) standard gravitational accel. */
#define P_STD               101325.0      /* (Pa) standard pressure at 0.0 m elevation */
#define T_STD                 288.15      /* (K) standard temp at 0.0 m elevation  */
#define MA                28.9644e-3      /* (kg/mol) molecular weight of air */
#define MW                18.0148e-3      /* (kg/mol) molecular weight of water */
#define LR_STD                0.0065      /* (-K/m) standard temperature lapse rate */
#define Rgas                  8.3144      /* (m3 Pa/ mol K) gas law constant */
#define EPS                   0.6219      /* (MW/MA) unitless ratio of molec weights */
#define ESTAR                0.61076      /* to compute saturation vapour pressure at the air temperature (KPa) */
#define PPFD50                  75.0      /* (umol/m2/s) PPFD for 1/2 stomatal closure */
#define EPAR                    4.55      /* (umol/J OR mol/MJ) PAR photon energy ratio */
#define SLAMKG                   0.1	  /* Convert SLA IN cm ^2/gC in m^2/KgC */
#define E20                      2.2      /* rate of change of saturated VPD with T at 20C */
#define VPDCONV             0.000622      /* convert VPD to saturation deficit = 18/29/1000 */
#define EVAPOCOEFF              1.32      /* Priestley Taylor Coefficient (Hobbins et al., 2001) */
#define MWratio                0.622      /* ratio molecular weight of water vapour air */
#define TempAbs               273.13      /* absolute temperature in Kelvin */
#define GDD_BASIS                  5      /* temperature C° basis for thermic sum */
#define EMLEAF                  0.95      /* Emissivity of thermal radiation by leaf (see MAESPA) */
#define EMCANOPY               0.987      /* Emissivity of thermal radiation by canopy (see Campbell & Norman, Environmental Biophysics, 2nd edition, page 273) */
#define EMSOIL                  0.95      /* Emissivity of thermal radiation by soil (see MAESPA) */
#define Pi               3.141592654      /* Pi greek */
#define ln2              0.693147181      /* logarithm constant */
#define eps                     1e-6      /* differences in comparison */
#define e                 2.71828183      /* nepero/eulero constant */
#define GRPERC                   0.3      /* (DIM) percentage of growth resp per unit of C grown see BIOME, 0.25 for LPJ */
#define refCO2CONC               380 /*281 */     /* ppmv reference co2 concentration see Veroustraete 1994, 2002 */
#define O2CONC                  20.9      /* % of O2 concentration see Verstraeten 1994, 2002 */
#define STEMPOWER_A	            2.08      /* Power in the Stem Mass v. diameter relationship for DBH < 9 cm */
#define STEMPOWER_B	            2.64      /* Power in the Stem Mass v. diameter relationship for 9 < DBH < 15 cm */
#define STEMPOWER_C	            2.51      /* Power in the Stem Mass v. diameter relationship for DBH > 15 cm */

#define MAX_N_TREE_LAYER           3      /* define the maximum number for tree layers (todo move into setting.txt) */

//MARCONI CONSTANTS::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//soil constants
#define DRF                     0.04      //0.03
#define EFFRB                    .67
#define EFFAC                     .2
#define RBO                     0.02
#define SRB                      0.9
#define KRCVL                   0.25
#define KRCL                   0.074
#define KRCR                    0.02
#define KRB                     0.12
#define HRB                     0.04
#define EFFNO                    0.2
//#define um_no3   .67
//#define um_no2   .67
//#define um_no    .34
//#define um_n2o   .34

#endif /* CONSTANTS_H */
