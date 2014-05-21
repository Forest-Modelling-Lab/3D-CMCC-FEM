/*
 * soilmodel.c
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types.h"
#include "constants.h"

/*
 	 FOR ALESSIO: VARIABLES TO BE INPUTTED IN types.h
	typedef struct {
		...
	 	// ROTHC SPECIFIC PARAMETERS
		double kDPM;	//decomposition rate constant for Decomposable plant material (DPM)
		double kRPM;	//decomposition rate constant for Resistant plant material (RPM)
		double kBIO;	//decomposition rate constant for Microbial Biomass (BIO)
		double kHUM;	//decomposition rate constant for Humified Organic Matter  (HUM)
		double litterToDPM;	//ratio of litterfall OM decaying into DPM pool
		double partitioningToBio; 	//ratio of decayed specific compartment OM decaying into BIO pool

	} site_t;

	typedef struct {
	 	// ROTHC SPECIFIC VARIABLES
		double decomposablePlantMaterial;	//decomposable plant material carbon pool
		double resistantPlantMaterial;	//decomposable plant material carbon pool
		double microbialBiomass;	//resistant plant material carbon pool
		double humifiedOM;	//humified organic matter carbon pool
		double inertOM;	//inert humified carbon pool
		double soilCO2;	//carbon dioxide emission from soil (soil  respiration)
		double accumulatedSoilMoistDeficit; //quantity of soil moisture deficit cumulated in the previous months following the scheme of RothC (Coleman and Jenkinson, 2008)
	} SOIL;

 */

/*
 * USER GUIDE:
 * RothC-26.3 is a model for the turnover of organic carbon in non-waterlogged
 * topsoils that allows for the effects of soil type, temperature, moisture content
 * and plant cover on the turnover process.
 * It uses a monthly time step to calculate total organic carbon (t ha -1)
 * It should be used cautiously on subsoils, soils developed on recent volcanic ash
 * (but see Parshotam et al 1995, Tate et al 1996 and Saggar et al 1996), soils from the tundra and taiga
 * and not at all on soils that are permanently waterlogged
 */

//this function should be used only for function not directly related to the forest layers, it just run at the end
//of all physiological function for all forest/crop layers
void soil_rothC(MATRIX *const m, const YOS *const yos, const int years, const int month, const int day, const int years_of_simulation)
{
	MET_DATA *met;
	int iC;
	int cell;
	int height;
	int age;
	int species;
	int soil;
	int bVeg;
	double temperatureModifier;
	double moistureModifier;
	double soilCoverModifier;
	//double accumulatedSoilMoistDeficit;
	double maxSoilMoistDeficit;
	//double panEvapotranspiration;
	double decomposedCarbon;
	double ratioCO2;
	double maxSoilMoistDeficitModifier;
	double dBIO, dHUM;
	double ETO[12] = {25.0,31.0,53.0,73.0,104.0,124.0,140.0,124.0,86.0,54.0,31.0,23.0};
	double RAIN[12] = {94.0,134.0,110.0,79.0,39.0,20.0,22.0,24.0,64.0,94.0,110.0,158.0};
	double TAIR[12] = {7.0,7.0,9.0,11.0,15.0,19.0,23.0,21.5,20.0,16.0,11.0,8.0};
	double LITTER[12] = {.05,.05,.14,.34,.34,.14,.05,.05,.14,.24,.14,.05};

	double monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

	int debug;
	// check parameters;




	assert(m);
	met = (MET_DATA*) yos[years].m;

	for (cell = 0; cell < m->cells_count; cell++)
	{
		m->cells[cell].monthly_tot_et += m->cells[cell].daily_tot_c_evapotransp + m->cells[cell].soil_evaporation;
	}



	//todo Marconi Sergio 2/9/14: in my opinion rothC can be tested also as a daily version; in future I'd try without this condition
	if(day == monthDays[month]-1)
	{


		debug = 0;


		//these should be input parameters to be included in site.txt
		site->soilTimescale = 1 / 12.0;
		site-> kDPM = 10.0;
		site->kRPM = .3;
		site->kBIO = .66;
		site->kHUM = .02;
		site->litterToDPM = .59;
		site->partitioningToBio = .46;
		m->cells[0].soils_count = 1;
		dBIO = 0;
		dHUM = 0.0;

		for (cell = 0; cell < m->cells_count; cell++)
		{
			if  (m->cells[cell].landuse == F)
				soil_Log("\n*******landuse forest");
			else if  (m->cells[cell].landuse == Z)
				soil_Log("\n*******landuse crop");
//			else if  (m->cells[cell].landuse == B)
//				soil_Log("\n*******landuse forest with data");
			else
				soil_Log("\n*******bad");

			if (debug == 0 )
			{
				site->clay_perc = 20;
				met->cum_monthly_rain = RAIN[month];
				m->cells[cell].monthly_tot_et = ETO[month];
				m->cells[cell].monthly_tot_litterfall = LITTER[month];
				met->avg_monthly_temp = TAIR[month];
			}
			else
			{
				met->cum_monthly_rain = 0;
				met->avg_monthly_temp = 0;
				for(iC = 0; iC < monthDays[month]-1; iC ++)
				{
					met->cum_monthly_rain += met[month].d[iC].rain;
					met->avg_monthly_temp += met[month].d[iC].tavg;
				}
				met->avg_monthly_temp /= monthDays[month]-1;
				//m->cells[cell].monthly_tot_et /= monthDays[month]-1;
				//[soil] represents the soil layer number
				//0 index if for the upper layer
			}
			for (soil = 0; soil < m->cells[cell].soils_count; soil++)
			{
				// assuming  the value of 0 at the beginning of each simulation for each pool; this is intended to be changed since
				// it will host the quantity of carbon in each pool at the beginning of simulation (assumed as in equilibrium stage)
				if( month == JANUARY && years == 0) {
					m->cells[cell].soils[soil].decomposablePlantMaterial = 0.0;
					m->cells[cell].soils[soil].resistantPlantMaterial  = 0.0;
					m->cells[cell].soils[soil].microbialBiomass  = .0;
					m->cells[cell].soils[soil].humifiedOM  = .0;
					m->cells[cell].soils[soil].PrevMicrobialBiomass = .0;
					m->cells[cell].soils[soil].PrevHumifiedOM  = .0;
				}
				if (month == JANUARY) m->cells[cell].year_soil_het_resp  = .0;
				// value of monthly heterotrophic respiration at the beginning of each month
				m->cells[cell].soils[soil].soil_het_resp  = 0.0;
				bVeg = 0;


				/************************************************************
				 * 		DETERMINATION OF DECOMPOSITION FACTORS
				 ************************************************************/
				// variable used to calculate moistureModifier: function of clay%
				maxSoilMoistDeficit = -(20.0 +1.3 * site->clay_perc - .01 * pow(site->clay_perc, 2));
				//
				for (height = m->cells[cell].heights_count -1; height >=0; height --)
				{
					for ( age = m->cells[cell].heights[height].ages_count - 1 ; age >= 0 ; age-- )
					{
						for (species = 0; species < m->cells[cell].heights[height].ages[age].species_count; species++)
						{
							if (m->cells[cell].heights[height].ages[age].species[species].counter[VEG_UNVEG] ==1)
							{
								//vegetative period: the boolean variable is set to 1 (true) and it is sufficient to considered the top soil as not bare (exiting the nested for loop)
								soilCoverModifier = .6;
								maxSoilMoistDeficitModifier = 1.0;
								bVeg = 1;
								m->cells[cell].monthly_tot_litterfall = .1; //m->cells[cell].litter / (month +1);

								break;
							}else
								// unvegetative period; in this case the nested loop goes on, to check if there is an height, age or species class in vegetative period
							{
								//in case of a bare topspoil, the maximum factor is divided by 1.8 to represent the reduced evapotranspiration from bare soil
								maxSoilMoistDeficitModifier = 1.8;
								// in case of a bare soil soilCoverFactor assumes its maximum value
								soilCoverModifier = 1.0;
								// if soil is bare, it is not allowed to be dried out more than maxSoilMoistDefict/1.8
								//m->cells[cell].monthly_tot_litterfall = .0; //m->cells[cell].litter / (month +1);

							}
						}if(bVeg ==1) break;
					}if(bVeg==1) break;
				}
				if (debug == 0 )
				{
					//vegetative period: the boolean variable is set to 1 (true) and it is sufficient to considered the top soil as not bare (exiting the nested for loop)
					soilCoverModifier = .6;
					maxSoilMoistDeficitModifier = 1.0;
					bVeg = 1;
					//m->cells[cell].monthly_tot_litterfall = .1; //m->cells[cell].litter / (month +1);
				}
				maxSoilMoistDeficit = maxSoilMoistDeficit / maxSoilMoistDeficitModifier;

				//calculating temperature modifier as a function of  monthly average air temperature
				temperatureModifier = 47.9 / ( 1 + exp(106/(met->avg_monthly_temp+18.3)));

				//todo check if is panEvapotranspiration is the same of 		m->cells[cell].daily_evapotranspiration;
				/*
				 * 	COMPUTING MOISTURE RATE MODFYING FACTOR
				 * 	the accumulated Topsoil moisture deficit (TSMD) for the specific layer
				 * 	is calculated from the first month in which rainfall is greater than .75 * panEvaporation
				 * 	-> stops when accTSMD reaches maxTSMD [stays constant from that moment on]
				 * 	-> calculated again  after rainfall exceeds .75 *panEvaporation [soil wets up again]
				 * 	-> todo if open panEvaporation is not known, use 1/(.75) * potential ET (from Muller, 1982)
				 */
				if(met->cum_monthly_rain < m->cells[cell].monthly_tot_et * 0.75 ) m->cells[cell].soils[soil].boolAccTSMD = 1;
				// this value is going to be reduced in case rainfall - 0.75panET is less than 0
				if (m->cells[cell].soils[soil].boolAccTSMD == 1) m->cells[cell].soils[soil].accumulatedSoilMoistDeficit += met->cum_monthly_rain - 0.75 * m->cells[cell].monthly_tot_et;
				if(m->cells[cell].soils[soil].accumulatedSoilMoistDeficit < maxSoilMoistDeficit) m->cells[cell].soils[soil].accumulatedSoilMoistDeficit = maxSoilMoistDeficit;

				if(m->cells[cell].soils[soil].accumulatedSoilMoistDeficit >= 0 ||  month == JANUARY)
				{
					//toask assumed that the limit is 0 (a decifit more than 0 would mean that
					m->cells[cell].soils[soil].accumulatedSoilMoistDeficit = 0;
					//toask; I suppose that after arriving at a situation of 0 the cycle can be considered interrupted
					m->cells[cell].soils[soil].boolAccTSMD = 0;
				}

				//computing moistureModifier in the userguide there is the opposite sign BUT THAT is the error; it would not be [0:1]otherwise
				if(m->cells[cell].soils[soil].accumulatedSoilMoistDeficit > .444 * maxSoilMoistDeficit)moistureModifier = 1.0;
				else{moistureModifier = .2 + (1.0-.2) * (maxSoilMoistDeficit - m->cells[cell].soils[soil].accumulatedSoilMoistDeficit)/(maxSoilMoistDeficit - .444*maxSoilMoistDeficit);};

				// computing the partitioning of carbon between that lost from the soil (CO2) and that remaining (BIO + HUM); assumed as a fraction of clay percent
				// of this quantity (x) x/(x+1) is the respired fraction; 1/(x+1) is formed as BIO + HUM
				ratioCO2 = 1.67 * ( 1.85 + 1.6 * exp(-0.0786 * site->clay_perc));

				/************************************************************
			 DETERMINATION AND DECAY OF DECOMPOSABLE PLANT MATERIAL
				 ************************************************************/
				/*
				 * toask flowchart assumed:
				 * for each pool there are 2 processes determining the decay:
				 * (1) Decomposition of the specific active compartment; function of the previously calculated factors (with the only kPOOL one varying) and dPool
				 * (2) Partitioning of decomposed carbon in heterotrophicRespiration and BIO + HUM pools
				 * the flowchart of the decomposition is assumed linear and occurs just once per simulated month (including HUM and BIO):
				 *  DPM -> RPM -> BIO -> HUM->
				 *  in each case the BIO + HUM is partitioned in the two different carbon pools as inferred from RothC userGuide
				 */

				// partitioning of litterfall into the two active pools DPM and RPM as a function of the plant functional type (PFT) covering the soil
				m->cells[cell].soils[soil].decomposablePlantMaterial += m->cells[cell].monthly_tot_litterfall * site->litterToDPM;
				// to compute the quantity of carbon decomposed for each pool we used always the same variable "decomposedCarbon.
				// as a matter of fact it represents the total amount of C decomposed, and will be partitioned into CO2 and BIO + HUM as a function of ratioCO2
				decomposedCarbon =m->cells[cell].soils[soil].decomposablePlantMaterial * (1- exp(-temperatureModifier * soilCoverModifier * moistureModifier * site->soilTimescale * site->kDPM));
				// at the end of the month the DPM pool declines to DPM * exp(-a b c k t)
				m->cells[cell].soils[soil].decomposablePlantMaterial  *= exp(-temperatureModifier * soilCoverModifier * moistureModifier * (site->soilTimescale) * site->kDPM);
				//quantity of soil heterotrophic respiration coming from DPM carbon compartment
				m->cells[cell].soils[soil].soil_het_resp += ratioCO2 / (ratioCO2 + 1) * decomposedCarbon;
				//updating BIO and HUM pools
				dBIO+= (1 /(ratioCO2 + 1)) * 	site->partitioningToBio * decomposedCarbon;
				dHUM+= (1 /(ratioCO2 + 1)) * (1- site->partitioningToBio) * decomposedCarbon;

				/************************************************************
				 * 							RESISTANT PLANT MATERIAL
				 ************************************************************/
				//following the same scheme as above
				// the total amount of Litter is partitioned between DPM and RPM; that is why 1-litterToDPM goes to RPM
				m->cells[cell].soils[soil].resistantPlantMaterial += m->cells[cell].monthly_tot_litterfall * (1 - site->litterToDPM);

				decomposedCarbon =m->cells[cell].soils[soil].resistantPlantMaterial * (1- exp(-temperatureModifier * soilCoverModifier * moistureModifier * site->soilTimescale* site->kRPM));
				m->cells[cell].soils[soil].resistantPlantMaterial  *= exp(- temperatureModifier * soilCoverModifier * moistureModifier * (site->soilTimescale) * site->kRPM);
				//m->cells[cell].soils[soil].resistantPlantMaterial -= decomposedCarbon;
				m->cells[cell].soils[soil].soil_het_resp  += ratioCO2 / (ratioCO2 + 1) * decomposedCarbon;
				dBIO+= (1 /(ratioCO2 + 1)) * site->partitioningToBio * decomposedCarbon;
				dHUM+= (1 /(ratioCO2 + 1)) * (1 - site->partitioningToBio) * decomposedCarbon;

				/************************************************************
				 * 			MICROBIAL BIOMASS FROM RESISTANT POOL
				 ************************************************************/
				decomposedCarbon =m->cells[cell].soils[soil].microbialBiomass * (1- exp(-temperatureModifier * soilCoverModifier * moistureModifier * site->soilTimescale * site->kBIO));
				dBIO += m->cells[cell].soils[soil].microbialBiomass  * exp(-temperatureModifier * soilCoverModifier * moistureModifier * (site->soilTimescale) * site->kBIO);
				//dBIO -= decomposedCarbon;
				m->cells[cell].soils[soil].soil_het_resp   += ratioCO2 / (ratioCO2 + 1) * decomposedCarbon;
				dBIO += (1 /(ratioCO2 + 1)) * site->partitioningToBio * decomposedCarbon;
				dHUM += (1 /(ratioCO2 + 1)) * (1 - site->partitioningToBio) * decomposedCarbon;


				/************************************************************
				 * 				HUMUS BIOMASS FROM RESISTANT POOL
				 ************************************************************/
				decomposedCarbon =m->cells[cell].soils[soil].humifiedOM * (1- exp(-temperatureModifier * soilCoverModifier * moistureModifier * site->soilTimescale* site->kHUM));
				dHUM += m->cells[cell].soils[soil].humifiedOM  * exp(-temperatureModifier * soilCoverModifier * moistureModifier * (site->soilTimescale) * site->kHUM);
				//dHUM -= decomposedCarbon;
				m->cells[cell].soils[soil].soil_het_resp += ratioCO2 / (ratioCO2 + 1) * decomposedCarbon;
				dBIO+= (1 /(ratioCO2 + 1)) * site->partitioningToBio * decomposedCarbon;
				dHUM += (1 /(ratioCO2 + 1)) * (1 - site->partitioningToBio) * decomposedCarbon;


				/**************************************************************************************************
				 * 		INERT ORGANIC MATTER computed as the sum of the other 4 compartments (from Falloon et al., 1998)
				 **************************************************************************************************/
				m->cells[cell].soils[soil].inertOM = .049 * (m->cells[cell].soils[soil].decomposablePlantMaterial +m->cells[cell].soils[soil].humifiedOM +
						m->cells[cell].soils[soil].resistantPlantMaterial +m->cells[cell].soils[soil].microbialBiomass);

				/************************************************************
				 * 	UPDATING MICROBIC AND HUMADS POOLS
				 ************************************************************/

				m->cells[cell].soils[soil].humifiedOM = dHUM;
				//m->cells[cell].year_humifiedOM += m->cells[cell].soils[soil].humifiedOM;

				m->cells[cell].soils[soil].microbialBiomass = dBIO;
				//m->cells[cell].year_microbialBiomass += m->cells[cell].soils[soil].microbialBiomass;

				m->cells[cell].year_soil_het_resp += m->cells[cell].soils[soil].soil_het_resp ;
//				soil_Log("\n\nn_day: \t %d", met[month].d[day].n_days);
//				soil_Log("\n*********************\nRothamsted Carbon Model routine"
//						"\nday: \t %d\nmonth: \t %d"
//						"\nTair: \t %f"
//						"\nrain: \t %f"
//						"\nlitter: \t%f"
//						"\nETO: \t%f"
//						"\n\ntemperature modifier: \t%g"
//						"\nsoilCover factor: \t %f"
//						"\nmoisture Factor: \t %f"
//						"\nsoil respiration: \t %f"
//						"\nmicrobial biomass: \t %f"
//						"\nhumified OM: \t %f"
//						"\nresistant OM: \t %f"
//						"\ndecomposable OM: \t %f"
//						"\ninertOM:\t %f"
//						"\nratioCO2 / BIO + HUM:\t %f", day, month, met->avg_monthly_temp, met->cum_monthly_rain, m->cells[cell].monthly_tot_litterfall , m->cells[cell].monthly_tot_et,  temperatureModifier, soilCoverModifier,moistureModifier,m->cells[cell].soils[soil].soil_het_resp ,m->cells[cell].soils[soil].microbialBiomass,
//						m->cells[cell].soils[soil].humifiedOM,m->cells[cell].soils[soil].resistantPlantMaterial,m->cells[cell].soils[soil].decomposablePlantMaterial, m->cells[cell].soils[soil].inertOM, ratioCO2);
//
//
//				soil_Log("\n****parameter to decomposition:****"
//						"\na:\t%f"
//						"\nb:\t%f"
//						"\nc:\t%f"
//						"\nt:\t%f"
//						"\nfactor:\t%f",  temperatureModifier, moistureModifier, soilCoverModifier,site->soilTimescale,  temperatureModifier *  moistureModifier *  soilCoverModifier * site->soilTimescale);
//
//				soil_Log("\n");
				m->cells[cell].monthly_tot_et =0;
				m->cells[cell].temperatureModifier = temperatureModifier;
				m->cells[cell].soilCoverModifier = soilCoverModifier;
				m->cells[cell].moistureModifier = moistureModifier;
			}
		}
	}
}
