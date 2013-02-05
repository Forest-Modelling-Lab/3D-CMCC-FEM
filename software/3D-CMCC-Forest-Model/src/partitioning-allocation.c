/*partitionign_allocation.c*/

/* includes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "constants.h"

/**/

void Get_Fruit_Allocation_LPJ (SPECIES *const s, int z, int years, float Yearly_Rain, float canopy_cover_dominant )
{
	//static float WseedTree;    //fruit biomass per Tree
	static int NseedTree;      //Number of fruits per Tree

	//static float FProCov;   //LPJ Foliage Projective  Cover for Seed Establishment
	//static float LightAbsorb;
	//static float EstabRate;   //Seed establishment rate Under Dominant Canopy
	//static int Nsapling;      //Number of saplings Under and Outside Dominant Canopy


	Log("**SEEDS-ALLOCATION**\n");

	Log("\n--LPJ FRUIT ALLOCATION--\n");





	//il 10% lo do ai frutti
	//fraction of total NPP to Fruit compart
	//biomass to seeds
	s->value[W_SEED] = s->value[YEARLY_NPP] * s->value[FRACFRUIT];
	Log("Costant Fraction Rate of Annual NPP for Fruit Production using LPJ = %g %%\n", s->value[FRACFRUIT] * 100);
	Log("Annual NPP to Seeds Biomass Compart = %g tDM/ha/year\n", s->value[W_SEED]);


	//WseedTree = s->value[W_SEED] / s->counter[N_TREE];



	//Log("Annual Biomass for Seeds Compart for Tree  = %g in tDM/tree/year\n", WseedTree);
	//Log("Annual Biomass for Seeds Compart for Tree  = %g in Kg/tree/year\n", WseedTree * 1000);
	//Number of seeds from tDM to grammes
	s->counter[N_SEED] = (s->value[W_SEED] * 1000000)/ s->value[WEIGHTSEED];
	//Log("Annual Number of seeds using LPJ  = %d seeds/ha/year\n", s->counter[N_SEED]);
	/*
	   Log("NSEED %g\n", s->counter[N_SEED]);
	   Log("NTREE %g\n", s->counter[N_TREE]);
	   NseedTree = s->counter[N_SEED] / s->counter[N_TREE];
	 */
	Log("Annual Number of Seeds Produced for Tree using LPJ = %d seeds/ha/year\n", s->counter[N_SEED]);
	Log("Annual Number of Seeds Produced for Tree using LPJ = %d seeds/tree/year\n", NseedTree);

	/*
	   Log("Intrinsic Germinability Rate = %g %% \n", s->value[GERMCAPACITY] * 100);


	   if (Yearly_Rain > s->value[MINRAIN])
	   {

	   Log("Annual Number of seeds using LPJ  = %d seeds/ha/year\n", s->counter[N_SEED]);

	//da ricontrollare per vedere quale Lai prende di quale layer e semmai non utilizzare il FProCov





	LightAbsorb = 1 -  (exp(- s->value[K] * s->value[LAI]));

	Log("Light Absorb in Dominant Canopy = %g \n", LightAbsorb);
	FProCov = canopy_cover_dominant * LightAbsorb;
	Log("Canopy Cover in  Dominant layer with DBHDC function = %g \n", canopy_cover_dominant);

	Log("LPJ Fractional Projective Cover FPC = %g \n", FProCov);


	EstabRate = s->value[GERMCAPACITY] * (1 - exp((-5) * (1 - FProCov))) * (1 - FProCov);
	Log("Seed Establishment Rate from LPJ = %g saplings/m^2 \n", EstabRate);

	Nsapling = s->counter[N_SEED] * EstabRate ;
	Log("Annual Number of Saplings per hectare using LPJ = %d Saplings/year hectare\n", Nsapling);
	Log("Annual Number of Saplings using LPJ = %g Saplings/year m^2\n", (float) Nsapling / SIZECELL );
	Log("Percentage of seeds survived using LPJ = %g %% seeds/year hectare\n", ((float)Nsapling * 100)/(float)s->counter[N_SEED] );
	//Log("------THE MODEL STILL DOESN'T TAKE INTO ACCOUNT THE PRESENTS OF SUBDOMINANTS SPECIES!!!!------\n");
	}
	else
	{
	Log("Not enough Rain for Seeds Establishment!!!!\n");
	}

	//s->value[YEARLY_NPP] = s->value[YEARLY_NPP] - s->value[W_SEED];
	//Log("Annual NPP  less Seeds Biomass using LPJ = %g tDM/ha/year\n", s->value[YEARLY_NPP]);
	 */

}


/**/
int Get_Fruit_Allocation_Logistic_Equation (SPECIES *const s, AGE *const a)
{
	/*USING A LOGISTIC EQUATION*/
	static int NumberSeed;                  //Number of Seeds per tree
	static int PopNumberSeeds;              //Number of Seeds per Population
	static int MaxSeed = 2000;              //Maximum seeds number
	static int OptSexAge = 100;             //Age at maximum seeds production
	static int MinSexAge = 20;              //Minimum age for sex maturity
	static float WseedLE ;                  //Weight of seeds of population from Logistic Equation

	Log("------LOGISTIC EQUATION FRUIT ALLOCATION------\n");



	NumberSeed = (MaxSeed/ (1 + OptSexAge * exp (-0.1 * (a->value - MinSexAge))));
	Log("Annual Number of Seeds for Tree from Logistic Equation = %d seeds/tree/year\n", NumberSeed);

	PopNumberSeeds = NumberSeed * s->counter[N_TREE];
	Log("Annual Number of Seeds for Population from Logistic Equation = %d seeds/ha/year\n", PopNumberSeeds);

	WseedLE = ((float)PopNumberSeeds * s->value[WEIGHTSEED]) / 1000000 /* to convert in tonnes*/;
	Log("Biomass for Seed from Logistic Equation = %g tDM/ha\n", WseedLE);
	Log("Fraction of Biomass allocated for Seed from Logistic Equation = %g tDM/ha\n", WseedLE);
	Log("Fraction of NPP allocated for Seed from Logistic Equation = %.4g %%\n", (WseedLE * 100) /s->value[YEARLY_NPP] );

	return NumberSeed;
}
/**/

int Get_Fruit_Allocation_TREEMIG (SPECIES *const s, AGE *const a)
{
	static int NumberSeed;
	static float heigthdependence;
	static float WseedT;            //heigth dependence factor

	Log("------TREEMIG FRUIT ALLOCATION------\n");

	heigthdependence = s->value[LAI] / s->value[LAIGCX] ;//sarebbe Lai  / Lai max;
	Log("heigthdependence = %g \n", heigthdependence);

	//numero semi prodotti
	NumberSeed = (float)s->counter[N_TREE] * s->value[MAXSEED] * heigthdependence * 0.51 *
			( 1 + sin((2 * Pi * (float)a->value ) / s->value[MASTSEED]));
	Log("Nseed per cell at the End of the This Year = %d seeds per cell\n", NumberSeed);

	//Biomassa allocata nei semi in tDM/ha
	WseedT = (float)NumberSeed * s->value[WEIGHTSEED] / 1000000;  //per convertire in tonnellate biomassa allocata per i semi
	Log("Seeds Biomass per cell at the End of the This Year = %g tonnes of seeds/ha \n", WseedT);
	Log("Fraction of NPP allocated for Seed from TREEMIG = %.4g %%\n", (WseedT * 100) /s->value[YEARLY_NPP] );

	return NumberSeed;

}

void Get_Partitioning_Allocation_3PG (SPECIES *const s, int z, int years, int management)
{
	/*3PG VERSION*/
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

	Log("\n-- (3PG) BIOMASS PARTITIONING-ALLOCATION FOR LAYER %d --\n", z, years);

	if (management == 0)
	{
		Log("Management type = TIMBER\n");
	}
	else
	{
		Log("Management type = COPPICE\n");
	}


	oldW = s->value[WF] + s->value[WS] + s->value[WRT];

	/*annual average of physiological modifier for partitioning*/
	s->value[AVERAGE_PHYS_MOD] = s->value[YEARLY_PHYS_MOD] / s->counter[VEG_MONTHS];
	Log("Average Physmod = %g \n", s->value[AVERAGE_PHYS_MOD]);

	emme = site->m0 + (1 - site->m0) * site->fr;
	//Log("emme = %g\n", emme );

	/* Calculate Partitioning Coefficients */
	// il driver del partitioning sono: fSW-fVPD-fAGE
	// 'emme' è omogeneo per tutte le celle

	/*timber routine*/
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


		pFS = pfsConst * pow( s->value[AVDBH], pfsPower);     //Foliage:Stem Partitioning Ratio uguale per tutte le celle fisso per ogni anno
		Log("pFS = %g\n", pFS);

		// % di NPP to roots
		pR = s->value[PRX] * s->value[PRN] / (s->value[PRN] + (s->value[PRX] - s->value[PRN]) * s->value[AVERAGE_PHYS_MOD] * emme);
		Log("pR = %g\n", pR);
		Log("EOY average PHYS_MOD = %g \n",  s->value[AVERAGE_PHYS_MOD]);
		//Log("PRX = %g\n", s->value[PRX]);
		//Log("PRN = %g\n", s->value[PRN]);
		Log("3PG ratio to roots for timber= %g %%\n", pR * 100);
	}
	//end of timber routine
	/*coppice routine*/
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
		Log("pFS = %g\n", pFS);

		// % di NPP to roots
		pR = s->value[PRX_C] * s->value[PRN_C] / (s->value[PRN_C] + (s->value[PRX_C] - s->value[PRN_C]) * s->value[AVERAGE_PHYS_MOD] * emme);
		//Log("pR = %g\n", pR);
		//Log("PRX_C = %g\n", s->value[PRX_C]);
		//Log("PRN_C = %g\n", s->value[PRN_C]);
		Log("3PG ratio to roots for coppice = %g %%\n", pR * 100);
	}

	//end of coppice routine


	//if (fabs(APAR) < 0.000001) APAR = 0.000001  da 3PG ma che è????????????



	// % di NPP to stem
	pS = (1 - pR) / (1 + pFS);
	//Log("pS = %g\n", pS);
	Log("3PG ratio to stem = %g %%\n", pS * 100);

	// % di NPP to foliage
	pF = 1 - pR - pS;
	//Log("pF = %g\n", m->lpCell[index].pF);
	Log("3PG ratio to foliage = %g %%\n", pF * 100);


	RatioSum = pF + pS + pR;
	//Log("pF + pS + pR = %d\n", (int)RatioSum);
	if (RatioSum != 1)
	{
		Log("Error in Biomass Partitioning!!\n");
	}

	Log("End of Year Cumulated NPP for this layer  = %g tDM/ha\n",  s->value[YEARLY_NPP]);

	//Foliage compart
	//Log("Initial Foliage Biomass (Wf) = %g tDM/ha\n", s->value[INITIAL_WF]);

	// Fraction of NPP to Foliage
	s->value[DEL_WF] = s->value[YEARLY_NPP] * pF;
	Log("Increment Yearly Biomass allocated (delWf) = %g tDM/ha\n", s->value[DEL_WF]);


	// Total Foliage Biomass
	if (!years)
	{
		if ( s->value[PHENOLOGY] == 0)
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
		if ( s->value[PHENOLOGY] == 0)
		{
			s->value[WF] = s->value[DEL_WF];
		}
		else
		{
			s->value[WF] = s->value[DEL_WF] + s->value[WF];
		}

	}



	Log("Foliage Biomass (Wf) = %g tDM/ha\n", s->value[WF]);



	//Roots compart
	//Log("Initial Root Biomass (Wr) = %g tDM/ha\n", s->value[INITIAL_WR]);

	// Fraction of NPP to Roots
	s->value[DEL_WR] = s->value[YEARLY_NPP] * pR;
	Log("Increment Yearly Biomass allocated (delWr) = %g tDM/ha\n", s->value[DEL_WR]);

	// Total Roots Biomass

	s->value[WRT] = s->value[WRT] + s->value[DEL_WR];

	Log("Root Biomass (Wr) = %g tDM/ha\n", s->value[WRT]);



	//Stem compart
	//Log("Initial Stem Biomass (Ws) = %g tDM/ha\n", s->value[INITIAL_WS]);

	// Fraction of NPP to Stem
	s->value[DEL_WS] = s->value[YEARLY_NPP] * pS;
	Log("Increment Yearly Biomass allocated (delWs) = %g tDM/ha\n", s->value[DEL_WS]);

	//Total Stem Biomass
	//remove the part allocated to the branch and bark
	s->value[DEL_BB] = s->value[DEL_WS] * s->value[FRACBB];
	//Log("Branch and bark fraction = %g %%\n", s->value[FRACBB] * 100);
	//Log("Branch and bark Biomass (del_BB)= %g tDM/ha\n", s->value[DEL_BB]);

	//allocation to stem
	s->value[DEL_WS] -= s->value[DEL_BB];
	s->value[WS] = s->value[WS] + s->value[DEL_WS];
	Log("Increment Yearly Biomass allocated (delWs) = %g tDM/ha\n", s->value[DEL_WS]);
	Log("Stem Biomass without branch and bark (Ws) = %g tDM/ha\n", s->value[WS]);


	// Total Biomass Increment
	s->value[DEL_TOTAL_W] = s->value[DEL_WF] + s->value[DEL_WR] + s->value[DEL_WS];
	Log("Increment Yearly Total Biomass  (delTotalW) = %g tDM/ha\n", s->value[DEL_TOTAL_W]);


	// Total Biomass
	s->value[TOTAL_W] = s->value[WF] +s->value[WS] + s->value[WRT];
	Log("Previous Total W = %g tDM/ha\n", oldW);
	Log("Total Biomass = %g tDM/ha\n", s->value[TOTAL_W]);







	/*control*/
	if (oldW > s->value[TOTAL_W])
	{
		Log("ERROR in Partitioning-Allocation!!!\n");

		Log("Previous Total W = %g tDM/ha\n", oldW);

	}




	Log("******************************\n");


}
/**/

void Get_Partitioning_Allocation_NASACASA (SPECIES *const s, int z)
{
	/*NASA-CASA VERSION*/

	/*NASACASA*/
	float const r0nasacasa = 0.3;
	float const s0nasacasa = 0.3;
	float pS_NASACASA;
	float pR_NASACASA;
	float pF_NASACASA;

	Log("\n-- (NASA-CASA) BIOMASS PARTITIONING-ALLOCATION FOR LAYER %d --\n", z);


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

	//Log("Yearly Cumulated NPP for this layer  = %g tDM/ha\n",  s->value[YEARLY_NPP]);

	s->value[BIOMASS_ROOTS_NASA_CASA] = s->value[YEARLY_NPP] * pR_NASACASA;
	//Log("BiomassRoots NASA-CASA = %g tDM/ha\n", s->value[BIOMASS_ROOTS_NASA_CASA]);

	s->value[BIOMASS_STEM_NASA_CASA] = s->value[YEARLY_NPP] * pS_NASACASA;
	//Log("BiomassStem NASA-CASA = %g tDM/ha\n", s->value[BIOMASS_STEM_NASA_CASA]);

	s->value[BIOMASS_FOLIAGE_NASA_CASA] = s->value[YEARLY_NPP] * pF_NASACASA;
	//Log("BiomassFoliage NASA-CASA = %g tDM/ha\n", s->value[BIOMASS_FOLIAGE_NASA_CASA]);

	Log("******************************\n");
}
/**/

void Get_Partitioning_Allocation_CTEM (SPECIES *const s, int z)
{
	/*CTEM VERSION*/

	float const omegaCtem = 0.8;
	//ratio of partitioning in optimum condition
	float const s0Ctem = 0.1;
	float const r0Ctem = 0.55;
	float pS_CTEM;
	float pR_CTEM;
	float pF_CTEM;

	//(Arora V. K., Boer G. J., GCB, 2005)

	Log("\n-- (CTEM) BIOMASS PARTITIONING-ALLOCATION FOR LAYER %d --\n", z);


	//Partitioning Ratio

	//Log("Light Trasmitted for this layer = %g\n", s->value[LIGHT_TRASM]);

	pR_CTEM = (r0Ctem + (omegaCtem * ( 1 - s->value[F_SW] ))) / (1 + (omegaCtem * ( 2 - s->value[LIGHT_TRASM] - s->value[F_SW] )));
	Log("Roots CTEM ratio = %g %%\n", pR_CTEM * 100);


	pS_CTEM = (s0Ctem + (omegaCtem * ( 1 - s->value[LIGHT_TRASM]))) / (1 + ( omegaCtem * ( 2 - s->value[LIGHT_TRASM] - s->value[F_SW] )));
	Log("Stem CTEM ratio = %g %%\n", pS_CTEM * 100);


	pF_CTEM = (1 - pS_CTEM - pR_CTEM);
	Log("Foliage CTEM ratio = %g %%\n", pF_CTEM * 100);


	// Biomass allocation

	s->value[BIOMASS_ROOTS_TOT_CTEM] = s->value[YEARLY_NPP] * pR_CTEM;
	//Log("BiomassRoots CTEM = %g tDM/ha\n", s->value[BIOMASS_ROOTS_TOT_CTEM]);


	s->value[BIOMASS_STEM_CTEM] = s->value[YEARLY_NPP] *  pS_CTEM;
	//Log("BiomassStem CTEM = %g tDM/ha\n", s->value[BIOMASS_STEM_CTEM]);


	s->value[BIOMASS_FOLIAGE_CTEM] = s->value[YEARLY_NPP] * pF_CTEM;
	//Log("BiomassFoliage CTEM = %g tDM/ha\n", s->valueBIOMASS_FOLIAGE_CTEM] );

	Log("******************************\n");
}



