#include <stdio.h>
#include <stdarg.h>
#include "types.h"

extern int log_enabled;

static FILE *file_log ;

int logInit(char * logFileName)
{
	file_log = fopen(logFileName, "w");

	if ( !file_log ) return 0;

	return 1;
}

void Log(const char *szText, ...)
{
	char szBuffer[BUFFER_SIZE_LOG] = { 0 };
	//va_list pArgList = { 0 };
	va_list pArgList;

	va_start(pArgList, szText);
	vsnprintf(szBuffer, BUFFER_SIZE_LOG, szText, pArgList);
	va_end(pArgList);

	fputs(szBuffer, stdout);

	if ( log_enabled && file_log )
		fputs(szBuffer, file_log);
}

void logClose(void)
{
	if ( file_log )	fclose(file_log);
}

//for daily output
static FILE *daily_file_log ;

int daily_logInit(char * daily_logFileName)
{
	daily_file_log = fopen(daily_logFileName, "w");

	if ( !daily_file_log ) return 0;

	return 1;
}

void Daily_Log(const char *szText, ...)
{
	char szBuffer[BUFFER_SIZE_LOG] = { 0 };
	//va_list pArgList = { 0 };
	va_list pArgList;

	va_start(pArgList, szText);
	vsnprintf(szBuffer, BUFFER_SIZE_LOG, szText, pArgList);
	va_end(pArgList);

	fputs(szBuffer, stdout);

	if ( log_enabled && daily_file_log )
		fputs(szBuffer, daily_file_log);
}

void daily_logClose(void)
{
	if ( daily_file_log )	fclose(daily_file_log);
}

//for monthly output
static FILE *monthly_file_log ;

int monthly_logInit(char * monthly_logFileName)
{
	monthly_file_log = fopen(monthly_logFileName, "w");

	if ( !monthly_file_log ) return 0;

	return 1;
}

void Monthly_Log(const char *szText, ...)
{
	char szBuffer[BUFFER_SIZE_LOG] = { 0 };
	//va_list pArgList = { 0 };
	va_list pArgList;

	va_start(pArgList, szText);
	vsnprintf(szBuffer, BUFFER_SIZE_LOG, szText, pArgList);
	va_end(pArgList);

	fputs(szBuffer, stdout);

	if ( log_enabled && monthly_file_log )
		fputs(szBuffer, monthly_file_log);
}

void monthly_logClose(void)
{
	if ( monthly_file_log )	fclose(monthly_file_log);
}

//for annual output
static FILE *annual_file_log ;

int annual_logInit(char * annual_logFileName)
{
	annual_file_log = fopen(annual_logFileName, "w");

	if ( !annual_file_log ) return 0;

	return 1;
}

void Annual_Log(const char *szText, ...)
{
	char szBuffer[BUFFER_SIZE_LOG] = { 0 };
	//va_list pArgList = { 0 };
	va_list pArgList;

	va_start(pArgList, szText);
	vsnprintf(szBuffer, BUFFER_SIZE_LOG, szText, pArgList);
	va_end(pArgList);

	fputs(szBuffer, stdout);

	if ( log_enabled && annual_file_log )
		fputs(szBuffer, annual_file_log);
}

void annual_logClose(void)
{
	if ( annual_file_log )	fclose(annual_file_log);
}


extern void Get_EOY_cumulative_balance_layer_level (SPECIES *s, HEIGHT *h)
{

	//CUMULATIVE BALANCE FOR ENTIRE LAYER
	Log("**CUMULATIVE BALANCE for layer %d ** \n", h->z);
	Log("END of Year Yearly Cumulated GPP for layer %d  = %f gCm^2 year\n", h->z, s->value[YEARLY_POINT_GPP_G_C]);
	Log("END of Year Yearly Cumulated NPP for layer %d  = %f tDM/area year\n", h->z, s->value[YEARLY_NPP]);
	Log("END of Year Yearly Cumulated NPP/ha_area_covered for layer %d  = %f tDM/ha_area_covered year\n", h->z, (s->value[YEARLY_NPP]/settings->sizeCell) * (10000 * s->value[CANOPY_COVER_DBHDC]));
	Log("END of Year Yearly Cumulated DEL STEM layer %d  = %f tDM/area year\n", h->z, s->value[DEL_Y_WS]);
	Log("END of Year Yearly Cumulated DEL FOLIAGE layer %d  = %f tDM/area year\n", h->z, s->value[DEL_Y_WF]);
	Log("END of Year Yearly Cumulated DEL FINE ROOT layer %d  = %f tDM/area year\n", h->z, s->value[DEL_Y_WFR]);
	Log("END of Year Yearly Cumulated DEL COARSE ROOT layer %d  = %f tDM/area year\n", h->z, s->value[DEL_Y_WCR]);
	Log("END of Year Yearly Cumulated DEL RESERVE layer %d  = %f tDM/area year\n", h->z, s->value[DEL_Y_WRES]);
	Log("END of Year Yearly Cumulated DEL RESERVE layer %d  = %f KgC tree year\n", h->z, (s->value[DEL_Y_WRES]*2000)/s->counter[N_TREE]);
	Log("END of Year Yearly Cumulated DEL BB layer %d  = %f tDM/area year\n", h->z, s->value[DEL_Y_BB]);
	Log("END of Year Yearly Cumulated DEL TOT ROOT layer %d  = %f tDM/area year\n", h->z, s->value[DEL_Y_WR]);

	if (s->value[DEL_Y_WS] + s->value[DEL_Y_WR] + s->value[DEL_Y_WF] + s->value[DEL_Y_WRES] + s->value[DEL_Y_BB] != s->value[YEARLY_NPP])
	{
		Log("ATTENTION SUM OF ALL INCREMENTS DIFFERENT FROM YEARLY NPP \n");
		Log("DEL SUM = %f \n", s->value[DEL_Y_WS] + s->value[DEL_Y_WCR] + s->value[DEL_Y_WFR] + s->value[DEL_Y_BB] + s->value[DEL_Y_WF] + s->value[DEL_Y_WRES]);
	}
}


extern void Get_EOY_cumulative_balance_cell_level (CELL *c, const YOS *const yos, int years, int years_of_simulation)
{
	static double avg_gpp[3], avg_npp[3], avg_ce[3], avg_gpp_tot, avg_npp_tot, avg_ce_tot;
	static double avg_ar[3], avg_ar_tot;
	static double avg_cf[3], avg_cf_tot;
	static int tot_dead_tree_tot;

	static int previous_layer_number;


	if (years == 0)
	{
		Annual_Log("Site name = %s\n", site->sitename);
		Annual_Log("Annual summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",settings->version, settings->time, settings->spatial);
		Annual_Log("years of simulation = %d\n", years_of_simulation);
		Annual_Log("\n\nCell %d, %d, Lat = %f, Long  = %f\n\n\n", c->x, c->y, site->lat, site->lon );
		Annual_Log("HC(n) = height class counter for n layer\n");
		Annual_Log("Annual GPP = annual total gross primary production (gC/m2/year)\n");
		Annual_Log("Annual AR = annual total autotrophic respiration (gC/m2/year)\n");
		Annual_Log("Annual Cf = annual c-fluxes (gC/m2/year)\n");
		Annual_Log("Annual Y = NPP/GPP ratio (%)\n");
		Annual_Log("Annual NPP = annual total net primary production (tDM/m2/year)\n");
		Annual_Log("Annual CE = annual canopy evapotranspiration(mm/year)\n");
		Annual_Log("Annual ASW = end of year annual available soil water(mm)\n");
		Annual_Log("Annual Wf = annual water-fluxes (mm/year)\n");
		Annual_Log("Annual PEAK_LAI = annual Peak Lai (m^2/m^2)\n");
		Annual_Log("Annual Dead tree = annual dead tree (n tree/cell)\n\n\n");
	}

	//reset
	if (years == 0)
	{
		previous_layer_number = c->annual_layer_number;

		avg_gpp_tot = 0;
		avg_ar_tot = 0;
		avg_cf_tot = 0;
		avg_npp_tot = 0;
		avg_ce_tot = 0;
		tot_dead_tree_tot = 0;

		if (c->annual_layer_number == 1)
		{
			avg_gpp[0] = 0;
			avg_ar[0] = 0;
			avg_cf[0] = 0;
			avg_npp[0] = 0;
			avg_ce[0] = 0;
		}
		if (c->annual_layer_number == 2)
		{
			avg_gpp[1] = 0;
			avg_ar[1] = 0;
			avg_cf[1] = 0;
			avg_npp[1] = 0;
			avg_ce[1] = 0;
			avg_gpp[0] = 0;
			avg_ar[0] = 0;
			avg_cf[0] = 0;
			avg_npp[0] = 0;
			avg_ce[0] = 0;
		}
		if (c->annual_layer_number == 3)
		{
			avg_gpp[2] = 0;
			avg_ar[2] = 0;
			avg_cf[2] = 0;
			avg_npp[2] = 0;
			avg_ce[2] = 0;
			avg_gpp[1] = 0;
			avg_ar[1] = 0;
			avg_cf[1] = 0;
			avg_npp[1] = 0;
			avg_ce[1] = 0;
			avg_gpp[0] = 0;
			avg_ar[0] = 0;
			avg_cf[0] = 0;
			avg_npp[0] = 0;
			avg_ce[0] = 0;
		}
	}
	else
	{
		//check if layer number is changed since last yearly run
		if(previous_layer_number != c->annual_layer_number)
		{
			Annual_Log ("\n\nANNUAL_LAYER_NUMBER_IS_CHANGED_SINCE_PREVIOUS_YEAR!!!\n\n");
		}
	}

	//Annual_Log ("-%d %10f %10f %10f %10f %10f %10f %10d\n", yos[years].year, c->annual_gpp, c->annual_npp, c->annual_et, c->stand_agb, c->stand_bgb, c->annual_peak_lai, c->dead_tree);


	if (c->annual_layer_number == 1)
	{
		if (years == 0 || previous_layer_number != c->annual_layer_number)
		{
			Annual_Log ("\n%s \t%2s \t%6s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%8s \t%8s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n",
					"YEAR", "HC(0)", "GPP (tot)", "AR (tot)", "Cf(tot)", "Y(%tot)", "NPP(tot)", "NPP(gC/m2yr)", "CE(tot)", "ASW", "Wf(tot)", "PEAK_LAI",
					"CC(tot)", "DEAD TREE(tot)", "wf", "ws", "wbb", "wfr", "wcr", "Wres", "DELTA-Wres");
		}
		Annual_Log ("%d \t%2d \t%10.2f \t%10.2f \t%10.2f \t%10.2f\t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%12.2f \t%12.2f \t%14.2d \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f\n",
				yos[years].year,
				c->height_class_in_layer_dominant_counter,
				c->annual_tot_gpp,
				c->annual_tot_aut_resp,
				c->annual_tot_c_flux,
				((c->annual_tot_aut_resp * 100.0)/c->annual_tot_gpp),
				c->annual_tot_npp,
				c->annual_tot_npp_g_c,
				c->annual_tot_c_evapotransp ,
				c->available_soil_water,
				c->annual_tot_w_flux,
				c->annual_peak_lai[0],
				c->annual_cc[0],
				c->annual_tot_dead_tree,
				c->annual_wf[0],
				c->annual_ws[0],
				c->annual_wbb[0],
				c->annual_wfr[0],
				c->annual_wcr[0],
				c->annual_wres[0],
				c->annual_delta_wres[0]);

		previous_layer_number = c->annual_layer_number;

		//compute average or total
		avg_gpp[0] += c->annual_gpp[0];
		avg_ar[0] += c->annual_aut_resp[0];
		avg_cf[0] += c->annual_c_flux[0];
		avg_npp[0] += c->annual_npp[0];
		avg_ce[0] += c->annual_c_evapotransp[0];
		avg_gpp_tot += c->annual_gpp[0];
		avg_ar_tot += c->annual_aut_resp[0];
		avg_cf_tot += c->annual_c_flux[0];
		avg_npp_tot += c->annual_npp[0];
		avg_ce_tot += c->annual_c_evapotransp[0];
		tot_dead_tree_tot += c->annual_tot_dead_tree;

		//reset
		c->annual_gpp[0] = 0;
		c->annual_aut_resp[0] = 0;
		c->annual_c_flux[0] = 0;
		c->annual_npp[0] = 0;
		c->annual_c_evapotransp[0] = 0;
		c->annual_cc[0] = 0;
		c->annual_dead_tree[0] = 0;
		c->annual_peak_lai[0] = 0;
		c->annual_wres[0] = 0;
		c->annual_delta_ws[0] = 0;
		c->annual_delta_wres[0] = 0;
		c->annual_wf[0]= 0;
		c->annual_ws[0]= 0;
		c->annual_wbb[0]= 0;
		c->annual_wfr[0]= 0;
		c->annual_wcr[0]= 0;
	}
	if (c->annual_layer_number == 2)
	{
		if (years == 0 || previous_layer_number != c->annual_layer_number)
		{
			Annual_Log ("\n%s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%7s \t%4s \t%4s \t%4s \t%4s \t%4s\n\n",
					"YEAR", "HC(1)", "HC(0)", "GPP(1)", "GPP(0)", "GPP(tot)", "AR(1)", "AR(0)", "AR(tot)", "Cf(1)", "Cf(0)", "Cf(tot)", "Y(1)", "Y(0)", "Y(tot)", "NPP(1)", "NPP(0)", "NPP (tot)", "NPP(gC/m2yr)", "CE(1)", "CE(0)", "CE (tot)", "ASW"
					"Wf", "PEAK_LAI(1)", "PEAK_LAI(0)", "CC(1)", "CC(0)", "DEAD TREE(1)", "DEAD TREE(0)", "DEAD TREE(tot)",
					"Wres(1)", "Wres(0)");
		}
		Annual_Log ("%d \t%4d \t%4d \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f"
				" \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%12d \t%12d \t%12d \t%4.2f \t%4.2f\n",
				yos[years].year,
				c->height_class_in_layer_dominant_counter, c->height_class_in_layer_dominated_counter,
				c->annual_gpp[1],c->annual_gpp[0], c->annual_tot_gpp,
				c->annual_aut_resp[1],c->annual_aut_resp[0], c->annual_tot_aut_resp,
				c->annual_c_flux[1], c->annual_c_flux[0], c->annual_tot_c_flux,
				(c->annual_aut_resp[1]*100)/ c->annual_gpp[1],((c->annual_aut_resp[0]*100)/ c->annual_gpp[0]),(c->annual_tot_aut_resp*100)/c->annual_tot_gpp,
				c->annual_npp[1], c->annual_npp[0],	c->annual_tot_npp, c->annual_tot_npp_g_c,
				c->annual_c_evapotransp[1], c->annual_c_evapotransp[0], c->annual_tot_c_evapotransp,
				c->available_soil_water,
				c->annual_tot_w_flux,
				c->annual_peak_lai[1], c->annual_peak_lai[0],
				c->annual_cc[1], c->annual_cc[0],
				c->annual_dead_tree[1], c->annual_dead_tree[0], c->annual_tot_dead_tree,
				c->annual_wres[1], c->annual_wres[0]);

		previous_layer_number = c->annual_layer_number;

		//compute average
		avg_gpp[1] += c->annual_gpp[1];
		avg_ar[1] += c->annual_aut_resp[1];
		avg_cf[1] += c->annual_c_flux[1];
		avg_npp[1] += c->annual_npp[1];
		avg_ce[1] += c->annual_c_evapotransp[1];
		avg_gpp[0] += c->annual_gpp[0];
		avg_ar[0] += c->annual_aut_resp[0];
		avg_cf[0] += c->annual_c_flux[0];
		avg_npp[0] += c->annual_npp[0];
		avg_ce[0] += c->annual_c_evapotransp[0];
		avg_gpp_tot += c->annual_gpp[1] + c->annual_gpp[0];
		avg_ar_tot += c->annual_aut_resp[1] + c->annual_aut_resp[0];
		avg_npp_tot += c->annual_npp[1] + c->annual_npp[0];
		avg_ce_tot += c->annual_c_evapotransp[1] + c->annual_c_evapotransp[0];
		tot_dead_tree_tot += c->annual_tot_dead_tree;


		//reset
		c->annual_gpp[1] = 0;
		c->annual_aut_resp[1] = 0;
		c->annual_c_flux[1] = 0;
		c->annual_npp[1] = 0;
		c->annual_c_evapotransp[1] = 0;
		c->annual_cc[1] = 0;
		c->annual_dead_tree[1] = 0;
		c->annual_delta_ws[1] = 0;
		c->annual_ws[1] = 0;
		c->annual_wres[1] = 0;

		c->annual_gpp[0] = 0;
		c->annual_aut_resp[0] = 0;
		c->annual_c_flux[0] = 0;
		c->annual_npp[0] = 0;
		c->annual_c_evapotransp[0] = 0;
		c->annual_cc[0] = 0;
		c->annual_dead_tree[0] = 0;
		c->annual_delta_ws[0] = 0;
		c->annual_ws[0] = 0;
		c->annual_wres[0] = 0;
		c->annual_peak_lai[1] = 0;
		c->annual_peak_lai[0] = 0;
	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 3)
	{
		if (years == 0 || previous_layer_number != c->annual_layer_number)
		{
			Annual_Log ("\n%s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s\t%2s \t%s \t%s \t%s \t%s \t%2s \t%2s \t%2s \t%2s \t%2s "
					"\t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s\n\n",
					"YEAR", "HC(2)", "HC(1)", "HC(0)", "GPP(2)","GPP(1)", "GPP(0)", "GPP (tot)", "AR(2)", "AR(1)", "AR(0)", "AR(tot)",
					"Cf(2)", "Cf(1)", "Cf(0)", "Cf(tot)", "Y(2)", "Y(1)", "Y(0)", "Y(tot)","NPP(2)", "NPP(1)", "NPP(0)", "NPP (tot)",
					"CE(2)", "CE(1)", "CE(0)", 	"CE(tot)", "ASW", "Wf",  "PEAK_LAI(2)", "PEAK_LAI(1)", "PEAK_LAI(0)",
					"CC(2)", "CC(1)", "CC(0)", "DEAD TREE(2)","DEAD TREE(1)",
					"DEAD TREE(0)", "DEAD TREE(tot)", "Wres(2)", "Wres(1)", "Wres(0)");
		}
		Annual_Log ("%d \t%2d \t%2d \t%2d \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f "
				" \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2d \t%2d \t%2d \t%2d "
				" \t%5.2f \t%5.2f \t%5.2f \n",
				yos[years].year,
				c->height_class_in_layer_dominant_counter, c->height_class_in_layer_dominated_counter, c->height_class_in_layer_subdominated_counter,
				c->annual_gpp[2], c->annual_gpp[1],c->annual_gpp[0], c->annual_tot_gpp,
				c->annual_aut_resp[2], c->annual_aut_resp[1],c->annual_aut_resp[0], c->annual_tot_aut_resp,
				c->annual_c_flux[2], c->annual_c_flux[1], c->annual_c_flux[0], c->annual_tot_c_flux,
				(c->annual_aut_resp[2]*100.0)/c->annual_gpp[2],((c->annual_aut_resp[1]*100.0)/c->annual_gpp[1]),((c->annual_aut_resp[0]*100.0)/c->annual_gpp[0]),(c->annual_tot_aut_resp*100.0)/c->annual_tot_gpp,
				c->annual_npp[2], c->annual_npp[1],	c->annual_npp[0], c->annual_tot_npp,
				c->annual_c_evapotransp[2],c->annual_c_evapotransp[1], c->annual_c_evapotransp[0], c->annual_tot_c_evapotransp,
				c->available_soil_water, c->annual_tot_w_flux,
				c->annual_peak_lai[2], c->annual_peak_lai[1], c->annual_peak_lai[0],
				c->annual_cc[2],c->annual_cc[1], c->annual_cc[0],
				c->annual_dead_tree[2], c->annual_dead_tree[1], c->annual_dead_tree[0], c->annual_tot_dead_tree,
				c->annual_wres[2], c->annual_wres[1], c->annual_wres[0]);

		previous_layer_number = c->annual_layer_number;

		//compute average
		avg_gpp[2] += c->annual_gpp[2];
		avg_ar[2] += c->annual_aut_resp[2];
		avg_cf[2] += c->annual_c_flux[2];
		avg_npp[2] += c->annual_npp[2];
		avg_ce[2] += c->annual_c_evapotransp[2];
		avg_gpp[1] += c->annual_gpp[1];
		avg_ar[1] += c->annual_aut_resp[1];
		avg_cf[1] += c->annual_c_flux[1];
		avg_npp[1] += c->annual_npp[1];
		avg_ce[1] += c->annual_c_evapotransp[1];
		avg_gpp[0] += c->annual_gpp[0];
		avg_ar[0] += c->annual_aut_resp[0];
		avg_cf[0] += c->annual_c_flux[0];
		avg_npp[0] += c->annual_npp[0];
		avg_ce[0] += c->annual_c_evapotransp[0];
		avg_gpp_tot += c->annual_gpp[2] +c->annual_gpp[1] + c->annual_gpp[0];
		avg_ar_tot += c->annual_aut_resp[2] +c->annual_aut_resp[1] + c->annual_aut_resp[0];
		avg_cf_tot += c->annual_c_flux[2] +c->annual_c_flux[1] + c->annual_c_flux[0];
		avg_npp_tot += c->annual_npp[2] +c->annual_npp[1] + c->annual_npp[0];
		avg_ce_tot += c->annual_c_evapotransp[2] + c->annual_c_evapotransp[1] + c->annual_c_evapotransp[0];
		tot_dead_tree_tot += c->annual_tot_dead_tree;



		//reset
		c->annual_gpp[2] = 0;
		c->annual_aut_resp[2] = 0;
		c->annual_c_flux[2] = 0;
		c->annual_npp[2] = 0;
		c->annual_c_evapotransp[2] = 0;
		c->annual_cc[2] = 0;
		c->annual_dead_tree[2] = 0;
		c->annual_delta_ws[2] = 0;
		c->annual_ws[2] = 0;
		c->annual_wres[2] = 0;

		c->annual_gpp[1] = 0;
		c->annual_aut_resp[1] = 0;
		c->annual_c_flux[1] = 0;
		c->annual_npp[1] = 0;
		c->annual_c_evapotransp[1] = 0;
		c->annual_cc[1] = 0;
		c->annual_dead_tree[1] = 0;
		c->annual_delta_ws[1] = 0;
		c->annual_ws[1] = 0;
		c->annual_wres[1] = 0;

		c->annual_gpp[0] = 0;
		c->annual_aut_resp[0] = 0;
		c->annual_c_flux[0] = 0;
		c->annual_npp[0] = 0;
		c->annual_c_evapotransp[0] = 0;
		c->annual_cc[0] = 0;
		c->annual_dead_tree[0] = 0;
		c->annual_delta_ws[0] = 0;
		c->annual_ws[0] = 0;
		c->annual_wres[0] = 0;
		c->annual_peak_lai[2] = 0;
		c->annual_peak_lai[1] = 0;
		c->annual_peak_lai[0] = 0;

	}
	c->annual_tot_gpp = 0;
	c->annual_tot_aut_resp = 0;
	c->annual_tot_c_flux = 0;
	c->annual_tot_npp = 0;
	c->annual_tot_npp_g_c = 0;
	c->annual_tot_c_evapotransp = 0;
	c->annual_tot_w_flux = 0;
	c->annual_tot_dead_tree = 0;

	//compute average values
	if (years == years_of_simulation -1 && years_of_simulation > 1)
	{
		if (c->annual_layer_number == 1)
		{
			avg_gpp[0] /= years_of_simulation;
			avg_ar[0] /= years_of_simulation;
			avg_cf[0] /= years_of_simulation;
			avg_npp[0] /= years_of_simulation;
			avg_ce[0] /= years_of_simulation;
			avg_gpp_tot /= years_of_simulation;
			avg_ar_tot /= years_of_simulation;
			avg_cf_tot /= years_of_simulation;
			avg_npp_tot /= years_of_simulation;
			avg_ce_tot /= years_of_simulation;
			Annual_Log ("-----------------------------------------------------------------------------------------------------------------------------------------------------"
					"---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			Annual_Log ("AVG/TOT \t\t%5f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t\t\t\t%49d\n",
					   avg_gpp_tot, avg_ar_tot, avg_cf_tot, (avg_ar_tot*100.0)/avg_gpp_tot, avg_npp_tot, ((avg_npp_tot/settings->sizeCell)*1000000)/GC_GDM , avg_ce_tot, tot_dead_tree_tot);
		}
		if (c->annual_layer_number == 2)
		{
			avg_gpp[1] /= years_of_simulation;
			avg_ar[1] /= years_of_simulation;
			avg_cf[1] /= years_of_simulation;
			avg_npp[1] /= years_of_simulation;
			avg_ce[1] /= years_of_simulation;
			avg_gpp[0] /= years_of_simulation;
			avg_ar[0] /= years_of_simulation;
			avg_cf[0] /= years_of_simulation;
			avg_npp[0] /= years_of_simulation;
			avg_ce[0] /= years_of_simulation;
			avg_gpp_tot /= years_of_simulation;
			avg_ar_tot /= years_of_simulation;
			avg_cf_tot /= years_of_simulation;
			avg_npp_tot /= years_of_simulation;
			avg_ce_tot /= years_of_simulation;
			Annual_Log ("-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			Annual_Log ("AVG/TOT \t\t\t%14f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t\t\t\t\t\t\t%76d \n",
					 avg_gpp[1], avg_gpp[0], avg_gpp_tot, avg_ar[1], avg_ar[0], avg_ar_tot, avg_cf[1], avg_cf[0], avg_cf_tot, (avg_ar[1]*100.0)/avg_gpp[1],
					 ((avg_ar[0]*100.0)/avg_gpp[0]),(avg_ar_tot*100.0)/avg_gpp_tot,avg_npp[1], avg_npp[0], avg_npp_tot, avg_ce[1], avg_ce[0], avg_ce_tot, tot_dead_tree_tot);
		}
		if (c->annual_layer_number == 3)
		{
			avg_gpp[2] /= years_of_simulation;
			avg_ar[2] /= years_of_simulation;
			avg_cf[2] /= years_of_simulation;
			avg_npp[2] /= years_of_simulation;
			avg_ce[2] /= years_of_simulation;
			avg_gpp[1] /= years_of_simulation;
			avg_ar[1] /= years_of_simulation;
			avg_cf[1] /= years_of_simulation;
			avg_npp[1] /= years_of_simulation;
			avg_ce[1] /= years_of_simulation;
			avg_gpp[0] /= years_of_simulation;
			avg_ar[0] /= years_of_simulation;
			avg_cf[0] /= years_of_simulation;
			avg_npp[0] /= years_of_simulation;
			avg_ce[0] /= years_of_simulation;
			avg_gpp_tot /= years_of_simulation;
			avg_ar_tot /= years_of_simulation;
			avg_cf_tot /= years_of_simulation;
			avg_npp_tot /= years_of_simulation;
			avg_ce_tot /= years_of_simulation;

			Annual_Log ("----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			Annual_Log ("AVG/TOT \t\t\t\t%17f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t\t\t\t\t\t\t\t\t%42d \n",
					avg_gpp[2], avg_gpp[1], avg_gpp[0], avg_gpp_tot, avg_ar[2], avg_ar[1], avg_ar[0], avg_ar_tot,
					avg_cf[2], avg_cf[1], avg_cf[0], avg_cf_tot,
					(avg_ar[2]*100.0)/avg_gpp[2], (avg_ar[1]*100.0)/avg_gpp[1], (avg_ar[0]*100.0)/avg_gpp[0], (avg_ar_tot*100.0)/avg_gpp_tot,
					avg_npp[2], avg_npp[1], avg_npp[0], avg_npp_tot, avg_ce[2], avg_ce[1], avg_ce[0], avg_ce_tot, tot_dead_tree_tot);		//
			//}
		}
	}
}

extern void Get_EOM_cumulative_balance_cell_level (CELL *c, const YOS *const yos, int years, int month)
{
	static int previous_layer_number;

	if(month == 0 && years == 0)
	{
		Monthly_Log("Site name = %s\n", site->sitename);
		Monthly_Log("Monthly summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",settings->version, settings->time, settings->spatial);
		Monthly_Log("\n\nCell %d, %d, Lat = %f, Long  = %f\n\n\n", c->x, c->y, site->lat, site->lon );
		Monthly_Log("Monthly GPP = monthly total gross primary production (gC/m2/month)\n");
		Monthly_Log("Monthly AR = monthly total autotrophic respiration (gC/m2/month)\n");
		Monthly_Log("Monthly Cf = monthly c-fluxes (gC/m2/month)\n");
		Monthly_Log("Monthly NPP = monthly total net primary production (tDM/m2/month)\n");
		Monthly_Log("Monthly CE = monthly canopy evapotranspiration(mm/month)\n");
		Monthly_Log("Monthly ASW = monthly ASW (at the end of month) (mm)\n");
		Monthly_Log("Monthly Cw = monthly w-fluxes (mm/m2/month)\n");
		Monthly_Log("Monthly DEAD TREE = monthly dead tree (n tree/cell)\n\n\n");
	}
	if (years == 0)
	{
		previous_layer_number = c->annual_layer_number;
	}
	else
	{
		//check if layer number is changed since last yearly run
		if(previous_layer_number != c->annual_layer_number)
		{
			Monthly_Log ("\n\nANNUAL_LAYER_NUMBER_IS_CHANGED_SINCE_PREVIOUS_YEAR!!!\n\n");
		}
		previous_layer_number = c->annual_layer_number;
	}

	if (c->annual_layer_number == 1)
	{
		if (month == 0 && years == 0)
		{
			Monthly_Log ("\n%s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n\n",
					"YEAR", "MONTH", "GPP (tot)", "AR(tot)", "Cf(tot)", "NPP(tot)", "NPPgC", "CE(tot)", "ASW",  "Cw", "CC(0)", "DEAD TREE(tot)");
		}
		Monthly_Log ("%d \t%10d \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10d \n",
				yos[years].year, month+1,
				c->monthly_tot_gpp,
				c->monthly_tot_aut_resp,
				c->monthly_tot_c_flux,
				c->monthly_tot_npp,
				c->monthly_tot_npp_g_c,
				c->monthly_tot_c_evapotransp,
				c->available_soil_water,
				c->monthly_tot_w_flux,
				c->monthly_cc[0],
				c->monthly_tot_dead_tree);

		//reset
		c->monthly_gpp[0] = 0;
		c->monthly_aut_resp[0] = 0;
		c->monthly_npp[0] = 0;
		c->monthly_c_evapotransp[0] = 0;
		c->monthly_cc[0] = 0;
		c->monthly_dead_tree[0] = 0;
	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 2)
	{
		if (month == 0 && years == 0)
		{
			Monthly_Log ("\n%s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s \t%5s\n\n",
					"YEAR", "MONTH", "GPP(1)", "GPP(0)", "GPP(tot)",  "AR(1)", "AR(0)", "AR(tot)", "Cf(1)", "Cf(0)", "Cf(tot)", "NPP(1)", "NPP(0)", "NPP(tot)", "CE(1)",
					"CE(0)", "CE(tot)", "ASW", "Cw", "CC(1)", "CC(0)", "DEAD TREE(1)", "DEAD TREE(0)", "DEAD TREE(tot)");
		}
		Monthly_Log ("%d \t%5d \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5f \t%5d \t%5d \t%5d\n",
				yos[years].year, month+1,
				c->monthly_gpp[1],c->monthly_gpp[0], c->monthly_tot_gpp,
				c->monthly_aut_resp[1],c->monthly_aut_resp[0], c->monthly_tot_aut_resp,
				c->monthly_c_flux[1], c->monthly_c_flux[0], c->monthly_tot_c_flux,
				c->monthly_npp[1], c->monthly_npp[0],c->monthly_tot_npp,
				c->monthly_c_evapotransp[1], c->monthly_c_evapotransp[0], c->monthly_tot_c_evapotransp,
				c->available_soil_water,
				c->monthly_tot_w_flux,
				c->monthly_cc[1], c->monthly_cc[0],
				c->monthly_dead_tree[1], c->monthly_dead_tree[0], c->monthly_tot_dead_tree);

		//reset
		c->monthly_gpp[1] = 0;
		c->monthly_aut_resp[1] = 0;
		c->monthly_c_flux[1] = 0;
		c->monthly_npp[1] = 0;
		c->monthly_c_evapotransp[1] = 0;
		c->monthly_cc[1] = 0;
		c->monthly_dead_tree[1] = 0;

		c->monthly_gpp[0] = 0;
		c->monthly_aut_resp[0] = 0;
		c->monthly_c_flux[0] = 0;
		c->monthly_npp[0] = 0;
		c->monthly_c_evapotransp[0] = 0;
		c->monthly_cc[0] = 0;
		c->monthly_dead_tree[0] = 0;
	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 3)
	{
		if (month == 0 && years == 0)
		{
			Monthly_Log ("\n%s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s\n\n",
					"YEAR", "MONTH", "GPP(2)","GPP(1)", "GPP(0)", "GPP (tot)", "AR(2)","AR(1)", "AR(0)", "AR(tot)", "Cf(2)", "Cf(1)", "Cf(0)", "Cf(tot)",
					"NPP(2)","NPP(1)", "NPP(0)","NPP (tot)",
					"CE(2)","CE(1)", "CE(0)", "CE(tot)", "ASW", "Cw", "CC(2)", "CC(1)", "CC(0)", "DEAD TREE(2)","DEAD TREE(1)",
					"DEAD TREE(0)", "DEAD TREE(tot)");
		}
		Monthly_Log ("%d \t%2d \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2f \t%2d \t%2d \t%2d \t%2d\n",
				yos[years].year, month+1,
				c->monthly_gpp[2], c->monthly_gpp[1],c->monthly_gpp[0], c->monthly_tot_gpp,
				c->monthly_aut_resp[2], c->monthly_aut_resp[1],c->monthly_aut_resp[0], c->monthly_tot_aut_resp,
				c->monthly_c_flux[2],c->monthly_c_flux[1], c->monthly_c_flux[0], c->monthly_tot_c_flux,
				c->monthly_npp[2], c->monthly_npp[1], c->monthly_npp[0], c->monthly_tot_npp,
				c->monthly_c_evapotransp[2],c->monthly_c_evapotransp[1], c->monthly_c_evapotransp[0], c->monthly_tot_c_evapotransp,
				c->available_soil_water,
				c->monthly_tot_w_flux,
				c->monthly_cc[2], c->monthly_cc[1], c->monthly_cc[0],
				c->monthly_dead_tree[2], c->monthly_dead_tree[1], c->monthly_dead_tree[0], c->monthly_tot_dead_tree);

		//reset
		c->monthly_gpp[2] = 0;
		c->monthly_aut_resp[2] = 0;
		c->monthly_c_flux[2] = 0;
		c->monthly_npp[2] = 0;
		c->monthly_c_evapotransp[2] = 0;
		c->monthly_cc[2] = 0;
		c->monthly_dead_tree[2] = 0;

		c->monthly_gpp[1] = 0;
		c->monthly_aut_resp[1] = 0;
		c->monthly_c_flux[1] = 0;
		c->monthly_npp[1] = 0;
		c->monthly_c_evapotransp[1] = 0;
		c->monthly_cc[1] = 0;
		c->monthly_dead_tree[1] = 0;

		c->monthly_gpp[0] = 0;
		c->monthly_aut_resp[0] = 0;
		c->monthly_c_flux[0] = 0;
		c->monthly_npp[0] = 0;
		c->monthly_c_evapotransp[0] = 0;
		c->monthly_cc[0] = 0;
		c->monthly_dead_tree[0] = 0;
	}
	c->monthly_tot_gpp = 0;
	c->monthly_tot_aut_resp = 0;
	c->monthly_tot_c_flux = 0;
	c->monthly_tot_npp = 0;
	c->monthly_tot_npp_g_c = 0;
	c->monthly_tot_c_evapotransp = 0;
	c->monthly_tot_w_flux = 0;
	c->monthly_tot_dead_tree = 0;
}



extern void Get_EOD_cumulative_balance_cell_level (CELL *c, const YOS *const yos, int years, int month, int day )
{

	static int previous_layer_number;
	static int doy;

	if(day  == 0 && month == 0 && years == 0)
	{
		Daily_Log("Site name = %s\n", site->sitename);
		Daily_Log("Daily summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",settings->version, settings->time, settings->spatial);
		Daily_Log("\n\nCell %d, %d, Lat = %f, Long  = %f\n\n\n", c->x, c->y, site->lat, site->lon );


		Daily_Log ("HC\n");

		Daily_Log("Daily GPP = daily total gross primary production (gC/m2/day)\n");
		Daily_Log("Daily AR = daily total autotrophic respiration (gC/m2/day)\n");
		Daily_Log("Daily ARtDM = daily total autotrophic respiration (tDM/day cell)\n");
		Daily_Log("Daily Cf = daily c-fluxes (gC/m2/day)\n");
		Daily_Log("Daily CftDM = daily c-fluxes (tDM/day cell)\n");
		Daily_Log("Daily NPP = daily total net primary production (tDM/m2/day)\n");
		Daily_Log("Daily CE = daily canopy evapotranspiration(mm/day)\n");
		Daily_Log("Daily ASW = daily Available Soil Water(mm/day)\n");
		Daily_Log("Daily Wfl = daily water fluxes (mm/day)\n");
		Daily_Log("Daily LAI = daily Leaf Area Index (m^2/m^2)\n");
		Daily_Log("Daily D-Wf = daily fraction of NPP to foliage pool (tDM/day cell)\n");
		Daily_Log("Daily D-Ws = daily fraction of NPP to stem pool (tDM/day cell)\n");
		Daily_Log("Daily D-Wbb = daily fraction of NPP to branch and bark pool (tDM/day cell)\n");
		Daily_Log("Daily D-Wfr = daily fraction of NPP to fine root pool (tDM/day cell)\n");
		Daily_Log("Daily D-Wcr = daily fraction of NPP to coarse root pool (tDM/day cell)\n");
		Daily_Log("Daily D-Wres = daily fraction of NPP to reserve pool (tDM/day cell)\n");

	}

	if (years == 0)
	{
		previous_layer_number = c->annual_layer_number;
	}
	else
	{
		//check if layer number is changed since last yearly run
		if(previous_layer_number != c->annual_layer_number)
		{
			Daily_Log ("\n\nANNUAL_LAYER_NUMBER_IS_CHANGED_SINCE_PREVIOUS_YEAR!!!\n\n");
		}
	}
	if (c->annual_layer_number == 1)
	{
		if ((day == 0 && month == 0 && years == 0) || previous_layer_number != c->annual_layer_number)
		{
			Daily_Log ("\n%s \t%s \t%2s \t%2s \t%2s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s\n",
					"DOY", "YEAR", "MONTH", "DAY", "HC","GPP(0)", "AR(0)", "ARtDM(0)", "Cf", "CftDM", "NPP(0)", "NPPgC", "CE(0)", "ASW", "Wfl", "LAI(0)", "CC(0)", "DEADTREE(0)", "D-Wf", "D-Ws", "D-Wbb", "D-Wfr", "D-Wcr", "D-Wres", "Wres");
		}
		if ((day == 0 && month == 0) || previous_layer_number != c->annual_layer_number)
		{
			doy = 1;
		}
		Daily_Log ("%d \t%8d \t%5d \t%3d \t%2d \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f "
				"\t%8.6f \t%8d \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.9f \t%8.9f\n",
				doy++, yos[years].year, month+1, day+1,
				c->height_class_in_layer_dominant_counter,
				c->daily_gpp[0],
				c->daily_aut_resp[0],
				c->daily_aut_resp_tDM[0],
				c->daily_c_flux[0],
				c->daily_c_flux_tDM[0],
				c->daily_npp[0],
				c->daily_npp_g_c[0],
				c->daily_c_evapotransp[0],
				c->available_soil_water,
				c->daily_tot_w_flux,
				c->daily_lai[0],
				c->daily_cc[0]*100,
				c->daily_dead_tree[0],
				c->daily_delta_wf[0],
				c->daily_delta_ws[0],
				c->daily_delta_wbb[0],
				c->daily_delta_wfr[0],
				c->daily_delta_wcr[0],
				c->daily_delta_wres[0],
				c->daily_wres[0]);


		previous_layer_number = c->annual_layer_number;

		//reset
		c->daily_gpp[0] = 0;
		c->daily_aut_resp[0] = 0;
		c->daily_aut_resp_tDM[0] = 0;
		c->daily_c_flux_tDM[0] = 0;
		c->daily_npp[0] = 0;
		c->daily_npp_g_c[0]= 0;
		c->daily_c_int[0] = 0;
		c->daily_c_transp[0] = 0;
		c->daily_c_evapotransp[0] = 0;
		c->daily_et[0] = 0;
		c->daily_cc[0] = 0;
		c->daily_dead_tree[0] = 0;
	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 2)
	{
		if ((day == 0 && month == 0 && years == 0) || previous_layer_number != c->annual_layer_number)
		{
			Daily_Log ("\n%s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s  \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s\n\n",
					"YEAR", "MONTH", "DAY", "HC", "GPP(1)", "GPP(0)", "GPP(tot)", "AR(1)", "AR", "AR(tot)", "Cf(1)", "Cf(0)", "Cf(tot)", "NPP(1)", "NPP(0)", "NPP(tot)","CE(1)", "CE(0)", "CE(tot)","ASW",
					"LAI(1)", "LAI(0)", "CC(1)", "CC(0)", "DEADTREE(1)", "DEADTREE(0)", "DEADTREE(tot)");
			}
		Daily_Log ("%d \t%4d \t%4d /t%4d \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4.3f \t%4d \t%4d \t%4d\n",
				yos[years].year, month+1, day+1,
				c->height_class_in_layer_dominant_counter,
				c->daily_gpp[1], c->daily_gpp[0], c->daily_tot_gpp,
				c->daily_aut_resp[1], c->daily_aut_resp[0], c->daily_tot_aut_resp,
				c->daily_c_flux[1], c->daily_c_flux[0], c->daily_tot_c_flux,
				c->daily_npp[1], c->daily_npp[0], c->daily_tot_npp,
				c->daily_c_evapotransp[1], c->daily_c_evapotransp[0], c->daily_tot_c_evapotransp,
				c->available_soil_water,
				c->daily_lai[1], c->daily_lai[0],
				c->daily_cc[1]*100, c->daily_cc[0]*100,
				c->daily_dead_tree[1], c->daily_dead_tree[0], c->daily_tot_dead_tree);

		previous_layer_number = c->annual_layer_number;

		//reset
		c->daily_gpp[1] = 0;
		c->daily_aut_resp[1] = 0;
		c->daily_c_flux[1] = 0;
		c->daily_npp[1] = 0;
		c->daily_c_int[1] = 0;
		c->daily_c_transp[1] = 0;
		c->daily_c_evapotransp[1] = 0;
		c->daily_et[1] = 0;
		c->daily_cc[1] = 0;
		c->daily_dead_tree[1] = 0;

		c->daily_gpp[0] = 0;
		c->daily_aut_resp[0] = 0;
		c->daily_c_flux[0] = 0;
		c->daily_npp[0] = 0;
		c->daily_c_int[0] = 0;
		c->daily_c_transp[0] = 0;
		c->daily_c_evapotransp[0] = 0;
		c->daily_et[0] = 0;
		c->daily_cc[0] = 0;
		c->daily_dead_tree[0] = 0;

	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 3)
	{
		if ((day == 0 && month == 0 && years == 0) || previous_layer_number != c->annual_layer_number)
		{
			Daily_Log ("\n\t%s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s\n\n",
					"YEAR", "MONTH", "DAY", "GPP(2)","GPP(1)", "GPP(0)", "GPP(tot)", "AR(2)","AR(1)", "AR(0)", "AR(tot)", "Cf(2)", "Cf(1)", "Cf(0)", "Cf(tot)", "NPP(2)","NPP(1)", "NPP(0)", "NPP(tot)", "NPPgC(2)","NPPgC(1)", "NPPgC(0)", "NPPgC(tot)",
					"CE(2)","CE(1)", "CE(0)", "CE(tot)", "ASW",
					"LAI(2)","LAI(1)", "LAI(0)", "CC(2)", "CC(1)", "CC(0)", "DEADTREE(2)", "DEADTREE(1)", "DEADTREE(0)", "DEADTREE(tot)");
		}
		Daily_Log ("\t%d \t%2d \t%2d \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2.3f \t%2d \t%2d \t%2d \t%2d\n",
				yos[years].year, month+1, day+1,
				c->daily_gpp[2], c->daily_gpp[1],c->daily_gpp[0], c->daily_tot_gpp,
				c->daily_aut_resp[2], c->daily_aut_resp[1],c->daily_aut_resp[0], c->daily_tot_aut_resp,
				c->daily_c_flux[2],c->daily_c_flux[1],c->daily_c_flux[0],c->daily_tot_c_flux,
				c->daily_npp[2], c->daily_npp[1],c->daily_npp[0], c->daily_tot_npp,
				c->daily_npp_g_c[2], c->daily_npp_g_c[1],c->daily_npp_g_c[0], c->daily_tot_npp_g_c,
				c->daily_c_evapotransp[2],c->daily_c_evapotransp[1], c->daily_c_evapotransp[0], c->daily_tot_c_evapotransp,
				c->available_soil_water,
				c->daily_lai[2], c->daily_lai[1], c->daily_lai[0],
				c->daily_cc[2]*100, c->daily_cc[1]*100,c->daily_cc[0]*100,
				c->daily_dead_tree[2], c->daily_dead_tree[1], c->daily_dead_tree[0], c->daily_tot_dead_tree);

		previous_layer_number = c->annual_layer_number;

		//reset
		c->daily_gpp[2] = 0;
		c->daily_aut_resp[2] = 0;
		c->daily_c_flux[2] = 0;
		c->daily_npp[2] = 0;
		c->daily_npp_g_c[2] = 0;
		c->daily_c_int[2] = 0;
		c->daily_c_transp[2] = 0;
		c->daily_c_evapotransp[2] = 0;
		c->daily_et[2] = 0;
		c->daily_cc[2] = 0;
		c->daily_dead_tree[2] = 0;

		c->daily_gpp[1] = 0;
		c->daily_aut_resp[1] = 0;
		c->daily_c_flux[1] = 0;
		c->daily_npp[1] = 0;
		c->daily_npp_g_c[1] = 0;
		c->daily_c_int[1] = 0;
		c->daily_c_transp[1] = 0;
		c->daily_c_evapotransp[1] = 0;
		c->daily_et[1] = 0;
		c->daily_cc[1] = 0;
		c->daily_dead_tree[1] = 0;

		c->daily_gpp[0] = 0;
		c->daily_aut_resp[0] = 0;
		c->daily_c_flux[0] = 0;
		c->daily_npp[0] = 0;
		c->daily_npp_g_c[0] = 0;
		c->daily_c_int[0] = 0;
		c->daily_c_transp[0] = 0;
		c->daily_c_evapotransp[0] = 0;
		c->daily_et[0] = 0;
		c->daily_cc[0] = 0;
		c->daily_dead_tree[0] = 0;



	}

	c->daily_tot_gpp = 0;
	c->daily_tot_aut_resp = 0;
	c->daily_tot_c_flux = 0;
	c->daily_tot_npp = 0;
	c->daily_tot_npp_g_c = 0;
	c->daily_tot_c_int = 0;
	c->daily_tot_c_transp = 0;
	c->daily_tot_c_evapotransp = 0;
	c->daily_tot_et = 0;
	c->daily_tot_dead_tree = 0;


	//reset after printed at the end of the day
	/*
	c->daily_gpp = 0;
	c->daily_npp = 0;
	c->daily_et = 0;
	c->daily_lai = 0;
	 */
	c->daily_f_sw = 0;
	c->daily_f_psi = 0;
	c->daily_f_t = 0;
	c->daily_f_vpd = 0;

}
