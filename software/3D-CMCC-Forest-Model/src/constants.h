/*constants.h*/


#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types.h"


/* */

//----------------------------CONSTANTS------------------------------------

//LIGHT
#define QA                       -90      //Intercept of Net vs Solar Radiation (W/m^2)
#define QB                       0.8      //Slope of Net vs Solar Radiation
#define RAD2PAR                 0.45     // (DIM) ratio PAR / SWtotal
#define RAD2PPFD                0.45     //

#define GC_GDM                     2      //convert grams C to grams DM (Campioli et al., 2013)
#define GLUCOSE_C                0.4      //fraction of Carbon in glucose reserve (Barbaroux et al., 2002)
#define MOLPAR_MJ                2.3      //Conversion of Solar Radiation to PAR - CONVERT MJ TO MOLPAR -- 1 MJ = 2.3 molPAR
#define GDM_MOL                   24      //Molecular weight of dry matter - CONVERT molC TO gDM (12 Molecular weight of C * 2 to have DM)(IPCC guidelines 1996)
#define GC_MOL                    12      //Convert molC to grams of C
#define W_MJ                 1000000      //convert Watt to MegaJoule/sec m^2
#define SBC                  5.67e-8      //(W/(m^2 K^4)) Stefan-Boltzmann constant
#define CP                    1010.0      //(J/kg K) specific heat of air
#define G_STD    			 9.80665      //(m/s2) standard gravitational accel. */
#define P_STD               101325.0      //(Pa) standard pressure at 0.0 m elevation */
#define T_STD                 288.15      //(K) standard temp at 0.0 m elevation  */
#define MA                28.9644e-3      //(kg/mol) molecular weight of air */
#define MW                18.0148e-3      //(kg/mol) molecular weight of water */
#define LR_STD                0.0065      //(-K/m) standard temperature lapse rate */
#define Rgas                     8.3144      //(m3 Pa/ mol K) gas law constant */
#define EPS                   0.6219      //(MW/MA) unitless ratio of molec weights */
#define PPFD50      			75.0      //(umol/m2/s) PPFD for 1/2 stomatal closure */
#define EPAR        			4.55      //(umol/J) PAR photon energy ratio */
#define SLAMKG				     0.1	  //Convert SLA IN cm ^2/gC in m^2/KgC

#define E20						2.2       //rate of change of saturated VPD with T at 20C
#define VPDCONV			   0.000622       //convert VPD to saturation deficit = 18/29/1000
#define EVAPOCOEFF			   1.32       //Priestley Taylor Coefficient (Hobbins et al., 2001)

#define MWratio               0.622       //ratio molecular weight of water vapour air
#define TempAbs				 273.13        //absolute temperature in Kelvin

#define GDD_BASIS                 5        //temperature C° basis for thermic sum


//COSTANTS
#define Pi               3.141592654     //Pi greco
#define ln2              0.693147181
#define eps                   0.0001
#define e				  2.71828183     //nepero/eulero number

#define GRPERC                   0.3     //(DIM) percentage of growth resp per unit of C grown see BIOME, 0.25 for LPJ

#define refCO2CONC				 281 //ppmv reference co2 concentration see Veroustraete 1994, 2002
#define O2CONC				    20.9     //% of O2 concentration see Verstraeten 1994, 2002

#define  STEMPOWER_A	        2.08    //Power in the Stem Mass v. diameter relationship for DBH < 9 cm
#define  STEMPOWER_B	        2.64    //Power in the Stem Mass v. diameter relationship for 9 < DBH < 15 cm
#define  STEMPOWER_C	        2.51    //Power in the Stem Mass v. diameter relationship for DBH > 15 cm

//MARCONI CONSTANTS::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
#define MAXTURNTIME 5000
//soil constants
#define DRF   0.04//0.03
#define EFFRB .67
#define EFFAC .2
#define RBO 0.02
#define SRB 0.9
#define KRCVL 0.25
#define KRCL 0.074
#define KRCR 0.02
#define KRB    0.12
#define HRB    0.04
#define EFFNO  0.2
//#define um_no3   .67
//#define um_no2   .67
//#define um_no    .34
//#define um_n2o   .34

