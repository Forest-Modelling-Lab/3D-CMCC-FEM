/*peak_lai.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"

//TO COMPUTE YEARLY PEAK LAI FROM PROVIOUS YEARLY LAI
/*
void Get_peak_lai (SPECIES *const s, int years, int month)
{

    if (years == 0 )
    {
        //JANUARY
        if (month == 0)
        {
            s->value[PEAK_Y_LAI] = s->value[LAI];
            Log("Peak Y Lai from input data = %g \n", s->value[PEAK_Y_LAI]);

            s->value[OLD_LAI] = 0;
        }
        else
        {
            s->value[PEAK_LAI] = Maximum (s->value[OLD_LAI], s->value[LAI]);
            //Log("Peak Lai = %g \n", s->value[PEAK_Y_LAI]);

            if (s->value[LAI] > s->value[PEAK_Y_LAI] )
            {
                Log ("LAI > Peak Y Lai \n ");
                //s->value[LAI] = s->value[PEAK_Y_LAI];
            }


        }
        if (month == 11)
        {
            s->value[PEAK_Y_LAI] = s->value[PEAK_LAI];
            s->value[PEAK_LAI] = 0;
        }
    }
    else
    {
        if (month == 0)
        {
            s->value[OLD_LAI] = 0;
        }

        s->value[PEAK_LAI] = Maximum (s->value[OLD_LAI], s->value[LAI]);
        Log("Peak Lai = %g \n", s->value[PEAK_LAI]);
        Log("Peak Y Lai = %g \n", s->value[PEAK_Y_LAI]);


        if (m->cells[cell].heights[height].ages[age].species[species].value[LAI] > m->cells[cell].heights[height].ages[age].species[species].value[PEAK_Y_LAI])
        {
            m->cells[cell].heights[height].ages[age].species[species].value[LAI] = m->cells[cell].heights[height].ages[age].species[species].value[PEAK_Y_LAI];
        }

        if (month == 11)
        {
            s->value[PEAK_Y_LAI] = s->value[PEAK_LAI];
            s->value[PEAK_LAI] = 0;
        }
    }
}
 */

void Get_peak_lai_from_pipe_model (SPECIES *const s, int years, int month)
{
	static float oldBasalArea;


	static float sapwood_perc;

	//compute Basal Area
	if (years == 0)
	{
		s->value[BASAL_AREA] = (((pow((s->value[AVDBH] / 2), 2)) * Pi) / 10000);
		s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
	}
	else
	{
		oldBasalArea = s->value[BASAL_AREA];
		s->value[BASAL_AREA] = (((pow((s->value[AVDBH] / 2), 2)) * Pi) / 10000);
		s->value[STAND_BASAL_AREA] = s->value[BASAL_AREA] * s->counter[N_TREE];
		Log("old basal area = %g \n", oldBasalArea);
		Log(" Basal Area Increment= %g m^2/tree \n", s->value[BASAL_AREA] - oldBasalArea);
		Log(" Basal Area Increment= %g cm^2/tree \n", (s->value[BASAL_AREA] - oldBasalArea) * 10000);

	}

	Log("Basal Area for this layer = %g m^2/tree\n", s->value[BASAL_AREA]);
	Log("Stand Basal Area for this layer = %g m^2/area\n", s->value[STAND_BASAL_AREA]);


	//Log("**Kostner eq** \n");
	//sapwood area
	//see Kostner et al in Biogeochemistry of Forested Catchments in a Changing Environment, Matzner, Springer for Q. petraea
	s->value[SAPWOOD_AREA] = s->value[SAP_A] * pow (s->value[AVDBH], s->value[SAP_B]);

	Log("sapwood from Kostner = %g cm^2\n", s->value[SAPWOOD_AREA]);
	s->value[HEARTWOOD_AREA] = (s->value[BASAL_AREA] * 10000) - s->value[SAPWOOD_AREA];
	Log("heartwood from Wang et al 2010 = %g cm^2\n", s->value[HEARTWOOD_AREA]);

	sapwood_perc = (s->value[SAPWOOD_AREA] / 10000) / s->value[BASAL_AREA];
	Log("Sapwood/Basal Area = %g \n", sapwood_perc );
	Log("Sapwood/Basal Area = %g %%\n",sapwood_perc * 100 );


	//compute sapwood pools and heatwood pool
	s->value[WS_sap] =  s->value[BIOMASS_STEM_CTEM] * sapwood_perc;
	Log("Stem biomass = %g tDM/area \n", s->value[BIOMASS_STEM_CTEM]);
	Log("Sapwood biomass = %g tDM/area \n", s->value[WS_sap]);
	s->value[WS_heart] = s->value[BIOMASS_STEM_CTEM] - s->value[WS_sap];
	Log("Heartwood biomass = %g tDM/area \n", s->value[WS_heart]);

	Log("Leaf Area from Kostner-LPJ = %g m^2\n", (s->value[SAPWOOD_AREA] / 10000) * s->value[SAP_LEAF]);
	Log("Crown diameter = %g m^2\n", s->value[CROWN_DIAMETER_DBHDC_FUNC]);
	Log("Crown Area for Kostner = %g m^2\n", s->value[CROWN_AREA_DBHDC_FUNC]);

	s->value[PEAK_Y_LAI] = ((s->value[SAPWOOD_AREA] / 10000) * s->value[SAP_LEAF]) / s->value[CROWN_AREA_DBHDC_FUNC];
	Log("year %d PEAK LAI from Kostner = %g m^2 m^-2\n",years, s->value[PEAK_Y_LAI]);


	s->value[MAX_BIOMASS_FOLIAGE_CTEM] = ((s->value[PEAK_Y_LAI] * (s->value[CANOPY_COVER_DBHDC] * settings->sizeCell))/ (s->value[SLAmkg]* 2.0)) / 1000;
	Log("Maximum foliage biomass = %g tDM/area \n", s->value[MAX_BIOMASS_FOLIAGE_CTEM]);
	Log("Maximum foliage biomass = %g gDM/tree \n", (s->value[MAX_BIOMASS_FOLIAGE_CTEM] /s->counter[N_TREE])* 1000000);

	s->value[MAX_BIOMASS_FOLIAGE_CTEM] = ((s->value[PEAK_Y_LAI] / (s->value[SLAmkg]* 2.0))*(s->value[CANOPY_COVER_DBHDC] * settings->sizeCell)/1000);
	Log("Maximum foliage biomass = %g Kg/area \n", s->value[MAX_BIOMASS_FOLIAGE_CTEM]/s->counter[N_TREE]);
	Log("SLA in mod= %g KgC/m^2 \n", s->value[SLAmkg]);




}
