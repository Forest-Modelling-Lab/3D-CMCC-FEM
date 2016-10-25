/* allometry.c */
#include "allometry.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "constants.h"
#include "logger.h"
#include "settings.h"

extern logger_t* g_debug_log;
extern settings_t* g_settings;

void crown_allometry (cell_t *const c)
{

	int height;
	int dbh;
	int age;
	int species;
	int crown_form_factor;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	logger(g_debug_log,"\n*CROWN ALLOMETRY*\n");

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

					/* Crown allometry */
					/* Crown Projected Diameter using DBH-DC */
					s->value[CROWN_DIAMETER] = d->value * s->value[DBHDC_EFF];
					logger(g_debug_log, "-Crown Projected Diameter = %g m\n", s->value[CROWN_DIAMETER]);

					/* Crown Projected Radius using DBH-DC */
					s->value[CROWN_RADIUS] = s->value[CROWN_DIAMETER] / 2.;
					logger(g_debug_log, "-Crown Projected Radius = %g m\n", s->value[CROWN_RADIUS]);

					/* Crown Projected Area using DBH-DC */
					s->value[CROWN_AREA] = ( Pi / 4) * pow (s->value[CROWN_DIAMETER], 2 );
					logger(g_debug_log, "-Crown Projected Area = %g m^2\n", s->value[CROWN_AREA]);

					/* Canopy Projected Cover using DBH-DC */
					s->value[CANOPY_COVER] = s->value[CROWN_AREA] * s->counter[N_TREE] / g_settings->sizeCell;
					logger(g_debug_log, "-Canopy Projected Cover = %g %%\n", s->value[CANOPY_COVER] * 100.0);

					/* Crown Height */
					/* it mainly follows SORTIE-ND approach in the form of x = a*tree height^c */
					/* note when b = 1 the function is prettily a linear function */

					s->value[CROWN_HEIGHT] = s->value[CROWN_A] * pow(h->value, s->value[CROWN_B]);
					logger(g_debug_log, "-Crown Height = %g m\n", s->value[CROWN_HEIGHT]);

					/* cast s->value[CROWN_FORM_FACTOR] to integer */
					crown_form_factor = s->value[CROWN_FORM_FACTOR];

					/* note: crown surface area is considered as the area that can intercept light */
					switch (crown_form_factor)
					{
					case 0: /* cylinder */
						logger(g_debug_log, "-Crown form factor = cylinder\n");
						s->value[CROWN_SURFACE_AREA] = (pow(s->value[CROWN_RADIUS], 2.) * Pi) + ((s->value[CROWN_DIAMETER] * Pi) * s->value[CROWN_HEIGHT]);
						break;

					case 1: /* cone */
						logger(g_debug_log, "-Crown form factor = cone\n");
						s->value[CROWN_SURFACE_AREA] = Pi * s->value[CROWN_RADIUS] * sqrt(pow(s->value[CROWN_RADIUS],2.) + pow(s->value[CROWN_HEIGHT],2.));
						break;

					case 2: /* sphere */
						logger(g_debug_log, "-Crown form factor = sphere\n");
						s->value[CROWN_SURFACE_AREA] = pow(s->value[CROWN_RADIUS], 2.) * 4. * Pi;
						break;

					case 3: /* ellipsoid */
						logger(g_debug_log, "-Crown form factor = ellipsoid\n");
						break;
					}
					logger(g_debug_log, "-Crown Surface Area = %g m2\n", s->value[CROWN_SURFACE_AREA]);
				}
			}
		}
	}
}

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




