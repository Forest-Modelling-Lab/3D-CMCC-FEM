//
#include <math.h>
#include "types.h"





extern void Get_Veg_Months (MATRIX *const m, const YOS *const yos, const int month, const int years)
{
	MET_DATA *met;
	static int cell;
	static int height;
	static int age;
	static int species;


	met = (MET_DATA*) yos[years].m;


	for ( cell = 0; cell < m->cells_count; cell++)
	{
		for ( height = m->cells[cell].heights_count - 1; height >= 0; height-- )
		{
			for ( age = m->cells[cell].heights[height].ages_count - 1 ; age >= 0 ; age-- )
			{
				for (species = 0; species < m->cells[cell].heights[height].ages[age].species_count; species++)
				{
					if (settings->version == 'u')
					{
						if (!month)
						{
							m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] = 0;
						}
						if (m->cells[cell].heights[height].ages[age].species[species].phenology == D)
						{
							if ((met[month].tav >= m->cells[cell].heights[height].ages[age].species[species].value[GROWTHSTART] && month < 6)
									|| (met[month].tav >= m->cells[cell].heights[height].ages[age].species[species].value[GROWTHEND] && month >= 6))
							{
								m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] += 1;
								//Log("MONTHs = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
								Log("----- Vegetative month = %d for species %s\n", month + 1, m->cells[cell].heights[height].ages[age].species[species].name );
							}
						}
						else
						{
							m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] = 12;
						}
						if (month == 11)
						{
							Log("----- TOTAL VEGETATIVE MONTHS for species %s = %d \n\n", m->cells[cell].heights[height].ages[age].species[species].name, m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
						}
					}
					else
					{
						if (!month)
						{
							m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] = 0;
						}
						if (m->cells[cell].heights[height].ages[age].species[species].phenology == 0)
						{
							if (met[month].ndvi_lai >= 0.5)
							{
								m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] += 1;
								//Log("MONTHs = %d \n", m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
								Log("----- Vegetative month = %d \n", month + 1);
							}
						}
						else
						{
							m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE] = 12;
						}
						if (month == 11)
						{
							Log("----- TOTAL VEGETATIVE MONTHS = %d \n\n", m->cells[cell].heights[height].ages[age].species[species].counter[MONTH_VEG_FOR_LITTERFALL_RATE]);
						}
					}
				}
			}
		}
	}
}



//----------------------------------------------------------------------------//
//                                                                            //
//                             GetDayLength                                   //
//                                                                            //
//----------------------------------------------------------------------------//



/*

   3PG version

   float GetDayLength (float latitude, int MonthLength)
   {
// gets fraction of day when sun is "up"
float sLat, cLat, sinDec, cosH0;

sLat = sin(Pi * latitude / 180);
cLat = cos(Pi * latitude / 180);

sinDec = 0.4 * sin(0.0172 * (dayOfYear - 80));
cosH0 = sinDec * sLat / (cLat * sqrt(1 - pow(sinDec,2)));
if (cosH0 > 1)
return 0;
else if (cosH0 < -1)
return 1;
else {
return acos(cosH0) / Pi;
}
 */


//BIOME-BGC version
//Running-Coughlan 1998, Ecological Modelling

void GetDayLength ( CELL * c,  int MonthLength)
{
	//int cell;
	//Log("MonthLenght = %d \n", MonthLength);



	float ampl;  //seasonal variation in Day Length from 12 h
	ampl = (exp (7.42 + (0.045 * site->lat))) / 3600;


	c->daylength = ampl * (sin ((MonthLength - 79) * 0.01721)) + 12;


	Log("daylength = %g \n", c->daylength);
}


void Get_Abscission_DayLength ( CELL * c)
{
	//from Schwalm and Ek, 2004
	c->abscission_daylength = (39132 + (pow (1.088, (site->lat + 60.753))))/(60*60);
	Log("Abscission day length = %g \n", c->abscission_daylength);

}
