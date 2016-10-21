/* allometry.c */
#include "allometry.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "constants.h"
#include "logger.h"

extern logger_t* g_debug_log;

void allometry_power_function(cell_t *const c)
{
	int height;
	int dbh;
	int age;
	int species;

	int mod_age;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	logger(g_debug_log,"\nAllometry Power Function\n");

	for ( height = 0; height < c->heights_count; ++height )
	{
		h = &c->heights[height];

		for ( dbh = 0; dbh < h->dbhs_count; ++dbh )
		{
			d =  &h->dbhs[dbh];

			for ( age = 0; age < d->ages_count; ++age )
			{
				a = &d->ages[age];

				for ( species = 0; species < a->species_count; ++species )
				{
					s = &a->species[species];

					logger(g_debug_log, "Species = %s\n", s->name);

					logger(g_debug_log, "Age = %d\n", a->value);

					/* note: ISIMIP special case */
					if (a->value == 0)
					{
						mod_age = 1;
					}
					else
					{
						mod_age = a->value;
					}
					logger(g_debug_log, "Age (used in function) = %d\n", mod_age);

					s->value[MASS_DENSITY] = s->value[RHOMAX] + (s->value[RHOMIN] - s->value[RHOMAX]) * exp(-ln2 * (mod_age / s->value[TRHO]));
					logger(g_debug_log, "-Mass Density = %g (tDM/m3)\n", s->value[MASS_DENSITY]);

					if ( d->value < 9 )
					{
						s->value[STEMCONST] = pow (e, -1.6381);
					}
					else if ( d->value > 9 && d->value < 15 )
					{
						s->value[STEMCONST] = pow (e, -3.51+1.27* s->value[MASS_DENSITY]);
					}
					else
					{
						s->value[STEMCONST] = pow (e, -3.51+1.27*s->value[MASS_DENSITY]);
					}
					logger(g_debug_log, "-Stem const = %f\n", s->value[STEMCONST]);
				}
			}
		}
	}
}

