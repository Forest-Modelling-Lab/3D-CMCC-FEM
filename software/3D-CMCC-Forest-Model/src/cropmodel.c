/*cropmodel.c*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types.h"
#include "constants.h"

extern  const char *szMonth[MONTHS];
extern int MonthLength[];
//extern int DaysInMonth[];
//extern int fill_cell_from_heights(CELL *const c, const ROW *const row);


/* */
int crop_model_M(MATRIX *const m, const YOS *const yos, const int years, const int month, const int years_of_simulation)
{
	

	static int cell;
	//static int height;
	//static int age;
	//static int species;

	//compute VPD
	static float vpd;

	MET_DATA *met;

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

	//monthly loop on each cell
	for ( cell = 0; cell < m->cells_count; cell++)
	{

		Log("MONTH SIMULATED = %s\n", szMonth[month]);

		GetDayLength (&m->cells[cell], MonthLength[month]);

		//compute vpd
		//todo remove if used VPD
		//if the VPD input data are in KPa then multiply for 10 to convert in mbar
		//VPD USED MUST BE IN mbar
		vpd =  met[month].vpd * 10.0; //Get_vpd (met, month);

		//all your function must be written here!!!!!!!


		Log("PROVA TEMPERATURA = %g\n", met[month].tav);
		Log("PROVA SETTINGS = %c\n", settings->spatial);

	}






	Log("****************END OF CELL***************\n");
	/* ok */
	return 1;



}
