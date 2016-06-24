/*cropmodel.c*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "soil_settings.h"
#include "cropmodel_daily.h"
#include "common.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_log;
extern soil_settings_t *g_soil_settings;

//define
#define SNOW_COEFF 15 		// snow cm;
#define DAY 24
#define THERMAL_STAGE1 400			// thermal period for stage 1
#define S_ALT -0.83

//#define E_SPIN

/* */
int crop_model_D(matrix_t *const m, const int _cell, const int year, const int month, const int day, const int years_of_simulation)
{
	meteo_t *met;

	int cell;
	int height;
	int age;
	int species;

	/**************************************************************************************************************************************
	 * 									STATIC AND CONST PARAMETERS
	 **************************************************************************************************************************************/

	const int soilLayer = 1;						// when done a cropType.h point to the value in site
	const int phyllocron = 95;						// time interval between leaf tip appearancE it shoul be somehow variable (see Ritchie et al., 91)
	const int     MAXINT = 32767;
	const double   scatterLightParameter = 0.2;			//scatter of light parameter; set as default 0.02 (Zhang et al. 2002)
	const double   rootWaterUptakeCoefficient = 0.003;	// root water uptake coefficient (0.003 cm water/cm root)

	double max = -MAXINT;
	double min =  MAXINT;

	//	static double vpd;
	//	const int     MAX_DIM = 100;

	//dichiara e inizializza qui tutte le variabili che ti servono

	/**************************************************************************************************
	 * 									TRASVERSALLY USED IN CROP MODEL
	 **************************************************************************************************/

	int   l;									// enumerator;
	int   i;									// enumerator;
	int   speciesParam;

	//**************************************************************************************************
	//									DAY LENGTH
	//**************************************************************************************************

	int   julianDate;							// count_month; last valid day for each month;
	int   actualDate = 0;
	double albedo;								// field albedo
	double Sfactor;
	double Cfactor;
	double daylength;
	double solarDeclination;
	double solarElevation;						// elevation angle of the sun (gradient)

	//int   maxAltitudeJulianDate;				// julian date when solar altitude is the highest

	//***************************************************************************************************
	//								DAILY SOLAR RADIATION
	//***************************************************************************************************

	int   landUseType;
	double solarConstant; 						// solar constant (J/m^2/s)
	double extraTerrestrialInsolation;
	double empiric_param_a;
	double empiric_param_b;
	double dailySolarRadiation;					// daily solar radiation (J/m^2/s) it would be in type.h:	m->cells[cell].net_radiation
	double bareGroundAlbedo;						// bare ground albedo
	double snow;									// snow (cm water) it should be m->met[month].snow
	// ALESSIOR LAI was already used in types.h...renamed to _LAI
	double _LAI;									// in m->cells[cell].heights[height].species[species].value[LAI]

	//double maxPhotoperiodismDaylength;			// max daylength for photoperiodism; is useful only for those lpaces in which daylength exceeds 20h

	/*********************************************************************************************************
	 * 									TEMPERATURE
	 *********************************************************************************************************/

	double snow_eff;								// snow effect on canopy temperature
	double canopyMaxTemperature;					// max (daily) canopy temperature
	double canopyMinTemperature;					// min (daily) canopy temperature
	double canopyDaytimeMeanTemperature;			// canopy daytime (mean) temperature
	double meanCanopyTemperature;				// mean canopy temperature

	double annualTemperatureAmplitude;			// amplitude of annual temperatures (°C)
	double meanAnnualTemperature;				// mean of annual temperatures (°C)
	double soilSurfaceTemperature;				// soil surface temperature in day k

	double soilLayerThickness[1];				// thickness of soil layer
	double soilLayerDepth[1];					//soil layer specific depth from ground line

	double maxDampingDepth;
	double mid2;
	double dampingDepth[1];						// damping depth in soil temperature estimation (cm water)
	double soilBulkDensity;						// average soil bulk density: each array cell stores its' array profile BD; BD[l+1] = sum BD[i];
	double layerMoisture[1];						// soil moisture of specific layer (cm^3 water / cm^3 soil)

	double maxAirTemperature;
	double minAirTemperature;
	double meanDailyTemperature;
	double soilSurfaceTemperatureDNDC;
	double soilLayerTemperature[1];				//soil layer temperature at its center
	double yos0SoilSurfaceTemperature;

	//double snowCoverLagFactor; 				// lagging factor simulating residues and and snow cover effects on soil surface
	//int   hourDay;							// hour of the day
	//int   wetDays;							// number of rainy days in the current month;
	//int   dryDays;							// number of dry days in the current month

	//double meanDiurnalCanopyTemperature;		// mean diurnal canopy temperature as arithmetic mean of Tc_d2
	//double Tcm;								// canopy daily mean temperature (°C)
	//double soilSurfaceTempPrevious;			// soil surface temperature at day k - 1;
	//	double soilSurfaceTemperature2;			// soil surface temperature in day k (computed with Parton)

	//double Tmax;								// daily air max temperature (°C)
	//double Tmin;								// daily air minimum temperature (°C)
	//	double surfaceTemperatureAdjustment;		// adjustment factor for the effects of surface temperature
	//	double Zmax;								// soil depth from the surface

	//double temperatureFactor1[1];				// factor to make near surface temperature strong function of the surface temperature
	//double temperatureFactor2[1];				// factor to make near surface temperature strong function of the surface temperature

	//----------------------------------APEX-EPIC REQUESTED ----------------------------------------------------------------------------------------------------------------

	double displacementHeight;
	double lag; 									// coeff. ranging from 0.0 to 1.0: allows weighting of (d-1)’s soil temp. with the current day’s
	// set at 0.5: weighted the current T estimates equally with the previous day’s temperature.
	double prevDaySoilLayerTemperature[1];
	double depthTempFactor;						// epic factor to consider soil layer distance from surface in computing soil layer temperature
	double canopyHeight = 0.0;							// rop height: used to compute displacement height

	//---------------------------------- PARTON -------------------------------

	//oat epigeousBiomass;						// parton epigeous biomass fraction



	/****************************************************************************************************
	 *										RUNOFF AND WATER REDISTRIBUITION
	 ****************************************************************************************************/

	int   drainageClass;						// soil drainage class, used to determine SWcon factor as defined by Ritchie and Iglesias
	double dailyMaximumSnowMelt;					// daily maximum snow melt
	double dailyMaximumPlantInterception; 		// daily maximum plant interception  (cm water)
	double maxInfiltration;
	double Runoff;

	//watch out if it is correctly formatted
	char Luse[20] = "RowCrops";
	char pract[20]  = "straightRaw";
	int  hydroCondition;
	int  hydroGroup;

	double CN;
	double SWcon;								// drainage coefficient (Ritchie, 88)
	double profileWaterContent;					//soil water content from input: it is a whole profile value
	double drain[1];								// daily water draining flux (cm)

	//	double landuse;

	/********************************************************************************************************************************
	 * 												EVAPOTRANSPIRATION and SOIL WATER BALANCE
	 *******************************************************************************************************************************/

	double fieldCapacityLayerMoisture[1];		// soil moisture at field capacity of the specific layer;
	double actualTranspiration;					// actual crop transpiration (cm water)
	double layerWilting[1];						// soil moisture at wilting point of layer l_n
	double potentialEvaporation;					// potential evaporation
	double equilibriumEvapotranspiration;		// equilibrium evapotranspiration (cm)
	double potentialEvapotranspiration;			// potential evapotranspiration (cm)
	double moistureSoilEvaporationEffect;		// effects of soil moisture on soil evaporation


	double waterUptakeSoilLayer;					// water uptake from specific soil layer
	double potentialTranspiration;

	double deltaMoisture;
	double moistureEffectWaterUptake1[1];		// effect of moisture on water uptake

	double infiltration;							// quantity of water infiltrated in a soil layer
	double Hold[1];								// quantity of water holded in the specific layer???
	double layerFieldSaturation[1];				// see ceres wheat 2.0
	double flux;									// layer specific soil water flux
	double normVolumetricWater[1];

	double Diffusion;
	double actualSoilEvaporation;				// actual soil evaporation
	double fractionFactor;

	double driestSoilWaterContent;

	//	double moistureEffectNitrogenUptake2[1];			// effect of moisture on nitrogen uptake
	//  double depthTopSoilAffectingEvaporation;			// depth of top soil affecting evaporation (20cm) (cm)
	//	double soilWaterDiffusionCoeff;					// diffusion coefficient of soil water
	//	double potentialTranspiration;					// potential crop transpiration (cm water)
	//	double layerMoistureAboveWilting[1];				// soil moisture above the layer's wilting point (cm^3 water / cm^3 soil)
	//  double SWcon;									// drainage coefficient (Ritchie, 88)


	/*********************************************************************************************************************************
	 * 										THERMAL TIME APPROACH
	 *********************************************************************************************************************************/
	int   stageLimit[9];
	double dailyThermalTime;
	double basalTemperature;						// basal temperature for wheat; corn (8), rice (10)
	double developmentRate;						// developmental rate
	double maxDevelopmentTemperature;			// max temperature in which possible development
	double vernalizationFactor;
	double photoperiodFactor;

	double photoperiodCoefficient;
	double vernalizationCoefficient;
	double temperatureVernFactor;
	double vernTemperature;
	double maxVernTemperature;
	double vernOptimumTemperature;
	double dailyVernalizationEffectiveness = 0.0;
	double vernalizationDays;


	/********************************************************************************************************************************
	 * 											DEVELOPMENT STAGE SETTING VARIABLES
	 *******************************************************************************************************************************/

	int   sowingdepth;							// INPUT!!!!!!  sowing depth input parameter (cm)
	int   stage;								// developmental scale used to estimate crop growth
	int   stop_cycle;							// boolean: if equal to 1 cycle has finished before the end of the year: ignore dev. module from that point on
	int   cumPhyllocrons;


	/********************************************************************************************************************************
	 * 												PHOTOSYNTHESIS MODULE
	 *******************************************************************************************************************************/

	double  fCO2;									// effects of CO2 concentration on photosynthesis
	double  co2EffectPhotosynthesis;				// crop parameter for CO2 effects on photosynthesis (0.4 for C4; 0.8 for C3)
	double lightSaturationPhotoRate;				// Photosynthesis rate at light saturation (kg CO2/ha/h)
	double optimumPhotoRate;						// Photosynthesis rate at light saturation when T is optimal;

	double blackBodyExtinctionCoeff;				// Extinction coefficient of assumed black body leaves

	//gaussian integration parameters:
	double gaussianParameter1[3];

	double solarElevationSumDNDC;
	double absorbedDiffuseLight ,
	absorptionDirectLight,
	absorptionDirectDirectLight;

	double layerSoilTempEffectPhotoRate;			// effecsoilLayerTemperature of temperature on photosynthesis rate at light saturation


	//photosynthesis
	double canopyHeightLAI[3];					// lai at a specific canopy height
	double gaussianIntegrationHour[3];			// hour chosen to compute the gaussian integration

	//light variables
	double diffuseLightExtinctionCoeff; 			// extinction coefficient of diffuse light
	double diffuseLightFraction;					// fraction of diffuse light
	double directLightExtinctionCoeff;			// extinction coefficient of direct light
	double hourPar;								// mean par at a specific hour
	double diffuseLightAboveCanopy;				// diffuse light above the canopy
	double directLightAboveCanopy;				// direct light above the canopy
	double directLight;							// Direct light
	double diffuseLight;							// Diffuse light

	double atmosphericTrasmissionCoeff;			// atmospheric transmission coefficient
	double canopyHorizontalReflectivity;			// Reflectivity of horizontally distribuited canopy
	double canopySpharicalReflectivity;			// Reflectivity of sphaerical distribuited canopy

	double midVariable4;							// mid variable
	double midVariable3;							// mid variable

	double shadeLeavesLightAbsor;				// light adsorbed by shaded leaves in a layer
	double leafSurface90degLight;				// light which is perpendicoular to leeaf surface
	double shadLeavesPhotoRate;					// photosynthesis rate of shaded leaves
	double sunlitLeavesPhotoRate;				// photosynthesis rate of sunlit leaves
	double gaussIntegrPhoto;						// gaussian integration f(time) of photosynthesis for a canopy layer
	double gaussIntegPhotoSum; 					// photosynthesi value for a canopy point, as proposed for a gaussian integration
	double  dailyGrossPhoto;						// daily gross photosinthesis (g/m^2)
	double initialLightUseEfficiency;			// initial light use efficiency   -> taken as PLEI (penning de vries)
	double layerGrossPhotoRate,
	sunlitLeafAreaFraction;


	/******************************************************************************************************************************
	 * 										RESPIRATION MODULE
	 *****************************************************************************************************************************/

	double maintenanceRespiration;				// maintenance respiration
	double compartMaintenanceResp[4];			// maintenance respiration factor for organ i
	double compartBiomass[4];					// biomass of crop organ i
	double Q10;									// Q10 value
	double growthRespiration;					// growth respiration
	double growthRespEfficiency;					// average growth efficiency

	/********************************************************************************************************************************
	 * 							 BIOMASS PARTITIONING  AND ALLOCATION (ABOVEGROUND BIOMASS)
	 *******************************************************************************************************************************/

	double potentialStemGrowth;
	double soilWaterInfluenceOnAssimilation;

	double previousDaySoilTemp;					// layer 2 soil temperature of the previous day; if soil is not layered, assumed as the whole soil temperature
	double plantLeafAreaGrowthRate;
	double leafAreaGrowthRate;
	double tillNumber;
	double leafAreaToAssimilateWeight,
	potentialLeafGrowth,
	potentialRootGrowth,
	dailyAssimilate,
	totalCumulativeLeafArea,
	tillerRate1,
	tillersPerSquareMeter;

	// ALESSIOR this var was used without being initialized
	double singleTillerPotentialBiomassGain = 0.;
	double dailyRatioStemToTotalWeight;
	double stemWeight;
	double sumDailyThermalTime;
	double geneticFactor4; 					//final potential dry weight for a single stem plus ear at anthesis when the crop has grown under optimum conditions
	double geneticFactor2;

	int plants,
	leafNumber,
	kernelsPerPlant,
	phylloCounter;

	double tillerRate2,
	cumulativeLeafAreaPhyllocron[5],
	leafAreaLossRate,
	senescenceLeafArea;

	double proportionalityConstant;
	double plantAssimilatesTopFraction;
	double leafWeight = 0.0;
	double grainWeight;
	double leafFallWeight = 0.0;

	double waterStressFactor;					// water stress factor (from 0 to 1: formula to be found)
	double nitrogenStressFactor;					// nitrogen stress factor (nitrogenStressFactor to be evaluated with a formula; to be found)
	double minimumStemWeight;

	double grainFillingRate;
	double plantPotentialGrainGrowth;
	double geneticFactor3;
	double actualGrainGrowth;
	double dryWeightYeld;
	/***********************************************************************************************************************
	 * 												BELOVEGROUND BIOMASS
	 ********************************************************************************************************************/
	double layerRootLengthDensity[1];			// root length density of layer l_n (cm root/ cm^3 soil)

	//	double dlayerRootLengthDensity[l];		// daily increase of root length density in layer l


	/*******************************************************************************************
	 * 							RITCHIE 1988
	 *******************************************************************************************/

	//-----------------------------------------------------------------------------------------------------------------------------------


	/*****************************************************************************************************************************
	 * 										############################################
	 * 									       CROP MODULE OF 3D-CMCC-LANDSCAPE MODEL
	 * 										############################################
	 ******************************************************************************************************************************/


	// check parameters
	assert(m);
	met = m->cells[_cell].years[year].m;

	//control if all soil data are available
	for ( cell = 0; cell < m->cells_count; cell++)
	{
		if (	IS_INVALID_VALUE(g_soil_settings->values[SOIL_SAND_PERC])
				|| IS_INVALID_VALUE(g_soil_settings->values[SOIL_CLAY_PERC])
				|| IS_INVALID_VALUE(g_soil_settings->values[SOIL_SILT_PERC])
				|| IS_INVALID_VALUE(g_soil_settings->values[SOIL_DEPTH]) )
		{
			logger(g_log, "NO SOIL DATA AVAILABLE\n");
			return 0;
		}
	}




	//--------------------------------------------------------------------------------------------------------------------------------------

	//DAILY loop on each cell
	for ( cell = 0; cell < m->cells_count; cell++)
	{
		logger(g_log, "---DAY SIMULATED = %d\n", met[month].d[day].n_days);
		for ( height = m->cells[cell].heights_count -1 ; height >= 0; height-- )
		{
			//loop on each ages
			for ( age = m->cells[cell].heights[height].ages_count - 1 ; age >= 0 ; age-- )
			{
				/*increment age after first year*/

				if ( day == 0 && month == JANUARY && year == 0)
				{
					logger(g_log, "***** VERY FIRST VARIABLES INITIALIZATION *****\n"); 	//PAY ATTENTION, IS THIS ALLRIGHT TO CALL IT INITIALIZATION!?
					//shandong K coefficient
					//extinctionCoeff = 0.6;

					//gaussian canpopy integration
					gaussianParameter1[0] 		= 0.5 - sqrt(0.15);
					gaussianParameter1[1] 		= 0.5;
					gaussianParameter1[2] 		= 0.5 + sqrt(0.15);

					drainageClass				= 3;
					hydroCondition 				= 1;
					hydroGroup 					= 2;

					//drainage profile coefficient SWcon (Ana Iglesias)
					if (drainageClass == -9999)
					{
						SWcon 					= 0.5;
						logger(g_log, "\nAttention! no drainage factor found: used a standard 0.5 value of SWcon");
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
					 * 					SOIL PARAMETERS
					 ********************************************************/
					waterStressFactor 				= .5;
					yos0SoilSurfaceTemperature		= 8.1;

					// critical daylenght for max photoperiodism
					stage 							= 0;
					stop_cycle 						= 0;
					speciesParam							= 0;
					landUseType 					= 0;
					sowingdepth 					= 60;  			// INPUT!!!!!!  sowing depth input parameter (cm)
					bareGroundAlbedo 				= 0.17;
					snow 							= 0.00;
					soilSurfaceTemperature 			= 0.0;			// soil temperature: setted to 0 so that also soilSurfaceTemperature - 1 is:ATTENTION! NOT TO BE USED IN JAN 1st
					soilBulkDensity					= 1.49;			// (g/cm3)
					fractionFactor 					= 0.03; 		// if heavyclay, 0.03; if sandy 0.07
					Q10 							= 2.0;			// assumed by penning de vries, pag. 53
					soilLayerThickness[0] 			= 80.0;
					soilLayerDepth[0] 				= 80.0;
					layerMoisture [0] 				= 15.0;
					fieldCapacityLayerMoisture[0] 	= 30.0;			// it shoul be set at MAXASW (i suppose), but on site is putted as a comment
					lag 							= 0;			// set at 0.5: weighted the current T estimates equally with the previous day’s temperature.

					//soil moisture of layer l at wilting point; assumed as 10% fraction of max swc ( filed capacity) (Collalti)
					layerWilting[0] = fieldCapacityLayerMoisture[0] * g_settings->init_frac_maxasw;
					driestSoilWaterContent = 0.5 * layerWilting[0];

					/****************************************************
					 * 				GENERAL SITE PARAMETERS
					 ***************************************************/

					julianDate = (4713 * 365) + (4713 + m->cells[_cell].years[year].year) / 4 + (m->cells[_cell].years[year].year * 365) - 14;

					for (l = 0; l < soilLayer; l++)
					{
						layerFieldSaturation[l] 	= 1.0 - soilBulkDensity / 2.65 - fractionFactor;
						layerFieldSaturation[l] 	= 30.0;		//fixsergio sergio find out a way to compute water content at saturation (macropore occupied by water)
					}


					//profileWaterContent = 0.5;
					//depthTopSoilAffectingEvaporation 			= 20.0;
					//airTemperature = 15.0; //??
					//maxAltitudeJulianDate = 144;				//21/6


					/***********************************************************
					 * 			SPECIE SPECIFIC PARAMETERS INITIALIZATION
					 **********************************************************/

					//species' specific initial parameters

					if (speciesParam == 0)			//and day is 1st
					{
						_LAI 						= 0.5;
						totalCumulativeLeafArea 	= _LAI;
						plants 						= 300;
						tillNumber 					= 1.0;

						maxDevelopmentTemperature 	= 34.0;
						photoperiodCoefficient 		= 0.0055;
						vernalizationCoefficient 	= 0.027;

						//### PHOTOSINTHESYS ###

						optimumPhotoRate 			= 4.0;		// taken from spitters, g CO2 /m2/h
						initialLightUseEfficiency	= 0.0000125;		// (g CO2 /J) ( J/(m^2 sec))  for spitters
						co2EffectPhotosynthesis		= 0.8;
						nitrogenStressFactor 		= 0.6;
						vernalizationFactor 		= 0.5;
						photoperiodFactor 			= 0.4;

						//###  BIOMASS  ###
						geneticFactor4				= 3.0;
						geneticFactor3				= 3.0;
						geneticFactor2 				= 3.0;

						//abovegroundFraction 		= 80.0;		//aboveground biomass fraction (understand if it is the same)
						//epigeousBiomass 			= 0.10;		//invented; it will change while running
						basalTemperature 			= 1.0;		//basal temperature for wheat; corn (8), rice (10)
						developmentRate 			= 0.0;		// developmental rate

						proportionalityConstant		= 115.0;	//cm^2/(g m) proportionality constant for wheat assuming only leaf blade area in factoring weight

						//### SPECIE DEVELOPMENT ###

						compartBiomass[0] 			= 10.0;	// leaf biomass
						compartBiomass[1] 			= 5.0;	// stem biomass
						compartBiomass[2] 			= 0.0;	// grain biomass
						compartBiomass[3] 			= 10.0;	// root biomass

						maxVernTemperature = 11.8;
						vernOptimumTemperature = 7.5;

						cumulativeLeafAreaPhyllocron[0] = 0;
						cumulativeLeafAreaPhyllocron[1] = 0;
						cumulativeLeafAreaPhyllocron[2] = 0;
						cumulativeLeafAreaPhyllocron[3] = 0;
						cumulativeLeafAreaPhyllocron[4] = 0;

						layerRootLengthDensity[0] 		= 1.0;	//fixsergio sergio cm of very first root length; invented by me to avoid 0/0 problem

						//growth respiration efficiency; taken fron Penning de Vries
						growthRespEfficiency = (0.451 + 0.532 + 0.774 + 0.690 + 0.375) / 5.0;		//from listing 1, pag 205; is the mean coefficient  of specific chemical compound CO2 production not totally secure though

						//maxDevelopmentTemperature = 34.0;				//max temperature in which possible development

						//to be further evaluated (values taken from penning de vries, pag. 52): IMPROVABLE; function of temperature and photosynthesis!! pag 53

						compartMaintenanceResp[0] 	= 0.016;	// wheat, sage: _______________
						compartMaintenanceResp[1] 	= 0.010;	// general annual plants; in young stems tissue, rate 1.5 2 times higher
						compartMaintenanceResp[2]	= 0.010;  	// assumed equal to stem maintenance respiration
						compartMaintenanceResp[3] 	= 0.015; 	// general for annual plants

						maintenanceRespiration += compartMaintenanceResp[i] * pow(Q10, (meanCanopyTemperature - 25)/10.0) * compartBiomass[i];
						logger(g_log, "\nMaintenanace respiration cumulated for %d compartments: %f g CO2/m^2\n", i, maintenanceRespiration);


					}
					cumPhyllocrons 						= 1; //number of cumulative phyllocrones since emergence

					logger(g_log, "\nhydroCondition %d \nHydrogroup %d \nLuse %s \npract %s", hydroCondition, hydroGroup, Luse, pract);
					/*********************************************************************************************************************************
					 * 									SETTING OF DEVELOPMENTAL STAGES (INITIALIZATION OF VARIABLES)
					 ********************************************************************************************************************************/

					//riordina perbene


					stageLimit[0] 			= 400;
					stageLimit[1] 			= 3 * phyllocron;
					stageLimit[2] 			= 2 * phyllocron;
					stageLimit[3] 			= THERMAL_STAGE1;
					stageLimit[4] 			= 200;
					stageLimit[5] 			= 500;
					stageLimit[6] 			= MAXINT;							//set by harvest date; switch (if(date == harvest date) {stage++}
					stageLimit[7]			= 40 + 10 * sowingdepth / 5;	//to be clarified
					stageLimit[8] 			= 1; 								//to be clarified

					dailyThermalTime		= 0;
					phylloCounter 			= 0;
					leafNumber				= 0;
					cumPhyllocrons 			= 0;
					senescenceLeafArea 		= 0;

					canopyHeight 		   += 0.1; 	//fixsergio sergio find out a way to compute crop height; now setted as a +10cm per month
				}
				else if( day == 0 && month == JANUARY && year != 0)
				{
					m->cells[cell].heights[height].ages[age].value += 1;
				}
				if (day == 0 && month == JANUARY)
				{

				}
				//loop on each species
				for (species = 0; species < m->cells[cell].heights[height].ages[age].species_count; species++)
				{
					if(day != 1 && month != JANUARY)
					{
						for (i = 0; i < soilLayer; i++)
						{
							prevDaySoilLayerTemperature[i] = soilLayerTemperature[i];
						}
					}

					/****************************************************************************************************
					 * 									---------------------------
					 * 											CROPMODEL_M
					 * 									---------------------------
					 ****************************************************************************************************/




					/**************************************************************************************************/
					//								SOLAR QUANTITIES
					//*************************************************************************************************


					//julian date
					actualDate ++;
					logger(g_log, "\nstarting from year %d\njulianDate: %d", m->cells[_cell].years[year].year, actualDate);

					//should be set here maxAltitudeJulianDate: julian date when sun is higher?

					//------------------------------------------------------------------------------------------------------------------
					//solar declination
					solarDeclination = asin(-sin(23.45 * Pi / 180.0) * cos(2.0 * Pi * (actualDate +10) / 365.0));
					logger(g_log, "\nsolarDeclination = %f", solarDeclination);

					// there are some potential dicrepancies with literature versions; watch out! c computes angle operation ALWAYS as radiants!

					//and what if they're in radiants and not degrees!?!?!
					Cfactor = cos(g_soil_settings->values[SOIL_LAT] * 2 * Pi / 360.0) * cos(solarDeclination);
					Sfactor = sin(g_soil_settings->values[SOIL_LAT] * 2 * Pi / 360.0) * sin(solarDeclination);
					logger(g_log, "\nC value = %f\nS value = %f", Cfactor, Sfactor);

					//------------------------------------------------------------------------------------------------------------------


					daylength = 12 + (24.0 / Pi) * asin(Sfactor / Cfactor);
					logger(g_log, "\nDaylength = %f hours\n", daylength);

					//compare with get_daylenght(&m->cells[cell])

					// (substitute  to be understood if it is a net radiation formulation; comparison with net radiation
					solarConstant = 1370.0 * (1.0 + 0.033 * cos(2.0 * Pi * julianDate/365.0));
					logger(g_log, "\nsolarConstant = %f\n", solarConstant);

					// extraterrestrial insulation /J/(m^2 s)
					extraTerrestrialInsolation = 3600 * solarConstant * (daylength * Sfactor + (24 * Cfactor * sqrt(1 - (pow(Sfactor,2) / pow(Cfactor,2)))) / Pi);
					logger(g_log, "\nextraterrestrialInsolation = %f\n", extraTerrestrialInsolation);


					/* *******************************************************************************************************************
					 * 								A SOLAR EQ. ALTERNATIVE: STOCKLE AND CAMPBELL
					 * ********************************************************************************************************************/


					logger(g_log, "\n------------------------------------------------------------------------");
					logger(g_log, "\n\n******* DERIVING SOLAR RADIATION FROM SUNSHINE DURATION ********\n\n");

					landUseType = 0;
					if (landUseType == 1)
					{
						logger(g_log, "for a %d landUseType \n", landUseType);
						empiric_param_a = 0.25;
						empiric_param_b = 0.45;
						logger(g_log, "the empirical parameters are:\n\ta: %f\n\tb: %f\n", empiric_param_a, empiric_param_b);
					}
					else if (landUseType == 2)
					{
						logger(g_log, "for a %d landUseType \n", landUseType);
						empiric_param_a = 0.29;
						empiric_param_b = 0.18;
						logger(g_log, "the empirical parameters are:\n\ta: %f\n\tb: %f\n", empiric_param_a, empiric_param_b);
					}
					else if (landUseType == 0)
					{
						logger(g_log, "for a %d landUseType \n", landUseType);
						empiric_param_a = 0.18;
						empiric_param_b = 0.55;
						logger(g_log, "the empirical parameters are:\n\ta: %f\n\tb: %f\n", empiric_param_a, empiric_param_b);
					}
					else
					{
						logger(g_log, "ERROR; NO PROPER LAND_TYPE INPUT");
					}
					logger(g_log, "--------------------------------------------------------------------------\n\n");

					if (met[month].d[day].solar_rad != -9999)
					{
						// in MJ/(m^2 d)
						logger(g_log, "\n\nSolarRadiation (field); MJ/(m^2 d)");
						dailySolarRadiation = met[month].d[day].solar_rad;
						logger(g_log, "= %f", met[month].d[day].solar_rad);

						//converted in J/(m^2 s)
						dailySolarRadiation *= (1000000 / (3600 * daylength));
						logger(g_log, "\n\ndailySolarRadiation in J/(m^2 s) %f", dailySolarRadiation);

					}


					//albedo
					logger(g_log, "******* COMPUTING ALBEDO OF THE FIELD ******* \n\n");
					if (snow > 0.5)
					{
						albedo = 0.6;
						logger(g_log, "albedo: %f\n", albedo);
					}
					else //FIXSERGIO ORDINA
					{
						if (stage < 4)
						{
							albedo = 0.23 + pow((_LAI - 4), 2) / 160.0;
							logger(g_log, "albedo: %f\n", albedo);

						}
						else if (stage > 3 && stage < 7)
						{
							albedo = 0.23 - (0.23 - bareGroundAlbedo) * (- 0.75) * _LAI;
							logger(g_log, "albedo: %f\n", albedo);
						}
						else if (stage < 9 && stage > 6)
						{
							albedo = bareGroundAlbedo;
							logger(g_log, "albedo: %f\n", albedo);
						}

					}



					/********************************************************************************************************************
					 *										TEMPERATURE MODULE
					 * ********************************************************************************************************************/

					//############### air temperature ###################

					logger(g_log, "\n******** CANOPY MONTHLY TEMPERATURES ********");

					//setting the maximum and minimum temperature of the whole year
					/*
							for(i = 0; i < 12; i++)
							{
								for (l = 0; l < met[i].n_days; l++)
								{
								if (met[i].d[l].tavg > max)
								{
									max = met[i].d[l].tavg;
								}
								if ( met[i].d[l].tavg < min)
								{
									min =  met[i].d[l].tavg;
								}
								}
							}
					 */
					//if it is intended as the mean monthly value
					if (met[month].d[day].tavg != INVALID_VALUE)
					{
						logger(g_log, "ERRORE IN CROPMODEL_DAILY.C!!! RIGA 809\n");
						logger(g_log, "ERRORE IN CROPMODEL_DAILY.C!!! RIGA 809\n");
						logger(g_log, "ERRORE IN CROPMODEL_DAILY.C!!! RIGA 809\n");
						/*for(i = 0 ; i < 12; i++)
						{
							for (l = 0; l < met[i].n_days; l++)
							{
								monthlyAverageTemperature += met[i].d[l].tavg;
							}
							monthlyAverageTemperature /= met[i].n_days;

							if (monthlyAverageTemperature > max)
							{
								max = monthlyAverageTemperature;
							}
							if (monthlyAverageTemperature < min)
							{
								min = monthlyAverageTemperature;
							}
						}*/
					}
					else
					{
						logger(g_log, "ERRORE IN CROPMODEL_DAILY.C!!! RIGA 832\n");
						logger(g_log, "ERRORE IN CROPMODEL_DAILY.C!!! RIGA 832\n");
						logger(g_log, "ERRORE IN CROPMODEL_DAILY.C!!! RIGA 832\n");
						/*for(i = 0 ; i < 12; i++)
						{
							for (l = 0; l < met[i].n_days; l++)
							{
								monthlyAverageTemperature += (met[i].d[l].tmax * 0.604 + met[i].d[l].tmin * 0.396);
							}
							monthlyAverageTemperature /= met[i].n_days;

							if (monthlyAverageTemperature > max)
							{
								max = monthlyAverageTemperature;
							}
							if (monthlyAverageTemperature < min)
							{
								min = monthlyAverageTemperature;
							}
						}*/
					}
					annualTemperatureAmplitude = max - min;
					meanAnnualTemperature = max * 0.5 + min * 0.5;
					logger(g_log, "\nThe difference between the annual maximum and annual minimum temperature is:%f", annualTemperatureAmplitude);
					logger(g_log, "\nthe mean value between annual max and min temperature: %f", meanAnnualTemperature);

					//------  setting air temperature (max, mean, min)  -------------------------------------------------------

					maxAirTemperature = met[month].d[day].tmax;
					minAirTemperature = met[month].d[day].tmin;
					if (met[month].d[day].tavg != -9999)
					{
						meanDailyTemperature = met[month].d[day].tavg;
					}
					else
					{
						meanDailyTemperature = met[month].d[day].tmax * 0.604 * met[month].d[day].tmin * 0.396;
						logger(g_log, "\nmeanDailyTemperature estimated using Runing et al.");
					}
					logger(g_log, "\nmax Tair: %f\nMin Tair: %f", maxAirTemperature, minAirTemperature);
					logger(g_log, "\nmeanDailyTemperature: %f", meanDailyTemperature);

					//---------------------------------------------------------------------------------------------------------

					//########## CANOPY TEMPERATURE ############

					// effect of snow on cover on canopy temperature
					if (meanDailyTemperature >=0) 			//not totally clear on Zhang 2002 appendix; anyway I'm quite sure it's to be used Tair instead of Tmax/min
					{
						snow_eff = 1;
					}
					else if (meanDailyTemperature <0 )
					{
						snow_eff = 2 + meanDailyTemperature * (0.4 + 0.0018 * pow((MIN(SNOW_COEFF,snow) -15), 2));
					}
					logger(g_log, "\nsnow effect coefficient equal to %f", snow_eff);

					canopyMaxTemperature = snow_eff * maxAirTemperature; //met[month].tavg;    T_max in input
					logger(g_log, "\nCanopy max temperature is %f\n", canopyMaxTemperature);


					canopyMinTemperature = snow_eff * minAirTemperature;	//met[month].tavg;	  T_min in input
					logger(g_log, "\nCanopy min temperature is %f\n", canopyMinTemperature);

					//Canopy daily mean temperature
					meanCanopyTemperature = 0.6 * canopyMaxTemperature + 0.4 * canopyMinTemperature;
					logger(g_log, "\nCanopy mean temperature is %f\n", meanCanopyTemperature);

					//canopy day time mean temperature
					canopyDaytimeMeanTemperature = 0.5 * meanCanopyTemperature + 0.5 * canopyMaxTemperature;
					logger(g_log, "\nCanopy daytime mean temperature is %f\n", canopyDaytimeMeanTemperature);





					//setting layerSoilTempEffectPhotoRate: temperature reduction factor
					if (meanCanopyTemperature <= 5.0)		//or 10 °C???
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

					layerSoilTempEffectPhotoRate = 1.0;
					logger(g_log, "soilLayerTempEffectPhotoRate = %f",layerSoilTempEffectPhotoRate);


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
					logger(g_log, "\ndailyMaximumSnowMelt",dailyMaximumSnowMelt);

					//daily maximum crop interception (cm of water)
					dailyMaximumPlantInterception = 0.02 * _LAI;
					logger(g_log, "\ndailyMaximumSnowMelt: %f \nDaily maximum plant interception: %f", dailyMaximumSnowMelt, dailyMaximumPlantInterception);


					logger(g_log, "\n******* CALCULATING WATER SURFACE RUNOFF: CURVE NUMBER ESTIMATION METHOD ********\n\n");



					/***********************************************************************************************************************
					 *									CURVE NUMBER SETTING  stagione riposo: set it using development module
					 ************************************************************************************************************************/

					/***************************************************************************
					 * 						RUNOFF CURVE NUMBER
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
					logger(g_log, "\nCURVE NUMBER VALUE\n");
					logger(g_log, "\nCurveNumber %f", CN);

					//--------------------------------------------------------------------------------------------------------------------


					//SCS CN correction method:

					if (stage >= 3 && stage < 8)
					{
						if(met[month].d[day-4].prcp + met[month].d[day-3].prcp + met[month].d[day-2].prcp + met[month].d[day-1].prcp + met[month].d[day].prcp < 13)
						{
							CN = 4.2 * CN / (10.0 - 0.058 * CN);
						}
						else if (met[month].d[day-4].prcp + met[month].d[day-3].prcp + met[month].d[day-2].prcp + met[month].d[day-1].prcp + met[month].d[day].prcp > 28)
						{
							CN = 23.0 * CN / (10.0 - 0.058 * CN);
						}
					}
					else
					{
						if (day > 4)
						{
							if(met[month].d[day-4].prcp + met[month].d[day-3].prcp + met[month].d[day-2].prcp + met[month].d[day-1].prcp + met[month].d[day].prcp < 36)
							{
								CN = 4.2 * CN / (10.0 - 0.058 * CN);
							}
							else if (met[month].d[day-4].prcp + met[month].d[day-3].prcp + met[month].d[day-2].prcp + met[month].d[day-1].prcp + met[month].d[day].prcp > 54)
							{
								CN = 23.0 * CN / (10.0 - 0.058 * CN);
							}
						}
					}


					logger(g_log, "\nCurveNumber corrected by SCS methodn on previous rainy days number: %f", CN);

					/*************************************************************************************************************************************/

					//retention factor
					maxInfiltration = 254.0 * (100 - CN) / CN;
					if(met[month].d[day].prcp >= 0.2 * maxInfiltration)
					{
						//Runoff
						Runoff = 0.1 * pow((met[month].d[day].prcp - 0.2 * maxInfiltration),2) / (met[month].d[day].prcp + 0.8 * maxInfiltration);
					}else
					{
						//Runoff
						Runoff = 0;
					}
					logger(g_log, "\nfor a retention factor: %f", maxInfiltration);
					//logger(g_log, "\nand for a monthly rainfall of %f mm", met[month].rain);
					logger(g_log, "\nestimated runoff: %f\n", Runoff);


					if (met[month].d[day].swc != -9999)
					{
						//soil water content from input: it is a whole profile value
						profileWaterContent = met[month].d[day].swc;
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

					logger(g_log, "\n**************************************\n"
							"soil water balance (CERES WHEAT 2.0)\n"
							"****************************************\n");

					//assumed as infiltrated water, the net amount of (precipitation + melt) - (runoff and interception)
					infiltration = (met[month].d[day].prcp + dailyMaximumSnowMelt - Runoff - dailyMaximumPlantInterception) / 10.0;	//watch out if the variables are set in mm; here in cm
					logger(g_log, "\ninfiltration %f", infiltration);

					for (l = 0; l < soilLayer; l++)
					{
						//fixsergio flux computation has to be confirmed
						if (l == 0)
						{
							//supposed by me: at the very first layer all the water infiltrated moves from the surface to this layer
							flux = infiltration;
							logger(g_log, "\nflux: %f", flux);
						}

						if (flux > 0)
						{
							Hold[l] = (layerFieldSaturation[l] - layerMoisture[l]) * soilLayerThickness[l];
							logger(g_log, "\nhold[%d]: %f", l, Hold[l]);
							if (flux <= Hold[l])
							{
								layerMoisture[l] += flux / soilLayerThickness[l];	//water = previous water + (new water / soil thickness)
								logger(g_log, "\nflux < hold; SW: %f", layerMoisture[l]);
								//at this point is evaluated if infiltration is greater than layer field capacity; if yes there's infiltration
								if ( layerMoisture[l] < fieldCapacityLayerMoisture[l])
								{
									drain[l] = 0.0;
									logger(g_log, "\nlayerMoisture < fieldCapacity: drain = %f", drain[l]);
								}
								else
								{
									drain[l] = (layerMoisture[l] - fieldCapacityLayerMoisture[l]) * SWcon * soilLayerThickness[l];
									flux = drain[l];
									logger(g_log, "\nlayerMoisture > fieldCapacity: drain = %f", drain[l]);
									layerMoisture[l] = layerMoisture[l] - drain[l] / soilLayerThickness[l];
									logger(g_log, "\nnew SW[l]: %f", layerMoisture[l]);
								}
							}
							else
							{
								drain[l] = SWcon * (layerFieldSaturation[l] - fieldCapacityLayerMoisture[l]) * soilLayerThickness[l];
								flux = flux - Hold[l] + drain[l];
								logger(g_log, "\nflux > hold: drain[%d] = %f\n\tflux = %f",l,drain[l],flux);
							}
							logger(g_log, "\n\n****UPWARD MOVEMENT****");
							normVolumetricWater[l] = MAX(layerMoisture[l] - layerWilting[l],0);		// LL[i] constant (wilting point)
							logger(g_log, "\nnormalizedVolumetricWaterContent %f", normVolumetricWater[l]);
						}
					}


					/*******************************************************************************************************************************
					 * 											EVAPOTRANSPIRATION MODULE
					 ******************************************************************************************************************************/

					//equilibrium evapotranspiration (solar rad is supposed Mj m-2 day-1; EEq cm day-1)
					equilibriumEvapotranspiration = 0.0001 * dailySolarRadiation * (4.88 - 4.37 * albedo) * (canopyDaytimeMeanTemperature + 29);	//equilibriumEvapotranspiration
					logger(g_log, "\nEquilibrium evapotranspiration: %f", equilibriumEvapotranspiration);
					logger(g_log, "\ncanopy temperature %f", canopyDaytimeMeanTemperature);

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
						//potentialEvapotranspiration = equilibriumEvapotranspiration * (1-0.43 * _LAI);
					}


					// potential evaporation
					if (_LAI < 1)
					{
						potentialEvaporation = potentialEvapotranspiration * (1 - 0.43 * _LAI);
					}
					else
					{
						potentialEvaporation = potentialEvapotranspiration / 1.1 * exp (-0.4 * _LAI);
					}
					logger(g_log, "\nptentialEvapotranspiration %f\npotentialEvaporation %f", potentialEvapotranspiration, potentialEvaporation);

					/*******************************************************************************
					 * 					RITCHIE ACTUAL RATE OF SOIL EVAPORATION
					 ******************************************************************************/
					/*
								logger(g_log, "\n*****SOIL EVAPORATION MODULE*****");

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
								logger(g_log, "\n*****END OF SOIL EVAPORATION MODULE*****");

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
									logger(g_log, "\ndiffusion flux = %f",Diffusion);
									//to be reviewed
									layerMoisture[l] -= Diffusion / soilLayerThickness[l];
									logger(g_log, "\nnew SW[%d] %f",l,layerMoisture[l]);
									if(l-1 >= 0)
									{
										layerMoisture[l - 1] += Diffusion / soilLayerThickness[l-1];
										logger(g_log, "\nnew SW[%d] %f",l-1,layerMoisture[l-1]);
									}
									//SW[l] correction
									//SW[l] += Diffusion
									logger(g_log, "\nlayerMoisture: \n\t layer %d, %f \n\t layer %d, %f",l,layerMoisture[l],l-1,layerMoisture[l-1]);
								}
								if (_LAI < 3)
								{
									potentialTranspiration = potentialEvapotranspiration * _LAI /3
								}
								else
								{
									potentialTranspiration = potentialEvapotranspiration
								}
								if ( actualSoilEvaporation + potentialTranspiration > potentialEvapotranspiration)
								{
									potentialTranspiration = potentialEvapotranspiration - actualSoilEvaporation;
								}

								logger(g_log, "\nEnd of soil waer balance module\n\n");


								logger(g_log, "\n*******ACTUAL EVAPORATION*********");
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




							############################################################################################################################################
					 */
					// potential transpiration
					potentialTranspiration = potentialEvapotranspiration - potentialEvaporation;
					logger(g_log, "\nfor a potential evapotranspiration of: %f \npotential transpiration: %f", potentialEvapotranspiration, potentialTranspiration);


					//effect of soil moisture on evaporation
					for (l = 0; l < soilLayer; l++)
					{
						moistureSoilEvaporationEffect += ((layerMoisture[l] - layerWilting[l]) / (fieldCapacityLayerMoisture[l] - layerWilting[l])) * soilLayerThickness[l] / g_soil_settings->values[SOIL_DEPTH];
					}
					//moistureSoilEvaporationEffect /= g_soil_settings->values[SOIL_DEPTH];
					logger(g_log, "\nSoil moisture effect on evaporation is: %f\n", moistureSoilEvaporationEffect);

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

						logger(g_log, "\nWater uptake cumulated till layer %d is: %f",l, waterUptakeSoilLayer);
					}
					logger(g_log, "\nwater total uptake is %f", waterUptakeSoilLayer);

					//Actual transpiration
					actualTranspiration = MIN(potentialTranspiration, waterUptakeSoilLayer);
					logger(g_log, "\nActual transpiration: %f", actualTranspiration);

					/*********************************************************************************************************************************************************
					 * 												WATER REDISTRIBUITION
					 *********************************************************************************************************************************************************/

					//checked

					//fixsergio a surface flooding module
					for (l = soilLayer; l > 1; l--)
					{

						//to be seen if new moisture is > fieldCapacity: if it's greater than field capacity what should be done!?
						// supposed: if the lower layer is full, then put it on the upper one; if it is the layer 0, it's a pool
						Diffusion = 0.88 * exp(35.4 * 0.5 *(normVolumetricWater[l-1] + normVolumetricWater[l])) *
								(normVolumetricWater[l] - normVolumetricWater[l-1]) / (soilLayerThickness[l] + soilLayerThickness[l-1]) * 0.5;
						logger(g_log, "\ndiffusion flux = %f",Diffusion);
						//to be reviewed
						layerMoisture[l] -= Diffusion / soilLayerThickness[l];
						logger(g_log, "\nnew SW[%d] %f",l,layerMoisture[l]);
						if(l-1 >= 0)
						{
							layerMoisture[l - 1] += Diffusion / soilLayerThickness[l-1];
							logger(g_log, "\nnew SW[%d] %f",l-1,layerMoisture[l-1]);
						}
						// SW[l] correction
						//SW[l] += Diffusion
						logger(g_log, "\nlayerMoisture: \n\t layer %d, %f \n\t layer %d, %f",l,layerMoisture[l],l-1,layerMoisture[l-1]);
					}
					logger(g_log, "\nEnd of soil water balance module\n\n");

					/*******************************************************************************************************************************
					 * 											EVAPOTRANSPIRATION MODULE
					 ******************************************************************************************************************************/

					//equilibrium evapotranspiration (solar rad is supposed Mj m-2 day-1; EEq cm day-1)
					equilibriumEvapotranspiration = 0.0001 * dailySolarRadiation * (4.88 - 4.37 * albedo) * (canopyDaytimeMeanTemperature + 29);	//equilibriumEvapotranspiration
					logger(g_log, "\nEquilibrium evapotranspiration: %f", equilibriumEvapotranspiration);
					logger(g_log, "\ncanopy temperature %f", canopyDaytimeMeanTemperature);
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
						potentialEvapotranspiration = equilibriumEvapotranspiration * ((canopyMaxTemperature - 24.0) * 0.05 + 1.1);
						//version read in zhang 2002
						//potentialEvapotranspiration = equilibriumEvapotranspiration * (1-0.43 * _LAI);
					}


					// potential evaporation
					if (_LAI < 1)
					{
						potentialEvaporation = potentialEvapotranspiration * (1 - 0.43 * _LAI);
					}
					else
					{
						potentialEvaporation = potentialEvapotranspiration / 1.1 * exp (-0.4 * _LAI);
					}

					// potential transpiration
					actualTranspiration = potentialEvapotranspiration - potentialEvaporation;
					logger(g_log, "\nfor a potential evapotranspiration of: %f \npotential transpiration: %f", potentialEvaporation, actualTranspiration);



					//effect of soil moisture on evaporation
					for (l = 0; l < soilLayer; l++)
					{
						moistureSoilEvaporationEffect += ((layerMoisture[l] - layerWilting[l]) / (fieldCapacityLayerMoisture[l] - layerWilting[l])) * soilLayerThickness[l]/g_soil_settings->values[SOIL_DEPTH];
					}
					//moistureSoilEvaporationEffect /= g_soil_settings->values[SOIL_DEPTH];
					logger(g_log, "\nSoil moisture effect on evaporation is: %f\n", moistureSoilEvaporationEffect);

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
						waterUptakeSoilLayer += rootWaterUptakeCoefficient * layerRootLengthDensity[l] / (0.2 + 0.2 *
								layerRootLengthDensity[l]) * moistureEffectWaterUptake1[l] * soilLayerThickness[l];

						logger(g_log, "\nWater uptake cumulated till layer %d is: %f", waterUptakeSoilLayer);
					}
					logger(g_log, "\nwater total uptake is %f", waterUptakeSoilLayer);

					//Actual transpiration
					actualTranspiration = MIN(actualTranspiration, waterUptakeSoilLayer);
					logger(g_log, "\nActual transpiration: %f", actualTranspiration);

					if (potentialTranspiration == 0)
					{
						waterStressFactor = 0;
					}
					else
					{
						//Crop water stress factor
						waterStressFactor = actualTranspiration / potentialTranspiration;
						logger(g_log, "\nwaterStressFactor = %f",waterStressFactor);
					}
					//---------------------------------------------------------------------------------------------------------------------------------------------------

					logger(g_log, "\n\n*****************************************************************************************"
							"*\n 					DNDC SOIL SURFACE TEMPERATURE PREDICTION"
							"*\n****************************************************************************************");
					/*
					 * DNDC uses an albedo algorithm which is function of snow and species' phenological stage; somehow it considers both aboveground biomass and snow buffering.
					 * as a matter of fact, it is considereded a function of LAI (differently, as wheat is in vegetative or maturity stage), or of snow.
					 * at the end i've chosen to use albedo, because it varies between bare ground (about 0.17) and a max value of 0.6 (snowCover); there are two questions anyway
					 * 		- using a snow treshold of 5mm, albedo doesn't change any further with increasing snow depth; this may be ok (snow ensure a strong buffering effect, so
					 * 			temperature below may stay inalterated and quite above 0
					 * 		- biomass influence is summarized by lai, and not by weight; is it meaning that litter doesn't influence soilSurfaceTempearture?
					 *
					 * anyway, in order to avoid "tuning", the author prefers not to use a hybrid algorithm, even though Zhang himself suggests his being taken from Williams'
					 * and use the one suggested by Zhang and giving quite reliable results
					 *
					 *

					 	 	 //albedo: here is imposed as a comment since it has to be computed before soilWater module, which preceeds soilTemperature
					 	 	//Zhang's way to compute albedo is taken from Ritchie '88: Ceres Wheat 2.0, cap. 3: Soil Water Balance
							if (snow > 0.5)			// mm of water equivalent
							{
								//is the same as Williams for a snow cover of 5mm; here's imposed as a maximum threshold
								albedo = 0.6;
								logger(g_log, "albedo: %f\n", albedo);
							}
							else
							{
								//this means that the algorithm predicts a different way to compute albedo in case plants are still in non mature vegetative state (stage < 4)
								if (stage < 4)
								{
									albedo = 0.23 + pow((_LAI - 4), 2) / 160.0;
									logger(g_log, "albedo: %f\n", albedo);

								}
								//or in mature stage (from grain filling to harvest)
								else if (stage > 3 && stage < 7)
											{
												albedo = 0.23 - (0.23 - bareGroundAlbedo) * (- 0.75) * _LAI;
												logger(g_log, "albedo: %f\n", albedo);
											}
								else if (stage < 9 && stage > 6)
								// or finally if there's no plant cover (the cases from harvest to emergence)
								{
									albedo = bareGroundAlbedo;
													logger(g_log, "albedo: %f\n", albedo);
								}
					 */

					//this fork is meant to avoid the previous soilSurfaceTemperature missinig value to slightly reduce the estimated current day estimation
					if (month == JANUARY)
					{
						previousDaySoilTemp = yos0SoilSurfaceTemperature;
						//fixsergio is it possible to take the last value of the previous one for following years?!
						logger(g_log, "\nIt's january the first! impossible to take previous soil surface temperature; \n\tpreviousDaySoilTemp %f = ", previousDaySoilTemp);
					}

					/*
					 * DNDC uses canopy temperature: it is the same as air temperature, for Tair > 0;
					 * for Tair < 0 it corresponds to Tair * (2 + T [ 0.4 + 0.0018 * min(15.0, Snow) - 15)^2]
					 * anyway i suppose there shouldn't be a great bias in simply using tmax and tmin
					 */

					//DNDC formulation: (1 - albedo) (Tmean + (Tmax - Tmin) (0.03 SolarRadiation [MJ/m^2d])^(0.5) + (albedo Tsoil)
					//soil surface temperature using DNDC
					soilSurfaceTemperatureDNDC = (1 - albedo) *
							(met[month].d[day].tavg + (met[month].d[day].tmax - met[month].d[day].tmin) *
									sqrt(0.03 * met[month].d[day].solar_rad) + albedo * previousDaySoilTemp);
					logger(g_log, "\nsoilSurfaceTemperatureDNDC %f", soilSurfaceTemperatureDNDC);

					logger(g_log, "\ntmax %f",met[month].d[day].tmax);
					logger(g_log, "\ntmin %f", met[month].d[day].tmin);
					logger(g_log, "\nsolarRadiation %f",met[month].d[day].solar_rad);
					logger(g_log, "\nsoilSurfaceTemperatureDNDC %f", soilSurfaceTemperatureDNDC);

					//setting the previous day's soilSurfaceTemperature
					previousDaySoilTemp = soilSurfaceTemperatureDNDC;

					//-----------------------------------------------------------------------------------------------------------------------------




					/*****************************************************************************************************
					 * 			SOIL TEMPERATURE AT DIFFERENT LAYER DEPTH: IMPORTANT! THIS MUST OCCURR AFTER SWC MODULE
					 **********************************************************************************************************/

					//computing soil temperature using T
					for (l = 0; l < soilLayer; l++)
					{
						logger(g_log, "\n\n**** starting evaluation of soil temperature in different layers ****");
						logger(g_log, "\nfor a %d number of soil layer used:", soilLayer);
						logger(g_log, "\n****** LAYER %d ********\n", l);

						//assumed soil water content and bulk density as a soil constant

						// SW and BD have to be layer specific not the value for the whole profile
						mid2 = profileWaterContent / (0.356 - 0.144 * soilBulkDensity) * g_soil_settings->values[SOIL_DEPTH];
						maxDampingDepth = 1.00 + 2.5 * soilBulkDensity / (soilBulkDensity + exp (6.53 - 5.63 * soilBulkDensity));
						logger(g_log, "\n***Coefficients to evaluate depth weighting factors ***\ncoeff1: %f \ncoeff2: %f", maxDampingDepth, mid2);

						//damping depth: factor which simulates soil buffer effect on temperature oscillations:
						//the more the layer is deepened , the more temperature is constant during the year (function of annual mean temperature)
						dampingDepth[l] = maxDampingDepth * exp (log(0.5 / maxDampingDepth) * pow(((1.0 - mid2)/(1.0 + mid2)), 2.0));
						logger(g_log, "\ndamping depth: %f", dampingDepth[l]);

						logger(g_log, "\n\nCHECKED!!!");



						displacementHeight = 0.702 * pow(canopyHeight, 0.979);		//taken from Penmam-Monteith



						if ( soilLayer == 1)
						{


							depthTempFactor = displacementHeight / ((soilLayerDepth[l] / dampingDepth[l]) + exp(-0.8669 - 2.0775 * soilLayerDepth[l] / dampingDepth[l]));
							//soilLayerTemperature[l] = met[month].tavg + exp(-1.0 / dampingDepth);

							soilLayerTemperature[l] = lag * prevDaySoilLayerTemperature[l] + (1.0 - lag) * (depthTempFactor * (meanAnnualTemperature -
									soilSurfaceTemperature) + soilSurfaceTemperature);

						}
						else
						{
							depthTempFactor = displacementHeight / ((0.5 * (soilLayerDepth[l] + soilLayerDepth[l-1]) / dampingDepth[l])
									+ exp(-0.8669 -2.0775 * (0.5 * (soilLayerDepth[l] + soilLayerDepth[l-1]) / dampingDepth[l])));

							soilLayerTemperature[l] = lag * prevDaySoilLayerTemperature[l] + (1.0 - lag) * (depthTempFactor * (meanAnnualTemperature -
									soilSurfaceTemperature) + soilSurfaceTemperature);
							logger(g_log, "\n\nsoilLayerTemp = %f", soilLayerTemperature[l]);
						}

					}
					logger(g_log, "\nend of the soil temperature module (DNDC||EPIC)");
					//layer specific soil temperature, as estimated at the center of the layer


					//--------------------------------------------------------------------------------------------------------------------------------------



					/*****************************************************************************************************************/

					// CHECKED: SEEMS OK!!!!



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

					/*
							//estimation of canopy hourly diurnal temperature; not in use for now

							// canopy hourly temperature during daytime
							for(hourDay = dawn3; hourDay < (sunset); hourDay ++)
							{
								//canopyMinTemperature + (canopyMaxTemperature - canopyMinTemperature) *
								meanDiurnalCanopyTemperature = canopyMinTemperature + (canopyMaxTemperature - canopyMinTemperature) * sin(Pi * ((double)hourDay - 11.82 + 0.5 * daylength) / (daylength - 3.3));
								logger(g_log, "\nat hour %d canopy mean temperature assumed as: %f °C", hourDay, meanDiurnalCanopyTemperature);
								canopyDaytimeMeanTemperature2 += meanDiurnalCanopyTemperature;
							}
							canopyDaytimeMeanTemperature2 = canopyDaytimeMeanTemperature2 / (int)sunshine;
							logger(g_log, "\n\nCanopy hourly estimated temperature in daytime (DNDC): %f °C", canopyDaytimeMeanTemperature2);


							//mean nighttime temperature, cumulated for the non day time hours as integers
							Tc_n = canopyMinTemperature + (Tset - canopyMinTemperature) * exp(2 * Pi * (hourDay[i] - 11.82 + 0.5 * daylength)/ (24 - daylength));
							logger(g_log, "\nCanopy hourly estimated temperature in night time: %f °C",Tc_n);

					 */

					developmentRate = dailyThermalTime / stageLimit[stage];
					logger(g_log, "\nfirst DR = %f",developmentRate);

					if (developmentRate >= 1.0)
					{
						dailyThermalTime -= stageLimit[stage];
						stage += 1;
						developmentRate = dailyThermalTime / stageLimit[stage];		//it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
						// stage rather than surfaceTemperatureAdjustment)
						logger(g_log, "\n\ndailyThermalTime: %f", dailyThermalTime);
						logger(g_log, "\nfirst DR = %f",developmentRate);
					}
					if (stop_cycle == 0)
					{
						if (speciesParam == 0)
						{

							/*********************************************************************************************************
							 * 							species_t/ VARIETAS SPECIFIC VARIABLES' SETTINGS
							 ********************************************************************************************************/

							//getDaylength(&m->cells[cell], day, month, years, MonthLength[month], yos);
							logger(g_log, "\nDaylength %f", met[month].d[day].daylength);

							//########################################################################################################

							switch(stage)		// several different plant types: this case valid for wheat
							{
							case 0:
								logger(g_log, "\n*****************"
										"\n\t* Emergence to terminal spikelet *"
										"\n****************");



								/*********************************************
								 * 		DEFINING DTT REDUCTING FACTORS
								 ********************************************/

								//Cao et al. 1997 "Modelling phasic development in wheat: a conceptual integration of physiological components":
								//assumed the same structure, using Tavg as Temp[i] and using a fixed value of vernalization (Ritchie, 88, to which Zhang refers)

								for (i = 1 ; i < 9; i++)
								{
									//imposing temperature vernalziation factor;
									temperatureVernFactor = 0.931 + 0.114 * i - 0.0703 * pow(i,2) + 0.0053 * pow(i,3);

									//computing vernalization effectiveness
									vernTemperature = met[month].d[day].tmin + temperatureVernFactor  * (met[month].d[day].tmax - met[month].d[day].tmin);
									if(vernTemperature <= 0 && vernTemperature >= maxVernTemperature)
									{
										vernalizationDays = 0.0;
									}
									else if(vernTemperature < 1 && vernTemperature > 0)
									{
										vernalizationDays = vernTemperature;
									}
									else if (vernTemperature <= vernOptimumTemperature && vernTemperature >= 1)
									{
										vernalizationDays = 1;
									}
									else if(vernTemperature < maxVernTemperature && vernTemperature > vernOptimumTemperature)
									{
										vernalizationDays = (maxVernTemperature - vernTemperature) / (maxVernTemperature - vernOptimumTemperature);
									}
									dailyVernalizationEffectiveness += 0.125 * vernalizationDays;

									//computing devernalization influence on vernalization effectiveness
									if ( vernTemperature < 27.0 && vernalizationDays < (0.3 * vernalizationCoefficient * 50))
									{
										vernalizationDays -= (temperatureVernFactor - 27.0) * 0.5;
									}
								}
								vernalizationFactor = vernalizationDays / (50.0 * vernalizationCoefficient);
								logger(g_log, "\nvernalizationFactor %f", vernalizationFactor);

								//photoperiodism effect
								if (daylength < 20)
								{
									//fixsergio: use a photoperiod variable instead of daylength
									photoperiodFactor = 1 - photoperiodCoefficient * pow(20.0 - daylength, 2);
								}
								logger(g_log, "\nPhotoperiodFactor %f", photoperiodFactor);

								//fixsergio to be adjusted; in theory it is supposed to be used the value of each hour, using a canopy temperature hourly estimation
								// at least divide the progression between daily and nightly cases

								//added a fork to avoid photoperiodFactor or vernalization factor to bve equal to 0; so that Dtt 'd not increase by 0
								if (photoperiodFactor <= 0 || vernalizationFactor <= 0)
								{
									dailyThermalTime += MIN((maxDevelopmentTemperature - basalTemperature), MAX(0, meanCanopyTemperature));
									logger(g_log, "\ndailyThermalTime = %f", dailyThermalTime);
								}
								else
								{
									dailyThermalTime += MIN((maxDevelopmentTemperature - basalTemperature), MAX(0, meanCanopyTemperature))
																													* MIN(photoperiodFactor, vernalizationFactor);
									logger(g_log, "\ndailyThermalTime = %f", dailyThermalTime);
								}

								sumDailyThermalTime += dailyThermalTime;
								logger(g_log, "\ncumulated dailyThermalTime %f", sumDailyThermalTime);

								developmentRate = dailyThermalTime / stageLimit[stage];
								logger(g_log, "\ndevelopmentRate = %f",developmentRate);



								/*******************************************
								 * 				PHOTOSYNTHESIS
								 ******************************************/



								logger(g_log, "\n\n******** PHOTOSYNTHESIS MODULE *********\n ");


								// Light which is perpendicular to leaf optimumPhotoRate
								lightSaturationPhotoRate = optimumPhotoRate * layerSoilTempEffectPhotoRate;
								logger(g_log, "\nPhotosynthesis rate at light saturation: %f kg CO2/ha/h", lightSaturationPhotoRate);


								logger(g_log, "\n\n******** PHOTOSINTETICALLY ACTIVE RADIATION AT CERTAIN TIME ********* \n ");
								//get photosynthetically active radiation at certain time

								//integral of solar elevation during the day
								solarElevationSumDNDC = 3600.0 * (daylength * (Sfactor + 0.4 * (pow(Sfactor, 2) + 0.5 * pow(Cfactor, 2))) +
										24.0 / Pi * Cfactor * (1 + 0.6 * Sfactor) * sqrt(1 - pow(Sfactor/ Cfactor,2)));
								logger(g_log, "solarElevationSUM %f",solarElevationSumDNDC);

								//the external cycle has to be that computing for 3 different hours

								// Atmospheric transmission coefficient
								atmosphericTrasmissionCoeff = (dailySolarRadiation * 3600.0 * daylength) /
										(solarConstant * solarElevationSumDNDC);
								logger(g_log, "\nAtmospheric transmission coefficient: %f", atmosphericTrasmissionCoeff);

								//setting the three different hours by using a gaussian integration over a period from noon to sunset
								for (i = 0; i < 3; i++)
								{
									// setting the 3 different hours to evaluate mean daily photosynthesis

									// three points for Gaussian integration;
									gaussianIntegrationHour[i] = 12 + 0.5 * daylength * gaussianParameter1[i];
									logger(g_log, "\nAssuming the gaussian integration, setted the hour %f,", gaussianIntegrationHour[i]);

									// canopy hourly temperature during daytime

									//sine of the elevation angle of the sun
									solarElevation = Sfactor + Cfactor * cos(2 * Pi * (gaussianIntegrationHour[i] - 12.0) / 24.0);
									logger(g_log, "\nsine of the elevation angle of the sun: %f", solarElevation);

									//mean par for the certain time;
									//hourPar = 0.55 * dailySolarRadiation * solarElevation * (1 + 0.4 * solarElevation) / solarElevationSumDNDC;
									hourPar = dailySolarRadiation / 2.0;
									logger(g_log, "\n\n\nhourPar = %f",hourPar);
									/*
											//reflection coefficient taken from eq.1 Spitters, 96
											reflectionCoefficient = (1 - sqrt(1 - scatterLightParameter)) / (1 + sqrt(1 - scatterLightParameter)) *
													(2 / (1 + 1.6 * solarElevation));
									 */
									// Reflectivity of horizontally distribuited canopy
									canopyHorizontalReflectivity = (1 - sqrt(1 - scatterLightParameter)) / (1 + sqrt(1-scatterLightParameter));
									logger(g_log, "\nreflectivity of horizontally distribuited canopy %f", canopyHorizontalReflectivity);

									// Reflectivity of sphaerical distribuited canopy = reflectionCoefficient
									canopySpharicalReflectivity = 2 * canopyHorizontalReflectivity / (1 + 2.0 * solarElevation);
									logger(g_log, "\nreflectivity of sphaerical distribuition canopy %f", canopySpharicalReflectivity);

									//taken from Spitters 96, eq 7
									diffuseLightExtinctionCoeff = 0.8 * sqrt(1 - scatterLightParameter);
									logger(g_log, "\ndiffuseLightExtinctionCoefficient: %f", diffuseLightExtinctionCoeff);

									// Extinction coefficient of assumed black body leaves
									blackBodyExtinctionCoeff = (0.5 * diffuseLightExtinctionCoeff) / (0.8 * solarElevation * sqrt(1.0 - scatterLightParameter));
									logger(g_log, "\nextinction coefficient of leaves, assuming them as black bodies: %f", blackBodyExtinctionCoeff);

									//extinction coefficient of direct light
									directLightExtinctionCoeff = blackBodyExtinctionCoeff * sqrt((1 - scatterLightParameter));
									logger(g_log, "\nextinction coefficient of direct light %f",directLightExtinctionCoeff);

									// midVariable3 && midVariable4: mid coefficients
									midVariable4 = 0.847 - 1.61 * solarElevation * pow(solarElevation, 2);
									midVariable3 = (11.47 - midVariable4) / 1.66;
									logger(g_log, "\nbeing the two mid coefficients midVariable3 and midVariable4, respectively: %f and %f",midVariable3,midVariable4);

									//Fraction of diffuse light above the canopy
									if (atmosphericTrasmissionCoeff <= 0.22)
									{
										diffuseLightFraction = 0.99;
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

									diffuseLightAboveCanopy = hourPar * diffuseLightFraction;

									directLightAboveCanopy = hourPar - diffuseLightAboveCanopy;

									//Light which is perpendicular to leaf surface
									leafSurface90degLight = (1 - scatterLightParameter) * directLightAboveCanopy / solarElevation;
									logger(g_log, "\nLight perpendicular to leaf surface: %f (J/m^2/s)", leafSurface90degLight);

									for (l = 0; l < 3; l++)
									{
										//Canopy layer for gaussian integration
										canopyHeightLAI[l] = _LAI * gaussianParameter1[l];
										logger(g_log, "\nLAI above layer %d: %f",l, canopyHeightLAI[l]);

										//Fraction of sunlit area
										sunlitLeafAreaFraction = exp(-blackBodyExtinctionCoeff * canopyHeightLAI[l]);

										//-------------------------------------------------------------------------------------------------------------------------------------
										// Direct light
										directLight = (1 - canopySpharicalReflectivity) * directLightAboveCanopy *  sqrt(1 - scatterLightParameter) * blackBodyExtinctionCoeff *
												exp(-1 * sqrt(1 - scatterLightParameter) * blackBodyExtinctionCoeff * canopyHeightLAI[l]);
										logger(g_log, "\ndirect light accepted by sunlit leaves: %f", directLight);

										// Diffuse light : fixsergio sergio; spitters canopy shpaerical!!!
										diffuseLight = (1 - canopySpharicalReflectivity) * diffuseLightAboveCanopy * diffuseLightExtinctionCoeff *
												exp(- diffuseLightExtinctionCoeff * canopyHeightLAI[l]);
										logger(g_log, "\ndiffuse light accepted by sunlit leaves: %f", diffuseLight);

										//absorption of diffuse light taken from Spitters
										absorbedDiffuseLight = (1 - canopySpharicalReflectivity) * diffuseLightAboveCanopy * diffuseLightExtinctionCoeff *
												exp( - diffuseLightExtinctionCoeff * canopyHeightLAI[l]);

										//absorption of direct light taken from spitters
										absorptionDirectLight = (1 - canopySpharicalReflectivity) * directLightAboveCanopy * sqrt(1 - scatterLightParameter) *
												blackBodyExtinctionCoeff * exp (- 1 * (1 - scatterLightParameter) * blackBodyExtinctionCoeff * canopyHeightLAI[l]);

										//absorption of direct component of direct light
										absorptionDirectDirectLight = (1 - scatterLightParameter) * directLightAboveCanopy * blackBodyExtinctionCoeff *
												exp(-sqrt(1 - scatterLightParameter) * blackBodyExtinctionCoeff * canopyHeightLAI[l]);

										//Light absorbed by shaded leaves in a layer
										shadeLeavesLightAbsor = absorbedDiffuseLight + absorptionDirectLight - absorptionDirectDirectLight;

										//Photosynthesis rate of shaded leaves
										shadLeavesPhotoRate = lightSaturationPhotoRate * (1 - exp(-1 * shadeLeavesLightAbsor * initialLightUseEfficiency * 3600.0 / lightSaturationPhotoRate));
										logger(g_log, "\nPhotosynthesis rate of shaded leaves: %f (g CO2/m^2/h)",shadLeavesPhotoRate);

										//Photosynthesis rate of sunlit leaves
										sunlitLeavesPhotoRate = lightSaturationPhotoRate * (1 - ((lightSaturationPhotoRate - shadLeavesPhotoRate) *
												(1 - exp(-1 * leafSurface90degLight * initialLightUseEfficiency * 3600.0 / lightSaturationPhotoRate))) /
												(leafSurface90degLight * initialLightUseEfficiency *3600.0));
										logger(g_log, "\nPhotosynthesis rate of sunlit leaves: %f (g CO2/m^2/h)", sunlitLeavesPhotoRate);

										//Gross photosynthesis rate at layer Li and time tj
										layerGrossPhotoRate = sunlitLeafAreaFraction * sunlitLeavesPhotoRate + (1 - sunlitLeafAreaFraction) * shadLeavesPhotoRate;
										logger(g_log, "\n\nGross photosynthesis rate at crop layer %d: %f ( gCO2/m^2/h)",l, layerGrossPhotoRate);

										switch (l)
										{
										case 0:
											gaussIntegrPhoto = layerGrossPhotoRate * _LAI;
											logger(g_log, "\n partial denominator in the photosynthesis module: %f",gaussIntegrPhoto);
											break;
										case 1:
											gaussIntegrPhoto += 1.6 * layerGrossPhotoRate * _LAI;
											logger(g_log, "\n partial denominator in the photosynthesis module: %f",gaussIntegrPhoto);
											break;
										case 2:
											gaussIntegrPhoto += layerGrossPhotoRate * _LAI;
											logger(g_log, "\n partial denominator in the photosynthesis module: %f",gaussIntegrPhoto);
											break;
										}
									}
									//hourlyCanopyAssimilation: assumed * _LAI as seen in table 1 gaudriaan
									gaussIntegrPhoto /= 3.6;
									switch (i)
									{
									case 0:
										gaussIntegPhotoSum = daylength * gaussIntegrPhoto;
										logger(g_log, "\ngaussIntegrationPhotoSum %f", gaussIntegPhotoSum);
										break;
									case 1:
										gaussIntegPhotoSum += 1.6 * daylength * gaussIntegrPhoto;
										logger(g_log, "\ngaussIntegrationPhotoSum %f", gaussIntegPhotoSum);

										break;
									case 2:
										gaussIntegPhotoSum += daylength * gaussIntegrPhoto;
										logger(g_log, "\ngaussIntegrationPhotoSum %f", gaussIntegPhotoSum);

										break;
									}
									logger(g_log, "\nformatting gaussIntegrPhoto value: %f", gaussIntegPhotoSum);
								}

								//dailyCanopyAssimilation
								gaussIntegPhotoSum /= 3.6;

								// Effects of CO2 concentration on photosynthesis
								fCO2 = 1 + co2EffectPhotosynthesis * log(g_settings->co2Conc/340.0);

								logger(g_log, "\nwaterStress %f \nnitrogenStress %f \n fco2 %f", waterStressFactor, nitrogenStressFactor, fCO2);
								//daily gross photosynthesis (g/m^2) = 0.1×30/44min(waterStressFactor,ns)fCO2 sum(sum(P(Li,tj )LAI DLw2j w2)3, 3)
								dailyGrossPhoto = MIN(waterStressFactor,nitrogenStressFactor) * fCO2 * gaussIntegPhotoSum;
								logger(g_log, "\ndaily gross photosynthesis: %f g/m^2", dailyGrossPhoto);




								/****************************************************************
								 * 						RESPIRATION
								 ***************************************************************/

								logger(g_log, " ********** RESPIRATION ***********\n");

								// maintenance respiration: k stands for the number of comparts
								for (i = 0; i < 4; i++)
								{
									maintenanceRespiration += compartMaintenanceResp[i] * pow(Q10, (meanCanopyTemperature - 25)/10.0) * compartBiomass[i];
									logger(g_log, "\nMaintenanace respiration cumulated for %d compartments: %f g CO2/m^2\n", i, maintenanceRespiration);
								}

								//growth respiration
								growthRespiration = (dailyGrossPhoto - maintenanceRespiration) * (1 - 1 / growthRespEfficiency);
								logger(g_log, "Maintenance total respiration: %f\nGrowth respiration: %f\n", maintenanceRespiration, growthRespiration);

								//fixsergio sergio
								dailyAssimilate = dailyGrossPhoto - growthRespiration - maintenanceRespiration;  //assumed as the difference between GPP and Resp

								/*******************************************
								 * 			   	LEAF GROWTH
								 ******************************************/


								leafAreaGrowthRate = 7.5 * sqrt(cumPhyllocrons) * dailyThermalTime / phyllocron;
								plantLeafAreaGrowthRate = leafAreaGrowthRate * tillNumber;

								logger(g_log, "\ndailyAssimilate %f; \nleafAreaGrowthRate %f; \nplantLeafAreaGrowthRate %f\n", dailyAssimilate, leafAreaGrowthRate, plantLeafAreaGrowthRate);


								//uncertainties: should vary much more
								leafAreaToAssimilateWeight = 150.0 - 0.075 * dailyThermalTime;

								//mass of assimilates required tosupport that amount of daily expansion growth
								potentialLeafGrowth = plantLeafAreaGrowthRate  / leafAreaToAssimilateWeight;

								//remainder of the daily assimilate supply
								potentialRootGrowth = dailyAssimilate - potentialLeafGrowth;

								if (potentialRootGrowth < dailyAssimilate * .35)
								{
									potentialRootGrowth = dailyAssimilate * .35;
									potentialLeafGrowth = dailyAssimilate * .65;
								}

								totalCumulativeLeafArea += plantLeafAreaGrowthRate;

								logger(g_log, "\nleafAreaToAssimilateWeight; %f \npotentialLEafGrowth: %f, \npotenitialRootGrowth; %f;"
										" \ntotalCumulativeLeafArea; %f;", leafAreaToAssimilateWeight, potentialLeafGrowth, potentialRootGrowth, totalCumulativeLeafArea);

								/*****************************************
								 * 				TILLERING
								 ***************************************/

								//tillers

								//tillerRate1 depends on thermal time after emergence; after three phyllocrons tillers produced in direct proportion to the leaf number
								if (cumPhyllocrons == 3)
								{
									tillerRate1 = -2.5 + cumPhyllocrons;
								}
								else
								{
									tillerRate1 = 0.0;
								}
								tillersPerSquareMeter = plants * tillNumber;

								tillerRate2 = 2.56 * exp(-10.0) * pow(2500 - tillersPerSquareMeter, 3.0);

								tillNumber += dailyThermalTime / phyllocron * MIN(tillerRate1, tillerRate2);

								//tillering factor corrected with SWDF2 from water balance routine

								logger(g_log, "\n\n### TILLERS ###"
										"\ntillerRate1; %f \ntillerRate2; %f;\ntillNumber; %f", tillerRate1, tillerRate2, tillNumber);

								/******************************************
								 * 				LEAF SENESCENCE
								 *****************************************/

								if (phylloCounter >= phyllocron)
								{

									leafNumber++;
									//only 4 leaves per stem
									if (leafNumber > 4)
									{
										//fixsergio: is a rate Dtt/Ph gives the tt specific death rate
										leafAreaLossRate = (cumulativeLeafAreaPhyllocron[leafNumber - 4] -
												cumulativeLeafAreaPhyllocron[leafNumber - 5]) * dailyThermalTime / phyllocron;
										senescenceLeafArea += leafAreaLossRate;
										leafNumber = 4;

										//soilWaterStress limiting factor
									}

									phylloCounter -= phyllocron;
									for(l = 0; l < leafNumber - 2 ; l++)
									{
										cumulativeLeafAreaPhyllocron[l] = cumulativeLeafAreaPhyllocron[l+1];
									}
									cumulativeLeafAreaPhyllocron[4] = totalCumulativeLeafArea; //here was cumulative leaf area
									cumPhyllocrons ++;
								}

								logger(g_log, "\nLEAF SENESCENCE"
										"\nleafNumber %f;\nphylloCounter; %f; \ncumPhyllocrons; %d, \n", leafNumber, phylloCounter, cumPhyllocrons);


								//green leaf LAI calculation, m2
								_LAI = (totalCumulativeLeafArea - senescenceLeafArea) * plants * 0.0001;

								totalCumulativeLeafArea = cumulativeLeafAreaPhyllocron[leafNumber -1];

								leafWeight += potentialLeafGrowth;
								stemWeight = 0;
								grainWeight = 0;

								/******************************************************************************************************
								 * 						END OF ABOVEGROUND CROP DEVELOPMENT
								 *****************************************************************************************************/


								if (developmentRate >= 1.0)
								{
									dailyThermalTime -= stageLimit[stage];
									stage += 1;

									tillersPerSquareMeter = MIN(1000, tillNumber);

									developmentRate = dailyThermalTime / stageLimit[stage];		//it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
									// stage rather than surfaceTemperatureAdjustment)
									logger(g_log, "\n\ndailyThermalTime: %f", dailyThermalTime);
									logger(g_log, "\nfirst DR = %f",developmentRate);
								}

								break;

							case 1:
								logger(g_log, "\n**************************************"
										"\n from spikelet to end of leaf growth"
										"\n***************************************");

								/**********************************************************************************************************
								 * 				FROM SPIKELET TO END OF LEAF GROWTH
								 **********************************************************************************************************/

								//number of tilelrs developed usually exceeds the one that can develope stems and ears (potential exceeding sink)
								soilWaterInfluenceOnAssimilation = 0.1;

								dailyThermalTime += MIN((maxDevelopmentTemperature - basalTemperature), MAX(0, meanCanopyTemperature));
								logger(g_log, "\ndailyThermalTime = %f", dailyThermalTime);

								sumDailyThermalTime += dailyThermalTime;
								logger(g_log, "\ncumulated dailyThermalTime %f", sumDailyThermalTime);

								developmentRate = dailyThermalTime / stageLimit[stage];
								logger(g_log, "\ndevelopmentRate = %f",developmentRate);

								/*******************************************
								 * 				PHOTOSYNTHESIS
								 ******************************************/



								logger(g_log, "\n\n******** PHOTOSYNTHESIS MODULE *********\n ");


								// Light which is perpendicular to leaf optimumPhotoRate
								lightSaturationPhotoRate = optimumPhotoRate * layerSoilTempEffectPhotoRate;
								logger(g_log, "\nPhotosynthesis rate at light saturation: %f kg CO2/ha/h", lightSaturationPhotoRate);


								logger(g_log, "\n\n******** PHOTOSINTETICALLY ACTIVE RADIATION AT CERTAIN TIME ********* \n ");
								//get photosynthetically active radiation at certain time

								//integral of solar elevation during the day
								solarElevationSumDNDC = 3600.0 * (daylength * (Sfactor + 0.4 * (pow(Sfactor, 2) + 0.5 * pow(Cfactor, 2))) +
										24.0 / Pi * Cfactor * (1 + 0.6 * Sfactor) * sqrt(1 - pow(Sfactor/ Cfactor,2)));
								logger(g_log, "solarElevationSUM %f",solarElevationSumDNDC);

								//the external cycle has to be that computing for 3 different hours

								// Atmospheric transmission coefficient
								atmosphericTrasmissionCoeff = (dailySolarRadiation * 3600.0 * daylength) /
										(solarConstant * solarElevationSumDNDC);
								logger(g_log, "\nAtmospheric transmission coefficient: %f", atmosphericTrasmissionCoeff);

								//setting the three different hours by using a gaussian integration over a period from noon to sunset
								for (i = 0; i < 3; i++)
								{
									// setting the 3 different hours to evaluate mean daily photosynthesis

									// three points for Gaussian integration;
									gaussianIntegrationHour[i] = 12 + 0.5 * daylength * gaussianParameter1[i];
									logger(g_log, "\nAssuming the gaussian integration, setted the hour %f,", gaussianIntegrationHour[i]);

									// canopy hourly temperature during daytime

									//sine of the elevation angle of the sun
									solarElevation = Sfactor + Cfactor * cos(2 * Pi * (gaussianIntegrationHour[i] - 12.0) / 24.0);
									logger(g_log, "\nsine of the elevation angle of the sun: %f", solarElevation);

									//mean par for the certain time;
									//hourPar = 0.55 * dailySolarRadiation * solarElevation * (1 + 0.4 * solarElevation) / solarElevationSumDNDC;
									hourPar = dailySolarRadiation / 2.0;
									logger(g_log, "\n\n\nhourPar = %f",hourPar);

									// Reflectivity of horizontally distribuited canopy
									canopyHorizontalReflectivity = (1 - sqrt(1 - scatterLightParameter)) / (1 + sqrt(1-scatterLightParameter));
									logger(g_log, "\nreflectivity of horizontally distribuited canopy %f", canopyHorizontalReflectivity);

									// Reflectivity of sphaerical distribuited canopy = reflectionCoefficient
									canopySpharicalReflectivity = 2 * canopyHorizontalReflectivity / (1 + 2.0 * solarElevation);
									logger(g_log, "\nreflectivity of sphaerical distribuition canopy %f", canopySpharicalReflectivity);

									//taken from Spitters 96, eq 7
									diffuseLightExtinctionCoeff = 0.8 * sqrt(1 - scatterLightParameter);
									logger(g_log, "\ndiffuseLightExtinctionCoefficient: %f", diffuseLightExtinctionCoeff);

									// Extinction coefficient of assumed black body leaves
									blackBodyExtinctionCoeff = (0.5 * diffuseLightExtinctionCoeff) / (0.8 * solarElevation * sqrt(1.0 - scatterLightParameter));
									logger(g_log, "\nextinction coefficient of leaves, assuming them as black bodies: %f", blackBodyExtinctionCoeff);

									//extinction coefficient of direct light
									directLightExtinctionCoeff = blackBodyExtinctionCoeff * sqrt((1 - scatterLightParameter));
									logger(g_log, "\nextinction coefficient of direct light %f",directLightExtinctionCoeff);

									// midVariable3 && midVariable4: mid coefficients
									midVariable4 = 0.847 - 1.61 * solarElevation * pow(solarElevation, 2);
									midVariable3 = (11.47 - midVariable4) / 1.66;
									logger(g_log, "\nbeing the two mid coefficients midVariable3 and midVariable4, respectively: %f and %f",midVariable3,midVariable4);

									//Fraction of diffuse light above the canopy
									if (atmosphericTrasmissionCoeff <= 0.22)
									{
										diffuseLightFraction = 0.99;
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

									diffuseLightAboveCanopy = hourPar * diffuseLightFraction;

									directLightAboveCanopy = hourPar - diffuseLightAboveCanopy;

									//Light which is perpendicular to leaf surface
									leafSurface90degLight = (1 - scatterLightParameter) * directLightAboveCanopy / solarElevation;
									logger(g_log, "\nLight perpendicular to leaf surface: %f (J/m^2/s)", leafSurface90degLight);

									for (l = 0; l < 3; l++)
									{
										//Canopy layer for gaussian integration
										canopyHeightLAI[l] = _LAI * gaussianParameter1[l];
										logger(g_log, "\nLAI above layer %d: %f",l, canopyHeightLAI[l]);

										//Fraction of sunlit area
										sunlitLeafAreaFraction = exp(-blackBodyExtinctionCoeff * canopyHeightLAI[l]);

										//-------------------------------------------------------------------------------------------------------------------------------------
										// Direct light
										directLight = (1 - canopySpharicalReflectivity) * directLightAboveCanopy *  sqrt(1 - scatterLightParameter) * blackBodyExtinctionCoeff *
												exp(-1 * sqrt(1 - scatterLightParameter) * blackBodyExtinctionCoeff * canopyHeightLAI[l]);
										logger(g_log, "\ndirect light accepted by sunlit leaves: %f", directLight);

										// Diffuse light : fixsergio sergio; spitters canopy shpaerical!!!
										diffuseLight = (1 - canopySpharicalReflectivity) * diffuseLightAboveCanopy * diffuseLightExtinctionCoeff *
												exp(- diffuseLightExtinctionCoeff * canopyHeightLAI[l]);
										logger(g_log, "\ndiffuse light accepted by sunlit leaves: %f", diffuseLight);

										//absorption of diffuse light taken from Spitters
										absorbedDiffuseLight = (1 - canopySpharicalReflectivity) * diffuseLightAboveCanopy * diffuseLightExtinctionCoeff *
												exp( - diffuseLightExtinctionCoeff * canopyHeightLAI[l]);

										//absorption of direct light taken from spitters
										absorptionDirectLight = (1 - canopySpharicalReflectivity) * directLightAboveCanopy * sqrt(1 - scatterLightParameter) *
												blackBodyExtinctionCoeff * exp (- 1 * (1 - scatterLightParameter) * blackBodyExtinctionCoeff * canopyHeightLAI[l]);

										//absorption of direct component of direct light
										absorptionDirectDirectLight = (1 - scatterLightParameter) * directLightAboveCanopy * blackBodyExtinctionCoeff *
												exp(-sqrt(1 - scatterLightParameter) * blackBodyExtinctionCoeff * canopyHeightLAI[l]);

										//Light absorbed by shaded leaves in a layer
										shadeLeavesLightAbsor = absorbedDiffuseLight + absorptionDirectLight - absorptionDirectDirectLight;

										//Photosynthesis rate of shaded leaves
										shadLeavesPhotoRate = lightSaturationPhotoRate * (1 - exp(-1 * shadeLeavesLightAbsor * initialLightUseEfficiency * 3600.0 / lightSaturationPhotoRate));
										logger(g_log, "\nPhotosynthesis rate of shaded leaves: %f (g CO2/m^2/h)",shadLeavesPhotoRate);

										//Photosynthesis rate of sunlit leaves
										sunlitLeavesPhotoRate = lightSaturationPhotoRate * (1 - ((lightSaturationPhotoRate - shadLeavesPhotoRate) *
												(1 - exp(-1 * leafSurface90degLight * initialLightUseEfficiency * 3600.0 / lightSaturationPhotoRate))) /
												(leafSurface90degLight * initialLightUseEfficiency *3600.0));
										logger(g_log, "\nPhotosynthesis rate of sunlit leaves: %f (g CO2/m^2/h)", sunlitLeavesPhotoRate);

										//Gross photosynthesis rate at layer Li and time tj
										layerGrossPhotoRate = sunlitLeafAreaFraction * sunlitLeavesPhotoRate + (1 - sunlitLeafAreaFraction) * shadLeavesPhotoRate;
										logger(g_log, "\n\nGross photosynthesis rate at crop layer %d: %f ( gCO2/m^2/h)",l, layerGrossPhotoRate);

										switch (l)
										{
										case 0:
											gaussIntegrPhoto = layerGrossPhotoRate * _LAI;
											logger(g_log, "\n partial denominator in the photosynthesis module: %f",gaussIntegrPhoto);
											break;
										case 1:
											gaussIntegrPhoto += 1.6 * layerGrossPhotoRate * _LAI;
											logger(g_log, "\n partial denominator in the photosynthesis module: %f",gaussIntegrPhoto);
											break;
										case 2:
											gaussIntegrPhoto += layerGrossPhotoRate * _LAI;
											logger(g_log, "\n partial denominator in the photosynthesis module: %f",gaussIntegrPhoto);
											break;
										}
									}
									//hourlyCanopyAssimilation: assumed * _LAI as seen in table 1 gaudriaan
									gaussIntegrPhoto /= 3.6;
									switch (i)
									{
									case 0:
										gaussIntegPhotoSum = daylength * gaussIntegrPhoto;
										break;
									case 1:
										gaussIntegPhotoSum += 1.6 * daylength * gaussIntegrPhoto;
										break;
									case 2:
										gaussIntegPhotoSum += daylength * gaussIntegrPhoto;
										break;
									}
									logger(g_log, "\nformatting gaussIntegrPhoto value: %f\nPartial denominator: %f", gaussIntegrPhoto, gaussIntegPhotoSum);
								}

								//dailyCanopyAssimilation
								gaussIntegPhotoSum /= 3.6;

								// Effects of CO2 concentration on photosynthesis
								fCO2 = 1 + co2EffectPhotosynthesis * log(g_settings->co2Conc/340.0);

								//daily gross photosynthesis (g/m^2) = 0.1×30/44min(waterStressFactor,ns)fCO2 sum(sum(P(Li,tj )LAI DLw2j w2)3, 3)
								dailyGrossPhoto = MIN(waterStressFactor,nitrogenStressFactor) * fCO2 * gaussIntegPhotoSum;
								logger(g_log, "\ndaily gross photosynthesis: %f g/m^2", dailyGrossPhoto);


								/****************************************************************
								 * 						RESPIRATION
								 ***************************************************************/

								logger(g_log, " ********** RESPIRATION ***********\n");

								// maintenance respiration: k stands for the number of comparts
								for (i = 0; i < 4; i++)
								{
									maintenanceRespiration += compartMaintenanceResp[i] * pow(Q10, (meanCanopyTemperature - 25)/10.0) * compartBiomass[i];
									logger(g_log, "\nMaintenanace respiration cumulated for %d compartments: %f g CO2/m^2\n", i, maintenanceRespiration);
								}

								//growth respiration
								growthRespiration = (dailyGrossPhoto - maintenanceRespiration) * (1 - 1 / growthRespEfficiency);
								logger(g_log, "Maintenance total respiration: %f\nGrowth respiration: %f\n", maintenanceRespiration, growthRespiration);

								//fixsergio sergio
								dailyAssimilate = dailyGrossPhoto - growthRespiration - maintenanceRespiration;  //assumed as the difference between GPP and Resp

								/**************************************************
								 *			 ASSIMILATES PARTITIONING
								 ****************************************************/


								//partitioning of the assimilate to the non-root parts of the plant
								plantAssimilatesTopFraction = 0.7 + MIN(soilWaterInfluenceOnAssimilation, nitrogenStressFactor) * 0.1;

								//abovegroundFraction: fraction of assimilates allocated in stems
								potentialStemGrowth = (0.15 + 0.12 * dailyThermalTime /phyllocron) * plantAssimilatesTopFraction * dailyAssimilate;

								//abovegroundFraction: fraction of assimilates allocated in leaves
								potentialLeafGrowth = dailyAssimilate * plantAssimilatesTopFraction - potentialStemGrowth;

								totalCumulativeLeafArea += potentialLeafGrowth * proportionalityConstant;


								/******************************************
								 * 				LEAF SENESCENCE
								 *****************************************/

								if (phylloCounter >= phyllocron)
								{

									leafNumber++;
									//only 4 leaves per stem
									if (leafNumber > 4)
									{
										//fixsergio: is a rate Dtt/Ph gives the tt specific death rate
										leafAreaLossRate = (cumulativeLeafAreaPhyllocron[leafNumber - 4] -
												cumulativeLeafAreaPhyllocron[leafNumber - 5]) * dailyThermalTime / phyllocron;
										senescenceLeafArea += leafAreaLossRate;
										leafNumber = 4;

										//soilWaterStress limiting factor
									}

									phylloCounter -= phyllocron;
									for(l = 0; l < leafNumber - 2 ; l++)
									{
										cumulativeLeafAreaPhyllocron[l] = cumulativeLeafAreaPhyllocron[l+1];
									}
									cumulativeLeafAreaPhyllocron[4] = totalCumulativeLeafArea; //here was cumulative leaf area
									cumPhyllocrons ++;
								}

								logger(g_log, "\nLEAF SENESCENCE"
										"\nleafNumber %f;\nphylloCounter; %f; \ncumPhyllocrons; %d, \n", leafNumber, phylloCounter, cumPhyllocrons);


								//green leaf LAI calculation, m2
								_LAI = (totalCumulativeLeafArea - senescenceLeafArea) * plants * 0.0001;

								totalCumulativeLeafArea = cumulativeLeafAreaPhyllocron[leafNumber -1];


								//tiller death

								// result of an insufficient suupply of assimilate to support tiller growth
								singleTillerPotentialBiomassGain += geneticFactor4 * 0.0089 * dailyThermalTime * sumDailyThermalTime *
										soilWaterInfluenceOnAssimilation / pow(phyllocron, 2);

								//fixsergio to be clarified
								stemWeight += potentialStemGrowth;
								leafWeight += potentialLeafGrowth - leafFallWeight;

								//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
								dailyRatioStemToTotalWeight = stemWeight / (singleTillerPotentialBiomassGain * tillNumber);

								//number of tiller per plant
								tillNumber -= (tillNumber * dailyThermalTime * 0.005 * (1 - dailyRatioStemToTotalWeight));

								if ( fabs(stemWeight - (singleTillerPotentialBiomassGain * tillNumber)) > 0.0001 )
								{
									//fixsergio subtract lost tiller weight to the total stemWeight
								}

								/******************************************************************************************************
								 * 						END OF ABOVEGROUND CROP DEVELOPMENT
								 *****************************************************************************************************/


								if (developmentRate >= 1.0)
								{
									dailyThermalTime -= stageLimit[stage];
									stage += 1;

									developmentRate = dailyThermalTime / stageLimit[stage];		//it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
									// stage rather than surfaceTemperatureAdjustment)
									logger(g_log, "\n\ndailyThermalTime: %f", dailyThermalTime);
									logger(g_log, "\nfirst DR = %f",developmentRate);
								}

								break;

							case 2:
								logger(g_log, "\nstage %d", stage);

								dailyThermalTime += MIN((maxDevelopmentTemperature - basalTemperature), MAX(0, meanCanopyTemperature));
								logger(g_log, "\ndailyThermalTime = %f", dailyThermalTime);

								sumDailyThermalTime += dailyThermalTime;
								logger(g_log, "\ncumulated dailyThermalTime %f", sumDailyThermalTime);

								developmentRate = dailyThermalTime / stageLimit[stage];
								logger(g_log, "\ndevelopmentRate = %f",developmentRate);
								/*******************************************************************************************************
								 * 				ENDO OF LEAF GROWTH TO PRE-ANTHESIS EAR GROWTH
								 *******************************************************************************************************/

								//plant growth continues to the end of preanthesis ear growth; stage duration equals 2 phyllocrons
								//major growing parts are stem, ear and roots; ears and stems

								/*******************************************
								 * 				PHOTOSYNTHESIS
								 ******************************************/



								logger(g_log, "\n\n******** PHOTOSYNTHESIS MODULE *********\n ");


								// Light which is perpendicular to leaf optimumPhotoRate
								lightSaturationPhotoRate = optimumPhotoRate * layerSoilTempEffectPhotoRate;
								logger(g_log, "\nPhotosynthesis rate at light saturation: %f kg CO2/ha/h", lightSaturationPhotoRate);


								logger(g_log, "\n\n******** PHOTOSINTETICALLY ACTIVE RADIATION AT CERTAIN TIME ********* \n ");
								//get photosynthetically active radiation at certain time

								//integral of solar elevation during the day
								solarElevationSumDNDC = 3600.0 * (daylength * (Sfactor + 0.4 * (pow(Sfactor, 2) + 0.5 * pow(Cfactor, 2))) +
										24.0 / Pi * Cfactor * (1 + 0.6 * Sfactor) * sqrt(1 - pow(Sfactor/ Cfactor,2)));
								logger(g_log, "solarElevationSUM %f",solarElevationSumDNDC);

								//the external cycle has to be that computing for 3 different hours

								// Atmospheric transmission coefficient
								atmosphericTrasmissionCoeff = (dailySolarRadiation * 3600.0 * daylength) /
										(solarConstant * solarElevationSumDNDC);
								logger(g_log, "\nAtmospheric transmission coefficient: %f", atmosphericTrasmissionCoeff);

								//setting the three different hours by using a gaussian integration over a period from noon to sunset
								for (i = 0; i < 3; i++)
								{
									// setting the 3 different hours to evaluate mean daily photosynthesis

									// three points for Gaussian integration;
									gaussianIntegrationHour[i] = 12 + 0.5 * daylength * gaussianParameter1[i];
									logger(g_log, "\nAssuming the gaussian integration, setted the hour %f,", gaussianIntegrationHour[i]);

									// canopy hourly temperature during daytime

									//sine of the elevation angle of the sun
									solarElevation = Sfactor + Cfactor * cos(2 * Pi * (gaussianIntegrationHour[i] - 12.0) / 24.0);
									logger(g_log, "\nsine of the elevation angle of the sun: %f", solarElevation);

									//mean par for the certain time;
									//hourPar = 0.55 * dailySolarRadiation * solarElevation * (1 + 0.4 * solarElevation) / solarElevationSumDNDC;
									hourPar = dailySolarRadiation / 2.0;
									logger(g_log, "\n\n\nhourPar = %f",hourPar);

									// Reflectivity of horizontally distribuited canopy
									canopyHorizontalReflectivity = (1 - sqrt(1 - scatterLightParameter)) / (1 + sqrt(1-scatterLightParameter));
									logger(g_log, "\nreflectivity of horizontally distribuited canopy %f", canopyHorizontalReflectivity);

									// Reflectivity of sphaerical distribuited canopy = reflectionCoefficient
									canopySpharicalReflectivity = 2 * canopyHorizontalReflectivity / (1 + 2.0 * solarElevation);
									logger(g_log, "\nreflectivity of sphaerical distribuition canopy %f", canopySpharicalReflectivity);

									//taken from Spitters 96, eq 7
									diffuseLightExtinctionCoeff = 0.8 * sqrt(1 - scatterLightParameter);
									logger(g_log, "\ndiffuseLightExtinctionCoefficient: %f", diffuseLightExtinctionCoeff);

									// Extinction coefficient of assumed black body leaves
									blackBodyExtinctionCoeff = (0.5 * diffuseLightExtinctionCoeff) / (0.8 * solarElevation * sqrt(1.0 - scatterLightParameter));
									logger(g_log, "\nextinction coefficient of leaves, assuming them as black bodies: %f", blackBodyExtinctionCoeff);

									//extinction coefficient of direct light
									directLightExtinctionCoeff = blackBodyExtinctionCoeff * sqrt((1 - scatterLightParameter));
									logger(g_log, "\nextinction coefficient of direct light %f",directLightExtinctionCoeff);

									// midVariable3 && midVariable4: mid coefficients
									midVariable4 = 0.847 - 1.61 * solarElevation * pow(solarElevation, 2);
									midVariable3 = (11.47 - midVariable4) / 1.66;
									logger(g_log, "\nbeing the two mid coefficients midVariable3 and midVariable4, respectively: %f and %f",midVariable3,midVariable4);

									//Fraction of diffuse light above the canopy
									if (atmosphericTrasmissionCoeff <= 0.22)
									{
										diffuseLightFraction = 0.99;
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

									diffuseLightAboveCanopy = hourPar * diffuseLightFraction;

									directLightAboveCanopy = hourPar - diffuseLightAboveCanopy;

									//Light which is perpendicular to leaf surface
									leafSurface90degLight = (1 - scatterLightParameter) * directLightAboveCanopy / solarElevation;
									logger(g_log, "\nLight perpendicular to leaf surface: %f (J/m^2/s)", leafSurface90degLight);

									for (l = 0; l < 3; l++)
									{
										//Canopy layer for gaussian integration
										canopyHeightLAI[l] = _LAI * gaussianParameter1[l];
										logger(g_log, "\nLAI above layer %d: %f",l, canopyHeightLAI[l]);

										//Fraction of sunlit area
										sunlitLeafAreaFraction = exp(-blackBodyExtinctionCoeff * canopyHeightLAI[l]);

										//-------------------------------------------------------------------------------------------------------------------------------------
										// Direct light
										directLight = (1 - canopySpharicalReflectivity) * directLightAboveCanopy *  sqrt(1 - scatterLightParameter) * blackBodyExtinctionCoeff *
												exp(-1 * sqrt(1 - scatterLightParameter) * blackBodyExtinctionCoeff * canopyHeightLAI[l]);
										logger(g_log, "\ndirect light accepted by sunlit leaves: %f", directLight);

										// Diffuse light : fixsergio sergio; spitters canopy shpaerical!!!
										diffuseLight = (1 - canopySpharicalReflectivity) * diffuseLightAboveCanopy * diffuseLightExtinctionCoeff *
												exp(- diffuseLightExtinctionCoeff * canopyHeightLAI[l]);
										logger(g_log, "\ndiffuse light accepted by sunlit leaves: %f", diffuseLight);

										//absorption of diffuse light taken from Spitters
										absorbedDiffuseLight = (1 - canopySpharicalReflectivity) * diffuseLightAboveCanopy * diffuseLightExtinctionCoeff *
												exp( - diffuseLightExtinctionCoeff * canopyHeightLAI[l]);

										//absorption of direct light taken from spitters
										absorptionDirectLight = (1 - canopySpharicalReflectivity) * directLightAboveCanopy * sqrt(1 - scatterLightParameter) *
												blackBodyExtinctionCoeff * exp (- 1 * (1 - scatterLightParameter) * blackBodyExtinctionCoeff * canopyHeightLAI[l]);

										//absorption of direct component of direct light
										absorptionDirectDirectLight = (1 - scatterLightParameter) * directLightAboveCanopy * blackBodyExtinctionCoeff *
												exp(-sqrt(1 - scatterLightParameter) * blackBodyExtinctionCoeff * canopyHeightLAI[l]);

										//Light absorbed by shaded leaves in a layer
										shadeLeavesLightAbsor = absorbedDiffuseLight + absorptionDirectLight - absorptionDirectDirectLight;

										//Photosynthesis rate of shaded leaves
										shadLeavesPhotoRate = lightSaturationPhotoRate * (1 - exp(-1 * shadeLeavesLightAbsor * initialLightUseEfficiency * 3600.0 / lightSaturationPhotoRate));
										logger(g_log, "\nPhotosynthesis rate of shaded leaves: %f (g CO2/m^2/h)",shadLeavesPhotoRate);

										//Photosynthesis rate of sunlit leaves
										sunlitLeavesPhotoRate = lightSaturationPhotoRate * (1 - ((lightSaturationPhotoRate - shadLeavesPhotoRate) *
												(1 - exp(-1 * leafSurface90degLight * initialLightUseEfficiency * 3600.0 / lightSaturationPhotoRate))) /
												(leafSurface90degLight * initialLightUseEfficiency *3600.0));
										logger(g_log, "\nPhotosynthesis rate of sunlit leaves: %f (g CO2/m^2/h)", sunlitLeavesPhotoRate);

										//Gross photosynthesis rate at layer Li and time tj
										layerGrossPhotoRate = sunlitLeafAreaFraction * sunlitLeavesPhotoRate + (1 - sunlitLeafAreaFraction) * shadLeavesPhotoRate;
										logger(g_log, "\n\nGross photosynthesis rate at crop layer %d: %f ( gCO2/m^2/h)",l, layerGrossPhotoRate);

										switch (l)
										{
										case 0:
											gaussIntegrPhoto = layerGrossPhotoRate * _LAI;
											logger(g_log, "\n partial denominator in the photosynthesis module: %f",gaussIntegrPhoto);
											break;
										case 1:
											gaussIntegrPhoto += 1.6 * layerGrossPhotoRate * _LAI;
											logger(g_log, "\n partial denominator in the photosynthesis module: %f",gaussIntegrPhoto);
											break;
										case 2:
											gaussIntegrPhoto += layerGrossPhotoRate * _LAI;
											logger(g_log, "\n partial denominator in the photosynthesis module: %f",gaussIntegrPhoto);
											break;
										}
									}
									//hourlyCanopyAssimilation: assumed * LAI as seen in table 1 gaudriaan
									gaussIntegrPhoto /= 3.6;
									switch (i)
									{
									case 0:
										gaussIntegPhotoSum = daylength * gaussIntegrPhoto;
										break;
									case 1:
										gaussIntegPhotoSum += 1.6 * daylength * gaussIntegrPhoto;
										break;
									case 2:
										gaussIntegPhotoSum += daylength * gaussIntegrPhoto;
										break;
									}
									logger(g_log, "\nformatting gaussIntegrPhoto value: %f\nPartial denominator: %f", gaussIntegrPhoto, gaussIntegPhotoSum);
								}

								//dailyCanopyAssimilation
								gaussIntegPhotoSum /= 3.6;

								// Effects of CO2 concentration on photosynthesis
								fCO2 = 1 + co2EffectPhotosynthesis * log(g_settings->co2Conc/340.0);

								//daily gross photosynthesis (g/m^2) = 0.1×30/44min(waterStressFactor,ns)fCO2 sum(sum(P(Li,tj )LAI DLw2j w2)3, 3)
								dailyGrossPhoto = MIN(waterStressFactor,nitrogenStressFactor) * fCO2 * gaussIntegPhotoSum;
								logger(g_log, "\ndaily gross photosynthesis: %f g/m^2", dailyGrossPhoto);


								/****************************************************************
								 * 						RESPIRATION
								 ***************************************************************/

								logger(g_log, " ********** RESPIRATION ***********\n");

								// maintenance respiration: k stands for the number of comparts
								for (i = 0; i < 4; i++)
								{
									maintenanceRespiration += compartMaintenanceResp[i] * pow(Q10, (meanCanopyTemperature - 25)/10.0) * compartBiomass[i];
									logger(g_log, "\nMaintenanace respiration cumulated for %d compartments: %f g CO2/m^2\n", i, maintenanceRespiration);
								}

								//growth respiration
								growthRespiration = (dailyGrossPhoto - maintenanceRespiration) * (1 - 1 / growthRespEfficiency);
								logger(g_log, "Maintenance total respiration: %f\nGrowth respiration: %f\n", maintenanceRespiration, growthRespiration);

								//fixsergio sergio
								dailyAssimilate = dailyGrossPhoto - growthRespiration - maintenanceRespiration;  //assumed as the difference between GPP and Resp

								/*********************************************************
								 *					 ASSIMILATES PARTITIONING
								 ********************************************************/

								//partitioning of the assimilate to the non-root parts of the plant: for this stage it means it's stem (+ ears) allocation
								plantAssimilatesTopFraction = 0.75 + MIN(soilWaterInfluenceOnAssimilation, nitrogenStressFactor) * 0.1;

								//abovegroundFraction: fraction of assimilates allocated in stems
								potentialStemGrowth =  plantAssimilatesTopFraction * dailyAssimilate;

								//single tiller biomass potential assimilation
								singleTillerPotentialBiomassGain += geneticFactor4 * dailyThermalTime * 0.25 / phyllocron * soilWaterInfluenceOnAssimilation;

								//fixsergio stemWeight: weight of stem (+ ear) compart
								stemWeight += potentialStemGrowth;

								//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
								dailyRatioStemToTotalWeight = stemWeight / (singleTillerPotentialBiomassGain * tillNumber);

								//as stage 2 case, single tiller assimilate demand is balanced against supply; possible reduction in tillers
								tillNumber -= (tillNumber * dailyThermalTime * 0.005 * (1 - dailyRatioStemToTotalWeight));

								//calculated the minimum stem weight: stemWeight when reserves are used during grain filling
								//if the potential growth rate of the plant is such that the assimilate is not sufficient to meet the demand, stem carbo used
								//in this case stem weight may decrease, to the limit of minimumStemWeight


								//fixsergio: attention!! this is not a reliable solution!!!!
								minimumStemWeight = 3;

								//may be an error; check if it is directly considered LAI (and not LAI / plants / 0.0001)
								leafAreaLossRate =  0.0003 * dailyThermalTime * _LAI / plants / 0.0001;

								//senescence leaf area
								senescenceLeafArea += leafAreaLossRate;


								//begining of leaf area reduction phase;
								_LAI = (totalCumulativeLeafArea - senescenceLeafArea) * plants * 0.0001;


								/******************************************************************************************************
								 * 						END OF ABOVEGROUND CROP DEVELOPMENT
								 *****************************************************************************************************/

								if (developmentRate >= 1.0)
								{
									dailyThermalTime -= stageLimit[stage];
									stage += 1;

									developmentRate = dailyThermalTime / stageLimit[stage];		//it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
									// stage rather than surfaceTemperatureAdjustment)
									logger(g_log, "\n\ndailyThermalTime: %f", dailyThermalTime);
									logger(g_log, "\nfirst DR = %f",developmentRate);
								}

								break;

							case 3:
								logger(g_log, "\n**************************************************"
										"\n			FROM PRE-ANTHESIS TO GRAIN FILLING"
										"\n***************************************************");
								// cumulated phyllocrons

								dailyThermalTime += MIN((maxDevelopmentTemperature - basalTemperature), MAX(0, meanCanopyTemperature));
								logger(g_log, "\ndailyThermalTime = %f", dailyThermalTime);

								sumDailyThermalTime += dailyThermalTime;
								logger(g_log, "\ncumulated dailyThermalTime %f", sumDailyThermalTime);

								developmentRate = dailyThermalTime / stageLimit[stage];
								logger(g_log, "\ndevelopmentRate = %f",developmentRate);
								/*********************************************************************************************************
								 * 		FROM PRE-ANTHESIS TO GRAIN FILLING
								 **********************************************************************************************************/


								//no aboveground organs active expansion; carbo stored as a reservoir to be transported to the grains

								/*******************************************
								 * 				PHOTOSYNTHESIS
								 ******************************************/



								logger(g_log, "\n\n******** PHOTOSYNTHESIS MODULE *********\n ");


								// Light which is perpendicular to leaf optimumPhotoRate
								lightSaturationPhotoRate = optimumPhotoRate * layerSoilTempEffectPhotoRate;
								logger(g_log, "\nPhotosynthesis rate at light saturation: %f kg CO2/ha/h", lightSaturationPhotoRate);


								logger(g_log, "\n\n******** PHOTOSINTETICALLY ACTIVE RADIATION AT CERTAIN TIME ********* \n ");
								//get photosynthetically active radiation at certain time

								//integral of solar elevation during the day
								solarElevationSumDNDC = 3600.0 * (daylength * (Sfactor + 0.4 * (pow(Sfactor, 2) + 0.5 * pow(Cfactor, 2))) +
										24.0 / Pi * Cfactor * (1 + 0.6 * Sfactor) * sqrt(1 - pow(Sfactor/ Cfactor,2)));
								logger(g_log, "solarElevationSUM %f",solarElevationSumDNDC);

								//the external cycle has to be that computing for 3 different hours

								// Atmospheric transmission coefficient
								atmosphericTrasmissionCoeff = (dailySolarRadiation * 3600.0 * daylength) /
										(solarConstant * solarElevationSumDNDC);
								logger(g_log, "\nAtmospheric transmission coefficient: %f", atmosphericTrasmissionCoeff);

								//setting the three different hours by using a gaussian integration over a period from noon to sunset
								for (i = 0; i < 3; i++)
								{
									// setting the 3 different hours to evaluate mean daily photosynthesis

									// three points for Gaussian integration;
									gaussianIntegrationHour[i] = 12 + 0.5 * daylength * gaussianParameter1[i];
									logger(g_log, "\nAssuming the gaussian integration, setted the hour %f,", gaussianIntegrationHour[i]);

									// canopy hourly temperature during daytime

									//sine of the elevation angle of the sun
									solarElevation = Sfactor + Cfactor * cos(2 * Pi * (gaussianIntegrationHour[i] - 12.0) / 24.0);
									logger(g_log, "\nsine of the elevation angle of the sun: %f", solarElevation);

									//mean par for the certain time;
									//hourPar = 0.55 * dailySolarRadiation * solarElevation * (1 + 0.4 * solarElevation) / solarElevationSumDNDC;
									hourPar = dailySolarRadiation / 2.0;
									logger(g_log, "\n\n\nhourPar = %f",hourPar);

									// Reflectivity of horizontally distribuited canopy
									canopyHorizontalReflectivity = (1 - sqrt(1 - scatterLightParameter)) / (1 + sqrt(1-scatterLightParameter));
									logger(g_log, "\nreflectivity of horizontally distribuited canopy %f", canopyHorizontalReflectivity);

									// Reflectivity of sphaerical distribuited canopy = reflectionCoefficient
									canopySpharicalReflectivity = 2 * canopyHorizontalReflectivity / (1 + 2.0 * solarElevation);
									logger(g_log, "\nreflectivity of sphaerical distribuition canopy %f", canopySpharicalReflectivity);

									//taken from Spitters 96, eq 7
									diffuseLightExtinctionCoeff = 0.8 * sqrt(1 - scatterLightParameter);
									logger(g_log, "\ndiffuseLightExtinctionCoefficient: %f", diffuseLightExtinctionCoeff);

									// Extinction coefficient of assumed black body leaves
									blackBodyExtinctionCoeff = (0.5 * diffuseLightExtinctionCoeff) / (0.8 * solarElevation * sqrt(1.0 - scatterLightParameter));
									logger(g_log, "\nextinction coefficient of leaves, assuming them as black bodies: %f", blackBodyExtinctionCoeff);

									//extinction coefficient of direct light
									directLightExtinctionCoeff = blackBodyExtinctionCoeff * sqrt((1 - scatterLightParameter));
									logger(g_log, "\nextinction coefficient of direct light %f",directLightExtinctionCoeff);

									// midVariable3 && midVariable4: mid coefficients
									midVariable4 = 0.847 - 1.61 * solarElevation * pow(solarElevation, 2);
									midVariable3 = (11.47 - midVariable4) / 1.66;
									logger(g_log, "\nbeing the two mid coefficients midVariable3 and midVariable4, respectively: %f and %f",midVariable3,midVariable4);

									//Fraction of diffuse light above the canopy
									if (atmosphericTrasmissionCoeff <= 0.22)
									{
										diffuseLightFraction = 0.99;
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

									diffuseLightAboveCanopy = hourPar * diffuseLightFraction;

									directLightAboveCanopy = hourPar - diffuseLightAboveCanopy;

									//Light which is perpendicular to leaf surface
									leafSurface90degLight = (1 - scatterLightParameter) * directLightAboveCanopy / solarElevation;
									logger(g_log, "\nLight perpendicular to leaf surface: %f (J/m^2/s)", leafSurface90degLight);

									for (l = 0; l < 3; l++)
									{
										//Canopy layer for gaussian integration
										canopyHeightLAI[l] = _LAI * gaussianParameter1[l];
										logger(g_log, "\nLAI above layer %d: %f",l, canopyHeightLAI[l]);

										//Fraction of sunlit area
										sunlitLeafAreaFraction = exp(-blackBodyExtinctionCoeff * canopyHeightLAI[l]);

										//-------------------------------------------------------------------------------------------------------------------------------------
										// Direct light
										directLight = (1 - canopySpharicalReflectivity) * directLightAboveCanopy *  sqrt(1 - scatterLightParameter) * blackBodyExtinctionCoeff *
												exp(-1 * sqrt(1 - scatterLightParameter) * blackBodyExtinctionCoeff * canopyHeightLAI[l]);
										logger(g_log, "\ndirect light accepted by sunlit leaves: %f", directLight);

										// Diffuse light : fixsergio sergio; spitters canopy shpaerical!!!
										diffuseLight = (1 - canopySpharicalReflectivity) * diffuseLightAboveCanopy * diffuseLightExtinctionCoeff *
												exp(- diffuseLightExtinctionCoeff * canopyHeightLAI[l]);
										logger(g_log, "\ndiffuse light accepted by sunlit leaves: %f", diffuseLight);

										//absorption of diffuse light taken from Spitters
										absorbedDiffuseLight = (1 - canopySpharicalReflectivity) * diffuseLightAboveCanopy * diffuseLightExtinctionCoeff *
												exp( - diffuseLightExtinctionCoeff * canopyHeightLAI[l]);

										//absorption of direct light taken from spitters
										absorptionDirectLight = (1 - canopySpharicalReflectivity) * directLightAboveCanopy * sqrt(1 - scatterLightParameter) *
												blackBodyExtinctionCoeff * exp (- 1 * (1 - scatterLightParameter) * blackBodyExtinctionCoeff * canopyHeightLAI[l]);

										//absorption of direct component of direct light
										absorptionDirectDirectLight = (1 - scatterLightParameter) * directLightAboveCanopy * blackBodyExtinctionCoeff *
												exp(-sqrt(1 - scatterLightParameter) * blackBodyExtinctionCoeff * canopyHeightLAI[l]);

										//Light absorbed by shaded leaves in a layer
										shadeLeavesLightAbsor = absorbedDiffuseLight + absorptionDirectLight - absorptionDirectDirectLight;

										//Photosynthesis rate of shaded leaves
										shadLeavesPhotoRate = lightSaturationPhotoRate * (1 - exp(-1 * shadeLeavesLightAbsor * initialLightUseEfficiency * 3600.0 / lightSaturationPhotoRate));
										logger(g_log, "\nPhotosynthesis rate of shaded leaves: %f (g CO2/m^2/h)",shadLeavesPhotoRate);

										//Photosynthesis rate of sunlit leaves
										sunlitLeavesPhotoRate = lightSaturationPhotoRate * (1 - ((lightSaturationPhotoRate - shadLeavesPhotoRate) *
												(1 - exp(-1 * leafSurface90degLight * initialLightUseEfficiency * 3600.0 / lightSaturationPhotoRate))) /
												(leafSurface90degLight * initialLightUseEfficiency *3600.0));
										logger(g_log, "\nPhotosynthesis rate of sunlit leaves: %f (g CO2/m^2/h)", sunlitLeavesPhotoRate);

										//Gross photosynthesis rate at layer Li and time tj
										layerGrossPhotoRate = sunlitLeafAreaFraction * sunlitLeavesPhotoRate + (1 - sunlitLeafAreaFraction) * shadLeavesPhotoRate;
										logger(g_log, "\n\nGross photosynthesis rate at crop layer %d: %f ( gCO2/m^2/h)",l, layerGrossPhotoRate);

										switch (l)
										{
										case 0:
											gaussIntegrPhoto = layerGrossPhotoRate * _LAI;
											logger(g_log, "\n partial denominator in the photosynthesis module: %f",gaussIntegrPhoto);
											break;
										case 1:
											gaussIntegrPhoto += 1.6 * layerGrossPhotoRate * _LAI;
											logger(g_log, "\n partial denominator in the photosynthesis module: %f",gaussIntegrPhoto);
											break;
										case 2:
											gaussIntegrPhoto += layerGrossPhotoRate * _LAI;
											logger(g_log, "\n partial denominator in the photosynthesis module: %f",gaussIntegrPhoto);
											break;
										}
									}
									//hourlyCanopyAssimilation: assumed * _LAI as seen in table 1 gaudriaan
									gaussIntegrPhoto /= 3.6;
									switch (i)
									{
									case 0:
										gaussIntegPhotoSum = daylength * gaussIntegrPhoto;
										break;
									case 1:
										gaussIntegPhotoSum += 1.6 * daylength * gaussIntegrPhoto;
										break;
									case 2:
										gaussIntegPhotoSum += daylength * gaussIntegrPhoto;
										break;
									}
									logger(g_log, "\nformatting gaussIntegrPhoto value: %f\nPartial denominator: %f", gaussIntegrPhoto, gaussIntegPhotoSum);
								}

								//dailyCanopyAssimilation
								gaussIntegPhotoSum /= 3.6;

								// Effects of CO2 concentration on photosynthesis
								fCO2 = 1 + co2EffectPhotosynthesis * log(g_settings->co2Conc/340.0);

								//daily gross photosynthesis (g/m^2) = 0.1×30/44min(waterStressFactor,ns)fCO2 sum(sum(P(Li,tj )LAI DLw2j w2)3, 3)
								dailyGrossPhoto = MIN(waterStressFactor,nitrogenStressFactor) * fCO2 * gaussIntegPhotoSum;
								logger(g_log, "\ndaily gross photosynthesis: %f g/m^2", dailyGrossPhoto);


								/****************************************************************
								 * 						RESPIRATION
								 ***************************************************************/

								logger(g_log, " ********** RESPIRATION ***********\n");

								// maintenance respiration: k stands for the number of comparts
								for (i = 0; i < 4; i++)
								{
									maintenanceRespiration += compartMaintenanceResp[i] * pow(Q10, (meanCanopyTemperature - 25)/10.0) * compartBiomass[i];
									logger(g_log, "\nMaintenanace respiration cumulated for %d compartments: %f g CO2/m^2\n", i, maintenanceRespiration);
								}

								//growth respiration
								growthRespiration = (dailyGrossPhoto - maintenanceRespiration) * (1 - 1 / growthRespEfficiency);
								logger(g_log, "Maintenance total respiration: %f\nGrowth respiration: %f\n", maintenanceRespiration, growthRespiration);

								//fixsergio sergio
								dailyAssimilate = dailyGrossPhoto - growthRespiration - maintenanceRespiration;  //assumed as the difference between GPP and Resp

								/***************************************************
								 *					 ASSIMILATES PARTITIONING
								 ********************************************************/

								//partitioning of the assimilate to the non-root parts of the plant: for this stage it means it's stem (+ ears) allocation
								plantAssimilatesTopFraction = 0.8 + MIN(soilWaterInfluenceOnAssimilation, nitrogenStressFactor) * 0.1;

								//stemWeight
								stemWeight += dailyAssimilate * plantAssimilatesTopFraction;

								leafAreaLossRate = 0.0006 * dailyThermalTime * _LAI / plants / 0.0001;;

								//senescence leaf area
								senescenceLeafArea += leafAreaLossRate;

								//begining of leaf area reduction phase;
								_LAI = (totalCumulativeLeafArea - senescenceLeafArea) * plants * 0.0001;

								//fixsergio: attention!! this is not a reliable solution!!!!
								minimumStemWeight = 3;

								//fixsergio water stress may enforce leaffall process;

								//find out a way to define leaf weight reduction with ALI decrementation
								leafWeight += potentialLeafGrowth - leafFallWeight;

								//number of kernels (chicchi) per plant are assumed to be:
								kernelsPerPlant = (int)(stemWeight * geneticFactor2);

								if (developmentRate >= 1.0)
								{
									dailyThermalTime -= stageLimit[stage];
									stage += 1;

									developmentRate = dailyThermalTime / stageLimit[stage];		//it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
									// stage rather than surfaceTemperatureAdjustment)
									logger(g_log, "\n\ndailyThermalTime: %f", dailyThermalTime);
									logger(g_log, "\nfirst DR = %f",developmentRate);
								}

								break;

							case 4:
								logger(g_log, "\nstage %d", stage);

								logger(g_log, "\n*************************************"
										"\n		grain filling"
										"\n************************************");

								dailyThermalTime += MIN((maxDevelopmentTemperature - basalTemperature), MAX(0, meanCanopyTemperature));
								logger(g_log, "\ndailyThermalTime = %f", dailyThermalTime);

								sumDailyThermalTime += dailyThermalTime;
								logger(g_log, "\ncumulated dailyThermalTime %f", sumDailyThermalTime);

								developmentRate = dailyThermalTime / stageLimit[stage];
								logger(g_log, "\ndevelopmentRate = %f",developmentRate);
								//in this stage there are some difficulties;
								//	- enstablish rate of photosynthesis
								//	- some of the assimilates are used for grain filling
								//	- decrease of earlier growth rate; increase in maintenance respiration


								/*
								 *  	ASSIMILATE PARTITIONING
								 */

								//plantTopFraction of assimilates
								plantAssimilatesTopFraction = minimumStemWeight / stemWeight * 0.35 + 0.65;

								//approximated reduction factor of gross photosynthesis due to aging and sink assimilates demand
								dailyGrossPhoto = dailyGrossPhoto * (1 - (1.2 - 0.8 * minimumStemWeight / stemWeight) *
										(sumDailyThermalTime + 100) / (stageLimit[4] + 100));

								//seneescence assumed as a non linear equation
								leafAreaLossRate = _LAI / plants / 0.0001 - senescenceLeafArea * 2 * sumDailyThermalTime / pow(stageLimit[4], 2);

								//rate of grainFilling
								if ( met[month].d[day].tavg < 10 && met[month].d[day].tavg > 0)
								{
									grainFillingRate = 0.065 * met[month].d[day].tavg;
								}
								else if (met[month].d[day].tavg >= 10 )
								{
									grainFillingRate = 0.065 + ( 0.0787 - 0.00328 * (met[month].d[day].tmax - met[month].d[day].tmin)) * pow(met[month].d[day].tavg, 0.8);
								}
								else
								{
									grainFillingRate = 0.0;
								}
								grainFillingRate = MIN(1 , grainFillingRate);

								//whole plant potential grainGrowth
								plantPotentialGrainGrowth = grainFillingRate * kernelsPerPlant * geneticFactor3 * 0.001;

								//actualGrainGrowth
								actualGrainGrowth = dailyGrossPhoto * plantAssimilatesTopFraction - plantPotentialGrainGrowth;

								//stemWeight: may be reduced if actualGrainGrowth is greater than grosshoto * topFraction: it implies the use of reserves
								stemWeight += actualGrainGrowth;

								if(stemWeight < minimumStemWeight)
								{
									//kernelGrowth = dailyTopBiomasspartitioning
								}
								else
								{
									//kernelGrowth = potentialRate
								}

								grainWeight += plantPotentialGrainGrowth;

								dryWeightYeld = grainWeight * plants;

								if (developmentRate >= 1.0)
								{

									dailyThermalTime -= stageLimit[stage];
									stage += 1;

									developmentRate = dailyThermalTime / stageLimit[stage];		//it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
									// stage rather than surfaceTemperatureAdjustment)
									logger(g_log, "\n\ndailyThermalTime: %f", dailyThermalTime);
									logger(g_log, "\nfirst DR = %f",developmentRate);
								}

								break;

							case 5:
								logger(g_log, "\nstage %d", stage);

								dailyThermalTime += MIN((maxDevelopmentTemperature - basalTemperature), MAX(0, meanCanopyTemperature));
								logger(g_log, "\ndailyThermalTime = %f", dailyThermalTime);

								sumDailyThermalTime += dailyThermalTime;
								logger(g_log, "\ncumulated dailyThermalTime %f", sumDailyThermalTime);

								developmentRate = dailyThermalTime / stageLimit[stage];
								logger(g_log, "\ndevelopmentRate = %f",developmentRate);
								//fraction of assimilates pertitioned to shoot
								//					abovegroundFraction = 0.75 + 0.1 * MIN(waterStressFactor,ns);
								//waterStressFactor and ns are water and nitrogen stress factors
								/*
								//Leaf fraction of assimilare
								aboveBiomassFract = 0;

								//fraction of assimilates partitioned to stem
								stemBiomassFract = abovegroundFraction;

								//fraction of assimilates partitioned to grain
								grainBiomassFract = 0;
								 */

								if (developmentRate >= 1.0)
								{
									dailyThermalTime -= stageLimit[stage];
									stage += 1;

									developmentRate = dailyThermalTime / stageLimit[stage];		//it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
									// stage rather than surfaceTemperatureAdjustment)
									logger(g_log, "\n\ndailyThermalTime: %f", dailyThermalTime);
									logger(g_log, "\nfirst DR = %f",developmentRate);
								}

								break;

							case 6:

								dailyThermalTime += MIN((maxDevelopmentTemperature - basalTemperature), MAX(0, meanCanopyTemperature));
								logger(g_log, "\ndailyThermalTime = %f", dailyThermalTime);

								sumDailyThermalTime += dailyThermalTime;
								logger(g_log, "\ncumulated dailyThermalTime %f", sumDailyThermalTime);

								developmentRate = dailyThermalTime / stageLimit[stage];
								logger(g_log, "\ndevelopmentRate = %f",developmentRate);
								logger(g_log, "\nstage %d", stage);
								//fraction of assimilates pertitioned to shoot
								//				abovegroundFraction = 0.8 + 0.1 * Minimum(waterStressFactor,ns);
								/*
								//Leaf fraction of assimilare
								aboveBiomassFract = 0;

								//fraction of assimilates partitioned to stem
								stemBiomassFract = abovegroundFraction;

								//fraction of assimilates partitioned to grain
								grainBiomassFract = 0;
								 */

								if (developmentRate >= 1.0)
								{
									dailyThermalTime -= stageLimit[stage];
									stage += 1;

									developmentRate = dailyThermalTime / stageLimit[stage];		//it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
									// stage rather than surfaceTemperatureAdjustment)
									logger(g_log, "\n\ndailyThermalTime: %f", dailyThermalTime);
									logger(g_log, "\nfirst DR = %f",developmentRate);
								}

								break;

							case 7:

								dailyThermalTime += MIN((maxDevelopmentTemperature - basalTemperature), MAX(0, meanCanopyTemperature));
								logger(g_log, "\ndailyThermalTime = %f", dailyThermalTime);

								sumDailyThermalTime += dailyThermalTime;
								logger(g_log, "\ncumulated dailyThermalTime %f", sumDailyThermalTime);

								developmentRate = dailyThermalTime / stageLimit[stage];
								logger(g_log, "\ndevelopmentRate = %f",developmentRate);

								//fraction of assimilates pertitioned to shoot
								//					abovegroundFraction = 0.65 + 0.3 * stemBiomassFract / BMstem;
								// BMstem = current stem biomass; BMstem0 = stem biomass at flowering;

								//partition development according to the requirement of grain growth
								if (developmentRate >= 1.0)
								{
									stage += 1;
									dailyThermalTime = dailyThermalTime - (developmentRate * 40.0);
									// DR = dailyThermalTime / stageLimit[stage];		it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
									// stage rather than DT)

									if (developmentRate >= 1.0)
									{
										dailyThermalTime -= stageLimit[stage];
										stage += 1;

										developmentRate = dailyThermalTime / stageLimit[stage];		//it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
										// stage rather than surfaceTemperatureAdjustment)
										logger(g_log, "\n\ndailyThermalTime: %f", dailyThermalTime);
										logger(g_log, "\nfirst DR = %f",developmentRate);
									}

								}
								break;

							case 8:
								logger(g_log, "\nstage %d", stage);

								dailyThermalTime += MIN((maxDevelopmentTemperature - basalTemperature), MAX(0, meanCanopyTemperature));
								logger(g_log, "\ndailyThermalTime = %f", dailyThermalTime);

								sumDailyThermalTime += dailyThermalTime;
								logger(g_log, "\ncumulated dailyThermalTime %f", sumDailyThermalTime);

								developmentRate = dailyThermalTime / stageLimit[stage];
								logger(g_log, "\ndevelopmentRate = %f",developmentRate);

								if (developmentRate >= 1.0)
								{
									stage  = 0;
									dailyThermalTime = dailyThermalTime - (developmentRate * 40.0);
									// DR = dailyThermalTime / stageLimit[stage];		it can be probably deleted: it'll be set the very next cycle (we're interested in fact in
									// stage rather than DT)
								}
								break;

							}
							logger(g_log, "\n\ndailyThermalTime: %f", dailyThermalTime);
							logger(g_log, "\nstage of development: %d\n\n\n", stage);
							logger(g_log, "\n\n total termal denominator: %d",stageLimit[stage]);
							logger(g_log, "\nDR = %f",developmentRate);
						}
					}

					//*************************************************************************************************************************************************************************************

					/*


					//rooting
					logger(g_log, "******* ROOT LENGTH MODULE *********\n\n");

					// average specific root length
					for (l = 0; l < l_n; l++)
					{
						Resp += layerRootLengthDensity[l] * H[l] / BM_root;
					}

					//soil nitrogen limiting factor
					for (l = 0; l < soilLayer; l++)
					{
						Rtn[l] = 1 - 1.17 * exp (- 0.15 *(NO3_p[l] + NH4_p[l]));
						//soil temperature limiting factor; whole soil profile temperature!?!?!?
						Rtt[l] = cos(Pi * (Ts - 20)/40);

						// soil aeration limiting factor (layer specific
						Rta[l] = CPwet + (1 - CPwet) * (UL[l] - SW[l])/(UL[l] - fieldCapacityLayerMoisture[l]);

						// soil strength limiting factor (layer specific)
						Rts[l] = ((1.6 + 0.4 * g_soil_settings->values[SOIL_sand_perc - soilBulkDensity[l])/(0.5 - 0.1 * g_soil_settings->values[SOIL_sand_perc)) *
								sin(1.25 *((SW[l] - LL[l])* Pi)/((fieldCapacityLayerMoisture[l] - LL[l]) * 2));
						logger(g_log, "\nSoil nitrogen limiting factors (layer %d):\n\tsoil temperature limiting factor: "
								"%f\n\tsoil areation limiting factor: %f\n\tsoil strength limiting factor: %f"
								"\n\tSoil nitrogen limiting factor: %f", l, Rtt[l],Rta[l],Rts[l],Rtn[l]);
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
						logger(g_log, "\nSetting minimum limiting factor as: %f", Min_RT);
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
						Log ("daily increase of root length density in layer l: %f cm/cm^3 soil\n", dlayerRootLengthDensity[l]);

						//	dailyThermalTime = 1/DAY * Minimum(TD_m - TD_b, Maximum (0, Tc[hr] - TD_b));



						//Daily increase of root front depth
						dDroot = 0.2 * dailyThermalTime * Minimum (Rts[l], Rta[l]);		//pay attention to this problem:
						//DRoot shouldn't be single layer since
						//it is assumed to be function of layer specific parameters
					}

					logger(g_log, "******** CROP NITROGEN MODULE ********\n\n");

					//movable nitrogen in shoot and root
					Npool = BM_root * (Nr - Nr_min) + (BM_leaf + BM_stem) * (Ns - Ns_min);

					//Nitrogen stress factor
					nitrogenStressFactor = 1 - (Ns - Ns0) / (Ns0 - Ns_min);

					//Uptake capacity:	Check if those 4 parameters are not used by other processes; if so, change from array to double form
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

					logger(g_log, "\n******* DECOMPOSITION AND METHANE EMISSIONS ********\n\n");

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
					// on the paper it was Ch[l] and not Eh[l]; i suspect it is Eh associated indeed; anyway, how can i change Eh?!?!

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
						C_CH4 += Cs[l] + F_RS[l] * F_root[l] * BM_root *4;
					}

					//daily methane production
					CH4_p = 0.47 * C_CH4 * fTM * fEh * fpH_M;

					//Aerenchyma factor
					Aere = BM_root / 1000; 								// Rice;

					//daily methane oxidation
					CH4_o = CH4_p * (0.5 + 0.5 * Aere);

					// quantity of each Ci

					for (l = 0; l < soilLayer; l++)
					{
						//effect of clay adsorption
						mu_clay = log10(0.14 / clay[l]) + 1;

						//the effect of moisture
						mu_w = -1.7827 * pow(Wfps[l], 2) +2.3824 * Wfps[l] - 0.222;

						// the effect of temperature
						mu_T = 0.2161 + 0.093 * Ts - 0.0014 * pow (Ts, 2);

						//decomposition rate of i-th carbon pool  ricontrolla bene
						for (i = 0; i < 3; i++)
						{
							for (j = 0; j < 2; j++ )
							{
								//decomposition rate of a carbon pool
								dC[l][i][j] = mu_clay * mu_cn * mu_T * mu_w * mu_til * K_c[l][i][j] * Ci[l][i][j];
							}
						}
					}


					logger(g_log, " ****** SOIL NITROGEN MODULE ******\n\n");

					logger(g_log, "Nitrification rate\n");

					//Effect of pH on nitrification
					fpH_N = - 0.0604 * pow(pH, 2) + 0.7347 * pH - 1.2314;

					//Effects of soil moisture on nitrification
					Fw_n = -12.904 * pow(Wfps, 4) + 15.651 *pow(Wfps, 3) - 5.5368 * pow(Wfps, 2) + 0.9975 * Wfps - 0.0243;

					//Effects of temperature on nitrification
					fTN = -0.0272 * pow((0.1 * Ts), 4) + 0.1566 * pow(0.1 *Ts, 3) - 0.2234 * pow(0.1 *Ts, 2) + 0.03094 * Ts;

					//Nitrification rate NH4+ converted to NO3-
					NO3_N = NH4 * (1 - exp(-K35 * fTN)) * Fw_n * fpH_N;

					//Equilibrium of ammonium and ammonia (kNH4_NH3 = log10(K_nh4) - log10(K_h2o))  NH3m missing
					kNH4_NH3 = log10(NH4m / NH3m) + pH;

					//Ammonium adsorption
					F_NH4 = (0.41 - 0.47 *log10(NH4) * clay / clay_max);

					//diffusion coefficient
					Ds = 0.6 + 2.93 * pow(v, 1.11);				// v variable missing yet

					//solute movement
					Js = - SW[l] * Ds * Gs + Jw * Sc;			// here many variables missing yet (Gs, Jw, Sc)

					logger(g_log, " denitrification\n\n");


					//pH reduction factors: N20  sergio: pay attention; it's not clear if it is NO2 or N2O pH reduction factor
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

					logger(g_log, "N2 Emissions: %f", F_n2);

					//N2O and NO emissions
					Fn2o_no = 0.017 + (0.025 - 0.0013 * fclay[l]) * (1 - Wfps[l]) * pow(2, Ts/20);

					//Nitrate consumption
					dNO2_dt = (U_no2 / Y_NO2 + M_NO2 * NO2 / N) * B *mu_pH_NO2 * mu_T_DN;
					//nitrous oxide consumption
					dN2O_dt = (U_n2o / Y_N2O + M_N2O * N2O / N) * B *mu_pH_N2O * mu_T_DN;
					//Nitrite consumption
					dNO3_dt = (U_no3 / Y_NO3 + M_NO3 * NO3 / N) * B *mu_pH_NO3 * mu_T_DN;
					 */

					// variazione tempo termico
					//if ( Tc - Tb <0){thermal_time += Tc(or Tair) / 2 - T_basal) * d_ndays

					/*		logger(g_log, "\nDaily thermal time (DNDC method): %f °C\n", dailyThermalTime);
													//	logger(g_log, "\n\n total termal time to finish the stage: %f",stageLimit[stage]);
													logger(g_log, "\n\npercent to reach the following stage: %f",  DR);
													logger(g_log, "\nstage of development: %d\n\n\n", stage);
					 */

				}
				if (day == 31 && month == DECEMBER)
				{
					logger(g_log, "*****END OF YEAR******\n");
				}
				logger(g_log, "\n/*/*/*/*/*/*/*/*/*/*/*/*/*/\n");

				logger(g_log, "****************END OF species_t CLASS***************\n");
			}

			logger(g_log, "****************END OF AGE CLASS***************\n");
		}
		logger(g_log, "****************END OF HEIGHT CLASS***************\n");
	}
	logger(g_log, "\n****************END OF cell_t***************\n");

	/* ok */
	return 1;
}


