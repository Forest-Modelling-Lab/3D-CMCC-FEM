/*cropmodel.c*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types.h"
#include <string.h>

extern  const char *szMonth[MONTHS];
//extern int MonthLength[];
//extern int DaysInMonth[];
//extern int fill_cell_from_heights(CELL *const c, const ROW *const row);

//ciao!
//define
#define SNOW_COEFF 15 		// snow cm;
#define DAY 24
#define THERMAL_STAGE1 400			// thermal period for stage 1
#define S_ALT -0.83

//#define E_SPIN

/* */
int crop_model_M(MATRIX *const m, const YOS *const yos, const int years, const int month, const int years_of_simulation)
{
	MET_DATA *met;


	//static int height;
	//static int age;
	//static int species;

	/**************************************************************************************************************************************
	 * 													STATIC AND CONST PARAMETERS
	 **************************************************************************************************************************************/
//	static float vpd;
	static int cell;

	int   const   soilLayer = 1;						// when done a cropType.h point to the value in site
	int   const   phyllocron = 95;						// time interval between leaf tip appearance todo it shoul be somehow variable (see Ritchie et al., 91)
//	const int     MAX_DIM = 100;
	const int     MAXINT = 32767;
	const float   scatterLightParameter = 0.02;			//scatter of light parameter; set as default 0.02 (Zhang et al. 2002)
	const float   rootWaterUptakeCoefficient = 0.003;	// root water uptake coefficient (0.003 cm water/cm root)

	float max = -MAXINT;
	float min =  MAXINT;

	//dichiara e inizializza qui tutte le variabili che ti servono

	/**************************************************************************************************
	 * 											TRASVERSALLY USED IN CROP MODEL
	 **************************************************************************************************/

	int   l;																// enumerator;
	int   i;																// enumerator;		???crop layers' number
	int   species;


	//**************************************************************************************************
	//											DAY LENGTH
	//**************************************************************************************************

	int   maxAltitudeJulianDate;											// julian date when solar altitude is the highest
	int   julianDate;														// count_month; last valid day for each month;
	int   actualDate;
	float albedo;															// field albedo
	float Sfactor;
	float Cfactor;
	float daylength;
	float solarDeclination;
	float solarElevation;													// elevation angle of the sun (gradient)
	float Selev;															// elevation angle of the sun as taken fron zenit angle comutation
	float zenit;															// zenit angle of the sun in a certain hour
	//float half_rad;

	//***************************************************************************************************
	//										DAILY SOLAR RADIATION
	//***************************************************************************************************
	int   landUseType;
	float solarConstant; 													// solar constant (J/m^2/s)
	float extraTerrestrialInsolation;
	float sunshine;
	float empiric_param_a;
	float empiric_param_b;
	float dailySolarRadiation;												// daily solar radiation (J/m^2/s) it would be in type.h:	m->cells[cell].net_radiation
	float bareGroundAlbedo;													// bare ground albedo
	// used albedo
	float snow;																// snow (cm water) it should be m->met[month].snow
	float LAI;																// in m->cells[cell].heights[height].species[species].value[LAI]
	float LAI_Max;
	float Net_Radiation;
//	float maxPhotoperiodismDaylength;										// max daylength for photoperiodism; is useful only for those lpaces in which daylength exceeds 20h

	/*********************************************************************************************************
	 * 											TEMPERATURE
	 *********************************************************************************************************/

	float snow_eff;															// snow effect on canopy temperature
	float canopyMaxTemperature;												// max (daily) canopy temperature
	float canopyMinTemperature;												// min (daily) canopy temperature
	float canopyDaytimeMeanTemperature;										// canopy daytime (mean) temperature
	float canopyDaytimeMeanTemperature2;									//canopy daytime (mean) temperature estimated on a sin curve basis (???)
	float meanDiurnalCanopyTemperature;										// mean diurnal canopy temperature as arithmetic mean of Tc_d2
	float meanCanopyTemperature;											// mean canopy temperature
	//float Tcm;															// canopy daily mean temperature (°C)
	float annualTemperatureAmplitude;										// amplitude of annual temperatures (°C)
	float meanAnnualTemperature;											// mean of annual temperatures (°C)
	float soilSurfaceTemperature;											// soil surface temperature in day k
	float soilSurfaceTempPrevious;																// soil surface temperature at day k - 1;
//	float soilSurfaceTemperature2;											// soil surface temperature in day k (computed with Parton)

	//float Tmax;															// daily air max temperature (°C)
	//float Tmin;															// daily air minimum temperature (°C)

//	float surfaceTemperatureAdjustment;										// adjustment factor for the effects of surface temperature
	float soilLayerThickness[1];									// thickness of soil layer
	//	float Zmax;															// soil depth from the surface

	float mid1;
	float mid2;
	float dampingDepth[1];											// damping depth in soil temperature estimation (cm water)
	float soilBulkDensity;													// average soil bulk density: each array cell stores its' array profile BD; BD[l+1] = sum BD[i];
	float layerMoisture[1];											// soil moisture of specific layer (cm^3 water / cm^3 soil)
	float temperatureFactor1[1];									// factor to make near surface temperature strong function of the surface temperature
	float temperatureFactor2[1];									// factor to make near surface temperature strong function of the surface temperature
	float maxAirTemperature;
	float minAirTemperature;

	float snowCoverLagFactor; 												// lagging factor simulating residues and and snow cover effects on soil surface
	float soilLayerTemperature[1];									//soil layer temperature at its center
//	float soilMaxTemperature;
//	float soilMinimumTemperature;
	int   hourDay;															// hour of the day
	int   wetDays;															// number of rainy days in the current month;
	int   dryDays;															// number of dry days in the current month


	float lag; 																// coeff. ranging from 0.0 to 1.0: allows weighting of (d-1)’s soil temp. with the current day’s
	// set at 0.5: weighted the current T estimates equally with the previous day’s temperature.

	//---------------------------------- PARTON -------------------------------

	float epigeousBiomass;													// parton epigeous biomass fraction
	float deltaSoilAirTemperature;											// the difference between maximum surface temperature soil and air temperature
	float partonSoilSurfaceMaxTemperature;									// parton soil surface max temperature
	float partonSoilSurfaceMinTemperature;									// parton soil surface min temperature
	float partonSoilSurfaceMeanTemperature;									// parton estimated soil surface mean temperature


	/****************************************************************************************************
	 *										RUNOFF AND WATER REDISTRIBUITION
	 ****************************************************************************************************/

	int   drainageClass;													// soil drainage class, used to determine SWcon factor as defined by Ritchie and Iglesias
	float dailyMaximumSnowMelt;												// daily maximum snow melt
	float dailyMaximumPlantInterception; 									// daily maximum plant interception  (cm water)
	float maxInfiltration;
	float Runoff;

	//watch out if it is correctly formatted
	char Luse[20] = "RowCrops";
	char pract[20]  = "straightRaw";
	int  hydroCondition;
	int  hydroGroup;


//	float landuse;
	float CN;
	float SWcon;															// drainage coefficient (Ritchie, 88)
	float profileWaterContent;												//soil water content from input: it is a whole profile value
	float drain[1];													// daily water draining flux (cm)
//	float Diff;																// daily diffusiob flux of water between soil layers (cm)


	/********************************************************************************************************************************
	 * 												EVAPOTRANSPIRATION and SOIL WATER BALANCE
	 *******************************************************************************************************************************/

	//float SWcon;														// drainage coefficient (Ritchie, 88)
	float fieldCapacityLayerMoisture[1];						// soil moisture at field capacity of the specific layer;
	float actualTranspiration;											// actual crop transpiration (cm water)
//	float potentialTranspiration;										// potential crop transpiration (cm water)
//	float layerMoistureAboveWilting[1];							// soil moisture above the layer's wilting point (cm^3 water / cm^3 soil)
	float layerWilting[1];										// soil moisture at wilting point of layer l_n
//	float soilWaterDiffusionCoeff;										// diffusion coefficient of soil water
	float potentialEvaporation;											// potential evaporation
	float equilibriumEvapotranspiration;								// equilibrium evapotranspiration (cm)
	float potentialEvapotranspiration;									// potential evapotranspiration (cm)
	float moistureSoilEvaporationEffect;								// effects of soil moisture on soil evaporation
	float depthTopSoilAffectingEvaporation;								// depth of top soil affecting evaporation (20cm) (cm)

	float waterUptakeSoilLayer;											// water uptake from specific soil layer
//	float Wup_s;
	float potentialTranspiration;

	float moistureEffectWaterUptake1[1];						// effect of moisture on water uptake
//	float moistureEffectNitrogenUptake2[1];						// effect of moisture on nitrogen uptake

	float infiltration;													// quantity of water infiltrated in a soil layer
	float Hold[soilLayer];												// quantity of water holded in the specific layer???
	float layerFieldSaturation[1];								// see ceres wheat 2.0
	float flux;															// layer specific soil water flux
	float normVolumetricWater[1];

	float Diffusion;
	float actualSoilEvaporation;										// actual soil evaporation
	float fractionFactor;

	/*********************************************************************************************************************************
	 * 										THERMAL TIME APPROACH
	 *********************************************************************************************************************************/
	int   stageLimit[9];   //todo
	float dailyThermalTime;
	//float dailyThermalTime2;											// daily thermal time using penning the vries, 1989)
	float maxDevelopmentTemperature;									// max temperature in which possible development
	float basalTemperature;												// basal temperature for wheat; corn (8), rice (10)

	float developmentRate;												// developmental rate



	/********************************************************************************************************************************
	 * 											DEVELOPMENT STAGE SETTING VARIABLES
	 *******************************************************************************************************************************/

	int   sowingdepth;													// INPUT!!!!!!  sowing depth input parameter (cm)
	int   stage;														// developmental scale used to estimate crop growth
	float abovegroundFraction;											// aboveground biomass fraction
	int   stop_cycle;													// boolean: if equal to 1 cycle has finished before the end of the year: ignore dev. module from that point on

	/********************************************************************************************************************************
	 * 												PHOTOSYNTHESIS MODULE
	 *******************************************************************************************************************************/

	float fCO2;															// effects of CO2 concentration on photosynthesis
	float co2EffectPhotosynthesis;										// crop parameter for CO2 effects on photosynthesis (0.4 for C4; 0.8 for C3)
//	float CO2;															// atmospheric CO2;
	float lightSaturationPhotoRate;										// Photosynthesis rate at light saturation (kg CO2/ha/h)
	float ligthTempSaturationPhotoRate;									// Photosynthesis rate at light saturation when T is optimal;
//	float accumulatedNH3Loss; 											// accumulated NH3 loss at time t (kg N/ha) ex AM;
//	float dailyAssimilationRate;										// daily crop assimilation rate (P0 - Rg, g/m^2)

	float blackBodyExtinctionCoeff;										// Extinction coefficient of assumed black body leaves

	//gaussian integration parameters:
	float gaussianParameter1[3];
	float gaussianParameter2[3];

//	float SL;															//??????
	//float x;															//
	float layerSoilTempEffectPhotoRate;									// effecsoilLayerTemperature of temperature on photosynthesis rate at light saturation
	float canopyShape;													// canopy shape, influencing light diffusion coefficient
	float horizontalSemiax;												// leaves horizontal semiaxis distribuition
	float verticalSemiax;												// leaves vertical semiaxis distribuition


	//photosynthesis
	float canopyHeightLAI[3];											// lai at a specific canopy height
	float gaussianIntegrationHour[3];									// hour chosen to compute the gaussian integration

	//light variables
	float diffuseLightExtinctionCoeff; 									// extinction coefficient of diffuse light
	float extinctionCoeff;												// extinction coefficient; is this the one used for diffuse light!?
	float diffuseLightFraction;											// fraction of diffuse light
//	float blackBodyExtinctionCoeff2;									// Extinction coefficient of assumed black body leaves
	float directLightExtinctionCoeff;									// extinction coefficient of direct light



	float prova;




	float hourPar;														// mean par at a specific hour
	float diffuseLightAboveCanopy;										// diffuse light above the canopy
	float directLightAboveCanopy;										// direct light above the canopy
	float directComponentDirectLight;									// Direct component of the direct light after canopy scattering
	float directLight;													// Direct light
	float diffuseLight;													// Diffuse light
	float shadeLeavesLightAbsor;										// light adsorbed by shaded leaves in a layer
	float leafSurface90degLight;										// light which is perpendicoular to leeaf surface

	float atmosphericTrasmissionCoeff;									// atmospheric transmission coefficient
	float canopyHorizontalReflectivity;									// Reflectivity of horizontally distribuited canopy
	float canopySpharicalReflectivity;									// Reflectivity of sphaerical distribuited canopy

	float midVariable4;													// mid variable
	float midVariable3;													// mid variable

	float shadLeavesPhotoRate;											// photosynthesis rate of shaded leaves
	float sunlitLeavesPhotoRate;										// photosynthesis rate of sunlit leaves
	float P_Li_t;														// Grosials photosynthesis rate at layer Li and time tj
	float gaussIntegrPhoto;												// gaussian integration f(time) of photosynthesis for a canopy layer
	float gaussIntegPhotoSum; 											// photosynthesi value for a canopy point, as proposed for a gaussian integration
	float dailyGrossPhoto;												// daily gross photosinthesis (g/m^2)
	float initialLightUseEfficiency;									// initial light use efficiency   -> taken as PLEI (penning de vries)

	/******************************************************************************************************************************
	 * 											RESPIRATION MODULE
	 *****************************************************************************************************************************/
	float maintenanceRespiration;										// maintenance respiration
	float compartMaintenanceResp[4];									// maintenance respiration factor for organ i
	float compartBiomass[4];											// biomass of crop organ i
	float Q10;															// Q10 value
	float growthRespiration;											// growth respiration
	float growthRespEfficiency;											// average growth efficiency

	/********************************************************************************************************************************
	 * 										 BIOMASS PARTITIONING (ABOVEGROUND BIOMASS)
	 *******************************************************************************************************************************/

	float aboveBiomassFract;											// aboveground biomass fraction (todo understand if it is the same as potentialEvaporationi_biomass)
	float stemBiomassFract;												// stem biomass fraction
	float grainBiomassFract;											// grain biomass fraction

//	float dailyIncreaseLAI;												// daily increase of LAI
//	float dailyLeafSenescence;											// daily leaf senescence
//	float specificLeafArea;												// specific leaf area

	float waterStressFactor;											// water stress factor (from 0 to 1: formula to be found)
	float nitrogenStressFactor;											// nitrogen stress factor (nitrogenStressFactor to be evaluated with a formula; to be found)

	/***********************************************************************************************************************
	 * 												BELOVEGROUND BIOMASS
	 ********************************************************************************************************************/
	float layerRootLengthDensity[1];							// root length density of layer l_n (cm root/ cm^3 soil)
	//	float dlayerRootLengthDensity[l];								// daily increase of root length density in layer l
	int   cumPhyllocrons;




	/**************************************************************************************************************************
	 *												alternatives
	 *************************************************************************************************************************/
	/*										Claudio O. Stockle and Gaylon S. Campbell
	 *************************************************************************************************************************/
/*
	float solar_d;
	float half_rad;
	float time_eq;
	float s_middle;
	float dawn;
*/
	//Parton 1989:
//	float Tsoil;
//	float dawn2;

	//wiki: dawn
	float s_noon;					// approximate solar noon
	float mean_anomaly;				// solar mean anomaly
	float Center;					// equation of center
	float ec_lon;					// ecliptic longitude
//	float sol_transit;				// solar transit
	float sin_decl;					// declination of the sun
	float hour_angle;				// hour angle
	float j_cycle;					// actual julian cycle

	float hour_int;					// decimal of the hour transformed in its sexagesimal value
	int dawn3;
	int sunset;

	/*******************************************************************************************
	 * 							RITCHIE 1988
	 *******************************************************************************************/
	float deltaMoisture;
	float driestSoilWaterContent;
	float vernalizationFactor;
	float photoperiodFactor;
	float plantLeafAreaGrowthRate;
	float leafAreaGrowthRate;
	float tillNumber;
	float assimilateAreaToWeight,
		potentialLeafGrowth,
		potentialRootGrowth,
		dailyAssimilate,
		totalCumulativeLeafArea,
		tillerRate1,
		tillersPerSquareMeter;
	int plants,
		leafNumber,
		phylloCounter;
	float tillerRate2,
		cumulativeLeafAreaPhyllocron[5],
		leafAreaLossRate,
		senescenceLeafArea;

	//-----------------------------------------------------------------------------------------------------------------------------------

	/*****************************************************************************************************************************
	 * 										############################################
	 * 									       CROP MODULE OF 3D-CMCC-LANDSCAPE MODEL
	 * 										############################################
	 ******************************************************************************************************************************/


	// check parameters
	assert(m && yos);
	met = (MET_DATA*) yos[years].m;

	//control if all soil data are available
	for ( cell = 0; cell < m->cells_count; cell++)
	{
		if ((site->sand_perc == -999.0) ||
				(site->clay_perc == -999.0) ||
				(site->silt_perc == -999.0) ||
				(site->bulk_dens == -999.0) ||
				(site->soil_depth == -999.0) )
		{
			Log("NO SOIL DATA AVAILABLE\n");
			return 0;
		}
	}




	//--------------------------------------------------------------------------------------------------------------------------------------

	//monthly loop on each cell
	for ( cell = 0; cell < m->cells_count; cell++)
	{
		//**************************************************************************************************************************************
		// parameters initializated only the very first year
		if (month == JANUARY && years == 0)	//&& day == 0)
		{
			Log("***** VERY FIRST VARIABLES INITIALIZATION *****\n"); 	//PAY ATTENTION, IS THIS ALLRIGHT TO CALL IT INITIALIZATION!?

			//gaussian canpopy integration
			gaussianParameter1[0] = 0.5 - sqrt(0.15);
			gaussianParameter1[1] = 0.5;
			gaussianParameter1[2] = 0.5 + sqrt(0.15);

			//gaussian time integration
			gaussianParameter2[0] = 1.0 /3.6;
			gaussianParameter2[1] = 1.6 / 2.3;
			gaussianParameter2[2] = 1.0 / 3.6;

			//shandong K coefficient
			extinctionCoeff = 0.6;

			drainageClass = 3;
			hydroCondition = 1;
			hydroGroup = 2;

			//drainage profile coefficient SWcon (Ana Iglesias)
			if (drainageClass == -9999)
			{
				SWcon = 0.5;
				Log("\nAttention! no drainage factor found: used a standard 0.5 value of SWcon");
			}
			else
			{
				switch(drainageClass)
				{
				case 0:
					SWcon = 0.8;
					break;
				case 1:
					SWcon = 0.8;
					break;
				case 2:
					SWcon = 0.6;
					break;
				case 3:
					SWcon = 0.4;
					break;
				case 4:
					SWcon = 0.2;
					break;
				case 5:
					SWcon = 0.05;
					break;
				case 6:
					SWcon = 0.005;
					break;
				}
			}

			/*********************************************************
			 * 			SOIL CHARACTERISTICS
			 ********************************************************/
			waterStressFactor = .5;

			// critical daylenght for max photoperiodism
			//DL_max = 20.0;
			stage = 0;
			stop_cycle = 0;
			species = 0;
			landUseType = 0;

			sowingdepth = 60;  // INPUT!!!!!!  sowing depth input parameter (cm)
			bareGroundAlbedo = 0.2;

			snow = 0.00;
			//see if, using a single soil layer, this has to be removed
			depthTopSoilAffectingEvaporation = 20.0;
			//airTemperature = 15.0; //??
			soilSurfaceTemperature = 0.0;				//soil temperature: setted to 0 so that also soilSurfaceTemperature - 1 is: TODO ATTENTION! NOT TO BE USED IN JAN 1st
			maxDevelopmentTemperature = 34.0;				//max temperature in which possible development
			basalTemperature = 1.0;				//basal temperature for wheat; corn (8), rice (10)
			developmentRate = 0.0;					// developmental rate

			//JULIAN DATE: date at jan the 1st (first year of simulation)
			julianDate = (4713 * 365) + (4713 + yos[years].year) / 4 + (yos[years].year * 365) - 14;
			maxAltitudeJulianDate = 144;				//21/6

			soilBulkDensity = 1.49;				//(g/cm3)
			//profileWaterContent = 0.5;
			fractionFactor = 0.03;  // todo if heavyclay, 0.03; if sandy 0.07

			Q10 = 2.0;	//assumed by penning de vries, pag. 53

			for (l = 0; l < soilLayer; l++)
			{
				layerFieldSaturation[l] = 1.0 - soilBulkDensity / 2.65 - fractionFactor;
			}

			CN = 70;

			//Zmax = 63.5;

			abovegroundFraction = 80.0;			//aboveground biomass fraction (todo understand if it is the same)
			soilLayerThickness[0] = 50.0;
			//SW[0] = ;
			fieldCapacityLayerMoisture[0] = 30.0;			// it shoul be set at MAXASW (i suppose), but on site is putted as a comment
			//soil moisture of layer l at wilting point; assumed as 10% fraction of max swc ( filed capacity) (Collalti)
			layerWilting[0] = fieldCapacityLayerMoisture[0] * site->min_frac_maxasw;
			lag = 0;				//set at 0.5: weighted the current T estimates equally with the previous day’s temperature.
			driestSoilWaterContent = 0.5 * layerWilting[0];

			/***********************************************************
			 * 			SPECIE SPECIFIC PARAMETERS INITIALIZATION
			 **********************************************************/


			//species' specific initial parameters

			if (species == 0)			//and day is 1st
			{
				// taken from penning the vries: PLMX
				ligthTempSaturationPhotoRate = 40.0;
				// photorespiration factor
				co2EffectPhotosynthesis = 0.8;
				// initial light use efficiency
				initialLightUseEfficiency = 0.5;

				// nitrogen stress factor on photosynthesis
				nitrogenStressFactor = 0.4;

				vernalizationFactor = 0.5;
				photoperiodFactor = 0.4;

				LAI = 0.5;
				totalCumulativeLeafArea = LAI;
				LAI_Max = 0.8;

				plants = 300;
				tillNumber = 1;

				//canopy shape
				horizontalSemiax = 0.8;
				verticalSemiax = 1.0;
				canopyShape = horizontalSemiax / verticalSemiax;

				//first compartment biomass
				compartBiomass[0] = 10.0;	// leaf biomass
				compartBiomass[1] = 5.0;	// stem biomass
				compartBiomass[2] = 0.0;	// grain biomass
				compartBiomass[3] = 10.0;	// root biomass


				//to be further evaluated (values taken from penning de vries, pag. 52): IMPROVABLE; function of temperature and photosynthesis!! pag 53
				compartMaintenanceResp[0] = 0.016;	//wheat, sage: _______________
				compartMaintenanceResp[1] = 0.010; // general annual plants; in young stems tissue, rate 1.5 2 times higher
				compartMaintenanceResp[2] = 0.010;  // assumed equal to stem maintenance respiration
				compartMaintenanceResp[3] = 0.015; // general for annual plants

				cumulativeLeafAreaPhyllocron[0] = 0;
				cumulativeLeafAreaPhyllocron[1] = 0;
				cumulativeLeafAreaPhyllocron[2] = 0;
				cumulativeLeafAreaPhyllocron[3] = 0;
				cumulativeLeafAreaPhyllocron[4] = 0;

				//growth respiration efficiency; taken fron Penning de Vries
				growthRespEfficiency = (0.451 + 0.532 + 0.774 + 0.690 + 0.375) / 5.0;		//from listing 1, pag 205; is the mean coefficient  of specific chemical compound CO2 production not totally secure though
			}

			cumPhyllocrons = 1; //number of cumulative phyllocrones since emergence




			Log("\nAssumed a soil in:\n hydroCondition %d \nHydrogroup %d \nLuse %s \npract %s", hydroCondition, hydroGroup, Luse, pract);
			/*********************************************************************************************************************************
			 * 									SETTING OF DEVELOPMENTAL STAGES (INITIALIZATION OF VARIABLES)
			 ********************************************************************************************************************************/

			//riordina perbene


			stageLimit[0] = 400;
			stageLimit[1] = 3 * phyllocron;
			stageLimit[2] = 2 * phyllocron;
			stageLimit[3] = THERMAL_STAGE1;
			stageLimit[4] = 200;
			stageLimit[5] = 500;
			stageLimit[6] = MAXINT;		//here the end of the stage is set by harvest date; that's the switch (if(date == harvest date) {stage++}
			stageLimit[7] = 40.0 + 10.0 * sowingdepth / 5.0;	//to be clarified
			stageLimit[8] = 1; //to be clarified

			dailyThermalTime = 0;
			phylloCounter = 0;
			leafNumber= 0;
			cumPhyllocrons = 0;
			senescenceLeafArea = 0;
			// time interval between leaf tip appearance todo it shoul be somehow variable (see Ritchie et al., 91)
		}

/*
		//parameter reinitializated every year
		else if (month == JANUARY && years != 0)	// variable which have to be resetted at each yos
		{
			stop_cycle = 0;
			developmentRate = 0;
		}

		// here daily (monthly) resetted variables
		//todo	add a every month cycle; then the last out of if, adily resetting parameters

		CN = 70.0;

*/
		//to be set to 0 everyday/month
		canopyDaytimeMeanTemperature2 = 0;
		soilSurfaceTemperature = 0.0;
		moistureSoilEvaporationEffect = 0.0;
		waterUptakeSoilLayer = 0.0;
		gaussIntegPhotoSum = 0;
		maintenanceRespiration = 0.0;




		//*************************************************************************************************************************************


		Log("\n\nMONTH SIMULATED = %s\n", szMonth[month]);

		/*
		GetDayLength (&m->cells[cell], MonthLength[month]);
		vpd =  met[month].vpd * 10.0; //Get_vpd (met, month);
		Log("PROVA TEMPERATURA = %g\n", met[month].tavg);
		Log("PROVA SETTINGS = %c\n", settings->version);
		 */

		/**********************************************************************************************************************************
		 *											THERMOHYDRAULIC SUBMODEL
		 **********************************************************************************************************************************/
		//												SOLAR QUANTITIES
		//*************************************************************************************************


		//julian date
		julianDate += met[month].n_days;
		actualDate = met[month].n_days;
		Log("\nstarting from year %d\njulianDate: %d", yos[years].year, julianDate);

		//todo  should be set here maxAltitudeJulianDate: julian date when sun is higher?


		//solar declination
		solarDeclination = -asin(sin(23.4 * 180.0 / Pi) * cos(360.0 * (actualDate +10) / 365.0));
		Log("\nsolar declination = %g", solarDeclination);

		//todo try the 3D-CMCC-Forest-Model version and compare them

		Cfactor = cos(site->lat) * sin(solarDeclination);
		Sfactor = sin(site->lat) * sin(solarDeclination);
		Log("\nC value is %g\nS value is %g", Cfactor, Sfactor);

		//day length  TODO MAKE A COMPARISON OF THE TWO METHODS
		daylength = 12 + (24.0 / Pi) * asin(Sfactor / Cfactor);
		Log("\nDay length is equal to %g hours\n", daylength);

		// daily solar radiation using DNDC method
		Log("\n ****** COMPUTING DAILY SOLAR RADIATION ****** \n");

		//todo (substitute  to be understood if it is a net radiation formulation; comparison with net radiation
		solarConstant = 1370.0 * (1.0 + 0.033 * cos(2.0 * Pi * julianDate/365.0));
		Log("For a solar constant equal to %g\n", solarConstant);

		//extraterrestrial insulation
		extraTerrestrialInsolation = 3600 * solarConstant * (daylength * Sfactor + (24 * Cfactor * sqrt(1 - (pow(Sfactor,2) / pow(Cfactor,2)))) / Pi);
		Log("And the extra terrestrial insulation = %g\n", extraTerrestrialInsolation); // todo add unit


		//Collalti's net radiation function
		//Net_Radiation = QA + QB * (met[month].solar_rad * pow (10.0,  6)) / daylength;


		//##########################################################################################################################
		//taken from wikipedia at the voice "sunrise_equation"


		//5.07 ore vs 5.02

		Log("\n\n***** CALCULATING SUNRISE HOUR FROM SUNRISE EQUATION (WIKIPEDIA) *****");

		//	printf("\n%g",dawn2);
		j_cycle = (julianDate - 2451545 - 0.0009) - (site->lon /360.0);
		s_noon = 2451545 + 0.0009 + (site->lon / 360.0) + j_cycle;
		Log("\nsolar noon: %g",s_noon);
		mean_anomaly = (357.5291 + 0.98560028 * (s_noon - 2451545)) * 3.1415 / 180.0;
		Center = (1.9148 * sin (mean_anomaly)) + (0.0200 * sin(2 * mean_anomaly)) + (0.0003 * sin(3 * mean_anomaly));
		ec_lon = (mean_anomaly * 180.0 /3.1415) + 102.9372 + Cfactor + 180.0;
		//float s_transit =  s_noon + (0.0053 * sin(mean_anomaly)) - ( 0.0069 * sin(2*ec_lon * 3.1415 / 180.0));
		sin_decl = asin(sin(ec_lon * 3.1415 /180.0) * sin (23.45 * 3.1415 /180.0));
		hour_angle = acos((sin(-0.83 *3.1415 / 180.0) - sin(site->lat * 3.1415 /180.0) * sin(sin_decl))
				/(cos(site->lat * 3.1415 / 180.0) * cos(sin_decl)))*180.0/3.1415;
		hour_angle /= 15.0;
		Log("\nhour angle: %g", hour_angle);
		hour_int = (hour_angle - (int)hour_angle) * 60.0 / 100.0;
		Log("\nsexagesimal of the hour: %g", hour_int);
		if (hour_int > 0.50)
		{
			hour_int = (int)hour_angle + 1;
		}
		else
		{
			hour_int = (int)hour_angle;
		}
		dawn3 = 12 - hour_int;
		sunset = 12 + hour_int;
		Log("\nactual dawn hour, estimated by wiki function and given using 12 as half day:\n*** %d ***", dawn3);
		Log("\nactual sunset hour, estimated by wiki function and given using 12 as half day:\n*** %d ***", sunset);
		//sunshine is the real daylength: here assumed as 2*hour angle (that is half day duration calculated trough wikipedia)
		sunshine = 2 * hour_angle;

		//	sunshine = (int)daylength;
		Log("\nsunshine: %g h\n", sunshine);



		//##############################################################################################################################
		/*
		 * *******************************************************************************************************************
		 * 								A SOLAR EQ. ALTERNATIVE: STOCKLE AND CAMPBELL
		 * ********************************************************************************************************************/





		Log("\n------------------------------------------------------------------------");
		Log("\n\n******* DERIVING SOLAR RADIATION FROM SUNSHINE DURATION ********\n\n");

		landUseType = 0;
		if (landUseType == 1)
		{
			Log("for a %d landUseType \n", landUseType);
			empiric_param_a = 0.25;
			empiric_param_b = 0.45;
			Log("the empirical parameters are:\n\ta: %g\n\tb: %g\n", empiric_param_a, empiric_param_b);
		}
		else if (landUseType == 2)
		{
			Log("for a %d landUseType \n", landUseType);
			empiric_param_a = 0.29;
			empiric_param_b = 0.18;
			Log("the empirical parameters are:\n\ta: %g\n\tb: %g\n", empiric_param_a, empiric_param_b);
		}
		else if (landUseType == 0)
		{
			Log("for a %d landUseType \n", landUseType);
			empiric_param_a = 0.18;
			empiric_param_b = 0.55;
			Log("the empirical parameters are:\n\ta: %g\n\tb: %g\n", empiric_param_a, empiric_param_b);
		}
		else
		{
			Log("ERROR; NO PROPER LAND_TYPE INPUT");
			//return (0);}	//buttata là seminventato
		}
		Log("--------------------------------------------------------------------------\n\n");

		if (met[month].solar_rad != -9999)
		{
			//converted in J/(m^2 d)
			Log("\n\nSolar radiation measured in the field; converted in J/(M^2*d)");
			dailySolarRadiation = met[month].solar_rad * 1000000;
		}
		else
		{
			Log("\n******* DERIVING SOLAR RADIATION FROM SUNSHINE DURATION ********\n\n");
			//daily solar radiation formulation
			dailySolarRadiation = extraTerrestrialInsolation * (empiric_param_a + empiric_param_b * sunshine / daylength);		// compare with alessio's; i suppose sunshine's not
			//convert in MJ/m-2 d-1 : 1,000,000: perfect!! it is allineated to the values measured in field
			//ATTENTION! there's an error on Zhang et al 2002: the amount is (m^2d and not m^2s)
		}

		Log("daily solar radiation = %g J/(m^2*d)\n\n", dailySolarRadiation);

		//Zhang proposes also Bristow and campbell (1984) approach (the same of the first version of Daymet)



		//albedo			TODO: put it in shared
		Log("******* COMPUTING ALBEDO OF THE FIELD ******* \n\n");
		if (snow > 0.5)
		{
			albedo = 0.6;
			Log("albedo: %g\n", albedo);
		}
		else 		// nn ricordo il discorso di alessio; si può mettere = classico o per forza == Logico?
		{
			if (stage == 6 || stage == 7)
			{
				albedo = bareGroundAlbedo;
				Log("albedo: %g\n", albedo);
			}
			else if (stage == 4 || stage == 5)
			{
				albedo = 0.23 + pow((LAI - 4), 2) / 160;
				Log("albedo: %g\n", albedo);
			}
			else
			{
				albedo = 0.23 - (0.23 - bareGroundAlbedo) * - 0.75 * LAI;
				Log("albedo: %g\n", albedo);
			}
		}



		/********************************************************************************************************************
		 *										TEMPERATURE MODULE
		 * ********************************************************************************************************************/

		//############### air temperature ###################

		Log("\n******** CANOPY MONTHLY TEMPERATURES ********");

		//setting the maximum and minimum temperature of the whole year


		for(i = 0; i < 12; i++)
		{
			if (met[i].tavg > max)
			{
				max = met[i].tavg;
			}
			if (met[i].tavg < min)
			{
				min = met[i].tavg;
			}
		}
		annualTemperatureAmplitude = max - min;
		meanAnnualTemperature = max * 0.5 + min * 0.5;
		Log("\nThe difference between the annual maximum and annual minimum temperature is:%g", annualTemperatureAmplitude);
		Log("\nthe mean value between annual max and min temperature: %g", meanAnnualTemperature);

		//------ todo insert Tmax and T min -----------------------------------------------------------------------
		maxAirTemperature = met[month].tavg + 2;
		minAirTemperature = met[month].tavg -2;
		Log ("\nmax Tair: %g\nMin Tair: %g", maxAirTemperature, minAirTemperature);
		//---------------------------------------------------------------------------------------------------------

		//########## CANOPY TEMPERATURE ############

		// effect of snow on cover on canopy temperature
		if (met[month].tavg >=0) 			//not totally clear on Zhang 2002 appendix; anyway I'm quite sure it've to be used Tair instead of Tmax/min
		{
			snow_eff = 1;
		}
		else if (met[month].tavg <0 )
		{
			snow_eff = 2 + met[month].tavg * (0.4 + 0.0018 * pow((Minimum(SNOW_COEFF,snow) -15), 2));
		}
		Log("\nsnow effect coefficient equal to %g", snow_eff);

		canopyMaxTemperature = snow_eff * maxAirTemperature; //met[month].tavg;  todo  T_max in input
		Log("\nCanopy monthly max temperature is %g\n", canopyMaxTemperature);


		canopyMinTemperature = snow_eff * minAirTemperature;	//met[month].tavg;	todo  T_min in input
		Log("\nCanopy monthly min temperature is %g\n", canopyMinTemperature);

		//Canopy daily mean temperature
		meanCanopyTemperature = 0.5 * canopyMaxTemperature + 0.5 * canopyMinTemperature;
		Log("\nCanopy monthly mean temperature is %g\n", meanCanopyTemperature);
		//canopy day time mean temperature
		//try also 0.6 + 0.4
		canopyDaytimeMeanTemperature = 0.5 * meanCanopyTemperature + 0.5 * canopyMaxTemperature;
		Log("\nCanopy daytime mean temperature is %g\n", canopyDaytimeMeanTemperature);



		if(meanCanopyTemperature <= 0.0)
				{
					meanCanopyTemperature = 1.0;	//todo: elimina!!!!!
				}

				//setting layerSoilTempEffectPhotoRate: temperature reduction factor
				if (meanCanopyTemperature > 0.0 && meanCanopyTemperature <= 10.0)
				{
					layerSoilTempEffectPhotoRate = 0.0001;
				}
				else if(meanCanopyTemperature >= 10.0 && meanCanopyTemperature <= 35.0)
				{
					layerSoilTempEffectPhotoRate = 1.0;
				}
				else if ( meanCanopyTemperature > 35 && meanCanopyTemperature <= 50.0)
				{
					layerSoilTempEffectPhotoRate = 0.01;
				}
				else
				{
					layerSoilTempEffectPhotoRate = 0.0;
				}
/*
		//estimation of canopy hourly diurnal temperature; not in use for now

		// canopy hourly temperature during daytime
		for(hourDay = dawn3; hourDay < (sunset); hourDay ++)
		{
			//canopyMinTemperature + (canopyMaxTemperature - canopyMinTemperature) *
			meanDiurnalCanopyTemperature = canopyMinTemperature + (canopyMaxTemperature - canopyMinTemperature) * sin(Pi * ((float)hourDay - 11.82 + 0.5 * daylength) / (daylength - 3.3));
			Log("\nat hour %d canopy mean temperature assumed as: %g °C", hourDay, meanDiurnalCanopyTemperature);
			canopyDaytimeMeanTemperature2 += meanDiurnalCanopyTemperature;
		}
		canopyDaytimeMeanTemperature2 = canopyDaytimeMeanTemperature2 / (int)sunshine;
		Log("\n\nCanopy hourly estimated temperature in daytime (DNDC): %g °C", canopyDaytimeMeanTemperature2);


		//mean nighttime temperature, cumulated for the non day time hours as integers
		Tc_n = canopyMinTemperature + (Tset - canopyMinTemperature) * exp(2 * Pi * (hourDay[i] - 11.82 + 0.5 * daylength)/ (24 - daylength));
		Log("\nCanopy hourly estimated temperature in night time: %g °C",Tc_n);

*/
		/***********************************************************************************
		 * 									SOIL TEMPERATURE ESTIMATION
		 **********************************************************************************/

		Log("\n******* DAILY MEAN SOIL TEMPERATURE ********\n\n");

		//TODO sergio: attenzione qui c'è un soilSurfaceTemperature +1	its originis in Daymet first computation of Tt: anyway changed by Thornton 1988
		// SO I'VE SETTED


		if (month == JANUARY)		//todo add && day ==1)
		{
			Log("\n\nis January the 1st; influence of previous day not evaluated!");
			soilSurfaceTempPrevious = 0;
		}
		else
		{
			soilSurfaceTempPrevious = soilSurfaceTemperature;
		}

		if(met[month].ts_f != -9999)
		{
			soilSurfaceTemperature = met[month].ts_f;
			Log("\nsoil temperature measured: %g", soilSurfaceTemperature);
		}
		else
		{

			/**************************************************************************************************************
			 * 								SOIL SURFACE TEMPERATURE ESTIMETED
			 *************************************************************************************************************/

			//soilSurfaceTemperature = (1 - albedo) * (meanCanopyTemperature + (canopyMaxTemperature - canopyMinTemperature) * pow((0.03 * dailySolarRadiation), 0.5) + albedo *
			//		((1- albedoPrev) * ((meanCanopyTemperature + (canopyMaxTemperature - canopyMinTemperature) * pow((0.03 * dailySolarRadiation), 0.5) )));


			/*****************************************************************************************
			 * 					EPIC SOIL TEMPERATURE PREDICTION
			 *****************************************************************************************/
			/*
			 * this is a module usable just in case of a daily version; to be removed in case of monthly version
			 *
			 *
			 * 	for (t = 0; t < met[month].n_days; t++)
			 * 	{
			 * 		if(met[month].met[day].rain > 0)
			 * 		{
			 * 			wetDays ++;
			 * 		}
			 */
			//todo sergio: nested if to avoid the problem  of -i in case of thevery first days of the year
			// for now working with months as if they were days: substitute it with day == i && month == JANAURY

			Log("\n*****************************************************************************************\n"
					"*\n 					EPIC SOIL TEMPERATURE PREDICTION"
					"\n*****************************************************************************************");

			//todo: delete when daily model is ready
			wetDays = 10;
			dryDays = met[month].n_days - wetDays;

			if (settings->time == 'm')
			{
				Log("\nIs the first day(month) of the cycle: soilSurfaceTemperature not meaned");
				if (met[month].rain > 0.0)
				{
					//met[month].met[day - i].ta_min
					soilSurfaceTemperature += (met[month].tavg - 2 ) + ((float)wetDays / (float)met[month].n_days) *
							((float)met[month].tavg - (float)met[month].tavg -2);
					Log("\ncurrent day soilSurfaceTemperature: %g °C \nprevious day soilSurfaceTemperature: %g °C", soilSurfaceTemperature, soilSurfaceTempPrevious);
				}
				else
				{
					//met[month].met[day - i].ta_max
					soilSurfaceTemperature += met[month].tavg +2 + (dryDays / met[month].n_days) * (met[month].tavg +2 - met[month].tavg);
					Log("\ncurrent day soilSurfaceTemperature: %g °C \nprevious day soilSurfaceTemperature: %g °C", soilSurfaceTemperature, soilSurfaceTempPrevious);
				}
			}

			else
			{
/*
				if (month == JANUARY)
				{
					Log("\nIs the first day(month) of the cycle: soilSurfaceTemperature not meaned");
					if (met[month].rain > 0.0)
					{
						//met[month].met[day - i].ta_min
						soilSurfaceTemperature += (met[month].tavg -2 ) + ((float)wetDays / (float)met[month].n_days) *
								((float)met[month].tavg - (float)met[month].tavg -2);
						Log("\ncurrent day soilSurfaceTemperature: %g °C \nprevious day soilSurfaceTemperature: %g °C", soilSurfaceTemperature, soilSurfaceTempPrevious);
					}
					else
					{
						//met[month].met[day - i].ta_max
						soilSurfaceTemperature += met[month].tavg +2 + (dryDays / met[month].n_days) * (met[month].tavg +2 - met[month].tavg);
						Log("\ncurrent day soilSurfaceTemperature: %g °C \nprevious day soilSurfaceTemperature: %g °C", soilSurfaceTemperature, soilSurfaceTempPrevious);
					}
				}
				else if (month == FEBRUARY)
				{
					for (i = 2; i <= 0; i--)
					{
						//to be substituted with month met[month].met[day].rain
						if (met[month].rain > 0.0)
						{
							//met[month].met[day - i].ta_min
							soilSurfaceTemperature += (met[month - i].tavg -2 ) + (wetDays / met[month].n_days) * (met[month].tavg - met[month - i].tavg -2);
							Log("\npartial day (rainy) soilSurfaceTemperature: %g °C ", soilSurfaceTemperature);
						}
						else
						{
							//met[month].met[day - i].ta_max
							soilSurfaceTemperature += met[month - i].tavg +2 + (dryDays / met[month].n_days) * (met[month - i].tavg +2 - met[month].tavg);
							Log("\npartial day (dry) soilSurfaceTemperature: %g °C ", soilSurfaceTemperature);
						}
					}
					soilSurfaceTemperature /= 2;
					Log("\ncurrent day soilSurfaceTemperature: %g °C \nprevious day soilSurfaceTemperature: %g °C", soilSurfaceTemperature, soilSurfaceTempPrevious);
				}
				else if (month == MARCH)
				{
					for (i = 3; i <= 0; i--)
					{
						//to be substituted with month met[month].met[day].rain
						if (met[month].rain > 0.0)
						{
							//met[month].met[day - i].ta_min
							soilSurfaceTemperature += (met[month - i].tav -2 ) + (wetDays / met[month].n_days) * (met[month].tav - met[month - i].tav -2);
							Log("\npartial day (rainy) soilSurfaceTemperature: %g °C ", soilSurfaceTemperature);
						}
						else
						{
							//met[month].met[day - i].ta_max
							soilSurfaceTemperature += met[month - i].tav +2 + (dryDays / met[month].n_days) * (met[month - i].tav +2 - met[month].tav);
							Log("\npartial day (dry) soilSurfaceTemperature: %g °C ", soilSurfaceTemperature);
						}
					}
					soilSurfaceTemperature /= 3;
					Log("\ncurrent day soilSurfaceTemperature: %g °C \nprevious day soilSurfaceTemperature: %g °C", soilSurfaceTemperature, soilSurfaceTempPrevious);
				}
				else if (month == APRIL)
				{
					for (i = 4; i <= 0; i--)
					{
						//to be substituted with month met[month].met[day].rain
						if (met[month].rain > 0.0)
						{
							//met[month].met[day - i].ta_min
							soilSurfaceTemperature += (met[month - i].tav -2 ) + (wetDays / met[month].n_days) * (met[month].tav - met[month - i].tav -2);
							Log("\npartial day (rainy) soilSurfaceTemperature: %g °C ", soilSurfaceTemperature);
						}
						else
						{
							//met[month].met[day - i].ta_max
							soilSurfaceTemperature += met[month - i].tav +2 + (dryDays / met[month].n_days) * (met[month - i].tav +2 - met[month].tav);
							Log("\npartial day (dry) soilSurfaceTemperature: %g °C ", soilSurfaceTemperature);
						}
					}
					soilSurfaceTemperature /= 4;
					Log("\ncurrent day soilSurfaceTemperature: %g °C \nprevious day soilSurfaceTemperature: %g °C", soilSurfaceTemperature, soilSurfaceTempPrevious);
				}
				else
				{
					for (i = 5; i <= 0; i--)
					{
						//to be substituted with month met[month].met[day].rain
						if (met[month].rain > 0.0)
						{
							//met[month].met[day - i].ta_min
							soilSurfaceTemperature += (met[month - i].tav -2 ) + (wetDays / met[month].n_days) * (met[month].tav - met[month - i].tav -2);
							Log("\npartial day (rainy) soilSurfaceTemperature: %g °C ", soilSurfaceTemperature);
						}
						else
						{
							//met[month].met[day - i].ta_max
							soilSurfaceTemperature += met[month - i].tav +2 + (dryDays / met[month].n_days) * (met[month - i].tav +2 - met[month].tav);
							Log("\npartial day (dry) soilSurfaceTemperature: %g °C ", soilSurfaceTemperature);
						}
						//develope a system for the first 4 days of the very frst year of simulation
					}
					//estimate of bare soil surface temperature in the ith day
					soilSurfaceTemperature /= 5;
					Log("current day soilSurfaceTemperature: %g °C \nprevious day soilSurfaceTemperature: %g °C", soilSurfaceTemperature, soilSurfaceTempPrevious);
				}
*/
			}

			if (month == JANUARY)		//todo add && day == 1)
			{
				snowCoverLagFactor = 0.0;
				Log("\nthe very first cycle of the year! previous cycle effect imposed to 0!");
			}
			else
			{
				//snowCoverLagFactor = lagging factor simulating residues and and snow cover effects on soil surface
				snowCoverLagFactor = Maximum((abovegroundFraction / (abovegroundFraction + exp(5.3396 - 2.3951 * abovegroundFraction))),(snow * 10.0/
						(snow * 10.0 + exp(2.303 - 0.2197 * snow * 10.0))));
				Log("not the very first day(month) of the year: \nlagging factor snowCoverLagFactor: %g", snowCoverLagFactor);
			}
			//estimate of soil surface temperature considering biomass lag effect
			soilSurfaceTemperature = snowCoverLagFactor * soilSurfaceTempPrevious + (1 - snowCoverLagFactor) * soilSurfaceTemperature;
			Log("\n\nHence, soil surface temperature: %g\n", soilSurfaceTemperature);




			/*******************************************************************************************************
			 * 							PARTON SOIL TEMPERATURE ROUTINE (1984)
			 ******************************************************************************************************/


			Log("\n***** Parton (1984) estimation of soil temperature *****");
			//epi_biomass = 0.0;
			//soil surface temperature (By Parton, 1989)
			//soil max temperature (daily)
			epigeousBiomass = exp(-0.0048 * abovegroundFraction) - 0.13;				//g/m^2
			Log("\nParton epigeousBiomass: %g", epigeousBiomass);
			deltaSoilAirTemperature = 24.07 * (1.0 - exp(-0.000038 * dailySolarRadiation));		//solar radiation in Kj/dm^2)
			Log("\nParton SoilAirTemperature: %g", deltaSoilAirTemperature);
			partonSoilSurfaceMinTemperature = minAirTemperature + 0.006 * epigeousBiomass - 1.82;
			Log("\nParton SoilSurfaceMinTemperature: %g", partonSoilSurfaceMinTemperature);
			partonSoilSurfaceMaxTemperature = maxAirTemperature + (deltaSoilAirTemperature + 0.35 * maxAirTemperature) * epigeousBiomass;
			Log("\nParton SoilSurfaceMaxTemperature: %g", partonSoilSurfaceMaxTemperature);
			partonSoilSurfaceMeanTemperature = 0.41 * partonSoilSurfaceMaxTemperature + 0.59 * partonSoilSurfaceMinTemperature;
			Log("\nParton SoilSurfaceMeanTemperature: %g", partonSoilSurfaceMeanTemperature);

			/***************************************************************************************************************************
			 *
			 * 		ATTENTION!!! THIS ROUTINE IS RELATED TO PLANT DEVELOPMENT; TO AVOID CRASHES IS PLACED HERE FOR NOW
			 * 							REMEMBER TO PLACE IN EACH STAGE, WITH ITS DIFFERENCES ONCE DEBUGGED
			 **************************************************************************************************************************/

			/*********************************************************************************************************************************
			 * 											SOIL WATER MODULE
			 *******************************************************************************************************************************/

			//daily maximum snow melt
			dailyMaximumSnowMelt = 0.07 * meanCanopyTemperature;

			//daily maximum crop interception (cm of water)
			dailyMaximumPlantInterception = 0.02 * LAI;
			Log("\nDaily maximum snow melt: %g \nDaily maximum plant interception: %g", dailyMaximumSnowMelt);


			Log("\n******* CALCULATING WATER SURFACE RUNOFF: CURVE NUMBER ESTIMATION METHOD ********\n\n");
			//CN usually varies from 30 to 100; SCS curve number model

			// look at SCS method
			Log ("\nCURVE NUMBER VALUE\n");

			/***********************************************************************************************************************
											CURVE NUMBER SETTING TODO stagione riposo: set it using development module
			 ***********************************************************************************************************************/
			//TODO ATTENTION!!!! THIS IS COMPLETELY NONSENSE!!! IT IS USELESS TO USE THIS APPROACH ON A MONTHLY BASED MODEL; ITS USE IS
			// JUST FUNCTION OF A DEBUGGING APPROACH

			//todo develope an automated CN setting, based on silt/sand/clay % approach


			/***************************************************************************
			 * 											RUNOFF CURVE NUMBER
			 **************************************************************************/

			if(strncmp (Luse,"fallow", 4) == 0)
			{
				if (strncmp(pract,  "straightRow", 4) == 0)
				{
					switch (hydroGroup)
					{
					case 1:
						CN = 77;
						break;
					case 2:
						CN = 86;
						break;
					case 3:
						CN = 91;
						break;
					case 4:
						CN = 94;
						break;
					}
				}
			}
			else if (strncmp(Luse,  "RowCrops", 4) == 0)
			{
				if (strncmp(pract,  "straightRow", 4) == 0)
				{
					if (hydroCondition == 0)	// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 72;
							break;
						case 2:
							CN = 81;
							break;
						case 3:
							CN = 88;
							break;
						case 4:
							CN = 91;
							break;
						}
					}
					if (hydroCondition == 1)
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 67;
							break;
						case 2:
							CN = 78;
							break;
						case 3:
							CN = 85;
							break;
						case 4:
							CN = 89;
							break;
						}
					}
				}
				else if (strncmp(pract,  "contoured", 4) == 0)
				{
					if (hydroCondition == 0)	// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 70;
							break;
						case 2:
							CN = 79;
							break;
						case 3:
							CN = 84;
							break;
						case 4:
							CN = 88;
							break;
						}
					}
					if (hydroCondition == 1)
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 65;
							break;
						case 2:
							CN = 75;
							break;
						case 3:
							CN = 82;
							break;
						case 4:
							CN = 86;
							break;
						}
					}
				}
				else if (strncmp(pract,  "terraced", 4) == 0)
				{
					if (hydroCondition == 0)	// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 66;
							break;
						case 2:
							CN = 74;
							break;
						case 3:
							CN = 80;
							break;
						case 4:
							CN = 82;
							break;
						}
					}
					else if (hydroCondition == 1)
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 62;
							break;
						case 2:
							CN = 71;
							break;
						case 3:
							CN = 78;
							break;
						case 4:
							CN = 81;
							break;
						}
					}
				}
			}

			else if (strncmp(Luse,  "smallgrain", 4) == 0)
			{
				if (strncmp(pract,  "straightRow", 4) == 0)
				{
					if (hydroCondition == 0)// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 65;
							break;
						case 2:
							CN = 76;
							break;
						case 3:
							CN = 84;
							break;
						case 4:
							CN = 88;
							break;
						}
					}
					if (hydroCondition == 1)
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 63;
							break;
						case 2:
							CN = 75;
							break;
						case 3:
							CN = 83;
							break;
						case 4:
							CN = 87;
							break;
						}
					}
				}
				else if (strncmp(pract,  "contoured", 4) == 0)
				{
					if (hydroCondition == 0)// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 63;
							break;
						case 2:
							CN = 74;
							break;
						case 3:
							CN = 82;
							break;
						case 4:
							CN = 85;
							break;
						}
					}
					if (hydroCondition == 1)
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 61;
							break;
						case 2:
							CN = 73;
							break;
						case 3:
							CN = 81;
							break;
						case 4:
							CN = 84;
							break;
						}
					}
				}
				else if (strncmp(pract,  "terraced", 4) == 0)
				{
					if (hydroCondition == 0)// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 61;
							break;
						case 2:
							CN = 72;
							break;
						case 3:
							CN = 79;
							break;
						case 4:
							CN = 82;
							break;
						}
					}
					else if (hydroCondition == 1)
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 59;
							break;
						case 2:
							CN = 70;
							break;
						case 3:
							CN = 78;
							break;
						case 4:
							CN = 81;
							break;
						}
					}
				}
			}
			else if (strncmp(Luse,  "legumes", 4) == 0)
			{
				if (strncmp(pract,  "straightRow", 4) == 0)
				{
					if (hydroCondition == 0)// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 66;
							break;
						case 2:
							CN = 77;
							break;
						case 3:
							CN = 85;
							break;
						case 4:
							CN = 89;
							break;
						}
					}
					if (hydroCondition == 1)
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 58;
							break;
						case 2:
							CN = 72;
							break;
						case 3:
							CN = 81;
							break;
						case 4:
							CN = 85;
							break;
						}
					}
				}
				else if (strncmp(pract,  "contoured", 4) == 0)
				{
					if (hydroCondition == 0)// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 64;
							break;
						case 2:
							CN = 75;
							break;
						case 3:
							CN = 83;
							break;
						case 4:
							CN = 85;
							break;
						}
					}
					if (hydroCondition == 1)
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 55;
							break;
						case 2:
							CN = 69;
							break;
						case 3:
							CN = 78;
							break;
						case 4:
							CN = 83;
							break;
						}
					}
				}
				else if (strncmp(pract,  "terraced", 4) == 0)
				{
					if (hydroCondition == 0)// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 63;
							break;
						case 2:
							CN = 73;
							break;
						case 3:
							CN = 80;
							break;
						case 4:
							CN = 83;
							break;
						}
					}
					else if (hydroCondition == 1)
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 51;
							break;
						case 2:
							CN = 67;
							break;
						case 3:
							CN = 76;
							break;
						case 4:
							CN = 80;
							break;
						}
					}
				}
			}
			else if (strncmp(Luse,  "pasture", 4) == 0)
			{

				if (strncmp(pract,  "straightRow", 4) == 0)
				{
					if (hydroCondition == 0)// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 68;
							break;
						case 2:
							CN = 79;
							break;
						case 3:
							CN = 86;
							break;
						case 4:
							CN = 89;
							break;
						}
					}
					if (hydroCondition == 1)// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 49;
							break;
						case 2:
							CN = 69;
							break;
						case 3:
							CN = 79;
							break;
						case 4:
							CN = 84;
							break;
						}
					}
					if (hydroCondition == 2)
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 39;
							break;
						case 2:
							CN = 61;
							break;
						case 3:
							CN = 74;
							break;
						case 4:
							CN = 80;
							break;
						}
					}
				}
				else if (strncmp(pract,  "contoured", 4) == 0)
				{
					if (hydroCondition == 0)// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 47;
							break;
						case 2:
							CN = 67;
							break;
						case 3:
							CN = 81;
							break;
						case 4:
							CN = 88;
							break;
						}
					}
					if (hydroCondition == 1)
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 25;
							break;
						case 2:
							CN = 59;
							break;
						case 3:
							CN = 75;
							break;
						case 4:
							CN = 83;
							break;
						}
					}
					if (hydroCondition == 2)// 0 stands for poor; 1 for good
					{
						switch (hydroGroup)
						{
						case 1:
							CN = 6;
							break;
						case 2:
							CN = 35;
							break;
						case 3:
							CN = 70;
							break;
						case 4:
							CN = 79;
							break;
						}
					}
				}
			}

			else
			{
				CN = 50;
			}

			Log("\nCurveNumber %g", CN);

			//--------------------------------------------------------------------------------------------------------------------


			//SCS CN correction method:
			/*
			if (stage >= 3 && stage < 8)
			{
				if(met[month-4].rain + met[month-3].rain + met[month-2].rain + met[month-1].rain + met[month].rain < 13)
				{
					CN = 4.2 * CN / (10.0 - 0.058 * CN);
				}
				else if (met[month-4].rain + met[month-3].rain + met[month-2].rain + met[month-1].rain + met[month].rain > 28)
				{
					CN = 23.0 * CN / (10.0 - 0.058 * CN);
				}
			}
			else
			{
				if (month > 4) // it would be if day > 4° of the year
				{
					if(met[month-4].rain + met[month-3].rain + met[month-2].rain + met[month-1].rain + met[month].rain < 36)
					{
						CN = 4.2 * CN / (10.0 - 0.058 * CN);
					}
					else if (met[month-4].rain + met[month-3].rain + met[month-2].rain + met[month-1].rain + met[month].rain > 54)
					{
						CN = 23.0 * CN / (10.0 - 0.058 * CN);
					}
				}
			}
			Log("\nCurveNumber corrected by SCS methodn on previous rainy days number: %g", CN);
			 *************************************************************************************************************************************/

			//retention factor
			maxInfiltration = 254.0 * (100 - CN) / CN;
			if(met[month].rain >= 0.2 * maxInfiltration)
			{
				//Runoff
				Runoff = 0.1 * pow((met[month].rain - 0.2),2) / (met[month].rain + 0.8 * maxInfiltration);
			}else
			{
				//Runoff
				Runoff = 0;
			}
			Log("\nfor a retention factor: %g", maxInfiltration);
			Log("\nand for a monthly rainfall of %g mm", met[month].rain);
			Log("\nestimated runoff: %g\n", Runoff);


			if (met[month].swc != 9999)
			{
				//soil water content from input: it is a whole profile value
				profileWaterContent = met[month].swc;
			}
			else
			{
				profileWaterContent = 0;
				for (l = 0; l < soilLayer; l++)
				{
					profileWaterContent += layerMoisture[l];
				}
			}

			//********************************************************************************************************************************
			//------------------------ SOIL WATER BALANCE PARTIALLY TAKEN FROM CERES WHEAT 2.0 -----------------------------
			//********************************************************************************************************************************

			Log("\n**************************************\n"
					"soil water balance (CERES WHEAT 2.0)\n"
					"****************************************\n");
			//ratio of actual to potentially-extractable soil water (SWR) for layer 1 (the upper layer)
			//DUL : upper limit of soil water content
			//SWR = (SW[0] - LL[0])/(fieldCapacityLayerMoisture[0] - LL[0]);			to be used in case of ceres CN correction usage
			for (l = 0; l < soilLayer; l++)
			{
				//assumed as infiltrated water, the net amount of (precipitation + melt) - (runoff and interception)
				infiltration = (met[month].rain + dailyMaximumSnowMelt - Runoff - dailyMaximumPlantInterception) / 10.0;	//whatch out if the variablesare set in mm; here in cm
				Log("\ninfiltration %g", infiltration);
				//todo flux computation has to be confirmed
				if (l == 0)
				{
					//supposed by me: at the very first layer all the water infiltrated moves from the surface to this layer
					flux = infiltration;
					Log("\nflux: %g", flux);
				}

				if (flux > 0)
				{
					Hold[l] = (layerFieldSaturation[l] - layerMoisture[l]) * soilLayerThickness[l];
					Log("\nhold[%d]: %g", l, Hold[l]);
					if (flux <= Hold[l])
					{
						layerMoisture[l] += flux / soilLayerThickness[l];	//water = previous water + (new water / soil thickness)
						Log("\nflux < hold; SW: %g", layerMoisture[l]);
						//at this point is evaluated if infiltration is greater than layer field capacity; if yes there's infiltration
						if ( layerMoisture[l] < fieldCapacityLayerMoisture[l])
						{
							drain[l] = 0.0;
							Log("\nlayerMoisture < fieldCapacity: drain = %g", drain[l]);
						}
						else
						{
							drain[l] = (layerMoisture[l] - fieldCapacityLayerMoisture[l]) * SWcon * soilLayerThickness[l];
							flux = drain[l];
							Log("\nlayerMoisture > fieldCapacity: drain = %g", drain[l]);
							layerMoisture[l] = layerMoisture[l] - drain[l] / soilLayerThickness[l];
							Log("\nnew SW[l]: %g", layerMoisture[l]);
						}
					}
					else
					{
						drain[l] = SWcon * (layerFieldSaturation[l] - fieldCapacityLayerMoisture[l]) * soilLayerThickness[l];
						flux = flux - Hold[l] + drain[l];
						Log("\nflux > hold: drain[%d] = %g\n\tflux = %g",l,drain[l],flux);
					}
					Log("\n\n****UPWARD MOVEMENT****");
					normVolumetricWater[l] = Maximum(layerMoisture[l] - layerWilting[l],0);		// LL[i] constant (wilting point)
					Log("\nnormalizedVolumetricWaterContent %g", normVolumetricWater[l]);
				}
			}


			/*******************************************************************************************************************************
			 * 											EVAPOTRANSPIRATION MODULE
			 ******************************************************************************************************************************/

			//equilibrium evapotranspiration (solar rad is supposed Mj m-2 day-1; EEq cm day-1)
			equilibriumEvapotranspiration = 0.0001 * dailySolarRadiation * (4.88 - 4.37 * albedo) * (canopyDaytimeMeanTemperature + 29);	//equilibriumEvapotranspiration
			Log("\nEquilibrium evapotranspiration: %g", equilibriumEvapotranspiration);
			Log("\ncanopy temperature %g", canopyDaytimeMeanTemperature);

			//Potential evapotranspiration
			if (canopyMaxTemperature < 5)
			{
				potentialEvapotranspiration = 0.01 * equilibriumEvapotranspiration * exp (0.18 * (canopyMaxTemperature + 20));
			}
			else if (canopyMaxTemperature >= 5 && canopyMaxTemperature < 24)
			{
				potentialEvapotranspiration = 1.1 * equilibriumEvapotranspiration;
			}
			else
			{
				//version of Ritchie, 88, from whom taken the other ones
				potentialEvapotranspiration = equilibriumEvapotranspiration * ((canopyMaxTemperature - 24.0) * 0.05 +1.1);
				//version read in zhang 2002
				//potentialEvapotranspiration = equilibriumEvapotranspiration * (1-0.43 * LAI);
			}


			// potential evaporation
			if (LAI < 1)
			{
				potentialEvaporation = potentialEvapotranspiration * (1 - 0.43 * LAI);
			}
			else
			{
				potentialEvaporation = potentialEvapotranspiration / 1.1 * exp (-0.4 * LAI);
			}


			/*******************************************************************************
			 * 					RITCHIE ACTUAL RATE OF SOIL EVAPORATION
			 ******************************************************************************/
			/*
			Log("\n*****SOIL EVAPORATION MODULE*****");

			if (esStage2 == 0)
			{
				if (met[month].rain > 0.0)
				{
					if (infiltration >= soilEvaporationSum1)
					{
						soilEvaporationSum1 = 0;
					}
					else
					{
						soilEvaporationSum1 -= infiltration;
					}
				}
				if (soilEvaporationSum1 < stageUpperLimit1)
				{
					soilEvaporationSum1 += potentialEvaporation;
					if (soilEvaporationSum1 <= stageUpperLimit1)
					{
						actualTranspiration = potentialEvaporation;
					}
					else
					{
						actualTranspiration = potentialEvaporation - 0.4 * (soilEvaporationSum1 - stageUpperLimit1);
						soilEvaporationSum2 = 0.6 * (soilEvaporationSum1 - stageUpperLimit);
						if (stage2EvaporationLagTime == 0 )
						{
							stage2EvaporationLagTime = pow(soilEvaporationSum2 / 3.5, 2);
						}
						else
						{
							stage2EvaporationLagTime++;
						}
						esStage2 = 1;

					}
				}
			}
			if (esStage2 == 1)
			{

				actualTranspiration = 3.5 * pow(stage2EvaporationLagTime, 0.5) -soilEvaporationSum2;
				if (actualTranspiration < potentialEvaporation)
				{
					actualTranspiration = potentialEvaporation;
				}
				soilEvaporationSum2 += actualTranspiration - infiltration;
				if (infiltration > soilEvaporationSum2)
				{
					esStage2 = 0;
					stage2EvaporationLagTime = 0;
					soilEvaporationSum1 = stageUpperLimit1 - infiltration +soilEvaporationSum2;

				}
			}
			Log("\n*****END OF SOIL EVAPORATION MODULE*****");

			deltaMoisture = layerMoisture[0] - actualTranspiration;
			if (deltaMoisture < driestSoilWaterContent * layerWilting[0])
			{
				actualTranspiration = deltaMoisture - layerMoisture[0];
				layerMoisture[0] = driestSoilWaterContent * layerWilting[0];

			}


			for (l = soilLayer; l > 1; l--)
			{

				//to be seen if new moisture is > fieldCapacity: if it's greater than field capacity what should be done!?
				// supposed: if the lower layer is full, then put it on the upper one; if it is the layer 0, it's a pool
				Diffusion = 0.88 * exp(35.4 * 0.5 *(normVolumetricWater[l-1] + normVolumetricWater[l])) *
						(normVolumetricWater[l] - normVolumetricWater[l-1]) / (soilLayerThickness[l] + soilLayerThickness[l-1]) * 0.5;
				Log("\ndiffusion flux = %g",Diffusion);
				//to be reviewed
				layerMoisture[l] -= Diffusion / soilLayerThickness[l];
				Log("\nnew SW[%d] %g",l,layerMoisture[l]);
				if(l-1 >= 0)
				{
					layerMoisture[l - 1] += Diffusion / soilLayerThickness[l-1];
					Log("\nnew SW[%d] %g",l-1,layerMoisture[l-1]);
				}
				//todo SW[l] correction
				//SW[l] += Diffusion
				Log("\nlayerMoisture: \n\t layer %d, %g \n\t layer %d, %g",l,layerMoisture[l],l-1,layerMoisture[l-1]);
			}
			if (LAI < 3)
			{
				potentialTranspiration = potentialEvapotranspiration * LAI /3
			}
			else
			{
				potentialTranspiration = potentialEvapotranspiration
			}
			if ( actualSoilEvaporation + potentialTranspiration > potentialEvapotranspiration)
			{
				potentialTranspiration = potentialEvapotranspiration - actualSoilEvaporation;
			}

			Log("\nEnd of soil waer balance module\n\n");


			Log("\n*******ACTUAL EVAPORATION*********");
			for( l = 0 ; l < layerNumber; l++)
			{
			potentialRootUptake[l] = Maximum(0.00267 * exp(62 * (layerMoisture[l] - layerWilting[l]))/(6.68 - log(rootLengthDensity[l])), MAX_UPTAKE);

			//potential uptake transformed from cm^3/cm to cm water
			potentialRootUptake[l] = potentialRootUptake[l] * layerThickness[l] * rootLengthDensity[l];
			cropUptakeCapacity += potentialRootUptake[l];
			}
			if ( potentialTranspiration / cropUptakeCapacity <= 1.0)
			{
				actualTranspiration = potentialTranspiration;
			}
			else
			{
				for(l = 0 ; l < layerNumber; l++)
				{
					potentialRootUptake[l] *= potentialTranspiration / cropUptakeCapacity;
					//specific for cm3: how's in dndc!?
					layerMoisture[l] -= potentialRootUptake[l]/layerThickness[l];
				}
			}




			//############################################################################################################################################
			 */
			// potential transpiration
			potentialTranspiration = potentialEvapotranspiration - potentialEvaporation;
			Log("\nfor a potential evapotranspiration of: %g \npotential transpiration: %g", potentialEvapotranspiration, potentialTranspiration);


			//effect of soil moisture on evaporation
			for (l = 0; l < soilLayer; l++)
			{
				moistureSoilEvaporationEffect += ((layerMoisture[l] - layerWilting[l]) / (fieldCapacityLayerMoisture[l] - layerWilting[l])) * soilLayerThickness[l] / site->soil_depth;
			}
			//moistureSoilEvaporationEffect /= site->soil_depth;
			Log("\nSoil moisture effect on evaporation is: %g\n", moistureSoilEvaporationEffect);

			//actual soil evaporation
			actualSoilEvaporation = potentialEvaporation * moistureSoilEvaporationEffect;

			deltaMoisture = layerMoisture[0] - actualSoilEvaporation;
			if (deltaMoisture < driestSoilWaterContent * layerWilting[0])
			{
				actualSoilEvaporation = deltaMoisture - layerMoisture[0];
				layerMoisture[0] = driestSoilWaterContent * layerWilting[0];
			}

			//________________________________________________________________________________________________________________________
			//MOVE IN CROP DEVELOPMENT
			//Crop uptake capacity
			for (l = 0; l < soilLayer; l++ )
			{
				moistureEffectWaterUptake1[l] = sin((layerMoisture[l] - layerWilting[l])/(fieldCapacityLayerMoisture[l] - layerWilting[l]) * 1.25 * Pi /2);

				//find out where computed  layerRootLengthDensity[l]
				//A Generalized Function of Wheat's Root Length Density Distributions

				//crop uptake capacity
				waterUptakeSoilLayer += rootWaterUptakeCoefficient * layerRootLengthDensity[l] / (0.2 + 0.2 * layerRootLengthDensity[l]) * moistureEffectWaterUptake1[l] * soilLayerThickness[l];

				Log("\nWater uptake cumulated till layer %d is: %g",l, waterUptakeSoilLayer);
			}
			Log("\nwater total uptake is %g", waterUptakeSoilLayer);

			//Actual transpiration
			actualTranspiration = Minimum(potentialTranspiration, waterUptakeSoilLayer);
			Log("\nActual transpiration: %g", actualTranspiration);

			/*********************************************************************************************************************************************************
			 * 												WATER REDISTRIBUITION
			 *********************************************************************************************************************************************************/



			//todo a surface flooding module
			for (l = soilLayer; l > 1; l--)
			{

				//to be seen if new moisture is > fieldCapacity: if it's greater than field capacity what should be done!?
				// supposed: if the lower layer is full, then put it on the upper one; if it is the layer 0, it's a pool
				Diffusion = 0.88 * exp(35.4 * 0.5 *(normVolumetricWater[l-1] + normVolumetricWater[l])) *
						(normVolumetricWater[l] - normVolumetricWater[l-1]) / (soilLayerThickness[l] + soilLayerThickness[l-1]) * 0.5;
				Log("\ndiffusion flux = %g",Diffusion);
				//to be reviewed
				layerMoisture[l] -= Diffusion / soilLayerThickness[l];
				Log("\nnew SW[%d] %g",l,layerMoisture[l]);
				if(l-1 >= 0)
				{
					layerMoisture[l - 1] += Diffusion / soilLayerThickness[l-1];
					Log("\nnew SW[%d] %g",l-1,layerMoisture[l-1]);
				}
				//todo SW[l] correction
				//SW[l] += Diffusion
				Log("\nlayerMoisture: \n\t layer %d, %g \n\t layer %d, %g",l,layerMoisture[l],l-1,layerMoisture[l-1]);
			}
			Log("\nEnd of soil waer balance module\n\n");

			/*******************************************************************************************************************************
			 * 											EVAPOTRANSPIRATION MODULE
			 ******************************************************************************************************************************/

			//equilibrium evapotranspiration (solar rad is supposed Mj m-2 day-1; EEq cm day-1)
			equilibriumEvapotranspiration = 0.0001 * dailySolarRadiation * (4.88 - 4.37 * albedo) * (canopyDaytimeMeanTemperature + 29);	//equilibriumEvapotranspiration
			Log("\nEquilibrium evapotranspiration: %g", equilibriumEvapotranspiration);
			Log("\ncanopy temperature %g", canopyDaytimeMeanTemperature);
			//Potential evapotranspiration
			if (canopyMaxTemperature < 5)
			{
				potentialEvapotranspiration = 0.01 * equilibriumEvapotranspiration * exp (0.18 * (canopyMaxTemperature + 20));
			}
			else if (canopyMaxTemperature >= 5 && canopyMaxTemperature < 24)
			{
				potentialEvapotranspiration = 1.1 * equilibriumEvapotranspiration;
			}
			else
			{
				//version of Ritchie, 88, from whom taken the other ones
				potentialEvapotranspiration = equilibriumEvapotranspiration * ((canopyMaxTemperature - 24.0) * 0.05 +1.1);
				//version read in zhang 2002
				//potentialEvapotranspiration = equilibriumEvapotranspiration * (1-0.43 * LAI);
			}


			// potential evaporation
			if (LAI < 1)
			{
				potentialEvaporation = potentialEvapotranspiration * (1 - 0.43 * LAI);
			}
			else
			{
				potentialEvaporation = potentialEvapotranspiration / 1.1 * exp (-0.4 * LAI);
			}

			// potential transpiration
			actualTranspiration = potentialEvapotranspiration - potentialEvaporation;
			Log("\nfor a potential evapotranspiration of: %g \npotential transpiration: %g", potentialEvaporation, actualTranspiration);



			//effect of soil moisture on evaporation
			for (l = 0; l < soilLayer; l++)
			{
				moistureSoilEvaporationEffect += ((layerMoisture[l] - layerWilting[l]) / (fieldCapacityLayerMoisture[l] - layerWilting[l])) * soilLayerThickness[l]/site->soil_depth;
			}
			//moistureSoilEvaporationEffect /= site->soil_depth;
			Log("\nSoil moisture effect on evaporation is: %g\n", moistureSoilEvaporationEffect);

			//actual soil evaporation
			actualSoilEvaporation = potentialEvaporation * moistureSoilEvaporationEffect;

			//________________________________________________________________________________________________________________________
			//MOVE IN CROP DEVELOPMENT
			//Crop uptake capacity
			for (l = 0; l < soilLayer; l++ )
			{
				moistureEffectWaterUptake1[l] = sin((layerMoisture[l] - layerWilting[l])/(fieldCapacityLayerMoisture[l] - layerWilting[l]) * 1.25 * Pi /2);

				//find out where computed  layerRootLengthDensity[l]
				//A Generalized Function of Wheat's Root Length Density Distributions

				//crop uptake capacity
				waterUptakeSoilLayer += rootWaterUptakeCoefficient * layerRootLengthDensity[l] / (0.2 + 0.2 * layerRootLengthDensity[l]) * moistureEffectWaterUptake1[l] * soilLayerThickness[l];

				Log("\nWater uptake cumulated till layer %d is: %g", waterUptakeSoilLayer);
			}
			Log("\nwater total uptake is %g", waterUptakeSoilLayer);

			//Actual transpiration
			actualTranspiration = Minimum(actualTranspiration, waterUptakeSoilLayer);
			Log("\nActual transpiration: %g", actualTranspiration);


			//Crop water stress factor
			waterStressFactor = actualTranspiration / potentialTranspiration;
			//_____________________________________________________________________________________________________________________________

			/*****************************************************************************************************
			 * 			SOIL TEMPERATURE AT DIFFERENT LAYER DEPTH: IMPORTANT! THIS MUST OCCURR AFTER SWC MODULE
			 **********************************************************************************************************/

			//computing soil temperature using T
			for (l = 0; l < soilLayer; l++)
			{
				Log("\n\n**** starting evaluation of soil temperature in different layers ****");
				Log("\nfor a %d number of soil layer used:", soilLayer);
				Log("\n****** LAYER %d ********\n", l);

				//assumed soil water content and bulk density as a soil constant

				//todo: whatch out if SW and BD have to be layer specific or the value for the whole profile
				mid2 = profileWaterContent / (0.356 - 0.144 * soilBulkDensity) * site->soil_depth;
				mid1 = 1.00 + 2.5 * soilBulkDensity / (soilBulkDensity + exp (6.53 - 5.63 * soilBulkDensity));
				Log("\n***Coefficients to evaluate depth weigthning factors ***\ncoeff1: %g \ncoeff2: %g", mid1, mid2);

				//damping depth: factor which simulates soil buffer effect on temperature oscillations:
				//the more the layer is deepened , the more temperature is constant during the year (function of annual mean temperature)
				dampingDepth[l] = mid1 * exp (log(0.5 / mid1) * pow(((1.0 - mid2)/(1.0 + mid2)), 2.0));
				Log("\ndamping depth: %g", dampingDepth[l]);


				temperatureFactor1[l] = (soilLayerThickness[l-1] + soilLayerThickness[l])/2.0 /dampingDepth[l];
				Log("\ntemperatureFactor1 factor: %g", temperatureFactor1[l]);
				//depth weightning factor; attention! it is computed in Mg/m^3!!!!!
				temperatureFactor2[l] = temperatureFactor1[l] / (temperatureFactor1[l] + exp(-0.867 - 2.08 * temperatureFactor1[l]));
				Log("\ndepth weigthning factor temperatureFactor2: %g (Mg/m^3)", temperatureFactor2[l]);

				if (month == JANUARY)		// todo: add && day == 1
				{
					lag = 0.0;
					Log("\nIt's january the first! no lag factor imposed!");
				}
				else
				{
					lag = 0.5;
					Log("\nlag factor imposed as %g as default", lag);
				}
				//soil temperature at the center of layer's depth
				soilLayerTemperature[l] = soilLayerTemperature[l] * lag + (1.0 - lag) * ((meanAnnualTemperature - soilSurfaceTemperature) * temperatureFactor2[l] + soilSurfaceTemperature);
				Log("\nFinally!!!! soilLayerTemperature at the center of the layer: %g", soilLayerTemperature[l]);

				//soil temperature at the center of layer's depth (parton)
				soilLayerTemperature[l] = soilLayerTemperature[l] * lag + (1.0 - lag) * ((meanAnnualTemperature - partonSoilSurfaceMeanTemperature) * temperatureFactor2[l] + soilSurfaceTemperature);
				Log("\n\nFinally!!!! soilLayerTemperature at the center of the layer: %g", soilLayerTemperature[l]);
			}
			Log("\nend of the soil temperature module (DNDC||EPIC)");
			//layer specific soil temperature, as estimated at the center of the layer
			Log("\nend of the soil temperature module (Parton)");
			//layer specific soil temperature, as estimated at the center of the layer

		}

		//--------------------------------------------------------------------------------------------------------------------------------------



		/*****************************************************************************************************************/

		//##################################################CHECKED: SEEMS OK!!!!



		/************************************************************************************************************************
		 *										   CROP NITROGEN AND DEVELOPMENT
		 ***********************************************************************************************************************/


		/************************************************************************************************************************
		 *										   CROP DEVELOPMENT SUBMODEL
		 **************************************************************************************************************************
		 * 											TO BE FURTHER DEVELOPED
		 **************************************************************************************************************************/


		//(or Tair)			chosen Tc(0) and Tc(1): to be clearly chosen
		//dailyThermalTime += meanCanopyTemperature / 2 - Tbasal) * d_ndays;

		if (meanCanopyTemperature - basalTemperature > 0 )	//&& stage != 3
		{
			dailyThermalTime += (meanCanopyTemperature - basalTemperature) * met[month].n_days;
			Log("\n\ndailyThermalTime: %g", dailyThermalTime);
			//dailyThermalTime2 += Minimum (maxDevelopmentTemperature - basalTemperature, Maximum(0, meanCanopyTemperature - basalTemperature)) * met[month].n_days;
			//Log("\n\ndailyThermalTime2: %g", dailyThermalTime2);
		}
		else if (meanCanopyTemperature - basalTemperature > 0 )	//&& stage == 3
		{
			//	dailyThermalTime += ((meanCanopyTemperature - basalTemperature)* met[month].n_days) * Minimum(VF,DF);
			//	dailyThermalTime2 += (Minimum (maxDevelopmentTemperature - basalTemperature, Maximum(0, meanCanopyTemperature - basalTemperature)) * met[month].n_days) * Minimum(VF,DF);
		}


		developmentRate = dailyThermalTime / stageLimit[stage];
		Log("\nfirst DR = %g",developmentRate);

		if (developmentRate >= 1.0)
		{
			dailyThermalTime -= stageLimit[stage];
			stage += 1;
			developmentRate = dailyThermalTime / stageLimit[stage];		//it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
			// stage rather than surfaceTemperatureAdjustment)
			Log("\n\ndailyThermalTime: %g", dailyThermalTime);
			Log("\nfirst DR = %g",developmentRate);
		}
		if (stop_cycle == 0)
		{
			if (species == 0)		//todo correct it using a char
			{

				/*********************************************************************************************************
				 * 							SPECIES/ VARIETAS SPECIFIC VARIABLES' SETTINGS
				 ********************************************************************************************************/




				//########################################################################################################

				switch(stage)		// todo sergio do it for the several different plant types: this case valid for wheat
				{
				case 0:
					Log("\n*****************"
							"\n\t* Emergence to terminal spikelet *"
							"\n****************");
					//(or Tair)			chosen Tc(0) and Tc(1): to be clearly chosen
					//dailyThermalTime += meanCanopyTemperature / 2 - Tbasal) * d_ndays;

					//my attempt:


					if (meanCanopyTemperature - basalTemperature > 0 )	//&& stage != 3
					{
						dailyThermalTime += ((meanCanopyTemperature - basalTemperature) * met[month].n_days)* Minimum(vernalizationFactor,photoperiodFactor);
						Log("\n\ndailyThermalTime: %g", dailyThermalTime);
						//dailyThermalTime2 += Minimum (maxDevelopmentTemperature - basalTemperature, Maximum(0, meanCanopyTemperature - basalTemperature)) * met[month].n_days;
						//Log("\n\ndailyThermalTime2: %g", dailyThermalTime2);
					}
					else if (meanCanopyTemperature - basalTemperature > 0 )	//&& stage == 3
					{
						//	dailyThermalTime += ((meanCanopyTemperature - basalTemperature)* met[month].n_days) * Minimum(VF,DF);
						//	dailyThermalTime2 += (Minimum (maxDevelopmentTemperature - basalTemperature, Maximum(0, meanCanopyTemperature - basalTemperature)) * met[month].n_days) * Minimum(VF,DF);
					}


					developmentRate = dailyThermalTime / stageLimit[stage];
					Log("\nfirst DR = %g",developmentRate);



					/*******************************************
					 * 				PHOTOSYNTHESIS
					 ******************************************/

					Log("\n\n******** PHOTOSYNTHESIS MODULE *********\n ");

					// Effects of CO2 concentration on photosynthesis
					fCO2 = 1 + co2EffectPhotosynthesis * log(site->co2Conc/340);
					Log("\neffects of CO2 concentration on photosynthesis: %g", fCO2);

					// Light which is perpendicular to leaf surface
					lightSaturationPhotoRate = ligthTempSaturationPhotoRate * layerSoilTempEffectPhotoRate;													// todo fTP
					Log("\nPhotosynthesis rate at light saturation: %g kg CO2/ha/h", lightSaturationPhotoRate);

					// TODO sergio: ATTENTION hourPar[t]!!!: do not forget it is the value for a certain time; this function will have to be proposed afterwards
					Log("\n\n******** PHOTOSINTETICALLY ACTIVE RADIATION AT CERTAIN TIME ********* \n ");
					//get photosynthetically active radiation at certain time


					// Three canopy layers for Gaussian integration
					for (l = 0; l < 3; l++)
					{
						//Canopy layer for gaussian integration
						canopyHeightLAI[l] = LAI * gaussianParameter1[l];
						Log("\nLAI above layer %d: %g",l, canopyHeightLAI[l]);
						//diffuseLightExtinctionCoeff ???????????????????

						Log("\nmissingn coefficient of diffuse light extinction");
						// canopy extinction coefficient: taken from wimovac (Campbell and Norman (1989))
						//todo validate

						for (i = 0; i < 3; i++)
						{
							// three points for Gaussian integration;
							gaussianIntegrationHour[i] = 12 + 0.5 * daylength * gaussianParameter1[i];
							Log("\nAssuming the gaussian integration, setted the hour %g,", gaussianIntegrationHour[i]);
							// canopy hourly temperature during daytime


							//sine of the elevation angle of the sun
							solarElevation = Sfactor + Cfactor * cos(2 * Pi * (gaussianIntegrationHour[i] + 12) / 24.0);
							Log("\nsine of the elevation angle of the sun: %g", solarElevation);

							//-------------------------------------------------------------------------------------------------------------------------------
							//solar elevation taken from the web, to comute zenit angle
							Selev = hourDay * 60.0 / 4 - 180.0;

							//canopy zenith angle
							zenit = acos(sin(site->lat)*sin(solarDeclination) + cos(site->lat)*cos(solarDeclination) * cos(Selev));

							// extinction coefficient of diffuse light
							diffuseLightExtinctionCoeff = (pow(pow(canopyShape, 2.0) + pow(tan(zenit), 2.0), 1 / 2) * cos(zenit)) / (canopyShape + 1.744 *
									pow(canopyShape + 1.183 , -0.733));

							//---------------------------------------------------------------------------------------------------------------------------------

							// Extinction coefficient of assumed black body leaves
							blackBodyExtinctionCoeff = (0.5 * diffuseLightExtinctionCoeff) / (0.8 * solarElevation * pow(1.0 - scatterLightParameter, 0.5));		//TODO sergio: attention!! no diffuseLightExtinctionCoeff formulation!!!
							Log("\nextinction coefficient of leaves, assuming them as black bodies: %g", blackBodyExtinctionCoeff);
							//Fraction of sunlit area
							blackBodyExtinctionCoeff = exp(-blackBodyExtinctionCoeff * canopyHeightLAI[l]);


							// mean par for the certain time;
							hourPar = 0.55 * dailySolarRadiation * solarElevation * (1 + 0.4 * solarElevation) /
									(3600.0 * (daylength * (Sfactor + 0.4 * pow(Sfactor, 2) +	0.2 * pow (Cfactor, 2)) + 12.0 * Cfactor *
											(2 + 1.2 * Sfactor * canopyHeightLAI[l] * Pi * sqrt(1 - (pow(Sfactor, 2.0) / pow(Cfactor , 2.0))))));
							Log("\nPhotosynthetic active radiation for the &d-th estimated hour: %g J/m^2/s", gaussianIntegrationHour[i], hourPar);

							// Atmospheric transmission coefficient
							atmosphericTrasmissionCoeff = hourPar * (0.5 * solarConstant * solarElevation);			//todo da capire
							Log("\nAtmospheric transmission coefficient: %g", atmosphericTrasmissionCoeff);

							// midVariable3 && midVariable4: mid coefficients
							midVariable4 = 0.847 - 1.61 * solarElevation * pow(solarElevation, 2);
							midVariable3 = (11.47 - midVariable4) / 1.66;
							Log("\nbeing the two mid coefficients midVariable3 and midVariable4, respectively: %g and %g",midVariable3,midVariable4);
							// Fraction of diffuse light above the canopy
							if (atmosphericTrasmissionCoeff <= 0.22)
							{
								diffuseLightFraction = 1.0;
							}
							else if (atmosphericTrasmissionCoeff > 0.22 && atmosphericTrasmissionCoeff <= 0.35)
							{
								diffuseLightFraction = 1 - 6.4 * pow((atmosphericTrasmissionCoeff - 0.22), 2.0);
							}
							else if (atmosphericTrasmissionCoeff <= midVariable3 && atmosphericTrasmissionCoeff > 0.35)
							{
								diffuseLightFraction = 1.47 - 1.66 * atmosphericTrasmissionCoeff;
							}
							else
							{
								diffuseLightFraction = midVariable4;
							}
							Log("\nfraction of diffuse light above canopy: %g",diffuseLightFraction);
							//todo import here Am f(diffuseLightFraction)
							//diffuse light above the canopy
							diffuseLightAboveCanopy = hourPar * diffuseLightFraction;
							Log("\ndiffuse light above canopy %g", diffuseLightAboveCanopy);

							//direct light above the canopy
							directLightAboveCanopy = hourPar - diffuseLightAboveCanopy;
							Log("\ndirect light above the canopy %g",directLightAboveCanopy);



							//extinction coefficient of direct light
							directLightExtinctionCoeff = blackBodyExtinctionCoeff * sqrt((1 - scatterLightParameter));
							Log("\nextinction coefficient of direct light %g",directLightExtinctionCoeff);

							// Reflectivity of horizontally distribuited canopy
							canopyHorizontalReflectivity = (1 - sqrt((1 - scatterLightParameter))) / (1 + sqrt((1-scatterLightParameter)));
							Log("\nreflectivity of horizontally distribuited canopy %g", canopyHorizontalReflectivity);

							// Reflectivity of sphaerical distribuited canopy
							canopySpharicalReflectivity = 2 * canopyHorizontalReflectivity / (1 + 2.0 * solarElevation);
							Log("\nreflectivity of sphaerical distribuition canopy %g", canopySpharicalReflectivity);

							// Direct component of the direct light after canopy scatterLightParametering
							directComponentDirectLight = (1 - scatterLightParameter) * directLightAboveCanopy * blackBodyExtinctionCoeff * exp(- blackBodyExtinctionCoeff * canopyHeightLAI[l]); // l_c < 3; l++;
							Log("\ndirect light in canopy if the leaves are non scattered back: %g", directComponentDirectLight);

							// Direct light
							directLight = (1 - canopySpharicalReflectivity) * directLightAboveCanopy * directLightExtinctionCoeff * exp(- directLightExtinctionCoeff * canopyHeightLAI[l]); // l_c < 3; l++;
							Log("\ndirect light accepted by sunlit leaves: %g", directLight);
							// Diffuse light
							diffuseLight = (1 - canopyHorizontalReflectivity) * diffuseLightAboveCanopy * blackBodyExtinctionCoeff * exp(- diffuseLightExtinctionCoeff * canopyHeightLAI[l]); // l_c < 3; l++;			// TODO SERGIO: diffuseLightExtinctionCoeff mancante
							Log("\ndiffuse light accepted by sunlit leaves: %g", diffuseLight);
							//Light absorbed by shaded leaves in a layer
							shadeLeavesLightAbsor = diffuseLight + (directLight - directComponentDirectLight);
							Log("\nLight assorbed by shaded leaves in a canopy layer:%g (J/m^2/s)", shadeLeavesLightAbsor);
							// Light which is perpendicular to leaf surface
							leafSurface90degLight = (1 - scatterLightParameter) * directLightAboveCanopy / solarDeclination;
							Log("\nLight perpendicular to leaf surface: %g (J/m^2/s)", leafSurface90degLight);
							//Photosynthesis rate of shaded leaves

							//layerSoilTempEffectPhotoRate and Asm0 are parameters!!


							shadLeavesPhotoRate = lightSaturationPhotoRate * (1 - exp(-shadeLeavesLightAbsor * initialLightUseEfficiency / lightSaturationPhotoRate));				//E = todo initial light use efficiency???????????????
							Log("\nPhotosynthesis rate of shaded leaves: %g (kg CO2/ha/h)",shadLeavesPhotoRate);
							//Photosynthesis rate of sunlit leaves
							sunlitLeavesPhotoRate = lightSaturationPhotoRate * (1 - (lightSaturationPhotoRate - shadLeavesPhotoRate)) * (1- exp(-leafSurface90degLight * initialLightUseEfficiency / lightSaturationPhotoRate))/(leafSurface90degLight * initialLightUseEfficiency); //-k * LAI, trasmitted;
							Log("\nPhotosynthesis rate of sunlit leaves: %g (kg CO2/ha/h)", sunlitLeavesPhotoRate);

							//fraction of sunlit area
							blackBodyExtinctionCoeff = exp(- blackBodyExtinctionCoeff * canopyHeightLAI[l]);
							Log("\nFraction of sunlit area: %g", blackBodyExtinctionCoeff);
							//Gross photosynthesis rate at layer Li and time tj
							P_Li_t = blackBodyExtinctionCoeff * sunlitLeavesPhotoRate + (1 - blackBodyExtinctionCoeff) * shadLeavesPhotoRate;
							Log("Gross photosynthesis rate at crop layer %d: %g ( Kg CO2/ha/h)",l, P_Li_t);

							//mean value: sum(P(Li,t[j]) * LAI * DL * gaussianParameter2j * gaussianParameter2i), l_c)
							gaussIntegrPhoto += P_Li_t * LAI * daylength * gaussianParameter2[l] * gaussianParameter2[i];
							Log("\n partial denominator in the photosynthesis module: %g",gaussIntegrPhoto);
						}

						gaussIntegPhotoSum += gaussIntegrPhoto;
						gaussIntegrPhoto = 0;
						Log("\nformatting gaussIntegrPhoto value: %g\nPartial denominator: %g", gaussIntegrPhoto, gaussIntegPhotoSum);

					}

					// daily gross photosynthesis (g/m^2)
					dailyGrossPhoto = 0.1 * 30 / 44 * Minimum(waterStressFactor,nitrogenStressFactor) * fCO2 * gaussIntegPhotoSum; //dailyGrossPhoto = 0.1×30/44min(waterStressFactor,ns)fCO2 sum(sum(P(Li,tj )LAI DLw2j w2)3, 3)
					Log("\ndaily gross photosynthesis: %g g/m^2", dailyGrossPhoto);

					/****************************************************************
					 * 						RESPIRATION
					 ***************************************************************/

					Log(" ********** RESPIRATION ***********\n");

					// maintenance respiration: k stands for the number of comparts
					for (i = 0; i < 4; i++)
					{
						maintenanceRespiration += compartMaintenanceResp[i] * pow(Q10, (meanCanopyTemperature - 25)/10.0) * compartBiomass[i];
						Log("\nMaintenanace respiration cumulated for %d compartments: %g g CO2/m^2\n", i, maintenanceRespiration);

					}
					//growth respiration
					growthRespiration = (dailyGrossPhoto - maintenanceRespiration) * (1 - 1 / growthRespEfficiency);
					Log ("Maintenance total respiration: %g\nGrowth respiration: %g\n", maintenanceRespiration, growthRespiration);

					dailyAssimilate = dailyGrossPhoto - growthRespiration - maintenanceRespiration;  //assumed as the difference between GPP and Resp
					/*******************************************
					 * 			   	LEAF GROWTH
					 ******************************************/


					leafAreaGrowthRate = 7.5 * sqrt(cumPhyllocrons) * dailyThermalTime / phyllocron;
					plantLeafAreaGrowthRate = leafAreaGrowthRate * tillNumber;


					//uncertainties: should vary much more
					assimilateAreaToWeight = 150.0 - 0.075 * dailyThermalTime;

					//mass of assimilates required tosupport that amount of daily expansion growth
					potentialLeafGrowth = plantLeafAreaGrowthRate  / assimilateAreaToWeight;

					//remainder of the daily assimilate supply
					potentialRootGrowth = dailyAssimilate - potentialLeafGrowth;

					if (potentialRootGrowth < dailyAssimilate * .35)
					{
						potentialRootGrowth = dailyAssimilate * .35;
						potentialLeafGrowth = dailyAssimilate * .65;
					}

					totalCumulativeLeafArea += plantLeafAreaGrowthRate;

					/*****************************************
					 * 				TILLERING
					 ***************************************/

					//tillers
					tillerRate1 = -2.5 + cumPhyllocrons;
					tillersPerSquareMeter = plants * tillNumber;

					tillerRate2 = 2.56 * exp(-10.0) * pow(2500 - tillersPerSquareMeter, 3.0);

					tillNumber += dailyThermalTime / phyllocron * Minimum(tillerRate1, tillerRate2);

					//tillering factor corrected with SWDF2 from water balance routine



					/******************************************
					 * 				LEAF SENESCENCE
					 *****************************************/

					if (phylloCounter >= phyllocron)
					{
						leafNumber++;
						phylloCounter -= phyllocron;
						for(l = 0; l < leafNumber - 1 ; l++)
						{
							cumulativeLeafAreaPhyllocron[l +1] = cumulativeLeafAreaPhyllocron[l];
						}
						cumulativeLeafAreaPhyllocron[4] = totalCumulativeLeafArea; //here was cumulative leaf area
						cumPhyllocrons ++;
					}

					//only 4 leaves per stem
					if (leafNumber > 4)
					{
						leafAreaLossRate = (cumulativeLeafAreaPhyllocron[leafNumber - 4] - cumulativeLeafAreaPhyllocron[leafNumber - 5]) * dailyThermalTime / phyllocron;
						senescenceLeafArea += leafAreaLossRate;
						leafNumber = 4;

						//soilWaterStress limiting factor

					}

					//green leaf LAI calculation, m2
					LAI = (totalCumulativeLeafArea - senescenceLeafArea) * plants * 0.0001;

					if (developmentRate >= 1.0)
					{
						dailyThermalTime -= stageLimit[stage];
						stage += 1;

						tillNumber = Minimum(1000, tillNumber);

						developmentRate = dailyThermalTime / stageLimit[stage];		//it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
						// stage rather than surfaceTemperatureAdjustment)
						Log("\n\ndailyThermalTime: %g", dailyThermalTime);
						Log("\nfirst DR = %g",developmentRate);
					}

					/******************************************************************************************************
					 * 						END OF ABOVEGROUND CROP DEVELOPMENT
					 *****************************************************************************************************/

					break;

				case 1:
					Log("\nstage %d", stage);
					break;

				case 2:
					Log("\nstage %d", stage);
					break;

				case 3:
					Log("\nstage %d", stage);
					// cumulated phyllocrons







					break;

				case 4:
					Log("\nstage %d", stage);
					//fraction of assimilates pertitioned to shoot
					//abovegroundFraction = 0.7 + 0.1 * Minimum(waterStressFactor,ns);		//todo sergio: to be reviewed

					//Leaf fraction of assimilare
					aboveBiomassFract = (1 - 0.12 * dailyThermalTime / phyllocron) * abovegroundFraction - 0.1;

					//fraction of assimilates partitioned to stem
					stemBiomassFract = 0.15 + 0.12 * dailyThermalTime / phyllocron - abovegroundFraction;

					//fraction of assimilates partitioned to grain
					grainBiomassFract = 0;

					break;

				case 5:
					Log("\nstage %d", stage);
					//fraction of assimilates pertitioned to shoot
					//					abovegroundFraction = 0.75 + 0.1 * Minimum(waterStressFactor,ns);		//todo sergio: to be reviewed
					//waterStressFactor and ns are water and nitrogen stress factors

					//Leaf fraction of assimilare
					aboveBiomassFract = 0;

					//fraction of assimilates partitioned to stem
					stemBiomassFract = abovegroundFraction;

					//fraction of assimilates partitioned to grain
					grainBiomassFract = 0;

					break;

				case 6:
					Log("\nstage %d", stage);
					//fraction of assimilates pertitioned to shoot
					//				abovegroundFraction = 0.8 + 0.1 * Minimum(waterStressFactor,ns);			//todo sergio: to be reviewed
					//Leaf fraction of assimilare
					aboveBiomassFract = 0;

					//fraction of assimilates partitioned to stem
					stemBiomassFract = abovegroundFraction;

					//fraction of assimilates partitioned to grain
					grainBiomassFract = 0;

					break;

				case 7:
					//fraction of assimilates pertitioned to shoot
					//					abovegroundFraction = 0.65 + 0.3 * stemBiomassFract / BMstem;			//todo sergio: to be reviewed
					// BMstem = current stem biomass; BMstem0 = stem biomass at flowering;

					//partition development according to the requirement of grain growth
					if (developmentRate >= 1.0)
					{
						stage += 1;
						dailyThermalTime = dailyThermalTime - (developmentRate * 40.0);
						// DR = dailyThermalTime / stageLimit[stage];		it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
						// stage rather than DT)
					}
					break;

				case 8:
					Log("\nstage %d", stage);
					if (developmentRate >= 1.0)
					{
						stage += 1;
						dailyThermalTime = dailyThermalTime - (developmentRate * 40.0);
						// DR = dailyThermalTime / stageLimit[stage];		it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
						// stage rather than DT)
					}
					break;

				case 9:
					Log("\n\npercent to reach the following stage: %g",  developmentRate);
					Log("\nstage of development: %d\n\n\n", stage);
					developmentRate = dailyThermalTime / 37500.0;
					Log("\nfirst DR = %g",developmentRate);
					stage = 0;
					stop_cycle = 1;
					Log("\n**** DEVELOPMENT YEARLY CYCLE FINISHED BEFORE THE END OF THE YEAR: IGNORED DEVELOPMENT MODULE FOR NOW");
					break;
				}
				Log("\n\ndailyThermalTime: %g", dailyThermalTime);
				Log("\nstage of development: %d\n\n\n", stage);
				Log("\n\n total termal denominator: %d",stageLimit[stage]);
				Log("\nDR = %g",developmentRate);
			}
		}



		//*************************************************************************************************************************************************************************************
		/*

				Log(" ********** RESPIRATION ***********\n");

				// maintenance respiration: k stands for the number of comparts
				for (i = 0; i < k; i++)
				{
					Rm[i] = Rmo[i] * pow(Q10, (Tcm - 25)/10)*compartBiomass[i];
					Log("\nMaintenanace respiration of %s: %g g CO2/m^2\n", organ[i], Rm[i]);
					Rm_s += Rm[i];
				}
				//growth respiration
				growthRespiration = (dailyGrossPhoto - Rm_s) * (1 - 1 / growthRespEfficiency);
				Log ("Maintenance total respiration: %g\nGrowth respiration: %g\n", Rm_s, growthRespiration);

				//rooting
				Log("******* ROOT LENGTH MODULE *********\n\n");

				// average specific root length
				for (l = 0; l < l_n; l++)
				{
					Resp += layerRootLengthDensity[l] * H[l] / BM_root;
				}

				//soil nitrogen limiting factor
				for (l = 0; l < soilLayer; l++)
				{
					Rtn[l] = 1 - 1.17 * exp (- 0.15 *(NO3_p[l] + NH4_p[l]));
					//soil temperature limiting factor 	TODO here whole soil profile temperature!?!?!?
					Rtt[l] = cos(Pi * (Ts - 20)/40);

					// soil aeration limiting factor (layer specific
					Rta[l] = CPwet + (1 - CPwet) * (UL[l] - SW[l])/(UL[l] - fieldCapacityLayerMoisture[l]);

					// soil strength limiting factor (layer specific)
					Rts[l] = ((1.6 + 0.4 * site->sand_perc - soilBulkDensity[l])/(0.5 - 0.1 * site->sand_perc)) *
							sin(1.25 *((SW[l] - LL[l])* Pi)/((fieldCapacityLayerMoisture[l] - LL[l]) * 2));
					Log("\nSoil nitrogen limiting factors (layer %d):\n\tsoil temperature limiting factor: "
							"%g\n\tsoil areation limiting factor: %g\n\tsoil strength limiting factor: %g"
							"\n\tSoil nitrogen limiting factor: %g", l, Rtt[l],Rta[l],Rts[l],Rtn[l]);
					//minimum between limiting factors
					if (Rtn[l] < Rtt[l] && Rtn[l] < Rta[l] && Rtn[l] < Rts[l])
					{
						Min_RT = Rtn[l];
					} else if (Rtt[l] < Rtn[l] && Rtt[l] < Rta[l] && Rtt[l] < Rts[l])
					{
						Min_RT = Rtt[l];
					} else if(Rta[l] < Rtt[l] && Rta[l] < Rtn[l] && Rta[l] < Rts[l])
					{
						Min_RT = Rta[l];
					}else if (Rts[l] < Rtt[l] && Rts[l] < Rts[l] && Rts[l] < Rta[l])
					{
						Min_RT = Rts[l];
					}
					else
					{
						Min_RT = Rtn[l];
					}
					Log("\nSetting minimum limiting factor as: %g", Min_RT);
					//Fraction of daily root senescence
					F_RS[l] = 0.01 * (2 - Min_RT);
					//F_RSum += F_RS[l];
					// Root distribution factor
					f_root[l] = pow(((1 - Z[l])/300), CProot)* Min_RT;

					// sum(f_root[i] - layerRootLengthDensity[l] * F_RS[l], n)	// check if f_root[i] is right,
					//that is if it's intended that has to be summed the partial of the whole profile root distribuition
					for (i = 0; i < soilLayer; i++)
					{
						mid_var += f_root[i] - layerRootLengthDensity[l] * F_RS[l];
					}

					//daily increase of root length density in layer l
					dlayerRootLengthDensity[l] = R * dailyAssimilationRate * (1 - F)/H[l] *f_root[l]/mid_var;
					Log ("daily increase of root length density in layer l: %g cm/cm^3 soil\n", dlayerRootLengthDensity[l]);

				//	dailyThermalTime = 1/DAY * Minimum(TD_m - TD_b, Maximum (0, Tc[hr] - TD_b));



					//Daily increase of root front depth
					dDroot = 0.2 * dailyThermalTime * Minimum (Rts[l], Rta[l]);		//TODO: pay attention to this problem:
					//DRoot shouldn't be single layer since
					//it is assumed to be function of layer specific parameters
				}

				Log("******** CROP NITROGEN MODULE ********\n\n");

				//movable nitrogen in shoot and root
				Npool = BM_root * (Nr - Nr_min) + (BM_leaf + BM_stem) * (Ns - Ns_min);

				//Nitrogen stress factor
				nitrogenStressFactor = 1 - (Ns - Ns0) / (Ns0 - Ns_min);

				//Uptake capacity:		TODO check if those 4 parameters are not used by other processes; if so, change from array to float form
				for (l = 0; l < soilLayer; l++)
				{
					//effect of soil moisture
					moistureEffectNitrogenUptake2[l]= 1 - 0.5 * (SW[l] - LL[l])/(fieldCapacityLayerMoisture[l] - LL[l]);

					//effects of ammonium concentration
					fNH4[l] = 1 -exp(-0.0275 * NH4_p[l]);

					//effects of nitrate concentration
					fNO3[l] = 1 - exp(-0.0275 * NO3_p[l]);

					//Nitrogen uptake: for now sum of the layer specific effects
					Nup += moistureEffectNitrogenUptake2[l] * layerRootLengthDensity[l] * (fNO3[l] + fNH4[l]) * H[l];
				}

				// Nitrogen uptake capacity
				Nup *= 0.06;

				//Daily nitrogen demand
				//growth demand
				Ndem_g = dailyAssimilationRate * (F * Ns0 + (1 - F) * Nr0);
				//Deficiency demand
				Ndem_d = BM_root * (Nr0 - Nr) + (BM_leaf + BM_stem) * (Ns0 - Ns);

				//daily nitrogen demand
				Ndem = Ndem_g + Ndem_g;

				Log("\n******* DECOMPOSITION AND METHANE EMISSIONS ********\n\n");

				//Effects of pH
				fpH_M = ((pH - 5.5) * (pH - 9.0)) / ((pH - 5.5) * (pH - 9.0) - pow((pH - 7.5), 2));

				//Daily increase in of redox potential
				if (flood == 1)
				{
					dEh[l] = 100 * (0.5 * Aere - 0.5);
				} else if (flood == 0)
				{
					dEh[l] = 100 * ( 0.5 * Aere + 100 * (1 - Wfps[l]));
				}
				//TODO sergio: on the paper it was Ch[l] and not Eh[l]; i suspect it is Eh associated indeed; anyway, how can i change Eh?!?!

				//effecs of redox potential
				if (Eh > -200)
				{
					fEh = 0;
				} else if (Eh <= -200)
				{
					fEh = 1;
				}

				//effects of temperature
				fTM = exp(0.33 * (Ts -23)) / (1 + exp(0.33 * (Ts - 23)));

				//Carbon pool for methane production
				for (l = 0; l < soilLayer; l++)
				{
					C_CH4 += Cs[l] + F_RS[l] * F_root[l] * BM_root *4;		//TODO sergio: ricontrolla quali ci sono equali no
				}

				//daily methane production
				CH4_p = 0.47 * C_CH4 * fTM * fEh * fpH_M;

				//Aerenchyma factor
				Aere = BM_root / 1000; 								//TODO Rice;

				//daily methane oxidation
				CH4_o = CH4_p * (0.5 + 0.5 * Aere);

				//TODO quantity of each Ci

				for (l = 0; l < soilLayer; l++)
				{
					//effect of clay adsorption
					mu_clay = log10(0.14 / clay[l]) + 1;

					//the effect of moisture
					mu_w = -1.7827 * pow(Wfps[l], 2) +2.3824 * Wfps[l] - 0.222;

					// the effect of temperature
					mu_T = 0.2161 + 0.093 * Ts - 0.0014 * pow (Ts, 2);

					//decomposition rate of i-th carbon pool TODO ricontrolla bene
					for (i = 0; i < 3; i++)
					{
						for (j = 0; j < 2; j++ )
						{
							//decomposition rate of a carbon pool
							dC[l][i][j] = mu_clay * mu_cn * mu_T * mu_w * mu_til * K_c[l][i][j] * Ci[l][i][j];
						}
					}
				}


				Log(" ****** SOIL NITROGEN MODULE ******\n\n");

				Log("Nitrification rate\n");

				//Effect of pH on nitrification
				fpH_N = - 0.0604 * pow(pH, 2) + 0.7347 * pH - 1.2314;

				//Effects of soil moisture on nitrification
				Fw_n = -12.904 * pow(Wfps, 4) + 15.651 *pow(Wfps, 3) - 5.5368 * pow(Wfps, 2) + 0.9975 * Wfps - 0.0243;

				//Effects of temperature on nitrification
				fTN = -0.0272 * pow((0.1 * Ts), 4) + 0.1566 * pow(0.1 *Ts, 3) - 0.2234 * pow(0.1 *Ts, 2) + 0.03094 * Ts;

				//Nitrification rate NH4+ converted to NO3-
				NO3_N = NH4 * (1 - exp(-K35 * fTN)) * Fw_n * fpH_N;

				//Equilibrium of ammonium and ammonia (kNH4_NH3 = log10(K_nh4) - log10(K_h2o)) TODO NH3m missing
				kNH4_NH3 = log10(NH4m / NH3m) + pH;

				//Ammonium adsorption
				F_NH4 = (0.41 - 0.47 *log10(NH4) * clay / clay_max);

				//diffusion coefficient
				Ds = 0.6 + 2.93 * pow(v, 1.11);				//TODO v variable missing yet

				//solute movement
				Js = - SW[l] * Ds * Gs + Jw * Sc;			//TODO here many variables missing yet (Gs, Jw, Sc)

				Log(" denitrification\n\n");


				//pH reduction factors: N20 TODO sergio: pay attention; it's not clear if it is NO2 or N2O pH reduction factor
				mu_pH_N2O = 0.384 * (pH - 4.4);

				// pH reduction factor: NO2. same problem as before; it could be the antecedent one
				mu_pH_NO2 = 1.0;

				//pH reduction factor: NO3 case.
				mu_pH_NO3 = 0.313 * (pH - 3.18);

				//temperature reduction factor
				mu_T_DN = pow(2, (Ts - 22.5)/10);

				//NO production during nitrifiation
				NO_N = 0.0025 * NO3_N * pow(2.72, 34.6 - 9615 / (Ts + 273.15));

				//N2O production during nitrification
				N2O_N = 0.0006 * NO3_N * Wfps * pow(2.72, 34.6 - 9615 / (Ts + 273.15));

				//maximum denitrifier growth rate as to NO2
				U_no2 = Um_no2 * Cs / (K_c12 + Cs) * NO2 / (K_h_no2 + NO2);

				//maximum denitrifier growth rate as to NO2
				U_no3 = Um_no3 * Cs / (K_c12 + Cs) * NO3 / (K_h_no3 + NO3);

				//maximum denitrifier growth rate as to NO2
				U_n2o = Um_n2o * Cs / (K_c12 + Cs) * N2O / (K_h_n2o + N2O);

				//relative denitrifier growth
				Udn = mu_T_DN * (U_no3 * mu_pH_NO3 + U_no2 * mu_pH_NO2 + U_n2o * mu_pH_N2O);

				//denitrifier growth rate
				dB_dt_g = Udn * B;		//B(t)

				//denitrifier death rate
				dB_dt_d = Mc * Yc * B;	// B(t), Mc

				//Consumption of soluble carbon
				Ccon = (Udn / Yc + Mc) * B;

				//CO2 production
				CO2_dn = Ccon - dB_dt_g;

				//nitrogen assimilation rate
				dN_dt_asm = dB_dt_g * l / CNR_dn;		// to be understood what's l

				//N2 emissions
				F_n2 = (0.0006 + 0.0013  * fclay[l]) + (0.013 - 0.005 * fclay[l]) * (1 - Wfps[l]) * pow(2, (Ts/20));

				Log("N2 Emissions: %g", F_n2);

				//N2O and NO emissions
				Fn2o_no = 0.017 + (0.025 - 0.0013 * fclay[l]) * (1 - Wfps[l]) * pow(2, Ts/20);

				//Nitrate consumption
				dNO2_dt = (U_no2 / Y_NO2 + M_NO2 * NO2 / N) * B *mu_pH_NO2 * mu_T_DN;
				//nitrous oxide consumption
				dN2O_dt = (U_n2o / Y_N2O + M_N2O * N2O / N) * B *mu_pH_N2O * mu_T_DN;
				//Nitrite consumption
				dNO3_dt = (U_no3 / Y_NO3 + M_NO3 * NO3 / N) * B *mu_pH_NO3 * mu_T_DN;
		 */

		//todo variazione tempo termico
		//if ( Tc - Tb <0){thermal_time += Tc(or Tair) / 2 - T_basal) * d_ndays

		/*		Log("\nDaily thermal time (DNDC method): %g °C\n", dailyThermalTime);
		//	Log("\n\n total termal time to finish the stage: %g",stageLimit[stage]);
		Log("\n\npercent to reach the following stage: %g",  DR);
		Log("\nstage of development: %d\n\n\n", stage);
		 */
	}






	Log("\n****************END OF CELL***************\n");

	/* ok */
	return 1;
}


