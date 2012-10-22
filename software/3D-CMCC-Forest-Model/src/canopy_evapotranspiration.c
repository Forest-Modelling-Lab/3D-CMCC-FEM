/*canopy_evapotranspiration.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "types.h"


//this file computes canopy transpiration and rain interception

void Get_canopy_evapotranspiration (SPECIES *const s,  const MET_DATA *const met, int month, int z, float DayLenght, float Net_Radiation, float vpd)
{

    float CanCond;
    float defTerm;
    float duv;
    float Etransp;
    float CanopyTranspiration;
    //float DailyTransp;

    float Interception;

    float const e20 = 2.2;          // rate of change of saturated VP with T at 20C
	float const rhoAir = 1.2;       // density of air, kg/m3
	float const lambda = 2460000;   // latent heat of vapourisation of H2O (J/kg)
	float const VPDconv = 0.000622; // convert VPD to saturation deficit = 18/29/1000


    /*Canopy Conductance*/

    //Lai is different among layers so CanCond is different
    CanCond = s->value[MAXCOND] * s->value[PHYS_MOD] *  Minimum(1.0, s->value[LAI] / s->value[LAIGCX]);
    //Log("Canopy Conductance  = %g\n", CanCond);

    /*Canopy Traspiration*/

    // Penman-Monteith equation for computing canopy transpiration
    // in kg/m2/day, which is converted to mm/day.
    // The following are constants in the PM formula (Landsberg & Gower, 1997)
    defTerm = rhoAir * lambda * (VPDconv * vpd) * s->value[BLCOND];
    //Log("defTerm = %g\n", defTerm);
    duv = (1.0 + e20 + s->value[BLCOND] / CanCond);
    //Log("duv = %g\n", duv);

    //10 july 2012
    //net radiation should be takes into account of the ALBEDO effect
    //for APAR the ALBEDO is 1/3 as large for PAR because less PAR is reflected than NetRad
    //see Biome-BGC 4.2 tech report
    Etransp = (e20 * (Net_Radiation * ( 1 - s->value[MAXALB])) + defTerm) / duv;
    Log("Etransp con ALBEDO = %g J/m^2/sec\n", Etransp);

    Etransp = (e20 * Net_Radiation + defTerm) / duv;  // in J/m2/s
    Log("Etransp for dominant layer = %g J/m^2/sec\n", Etransp);
    //Log("NET RADIATION = %g \n", Net_Radiation);

    CanopyTranspiration = Etransp / lambda * DayLength;         // converted to kg-mm H2o/m2/day
    //1Kg m^2 H2o correspond to 1mm H2o
    Log("Daily Canopy Transpiration = %g mm-Kg H2o/m^2/day\n", CanopyTranspiration);
    DailyTransp = CanopyTranspiration;
    //Log("Daily Transpiration = %g mm/m^2/day\n", DailyTransp);
    //initial transpiration from Penman-Monteith (mm/day converted to mm/month)
    MonthTransp = CanopyTranspiration * DaysInMonth [month];
    Log("Monthly Canopy Transpiration = %g mm-Kg H2o/m^2/month\n", MonthTransp);

      /*Canopy evaporation of intercepted rainfall*/

    //quantità di Rain intercettata e che quindi non arriva al suolo
    //interception is a rate not a quantity

    if ( met[month].rain > 0 )
    {
        //compute interception for each class
        if (s->value[LAIMAXINTCPTN] <= 0)
        {
            Interception = s->value[MAXINTCPTN] ;
            Log("Rain Interception = MAXINTCPTN\n");
            //Log("LAIMAXINTCPTN = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[LAIMAXINTCPTN]);
        }
        else
        {

            Interception = s->value[MAXINTCPTN] * Minimum(1 , s->value[LAI] /                   m->cells[cell].heights[height].ages[age].species[species].value[LAIMAXINTCPTN]);
            Log("Rain Interception not use MAXINTCPTN\n");

        }
        Log("Rain Interception rate for dominant layer = %g\n", Interception);


        //see also CLM model for rain interception
        /*
        Interception = 1 - exp (-0.5 * m->cells[cell].heights[height].ages[age].species[species].value[LAI])
        */


        //taking into account cell coverage
        if ( z == top_layer)
        {

            //Rainfall intercepted
            //heighest height class
            //interception for the highest of the the dominant class
            if (dominant_counter == 1)
            {
                RainIntercepted = met[month].rain * Interception * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
                Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
                Log("Percentage of Rain Intercepted from dominant canopy = %g%%\n", (RainIntercepted * 100) / met[month].rain );
                /*Evapotranspiration*/

                Evapotranspiration = (met[month].rain * Interception + MonthTransp) * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
                Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].z, szMonth[month], Evapotranspiration );
                //control
                if (RainIntercepted >= met[month].rain)
                {
                       RainIntercepted = met[month].rain;
                       Log("Rain is completely intercepted by the highest height class in the dominant layer\n");
                       lessrain = 0;
                }
                else
                {
                        lessrain = met[month].rain - RainIntercepted;
                }
            }
            else
            {
                Log("Less Rain = %g mm\n", lessrain);
                if (lessrain <= 0)
                {
                        Log("Rainfall is completely intercepted from the upper layer\n");
                }
                else
                {
                        RainIntercepted = lessrain * Interception * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
                        Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
                }

                /*Evapotranspiration*/

                Evapotranspiration = (lessrain * Interception + MonthTransp) * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
                Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].z, szMonth[month], Evapotranspiration );

                lessrain -= RainIntercepted;

            }
        }
        //dominated layers
        else
        {
            if (m->cells[cell].heights[height].ages[age].species[species].value[LAIMAXINTCPTN] <= 0)
            {
                Interception = m->cells[cell].heights[height].ages[age].species[species].value[MAXINTCPTN];
                Log("Rain Interception = MAXINTCPTN\n");
                //Log("LAIMAXINTCPTN = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[LAIMAXINTCPTN]);
            }
            else
            {

                Interception = m->cells[cell].heights[height].ages[age].species[species].value[MAXINTCPTN] *
                        Minimum(1, m->cells[cell].heights[height].ages[age].species[species].value[LAI] /
                                        m->cells[cell].heights[height].ages[age].species[species].value[LAIMAXINTCPTN]);
                Log("Rain Interception not use MAXINTCPTN\n");
            }
            Log("Rain Interception rate for dominated layer = %g\n", Interception);

            if (lessrain > 0)
            {
                RainIntercepted = lessrain * Interception * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
                Log("Rainfall Intercepted = %g mm/month\n", RainIntercepted);
            }
            else
            {
                RainIntercepted = 0;
                Log("No Rainfall for this layer\n");
            }

            /*Evapotranspiration*/

            Evapotranspiration = (lessrain * Interception + MonthTransp) * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC];
            Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].z, szMonth[month], Evapotranspiration );

            lessrain -= RainIntercepted;


            //Assuming No Rain Intercpetion for Lower Layers
            //Interception = 0.01;
            //Log("No Rain Interception for Lower Layer = %g\n", Interception);
        }
    }
    else
    {
        Log("NO RAIN-NO INTERCEPTION\n");
        Evapotranspiration =  MonthTransp;
        Log("Monthly Evapotranspiration for layer %d at month %s = %g \n",m->cells[cell].z, szMonth[month], Evapotranspiration );
    }


    m->cells[cell].heights[height].ages[age].species[species].value[CUM_MONTHLY_EVAPOTRANSPIRATION] += Evapotranspiration;
    Log("Cumulated Evapotranspiration for this layer = %g mm\n", m->cells[cell].heights[height].ages[age].species[species].value[CUM_MONTHLY_EVAPOTRANSPIRATION]);

    Total_Yearly_Evapotransipration += Evapotranspiration;
    Log("TOTAL Cumulated Evapotranspiration = %g mm\n",Total_Yearly_Evapotransipration);


    //compute traspiration for area
    //Log("Monthly Canopy Transpiration per area = %g mm-Kg H2o/ha^-1/month\n", MonthTransp * m->cells[cell].heights[height].ages[age].species[species].value[CANOPY_COVER_DBHDC] * SIZECELL);
}
