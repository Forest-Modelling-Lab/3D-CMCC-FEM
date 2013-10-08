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

#define MOLPAR_MJ                2.3      //Conversion of Solar Radiation to PAR - CONVERT MJ TO MOLPAR -- 1 MJ = 2.3 molPAR
#define GDM_MOL                   24      //Molecular weight of dry matter - CONVERT molC TO gDM (12 Molecular weight of C * 2 to have DM)(IPCC guidelines 1996)
#define GC_MOL                    12      //Convert molC to grams of C
#define W_MJ                 1000000      //convert Watt to MegaJoule/sec m^2
#define LATENT_HEAT_FUSION       350	  //Latent heat of fusion (kJ/kg)
#define LATENT_HEAT_SUBLIMATION 2845      //Latent heat of sublimation (kJ/kg)
#define SBC                  5.67e-8      //(W/(m^2 K^4)) Stefan-Boltzmann constant
#define CP                    1010.0      //(J/kg K) specific heat of air
#define G_STD    			 9.80665      //(m/s2) standard gravitational accel. */
#define P_STD               101325.0      //(Pa) standard pressure at 0.0 m elevation */
#define T_STD                 288.15      //(K) standard temp at 0.0 m elevation  */
#define MA                28.9644e-3      //(kg/mol) molecular weight of air */
#define MW                18.0148e-3      //(kg/mol) molecular weight of water */
#define CP                    1010.0      //(J/kg K) specific heat of air */
#define LR_STD                0.0065      //(-K/m) standard temperature lapse rate */
#define R                     8.3143      //(m3 Pa/ mol K) gas law constant */
#define SBC                  5.67e-8      //(W/(m2 K4)) Stefan-Boltzmann constant */
#define EPS                   0.6219      //(MW/MA) unitless ratio of molec weights */
#define PPFD50      			75.0      //(umol/m2/s) PPFD for 1/2 stomatal closure */
#define EPAR        			4.55      //(umol/J) PAR photon energy ratio */
#define SLAMKG				     0.1	  //Convert SLA IN cm ^2/gC in m^2/KgC


//COSTANT
#define Pi               3.141592654     //Pi greco
#define ln2              0.693147181
#define eps                   0.0001
#define e				  2.71828183     //nepero/eulero number

#define GRPERC                   0.3     //(DIM) percentage of growth resp per unit of C grown see BIOME, 0.25 for LPJ






#define  STEMPOWER_A	        2.08    //Power in the Stem Mass v. diameter relationship for DBH < 9 cm
#define  STEMPOWER_B	        2.64    //Power in the Stem Mass v. diameter relationship for 9 < DBH < 15 cm
#define  STEMPOWER_C	        2.51    //Power in the Stem Mass v. diameter relationship for DBH > 15 cm
