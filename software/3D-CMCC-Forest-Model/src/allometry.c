/* allometry.c */
#include "allometry.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "constants.h"
#include "logger.h"
#include "settings.h"
#include "mortality.h"
#include "common.h"

extern logger_t* g_debug_log;

void crown_allometry (cell_t *const c, const int height, const int dbh, const int age, const int species)
{
	double old_crown_proj;
	int crown_form_factor;
	double c_diameter;        /* crown diameter (m) */
	double c_height ;         /* crown height   (m) */
	double ce;

	height_t *h;
	dbh_t *d;
	age_t *a;
	species_t *s;

	h = &c->heights[height];
	d = &h->dbhs[dbh];
	a = &d->ages[age];
	s = &a->species[species];

	/*** Crown allometry ***/

	logger(g_debug_log,"\n*CROWN ALLOMETRY for %s *\n", s->name);


	/* assign previous year crown area projected */
	old_crown_proj = s->value[CROWN_AREA_PROJ];

	/* Crown Projected Diameter using DBH-DC (at zenith angle) */
	s->value[CROWN_DIAMETER] = d->value * s->value[DBHDC_EFF];
	logger(g_debug_log, "-Crown Projected Diameter = %f m\n", s->value[CROWN_DIAMETER]);

	/* Crown Projected Radius using DBH-DC (at zenith angle) */
	s->value[CROWN_RADIUS] = s->value[CROWN_DIAMETER] / 2.;
	logger(g_debug_log, "-Crown Projected Radius   = %f m\n", s->value[CROWN_RADIUS]);

	/* Crown Projected Area using DBH-DC (at zenith angle) */
	s->value[CROWN_AREA_PROJ] = Pi  * pow (s->value[CROWN_RADIUS], 2 );
	logger(g_debug_log, "-Crown Projected Area     = %f m2\n", s->value[CROWN_AREA_PROJ]);


	/* check if current canopy cover has been reduced */
	if ( s->counter[YOS] && ( old_crown_proj > s->value[CROWN_AREA_PROJ] ) )
	{
		//FIXME it basically rarely happens since crown area proj never descreases...
		/******** self pruning ********/
		self_pruning ( c, height, dbh, age, species, old_crown_proj, s->value[CROWN_AREA_PROJ] );
	}


	/* Crown Height */
	/* it mainly follows SORTIE-ND approach in the form of x = a*tree height^c */
	/* note when b = 1 the function is prettily a linear function */

	s->value[CROWN_HEIGHT] = s->value[CROWN_A] * pow(h->value, s->value[CROWN_B]);
	logger(g_debug_log, "-Crown Height             = %f m\n", s->value[CROWN_HEIGHT]);

	/* controls and use it if need */
	if ( s->value[CROWN_HEIGHT] > h->value )
	{
		s->value[CROWN_HEIGHT] = h->value;
	}

	/* check */
	CHECK_CONDITION(s->value[CROWN_HEIGHT], >, h->value);

	/* cast s->value[CROWN_FORM_FACTOR] to integer */
	crown_form_factor = (int)s->value[CROWN_FORM_FACTOR];

	/* compute overall single tree crown area */
	switch (crown_form_factor)
	{
	case 0: /* cylinder */
		logger(g_debug_log, "-Crown form factor        = cylinder\n");

		s->value[CROWN_AREA]     = ( 2. * s->value[CROWN_AREA]) + (2 * Pi * s->value[CROWN_RADIUS] * s->value[CROWN_HEIGHT]);
		s->value[CROWN_VOLUME]   = s->value[CROWN_AREA_PROJ] * s->value[CROWN_HEIGHT];
		break;

	case 1: /* cone */
		logger(g_debug_log, "-Crown form factor        = cone\n");

		s->value[CROWN_AREA]     = s->value[CROWN_AREA_PROJ] + (Pi * s->value[CROWN_RADIUS] * (sqrt(pow(s->value[CROWN_RADIUS],2.) + pow(s->value[CROWN_HEIGHT],2.))));
		s->value[CROWN_VOLUME]   = (s->value[CROWN_AREA_PROJ] * s->value[CROWN_HEIGHT])/3.;
		break;

	case 2: /* sphere */
		logger(g_debug_log, "-Crown form factor        = sphere\n");

		s->value[CROWN_AREA]     = ( s->value[CROWN_AREA_PROJ] * 4 );
		s->value[CROWN_VOLUME]   = 4. / 3. * Pi * pow (s->value[CROWN_RADIUS],3.);
		break;

	case 3: /* tri-bi-axial ellipsoid */
		logger(g_debug_log, "-Crown form factor        = ellipsoid \n");

		c_diameter = s->value[CROWN_DIAMETER] / 2.;
		c_height   = s->value[CROWN_HEIGHT]   / 2.;
		ce = acos (c_diameter / c_height);

		/* ellissoide prolato */
		s->value[CROWN_AREA]     = 2. * Pi * ( pow(c_diameter, 2.) + ( c_diameter * c_height ) * ( ce / sin(ce) ));
		s->value[CROWN_VOLUME]   = ( 4. / 3. ) * Pi * pow ( c_diameter , 2. ) * c_height;

		break;
	}

	/****************************************************************************/

	/* crown area exposed */
	/* (ORIGINAL) Canopy Projected Cover (integrated all over all viewing angles) */
	/* following Cauchy's theorems Duursma et al., 2012, Tree Phys) */
	s->value[CROWN_AREA_EXP] = s->value[CROWN_AREA] / 4 ;

	/****************************************************************************/

	logger(g_debug_log, "-Crown Area               = %f m2\n", s->value[CROWN_AREA]);
	logger(g_debug_log, "-Crown Area (exp)         = %f m2\n", s->value[CROWN_AREA_EXP]);
	logger(g_debug_log, "-Crown Volume             = %f m3\n", s->value[CROWN_VOLUME]);

	/* Crown density (NOT USED) */
	/* following Duursma et al., 2012 */
	//s->value[CROWN_DENSITY] = (s->value[ALL_LAI_PROJ] / (double)s->counter[N_TREE]) / s->value[CROWN_AREA_EXP];
	//logger(g_debug_log, "-Crown Density = %f 1/m-1\n", s->value[CROWN_DENSITY]);

}

void allometry_power_function(cell_t *const c)
{
	int height;
	int dbh;
	int age;
	int species;

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

					s->value[MASS_DENSITY] = s->value[RHO1] + (s->value[RHO0] - s->value[RHO1]) * exp(-LN2 * ((double)a->value / s->value[TRHO]));
					logger(g_debug_log, "-Mass Density = %f (tDM/m3)\n", s->value[MASS_DENSITY]);

					if ( d->value < 9 )
					{
						s->value[STEMCONST] = pow (e, -1.6381);
					}
					else
					{
						s->value[STEMCONST] = pow (e, -3.51 + 1.27 * s->value[MASS_DENSITY]);
					}
					logger(g_debug_log, "-Stem const = %f\n", s->value[STEMCONST]);
				}
			}
		}
	}
}




