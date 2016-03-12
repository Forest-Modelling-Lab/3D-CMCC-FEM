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
//for soil output Marconi Sergio
static FILE *soil_file_log ;

int soil_logInit(char * soil_logFileName)
{
	soil_file_log = fopen(soil_logFileName, "w");

	if ( !soil_file_log ) return 0;

	return 1;
}

void soil_Log(const char *szText, ...)
{
	char szBuffer[BUFFER_SIZE_LOG] = { 0 };
	//va_list pArgList = { 0 };
	va_list pArgList;

	va_start(pArgList, szText);
	vsnprintf(szBuffer, BUFFER_SIZE_LOG, szText, pArgList);
	va_end(pArgList);

	fputs(szBuffer, stdout);

	if ( log_enabled && soil_file_log )
		fputs(szBuffer, soil_file_log);
}

void soil_logClose(void)
{
	if ( soil_file_log )	fclose(soil_file_log);
}

extern void EOY_cumulative_balance_layer_level (SPECIES *s, HEIGHT *h)
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


extern void EOY_cumulative_balance_cell_level (CELL *c, const YOS *const yos, int years, int years_of_simulation)
{
	static double avg_gpp[3], avg_npp[3], avg_ce[3], avg_gpp_tot, avg_npp_tot, avg_npp_tot_gC, avg_ce_tot;
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
		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Annual_Log("Annual NEE = annual total net ecosystem exchange (gC/m2/year)\n");
		}
		Annual_Log("Annual GPP = annual total gross primary production (gC/m2/year)\n");
		Annual_Log("Annual AR = annual total autotrophic respiration (gC/m2/year)\n");
		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Annual_Log("Annual HR = annual total heterotrophic respiration (gC/m2/year)\n");
			Annual_Log("Annual Reco = annual total ecosystem respiration (gC/m2/year)\n");
		}
		Annual_Log("Annual Cf = annual c-fluxes (gC/m2/year)\n");
		Annual_Log("Annual Y = NPP/GPP ratio (%%)\n");
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
		avg_npp_tot_gC = 0;
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
			Annual_Log ("\n%s \t%2s", "YEAR", "HC(0)");
			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Annual_Log ("\t%3s", "NEE");
			}
			Annual_Log ("\t%6s \t%10s" ,
					"GPP (tot)", "AR (tot)");
			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Annual_Log ("\t%3s, \t%3s", "HR (tot)", "Reco");
			}

			Annual_Log ("\t%6s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s  \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n",
					"Cf(tot)",
					"Y(%tot)", "NPP(tot)", "NPP(gC/m2yr)", "CE(tot)", "ASW", "Wf(tot)", "PEAK_LAI",
					"CC(tot)", "DEAD TREE(tot)", "wf", "ws", "wbb", "wfr", "wcr", "Wres", "DELTA-Wres");

		}
		Annual_Log ("%d \t%2d", yos[years].year, c->height_class_in_layer_dominant_counter);
		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Annual_Log ("\t%6.2f", c->annual_Nee);
		}
		Annual_Log("\t%10.2f \t%10.2f",
				c->annual_tot_gpp,
				c->annual_tot_aut_resp);

		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Annual_Log ("\t%10.2f \t%10.2f", c->annual_tot_het_resp, c->annual_Reco);
		}


		Annual_Log("\t%10.2f \t%10.2f\t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%12.2f "
				"\t%12.2f \t%14.2d \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f\n",
				c->annual_tot_c_flux,
				((c->annual_tot_aut_resp * 100.0)/c->annual_tot_gpp),
				c->annual_tot_npp,
				c->annual_tot_npp_g_c,
				c->annual_tot_c_evapotransp ,
				c->asw,
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
		avg_npp[0] += c->layer_annual_npp_tDM[0];
		avg_ce[0] += c->annual_c_evapotransp[0];
		avg_gpp_tot += c->annual_gpp[0];
		avg_ar_tot += c->annual_aut_resp[0];
		avg_cf_tot += c->annual_c_flux[0];
		avg_npp_tot += c->layer_annual_npp_tDM[0];
		avg_ce_tot += c->annual_c_evapotransp[0];
		tot_dead_tree_tot += c->annual_tot_dead_tree;

		//reset
		c->annual_gpp[0] = 0;
		c->annual_aut_resp[0] = 0;
		c->annual_c_flux[0] = 0;
		c->layer_annual_npp_tDM[0] = 0;
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
			Annual_Log ("\n%s \t%4s \t%4s",
					"YEAR", "HC(1)", "HC(0)");
			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Annual_Log ("\t%3s", "NEE");
			}

			Annual_Log ("\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s",
					"GPP(1)", "GPP(0)", "GPP(tot)", "AR(1)", "AR(0)", "AR(tot)");

			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Annual_Log ("\t%3s, \t%3s", "HR (tot)", "Reco");
			}

			Annual_Log ("\t%4s \t%4s\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%7s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s\n",
					"Y(1)", "Y(0)", "Y(tot)", "NPP(1)", "NPP(0)", "NPP (tot)", "NPP(gC/m2yr)", "CE(1)", "CE(0)", "CE (tot)", "ASW",
					"Wf", "PEAK_LAI(1)", "PEAK_LAI(0)", "CC(1)", "CC(0)", "DEAD TREE(1)", "DEAD TREE(0)", "DEAD TREE(tot)",
					"Wres(1)", "Wres(0)");

		}

		Annual_Log ("%d \t%4d \t%4d", yos[years].year,
				c->height_class_in_layer_dominant_counter, c->height_class_in_layer_dominated_counter);
		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Annual_Log ("\t%6.2f", c->annual_Nee);
		}

		Annual_Log ("\t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f  \t%4.2f",

				c->annual_gpp[1],c->annual_gpp[0], c->annual_tot_gpp,
				c->annual_aut_resp[1],c->annual_aut_resp[0], c->annual_tot_aut_resp);

		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Annual_Log ("\t%10.2f \t%10.2f", c->annual_tot_het_resp, c->annual_Reco);
		}

		Annual_Log ("\t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f"
				" \t%4.2f \t%2.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%12d \t%12d \t%12d \t%4.2f \t%4.2f\n",

				(c->annual_aut_resp[1]*100)/ c->annual_gpp[1],(c->annual_aut_resp[0]*100)/ c->annual_gpp[0],(c->annual_tot_aut_resp*100)/c->annual_tot_gpp,
				c->layer_annual_npp_tDM[1], c->layer_annual_npp_tDM[0],	c->annual_tot_npp, c->annual_tot_npp_g_c,
				c->annual_c_evapotransp[1], c->annual_c_evapotransp[0], c->annual_tot_c_evapotransp,
				c->asw,
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
		avg_npp[1] += c->layer_annual_npp_tDM[1];
		avg_npp_tot_gC += c->annual_tot_npp_g_c;
		avg_ce[1] += c->annual_c_evapotransp[1];
		avg_gpp[0] += c->annual_gpp[0];
		avg_ar[0] += c->annual_aut_resp[0];
		avg_cf[0] += c->annual_c_flux[0];
		avg_npp[0] += c->layer_annual_npp_tDM[0];
		avg_ce[0] += c->annual_c_evapotransp[0];
		avg_gpp_tot += c->annual_gpp[1] + c->annual_gpp[0];
		avg_ar_tot += c->annual_aut_resp[1] + c->annual_aut_resp[0];
		avg_npp_tot += c->layer_annual_npp_tDM[1] + c->layer_annual_npp_tDM[0];

		avg_ce_tot += c->annual_c_evapotransp[1] + c->annual_c_evapotransp[0];
		tot_dead_tree_tot += c->annual_tot_dead_tree;


		//reset
		c->annual_gpp[1] = 0;
		c->annual_aut_resp[1] = 0;
		c->annual_c_flux[1] = 0;
		c->layer_annual_npp_tDM[1] = 0;
		c->annual_c_evapotransp[1] = 0;
		c->annual_cc[1] = 0;
		c->annual_dead_tree[1] = 0;
		c->annual_delta_ws[1] = 0;
		c->annual_ws[1] = 0;
		c->annual_wres[1] = 0;

		c->annual_gpp[0] = 0;
		c->annual_aut_resp[0] = 0;
		c->annual_c_flux[0] = 0;
		c->layer_annual_npp_tDM[0] = 0;
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
			Annual_Log ("\n%s \t%2s \t%2s \t%2s",
					"YEAR", "HC(2)", "HC(1)", "HC(0)");

			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Annual_Log ("\t%3s", "NEE");
			}

			Annual_Log ("\t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s",
					"GPP(2)","GPP(1)", "GPP(0)", "GPP (tot)", "AR(2)", "AR(1)", "AR(0)", "AR(tot)");

			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Annual_Log ("\t%3s, \t%3s", "HR (tot)", "Reco");
			}

			Annual_Log ("\t%2s \t%s \t%s \t%s \t%s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s "
					"\t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s\n",
					"Cf(2)", "Cf(1)", "Cf(0)", "Cf(tot)", "Y(2)", "Y(1)", "Y(0)", "Y(tot)","NPP(2)", "NPP(1)", "NPP(0)", "NPP (tot)",
					"CE(2)", "CE(1)", "CE(0)", 	"CE(tot)", "ASW", "Wf",  "PEAK_LAI(2)", "PEAK_LAI(1)", "PEAK_LAI(0)",
					"CC(2)", "CC(1)", "CC(0)", "DEAD TREE(2)","DEAD TREE(1)",
					"DEAD TREE(0)", "DEAD TREE(tot)", "Wres(2)", "Wres(1)", "Wres(0)");

		}

		Annual_Log ("%d \t%2d \t%2d \t%2d", yos[years].year, c->height_class_in_layer_dominant_counter,c->height_class_in_layer_dominated_counter, c->height_class_in_layer_subdominated_counter);
		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Annual_Log ("\t%6.2f", c->annual_Nee);
		}

		Annual_Log ("\t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f ",
				c->annual_gpp[2], c->annual_gpp[1],c->annual_gpp[0], c->annual_tot_gpp,
				c->annual_aut_resp[2], c->annual_aut_resp[1],c->annual_aut_resp[0], c->annual_tot_aut_resp);

		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Annual_Log ("\t%10.2f \t%10.2f", c->annual_tot_het_resp, c->annual_Reco);
		}

		Annual_Log ("\t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f "
				" \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2d \t%2d \t%2d \t%2d "
				" \t%5.2f \t%5.2f \t%5.2f \t%5.2f \t%5.2f \n",
				c->annual_c_flux[2], c->annual_c_flux[1], c->annual_c_flux[0], c->annual_tot_c_flux,
				(c->annual_aut_resp[2]*100.0)/c->annual_gpp[2],((c->annual_aut_resp[1]*100.0)/c->annual_gpp[1]),((c->annual_aut_resp[0]*100.0)/c->annual_gpp[0]),(c->annual_tot_aut_resp*100.0)/c->annual_tot_gpp,
				c->layer_annual_npp_tDM[2], c->layer_annual_npp_tDM[1],	c->layer_annual_npp_tDM[0], c->annual_tot_npp,
				c->annual_c_evapotransp[2],c->annual_c_evapotransp[1], c->annual_c_evapotransp[0], c->annual_tot_c_evapotransp,
				c->asw, c->annual_tot_w_flux,
				c->annual_peak_lai[2], c->annual_peak_lai[1], c->annual_peak_lai[0],
				c->annual_cc[2],c->annual_cc[1], c->annual_cc[0],
				c->annual_dead_tree[2], c->annual_dead_tree[1], c->annual_dead_tree[0], c->annual_tot_dead_tree,
				c->annual_wres[2], c->annual_wres[1], c->annual_wres[0]);

		previous_layer_number = c->annual_layer_number;

		//compute average
		avg_gpp[2] += c->annual_gpp[2];
		avg_ar[2] += c->annual_aut_resp[2];
		avg_cf[2] += c->annual_c_flux[2];
		avg_npp[2] += c->layer_annual_npp_tDM[2];
		avg_ce[2] += c->annual_c_evapotransp[2];
		avg_gpp[1] += c->annual_gpp[1];
		avg_ar[1] += c->annual_aut_resp[1];
		avg_cf[1] += c->annual_c_flux[1];
		avg_npp[1] += c->layer_annual_npp_tDM[1];
		avg_ce[1] += c->annual_c_evapotransp[1];
		avg_gpp[0] += c->annual_gpp[0];
		avg_ar[0] += c->annual_aut_resp[0];
		avg_cf[0] += c->annual_c_flux[0];
		avg_npp[0] += c->layer_annual_npp_tDM[0];
		avg_ce[0] += c->annual_c_evapotransp[0];
		avg_gpp_tot += c->annual_gpp[2] +c->annual_gpp[1] + c->annual_gpp[0];
		avg_ar_tot += c->annual_aut_resp[2] +c->annual_aut_resp[1] + c->annual_aut_resp[0];
		avg_cf_tot += c->annual_c_flux[2] +c->annual_c_flux[1] + c->annual_c_flux[0];
		avg_npp_tot += c->layer_annual_npp_tDM[2] +c->layer_annual_npp_tDM[1] + c->layer_annual_npp_tDM[0];
		avg_ce_tot += c->annual_c_evapotransp[2] + c->annual_c_evapotransp[1] + c->annual_c_evapotransp[0];
		tot_dead_tree_tot += c->annual_tot_dead_tree;



		//reset
		c->annual_gpp[2] = 0;
		c->annual_aut_resp[2] = 0;
		c->annual_c_flux[2] = 0;
		c->layer_annual_npp_tDM[2] = 0;
		c->annual_c_evapotransp[2] = 0;
		c->annual_cc[2] = 0;
		c->annual_dead_tree[2] = 0;
		c->annual_delta_ws[2] = 0;
		c->annual_ws[2] = 0;
		c->annual_wres[2] = 0;

		c->annual_gpp[1] = 0;
		c->annual_aut_resp[1] = 0;
		c->annual_c_flux[1] = 0;
		c->layer_annual_npp_tDM[1] = 0;
		c->annual_c_evapotransp[1] = 0;
		c->annual_cc[1] = 0;
		c->annual_dead_tree[1] = 0;
		c->annual_delta_ws[1] = 0;
		c->annual_ws[1] = 0;
		c->annual_wres[1] = 0;

		c->annual_gpp[0] = 0;
		c->annual_aut_resp[0] = 0;
		c->annual_c_flux[0] = 0;
		c->layer_annual_npp_tDM[0] = 0;
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

	if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
	{
		c->annual_tot_het_resp = 0;
		c->annual_Reco = 0;
		c->annual_Nee = 0;
	}
	//aaa
	//compute average values
	if (years == years_of_simulation -1 && years_of_simulation > 1)
	{
		if (c->annual_layer_number == 1)
		{
			avg_gpp[0] /= years_of_simulation;
			avg_ar[0] /= years_of_simulation;
			avg_cf[0] /= years_of_simulation;
			avg_npp[0] /= years_of_simulation;
			avg_npp_tot_gC /= years_of_simulation;
			avg_ce[0] /= years_of_simulation;
			avg_gpp_tot /= years_of_simulation;
			avg_ar_tot /= years_of_simulation;
			avg_cf_tot /= years_of_simulation;
			avg_npp_tot /= years_of_simulation;
			avg_ce_tot /= years_of_simulation;
			Annual_Log ("-----------------------------------------------------------------------------------------------------------------------------------------------------"
					"---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			Annual_Log ("AVG/TOT \t\t%5f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t\t\t\t\t%49d\n",
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
			avg_npp_tot_gC /= years_of_simulation;
			avg_ce_tot /= years_of_simulation;
			Annual_Log ("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------,"
					"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			Annual_Log ("AVG/TOT \t\t\t%14f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t\t\t\t\t\t\t\t\t%76d \n",
					avg_gpp[1], avg_gpp[0], avg_gpp_tot, avg_ar[1], avg_ar[0], avg_ar_tot, (avg_ar[1]*100.0)/avg_gpp[1],
					((avg_ar[0]*100.0)/avg_gpp[0]),(avg_ar_tot*100.0)/avg_gpp_tot,avg_npp[1], avg_npp[0], avg_npp_tot, avg_npp_tot_gC, avg_ce[1], avg_ce[0], avg_ce_tot, tot_dead_tree_tot);
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

extern void EOM_cumulative_balance_cell_level (CELL *c, const YOS *const yos, int years, int month)
{
	static int previous_layer_number;

	if(month == 0 && years == 0)
	{
		Monthly_Log("Site name = %s\n", site->sitename);
		Monthly_Log("Monthly summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",settings->version, settings->time, settings->spatial);
		Monthly_Log("\n\nCell %d, %d, Lat = %f, Long  = %f\n\n\n", c->x, c->y, site->lat, site->lon );
		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Monthly_Log("Monthly NEE = Monthly total net ecosystem exchange (gC/m2/month)\n");
		}
		Monthly_Log("Monthly GPP = monthly total gross primary production (gC/m2/month)\n");
		Monthly_Log("Monthly AR = monthly total autotrophic respiration (gC/m2/month)\n");
		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Monthly_Log("Annual HR = Monthly total heterotrophic respiration (gC/m2/month)\n");
			Monthly_Log("Annual Reco = Monthly total ecosystem respiration (gC/m2/month)\n");
		}
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
			Monthly_Log ("\n%s \t%2s", "YEAR", "Month");
			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Monthly_Log ("\t%3s", "NEE");
			}
			Monthly_Log ("\t%6s \t%10s" ,
					"GPP (tot)", "AR (tot)");
			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Monthly_Log ("\t%3s, \t%3s", "HR (tot)", "Reco");
			}
			Monthly_Log ("\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s  \t%10s\n",
					"Cf(tot)", "NPP(tot)", "NPPgC", "CE(tot)", "ASW",  "Cw", "CC(0)", "DEAD TREE(tot)");
		}
		Monthly_Log ("%d \t%2d", yos[years].year, month+1);
		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Monthly_Log ("\t%6.2f", c->monthly_Nee);
		}
		Monthly_Log("\t%10.2f \t%10.2f",
				c->monthly_tot_gpp,
				c->monthly_tot_aut_resp);

		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Monthly_Log ("\t%10.2f \t%10.2f", c->monthly_tot_het_resp, c->monthly_Reco);
		}
		Monthly_Log("\t%14.2f \t%11.2f \t%11.2f \t%11.2f  \t%11.2f \t%11.2f \t%11.2f\n",
				c->monthly_tot_c_flux,
				c->monthly_tot_npp,
				c->monthly_tot_npp_g_c,
				c->monthly_tot_c_evapotransp,
				c->asw,
				c->monthly_tot_w_flux,
				c->monthly_cc[0]);

		//reset
		c->monthly_gpp[0] = 0;
		c->monthly_aut_resp[0] = 0;
		c->layer_monthly_npp_tDM[0] = 0;
		c->monthly_c_evapotransp[0] = 0;
		c->monthly_cc[0] = 0;
		c->monthly_dead_tree[0] = 0;
	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 2)
	{
		if (month == 0 && years == 0)
		{
			Monthly_Log ("\n%s \t%2s", "YEAR", "Month");
			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Monthly_Log ("\t%3s", "NEE");
			}
			Monthly_Log ("\t%6s \t%10s \t%10s \t%10s \t%10s \t%10s" ,
					"GPP(1)", "GPP(0)", "GPP(tot)",  "AR(1)", "AR(0)", "AR(tot)");
			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Monthly_Log ("\t%3s, \t%3s", "HR (tot)", "Reco");
			}
			Monthly_Log ("\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n",
					"Cf(1)", "Cf(0)", "Cf(tot)", "NPP(1)", "NPP(0)", "NPP(tot)", "CE(1)",
					"CE(0)", "CE(tot)", "ASW", "Cw", "CC(1)", "CC(0)", "DEAD TREE(1)", "DEAD TREE(0)", "DEAD TREE(tot)");
		}
		Monthly_Log ("%d \t%2d", yos[years].year, month+1);
		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Monthly_Log ("\t%6.2f", c->monthly_Nee);
		}
		Monthly_Log("\t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f",
				c->monthly_gpp[1],c->monthly_gpp[0], c->monthly_tot_gpp,
				c->monthly_aut_resp[1],c->monthly_aut_resp[0], c->monthly_tot_aut_resp);

		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Monthly_Log ("\t%10.2f \t%10.2f ", c->monthly_tot_het_resp, c->monthly_Reco);
		}
		Monthly_Log("\t%14.2f  \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f"
				" \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2d \t%11.2d \t%11.2d\n",
				c->monthly_c_flux[1], c->monthly_c_flux[0], c->monthly_tot_c_flux,
				c->layer_monthly_npp_tDM[1], c->layer_monthly_npp_tDM[0],c->monthly_tot_npp,
				c->monthly_c_evapotransp[1], c->monthly_c_evapotransp[0], c->monthly_tot_c_evapotransp,
				c->asw,
				c->monthly_tot_w_flux,
				c->monthly_cc[1], c->monthly_cc[0],
				c->monthly_dead_tree[1], c->monthly_dead_tree[0], c->monthly_tot_dead_tree);

		//reset
		c->monthly_gpp[1] = 0;
		c->monthly_aut_resp[1] = 0;
		c->monthly_c_flux[1] = 0;
		c->layer_monthly_npp_tDM[1] = 0;
		c->monthly_c_evapotransp[1] = 0;
		c->monthly_cc[1] = 0;
		c->monthly_dead_tree[1] = 0;

		c->monthly_gpp[0] = 0;
		c->monthly_aut_resp[0] = 0;
		c->monthly_c_flux[0] = 0;
		c->layer_monthly_npp_tDM[0] = 0;
		c->monthly_c_evapotransp[0] = 0;
		c->monthly_cc[0] = 0;
		c->monthly_dead_tree[0] = 0;
	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 3)
	{
		if (month == 0 && years == 0)
		{
			Monthly_Log ("\n%s \t%2s", "YEAR", "Month");
			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Monthly_Log ("\t%3s", "NEE");
			}
			Monthly_Log ("\t%6s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s" ,
					"GPP(2)","GPP(1)", "GPP(0)", "GPP (tot)", "AR(2)","AR(1)", "AR(0)", "AR(tot)");
			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Monthly_Log ("\t%3s, \t%3s", "HR (tot)", "Reco");
			}
			Monthly_Log ("\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s "
					"\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n",
					"Cf(2)", "Cf(1)", "Cf(0)", "Cf(tot)", "NPP(2)","NPP(1)", "NPP(0)","NPP (tot)", "CE(2)","CE(1)", "CE(0)", "CE(tot)",
					"ASW", "Cw", "CC(2)", "CC(1)", "CC(0)", "DEAD TREE(2)","DEAD TREE(1)", "DEAD TREE(0)", "DEAD TREE(tot)");
		}
		Monthly_Log ("%d \t%2d", yos[years].year, month+1);
		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Monthly_Log ("\t%6.2f", c->monthly_Nee);
		}
		Monthly_Log("\t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f",
				c->monthly_gpp[2], c->monthly_gpp[1],c->monthly_gpp[0], c->monthly_tot_gpp,
				c->monthly_aut_resp[2], c->monthly_aut_resp[1],c->monthly_aut_resp[0], c->monthly_tot_aut_resp);

		if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
		{
			Monthly_Log ("\t%10.2f \t%10.2f ", c->monthly_tot_het_resp, c->monthly_Reco);
		}
		Monthly_Log("\t%14.2f  \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f"
				" \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2d \t%11.2d \t%11.2d \t%11.2d\n",
				c->monthly_c_flux[2],c->monthly_c_flux[1], c->monthly_c_flux[0], c->monthly_tot_c_flux,
				c->layer_monthly_npp_tDM[2], c->layer_monthly_npp_tDM[1], c->layer_monthly_npp_tDM[0], c->monthly_tot_npp,
				c->monthly_c_evapotransp[2],c->monthly_c_evapotransp[1], c->monthly_c_evapotransp[0], c->monthly_tot_c_evapotransp,
				c->asw,
				c->monthly_tot_w_flux,
				c->monthly_cc[2], c->monthly_cc[1], c->monthly_cc[0],
				c->monthly_dead_tree[2], c->monthly_dead_tree[1], c->monthly_dead_tree[0], c->monthly_tot_dead_tree);

		//reset
		c->monthly_gpp[2] = 0;
		c->monthly_aut_resp[2] = 0;
		c->monthly_c_flux[2] = 0;
		c->layer_monthly_npp_tDM[2] = 0;
		c->monthly_c_evapotransp[2] = 0;
		c->monthly_cc[2] = 0;
		c->monthly_dead_tree[2] = 0;

		c->monthly_gpp[1] = 0;
		c->monthly_aut_resp[1] = 0;
		c->monthly_c_flux[1] = 0;
		c->layer_monthly_npp_tDM[1] = 0;
		c->monthly_c_evapotransp[1] = 0;
		c->monthly_cc[1] = 0;
		c->monthly_dead_tree[1] = 0;

		c->monthly_gpp[0] = 0;
		c->monthly_aut_resp[0] = 0;
		c->monthly_c_flux[0] = 0;
		c->layer_monthly_npp_tDM[0] = 0;
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

	if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
	{
		c->monthly_tot_het_resp = 0;
		c->monthly_Reco = 0;
		c->monthly_Nee = 0;
	}
}


extern void EOD_cumulative_balance_cell_level (CELL *c, const YOS *const yos, int years, int month, int day )
{

	static int previous_layer_number;
	static int doy;

	if(day  == 0 && month == 0 && years == 0)
	{

		//Daily_Log("Site name = %s\n", site->sitename);
		//Daily_Log("Daily summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",settings->version, settings->time, settings->spatial);
		//Daily_Log("\n\nCell %d, %d, Lat = %f, Long  = %f\n\n\n", c->x, c->y, site->lat, site->lon );


		//Daily_Log ("HC\n");
		if (!mystricmp(settings->dndc, "on"))
				{
					//Daily_Log("Daily NEE = daily total net ecosystem exchange (gC/m2/day)\n");
				}
		//Daily_Log("Daily GPP = daily total gross primary production (gC/m2/day)\n");
		//Daily_Log("Daily AR = daily total autotrophic respiration (gC/m2/day)\n");
		//Daily_Log("Daily ARtDM = daily total autotrophic respiration (tDM/day cell)\n");
		if (!mystricmp(settings->dndc, "on"))
		{
			//Daily_Log("Daily HR = daily total heterotrophic respiration (gC/m2/day)\n");
			//Daily_Log("Daily Reco = daily total ecosystem respiration (gC/m2/day)\n");
		}
		/*
		Daily_Log("Daily Cf = daily c-fluxes (gC/m2/day)\n");
		Daily_Log("Daily CftDM = daily c-fluxes (tDM/day cell)\n");
		Daily_Log("Daily NPP = daily total net primary production (tDM/m2/day)\n");
		Daily_Log("Daily CE = daily canopy evapotranspiration(mm/day)\n");
		Daily_Log("Daily LE = daily latent heat (W/m^2)\n");
		Daily_Log("Daily ASW = daily Available Soil Water(mm/day)\n");
		Daily_Log("Daily Wfl = daily water fluxes (mm/day)\n");
		Daily_Log("Daily LAI = daily Leaf Area Index (m^2/m^2)\n");
		Daily_Log("Daily D-Wf = daily fraction of NPP to foliage pool (tDM/day cell)\n");
		Daily_Log("Daily D-Ws = daily fraction of NPP to stem pool (tDM/day cell)\n");
		Daily_Log("Daily D-Wbb = daily fraction of NPP to branch and bark pool (tDM/day cell)\n");
		Daily_Log("Daily D-Wfr = daily fraction of NPP to fine root pool (tDM/day cell)\n");
		Daily_Log("Daily D-Wcr = daily fraction of NPP to coarse root pool (tDM/day cell)\n");
		Daily_Log("Daily D-Wres = daily fraction of NPP to reserve pool (tDM/day cell)\n");
		*/

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
			Daily_Log ("%s \t%s \t%2s \t%2s \t%2s", "DOY", "YEAR", "MONTH", "DAY", "HC");
			if (!mystricmp(settings->dndc, "on") || !mystricmp(settings->rothC, "on"))
			{
				Daily_Log ("\t%3s", "NEE");
			}
			Daily_Log ("\t%6s \t%10s \t%10s" ,
					"GPP(0)", "AR(0)","ARtDM(0)");
			if (!mystricmp(settings->dndc, "on"))
			{
				Daily_Log ("\t%3s, \t%3s", "HR (tot)", "Reco");
			}
			Daily_Log ("\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\t%10s \t%10s "
					"\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n",
					"Cf", "CftDM", "NPP(0)", "NPPgC", "CE(0)","LE(0)", "ASW", "Wfl", "LAI(0)",
					"CC(0)", "DEADTREE(0)", "D-Wf", "D-Ws", "D-Wbb", "D-Wfr", "D-Wcr", "D-Wres", "Wres");
		}
		if ((day == 0 && month == 0) || previous_layer_number != c->annual_layer_number)
		{
			doy = 1;
		}

		Daily_Log ("%d \t%8d \t%5d \t%5d \t%2d", doy++, yos[years].year, month+1, day+1,c->height_class_in_layer_dominant_counter);
		if (!mystricmp(settings->dndc, "on"))
		{
			Daily_Log ("\t%6.2f", c->nee);
		}
		Daily_Log("\t%10.2f \t%10.2f \t%10.2f",
				c->layer_daily_gpp[0],
				c->daily_aut_resp[0],
				c->daily_aut_resp_tDM[0]);

		if (!mystricmp(settings->dndc, "on"))
		{
			Daily_Log ("\t%10.2f \t%10.2f", c->daily_tot_het_resp, c->Reco);
		}
		Daily_Log("\t%14.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f  \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%14.2d \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f\n",
				c->daily_c_flux[0],
				c->daily_c_flux_tDM[0],
				c->layer_daily_npp_tDM[0],
				c->daily_npp_g_c[0],
				c->daily_c_evapotransp[0],
				c->daily_tot_latent_heat_flux,
				c->asw,
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
		//}

		previous_layer_number = c->annual_layer_number;

		//reset
		c->layer_daily_gpp[0] = 0;
		c->daily_aut_resp[0] = 0;
		c->daily_aut_resp_tDM[0] = 0;
		c->daily_c_flux_tDM[0] = 0;
		c->layer_daily_npp_tDM[0] = 0;
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
			Daily_Log ("\n%s \t%s \t%2s \t%2s \t%2s \t%2s", "DOY", "YEAR", "MONTH", "DAY", "HC(1)", "HC(0)");
			if (!mystricmp(settings->dndc, "on"))
			{
				Daily_Log ("\t%3s", "NEE");
			}
			Daily_Log ("\t%6s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s" ,
					"GPP(1)", "GPP(0)", "GPP(tot)", "AR(1)", "AR", "AR(tot)");
			if (!mystricmp(settings->dndc, "on"))
			{
				Daily_Log ("\t%3s, \t%3s", "HR (tot)", "Reco");
			}
			Daily_Log ("\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\t%10s \t%10s "
					"\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n",
					"Cf(1)", "Cf(0)", "Cf(tot)", "NPP(1)", "NPP(0)", "NPP(tot)","CE(1)", "CE(0)", "CE(tot)","ASW",
					"LAI(1)", "LAI(0)", "CC(1)", "CC(0)", "DEADTREE(1)", "DEADTREE(0)", "DEADTREE(tot)");
		}
		if ((day == 0 && month == 0) || previous_layer_number != c->annual_layer_number)
		{
			doy = 1;
		}

		Daily_Log ("%d \t%8d \t%5d \t%5d  \t%5d \t%2d",doy, yos[years].year, month+1, day+1,
				c->height_class_in_layer_dominant_counter,
				c->height_class_in_layer_dominated_counter);
		if (!mystricmp(settings->dndc, "on"))
		{
			Daily_Log ("\t%6.2f", c->nee);
		}
		Daily_Log("\t%10.2f \t%10.2f \t%10.2f\t%10.2f\t%10.2f \t%10.2f",
				c->layer_daily_gpp[1], c->layer_daily_gpp[0], c->layer_daily_gpp,
				c->daily_aut_resp[1], c->daily_aut_resp[0], c->daily_tot_aut_resp);

		if (!mystricmp(settings->dndc, "on"))
		{
			Daily_Log ("\t%10.2f \t%10.2f", c->daily_tot_het_resp, c->Reco);
		}
		Daily_Log("\t%14.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f  \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%14.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2d \t%11.2d \t%11.2d\n",
				c->daily_c_flux[1], c->daily_c_flux[0], c->daily_tot_c_flux,
				c->layer_daily_npp_tDM[1], c->layer_daily_npp_tDM[0], c->daily_tot_npp,
				c->daily_c_evapotransp[1], c->daily_c_evapotransp[0], c->daily_tot_c_evapotransp,
				c->asw,
				c->daily_lai[1], c->daily_lai[0],
				c->daily_cc[1]*100, c->daily_cc[0]*100,
				c->daily_dead_tree[1], c->daily_dead_tree[0], c->daily_tot_dead_tree);

		//}

		previous_layer_number = c->annual_layer_number;

		//reset
		c->layer_daily_gpp[1] = 0;
		c->daily_aut_resp[1] = 0;
		c->daily_c_flux[1] = 0;
		c->layer_daily_npp_tDM[1] = 0;
		c->daily_c_int[1] = 0;
		c->daily_c_transp[1] = 0;
		c->daily_c_evapotransp[1] = 0;
		c->daily_et[1] = 0;
		c->daily_cc[1] = 0;
		c->daily_dead_tree[1] = 0;

		c->layer_daily_gpp[0] = 0;
		c->daily_aut_resp[0] = 0;
		c->daily_c_flux[0] = 0;
		c->layer_daily_npp_tDM[0] = 0;
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
			Daily_Log ("\n%s \t%2s\t%2s ", "YEAR", "MONTH", "DAY");
			if (!mystricmp(settings->dndc, "on"))
			{
				Daily_Log ("\t%3s", "NEE");
			}
			Daily_Log ("\t%6s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s" ,
					"GPP(2)","GPP(1)", "GPP(0)", "GPP(tot)", "AR(2)","AR(1)", "AR(0)", "AR(tot)");
			if (!mystricmp(settings->dndc, "on"))
			{
				Daily_Log ("\t%3s, \t%3s", "HR (tot)", "Reco");
			}
			Daily_Log ("\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s "
					"\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n",
					"Cf(2)", "Cf(1)", "Cf(0)", "Cf(tot)", "NPP(2)","NPP(1)", "NPP(0)", "NPP(tot)", "NPPgC(2)","NPPgC(1)", "NPPgC(0)", "NPPgC(tot)",
					"CE(2)","CE(1)", "CE(0)", "CE(tot)", "ASW", "LAI(2)","LAI(1)", "LAI(0)", "CC(2)", "CC(1)", "CC(0)", "DEADTREE(2)", "DEADTREE(1)", "DEADTREE(0)", "DEADTREE(tot)");
		}
		Daily_Log ("%d \t%2d \t%2d", yos[years].year, month+1, day+1);
		if (!mystricmp(settings->dndc, "on"))
		{
			Daily_Log ("\t%6.2f", c->nee);
		}
		Daily_Log("\t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f",
				c->layer_daily_gpp[2], c->layer_daily_gpp[1],c->layer_daily_gpp[0], c->daily_tot_gpp,
				c->daily_aut_resp[2], c->daily_aut_resp[1],c->daily_aut_resp[0], c->daily_tot_aut_resp);

		if (!mystricmp(settings->dndc, "on"))
		{
			Daily_Log ("\t%10.2f \t%10.2f ", c->monthly_tot_het_resp, c->monthly_Reco);
		}
		Daily_Log("\t%14.2f  \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f"
				" \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f"
				"\t%11.2f \t%11.2d \t%11.2d \t%11.2d \t%11.2d\n",
				c->daily_c_flux[2],c->daily_c_flux[1],c->daily_c_flux[0],c->daily_tot_c_flux,
				c->layer_daily_npp_tDM[2], c->layer_daily_npp_tDM[1],c->layer_daily_npp_tDM[0], c->daily_tot_npp,
				c->daily_npp_g_c[2], c->daily_npp_g_c[1],c->daily_npp_g_c[0], c->daily_tot_npp_g_c,
				c->daily_c_evapotransp[2],c->daily_c_evapotransp[1], c->daily_c_evapotransp[0], c->daily_tot_c_evapotransp,
				c->asw,
				c->daily_lai[2], c->daily_lai[1], c->daily_lai[0],
				c->daily_cc[2]*100, c->daily_cc[1]*100,c->daily_cc[0]*100,
				c->daily_dead_tree[2], c->daily_dead_tree[1], c->daily_dead_tree[0], c->daily_tot_dead_tree);

		previous_layer_number = c->annual_layer_number;

		//reset
		c->layer_daily_gpp[2] = 0;
		c->daily_aut_resp[2] = 0;
		c->daily_c_flux[2] = 0;
		c->layer_daily_npp_tDM[2] = 0;
		c->daily_npp_g_c[2] = 0;
		c->daily_c_int[2] = 0;
		c->daily_c_transp[2] = 0;
		c->daily_c_evapotransp[2] = 0;
		c->daily_et[2] = 0;
		c->daily_cc[2] = 0;
		c->daily_dead_tree[2] = 0;

		c->layer_daily_gpp[1] = 0;
		c->daily_aut_resp[1] = 0;
		c->daily_c_flux[1] = 0;
		c->layer_daily_npp_tDM[1] = 0;
		c->daily_npp_g_c[1] = 0;
		c->daily_c_int[1] = 0;
		c->daily_c_transp[1] = 0;
		c->daily_c_evapotransp[1] = 0;
		c->daily_et[1] = 0;
		c->daily_cc[1] = 0;
		c->daily_dead_tree[1] = 0;

		c->layer_daily_gpp[0] = 0;
		c->daily_aut_resp[0] = 0;
		c->daily_c_flux[0] = 0;
		c->layer_daily_npp_tDM[0] = 0;
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

	c->daily_tot_latent_heat_flux = 0;

	if (!mystricmp(settings->dndc, "on"))
	{
		c->daily_tot_het_resp = 0.0;
		c->Reco = 0.0;
		c->nee = 0.0;
	}
	//reset after printed at the end of the day
	/*
	c->class_daily_gpp = 0;
	c->daily_npp = 0;
	c->daily_et = 0;
	c->daily_lai = 0;
	 */
	c->daily_f_sw = 0.0;
	c->daily_f_psi = 0.0;
	c->daily_f_t = 0.0;
	c->daily_f_vpd = 0.0;

}



extern void Get_EOD_soil_balance_cell_level (CELL *c, const YOS *const yos, int years, int month, int day )
{

	static int previous_layer_number;
	static int doy;
	int soil = 0;

	if(day  == 0 && month == 0 && years == 0)
	{
		if (!mystricmp(settings->rothC, "on"))
		{
			soil_Log("RUNNING ROTHC.....\n");
		}
		else if (!mystricmp(settings->dndc, "on"))
		{
			soil_Log("RUNNING DNDC.....\n");
			//fixSergio improve a multilayer based log, specular to the one used for the aboveground

		}
		else
		{
			soil_Log("No soil simulation!!!\n");
		}
	}
	//		Daily_Log("Site name = %s\n", site->sitename);
	//		Daily_Log("Daily summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",settings->version, settings->time, settings->spatial);
	//		Daily_Log("\n\nCell %d, %d, Lat = %f, Long  = %f\n\n\n", c->x, c->y, site->lat, site->lon );
	//
	//
	//		Daily_Log ("HC\n");
	//
	//		Daily_Log("Daily GPP = daily total gross primary production (gC/m2/day)\n");
	//		Daily_Log("Daily AR = daily total autotrophic respiration (gC/m2/day)\n");
	//		Daily_Log("Daily ARtDM = daily total autotrophic respiration (tDM/day cell)\n");
	//		Daily_Log("Daily Cf = daily c-fluxes (gC/m2/day)\n");
	//		Daily_Log("Daily CftDM = daily c-fluxes (tDM/day cell)\n");
	//		Daily_Log("Daily NPP = daily total net primary production (tDM/m2/day)\n");
	//		Daily_Log("Daily CE = daily canopy evapotranspiration(mm/day)\n");
	//		Daily_Log("Daily ASW = daily Available Soil Water(mm/day)\n");
	//		Daily_Log("Daily Wfl = daily water fluxes (mm/day)\n");
	//		Daily_Log("Daily LAI = daily Leaf Area Index (m^2/m^2)\n");
	//		Daily_Log("Daily D-Wf = daily fraction of NPP to foliage pool (tDM/day cell)\n");
	//		Daily_Log("Daily D-Ws = daily fraction of NPP to stem pool (tDM/day cell)\n");
	//		Daily_Log("Daily D-Wbb = daily fraction of NPP to branch and bark pool (tDM/day cell)\n");
	//		Daily_Log("Daily D-Wfr = daily fraction of NPP to fine root pool (tDM/day cell)\n");
	//		Daily_Log("Daily D-Wcr = daily fraction of NPP to coarse root pool (tDM/day cell)\n");
	//		Daily_Log("Daily D-Wres = daily fraction of NPP to reserve pool (tDM/day cell)\n");
	//
	//	}

	if (years == 0)
	{
		previous_layer_number = c->annual_layer_number;
	}
	else
	{
		//potentially useless for soil, but what if we consider erosion losses or soil deposition? maintain it
		//check if layer number is changed since last yearly run
		if(previous_layer_number != c->annual_layer_number)
		{
			soil_Log("\n\nANNUAL_LAYER_NUMBER_IS_CHANGED_SINCE_PREVIOUS_YEAR!!!\n\n");
		}
	}
	if (!mystricmp(settings->dndc, "on"))
	{
		//fixSergio improve a multilayer based log, specular to the one used for the aboveground
		if ((day == 0 && month == 0 && years == 0) || previous_layer_number != c->annual_layer_number)
		{
			//			soil_Log ("\n%s \t%s \t%2s \t%2s \t%2s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s "
			//					"\t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s"
			//					"\t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \n",
			//					"DOY", "YEAR", "MONTH", "DAY", "leafLittering","fineRootLittering", "woodLittering", "stemLittering", "coarseRootLittering", "day_C_mine",
			//					"day_N_mine", "day_N_assim", "day_soil_nh3", "day_clayn", "day_clay_N",
			//					"wrcvl", "wrcl", "wrcr", "wcrb", "wcrh","whumus", "wtcavai", "w_no3", "w_nh4", "w_nh3", "wsoc", "day_no2",
			//					"runoff_N", "previousSoilT", "soilSurfaceT", "temp_avet", "yr_avet","base_clay_N", "max_clay_N",
			//					"AddC", "AddCN", "AddC1", "AddC2", "AddC3");
			soil_Log ("\n%s \t%s \t%2s \t%2s \t%2s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s "
					"\t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s"
					"\t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \n",
					"DOY", "YEAR", "MONTH", "DAY", "soilMoisture","soilTemp", "leafLittering","fineRootLittering", "woodLittering",
					"stemLittering", "coarseRootLittering","soc", "doc", "rcvl","rcl", "rcr", "CRB1", "CRB2", "crhl",
					"crhr", "dphum", "no3", "no2", "nh4","nh3", "co2", "day_O2", "dailyGPP[0]", "dailyAutResp[0]", "dailyTotGPP", "dailyTotRespAut",
					"stemBranchBiomass", "inert_C", "AddC1", "AddC2", "AddC3","sts", "mmm",
					"initialOrganicC", "waterContent", "litco22", "litco23", "CEC");
		}
		if ((day == 0 && month == 0) || previous_layer_number != c->annual_layer_number)
		{
			doy = 1;
		}
		//		soil_Log ("%d \t%8d \t%5d \t%3d \t%2d \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f "
		//				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f"
		//				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f"
		//				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \n",
		//				doy++, yos[years].year, month+1, day+1,
		//				c->leafLittering,
		//				c->fineRootLittering,
		//				c->woodLittering,
		//				c->stemLittering,
		//				c->coarseRootLittering,
		//				c->day_C_mine,
		//				c->day_N_mine,
		//				c->day_N_assim,
		//				c->day_soil_nh3,
		//				c->day_clayn,
		//				c->day_clay_N,
		//				c->wrcvl,
		//				c->wrcl,
		//				c->wrcr,
		//				c->wcrb,
		//				c->wcrh,
		//				c->whumus,
		//				c->wtcavai,
		//				c->w_no3,
		//				c->w_nh4,
		//				c->w_nh3,
		//				c->wsoc,
		//				c->day_no2,
		//				c->runoff_N,
		//				c->previousSoilT,
		//				c->soilSurfaceT,
		//				c->temp_avet,
		//				c->yr_avet,
		//				c->base_clay_N,
		//				c->max_clay_N,
		//				c->AddC,
		//				c->AddCN,
		//				c->AddC1,
		//				c->AddC2,
		//				c->AddC3);

		//soil structure variables
		//fixSergio improve a multilayer based log, specular to the one used for the aboveground
		//		if ((day == 0 && month == 0 && years == 0) || previous_layer_number != c->annual_layer_number)
		//		{

		//		}
		//		if ((day == 0 && month == 0) || previous_layer_number != c->annual_layer_number)
		//		{
		//			doy = 1;
		//		}
		soil_Log ("%d \t%5d \t%3d \t%2d \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f "
				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f"
				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f"
				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f  \t%8.6f \n ",
				doy++, yos[years].year, month+1, day+1,
				c->soils[0].soilMoisture,
				c->soils[0].soilTemp,
				c->leafLittering,
				c->fineRootLittering,
				c->stemBrancLittering,
				c->stemLittering,
				c->coarseRootLittering,
				c->soils[0].soc,
				c->soils[0].doc,
				c->soils[0].rcvl,
				c->soils[0].rcl,
				c->soils[0].rcr,
				c->soils[0].CRB1,
				c->soils[0].CRB2,
				c->soils[0].crhl,
				c->soils[0].crhr,
				c->soils[0].dphum,
				c->soils[0].no3,
				c->soils[0].no2,
				c->soils[0].nh4,
				c->soils[0].nh3,
				c->soils[0].co2,
				c->soils[0].day_O2,
				//				c->soils[0].dcbavai,
				//				c->soils[0].drcvl,
				//				c->soils[0].DRCB1,
				//				c->soils[0].DRCB2,
				//				c->soils[0].clay_nh4,
				c->layer_daily_gpp[0],
				c->daily_aut_resp[0],
				c->daily_tot_gpp,
				c->daily_tot_aut_resp,
				c->stemBranchBiomass,

				c->soils[0].inert_C,
				c->AddC1,
				c->AddC2,
				c->AddC3,
				c->soils[0].sts,
				c->soils[0].mmm,
				c->soils[0].initialOrganicC,
				c->soils[0].waterContent,
				c->soils[0].litco22,
				c->soils[0].litco23,
				c->soils[0].CEC);
		c->leafLittering = 0;
		c->fineRootLittering = 0;
		c->coarseRootLittering =0;
		c->stemLittering =0;
		c->stemBrancLittering = 0;
		c->leaflitN = 0;
		c->fineRootlitN = 0;
		c->coarseRootlitN = 0;
		c->stemlitN = 0;
		c->stemBranclitN = 0;

		c->daily_tot_gpp = 0;
		c->daily_tot_aut_resp = 0;
		c->layer_daily_gpp[0] = 0;
		c->daily_aut_resp[0] = 0;

	}
	else if (!mystricmp(settings->rothC, "on"))
	{
		if ((month == 0 && years == 0) || previous_layer_number != c->annual_layer_number)
		{
			soil_Log ("\n%s  \t%2s  \t%2s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \n",
					"YEAR", "MONTH",  "Litter","ET", "fT", "soilEvapo", "coverT", "moistT", "HResp", "MicPool", "LabPool", "ResPool", "HumPool", "IOM"
					,"dailyAutResp[0]", "dailyTotGPP", "dailyTotRespAut");
		}
		if ((month == 0) || previous_layer_number != c->annual_layer_number)
		{
			doy = 1;
		}
		soil_Log ("%d \t%5d  \t%2.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f "
				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \n",
				yos[years].year, month+1,
				c->monthly_tot_litterfall,
				c->monthly_tot_et,
				c->temperatureModifier,
				c->soil_evaporation,
				c->soilCoverModifier,
				c->moistureModifier,
				c->soils[soil].soil_het_resp,
				c->soils[soil].microbialBiomass,
				c->soils[soil].decomposablePlantMaterial,
				c->soils[soil].resistantPlantMaterial,
				c->soils[soil].humifiedOM,
				c->soils[soil].inertOM),
						c->layer_daily_gpp[0],
						c->daily_aut_resp[0],
						c->daily_tot_gpp,
						c->daily_tot_aut_resp;



		//previous_layer_number = c->annual_layer_number;

		//reset
		c->monthly_tot_et =0;
		c->monthly_tot_litterfall = 0;
		c->daily_tot_gpp = 0;
		c->daily_tot_aut_resp = 0;
		c->layer_daily_gpp[0] = 0;
		c->daily_aut_resp[0] = 0;

	}

}
