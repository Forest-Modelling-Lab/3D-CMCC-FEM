#include "spinup.h"
#include "common.h"
#include <assert.h>

int spinup(matrix_t* const m, const int spinup_years_count)
{
	int cell;
	int year;

	assert(m);
	assert(spinup_years_count > 0);

	for ( cell = 0; cell < m->cells_count; ++cell )
	{
		for ( year = 0; year < spinup_years_count / m->cells[cell].years_count; year++ )
		{
			// TODO
		}
	}

	return 1;
}