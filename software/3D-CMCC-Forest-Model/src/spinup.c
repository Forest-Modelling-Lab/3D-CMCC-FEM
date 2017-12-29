#include "spinup.h"
#include "common.h"
#include "initialization.h"
#include <assert.h>

extern soil_settings_t* g_soil_settings;
extern int DaysInMonth[];

int spinup(matrix_t* const m, const int spinup_years_count)
{
	int cell;

	assert(m);
	assert(spinup_years_count > 0);

	for ( cell = 0; cell < m->cells_count; ++cell )
	{
		int year_loop;

		initialization_cell_soil_physic(&m->cells[cell]);
		// TODO
		
		for ( year_loop = 0; year_loop < spinup_years_count; ++year_loop )
		{
			int year;
			for ( year = 0; year < m->cells[cell].years_count; ++year )
			{
				int month;
				for ( month = 0; month < METEO_MONTHS_COUNT; ++month )
				{
					int day;
					int days_per_month;

					days_per_month = DaysInMonth[month];
					// 1 == february
					if ( 1 == month && IS_LEAP_YEAR(m->cells[cell].years[year].year) )
					{
						++days_per_month;
					}

					for ( day = 0; day < days_per_month; ++day )
					{
						// TODO
					}
				}
			}
		}
	}

	return 1;
}