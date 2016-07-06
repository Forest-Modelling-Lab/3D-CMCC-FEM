/* cumulative_balance.c */
#include "cumulative_balance.h"
#include "common.h"
#include "settings.h"
#include "logger.h"

extern settings_t* g_settings;
extern logger_t* g_log;
extern logger_t* g_daily_log;
extern logger_t* g_monthly_log;
extern logger_t* g_annual_log;
extern logger_t* g_soil_log;

void EOY_cumulative_balance_layer_level(const species_t* const s, const height_t* const h) {
	if ( s->value[DEL_Y_WS] + s->value[DEL_Y_WR] + s->value[DEL_Y_WF] + s->value[DEL_Y_WRES] + s->value[DEL_Y_BB] != s->value[YEARLY_NPP_tDM] ) {
		logger(g_log, "ATTENTION SUM OF ALL INCREMENTS DIFFERENT FROM YEARLY NPP \n");
		logger(g_log, "DEL SUM = %f \n", s->value[DEL_Y_WS] + s->value[DEL_Y_WCR] + s->value[DEL_Y_WFR] + s->value[DEL_Y_BB] + s->value[DEL_Y_WF] + s->value[DEL_Y_WRES]);
	}
}

void EOY_cumulative_balance_cell_level(cell_t *const c, const int year, const int years_of_simulation, const int cell_index)
{
	static double avg_gpp[3], avg_npp[3], avg_ce[3], avg_gpp_tot, avg_npp_tot, avg_npp_tot_gC, avg_ce_tot;
	static double avg_ar[3], avg_ar_tot;
	static double avg_cf[3], avg_cf_tot;
	static int tot_dead_tree_tot;

	static int previous_layer_number;

//	if (years == 0)
//	{
//		logger(g_annual_log, "Site name = %s\n", g_soil_settings->values[SOIL_sitename);
//		logger(g_annual_log, "Annual summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",g_settings->version, g_settings->time, g_settings->spatial);
//		logger(g_annual_log, "years of simulation = %d\n", years_of_simulation);
//		logger(g_annual_log, "\n\nCell %d, %d, Lat = %f, Long  = %f\n\n\n", c->x, c->y, g_soil_settings->values[SOIL_lat, g_soil_settings->values[SOIL_lon );
//		logger(g_annual_log, "HC(n) = height class counter for n layer\n");
//		logger(g_annual_log, "Annual GPP = annual total gross primary production (gC/m2/year)\n");
//		logger(g_annual_log, "Annual AR = annual total autotrophic respiration (gC/m2/year)\n");
//		if (!string_compare_i(g_settings->dndc, "on"))
//		{
//			logger(g_annual_log, "Annual HR = annual total heterotrophic respiration (gC/m2/year)\n");
//			logger(g_annual_log, "Annual Reco = annual total ecosystem respiration (gC/m2/year)\n");
//		}
//		logger(g_annual_log, "Annual Cf = annual c-fluxes (gC/m2/year)\n");
//		logger(g_annual_log, "Annual Y = NPP/GPP ratio (%%)\n");
//		logger(g_annual_log, "Annual NPP = annual total net primary production (tDM/m2/year)\n");
//		logger(g_annual_log, "Annual CE = annual canopy evapotranspiration(mm/year)\n");
//		logger(g_annual_log, "Annual ASW = end of year annual available soil water(mm)\n");
//		logger(g_annual_log, "Annual Wf = annual water-fluxes (mm/year)\n");
//		logger(g_annual_log, "Annual PEAK_LAI = annual Peak Lai (m^2/m^2)\n");
//		logger(g_annual_log, "Annual Dead tree = annual dead tree (n tree/cell)\n\n\n");
//	}

	//reset
	if ( 0 == year)
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
			logger(g_annual_log, "\n\nANNUAL_LAYER_NUMBER_IS_CHANGED_SINCE_PREVIOUS_YEAR!!!\n\n");
		}
	}

	//logger(g_annual_log, "-%d %10f %10f %10f %10f %10f %10f %10d\n", yos[years].year, c->annual_gpp, c->annual_npp, c->annual_et, c->stand_agb, c->stand_bgb, c->annual_peak_lai, c->dead_tree);

	if (c->annual_layer_number == 1)
	{
		if ((year == 0 || previous_layer_number != c->annual_layer_number) && cell_index == 0 )
		{
			logger(g_annual_log, "%s \t%4s \t%s", "YEAR", "CELL", "HC(0)");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_annual_log, "\t%3s", "NEE");
			}
			logger(g_annual_log, "\t%s \t%s \t%s \t%s" ,
					"GPP(gC/m2y)", "AR(gC/m2y)", "MR(gC/m2y)", "GR(gC/m2y)");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_annual_log, "\t%7s, \t%3s", "HR (tot)", "Reco");
			}

			logger(g_annual_log, "\t%10s \t%10s \t%6s \t%6s \t%8s \t%10s \t%3s \t%10s \t%10s \t%6s \t%6s \t%6s \t%6s \t%8s \t%8s  \t%8s \t%8s \t%8s \t%5s \t%10s \t%8s \t%8s \t%9s \t%7s \t%9s \t%8s \t%9s \t%7s \t%6s\n",
					"Y(%)", "NPP(gC/m2y)", "ET(mm/m2)", "LE(W/m2)", "ASW", "PEAK_LAI",
					"CC", "LIVE TREE", "DEAD TREE", "avDBH", "wf", "ws", "wsl", "ws-sap", "wbb", "wbbl", "wbb-sap", "wfr", "wcr", "wcrl", "wcr-sap", "wsap", "Wres(KG/t)", "D-Wres", "leafAR", "stemAR", "branchAR", "frAR", "crAR");

		}
		logger(g_annual_log, "%d \t%d,%d \t%2d", c->years[year].year, c->x, c->y, c->height_class_in_layer_dominant_counter);
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_annual_log, "\t%6.2f", c->annual_nee);
		}
		logger(g_annual_log, "\t%10.2f \t%10.2f \t%12.2f \t%10.2f",
				c->annual_gpp,
				c->annual_aut_resp,
				c->annual_maint_resp,
				c->annual_growth_resp);


		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_annual_log, "\t%10.2f \t%10.2f", c->annual_het_resp, c->annual_r_eco);
		}


		logger(g_annual_log, "\t%12.2f \t%8.2f \t%10.2f \t%10.2f \t%10.2f \t%6.2f "
				"\t%5.2f \t%8.2d \t%8.2d \t%8.2f \t%8.2f \t%8.2f \t%8.2f "
				"\t%8.2f \t%8.2f \t%7.2f \t%6.2f \t%6.2f \t%10.2f \t%8.2f "
				"\t%8.2f \t%8.2f \t%9.4f \t%6.2f \t%10.2f \t%8.2f \t%8.2f "
				"\t%9.2f \t%6.2f\n",
				((c->annual_aut_resp * 100.0)/c->annual_gpp),
				c->annual_npp_gC,
				c->annual_et ,
				c->annual_latent_heat_flux,
				c->asw,
				c->annual_peak_lai[0],
				c->layer_annual_cc[0],
				c->n_tree,
				c->annual_dead_tree,
				c->annual_layer_avDBH[0],
				c->annual_layer_leaf_c[0],
				c->annual_layer_stem_c[0],
				c->annual_layer_live_stem_c[0],
				c->annual_layer_stem_sapwood_c[0],
				c->annual_layer_branch_c[0],
				c->annual_layer_live_branch_c[0],
				c->annual_layer_branch_sapwood_c[0],
				c->annual_layer_fineroot_c[0],
				c->annual_layer_coarseroot_c[0],
				c->annual_layer_live_coarseroot_c[0],
				c->annual_layer_coarse_root_sapwood_c[0],
				c->annual_layer_sapwood_c[0],
				c->annual_layer_reserve_c[0]/c->n_tree*1000.0,
				c->annual_delta_wres[0],
				c->layer_annual_leaf_aut_resp[0],
				c->layer_annual_stem_aut_resp[0],
				c->layer_annual_branch_aut_resp[0],
				c->layer_annual_fine_root_aut_resp[0],
				c->layer_annual_coarse_root_aut_resp[0]);

		previous_layer_number = c->annual_layer_number;

		//compute average or total
		avg_gpp[0] += c->layer_annual_gpp[0];
		avg_ar[0] += c->layer_annual_aut_resp[0];
		avg_cf[0] += c->layer_annual_c_flux[0];
		avg_npp[0] += c->layer_annual_npp_tDM[0];
		avg_ce[0] += c->layer_annual_c_evapotransp[0];
		avg_gpp_tot += c->layer_annual_gpp[0];
		avg_ar_tot += c->layer_annual_aut_resp[0];
		avg_cf_tot += c->layer_annual_c_flux[0];
		avg_npp_tot += c->layer_annual_npp_tDM[0];
		avg_ce_tot += c->layer_annual_c_evapotransp[0];
		tot_dead_tree_tot += c->annual_dead_tree;

		//reset
		c->layer_annual_gpp[0] = 0;
		c->layer_annual_aut_resp[0] = 0;
		c->layer_annual_c_flux[0] = 0;
		c->layer_annual_npp_tDM[0] = 0;
		c->layer_annual_c_evapotransp[0] = 0;
		c->layer_annual_cc[0] = 0;
		c->layer_annual_dead_tree[0] = 0;
		c->annual_peak_lai[0] = 0;
		c->annual_layer_reserve_c[0] = 0;
		c->annual_delta_ws[0] = 0;
		c->annual_delta_wres[0] = 0;
		c->annual_layer_leaf_c[0]= 0;
		c->annual_layer_stem_c[0]= 0;
		c->annual_layer_live_stem_c[0]= 0;
		c->annual_layer_branch_c[0]= 0;
		c->annual_layer_live_branch_c[0]= 0;
		c->annual_layer_fineroot_c[0]= 0;
		c->annual_layer_coarseroot_c[0]= 0;
		c->annual_layer_live_coarseroot_c[0]= 0;
		c->layer_annual_leaf_aut_resp[0]= 0.0;
		c->layer_annual_stem_aut_resp[0]= 0.0;
		c->layer_annual_branch_aut_resp[0]= 0.0;
		c->layer_annual_fine_root_aut_resp[0]= 0.0;
		c->layer_annual_coarse_root_aut_resp[0]= 0.0;

	}
	if (c->annual_layer_number == 2)
	{
		if (year == 0 || previous_layer_number != c->annual_layer_number)
		{
			logger(g_annual_log, "\n%s \t%4s \t%4s",
					"YEAR", "HC(1)", "HC(0)");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_annual_log, "\t%3s", "NEE");
			}

			logger(g_annual_log, "\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s",
					"GPP(1)", "GPP(0)", "GPP(tot)", "AR(1)", "AR(0)", "AR(tot)");

			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_annual_log, "\t%3s, \t%3s", "HR (tot)", "Reco");
			}

			logger(g_annual_log, "\t%4s \t%4s\t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s \t%7s \t%4s \t%4s \t%4s \t%4s \t%4s \t%4s\n",
					"Y(1)", "Y(0)", "Y(tot)", "NPP(1)", "NPP(0)", "NPP (tot)", "NPP(gC/m2yr)", "CE(1)", "CE(0)", "CE (tot)", "ASW",
					"Wf", "PEAK_LAI(1)", "PEAK_LAI(0)", "CC(1)", "CC(0)", "DEAD TREE(1)", "DEAD TREE(0)", "DEAD TREE(tot)",
					"Wres(1)", "Wres(0)");

		}

		logger(g_annual_log, "%d \t%4d \t%4d", c->years[year].year,
				c->height_class_in_layer_dominant_counter, c->height_class_in_layer_dominated_counter);
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_annual_log, "\t%6.2f", c->annual_nee);
		}

		logger(g_annual_log, "\t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f  \t%4.2f",

				c->layer_annual_gpp[1],c->layer_annual_gpp[0], c->annual_gpp,
				c->layer_annual_aut_resp[1],c->layer_annual_aut_resp[0], c->annual_aut_resp);

		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_annual_log, "\t%10.2f \t%10.2f", c->annual_het_resp, c->annual_r_eco);
		}

		logger(g_annual_log, "\t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f"
				" \t%4.2f \t%2.2f \t%4.2f \t%4.2f \t%4.2f \t%4.2f \t%12d \t%12d \t%12d \t%4.2f \t%4.2f\n",

				(c->layer_annual_aut_resp[1]*100)/ c->layer_annual_gpp[1],(c->layer_annual_aut_resp[0]*100)/ c->layer_annual_gpp[0],(c->annual_aut_resp*100)/c->annual_gpp,
				c->layer_annual_npp_tDM[1], c->layer_annual_npp_tDM[0],	c->annual_npp_tDM, c->annual_npp_gC,
				c->layer_annual_c_evapotransp[1], c->layer_annual_c_evapotransp[0], c->annual_c_evapotransp,
				c->asw,
				c->annual_tot_w_flux,
				c->annual_peak_lai[1], c->annual_peak_lai[0],
				c->layer_annual_cc[1], c->layer_annual_cc[0],
				c->layer_annual_dead_tree[1], c->layer_annual_dead_tree[0], c->annual_dead_tree,
				c->annual_layer_reserve_c[1], c->annual_layer_reserve_c[0]);

		previous_layer_number = c->annual_layer_number;

		//compute average
		avg_gpp[1] += c->layer_annual_gpp[1];
		avg_ar[1] += c->layer_annual_aut_resp[1];
		avg_cf[1] += c->layer_annual_c_flux[1];
		avg_npp[1] += c->layer_annual_npp_tDM[1];
		avg_npp_tot_gC += c->annual_npp_gC;
		avg_ce[1] += c->layer_annual_c_evapotransp[1];
		avg_gpp[0] += c->layer_annual_gpp[0];
		avg_ar[0] += c->layer_annual_aut_resp[0];
		avg_cf[0] += c->layer_annual_c_flux[0];
		avg_npp[0] += c->layer_annual_npp_tDM[0];
		avg_ce[0] += c->layer_annual_c_evapotransp[0];
		avg_gpp_tot += c->layer_annual_gpp[1] + c->layer_annual_gpp[0];
		avg_ar_tot += c->layer_annual_aut_resp[1] + c->layer_annual_aut_resp[0];
		avg_npp_tot += c->layer_annual_npp_tDM[1] + c->layer_annual_npp_tDM[0];

		avg_ce_tot += c->layer_annual_c_evapotransp[1] + c->layer_annual_c_evapotransp[0];
		tot_dead_tree_tot += c->annual_dead_tree;


		//reset
		c->layer_annual_gpp[1] = 0;
		c->layer_annual_aut_resp[1] = 0;
		c->layer_annual_c_flux[1] = 0;
		c->layer_annual_npp_tDM[1] = 0;
		c->layer_annual_c_evapotransp[1] = 0;
		c->layer_annual_cc[1] = 0;
		c->layer_annual_dead_tree[1] = 0;
		c->annual_delta_ws[1] = 0;
		c->annual_layer_stem_c[1] = 0;
		c->annual_layer_reserve_c[1] = 0;

		c->layer_annual_gpp[0] = 0;
		c->layer_annual_aut_resp[0] = 0;
		c->layer_annual_c_flux[0] = 0;
		c->layer_annual_npp_tDM[0] = 0;
		c->layer_annual_c_evapotransp[0] = 0;
		c->layer_annual_cc[0] = 0;
		c->layer_annual_dead_tree[0] = 0;
		c->annual_delta_ws[0] = 0;
		c->annual_layer_stem_c[0] = 0;
		c->annual_layer_reserve_c[0] = 0;
		c->annual_peak_lai[1] = 0;
		c->annual_peak_lai[0] = 0;

	}


	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 3)
	{
		if (year == 0 || previous_layer_number != c->annual_layer_number)
		{
			logger(g_annual_log, "\n%s \t%2s \t%2s \t%2s",
					"YEAR", "HC(2)", "HC(1)", "HC(0)");

			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_annual_log, "\t%3s", "NEE");
			}

			logger(g_annual_log, "\t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s",
					"GPP(2)","GPP(1)", "GPP(0)", "GPP (tot)", "AR(2)", "AR(1)", "AR(0)", "AR(tot)");

			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_annual_log, "\t%3s, \t%3s", "HR (tot)", "Reco");
			}

			logger(g_annual_log, "\t%2s \t%s \t%s \t%s \t%s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s "
					"\t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s \t%2s\n",
					"Cf(2)", "Cf(1)", "Cf(0)", "Cf(tot)", "Y(2)", "Y(1)", "Y(0)", "Y(tot)","NPP(2)", "NPP(1)", "NPP(0)", "NPP (tot)",
					"CE(2)", "CE(1)", "CE(0)", 	"CE(tot)", "ASW", "Wf",  "PEAK_LAI(2)", "PEAK_LAI(1)", "PEAK_LAI(0)",
					"CC(2)", "CC(1)", "CC(0)", "DEAD TREE(2)","DEAD TREE(1)",
					"DEAD TREE(0)", "DEAD TREE(tot)", "Wres(2)", "Wres(1)", "Wres(0)");

		}

		logger(g_annual_log, "%d \t%2d \t%2d \t%2d", c->years[year].year, c->height_class_in_layer_dominant_counter,c->height_class_in_layer_dominated_counter, c->height_class_in_layer_subdominated_counter);
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_annual_log, "\t%6.2f", c->annual_nee);
		}

		logger(g_annual_log, "\t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f ",
				c->layer_annual_gpp[2], c->layer_annual_gpp[1],c->layer_annual_gpp[0], c->layer_annual_gpp,
				c->layer_annual_aut_resp[2], c->layer_annual_aut_resp[1],c->layer_annual_aut_resp[0], c->annual_aut_resp);

		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_annual_log, "\t%10.2f \t%10.2f", c->annual_het_resp, c->annual_r_eco);
		}

		logger(g_annual_log, "\t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f "
				" \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2.2f \t%2d \t%2d \t%2d \t%2d "
				" \t%5.2f \t%5.2f \t%5.2f \t%5.2f \t%5.2f \n",
				c->layer_annual_c_flux[2], c->layer_annual_c_flux[1], c->layer_annual_c_flux[0], c->annual_C_flux,
				(c->layer_annual_aut_resp[2]*100.0)/c->layer_annual_gpp[2],((c->layer_annual_aut_resp[1]*100.0)/c->layer_annual_gpp[1]),((c->layer_annual_aut_resp[0]*100.0)/c->layer_annual_gpp[0]),(c->annual_aut_resp*100.0)/c->annual_gpp,
				c->layer_annual_npp_tDM[2], c->layer_annual_npp_tDM[1],	c->layer_annual_npp_tDM[0], c->annual_npp_tDM,
				c->layer_annual_c_evapotransp[2],c->layer_annual_c_evapotransp[1], c->layer_annual_c_evapotransp[0], c->annual_c_evapotransp,
				c->asw, c->annual_tot_w_flux,
				c->annual_peak_lai[2], c->annual_peak_lai[1], c->annual_peak_lai[0],
				c->layer_annual_cc[2],c->layer_annual_cc[1], c->layer_annual_cc[0],
				c->layer_annual_dead_tree[2], c->layer_annual_dead_tree[1], c->layer_annual_dead_tree[0], c->annual_dead_tree,
				c->annual_layer_reserve_c[2], c->annual_layer_reserve_c[1], c->annual_layer_reserve_c[0]);

		previous_layer_number = c->annual_layer_number;

		//compute average
		avg_gpp[2] += c->layer_annual_gpp[2];
		avg_ar[2] += c->layer_annual_aut_resp[2];
		avg_cf[2] += c->layer_annual_c_flux[2];
		avg_npp[2] += c->layer_annual_npp_tDM[2];
		avg_ce[2] += c->layer_annual_c_evapotransp[2];
		avg_gpp[1] += c->layer_annual_gpp[1];
		avg_ar[1] += c->layer_annual_aut_resp[1];
		avg_cf[1] += c->layer_annual_c_flux[1];
		avg_npp[1] += c->layer_annual_npp_tDM[1];
		avg_ce[1] += c->layer_annual_c_evapotransp[1];
		avg_gpp[0] += c->layer_annual_gpp[0];
		avg_ar[0] += c->layer_annual_aut_resp[0];
		avg_cf[0] += c->layer_annual_c_flux[0];
		avg_npp[0] += c->layer_annual_npp_tDM[0];
		avg_ce[0] += c->layer_annual_c_evapotransp[0];
		avg_gpp_tot += c->layer_annual_gpp[2] +c->layer_annual_gpp[1] + c->layer_annual_gpp[0];
		avg_ar_tot += c->layer_annual_aut_resp[2] +c->layer_annual_aut_resp[1] + c->layer_annual_aut_resp[0];
		avg_cf_tot += c->layer_annual_c_flux[2] +c->layer_annual_c_flux[1] + c->layer_annual_c_flux[0];
		avg_npp_tot += c->layer_annual_npp_tDM[2] +c->layer_annual_npp_tDM[1] + c->layer_annual_npp_tDM[0];
		avg_ce_tot += c->layer_annual_c_evapotransp[2] + c->layer_annual_c_evapotransp[1] + c->layer_annual_c_evapotransp[0];
		tot_dead_tree_tot += c->annual_dead_tree;

		//reset
		c->layer_annual_gpp[2] = 0;
		c->layer_annual_aut_resp[2] = 0;
		c->layer_annual_c_flux[2] = 0;
		c->layer_annual_npp_tDM[2] = 0;
		c->layer_annual_c_evapotransp[2] = 0;
		c->layer_annual_cc[2] = 0;
		c->layer_annual_dead_tree[2] = 0;
		c->annual_delta_ws[2] = 0;
		c->annual_layer_stem_c[2] = 0;
		c->annual_layer_reserve_c[2] = 0;

		c->layer_annual_gpp[1] = 0;
		c->layer_annual_aut_resp[1] = 0;
		c->layer_annual_c_flux[1] = 0;
		c->layer_annual_npp_tDM[1] = 0;
		c->layer_annual_c_evapotransp[1] = 0;
		c->layer_annual_cc[1] = 0;
		c->layer_annual_dead_tree[1] = 0;
		c->annual_delta_ws[1] = 0;
		c->annual_layer_stem_c[1] = 0;
		c->annual_layer_reserve_c[1] = 0;

		c->layer_annual_gpp[0] = 0;
		c->layer_annual_aut_resp[0] = 0;
		c->layer_annual_c_flux[0] = 0;
		c->layer_annual_npp_tDM[0] = 0;
		c->layer_annual_c_evapotransp[0] = 0;
		c->layer_annual_cc[0] = 0;
		c->layer_annual_dead_tree[0] = 0;
		c->annual_delta_ws[0] = 0;
		c->annual_layer_stem_c[0] = 0;
		c->annual_layer_reserve_c[0] = 0;
		c->annual_peak_lai[2] = 0;
		c->annual_peak_lai[1] = 0;
		c->annual_peak_lai[0] = 0;

	}
	c->annual_gpp = 0;
	c->annual_aut_resp = 0;
	c->annual_aut_resp_tC = 0.0;
	c->annual_C_flux = 0;
	c->annual_npp_tDM = 0;
	c->annual_npp_gC = 0;
	c->annual_c_evapotransp = 0;
	c->annual_tot_w_flux = 0;
	c->annual_dead_tree = 0;

	if (!string_compare_i(g_settings->dndc, "on"))
	{
		c->annual_het_resp = 0;
		c->annual_r_eco = 0;
		c->annual_nee = 0;
	}
	//aaa
	//compute average values
	/*if (years == years_of_simulation -1 && years_of_simulation > 1)
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
			logger(g_annual_log, "-----------------------------------------------------------------------------------------------------------------------------------------------------"
					"---------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			logger(g_annual_log, "AVG/TOT \t\t%5f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t\t\t\t\t%49d\n",
					avg_gpp_tot, avg_ar_tot, avg_cf_tot, (avg_ar_tot*100.0)/avg_gpp_tot, avg_npp_tot, ((avg_npp_tot/g_settings->sizeCell)*1000000)/GC_GDM , avg_ce_tot, tot_dead_tree_tot);
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
			logger(g_annual_log, "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------,"
					"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			logger(g_annual_log, "AVG/TOT \t\t\t%14f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t\t\t\t\t\t\t\t\t%76d \n",
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

			logger(g_annual_log, "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
			logger(g_annual_log, "AVG/TOT \t\t\t\t%17f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t%10f \t\t\t\t\t\t\t\t\t%42d \n",
					avg_gpp[2], avg_gpp[1], avg_gpp[0], avg_gpp_tot, avg_ar[2], avg_ar[1], avg_ar[0], avg_ar_tot,
					avg_cf[2], avg_cf[1], avg_cf[0], avg_cf_tot,
					(avg_ar[2]*100.0)/avg_gpp[2], (avg_ar[1]*100.0)/avg_gpp[1], (avg_ar[0]*100.0)/avg_gpp[0], (avg_ar_tot*100.0)/avg_gpp_tot,
					avg_npp[2], avg_npp[1], avg_npp[0], avg_npp_tot, avg_ce[2], avg_ce[1], avg_ce[0], avg_ce_tot, tot_dead_tree_tot);		//
					//}
		}
	}*/
}

void EOM_cumulative_balance_cell_level(cell_t *const c, const int years, const int month, const int cell_index)
{
	static int previous_layer_number;

	/*if(month == 0 && years == 0)
	{
		logger(g_monthly_log, "Site name = %s\n", g_soil_settings->values[SOIL_sitename);
		logger(g_monthly_log, "Monthly summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",g_settings->version, g_settings->time, g_settings->spatial);
		logger(g_monthly_log, "\n\nCell %d, %d, Lat = %f, Long  = %f\n\n\n", c->x, c->y, g_soil_settings->values[SOIL_lat, g_soil_settings->values[SOIL_lon );
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_monthly_log, "Monthly NEE = Monthly total net ecosystem exchange (gC/m2/month)\n");
		}
		logger(g_monthly_log, "Monthly GPP = monthly total gross primary production (gC/m2/month)\n");
		logger(g_monthly_log, "Monthly AR = monthly total autotrophic respiration (gC/m2/month)\n");
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_monthly_log, "Annual HR = Monthly total heterotrophic respiration (gC/m2/month)\n");
			logger(g_monthly_log, "Annual Reco = Monthly total ecosystem respiration (gC/m2/month)\n");
		}
		logger(g_monthly_log, "Monthly Cf = monthly c-fluxes (gC/m2/month)\n");
		logger(g_monthly_log, "Monthly NPP = monthly total net primary production (tDM/m2/month)\n");
		logger(g_monthly_log, "Monthly CE = monthly canopy evapotranspiration(mm/month)\n");
		logger(g_monthly_log, "Monthly ASW = monthly ASW (at the end of month) (mm)\n");
		logger(g_monthly_log, "Monthly Cw = monthly w-fluxes (mm/m2/month)\n");
		logger(g_monthly_log, "Monthly DEAD TREE = monthly dead tree (n tree/cell)\n\n\n");
	}*/
	if (years == 0)
	{
		previous_layer_number = c->annual_layer_number;
	}
	else
	{
		//check if layer number is changed since last yearly run
		if(previous_layer_number != c->annual_layer_number)
		{
			logger(g_monthly_log, "\n\nANNUAL_LAYER_NUMBER_IS_CHANGED_SINCE_PREVIOUS_YEAR!!!\n\n");
		}
		previous_layer_number = c->annual_layer_number;
	}

	if (c->annual_layer_number == 1)
	{
		if (month == 0 && years == 0 && cell_index == 0)
		{
			logger(g_monthly_log, "%s \t%2s \t%s", "YEAR", "Month", "cell_x");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_monthly_log, "\t%3s", "NEE");
			}
			logger(g_monthly_log, "\t%6s \t%10s" ,
					"GPP (tot)", "AR (tot)");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_monthly_log, "\t%3s, \t%3s", "HR (tot)", "Reco");
			}
			logger(g_monthly_log, "\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n",
					"Cf(tot)", "NPP(tot)", "NPPgC", "CE(tot)", "LE", "ASW", "Cw", "CC(0)", "DEAD TREE(tot)");
		}
		logger(g_monthly_log, "%d \t%2d \t%4d", c->years[years].year, month+1, c->x);
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_monthly_log, "\t%6.2f", c->monthly_nee);
		}
		logger(g_monthly_log, "\t%10.2f \t%10.2f",
				c->monthly_gpp,
				c->monthly_aut_resp);

		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_monthly_log, "\t%10.2f \t%10.2f", c->monthly_het_resp, c->monthly_r_eco);
		}
		logger(g_monthly_log, "\t%14.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f\n",
				c->monthly_C_flux,
				c->monthly_npp_tDM,
				c->monthly_npp_gC,
				c->monthly_c_evapotransp,
				c->monthly_latent_heat_flux,
				c->swc,
				c->monthly_tot_w_flux,
				c->layer_monthly_cc[0]);

		//reset
		c->layer_monthly_gpp[0] = 0;
		c->layer_monthly_aut_resp[0] = 0;
		c->layer_monthly_npp_tDM[0] = 0;
		c->layer_annual_c_evapotransp[0] = 0;
		c->layer_monthly_cc[0] = 0;
		c->layer_monthly_dead_tree[0] = 0;
	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 2)
	{
		if (month == 0 && years == 0)
		{
			logger(g_monthly_log, "%s \t%2s", "YEAR", "Month");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_monthly_log, "\t%3s", "NEE");
			}
			logger(g_monthly_log, "\t%6s \t%10s \t%10s \t%10s \t%10s \t%10s" ,
					"GPP(1)", "GPP(0)", "GPP(tot)",  "AR(1)", "AR(0)", "AR(tot)");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_monthly_log, "\t%3s, \t%3s", "HR (tot)", "Reco");
			}
			logger(g_monthly_log, "\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n",
					"Cf(1)", "Cf(0)", "Cf(tot)", "NPP(1)", "NPP(0)", "NPP(tot)", "CE(1)",
					"CE(0)", "CE(tot)", "ASW", "Cw", "CC(1)", "CC(0)", "DEAD TREE(1)", "DEAD TREE(0)", "DEAD TREE(tot)");
		}
		logger(g_monthly_log, "%d \t%2d", c->years[years].year, month+1);
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_monthly_log, "\t%6.2f", c->monthly_nee);
		}
		logger(g_monthly_log, "\t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f",
				c->layer_monthly_gpp[1],c->layer_monthly_gpp[0], c->monthly_gpp,
				c->layer_monthly_aut_resp[1],c->layer_monthly_aut_resp[0], c->monthly_aut_resp);

		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_monthly_log, "\t%10.2f \t%10.2f ", c->monthly_het_resp, c->monthly_r_eco);
		}
		logger(g_monthly_log, "\t%14.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f"
				" \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2d \t%11.2d \t%11.2d\n",
				c->layer_monthly_c_flux[1], c->layer_monthly_c_flux[0], c->monthly_C_flux,
				c->layer_monthly_npp_tDM[1], c->layer_monthly_npp_tDM[0],c->monthly_npp_tDM,
				c->layer_annual_c_evapotransp[1], c->layer_annual_c_evapotransp[0], c->monthly_c_evapotransp,
				c->asw,
				c->monthly_tot_w_flux,
				c->layer_monthly_cc[1], c->layer_monthly_cc[0],
				c->layer_monthly_dead_tree[1], c->layer_monthly_dead_tree[0], c->monthly_dead_tree);

		//reset
		c->layer_monthly_gpp[1] = 0;
		c->layer_monthly_aut_resp[1] = 0;
		c->layer_monthly_c_flux[1] = 0;
		c->layer_monthly_npp_tDM[1] = 0;
		c->layer_annual_c_evapotransp[1] = 0;
		c->layer_monthly_cc[1] = 0;
		c->layer_monthly_dead_tree[1] = 0;

		c->layer_monthly_gpp[0] = 0;
		c->layer_monthly_aut_resp[0] = 0;
		c->layer_monthly_c_flux[0] = 0;
		c->layer_monthly_npp_tDM[0] = 0;
		c->layer_annual_c_evapotransp[0] = 0;
		c->layer_monthly_cc[0] = 0;
		c->layer_monthly_dead_tree[0] = 0;
	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 3)
	{
		if (month == 0 && years == 0)
		{
			logger(g_monthly_log, "\n%s \t%2s", "YEAR", "Month");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_monthly_log, "\t%3s", "NEE");
			}
			logger(g_monthly_log, "\t%6s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s" ,
					"GPP(2)","GPP(1)", "GPP(0)", "GPP (tot)", "AR(2)","AR(1)", "AR(0)", "AR(tot)");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_monthly_log, "\t%3s, \t%3s", "HR (tot)", "Reco");
			}
			logger(g_monthly_log, "\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s "
					"\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n",
					"Cf(2)", "Cf(1)", "Cf(0)", "Cf(tot)", "NPP(2)","NPP(1)", "NPP(0)","NPP (tot)", "CE(2)","CE(1)", "CE(0)", "CE(tot)",
					"ASW", "Cw", "CC(2)", "CC(1)", "CC(0)", "DEAD TREE(2)","DEAD TREE(1)", "DEAD TREE(0)", "DEAD TREE(tot)");
		}
		logger(g_monthly_log, "%d \t%2d", c->years[years].year, month+1);
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_monthly_log, "\t%6.2f", c->monthly_nee);
		}
		logger(g_monthly_log, "\t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f",
				c->layer_monthly_gpp[2], c->layer_monthly_gpp[1],c->layer_monthly_gpp[0], c->monthly_gpp,
				c->layer_monthly_aut_resp[2], c->layer_monthly_aut_resp[1],c->layer_monthly_aut_resp[0], c->monthly_aut_resp);

		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_monthly_log, "\t%10.2f \t%10.2f ", c->monthly_het_resp, c->monthly_r_eco);
		}
		logger(g_monthly_log, "\t%14.2f  \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f"
				" \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2d \t%11.2d \t%11.2d \t%11.2d\n",
				c->layer_monthly_c_flux[2],c->layer_monthly_c_flux[1], c->layer_monthly_c_flux[0], c->monthly_C_flux,
				c->layer_monthly_npp_tDM[2], c->layer_monthly_npp_tDM[1], c->layer_monthly_npp_tDM[0], c->monthly_npp_tDM,
				c->layer_annual_c_evapotransp[2],c->layer_annual_c_evapotransp[1], c->layer_annual_c_evapotransp[0], c->monthly_c_evapotransp,
				c->asw,
				c->monthly_tot_w_flux,
				c->layer_monthly_cc[2], c->layer_monthly_cc[1], c->layer_monthly_cc[0],
				c->layer_monthly_dead_tree[2], c->layer_monthly_dead_tree[1], c->layer_monthly_dead_tree[0], c->monthly_dead_tree);

		//reset
		c->layer_monthly_gpp[2] = 0;
		c->layer_monthly_aut_resp[2] = 0;
		c->layer_monthly_c_flux[2] = 0;
		c->layer_monthly_npp_tDM[2] = 0;
		c->layer_annual_c_evapotransp[2] = 0;
		c->layer_monthly_cc[2] = 0;
		c->layer_monthly_dead_tree[2] = 0;

		c->layer_monthly_gpp[1] = 0;
		c->layer_monthly_aut_resp[1] = 0;
		c->layer_monthly_c_flux[1] = 0;
		c->layer_monthly_npp_tDM[1] = 0;
		c->layer_annual_c_evapotransp[1] = 0;
		c->layer_monthly_cc[1] = 0;
		c->layer_monthly_dead_tree[1] = 0;

		c->layer_monthly_gpp[0] = 0;
		c->layer_monthly_aut_resp[0] = 0;
		c->layer_monthly_c_flux[0] = 0;
		c->layer_monthly_npp_tDM[0] = 0;
		c->layer_annual_c_evapotransp[0] = 0;
		c->layer_monthly_cc[0] = 0;
		c->layer_monthly_dead_tree[0] = 0;
	}
	c->monthly_gpp = 0;
	c->monthly_aut_resp = 0;
	c->monthly_C_flux = 0;
	c->monthly_npp_tDM = 0;
	c->monthly_npp_gC = 0;
	c->monthly_c_evapotransp = 0;
	c->monthly_tot_w_flux = 0;
	c->monthly_dead_tree = 0;

	if (!string_compare_i(g_settings->dndc, "on"))
	{
		c->monthly_het_resp = 0;
		c->monthly_r_eco = 0;
		c->monthly_nee = 0;
	}
}

void EOD_cumulative_balance_cell_level(cell_t *const c, const int years, const int month, const int day, const int cell_index)
{
	static int previous_layer_number;
	static int doy;

	if(day  == 0 && month == 0 && years == 0)
	{

		//logger(g_daily_log, "Site name = %s\n", g_soil_settings->values[SOIL_sitename);
		//logger(g_daily_log, "Daily summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",g_settings->version, g_settings->time, g_settings->spatial);
		//logger(g_daily_log, "\n\nCell %d, %d, Lat = %f, Long  = %f\n\n\n", c->x, c->y, g_soil_settings->values[SOIL_lat, g_soil_settings->values[SOIL_lon );


		//logger(g_daily_log, "HC\n");
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			//logger(g_daily_log, "Daily NEE = daily total net ecosystem exchange (gC/m2/day)\n");
		}
		//logger(g_daily_log, "Daily GPP = daily total gross primary production (gC/m2/day)\n");
		//logger(g_daily_log, "Daily AR = daily total autotrophic respiration (gC/m2/day)\n");
		//logger(g_daily_log, "Daily ARtDM = daily total autotrophic respiration (tDM/day cell)\n");
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			//logger(g_daily_log, "Daily HR = daily total heterotrophic respiration (gC/m2/day)\n");
			//logger(g_daily_log, "Daily Reco = daily total ecosystem respiration (gC/m2/day)\n");
		}
		/*
		logger(g_daily_log, "Daily Cf = daily c-fluxes (gC/m2/day)\n");
		logger(g_daily_log, "Daily CftDM = daily c-fluxes (tDM/day cell)\n");
		logger(g_daily_log, "Daily NPP = daily total net primary production (tDM/m2/day)\n");
		logger(g_daily_log, "Daily CE = daily canopy evapotranspiration(mm/day)\n");
		logger(g_daily_log, "Daily LE = daily latent heat (W/m^2)\n");
		logger(g_daily_log, "Daily ASW = daily Available Soil Water(mm/day)\n");
		logger(g_daily_log, "Daily Wfl = daily water fluxes (mm/day)\n");
		logger(g_daily_log, "Daily LAI = daily Leaf Area Index (m^2/m^2)\n");
		logger(g_daily_log, "Daily D-Wf = daily fraction of NPP to foliage pool (tDM/day cell)\n");
		logger(g_daily_log, "Daily D-Ws = daily fraction of NPP to stem pool (tDM/day cell)\n");
		logger(g_daily_log, "Daily D-Wbb = daily fraction of NPP to branch and bark pool (tDM/day cell)\n");
		logger(g_daily_log, "Daily D-Wfr = daily fraction of NPP to fine root pool (tDM/day cell)\n");
		logger(g_daily_log, "Daily D-Wcr = daily fraction of NPP to coarse root pool (tDM/day cell)\n");
		logger(g_daily_log, "Daily D-Wres = daily fraction of NPP to reserve pool (tDM/day cell)\n");
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
			logger(g_daily_log, "\n\nANNUAL_LAYER_NUMBER_IS_CHANGED_SINCE_PREVIOUS_YEAR!!!\n\n");
		}
	}
	if (c->annual_layer_number == 1)
	{
		if (((day == 0 && month == 0 && years == 0) || previous_layer_number != c->annual_layer_number) && cell_index == 0)
		{
			//todo include x and y log if cells > 1
			logger(g_daily_log, "%s \t%2s \t%2s \t%6s", "YEAR", "MONTH", "DAY", /*"CELL",*/ "HC");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_daily_log, "\t%3s", "NEE");
			}
			logger(g_daily_log, "\t%4s \t%6s \t%10s \t%8s",
					"GPP(gC/m2d)", "AR(gC/m2d)","MR(gC/m2d)", "GR(gC/m2d)");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_daily_log, "\t%s, \t%3s", "HR (tot)", "Reco");
			}
			logger(g_daily_log, "\t%6s \t%6s \t%4s \t%8s \t%6s\t%6s \t%10s "
					"\t%5s \t%8s \t%10s \t%11s \t%11s \t%11s \t%11s \t%10s \t%11s \t%11s \t%11s \t%11s\n",
					"NPP(gC/m2d)", "ET(mm/m2/d)","LE(W/m2)", "SWC(%)", "LAI",
					"CC", "DEADTREE", "D-Wf", "D-Ws", "D-Wbb", "D-Wfr", "D-Wcr", "D-Wres", "Wres", "wlAR", "wsAR", "wbbAR", "wfrAR", "wcrAR");
		}
		if ((day == 0 && month == 0) || previous_layer_number != c->annual_layer_number)
		{
			doy = 1;
		}

		logger(g_daily_log, "%d \t%3d \t%4d \t%3d", c->years[years].year, month+1, day+1,/* c->x, c->y,*/ c->height_class_in_layer_dominant_counter);
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_daily_log, "\t%6.2f", c->daily_nee);
		}
		logger(g_daily_log, "\t%10.4f \t%10.4f \t%10.4f \t%10.4f",
				c->layer_daily_gpp[0],
				c->layer_daily_aut_resp[0],
				c->layer_daily_maint_resp[0],
				c->layer_daily_growth_resp[0]);

		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_daily_log, "\t%10.2f \t%10.2f", c->daily_het_resp, c->daily_r_eco);
		}
		logger(g_daily_log, "\t%11.4f \t%8.4f \t%10.4f \t%11.4f \t%7.4f \t%7.4f \t%2.2d \t%9.4f \t%8.4f \t%10.4f"
				" \t%11.4f \t%11.4f \t%11.4f \t%11.4f \t%10.4f \t%11.4f \t%11.4f \t%11.4f \t%11.4f\n",
				c->layer_daily_npp_gC[0],
				c->daily_et,
				c->daily_latent_heat_flux,
				c->swc,
				c->daily_lai[0],
				c->layer_daily_cc[0]*100,
				c->layer_daily_dead_tree[0],
				c->daily_delta_wf[0],
				c->daily_delta_ws[0],
				c->daily_delta_wbb[0],
				c->daily_delta_wfr[0],
				c->daily_delta_wcr[0],
				c->daily_delta_wres[0],
				c->daily_layer_reserve_c[0],
				c->daily_leaf_aut_resp,
				c->daily_stem_aut_resp,
				c->daily_branch_aut_resp,
				c->daily_fine_root_aut_resp,
				c->daily_coarse_root_aut_resp);
		//}

		previous_layer_number = c->annual_layer_number;

		//reset
		c->layer_daily_gpp[0] = 0;
		c->layer_daily_aut_resp[0] = 0;
		c->layer_daily_maint_resp[0] = 0;
		c->layer_daily_growth_resp[0] = 0;
		c->layer_daily_aut_resp_tC[0] = 0;
		c->layer_daily_c_flux_tDM[0] = 0;
		c->layer_daily_npp_tDM[0] = 0;
		c->layer_daily_npp_gC[0]= 0;
		c->layer_daily_c_int[0] = 0;
		c->layer_daily_c_transp[0] = 0;
		c->layer_daily_c_evapotransp[0] = 0;
		c->layer_daily_et[0] = 0;
		c->layer_daily_cc[0] = 0;
		c->layer_daily_dead_tree[0] = 0;
	}
	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 2)
	{
		if ((day == 0 && month == 0 && years == 0) || previous_layer_number != c->annual_layer_number)
		{
			logger(g_daily_log, "\n%s \t%s \t%2s \t%2s \t%2s \t%2s", "DOY", "YEAR", "MONTH", "DAY", "HC(1)", "HC(0)");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_daily_log, "\t%3s", "NEE");
			}
			logger(g_daily_log, "\t%6s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s" ,
					"GPP(1)", "GPP(0)", "GPP(tot)", "AR(1)", "AR", "AR(tot)");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_daily_log, "\t%3s, \t%3s", "HR (tot)", "Reco");
			}
			logger(g_daily_log, "\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\t%10s \t%10s "
					"\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n",
					"Cf(1)", "Cf(0)", "Cf(tot)", "NPP(1)", "NPP(0)", "NPP(tot)","CE(1)", "CE(0)", "CE(tot)","ASW",
					"LAI(1)", "LAI(0)", "CC(1)", "CC(0)", "DEADTREE(1)", "DEADTREE(0)", "DEADTREE(tot)");
		}
		if ((day == 0 && month == 0) || previous_layer_number != c->annual_layer_number)
		{
			doy = 1;
		}

		logger(g_daily_log, "%d \t%8d \t%5d \t%5d  \t%5d \t%2d",doy, c->years[years].year, month+1, day+1,
				c->height_class_in_layer_dominant_counter,
				c->height_class_in_layer_dominated_counter);
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_daily_log, "\t%6.2f", c->daily_nee);
		}
		logger(g_daily_log, "\t%10.2f \t%10.2f \t%10.2f\t%10.2f\t%10.2f \t%10.2f",
				c->layer_daily_gpp[1], c->layer_daily_gpp[0], c->daily_gpp,
				c->layer_daily_aut_resp[1], c->layer_daily_aut_resp[0], c->daily_aut_resp);

		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_daily_log, "\t%10.2f \t%10.2f", c->daily_het_resp, c->daily_r_eco);
		}
		logger(g_daily_log, "\t%14.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f  \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%14.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2d \t%11.2d \t%11.2d\n",
				c->layer_daily_c_flux[1], c->layer_daily_c_flux[0], c->daily_C_flux,
				c->layer_daily_npp_tDM[1], c->layer_daily_npp_tDM[0], c->daily_npp_tDM,
				c->layer_daily_c_evapotransp[1], c->layer_daily_c_evapotransp[0], c->daily_c_evapotransp,
				c->asw,
				c->daily_lai[1], c->daily_lai[0],
				c->layer_daily_cc[1]*100, c->layer_daily_cc[0]*100,
				c->layer_daily_dead_tree[1], c->layer_daily_dead_tree[0], c->daily_dead_tree);

		//}

		previous_layer_number = c->annual_layer_number;

		//reset
		c->layer_daily_gpp[1] = 0;
		c->layer_daily_aut_resp[1] = 0;
		c->layer_daily_maint_resp[1] = 0;
		c->layer_daily_growth_resp[1] = 0;
		c->layer_daily_c_flux[1] = 0;
		c->layer_daily_npp_tDM[1] = 0;
		c->layer_daily_c_int[1] = 0;
		c->layer_daily_c_transp[1] = 0;
		c->layer_daily_c_evapotransp[1] = 0;
		c->layer_daily_et[1] = 0;
		c->layer_daily_cc[1] = 0;
		c->layer_daily_dead_tree[1] = 0;

		c->layer_daily_gpp[0] = 0;
		c->layer_daily_aut_resp[0] = 0;
		c->layer_daily_maint_resp[0] = 0;
		c->layer_daily_growth_resp[0] = 0;
		c->layer_daily_c_flux[0] = 0;
		c->layer_daily_npp_tDM[0] = 0;
		c->layer_daily_c_int[0] = 0;
		c->layer_daily_c_transp[0] = 0;
		c->layer_daily_c_evapotransp[0] = 0;
		c->layer_daily_et[0] = 0;
		c->layer_daily_cc[0] = 0;
		c->layer_daily_dead_tree[0] = 0;
	}

	//fixme model doesn't log correct value for more then one class within a layer
	if (c->annual_layer_number == 3)
	{
		if ((day == 0 && month == 0 && years == 0) || previous_layer_number != c->annual_layer_number)
		{
			logger(g_daily_log, "\n%s \t%2s\t%2s ", "YEAR", "MONTH", "DAY");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_daily_log, "\t%3s", "NEE");
			}
			logger(g_daily_log, "\t%6s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s" ,
					"GPP(2)","GPP(1)", "GPP(0)", "GPP(tot)", "AR(2)","AR(1)", "AR(0)", "AR(tot)");
			if (!string_compare_i(g_settings->dndc, "on"))
			{
				logger(g_daily_log, "\t%3s, \t%3s", "HR (tot)", "Reco");
			}
			logger(g_daily_log, "\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s "
					"\t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s \t%10s\n",
					"Cf(2)", "Cf(1)", "Cf(0)", "Cf(tot)", "NPP(2)","NPP(1)", "NPP(0)", "NPP(tot)", "NPPgC(2)","NPPgC(1)", "NPPgC(0)", "NPPgC(tot)",
					"CE(2)","CE(1)", "CE(0)", "CE(tot)", "ASW", "LAI(2)","LAI(1)", "LAI(0)", "CC(2)", "CC(1)", "CC(0)", "DEADTREE(2)", "DEADTREE(1)", "DEADTREE(0)", "DEADTREE(tot)");
		}
		logger(g_daily_log, "%d \t%2d \t%2d", c->years[years].year, month+1, day+1);
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_daily_log, "\t%6.2f", c->daily_nee);
		}
		logger(g_daily_log, "\t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f \t%10.2f",
				c->layer_daily_gpp[2], c->layer_daily_gpp[1],c->layer_daily_gpp[0], c->daily_gpp,
				c->layer_daily_aut_resp[2], c->layer_daily_aut_resp[1],c->layer_daily_aut_resp[0], c->daily_aut_resp);

		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_daily_log, "\t%10.2f \t%10.2f ", c->monthly_het_resp, c->monthly_r_eco);
		}
		logger(g_daily_log, "\t%14.2f  \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f"
				" \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f \t%11.2f"
				"\t%11.2f \t%11.2d \t%11.2d \t%11.2d \t%11.2d\n",
				c->layer_daily_c_flux[2],c->layer_daily_c_flux[1],c->layer_daily_c_flux[0],c->daily_C_flux,
				c->layer_daily_npp_tDM[2], c->layer_daily_npp_tDM[1],c->layer_daily_npp_tDM[0], c->daily_npp_tDM,
				c->layer_daily_npp_gC[2], c->layer_daily_npp_gC[1],c->layer_daily_npp_gC[0], c->daily_npp_gC,
				c->layer_daily_c_evapotransp[2],c->layer_daily_c_evapotransp[1], c->layer_daily_c_evapotransp[0], c->daily_c_evapotransp,
				c->asw,
				c->daily_lai[2], c->daily_lai[1], c->daily_lai[0],
				c->layer_daily_cc[2]*100, c->layer_daily_cc[1]*100,c->layer_daily_cc[0]*100,
				c->layer_daily_dead_tree[2], c->layer_daily_dead_tree[1], c->layer_daily_dead_tree[0], c->daily_dead_tree);

		previous_layer_number = c->annual_layer_number;

		//reset
		c->layer_daily_gpp[2] = 0;
		c->layer_daily_aut_resp[2] = 0;
		c->layer_daily_maint_resp[2] = 0;
		c->layer_daily_growth_resp[2] = 0;
		c->layer_daily_c_flux[2] = 0;
		c->layer_daily_npp_tDM[2] = 0;
		c->layer_daily_npp_gC[2] = 0;
		c->layer_daily_c_int[2] = 0;
		c->layer_daily_c_transp[2] = 0;
		c->layer_daily_c_evapotransp[2] = 0;
		c->layer_daily_et[2] = 0;
		c->layer_daily_cc[2] = 0;
		c->layer_daily_dead_tree[2] = 0;

		c->layer_daily_gpp[1] = 0;
		c->layer_daily_aut_resp[1] = 0;
		c->layer_daily_maint_resp[1] = 0;
		c->layer_daily_growth_resp[1] = 0;
		c->layer_daily_c_flux[1] = 0;
		c->layer_daily_npp_tDM[1] = 0;
		c->layer_daily_npp_gC[1] = 0;
		c->layer_daily_c_int[1] = 0;
		c->layer_daily_c_transp[1] = 0;
		c->layer_daily_c_evapotransp[1] = 0;
		c->layer_daily_et[1] = 0;
		c->layer_daily_cc[1] = 0;
		c->layer_daily_dead_tree[1] = 0;

		c->layer_daily_gpp[0] = 0;
		c->layer_daily_aut_resp[0] = 0;
		c->layer_daily_maint_resp[0] = 0;
		c->layer_daily_growth_resp[0] = 0;
		c->layer_daily_c_flux[0] = 0;
		c->layer_daily_npp_tDM[0] = 0;
		c->layer_daily_npp_gC[0] = 0;
		c->layer_daily_c_int[0] = 0;
		c->layer_daily_c_transp[0] = 0;
		c->layer_daily_c_evapotransp[0] = 0;
		c->layer_daily_et[0] = 0;
		c->layer_daily_cc[0] = 0;
		c->layer_daily_dead_tree[0] = 0;



	}

	c->daily_gpp = 0.0;
	c->daily_aut_resp = 0.0;
	c->daily_maint_resp = 0.0;
	c->daily_growth_resp = 0.0;
	c->daily_C_flux = 0.0;
	c->daily_npp_tDM = 0.0;
	c->daily_npp_gC = 0.0;
	c->daily_c_int = 0.0;
	c->daily_c_evapo = 0.0;
	c->daily_c_transp = 0.0;
	c->daily_c_evapotransp = 0.0;
	c->daily_et = 0.0;
	c->daily_dead_tree = 0;

	c->daily_latent_heat_flux = 0.0;

	if (!string_compare_i(g_settings->dndc, "on"))
	{
		c->daily_het_resp = 0.0;
		c->daily_r_eco = 0.0;
		c->daily_nee = 0.0;
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


void Get_EOD_soil_balance_cell_level(cell_t *const c, const int year, const int month, const int day)
{

	static int previous_layer_number;
	static int doy;
	int soil = 0;

	if(day  == 0 && month == 0 && year == 0)
	{
		if (!string_compare_i(g_settings->dndc, "on"))
		{
			logger(g_soil_log, "RUNNING DNDC.....\n");
			//fixSergio improve a multilayer based log, specular to the one used for the aboveground

		}
		else
		{
			logger(g_soil_log, "No soil simulation!!!\n");
		}
	}
	//		logger(g_daily_log, "Site name = %s\n", g_soil_settings->values[SOIL_sitename);
	//		logger(g_daily_log, "Daily summary output from 3D-CMCC version '%c', time '%c', spatial '%c'\n",g_settings->version, g_settings->time, g_settings->spatial);
	//		logger(g_daily_log, "\n\nCell %d, %d, Lat = %f, Long  = %f\n\n\n", c->x, c->y, g_soil_settings->values[SOIL_lat, g_soil_settings->values[SOIL_lon );
	//
	//
	//		logger(g_daily_log, "HC\n");
	//
	//		logger(g_daily_log, "Daily GPP = daily total gross primary production (gC/m2/day)\n");
	//		logger(g_daily_log, "Daily AR = daily total autotrophic respiration (gC/m2/day)\n");
	//		logger(g_daily_log, "Daily ARtC = daily total autotrophic respiration (tC/day cell)\n");
	//		logger(g_daily_log, "Daily Cf = daily c-fluxes (gC/m2/day)\n");
	//		logger(g_daily_log, "Daily CftDM = daily c-fluxes (tDM/day cell)\n");
	//		logger(g_daily_log, "Daily NPP = daily total net primary production (tDM/m2/day)\n");
	//		logger(g_daily_log, "Daily CE = daily canopy evapotranspiration(mm/day)\n");
	//		logger(g_daily_log, "Daily ASW = daily Available Soil Water(mm/day)\n");
	//		logger(g_daily_log, "Daily Wfl = daily water fluxes (mm/day)\n");
	//		logger(g_daily_log, "Daily LAI = daily Leaf Area Index (m^2/m^2)\n");
	//		logger(g_daily_log, "Daily D-Wf = daily fraction of NPP to foliage pool (tDM/day cell)\n");
	//		logger(g_daily_log, "Daily D-Ws = daily fraction of NPP to stem pool (tDM/day cell)\n");
	//		logger(g_daily_log, "Daily D-Wbb = daily fraction of NPP to branch and bark pool (tDM/day cell)\n");
	//		logger(g_daily_log, "Daily D-Wfr = daily fraction of NPP to fine root pool (tDM/day cell)\n");
	//		logger(g_daily_log, "Daily D-Wcr = daily fraction of NPP to coarse root pool (tDM/day cell)\n");
	//		logger(g_daily_log, "Daily D-Wres = daily fraction of NPP to reserve pool (tDM/day cell)\n");
	//
	//	}

	if (year == 0)
	{
		previous_layer_number = c->annual_layer_number;
	}
	else
	{
		//potentially useless for soil, but what if we consider erosion losses or soil deposition? maintain it
		//check if layer number is changed since last yearly run
		if(previous_layer_number != c->annual_layer_number)
		{
			logger(g_soil_log, "\n\nANNUAL_LAYER_NUMBER_IS_CHANGED_SINCE_PREVIOUS_YEAR!!!\n\n");
		}
	}
	if (!string_compare_i(g_settings->dndc, "on"))
	{
		//fixSergio improve a multilayer based log, specular to the one used for the aboveground
		if ((day == 0 && month == 0 && year == 0) || previous_layer_number != c->annual_layer_number)
		{
			//			soil_Log ("\n%s \t%s \t%2s \t%2s \t%2s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s "
			//					"\t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s"
			//					"\t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \n",
			//					"DOY", "YEAR", "MONTH", "DAY", "leafLittering","fineRootLittering", "woodLittering", "stemLittering", "coarseRootLittering", "day_C_mine",
			//					"day_N_mine", "day_N_assim", "day_soil_nh3", "day_clayn", "day_clay_N",
			//					"wrcvl", "wrcl", "wrcr", "wcrb", "wcrh","whumus", "wtcavai", "w_no3", "w_nh4", "w_nh3", "wsoc", "day_no2",
			//					"runoff_N", "previousSoilT", "soilSurfaceT", "temp_avet", "yr_avet","base_clay_N", "max_clay_N",
			//					"AddC", "AddCN", "AddC1", "AddC2", "AddC3");
			logger(g_soil_log, "\n%s \t%s \t%2s \t%2s \t%2s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s \t%8s "
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
		logger(g_soil_log, "%d \t%5d \t%3d \t%2d \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f "
				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f"
				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f"
				"\t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f \t%8.6f  \t%8.6f \n ",
				doy++, c->years[year].year, month+1, day+1,
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
				c->layer_daily_aut_resp[0],
				c->daily_gpp,
				c->daily_aut_resp,
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

		c->daily_gpp = 0;
		c->daily_aut_resp = 0;
		c->layer_daily_gpp[0] = 0;
		c->layer_daily_aut_resp[0] = 0;

	}
}
