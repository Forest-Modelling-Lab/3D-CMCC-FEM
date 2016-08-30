#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "mpfit.h"
#include "common.h"
#include "leaf_fall.h"
#include "constants.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_log;
extern logger_t* g_soil_log;

void leaf_fall_deciduous(cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	static double foliage_to_remove;
	static double fine_root_to_remove;
	static double fraction_to_retransl = 0.1; /* fraction of C to retranslocate (see Bossel et al., 2006 and Campioli et al., 2013 */
	static int senescenceDayOne;
	double previousLai, currentLai;
	double previousBiomass_lai, newBiomass_lai;

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_log, "\n**LEAF FALL DECIDUOUS **\n");

	logger(g_log, "Leaf fall counter = %d\n", s->counter[LEAF_FALL_COUNTER]);
	logger(g_log, "Leaf biomass = %f\n", s->value[LEAF_C]);
	logger(g_log, "Fine root biomass  = %f\n", s->value[FINE_ROOT_C]);

	if(s->counter[LEAF_FALL_COUNTER] == 1)
	{
		logger(g_log, "First day of Leaf fall\n");
		logger(g_log, "DAYS FOR FOLIAGE and FINE ROOT for_REMOVING = %d\n", s->counter[DAY_FRAC_FOLIAGE_REMOVE]);
		/* assuming that fine roots for deciduous species progressively die together with leaves */

		/* note: due to reduction during vegetative period for reduction in canopy cover MAX_LAI != PEAK_LAI */
		/* assign LAI values at the beginning of the sigmoid shape */
		s->value[MAX_LAI] = s->value[LAI];
		senescenceDayOne = c->doy;
	}

	if(s->counter[LEAF_FALL_COUNTER] < s->counter[DAY_FRAC_FOLIAGE_REMOVE])
	{
		/* following Campioli et al., 2013 and Bossel 1996 10% of foliage and fine root biomass is daily retranslocated as reserve in the reserve pool */
		/* compute amount of fine root biomass to retranslocate as reserve */
		s->value[C_LEAF_TO_RESERVE] = (s->value[LEAF_C] * fraction_to_retransl) / s->counter[DAY_FRAC_FOLIAGE_REMOVE];
		logger(g_log, "RETRANSL_C_LEAF_TO_RESERVE = %f\n", s->value[C_LEAF_TO_RESERVE]);
		s->value[C_FINEROOT_TO_RESERVE]= (s->value[FINE_ROOT_C] * fraction_to_retransl) /s->counter[DAY_FRAC_FOLIAGE_REMOVE];
		logger(g_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %f\n", s->value[C_FINEROOT_TO_RESERVE]);

		previousLai = s->value[LAI];

		/* sigmoid shape drives LAI reduction during leaf fall */
		currentLai = MAX(0,s->value[MAX_LAI] / (1 + exp(-(s->counter[DAY_FRAC_FOLIAGE_REMOVE]/2.0 + senescenceDayOne -
				c->doy)/(s->counter[DAY_FRAC_FOLIAGE_REMOVE] / (log(9.0 * s->counter[DAY_FRAC_FOLIAGE_REMOVE]/2.0 + senescenceDayOne) -
						log(.11111111111))))));
		logger(g_log, "previousLai = %f\n", previousLai);
		logger(g_log, "currentLai = %f\n", currentLai);

		/* check */
		CHECK_CONDITION(previousLai, < currentLai);

		previousBiomass_lai = previousLai * (s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1000.0);

		newBiomass_lai = (currentLai * (s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell) / (s->value[SLA_AVG] * 1000.0));
		foliage_to_remove = previousBiomass_lai - newBiomass_lai;
		logger(g_log, "foliage_to_remove = %f\n", foliage_to_remove);

		/* a simple linear correlation from leaf carbon to remove and fine root to remove */
		fine_root_to_remove = (s->value[FINE_ROOT_C]*foliage_to_remove)/s->value[LEAF_C];
		logger(g_log, "fineroot_to_remove = %f\n", fine_root_to_remove);

		s->value[C_TO_LEAF] = -foliage_to_remove ;
		logger(g_log, "C_TO_LEAF = %f\n", s->value[C_TO_LEAF]);
		s->value[C_TO_FINEROOT] = -fine_root_to_remove;
		logger(g_log, "C_TO_FINEROOT = %f\n", s->value[C_TO_FINEROOT]);
		s->value[C_TO_LITTER] = (foliage_to_remove - s->value[C_LEAF_TO_RESERVE]) + (fine_root_to_remove - s->value[C_FINEROOT_TO_RESERVE]);
		logger(g_log, "C_TO_LITTER = %f\n", s->value[C_TO_LITTER]);
	}
	else
	{
		logger(g_log, "Last day of leaffall\n");
		s->value[C_TO_LEAF] = -s->value[LEAF_C];
		logger(g_log, "C_TO_LEAF = %f\n", s->value[C_TO_LEAF]);
		s->value[C_TO_FINEROOT] = - s->value[FINE_ROOT_C];
		logger(g_log, "C_TO_FINEROOT = %f\n", -s->value[C_TO_FINEROOT]);
		s->value[C_LEAF_TO_RESERVE] = s->value[LEAF_C];
		logger(g_log, "RETRANSL_C_LEAF_TO_RESERVE = %f\n", s->value[C_LEAF_TO_RESERVE]);
		s->value[C_FINEROOT_TO_RESERVE] = s->value[FINE_ROOT_C];
		logger(g_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %f\n", s->value[C_FINEROOT_TO_RESERVE]);
		s->value[C_TO_LITTER] = 0.0;
		logger(g_log, "C_TO_LITTER = %f\n", s->value[C_TO_LITTER]);
	}
}

void leaf_fall_evergreen (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	static double daily_leaf_fineroot_turnover_rate;
	static double fraction_to_retransl = 0.1; /* fraction of C to retranslocate (see Bossel et al., 2006 and Campioli et al., 2013 */

	species_t *s;
	s = &c->heights[height].dbhs[dbh].ages[age].species[species];

	logger(g_log, "\n**LEAF FALL EVERGREEN**\n");

	/* compute rates */
	/* compute leaf and fine root turnover rate (ratio) */
	daily_leaf_fineroot_turnover_rate = s->value[LEAF_FINEROOT_TURNOVER]/(int)s->counter[DAY_VEG_FOR_LEAF_FALL];
	//logger(g_log, "Daily leaf fine root turnover rate = %g (ratio)\n", daily_leaf_fineroot_turnover_rate)

	logger(g_log, "Leaf pool before turnover = %g tC/cell\n", s->value[LEAF_C]);
	logger(g_log, "Fine root pool before turnover = %g tC/cell\n", s->value[FINE_ROOT_C]);

	/* compute daily amount of leaf and fine root to remove */
	s->value[C_LEAF_TO_LITTER] = (s->value[LEAF_C] * daily_leaf_fineroot_turnover_rate);
	logger(g_log, "Daily leaf turnover = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER]);

	s->value[C_FINE_ROOT_TO_LITTER] = (s->value[FINE_ROOT_C] * daily_leaf_fineroot_turnover_rate);
	logger(g_log, "Daily fine root turnover = %g tC/cell/day\n", s->value[C_FINE_ROOT_TO_LITTER]);

	logger(g_log, "Daily biomass turnover to litter before retranslocation = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER] + s->value[C_FINE_ROOT_TO_LITTER]);

	/* compute daily amount of C to re-translocate before remove leaf and fine root */
	s->value[C_LEAF_TO_RESERVE] = s->value[C_LEAF_TO_LITTER] * fraction_to_retransl;
	logger(g_log, "RETRANSL_C_LEAF_TO_RESERVE = %g tC/cell/day\n", s->value[C_LEAF_TO_RESERVE]);
	s->value[C_FINEROOT_TO_RESERVE] = s->value[C_FINE_ROOT_TO_LITTER] * fraction_to_retransl;
	logger(g_log, "RETRANSL_C_FINEROOT_TO_RESERVE = %g tC/cell/day\n", s->value[C_FINEROOT_TO_RESERVE]);

	/* update considering retranslocation */
	s->value[C_LEAF_TO_LITTER] -= s->value[C_LEAF_TO_RESERVE];
	logger(g_log, "Daily leaf turnover after retranslocation = %g tC/cell/day\n", s->value[C_LEAF_TO_LITTER]);

	s->value[C_FINE_ROOT_TO_LITTER] -= s->value[C_FINEROOT_TO_RESERVE];
	logger(g_log, "Daily fine root turnover after retranslocation = %g tC/cell/day\n", s->value[C_FINE_ROOT_TO_LITTER]);

	/**/
	s->value[C_TO_LEAF] -= s->value[C_LEAF_TO_LITTER];
	s->value[C_TO_FINEROOT] -= s->value[C_FINE_ROOT_TO_LITTER];

	/* considering that both leaf and fine root contribute to the litter pool */
	s->value[C_TO_LITTER] = (s->value[C_LEAF_TO_LITTER] + s->value[C_FINE_ROOT_TO_LITTER]);
	logger(g_log, "biomass to litter after retranslocation = %g tC/cell/day\n", s->value[C_TO_LITTER]);

}

/* NOT USED */
//void leaffall(species_t *const s, const meteo_t *const met, int* doy, int* toplayer, int z)
//{
//	/* Test harness routine, which contains test data, invokes mpfit() */
//	/* X - independent variable */
//	double previousLai, previousBiomass_lai;	//lai of the day before, used to calculate previous biomass and evaluate delta_foliage biomass
//	//double previous_Biomass_fineroot;
//	//s->counter[DAY_FRAC_FOLIAGE_REMOVE] = 130;
//
//	logger(g_log, "\n**LEAFFALL_MARCONI FUNCTION**\n");
//
//	if(*doy == s->counter[SENESCENCE_DAYONE])
//	{
//		logger(g_log, "Senescence day one\n");
//		s->value[MAX_LAI] = s->value[LAI];
//	}
//	previousLai = s->value[LAI];
//	//previous_Biomass_fineroot = s->value[BIOMASS_FINE_ROOT_tDM];
//
//
//
//	s->value[LAI] = MAX(0,s->value[MAX_LAI] / (1 + exp(-(s->counter[DAY_FRAC_FOLIAGE_REMOVE]/2.0 + s->counter[SENESCENCE_DAYONE] -
//			*doy)/(s->counter[DAY_FRAC_FOLIAGE_REMOVE] / (log(9.0 * s->counter[DAY_FRAC_FOLIAGE_REMOVE]/2.0 + s->counter[SENESCENCE_DAYONE]) -
//					log(.11111111111))))));
//	logger(g_log, "LAI = %f\n", s->value[LAI]);
//	previousBiomass_lai = previousLai * (s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell) / (s->value[SLA_AVG] * GC_GDM * 1000.0);
//	//s->value[BIOMASS_FOLIAGE_tDM] = (s->value[LAI] * (s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell) / (s->value[SLA_AVG] * GC_GDM * 1000.0));
//	s->value[LEAF_C] = (s->value[LAI] * (s->value[CANOPY_COVER_DBHDC] * g_settings->sizeCell) / (s->value[SLA_AVG] * GC_GDM * 1000.0));
//	s->value[C_TO_LEAF]  = -fabs(previousBiomass_lai - s->value[LEAF_C]);
//
//	/* a simple correlation from leaf carbon to remove and fine root to remove */
//	s->value[C_TO_FINEROOT] = (s->value[FINE_ROOT_C]*s->value[C_TO_LEAF])/s->value[LEAF_C];
//
//
//	CHECK_CONDITION(s->value[LEAF_C], < 0.0);
//	CHECK_CONDITION(s->value[FINE_ROOT_C], < 0.0);
//
//
//	//	//fixme
//	//	if(s->value[BIOMASS_FOLIAGE_tDM] > 0.0 || s->value[BIOMASS_FINE_ROOT_tDM] > 0.0)
//	//	{
//	//		logger(g_log, "Biomass foliage = %f\n", s->value[BIOMASS_FINE_ROOT_tDM]);
//	//		logger(g_log, "Biomass fine root = %f\n", s->value[BIOMASS_FOLIAGE_tDM]);
//	//		s->value[DEL_FOLIAGE]  = -fabs(previousBiomass_lai - s->value[BIOMASS_FOLIAGE_tDM]);
//	//		logger(g_log, "DEL_FOLIAGE = %f\n", s->value[DEL_FOLIAGE]);
//	//		s->value[DEL_ROOTS_FINE]  = -fabs(previous_Biomass_fineroot - s->value[BIOMASS_FINE_ROOT_tDM]);
//	//		logger(g_log, "DEL_ROOTS_FINE_CTEM = %f\n", s->value[DEL_ROOTS_FINE]);
//	//
//	//	}
//	//	else
//	//	{
//	//		logger(g_log, "Biomass foliage = %f\n", s->value[BIOMASS_FOLIAGE_tDM]);
//	//		s->value[DEL_FOLIAGE]  = 0.0;
//	//		logger(g_log, "DEL_FOLIAGE = %f\n", s->value[DEL_FOLIAGE]);
//	//		logger(g_log, "Biomass fine root = %f\n", s->value[BIOMASS_FINE_ROOT_tDM]);
//	//		s->value[DEL_FOLIAGE]  = 0.0;
//	//		logger(g_log, "DEL_FOLIAGE = %f\n", s->value[DEL_ROOTS_FINE]);
//	//	}
//	//	logger(g_log, "****************************\n\n");
//}

struct vars_struct {
	double *x;
	double *y;
	double *ey;
};

int endOfYellowing(const meteo_t *const met, species_t *const s)
{
	int endOfYellowing;
	int month, day;
	int actualMonth = 0, actualDay = 0;
	double monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	double dailyColouring, senescence = 0;
	double  senTexp = 1.0;
	double senPexp = 2.0;
	double  senTmax = 28.5;
	double  photoCrit = 800.0;
	double photoStart = s->value[MINDAYLENGTH];
	endOfYellowing = s->counter[SENESCENCE_DAYONE];
	//start from the first day of senescence
	for(day = 0; day <= endOfYellowing; day++)
	{
		actualDay ++;
		if(actualDay == monthDays[actualMonth])
		{
			actualDay = 0;
			actualMonth ++;
		}
	}


	for ( month = actualMonth; month < 12; month++)
	{
		for (day = actualDay; day < monthDays[month]; day++)
		{
			if(met[month].d[day].daylength <= photoStart)
			{
				if(met[month].d[day].tavg <= senTmax)
				{
					dailyColouring =  pow(senTmax - met[month].d[day].tavg, senTexp) *
							pow(met[month].d[day].daylength / photoStart, senPexp);
				}
				else
				{
					dailyColouring = 0;
				}
				senescence += dailyColouring;
			}
			endOfYellowing ++;
			if(senescence >= photoCrit)
			{
				return endOfYellowing;
			}

		}
	}
	logger(g_soil_log, "Attention!! senescenceNoOccurred!!!");
	endOfYellowing = 310;
	return endOfYellowing;
}
// Gaussian fitting of VpdSat data on the function of the type : y = p1 * exp(-(x-p2)/p3)^2 (Gaussian Function
// we are interested in fitting the VPD sat data to find out the value of the p2 parameter value, used to asses the first day of senescence on the
// basis of the VPDsat peak value assumption
// The residuals are defined as (y[i] - f(x[i]))/y_error[i]
// Original public domain version by B. Garbow, K. Hillstrom, J. More'
// (Argonne National Laboratory, MINPACK project, March 1980)
// Tranlation to C Language by S. Moshier (moshier.net)
// Enhancements, documentation and packaging by C. Markwardt
// (comparable to IDL fitting routine MPFIT
// see http://cow.physics.wisc.edu/~craigm/idl/idl.html)
// Copyright (C) 2003, 2004, 2006, 2007, 2009, 2010 Craig B. Markwardt

//ALESSIOC to remove
void get_vpsat(cell_t *const c, const int day, const int month, const int year, const int index)
{
	meteo_t *met;
	met = c->years[year].m;
	c->vpSat[index] = 0.35*(0.6107*exp(17.38*(met[month].d[day].tavg)/(239.0 +(met[month].d[day].tavg))))+0.154;
	//logger(g_soil_log, "\nvpSat: %g", c->vpSat[i]);
}

int gaussian_x_y(int m, int n, double *p, double *dy,
		double **dvec, void *vars)
{
	int i;
	struct vars_struct *v = (struct vars_struct *) vars;
	double *x, *y, *ey;
	double xc;
	/* Retrieve values of x, y and y_error from private structure */

	x = v->x;
	y = v->y;
	ey = v->ey;


	for (i=0; i<m; i++) {
		xc = x[i]-p[1];
		dy[i] = (y[i] - p[0] * exp(-pow((xc/p[2]),2)))/ey[i];
		//dy[i] = (y[i] - p[1]*exp(-0.5*xc*xc/sig2) - p[0])/ey[i];
	}

	return 0;
}


void senescenceDayOne(species_t *const s, const meteo_t *const met, cell_t *const c)
{
	/* Test harness routine, which contains test data, invokes mpfit() */
	/* X - independent variable */
	double x[365];
	/* Y - measured value of dependent quantity */
	double y[365];
	double ey[365];   /* Measurement uncertainty - initialized below */

	//	double x[] = {-1.7237128E+00,1.8712276E+00,-9.6608055E-01,
	//			-2.8394297E-01,1.3416969E+00,1.3757038E+00,
	//			-1.3703436E+00,4.2581975E-02,-1.4970151E-01,
	//			8.2065094E-01};
	//	double y[] = {-4.4494256E-02,8.7324673E-01,7.4443483E-01,
	//			4.7631559E+00,1.7187297E-01,1.1639182E-01,
	//			1.5646480E+00,5.2322268E+00,4.2543168E+00,
	//			6.2792623E-01};
	//	double ey[10];

	double p[] = {0.0, 1.0, 1.0, 1.0};       /* Initial conditions */
	double pactual[] = {0.0, 4.70, 0.0, 0.5};/* Actual values used to make data*/
	double perror[4];                   /* Returned parameter errors */
	mp_par pars[4];			   /* Parameter constraints */
	int i;
	struct vars_struct v;  /* Private data structure */
	int status;

	mp_result result;

	//Marconi: this part of the function now doesn't get executed; that is ok because Collalti wants it to run with
	//middaylenght photoperiod reasoning

	//filling the x array with DOY values
	for(i=0; i<365; i++)
	{
		x[i] = i+1;
	}

	//filling the y array with VPDsat values
	for(i = 0; i< 365; i++)
	{
		//use the formula to evaluate VPDsat using surface temperature; compute it elsewhere, without including here the nth for cycle
		y[i] = c->vpSat[i];
	}

	memset(&result,0,sizeof(result));       /* Zero results structure */
	result.xerror = perror;

	memset(pars,0,sizeof(pars));        /* Initialize constraint structure */
	/* No constraints */
	for (i=0; i<365; i++) ey[i] = 0.5;   /* Data errors */



	/* Fill private data structure */
	v.x = x;
	v.y = y;
	v.ey = ey;

	/* Call fitting function for 10 data points and 2 parameters */
	status = mpfit(gaussian_x_y, 365, 4, p, pars, 0, (void *) &v, &result);
	s->counter[SENESCENCE_DAYONE] = (int)p[1]; //check if 2 is the correct parameter
	//todo the 1.6 cap
	//logger(g_soil_log, "\nfirstDayOfSenescence senescence:\t%g", s->value[SENESCENCE_DAYONE]);
	//logger(g_soil_log, "\n*** testlinfit status = %d\n", status);
	/* ... print or use the results of the fitted parametres p[] here! ... */
	//Marconi: turn on the following function to print the parameters
	//printresult(p, pactual, &result);


}

/* Simple routine to print the fit results */
void printresult(double *x, double *xact, mp_result *result)
{
	int i;

	if ((x == 0) || (result == 0)) return;
	logger(g_soil_log, "  CHI-SQUARE = %f    (%d DOF)\n",
			result->bestnorm, result->nfunc-result->nfree);
	logger(g_soil_log, "        NPAR = %d\n", result->npar);
	logger(g_soil_log, "       NFREE = %d\n", result->nfree);
	logger(g_soil_log, "     NPEGGED = %d\n", result->npegged);
	logger(g_soil_log, "     NITER = %d\n", result->niter);
	logger(g_soil_log, "      NFEV = %d\n", result->nfev);
	logger(g_soil_log, "\n");
	if (xact) {
		for (i=0; i<result->npar; i++) {
			logger(g_soil_log, "  P[%d] = %f +/- %f     (ACTUAL %f)\n",
					i, x[i], result->xerror[i], xact[i]);
		}
	} else {
		for (i=0; i<result->npar; i++) {
			logger(g_soil_log, "  P[%d] = %f +/- %f\n",
					i, x[i], result->xerror[i]);
		}
	}

}




