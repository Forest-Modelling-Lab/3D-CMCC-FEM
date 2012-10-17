//
#include <math.h>
#include "types.h"



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
    int cell;
	//Log("MonthLenght = %d \n", MonthLength);



    float ampl;  //seasonal variation in Day Length from 12 h
    ampl = (exp (7.42 + (0.045 * site->lat))) / 3600;


    c->daylength = ampl * (sin ((MonthLength - 79) * 0.01721)) + 12;


    Log("daylength = %g \n", c->daylength);
}
