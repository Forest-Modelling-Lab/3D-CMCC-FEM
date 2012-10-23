
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
