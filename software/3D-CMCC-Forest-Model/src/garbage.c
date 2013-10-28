
/*
Log("**Wang et al eq** \n");
//sapwood area
//see Wang et al 2010
//versione Guido
sapwood_basal_area = fabs ((-1.05 * (log(10.0) * m->cells[cell].heights[height].ages[age].species[species].value[AVDBH]) + 2.10 ) / log(10.0));
Log("sapwood from Wang et al 2010 = %g cm^2\n", sapwood_basal_area);
heartwood_basal_area = (m->cells[cell].heights[height].ages[age].species[species].value[BASAL_AREA] * 10000) - sapwood_basal_area;
Log("heartwood from Wang et al 2010 = %g cm^2\n", heartwood_basal_area);
//versione Minchio
//sapwood_basal_area = pow (10, (-1.05 * (log(m->cells[cell].heights[height].ages[age].species[species].value[AVDBH]) + 2.10)));
//Log("sapwood = %g cm^2\n", sapwood_basal_area);



Log("Sapwood/Basal Area = %g \n", (sapwood_basal_area / 10000) / m->cells[cell].heights[height].ages[age].species[species].value[BASAL_AREA]);
Log("Sapwood/Basal Area = %g %%\n", ((sapwood_basal_area / 10000) / m->cells[cell].heights[height].ages[age].species[species].value[BASAL_AREA])*100 );
Log("Leaf Area from LPJ = %g m^2\n", (sapwood_basal_area / 10000) * m->cells[cell].heights[height].ages[age].species[species].value[SAP_LEAF]);
Log("Crown diameter = %g m^2\n", m->cells[cell].heights[height].ages[age].species[species].value[CROWN_DIAMETER_DBHDC_FUNC]);
Log("Crown Area = %g m^2\n", m->cells[cell].heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]);
Log("LAI from Wang et al 2010 = %g m^2\n", ((sapwood_basal_area / 10000) * m->cells[cell].heights[height].ages[age].species[species].value[SAP_LEAF]) / m->cells[cell].heights[height].ages[age].species[species].value[CROWN_AREA_DBHDC_FUNC]);
 */

/*

        //routine to split tree layers and soil
        //for ( strata = m->cells[cell].heights_count  ; strata >= 0; strata-- )
		//{
		    //if (strata > 0)
		    //{
		    //    tree_soil = 1;
		        //Log("strata tree = %d \n", strata);
		    //}
		    //last uneffective heights_count is for create soil layer
		    //else
		    //{
		    //    tree_soil = 0;
		        //Log("strata soil = %d \n", strata);
                //set soil layer = 0
            //    m->cells[cell].heights[height].z = 0;
		    //}

            //if (tree_soil > 0)
            //{
 */
/*
//CURRENTLY NOT USED
void M_Get_Partitioning_Allocation_NASACASA (SPECIES *const s, int z, float Stand_NPP)
{
	//NASA-CASA VERSION


	float const r0nasacasa = 0.3;
	float const s0nasacasa = 0.3;
	float pS_NASACASA;
	float pR_NASACASA;
	float pF_NASACASA;

	Log("(NASA-CASA) BIOMASS PARTITIONING-ALLOCATION FOR LAYER %d --\n", z);


	//Partitioning Ratio

	//Log("Light Trasmitted for this layer = %g\n", m->cells[cell].heights[height].ages[age].species[species].value[LIGHT_TRASM]);

	//Roots Ratio
	pR_NASACASA = (3 * r0nasacasa) * ( s->value[LIGHT_TRASM] / ( s->value[LIGHT_TRASM] + (2 * s->value[F_SW])));
	Log("Roots NASA-CASA ratio = %g %%\n", pR_NASACASA * 100);

	//Stem Ratio
	pS_NASACASA = ( 3 * s0nasacasa) * (s->value[F_SW] / (( 2 * s->value[LIGHT_TRASM] ) + s->value[F_SW]));
	Log("Stem NASA-CASA ratio = %g %%\n", pS_NASACASA * 100);

	//Foliage Ratio
	pF_NASACASA = 1 - ( pR_NASACASA + pS_NASACASA);
	Log("Foliage NASA-CASA ratio = %g %%\n", pF_NASACASA * 100);



	//Biomass Allocation

	//Log("Yearly Cumulated NPP for this layer  = %g tDM/area\n",  s->value[YEARLY_NPP]);

	s->value[BIOMASS_ROOTS_NASA_CASA] = Stand_NPP * pR_NASACASA;
	//Log("BiomassRoots NASA-CASA = %g tDM/area\n", s->value[BIOMASS_ROOTS_NASA_CASA]);

	s->value[BIOMASS_STEM_NASA_CASA] = Stand_NPP * pS_NASACASA;
	//Log("BiomassStem NASA-CASA = %g tDM/area\n", s->value[BIOMASS_STEM_NASA_CASA]);

	s->value[BIOMASS_FOLIAGE_NASA_CASA] = Stand_NPP * pF_NASACASA;
	//Log("BiomassFoliage NASA-CASA = %g tDM/area\n", s->value[BIOMASS_FOLIAGE_NASA_CASA]);

	Log("******************************\n");
}
*/

/*
//CURRENTLY NOT USED
void M_Get_Partitioning_Allocation_3PG (SPECIES *const s, int z, int years, int month, int management, float Stand_NPP, const MET_DATA *const met)
{
	//3PG VERSION
	float pfsPower;
	float pfsConst;
	static float emme;
	float pFS;
	float pR;
	float pS;
	float pF;
	float oldW;

	// control
	float RatioSum;

	//Log("\n-- (3PG) MONTHLY BIOMASS PARTITIONING-ALLOCATION FOR LAYER %d --\n", z, years);

	if (management == 0)
	{
		//Log("Management type = TIMBER\n");
	}
	else
	{
		//Log("Management type = COPPICE\n");
	}


	oldW = s->value[WF] + s->value[WS] + s->value[WRC] + s->value[WRF];


	//NOT USED IN MONTHLY PARTITIONING-ALLOCATION

	//annual average of physiological modifier for partitioning

	s->value[AVERAGE_PHYS_MOD] = s->value[YEARLY_PHYS_MOD] / s->counter[VEG_MONTHS];
	Log("Average Physmod = %g \n", s->value[AVERAGE_PHYS_MOD]);


	emme = site->m0 + (1 - site->m0) * site->fr;
	//Log("emme = %g\n", emme );

	// Calculate Partitioning Coefficients
	// il driver del partitioning sono: fSW-fVPD-fAGE
	// 'emme' è omogeneo per tutte le celle

	if ( s->value[LAI] <= (s->value[PEAK_Y_LAI] / 2.0 ))
	{
		//Log("**Maximum Growth**\n");
		//Log("allocating only into foliage pools\n");
		s->value[WF] += Stand_NPP;

		//Log("BiomassFoliage 3PG = %g tDM/area\n", s->value[WF] );
	}
	else
	{
		if (met[month].daylength > s->value[MINDAYLENGTH])
		{
			//timber routine
			if (management == 0)
			{
				pfsPower = log (s->value[PFS20] / s->value[PFS2]) / log (10.0);
				//Log("PFS20 = %g\n", s->value[PFS20]);
				//Log("PFS2 = %g\n", s->value[PFS2]);
				//Log("M0 = %d\n", site->m0);
				//Log("FR = %g\n", site->fr);
				//Log("pfsPower = %g\n", pfsPower );

				pfsConst = s->value[PFS2] / pow ( 2, pfsPower);
				//Log("pfsConst = %g\n", pfsConst );


				pFS = pfsConst * pow( s->value[AVDBH], pfsPower);     //Foliage:Stem Partitioning Ratio uguale per tutte le celle con ugual specie fisso per ogni anno
				//Log("pFS = %g\n", pFS);

				//IN MONTHLY RUOUTINE USE THE MONTHLY PHYSMOD

				// % di NPP to roots
				pR = s->value[PRX] * s->value[PRN] / (s->value[PRN] + (s->value[PRX] - s->value[PRN]) * s->value[PHYS_MOD] * emme);
				//Log("pR = %g\n", pR);
				//Log("PHYSMOD = %g\n", s->value[PHYS_MOD]);

				//Log("PRX = %g\n", s->value[PRX]);
				//Log("PRN = %g\n", s->value[PRN]);
				//Log("3PG ratio to roots layer %d = %g %%\n", z, pR * 100);
			}
			//end of timber routine
			//coppice routine
			else
			{
				//per ora la routine per i cedui non è dinamica, non considera infatti che
				//all'aumentare degli anni dal taglio i valori di PF2, PF20, PRX e PRN si
				//devono avvicinare a quelli della routine della fustaia
				//manca una variabile che tenga conto degli anni dal taglio tra i dati di
				//inizializzazione, forse si puo ovviare considerando gli anni dal taglio
				//considerandola come se fosse l'età

				pfsPower = log (s->value[PFS20_C] / s->value[PFS2_C]) / log (10.0);
				//Log("PFS20_C = %g\n", s->value[PFS20_C]);
				//Log("PFS2_C = %g\n", s->value[PFS2_C]);
				//Log("M0 = %d\n", site->m0);
				//Log("FR = %g\n", site->fr);

				//Log("pfsPower = %g\n", pfsPower );

				pfsConst = s->value[PFS2_C] / pow ( 2, pfsPower);
				//Log("pfsConst = %g\n", pfsConst );


				pFS = pfsConst * pow( s->value[AVDBH], pfsPower);     //Foliage:Stem Partitioning Ratio uguale per tutte le celle fisso per ogni anno
				//Log("pFS = %g\n", pFS);

				// % di NPP to roots
				pR = s->value[PRX_C] * s->value[PRN_C] / (s->value[PRN_C] + (s->value[PRX_C] - s->value[PRN_C]) * s->value[PHYS_MOD] * emme);
				//Log("pR = %g\n", pR);
				//Log("PRX_C = %g\n", s->value[PRX_C]);
				//Log("PRN_C = %g\n", s->value[PRN_C]);
				//Log("3PG ratio to roots for coppice = %g %%\n", pR * 100);
			}

			//end of coppice routine


			//if (fabs(APAR) < 0.000001) APAR = 0.000001  da 3PG ma che è????????????



			// % di NPP to stem
			pS = (1 - pR) / (1 + pFS);
			//Log("pS = %g\n", pS);
			//Log("3PG ratio to stem = %g %%\n", pS * 100);

			// % di NPP to foliage
			pF = 1 - pR - pS;
			//Log("pF = %g\n", m->lpCell[index].pF);
			//Log("3PG ratio to foliage = %g %%\n", pF * 100);

			RatioSum = pF + pS + pR;
			//Log("pF + pS + pR = %d\n", (int)RatioSum);
			if (RatioSum != 1)
			{
				//Log("Error in Biomass Partitioning!!\n");
			}

			//Log("End of month NPP for this layer  = %g tDM/area\n",  Stand_NPP);

			//Foliage compart
			//Log("Initial Foliage Biomass (Wf) = %g tDM/area\n", s->value[INITIAL_WF]);

			// Fraction of NPP to Foliage
			s->value[DEL_WF] = Stand_NPP * pF;
			//Log("Increment Yearly Biomass allocated (delWf) = %g tDM/area\n", s->value[DEL_WF]);


			// Total Foliage Biomass


			s->value[WF] = s->value[DEL_WF] + s->value[WF];


            if (!years)
            {
            if ( s->phenology == 0)
            {
                s->value[WF] = s->value[DEL_WF];
            }
            else
            {
                s->value[WF] = s->value[DEL_WF] + s->value[WF];
            }
            }
            else
            {
            if ( s->phenology == 0)
            {
                s->value[WF] = s->value[DEL_WF];
            }
            else
            {
                s->value[WF] = s->value[DEL_WF] + s->value[WF];
            }

            }




			//Log("Foliage Biomass (Wf) = %g tDM/area\n", s->value[WF]);



			//Roots compart
			//Log("Initial Root Biomass (Wr) = %g tDM/area\n", s->value[INITIAL_WR]);

			// Fraction of NPP to Roots
			s->value[DEL_WR] = Stand_NPP * pR;
			//Log("Increment Yearly Biomass allocated (delWr) = %g tDM/area\n", s->value[DEL_WR]);

			// Total Roots Biomass

			s->value[WRT] = s->value[WRT] + s->value[DEL_WR];
			//Log("Root Biomass (Wr) = %g tDM/area\n", s->value[WR]);



			//Stem compart
			//Log("Initial Stem Biomass (Ws) = %g tDM/area\n", s->value[INITIAL_WS]);

			// Fraction of NPP to Stem
			s->value[DEL_WS] = Stand_NPP * pS;
			//Log("Increment Yearly Biomass allocated (delWs) = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);

			//Total Stem Biomass
			//remove the part allocated to the branch and bark
			s->value[DEL_BB] = s->value[DEL_WS] * s->value[FRACBB];
			//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
			//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

			//allocation to stem
			s->value[DEL_WS] -= s->value[DEL_BB];
			s->value[WS] = s->value[WS] + s->value[DEL_WS];
			//Log("Increment Yearly Biomass allocated (delWs) = %g tDM/area\n", s->value[DEL_WS]);
			//Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[WS]);
		}
		else
		{
			//timber routine
			if (management == 0)
			{
				pfsPower = log (s->value[PFS20] / s->value[PFS2]) / log (10.0);
				//Log("PFS20 = %g\n", s->value[PFS20]);
				//Log("PFS2 = %g\n", s->value[PFS2]);
				//Log("M0 = %d\n", site->m0);
				//Log("FR = %g\n", site->fr);
				//Log("pfsPower = %g\n", pfsPower );

				pfsConst = s->value[PFS2] / pow ( 2, pfsPower);
				//Log("pfsConst = %g\n", pfsConst );


				pFS = pfsConst * pow( s->value[AVDBH], pfsPower);     //Foliage:Stem Partitioning Ratio uguale per tutte le celle con ugual specie fisso per ogni anno
				//Log("pFS = %g\n", pFS);

				//IN MONTHLY RUOUTINE USE THE MONTHLY PHYSMOD

				// % di NPP to roots
				pR = s->value[PRX] * s->value[PRN] / (s->value[PRN] + (s->value[PRX] - s->value[PRN]) * s->value[PHYS_MOD] * emme);
				//Log("pR = %g\n", pR);
				//Log("PHYSMOD = %g\n", s->value[PHYS_MOD]);

				//Log("PRX = %g\n", s->value[PRX]);
				//Log("PRN = %g\n", s->value[PRN]);
				//Log("3PG ratio to roots layer %d = %g %%\n", z, pR * 100);
			}
			//end of timber routine
			//coppice routine
			else
			{
				//per ora la routine per i cedui non è dinamica, non considera infatti che
				//all'aumentare degli anni dal taglio i valori di PF2, PF20, PRX e PRN si
				//devono avvicinare a quelli della routine della fustaia
				//manca una variabile che tenga conto degli anni dal taglio tra i dati di
				//inizializzazione, forse si puo ovviare considerando gli anni dal taglio
				//considerandola come se fosse l'età

				pfsPower = log (s->value[PFS20_C] / s->value[PFS2_C]) / log (10.0);
				//Log("PFS20_C = %g\n", s->value[PFS20_C]);
				//Log("PFS2_C = %g\n", s->value[PFS2_C]);
				//Log("M0 = %d\n", site->m0);
				//Log("FR = %g\n", site->fr);

				//Log("pfsPower = %g\n", pfsPower );

				pfsConst = s->value[PFS2_C] / pow ( 2, pfsPower);
				//Log("pfsConst = %g\n", pfsConst );


				pFS = pfsConst * pow( s->value[AVDBH], pfsPower);     //Foliage:Stem Partitioning Ratio uguale per tutte le celle fisso per ogni anno
				//Log("pFS = %g\n", pFS);

				// % di NPP to roots
				pR = s->value[PRX_C] * s->value[PRN_C] / (s->value[PRN_C] + ((s->value[PRX_C] - s->value[PRN_C]) * s->value[PHYS_MOD] * emme));
				//Log("pR = %g\n", pR);
				//Log("PRX_C = %g\n", s->value[PRX_C]);
				//Log("PRN_C = %g\n", s->value[PRN_C]);
				//Log("3PG ratio to roots for coppice = %g %%\n", pR * 100);
			}

			//end of coppice routine


			//if (fabs(APAR) < 0.000001) APAR = 0.000001  da 3PG ma che è????????????



			// % di NPP to stem
			pS = (1 - pR) / (1 + pFS);
			//Log("pS = %g\n", pS);
			//Log("3PG ratio to stem = %g %%\n", pS * 100);

			// % di NPP to foliage
			pF = 1 - pR - pS;
			//Log("pF = %g\n", m->lpCell[index].pF);
			//Log("3PG ratio to foliage = %g %%\n", pF * 100);

			pS += (pF / 2.0);
			pR += (pF / 2.0);
			pF = 0;

			RatioSum = pF + pS + pR;
			//Log("pF + pS + pR = %d\n", (int)RatioSum);
			if (RatioSum != 1)
			{
				//Log("Error in Biomass Partitioning!!\n");
			}

			//Log("End of month NPP for this layer  = %g tDM/area\n",  Stand_NPP);

			//Foliage compart
			//Log("Initial Foliage Biomass (Wf) = %g tDM/area\n", s->value[INITIAL_WF]);

			// Fraction of NPP to Foliage
			s->value[DEL_WF] = Stand_NPP * pF;
			//Log("Increment Yearly Biomass allocated (delWf) = %g tDM/area\n", s->value[DEL_WF]);


			// Total Foliage Biomass


			s->value[WF] = s->value[DEL_WF] + s->value[WF];


            if (!years)
            {
            if ( s->phenology == 0)
            {
                s->value[WF] = s->value[DEL_WF];
            }
            else
            {
                s->value[WF] = s->value[DEL_WF] + s->value[WF];
            }
            }
            else
            {
            if ( s->phenology == 0)
            {
                s->value[WF] = s->value[DEL_WF];
            }
            else
            {
                s->value[WF] = s->value[DEL_WF] + s->value[WF];
            }

            }




			//Log("Foliage Biomass (Wf) = %g tDM/area\n", s->value[WF]);



			//Roots compart
			//Log("Initial Root Biomass (Wr) = %g tDM/area\n", s->value[INITIAL_WR]);

			// Fraction of NPP to Roots
			s->value[DEL_WR] = Stand_NPP * pR;
			//Log("Increment Yearly Biomass allocated (delWr) = %g tDM/area\n", s->value[DEL_WR]);

			// Total Roots Biomass

			s->value[WRT] = s->value[WRT] + s->value[DEL_WR];
			//Log("Root Biomass (Wr) = %g tDM/area\n", s->value[WR]);



			//Stem compart
			//Log("Initial Stem Biomass (Ws) = %g tDM/area\n", s->value[INITIAL_WS]);

			// Fraction of NPP to Stem
			s->value[DEL_WS] = Stand_NPP * pS;
			//Log("Increment Yearly Biomass allocated (delWs) = %g tDM/area\n", s->value[DEL_STEMS_CTEM]);

			//Total Stem Biomass
			//remove the part allocated to the branch and bark
			s->value[DEL_BB] = s->value[DEL_WS] * s->value[FRACBB];
			//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
			//Log("Branch and bark Biomass (del_BB)= %g tDM/area\n", s->value[DEL_BB]);

			//allocation to stem
			s->value[DEL_WS] -= s->value[DEL_BB];
			s->value[WS] = s->value[WS] + s->value[DEL_WS];
			//Log("Increment Yearly Biomass allocated (delWs) = %g tDM/area\n", s->value[DEL_WS]);
			//Log("Stem Biomass (Ws) = %g tDM/area\n", s->value[WS]);

		}
	}


	// Total Biomass Increment
	s->value[DEL_TOTAL_W] = s->value[DEL_WF] + s->value[DEL_WR] + s->value[DEL_WS];
	//Log("Increment Yearly Total Biomass  (delTotalW) = %g tDM/area\n", s->value[DEL_TOTAL_W]);


	// Total Biomass
	s->value[TOTAL_W] = s->value[WF] + s->value[WS] + s->value[WRT];
	//Log("Previous Total W = %g tDM/area\n", oldW);
	//Log("Total Biomass = %g tDM/area\n", s->value[TOTAL_W]);



	//cumulated yearly biomass increment
	//s->value[DEL_Y_WS] += s->value[DEL_WS];
	//Log("Increment Yearly STEM Biomass = %g tDM/area\n", s->value[DEL_Y_WS]);
	//s->value[DEL_Y_WR] += s->value[DEL_WR];
	//Log("Increment Yearly ROOT Biomass = %g tDM/area\n", s->value[DEL_Y_WR]);
	//s->value[DEL_Y_WF] += s->value[DEL_WF];
	//Log("Increment Yearly FOLIAGE Biomass = %g tDM/area\n", s->value[DEL_Y_WF]);






	//control
	if (oldW > s->value[TOTAL_W])
	{
		//Log("ERROR in Partitioning-Allocation!!!\n");

		//Log("Previous Total W = %g tDM/area\n", oldW);

	}




	//Log("******************************\n");


}
*/
