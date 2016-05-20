#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "soil.h"
#include "types.h"
#include "constants.h"

extern soil_t *g_soil;

void soil_initialization(CELL *const c)
{
	double litterSOC, CRB, Thc;
	int l;
	double srh = .16;
	//c->soils_count = 1;
	for (l =0; l < c->soils_count; l++)
	{
		//debug: assumed the whole soil as a single layer of 20cm
		//		c->soils[l].dphum = g_soil->values[SOIL_IN_SOC] * g_soil->values[SOIL_HUMU_FRACT] * c->bulk_density * g_soil->values[SOIL_DEPTH];
		//		c->soils[l].initialOrganicC = g_soil->values[SOIL_IN_SOC] * g_soil->values[SOIL_HUMA_FRACT] * c->bulk_density * g_soil->values[SOIL_DEPTH];
		//		litterSOC = g_soil->values[SOIL_IN_SOC] * g_soil->values[SOIL_LIT_FRACT] * c->bulk_density  * g_soil->values[SOIL_DEPTH];
		//		//c->soils[l].inert_C = g_soil->values[SOIL_IN_SOC] * /*g_soil->values[SOIL_HUMU_FRACT]*/ .001 * c->bulk_density * g_soil->values[SOIL_DEPTH];


		//taken from Chiti 2010,
		//ratios taken from DNDC default (userGuide)
		//			Soil_porosity            0.451000
		//actually, it should be g_soil->values[SOIL_IN_SOC] * g_soil->values[SOIL_HUMU_FRACT] * g_soil->values[SOIL_sd * 1000 * 10000 * Maximum(0, Minimum(0.01 / g_soil->values[SOIL_porosity, g_soil->values[SOIL_DEPTH]));
		//		c->soils[l].dphum = g_soil->values[SOIL_IN_SOC] * g_soil->values[SOIL_HUMU_FRACT] * c->bulk_density * 1000 * 10000 * g_soil->values[SOIL_DEPTH];
		//		c->soils[l].initialOrganicC = g_soil->values[SOIL_IN_SOC] * g_soil->values[SOIL_HUMA_FRACT] * c->bulk_density * 1000 * 10000 * g_soil->values[SOIL_DEPTH];
		//		litterSOC = g_soil->values[SOIL_IN_SOC] * g_soil->values[SOIL_LIT_FRACT] * c->bulk_density * 1000 * 10000 * g_soil->values[SOIL_DEPTH];
		//		c->soils[l].inert_C = g_soil->values[SOIL_IN_SOC] * g_soil->values[SOIL_HUMA_FRACT] * c->bulk_density * 1000 * 10000 * g_soil->values[SOIL_DEPTH];

		//gC/m-2 profile-1
		//		c->soils[l].dphum = g_soil->values[SOIL_IN_SOC] * g_soil->values[SOIL_HUMU_FRACT] * 1000; // * g_soil->values[SOIL_DEPTH];
		//		c->soils[l].initialOrganicC = g_soil->values[SOIL_IN_SOC] * g_soil->values[SOIL_HUMA_FRACT] * 1000; // * g_soil->values[SOIL_DEPTH];
		//		litterSOC = g_soil->values[SOIL_IN_SOC] * g_soil->values[SOIL_LIT_FRACT]; // kg * g_soil->values[SOIL_DEPTH];
		//		c->soils[l].initialOrganicC = g_soil->values[SOIL_IN_SOC] - litterSOC; //kg
		//		c->soils[l].inert_C = g_soil->values[SOIL_IN_SOC] * g_soil->values[SOIL_HUMA_FRACT] * 1000; // * g_soil->values[SOIL_DEPTH];
		g_soil->values[SOIL_DEPTH] = .2;
		CRB = g_soil->values[SOIL_BIO_FRACT] * g_soil->values[SOIL_IN_SOC];	//kg
		c->soils[l].CRB1 = CRB * SRB;
		c->soils[l].CRB2 = CRB * (1.0 - SRB);
		litterSOC = (g_soil->values[SOIL_IN_SOC] - CRB)* g_soil->values[SOIL_LIT_FRACT];
		Thc = g_soil->values[SOIL_IN_SOC] - CRB - litterSOC;
		c->soils[l].crhl = Thc * g_soil->values[SOIL_HUMA_FRACT] * srh;
		c->soils[l].crhr = Thc * g_soil->values[SOIL_HUMA_FRACT] * (1 - srh);
		c->soils[l].dphum = Thc * g_soil->values[SOIL_HUMU_FRACT];
		c->soils[l].soilDepth = g_soil->values[SOIL_DEPTH];


		c->soils[l].soc = c->soils[l].crhl+ c->soils[l].crhr+c->soils[l].dphum+
				c->soils[l].CRB1 + c->soils[l].CRB2 + litterSOC; // ???
		c->soils[l].inert_C = .049 * pow(c->soils[l].soc, 1.139);

		c->soils[l].rcvl = litterSOC *.10;
		c->soils[l].rcr= litterSOC *.70;
		c->soils[l].rcl= litterSOC *.20;
		//		c->soils[l].crhl = c->soils[l].initialOrganicC;	//interpreted
		//		c->soils[l].crhr = c->soils[l].dphum;	//interpreted
		c->soils[l].clay_nh4 = g_soil->values[SOIL_NH4] * 0.3 * c->bulk_density ;
		c->soils[l].nh4 = g_soil->values[SOIL_NH4] * 0.7 * 10* c->bulk_density;
		c->soils[l].no3 = g_soil->values[SOIL_NO3] *10* c->bulk_density; //bulk_dens as mass
		g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY] = 0.063; // tab 5. Katie Price 2010
		//
		//		///////////////////////////
		//					double CRB, Thc;
		//					CRB = RBO * ocini[i];
		//					CRB1[i] = CRB * SRB;
		//					CRB2[i] = CRB * (1.0 - SRB);
		//					Thc = ocini[i] - CRB;
		//					crhl[i] = Thc * srh;
		//					crhr[i] = Thc - crhl[i];
		//					///////////////////////////////////
		////years != 0
		//		float rbo = 0.015 * 0.5 + 0.00001;
		//		float CRB = ocini[i] * rbo + 0.00001;
		//		CRB1[i] = CRB * SRB;
		//		CRB2[i] = CRB * (1.0 - SRB);
		//		float AHM = ocini[i] - CRB;
		//		if(AHM<=0.0) AHM = 0.0000001;
		//		crhl[i] = AHM * srh;
		//		crhr[i] = AHM * (1.0 - srh);
		//
		//		wcrb += (CRB1[i]+CRB2[i]);
		//		day_clay_N += clay_nh4[i];

		//	    Tranr[1] = Soil_OCatSurface * Soil_Litter * m;
		//	    OCini[1] = Soil_OCatSurface * Soil_humads * m;
		//	    Dphum[1] = Soil_OCatSurface * Soil_humus * m;
		//		CharC[1] = Soil_OCatSurface * PassiveC * m;
		//
		//	    s_NO3[1] = Soil_NO3 * m / 1000000.;
		//	    s_IniNH4[1] = Soil_NH4 * m / 1000000.;
		//
		//	    RCVL[1] = 0.0 * Tranr[1];
		//	    RCL[1]  = 0.0 * Tranr[1];
		//	    RCR[1]  = 1.0 * Tranr[1];
		//		Dbm = Soil_Density;
		//
		//	    s_NH4[1]   = s_IniNH4[1] * 0.7;
		//	    ClayNH4[1] = s_IniNH4[1] * 0.3;
	}
	//	c->till_fact;
	//	c->tilq;
	//	Soil_NO3(-)(mgN/kg)       0.50000
	//	Soil_NH4(+)(mgN/kg)       0.05000
}

void tree_leaves_fall(MATRIX *const m, int const cell)	//, int years, int month, int day, const MET_DATA *const met
{
	double AddN, AddC, AddCN, dInertC;
	float  RR1, RR2, RR3;
	float pa, pb, pc, pd, pe, pf;
	int l;
	double abovegroundCN = 50;
	double belovegroundCN = 50.0; //debugging
	// ALESSIOR this vars are used uninitialized
	float AddC1 = 0.f, AddC2 = 0.f, AddC3 = 0.f;
	float OutN;

	//if(m->cells[cell].fineRootLittering >.0005)m->cells[cell].fineRootLittering = 0.0005;
	if(m->cells[cell].leafLittering+m->cells[cell].stemLittering + m->cells[cell].fineRootLittering +
			m->cells[cell].coarseRootLittering + m->cells[cell].stemBrancLittering>0.0)
	{
		AddN = m->cells[cell].leaflitN +  m->cells[cell].stemlitN + m->cells[cell].stemBranclitN +  m->cells[cell].coarseRootlitN +
				m->cells[cell].fineRootlitN;

		//plantn[cwc] -= AddN;
		//if(plantn[cwc]<0.0) plantn[cwc] = 0.0;

		// Add dead shoot in stable
		AddC = m->cells[cell].leafLittering+m->cells[cell].stemLittering + m->cells[cell].stemBrancLittering;
		//bugfix CN too high for biome; neer gets AddC1
		AddCN = AddC / ( m->cells[cell].leaflitN + m->cells[cell].stemlitN + m->cells[cell].stemBranclitN); //abovegroundCN;
		//AddP = CropP[cwc] * 0.01 * AddC /0.4;


		//alloc_litter_into_pools(&AddCN, &AddC, 1, &AddC1, &AddC2, &AddC3, &dInertC, 0, rcnrvl, rcnrl, rcnrr);
		{
			dInertC = 0.0;

			if ( AddCN >= g_soil->values[SOIL_RCNRVL] &&  AddCN < g_soil->values[SOIL_RCNRL] )
			{
				RR1 = g_soil->values[SOIL_RCNRVL];
				RR2 = g_soil->values[SOIL_RCNRL];

				pc = (1.0 / AddCN);
				pd = (1.0 / RR1);
				pa = pc - pd;
				pe = (1.0 / RR2);
				pf = (1.0 / RR1);
				pb = pe - pf;

				AddC2 = AddC * (pa / pb);
				AddC1 = (AddC - AddC2);
				AddC3 = 0.0;
			}
			else if ( AddCN >= g_soil->values[SOIL_RCNRL] && AddCN <= g_soil->values[SOIL_RCNRR] )
			{
				RR2 = g_soil->values[SOIL_RCNRL];
				RR3 = g_soil->values[SOIL_RCNRR];
				pc = (1.0 / AddCN);
				pd = (1.0 / RR2);
				pa = pc - pd;
				pe = (1.0 / RR3);
				pf = (1.0 / RR2);
				pb = pe - pf;
				AddC3 = (float)(AddC * (pa / pb));
				AddC2 = (float)(AddC - AddC3);
				AddC1 = (float)0.0;
			}
			else if ( AddCN < g_soil->values[SOIL_RCNRVL] )
			{
				float ActN = AddC/g_soil->values[SOIL_RCNRVL];
				AddC3 = 0.0;
				AddC2 = 0.0;
				AddC1 = AddC;
				m->cells[cell].soils[0].nh4 += (AddC / (AddCN+0.0000001) - ActN);
				if( m->cells[cell].soils[0].nh4<0) m->cells[cell].soils[0].nh4	= .0000001;
				AddCN = g_soil->values[SOIL_RCNRVL];
			}
			else if ( AddCN > g_soil->values[SOIL_RCNRR] )
			{
				float ActC = AddC/ AddCN * g_soil->values[SOIL_RCNRR];
				AddC3 = ActC;
				AddC2 = 0.0;
				AddC1 = 0.0;

				//dInertC = AddC - ActC;
				//inert_C[1] += dInertC;
				//m->cells[cell].soils[0].inert_C += dInertC;
				AddC = ActC;
				AddCN = g_soil->values[SOIL_RCNRR];
			}

			if (AddC1 < 0.0) AddC1 = 0.0;
			if (AddC2 < 0.0) AddC2 = 0.0;
			if (AddC3 < 0.0) AddC3 = 0.0;



			/*	if(IfStub==0)
			{
				AddToSoilC = 0.05 * (*AddC1 + *AddC2 + *AddC3 + dInertC);
				AddToSoilN = 0.05* (*AddC1/rcnrvl + *AddC2/rcnrl + *AddC3/rcnrr);
			 *AddC1 *= 0.95;
			 *AddC2 *= 0.95;
			 *AddC3 *= 0.95;
				dInertC *= 0.95;
			}
			else
			{
				AddToSoilC = dInertC;
				AddToSoilN = 0.0;
				dInertC = 0.0;
			}*/

			/*	if(IfStub==1)//litter turn into stub
			{
			}
			else//litter turn into soil C pools
			{
				float AddToSoilC, AddToSoilN;
				AddToSoilC = (*AddC1 + *AddC2 + *AddC3 + dInertC);
				AddToSoilN = (*AddC1/rcnrvl + *AddC2/rcnrl + *AddC3/rcnrr);

				day_addC += AddToSoilC;
				yr_addrc += AddToSoilC;
				day_addrn += AddToSoilN;
				yr_addrn += AddToSoilN;
			}
			 */
		}
		OutN = AddC1/g_soil->values[SOIL_RCNRVL] + AddC2/g_soil->values[SOIL_RCNRL] + AddC3/g_soil->values[SOIL_RCNRR];

		m->cells[cell].soils[0].rcvl += AddC1;
		m->cells[cell].soils[0].rcl += AddC2;
		m->cells[cell].soils[0].rcr += AddC3;
		m->cells[cell].AddC1 += AddC1;
		m->cells[cell].AddC2 += AddC2;
		m->cells[cell].AddC3 += AddC3;
		//OrgP[1] += AddP;

		//		day_addC += (AddC1 + AddC2 + AddC3);
		//		day_addrn += (AddC1 / g_soil->values[SOIL_RCNRVL] + AddC2 / g_soil->values[SOIL_RCNRL] + AddC3 / g_soil->values[SOIL_RCNRR]);
		//		yr_addtc += (AddC1 + AddC2 + AddC3);
		//		yr_addtn += (AddC1 / g_soil->values[SOIL_RCNRVL] + AddC2 / g_soil->values[SOIL_RCNRL] + AddC3 / g_soil->values[SOIL_RCNRR]);

		AddC1 = 0.0;
		AddC2 = 0.0;
		AddC3 = 0.0;
		//		AddP = 0.0;

		//root litter incorporation
		for (l = 0; l < m->cells[cell].soils_count; l++ )
		{
			if ( l >= 0 && l <= 2 )
			{
				AddC = m->cells[cell].fineRootLittering +
						m->cells[cell].coarseRootLittering * 0.5 / 3.0;
				//AddP = CropP[cwc] * 0.01 /0.4 * sRoot * 0.5 / 3.0;
			}
			if ( l >= 3 && l <= 5 )
			{
				AddC = m->cells[cell].fineRootLittering +
						m->cells[cell].coarseRootLittering * 0.4 / 3.0;
				//AddP = CropP[cwc] * 0.01 /0.4 * sRoot * 0.4 / 3.0;
			}
			if ( l >= 6 && l < m->cells[cell].soils_count )
			{
				AddC = m->cells[cell].fineRootLittering +
						m->cells[cell].coarseRootLittering * 0.1 / (float)(m->cells[cell].soils_count - 6);
				//AddP = CropP[cwc] * 0.01 /0.4 * sRoot * 0.1 / (float)(q - 6);
			}
			AddCN =AddC / ( m->cells[cell].coarseRootlitN + m->cells[cell].fineRootlitN);//belovegroundCN;
			//alloc_litter_into_pools(&AddCN, &AddC, 10, &AddC1, &AddC2, &AddC3, &dInertC, 0, rcnrvl, rcnrl, rcnrr);
			{
				dInertC = 0.0;

				if ( AddCN >= g_soil->values[SOIL_RCNRVL] &&  AddCN < g_soil->values[SOIL_RCNRL] )
				{
					RR1 = g_soil->values[SOIL_RCNRVL];
					RR2 = g_soil->values[SOIL_RCNRL];

					pc = (1.0 / AddCN);
					pd = (1.0 / RR1);
					pa = pc - pd;
					pe = (1.0 / RR2);
					pf = (1.0 / RR1);
					pb = pe - pf;

					AddC2 = AddC * (pa / pb);
					AddC1 = (AddC - AddC2);
					AddC3 = 0.0;
				}
				else if ( AddCN >= g_soil->values[SOIL_RCNRL] && AddCN <= g_soil->values[SOIL_RCNRR] )
				{
					RR2 = g_soil->values[SOIL_RCNRL];
					RR3 = g_soil->values[SOIL_RCNRR];
					pc = (1.0 / AddCN);
					pd = (1.0 / RR2);
					pa = pc - pd;
					pe = (1.0 / RR3);
					pf = (1.0 / RR2);
					pb = pe - pf;
					AddC3 = (float)(AddC * (pa / pb));
					AddC2 = (float)(AddC - AddC3);
					AddC1 = (float)0.0;
				}
				else if ( AddCN < g_soil->values[SOIL_RCNRVL] )
				{
					float ActN = AddC/g_soil->values[SOIL_RCNRVL];
					AddC3 = 0.0;
					AddC2 = 0.0;
					AddC1 = AddC;
					m->cells[cell].soils[0].nh4 += (AddC / (AddCN+0.0000001) - ActN);
					if( m->cells[cell].soils[0].nh4<0) m->cells[cell].soils[0].nh4	= .0000001;
					AddCN = g_soil->values[SOIL_RCNRVL];
				}
				else if ( AddCN > g_soil->values[SOIL_RCNRR] )
				{
					float ActC = AddC/ AddCN * g_soil->values[SOIL_RCNRR];
					AddC3 = ActC;
					AddC2 = 0.0;
					AddC1 = 0.0;

					dInertC = AddC - ActC;
					//inert_C[1] += dInertC;
					//m->cells[cell].soils[0].inert_C += dInertC;
					AddC = ActC;
					AddCN = g_soil->values[SOIL_RCNRR];
				}

				if (AddC1 < 0.0) AddC1 = 0.0;
				if (AddC2 < 0.0) AddC2 = 0.0;
				if (AddC3 < 0.0) AddC3 = 0.0;



				/*	if(IfStub==0)
				{
					AddToSoilC = 0.05 * (*AddC1 + *AddC2 + *AddC3 + dInertC);
					AddToSoilN = 0.05* (*AddC1/rcnrvl + *AddC2/rcnrl + *AddC3/rcnrr);
				 *AddC1 *= 0.95;
				 *AddC2 *= 0.95;
				 *AddC3 *= 0.95;
					dInertC *= 0.95;
				}
				else
				{
					AddToSoilC = dInertC;
					AddToSoilN = 0.0;
					dInertC = 0.0;
				}*/

				/*	if(IfStub==1)//litter turn into stub
				{
				}
				else//litter turn into soil C pools
				{
					float AddToSoilC, AddToSoilN;
					AddToSoilC = (*AddC1 + *AddC2 + *AddC3 + dInertC);
					AddToSoilN = (*AddC1/rcnrvl + *AddC2/rcnrl + *AddC3/rcnrr);

					day_addC += AddToSoilC;
					yr_addrc += AddToSoilC;
					day_addrn += AddToSoilN;
					yr_addrn += AddToSoilN;
				}
				 */
			}
			m->cells[cell].soils[0].rcvl += AddC1;
			m->cells[cell].soils[0].rcl += AddC2;
			m->cells[cell].soils[0].rcr += AddC3;
			m->cells[cell].AddC1 += AddC1;
			m->cells[cell].AddC2 += AddC2;
			m->cells[cell].AddC3 += AddC3;
			//OrgP[l] += AddP;
			AddC1 = 0.0;
			AddC2 = 0.0;
			AddC3 = 0.0;
			//AddP = 0.0;
		}

		//		float AddToSoilC, AddToSoilN;
		//		AddToSoilC = sRoot;
		//		AddToSoilN = sRoot/AddCN;
		//
		//		day_addC += AddToSoilC;
		//		yr_addrc += AddToSoilC;
		//		day_addrn += AddToSoilN;
		//		yr_addrn += AddToSoilN;
		//
		//		sRoot = 0.0;

	}
}

void soil_dndc_sgm(MATRIX *const m, const YOS *const yos, const int years, const int month, const int day, const int years_of_simulation)
{
	MET_DATA *met;
	double  clayc, krh, hrh, DDRF;
	double drcvl,DRCB,lit_co2, DRCB1, DRCB2, sumn, fb_nh4, fb_co2, fh_nh4, fh_co2;
	int    cell, l = 0,ll, vv;
	double  RFM, RFMM, RFMT;
	double Fl;
	double Fhc, ActSOC, CRB, Thc,new_ActSOC,dActSOC;
	double dlitter, Ftw;
	double sum_drcvl, f_till_fact;
	double r_RFM, Frcvl, Frcl,Frcr;
	double PTAN, PDN, p_drcl, p_DRCB2, total_drcl;
	double ddN,rf,tin;
	double FungiLitterDecomposition, Ftill,	FungiN ,FungiMicrobe,AdditionCO21;
	double FungiLitterDecomposition2,FungiN2 ,FungiMicrobe2,AdditionCO22;
	double drcra, DRCB3, TN1, TN2, dTN;
	double p_drcr, p_DRCB3;
	double DCRB1, DCRB2, DCRB, FBBC, FBHC, Fo;
	double dchr, dchl,  dhbcr, dhbcl, nhumr, nhuml, k1, k2;
	double crhl_co2, crhr_co2;
	double d_humus, d_factor, Fclay, Fdepth, Ftemp;
	double wd1, wd2, Kw, Ka, hydrogen, hydroxide, V_water, mol_nh4, mol_nh3;
	double cvf,vol_nh3,soil_T,Fsd,Fpo,Fwi,Fst,ddf;
	double active_clay_nh4, totalN, FIXRATE, lbcn, lmcn,TotalNH4;
	double humad_co2;
	double EarthWormActivity;
	double ConsumedLitter1, ConsumedLitter2,ConsumedLitter3;
	double TB, BAC, dDOC;
	double fw;
	double VVw,TP,dLabP,dAdsP,TTP;
	double wrnvl, wrnl, wrnr, wnrb, wnrh,  whumusn;
	double total_drcr;
	double Ftill2;

	double PinW=0.0;//P concentration in soil pore water, mg P/L
	double PinS=0.0;//amount of P in adsorbed phase, mg P/kg soil
	double adsPmax=700.0;//P adsorption maximum, mg P/kg soil
	double adsPK=0.1;//Constant related to bonding energy, L/mg P
	double adsPo=0.0;//quantity of P retained under ambient conditions, mg P/kg soil


	assert(m);
	met = (MET_DATA*) yos[years].m;
	//annual daily loop on each cell before start
	for ( cell = 0; cell < m->cells_count; cell++)
	{
		//yeah does it work babe?
		//*************FOREST CHARACTERISTIC*********************
		//first day of the year: reset variables
		if (day == 0 && month== JANUARY && years == 0 )
		{
			//initialization
			drcvl=(double)0.0;
			DRCB=(double)0.0;
			DRCB1=(double)0.0;
			DRCB2=(double)0.0;
			lit_co2=(double)0.0;
			sumn=(double)0.0;
			fb_nh4=(double)0.0;
			fb_co2=(double)0.0;
			//dcbavai=(double)0.0;
			fh_co2=(double)0.0;
			fh_nh4=(double)0.0;
			soil_initialization(&m->cells[cell]);
		}
		else
		{
			//
			m->cells[cell].AddC = 0;
			m->cells[cell].AddC1 = 0;
			m->cells[cell].AddC2 = 0;
			m->cells[cell].AddC3 = 0;
			m->cells[cell].soils[0].co2 = 0.0;
			m->cells[cell].soils[0].day_O2 = 0.0;
			m->cells[cell].soils[0].drcvl = 0.0;
			m->cells[cell].soils[0].DRCB1 = 0.0;
			m->cells[cell].soils[0].DRCB2 = 0.0;
			m->cells[cell].soils[0].litco22 = 0.0;
			m->cells[cell].soils[0].litco23 = 0.0;

			tree_leaves_fall(m, cell);
		}

		if (day == 0 && month == JANUARY)
		{
			//reinitialize data
			get_av_year_temperature(&m->cells[cell], years, month, day, met);
		}
		soil_temperature(&m->cells[cell], years, month, day, met);



		m->cells[cell].soils[l].nh4 += 0.4;
		m->cells[cell].till_fact = 1.0;
		//effect of clay adsorption: according to Zhang no 2.3026
		clayc = (double)(log(.14 / (g_soil->values[SOIL_CLAY_PERC]/100)) / 2.3026 + 1);
		krh = (double).16 * clayc;
		hrh = (double).006 * clayc;

		//limiting factors f(clay%)
		if(krh>1.0) krh=1.0;
		if(hrh>1.0) hrh=1.0;
		for ( l = 0; l < m->cells[cell].soils_count;  l++ )
		{
			Fl=1.0;
			Fl = 0.6 * (double)pow(l+1, -0.1366);
			if(Fl>1.3) Fl = 1.3;
			if(Fl<0.0) Fl = 0.0;

			//m->cells[cell].soils[l].soilTemp = met[month].d[day].tsoil;
			//DRF is defined as a constant (0.03)
			//this considered the imapct of soil clay and microbio
			//activity in each layer

			DDRF = (DRF - 0.02 * g_soil->values[SOIL_CLAY_PERC]/100) * Fl;	   // * pow(m->cells[cell].MicrobioIndex, 0.1);
			if ( DDRF < 0.0 ) DDRF = 0.0;

			//sergio: assumed bucket model for now (unilayer situation)
			m->cells[cell].soils[l].soilMoisture = m->cells[cell].soil_moist_ratio;

			// Effect of temp/mois on decomposition
			if (m->cells[cell].soils[l].soilMoisture<0.00001) m->cells[cell].soils[l].soilMoisture= 0.00001;

			//RFMM = (-4.3717 * pow(m->cells[cell].soils[l].soilMoisture, 2) + 5.1364 * m->cells[cell].soils[l].soilMoisture - 0.5455);
			//RFMM = (-5.9991 * pow(m->cells[cell].soils[l].soilMoisture, 2) + 7.8176 * m->cells[cell].soils[l].soilMoisture - 1.6204);
			//RFMM = -4.1107* pow(m->cells[cell].soils[l].soilMoisture, 2) + 5.1597* m->cells[cell].soils[l].soilMoisture - 0.6429;
			RFMM = -2.8516* pow(m->cells[cell].soils[l].soilMoisture, 3)+ 1.4936* pow(m->cells[cell].soils[l].soilMoisture, 2) +
					1.7699* m->cells[cell].soils[l].soilMoisture - 0.0301;

			RFMM = Maximum(0.0, RFMM);
			//if(day_wfps[1]<wiltpt) RFMM *= 0.01;

			if (m->cells[cell].soils[l].soilTemp<=0.0)
				RFMT = 0.01;
			else
				RFMT = (-.0014 * m->cells[cell].soils[l].soilTemp * m->cells[cell].soils[l].soilTemp) + 0.0992 *
				m->cells[cell].soils[l].soilTemp + 0.0198;

			if (RFMT <= 0.0) RFMT = 0.0;
			RFM = RFMM * RFMT;
			//RFM=Minimum(1.0, Maximum(0.0, RFM));	if(st[l]>=12) *RFM *= 0.01;//0.1;


			if(m->cells[cell].soils[l].ice >0.0) RFM = (0.9 - m->cells[cell].soils[l].soilMoisture) * 0.05;//0.015;//
			RFM=Minimum(1.0, Maximum(0.0, RFM));

			// Redefine daily microbes/humads
			m->cells[cell].soils[l].soc = m->cells[cell].soils[l].rcvl + m->cells[cell].soils[l].rcl + m->cells[cell].soils[l].rcr + m->cells[cell].soils[l].CRB1 +
					m->cells[cell].soils[l].CRB2 + m->cells[cell].soils[l].crhl +m->cells[cell].soils[l].crhr + m->cells[cell].soils[l].dphum;

			m->cells[cell].soils[l].no3 += m->cells[cell].soils[l].no2;
			m->cells[cell].soils[l].no2 = 0.0;

			//decomposition of surface litter
			if((m->cells[cell].leafLittering + m->cells[cell].fineRootLittering+m->cells[cell].coarseRootLittering+
					m->cells[cell].stemLittering+m->cells[cell].stemBrancLittering)>0.0&&met[month].d[day].tavg>0.0)
			{
				float Ftw= met[month].d[day].tavg * 0.001;//0.005
				float dS1, dS2, dS3, dS4, dS5;

				//Ftw = temp[1] / 40.0 * day_wfps[1];
				Ftw = Maximum(0.0, Minimum(Ftw, 0.5));

				dS1 = m->cells[cell].leafLittering * Ftw;
				m->cells[cell].soils[l].co2 += dS1;
				m->cells[cell].soils[l].day_O2 += dS1;
				if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
				m->cells[cell].soils[l].nh4 += (dS1 / g_soil->values[SOIL_RCNRVL]);
				m->cells[cell].leafLittering -= dS1;
				if ( m->cells[cell].leafLittering <= 0.0000001 ) m->cells[cell].leafLittering = 0.0;

				dS2 = m->cells[cell].fineRootLittering * Ftw;
				m->cells[cell].soils[l].co2 += dS2;
				m->cells[cell].soils[l].day_O2 += dS2;
				if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
				m->cells[cell].soils[l].nh4 += (dS2 / g_soil->values[SOIL_RCNRVL]);
				m->cells[cell].fineRootLittering -= dS2;
				if ( m->cells[cell].fineRootLittering <= 0.0000001 ) m->cells[cell].fineRootLittering = 0.0;

				dS3 = m->cells[cell].coarseRootLittering * Ftw;
				m->cells[cell].soils[l].co2 += dS3;
				m->cells[cell].soils[l].day_O2 += dS3;
				if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
				m->cells[cell].soils[l].nh4 += (dS3 / g_soil->values[SOIL_RCNRL]);
				m->cells[cell].coarseRootLittering -= dS3;
				if ( m->cells[cell].coarseRootLittering <= 0.0000001 ) m->cells[cell].coarseRootLittering = 0.0;

				dS4 = m->cells[cell].stemLittering * Ftw;
				m->cells[cell].soils[l].co2 += dS4;
				m->cells[cell].soils[l].day_O2 += dS4;
				if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
				m->cells[cell].soils[l].nh4 += (dS4 / g_soil->values[SOIL_RCNRR]);
				m->cells[cell].stemLittering -= dS4;
				if ( m->cells[cell].stemLittering <= 0.0000001 ) m->cells[cell].stemLittering = 0.0;

				dS5 = m->cells[cell].stemBrancLittering * Ftw;
				m->cells[cell].soils[l].co2 += dS5;
				m->cells[cell].soils[l].day_O2 += dS5;
				if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
				m->cells[cell].soils[l].nh4 += (dS5 / g_soil->values[SOIL_RCNRR]);
				m->cells[cell].stemBrancLittering -= dS5;
				if ( m->cells[cell].stemBrancLittering <= 0.0000001 ) m->cells[cell].stemBrancLittering = 0.0;
			}

			// Decomposition of very labile litter
			f_till_fact = 1.0; // assumed no tillage on forested soils, thus even layer 0 is not tilled
			/* potential CO2 from rcvl decomposition */
			r_RFM = Maximum(0.0, Minimum(1.0, RFM));//max(0.7, min(1.0, RFM));
			Frcvl = 2.0 * r_RFM * KRCVL * DDRF * g_soil->values[SOIL_DC_LITTER] * f_till_fact;//* f_till_fact
			if(Frcvl>1.0)
				Frcvl=1.0;

			m->cells[cell].soils[l].drcvl = Frcvl * m->cells[cell].soils[l].rcvl;
			// microbes formed from rcvl decomposition
			m->cells[cell].soils[l].DRCB1 = (m->cells[cell].soils[l].drcvl) * EFFRB;
			sum_drcvl = (m->cells[cell].soils[l].drcvl) + (m->cells[cell].soils[l].DRCB1);
			m->cells[cell].soils[l].rcvl -= sum_drcvl;
			m->cells[cell].soils[l].nh4+= (sum_drcvl / g_soil->values[SOIL_RCNRVL]);
			m->cells[cell].soils[l].co2+= m->cells[cell].soils[l].drcvl;
			m->cells[cell].soils[l].day_O2+= m->cells[cell].soils[l].drcvl;

			if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
			m->cells[cell].soils[l].CRB1 += ((m->cells[cell].soils[l].DRCB1) * SRB);
			m->cells[cell].soils[l].CRB2+= ((m->cells[cell].soils[l].DRCB1) * (1.0 - SRB));
			m->cells[cell].soils[l].nh4 -= (m->cells[cell].soils[l].DRCB1 / g_soil->values[SOIL_RCNB]);

			// N demand for microbes formation
			//DRNB1 = (*DRCB1) / rcnb;
			// suplas N
			//FreeN1 = sum_drcvl / rcnrvl - DRNB1;

			m->cells[cell].day_C_mine += m->cells[cell].soils[l].drcvl;
			m->cells[cell].day_N_mine += m->cells[cell].soils[l].drcvl/g_soil->values[SOIL_RCNRVL];//1(sum_drcvl / rcnrvl);
			m->cells[cell].day_N_assim += (m->cells[cell].soils[l].DRCB1 / g_soil->values[SOIL_RCNB]);

			//----------------------------------------------------------------------------
			// Decomposition of labile litter
			//if ( l > (tilq + 3) ) f_till_fact = 1.0;
			if (l>=m->cells[cell].tilq) f_till_fact = 1.0;// + 3
			else  f_till_fact = m->cells[cell].till_fact;

			/* potential CO2 from rcl decomposition */
			Frcl = 2.0*RFM * KRCL * DDRF * g_soil->values[SOIL_DC_LITTER] * f_till_fact;//* f_till_fact
			if(Frcl>1.0)
				Frcl=1.0;

			p_drcl = m->cells[cell].soils[l].rcl * Frcl;

			/* potential microbe formation from rcl decomposition */
			p_DRCB2 = p_drcl * EFFRB;

			/* potential total rcl decomposition */
			total_drcl = p_drcl + p_DRCB2;

			/* N release from potential total rcl decomposition */
			PTAN = total_drcl / g_soil->values[SOIL_RCNRL];

			/* potential N demand for microbial formation */
			PDN = p_DRCB2 / g_soil->values[SOIL_RCNB];

			/* net release of N */
			ddN = PTAN - PDN;

			if(ddN >= 0.0)
			{
				m->cells[cell].soils[l].litco22 = p_drcl;
				m->cells[cell].soils[l].DRCB2 = p_DRCB2;

				//no3[l] += 0.5 * ddN;
				m->cells[cell].soils[l].nh4 += ddN;

				//day_minen += ddN;
			}
			else
			{
				if((m->cells[cell].soils[l].no3 + m->cells[cell].soils[l].nh4) >= (-ddN))
				{
					m->cells[cell].soils[l].litco22 = p_drcl;
					m->cells[cell].soils[l].DRCB2 = p_DRCB2;
					m->cells[cell].soils[l].no3 -= m->cells[cell].soils[l].no3/ (m->cells[cell].soils[l].no3+ m->cells[cell].soils[l].nh4) * (-ddN);
					m->cells[cell].soils[l].nh4-= m->cells[cell].soils[l].nh4/ (m->cells[cell].soils[l].no3+ m->cells[cell].soils[l].nh4) * (-ddN);
				}
				else
				{
					rf = (m->cells[cell].soils[l].no3 + m->cells[cell].soils[l].nh4) / (-ddN);

					m->cells[cell].soils[l].litco22 = rf * p_drcl;
					m->cells[cell].soils[l].DRCB2 = rf * p_DRCB2;

					m->cells[cell].soils[l].no3 = 0.0;
					m->cells[cell].soils[l].nh4= 0.0;
				}

				//day_minen += ddN;
			}

			m->cells[cell].soils[l].rcl -= (m->cells[cell].soils[l].litco22+m->cells[cell].soils[l].DRCB2);
			m->cells[cell].soils[l].co2 += m->cells[cell].soils[l].litco22;
			m->cells[cell].soils[l].day_O2+= m->cells[cell].soils[l].litco22;
			if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
			m->cells[cell].soils[l].CRB1 += ((m->cells[cell].soils[l].DRCB2) * SRB);
			m->cells[cell].soils[l].CRB2 += ((m->cells[cell].soils[l].DRCB2) * (1.0 - SRB));

			m->cells[cell].day_C_mine += m->cells[cell].soils[l].litco22;//net C release
			m->cells[cell].day_N_mine += m->cells[cell].soils[l].litco22 / g_soil->values[SOIL_RCNRL];//2(soils[l].litco22+*DRCB2) / rcnrl; //gross N mineralization
			m->cells[cell].day_N_assim += m->cells[cell].soils[l].DRCB2 / g_soil->values[SOIL_RCNB];//N assimilation

			if(l<=(int)(0.15/g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY]))
			{
				Ftill=0.01;
				FungiLitterDecomposition = m->cells[cell].soils[l].rcl * Ftill * (RFM);
				FungiN = FungiLitterDecomposition / g_soil->values[SOIL_RCNRL];
				FungiMicrobe = FungiN * g_soil->values[SOIL_RCNB];
				AdditionCO21 = FungiLitterDecomposition - FungiMicrobe;
				m->cells[cell].soils[l].co2 += AdditionCO21;
				m->cells[cell].soils[l].day_O2 += AdditionCO21;
				if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
				m->cells[cell].soils[l].rcl -= FungiLitterDecomposition;
				m->cells[cell].day_C_mine += AdditionCO21;
				m->cells[cell].day_N_assim += FungiN;
				m->cells[cell].soils[l].CRB1 += (FungiMicrobe * SRB);
				m->cells[cell].soils[l].CRB2 += (FungiMicrobe * (1.0 - SRB));
				m->cells[cell].soils[l].DRCB2 += FungiMicrobe;
			}

			//-------------------------------------------------------------------------------------
			// Decomposition of resistant litter
			TN1 = m->cells[cell].soils[l].rcr/g_soil->values[SOIL_RCNRR] + (m->cells[cell].soils[l].CRB1+m->cells[cell].soils[l].CRB2)/g_soil->values[SOIL_RCNB] + m->cells[cell].soils[l].no3 +m->cells[cell].soils[l].nh4 ;
			if (l>=m->cells[cell].tilq) f_till_fact = 1.0;// + 3
			else  f_till_fact = m->cells[cell].till_fact;

			/* potential CO2 from rcr decomposition */
			Frcr = 2.0*RFM * KRCR * DDRF * g_soil->values[SOIL_DC_LITTER] * f_till_fact;//

			if(Frcr>1.0) Frcr=1.0;
			//sergio if(l==q+1) Frcr *= 1000.0;
			Frcr = Minimum(1.0, Maximum(0.0, Frcr));

			p_drcr = Frcr *m->cells[cell].soils[l].rcr ;

			/* potential microbe formation from rcr decomposition */
			p_DRCB3 = p_drcr * EFFRB;

			/* potential total rcr decomposition */
			total_drcr = p_drcr + p_DRCB3;

			/* N release from potential total rcr decomposition */
			PTAN = total_drcr / g_soil->values[SOIL_RCNRR];

			/* potential N demand for microbe formation */
			PDN = p_DRCB3 / g_soil->values[SOIL_RCNB];

			/* deficiency of N */
			ddN = PTAN - PDN;

			if(ddN >= 0.0)
			{
				m->cells[cell].soils[l].litco23 = p_drcr;
				DRCB3 = p_DRCB3;

				//no3  += 0.5 * ddN;
				m->cells[cell].soils[l].nh4  += ddN;
				//day_minen += ddN;
			}
			else
			{
				if((m->cells[cell].soils[l].no3  +m->cells[cell].soils[l].nh4 ) >= (-ddN))
				{
					m->cells[cell].soils[l].litco23 = p_drcr;
					DRCB3 = p_DRCB3;
					tin = m->cells[cell].soils[l].no3  + m->cells[cell].soils[l].nh4 ;
					m->cells[cell].soils[l].no3  -= m->cells[cell].soils[l].no3 / tin * (-ddN);
					m->cells[cell].soils[l].nh4  -= m->cells[cell].soils[l].nh4 / tin * (-ddN);
				}
				else
				{
					rf = (m->cells[cell].soils[l].no3  + m->cells[cell].soils[l].nh4 ) / (-ddN);

					m->cells[cell].soils[l].litco23 = rf * p_drcr;
					DRCB3 = rf * p_DRCB3;

					m->cells[cell].soils[l].no3  = 0.0;
					m->cells[cell].soils[l].nh4  = 0.0;
				}

				//day_minen += ddN;
			}

			m->cells[cell].soils[l].rcr  -= (m->cells[cell].soils[l].litco23+DRCB3);
			m->cells[cell].soils[l].co2  += m->cells[cell].soils[l].litco23;
			m->cells[cell].soils[l].day_O2  += m->cells[cell].soils[l].litco23;
			if(m->cells[cell].soils[l].day_O2 <0.0) m->cells[cell].soils[l].day_O2 =0.0;

			m->cells[cell].soils[l].CRB1  += (DRCB3 * SRB);
			m->cells[cell].soils[l].CRB2  += (DRCB3 * (1.0 - SRB));

			TN2 = m->cells[cell].soils[l].rcr /g_soil->values[SOIL_RCNRR] +
					(m->cells[cell].soils[l].CRB1 +m->cells[cell].soils[l].CRB2 )/g_soil->values[SOIL_RCNB] +
					m->cells[cell].soils[l].no3 +m->cells[cell].soils[l].nh4 ;

			dTN = Maximum(TN2 - TN1, 0);

			m->cells[cell].soils[l].nh4  -= dTN;
			if(m->cells[cell].soils[l].nh4 <0.0) m->cells[cell].soils[l].nh4  = 0.0;

			m->cells[cell].day_C_mine += m->cells[cell].soils[l].litco23;//net C release
			m->cells[cell].day_N_mine += m->cells[cell].soils[l].litco23 / g_soil->values[SOIL_RCNRR];//3(soils[l].litco23+DRCB3) / rcnrl; //gross N mineralization
			m->cells[cell].day_N_assim += DRCB3 / g_soil->values[SOIL_RCNB];//N assimilation

			drcra = p_drcr;

			lit_co2 = drcvl + m->cells[cell].soils[l].litco22 + m->cells[cell].soils[l].litco23;//!!!

			sumn = drcvl / g_soil->values[SOIL_RCNRVL] + m->cells[cell].soils[l].litco22 / g_soil->values[SOIL_RCNRL] + drcra / g_soil->values[SOIL_RCNRR];   /* not accurate */

			m->cells[cell].soils[l].litco22 = 0.0;
			drcvl = 0.0;
			m->cells[cell].soils[l].litco23 = 0.0;

			if(l<=(int)(0.15/g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY]))
			{

				Ftill2=0.0001;//0.0005
				FungiLitterDecomposition2 = m->cells[cell].soils[l].rcr  * Ftill * (RFM);
				FungiN2 = FungiLitterDecomposition2 / g_soil->values[SOIL_RCNRR];
				FungiMicrobe2 = FungiN2 * g_soil->values[SOIL_RCNB];
				AdditionCO22 = FungiLitterDecomposition2 - FungiMicrobe2;
				m->cells[cell].soils[l].co2  += AdditionCO22;
				m->cells[cell].soils[l].day_O2  += AdditionCO22;
				if(m->cells[cell].soils[l].day_O2 <0.0) m->cells[cell].soils[l].day_O2 =0.0;
				m->cells[cell].soils[l].rcr  -= FungiLitterDecomposition2;
				m->cells[cell].soils[l].CRB1  += (FungiMicrobe2 * SRB);
				m->cells[cell].soils[l].CRB2  += (FungiMicrobe2 * (1.0 - SRB));
				m->cells[cell].day_C_mine += AdditionCO22;
				m->cells[cell].day_N_assim += FungiN2;
				DRCB3 += FungiMicrobe2;
			}

			DRCB = DRCB1 + DRCB2 + DRCB3;

			// Life and death of soil microbes
			//if(st[l]>=12) Fo=0.1;
			//else Fo=0.7;
			Fo=0.7;

			//if ( l > (tilq + 3) ) f_till_fact = 1.0;
			m->cells[cell].tilq = 1.0;
			if (l>=m->cells[cell].tilq) f_till_fact = 1.0;// + 3
			else  f_till_fact = m->cells[cell].till_fact;

			TN1 = (m->cells[cell].soils[l].CRB1+m->cells[cell].soils[l].CRB2)/g_soil->values[SOIL_RCNB] +
					m->cells[cell].soils[l].nh4 + m->cells[cell].soils[l].crhr/g_soil->values[SOIL_RCNH2];

			/* microbe decomposition */
			DCRB1 = RFM * KRB * DDRF * f_till_fact * m->cells[cell].soils[l].CRB1 * Fo * g_soil->values[SOIL_DC_HUMADS];
			DCRB2 = RFM * HRB * DDRF * f_till_fact * m->cells[cell].soils[l].CRB2 * Fo * g_soil->values[SOIL_DC_HUMADS];
			DCRB = DCRB1 + DCRB2;

			m->cells[cell].soils[l].CRB1 -= (DCRB1);
			m->cells[cell].soils[l].CRB2 -= (DCRB2);

			m->cells[cell].soils[l].nh4 += (DCRB / g_soil->values[SOIL_RCNB]);

			/* efficiency */
			FBBC = DCRB * EFFAC;	//    EFFAC = 0.2;

			/* new microbe */
			m->cells[cell].soils[l].CRB1 += (FBBC * SRB);
			m->cells[cell].soils[l].CRB2 += (FBBC * (1.0 - SRB));

			m->cells[cell].soils[l].nh4 -= (FBBC / g_soil->values[SOIL_RCNB]);
			m->cells[cell].day_N_assim += (FBBC / g_soil->values[SOIL_RCNB]);

			m->cells[cell].soils[l].dcbavai = FBBC;

			/* new resistant humads from microbe decomposition */
			FBHC = DCRB * EFFNO;
			m->cells[cell].soils[l].crhr += FBHC;

			m->cells[cell].soils[l].nh4 -= (FBHC / g_soil->values[SOIL_RCNH2]);
			//m->cells[cell].day_N_assim += (FBHC / g_soil->values[SOIL_RCNH]);


			/* CO2 from net microbe decomposition */
			fb_co2 = DCRB - FBBC - FBHC;
			m->cells[cell].soils[l].co2 += fb_co2;
			m->cells[cell].soils[l].day_O2 += fb_co2;
			if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
			m->cells[cell].day_C_mine += fb_co2;
			m->cells[cell].day_N_mine += fb_co2/ g_soil->values[SOIL_RCNB];//4

			/* NH4 from net microbe decomposition */
			fb_nh4 = (fb_co2) / g_soil->values[SOIL_RCNB];


			TN2 = (m->cells[cell].soils[l].CRB1+m->cells[cell].soils[l].CRB2)/g_soil->values[SOIL_RCNB] +
					m->cells[cell].soils[l].nh4 + m->cells[cell].soils[l].crhr/g_soil->values[SOIL_RCNH2];
			dTN = TN2 - TN1;
			m->cells[cell].soils[l].nh4 -= dTN;
			m->cells[cell].soils[l].nh4 = Maximum(0.0, m->cells[cell].soils[l].nh4);




			// Decomposition of humads (this function has error source)
			//if ( l > (tilq + 3) ) f_till_fact = 1.0;
			if (l>=m->cells[cell].tilq) f_till_fact = 1.0;// + 3
			else  f_till_fact =m->cells[cell].till_fact;

			// humads decomposition */
			Fclay = 0.5 * pow( g_soil->values[SOIL_CLAY_PERC] /100 , -0.471);

			k1 = 0.8 * RFM * hrh * DDRF * f_till_fact * Fclay * g_soil->values[SOIL_DC_HUMADS];//1.0
			k2 = 0.8 * RFM * krh * DDRF * f_till_fact * Fclay * g_soil->values[SOIL_DC_HUMADS];//1.0

			if(l==(m->cells[cell].soils_count+5))
			{
				k1 *= 10000.0;
				k2 *= 10000.0;
			}

			if(k1>1.0) k1=1.0;
			if(k2>1.0) k2=1.0;
			dchr = k1 * m->cells[cell].soils[l].crhr ;
			dchl = k2 * m->cells[cell].soils[l].crhl ;
			//
			//	double dPP = k1 * *OrgP ;
			//	*LabP  += dPP;
			//	*OrgP  -= dPP;

			TN1 = m->cells[cell].soils[l].crhr /g_soil->values[SOIL_RCNH2] + m->cells[cell].soils[l].crhl /
					g_soil->values[SOIL_RCNH] +m->cells[cell].soils[l].nh4  +
					(m->cells[cell].soils[l].CRB1 +m->cells[cell].soils[l].CRB2 )/g_soil->values[SOIL_RCNB] +
					m->cells[cell].soils[l].dphum /g_soil->values[SOIL_RCNM];//new humads

			m->cells[cell].soils[l].crhr  -= dchr;
			m->cells[cell].soils[l].crhl  -= dchl;

			crhr_co2 = dchr * 0.5;
			m->cells[cell].soils[l].co2  += crhr_co2;
			m->cells[cell].soils[l].day_O2  += crhr_co2;
			if(m->cells[cell].soils[l].day_O2 <0.0) m->cells[cell].soils[l].day_O2 =0.0;
			m->cells[cell].soils[l].nh4  += (crhr_co2 / g_soil->values[SOIL_RCNH2]);

			// efficiency */
			dhbcr = dchr * 0.2;
			// new microbe
			m->cells[cell].soils[l].CRB1  += (dhbcr * SRB);
			m->cells[cell].soils[l].CRB2  += (dhbcr * (1.0 - SRB));
			nhumr = dchr - (dhbcr + crhr_co2);
			m->cells[cell].soils[l].dphum  += nhumr;

			crhl_co2 = dchl * 0.8;
			m->cells[cell].soils[l].co2  += crhl_co2;
			m->cells[cell].soils[l].day_O2  += crhl_co2;
			if(m->cells[cell].soils[l].day_O2 <0.0) m->cells[cell].soils[l].day_O2 =0.0;
			m->cells[cell].soils[l].nh4  += (crhl_co2 / g_soil->values[SOIL_RCNH]);
			// efficiency */
			dhbcl = dchl * 0.15;
			// new microbe
			m->cells[cell].soils[l].CRB1  += (dhbcl * SRB);
			m->cells[cell].soils[l].CRB2  += (dhbcl * (1.0 - SRB));
			nhuml = dchl - (dhbcl + crhl_co2);
			m->cells[cell].soils[l].dphum  += nhuml;

			TN2 = m->cells[cell].soils[l].crhr /g_soil->values[SOIL_RCNH2] + m->cells[cell].soils[l].crhl /
					g_soil->values[SOIL_RCNH] + m->cells[cell].soils[l].nh4  + (m->cells[cell].soils[l].CRB1 +
							m->cells[cell].soils[l].CRB2 )/g_soil->values[SOIL_RCNB] + m->cells[cell].soils[l].dphum /g_soil->values[SOIL_RCNM];
			dTN = TN2 - TN1;
			m->cells[cell].soils[l].nh4  -= dTN;

			m->cells[cell].soils[l].nh4  = Maximum(0.0, m->cells[cell].soils[l].nh4 );

			humad_co2 = crhr_co2 + crhl_co2;
			m->cells[cell].day_C_mine += humad_co2;
			m->cells[cell].day_N_mine += (crhl_co2/g_soil->values[SOIL_RCNH] + crhr_co2/g_soil->values[SOIL_RCNH2]);

			if (m->cells[cell].soils[l].soilTemp  >= 0.0)
			{
				//m->cells[cell].soils[l].Odoc  += (dcbavai + dhbcr+dhbcl + DRCB);
			}
			else
			{
			}


			//decomp_humus

			//d_factor = dphum[l] * 0.000005 - 0.004;
			//if ( l > (tilq + 3) ) f_till_fact = 1.0;
			if (l>=m->cells[cell].tilq) f_till_fact = 1.0;// + 3
			else  f_till_fact = m->cells[cell].till_fact;

			//			if(l>m->cells[cell].soils_count-1)
			//			{
			//				g_soil->values[SOIL_CLAY_PERC]= 0.01;
			//				//previously soilDepth[l]
			//				m->cells[cell].soils[l].soilDepth = 0.01;
			//			}

			//if(st[l]<12)
			Fclay = 0.1793 * pow(g_soil->values[SOIL_CLAY_PERC] /100, -0.471) * 20.0;

			Fclay=Maximum(0.0, Minimum(1.0, Fclay));

			//if(st==12) Fclay *= 0.1;
			//fixSergio being a single layer, assumed the value at depth .5
			//Fdepth = (double)pow(10.0, (-20.0 * m->cells[cell].soils[l].soilDepth) + 1.0);
			Fdepth = (double)pow(10.0, (-20.0 * g_soil->values[SOIL_DEPTH]) + 1.0); //it shouyld be layer depth
			Fdepth=Maximum(0.0, Minimum(1.0, Fdepth));

			Ftemp = m->cells[cell].soils[l].soilTemp / 25.0;
			Ftemp = Minimum(1.0, Maximum(0.001, Ftemp));

			d_factor = 0.00001 * RFM * DDRF * f_till_fact * Fclay * Fdepth * g_soil->values[SOIL_DC_HUMUS] * Ftemp;//0.0008

			if(d_factor < 0.0) d_factor = 0.0;

			d_humus = m->cells[cell].soils[l].dphum* d_factor;

			TN1 = m->cells[cell].soils[l].dphum/g_soil->values[SOIL_RCNM] + m->cells[cell].soils[l].nh4;

			m->cells[cell].soils[l].dphum -= d_humus;
			if(m->cells[cell].soils[l].dphum < 0.0)
			{
				m->cells[cell].soils[l].dphum = 0.0;
				d_humus = 0.0;
			}

			m->cells[cell].soils[l].nh4 += (d_humus / g_soil->values[SOIL_RCNM]);
			m->cells[cell].soils[l].co2 += d_humus;
			m->cells[cell].soils[l].day_O2 += d_humus;
			if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;

			m->cells[cell].day_N_mine += (d_humus / g_soil->values[SOIL_RCNM]);//6

			m->cells[cell].day_C_mine += d_humus;

			TN2 = m->cells[cell].soils[l].dphum/g_soil->values[SOIL_RCNM] + m->cells[cell].soils[l].nh4;

			dTN = TN2 - TN1;

			m->cells[cell].soils[l].nh4 -= dTN;

			if(m->cells[cell].soils[l].nh4<0.0)m->cells[cell].soils[l].nh4 = 0.0;
			// NH4+/NH3(l) equilibrium in liquid phase
			//if(l<=q)

			//Chemical equilibrium: NH4+ + OH- = NH3 + H2O

			wd1 = (double)pow(10.0, -15.0);
			wd2 = (double)pow(10.0, -5.0);
			Kw = 1.945 * exp(0.0645 * m->cells[cell].soils[l].soilTemp) * wd1;//water dissociation constant
			Ka = (1.416 + 0.01357 * m->cells[cell].soils[l].soilTemp) * wd2;//NH4+/NH3 equilibrium constant
			hydrogen = (double)pow(10.0, -g_soil->values[SOIL_PH]);//mol/L
			hydroxide = Kw / hydrogen;//mol/L

			//suspect its porosity
			V_water = m->cells[cell].soils[l].soilMoisture * g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY] *10000 * 1000.0;//liter water/ha in layer l

			mol_nh4 = 1000.0 * m->cells[cell].soils[l].nh4 / 14.0 / V_water;//kg N -> mol/L
			mol_nh3 = mol_nh4 * hydroxide / Ka;//mol/L

			if((mol_nh4 + mol_nh3)<=0.0)
			{
				m->cells[cell].soils[l].nh4 = 0.0;
				m->cells[cell].soils[l].nh3 = 0.0;
			}
			else
			{
				cvf = 0.1 * mol_nh4 / (mol_nh4 + mol_nh3);
				cvf = Minimum(1.0, Maximum(0.0, cvf));
				m->cells[cell].soils[l].nh4 = (m->cells[cell].soils[l].nh4+m->cells[cell].soils[l].nh3) * cvf;
				m->cells[cell].soils[l].nh3 = (m->cells[cell].soils[l].nh4+m->cells[cell].soils[l].nh3) - m->cells[cell].soils[l].nh4;//kg N/ha in layer l

				//-------------------------------------------------------------------------------------------------
				// NH3 volatilization from soil
				vol_nh3=0.0;

				if (m->cells[cell].soils[l].soilTemp < 0.0) soil_T = 0.0;
				else soil_T = m->cells[cell].soils[l].soilTemp;

				if(l<=m->cells[cell].soils_count &&m->cells[cell].soils[l].nh3>0.0) //field soil
				{
					//effects on gas difussion
					Fsd = 1.0 * (double)(m->cells[cell].soils_count-l)/(double)m->cells[cell].soils_count;//-3.6735 * (double)pow((double)l, 2.0)/(double)q - 0.7551;//effect of depth
					Fpo = 1.0;// - day_wfps[l];//effect of pores
					//		double Fwi = 1.5 * wind_speed / (1.0 + wind_speed);//0.5, effect of wind speed
					Fwi = .7;	//sergio debug
					Fst = soil_T / (50.0 + soil_T);//10.0


					ddf = 0.25 * Fsd * Fpo * Fwi * Fst;//0.25
					ddf = Maximum(0.0, Minimum(1.0, ddf));

					vol_nh3 = m->cells[cell].soils[l].nh3 * ddf;
					m->cells[cell].soils[l].nh3 -= vol_nh3;

					m->cells[cell].day_soil_nh3 += vol_nh3;


				}
				else
				{
					vol_nh3 = 0.0;
				}

				m->cells[cell].soils[l].nh4 += m->cells[cell].soils[l].nh3;
				m->cells[cell].soils[l].nh3 = 0.0;

				// NH4+ adsorption on clay
				TotalNH4 = m->cells[cell].soils[l].nh4 + m->cells[cell].soils[l].clay_nh4;
				soilCEC(&m->cells[cell]);
				lbcn = m->cells[cell].base_clay_N * (double)pow(0.5, (double)l+1);
				lmcn = m->cells[cell].max_clay_N * (double)pow(0.5, (double)l+1);

				if(m->cells[cell].soils[l].clay_nh4 > lbcn) active_clay_nh4 = m->cells[cell].soils[l].clay_nh4 - lbcn;
				else active_clay_nh4 = 0.0;

				totalN = m->cells[cell].soils[l].nh4 + active_clay_nh4;

				FIXRATE = 0.5 * (7.2733*pow(g_soil->values[SOIL_CLAY_PERC] /100, 3.0) - 11.22*pow(g_soil->values[SOIL_CLAY_PERC] /100, 2.0) + 5.7198*g_soil->values[SOIL_CLAY_PERC]/100 + 0.0263);//0.99

				//FIXRATE =  0.0144 * (double)exp(0.0981*CEC[l]);

				if ( FIXRATE < 0.01 ) FIXRATE = 0.01;
				if ( FIXRATE > 0.999) FIXRATE = 0.999;

				active_clay_nh4 = FIXRATE * totalN;

				m->cells[cell].soils[l].nh4 = (1.0 - FIXRATE) * totalN;

				m->cells[cell].soils[l].clay_nh4 = TotalNH4 - m->cells[cell].soils[l].nh4;//lbcn + active_clay_nh4;

				if(m->cells[cell].soils[l].clay_nh4 > lmcn)
				{
					m->cells[cell].soils[l].nh4 += (m->cells[cell].soils[l].clay_nh4 - lmcn);
					m->cells[cell].soils[l].clay_nh4 = lmcn;
				}
				//earthworms activity
				if(m->cells[cell].soils_count > 1){
					if(l+1<(int)(0.2/g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY]))
					{
						EarthWormActivity = (float)pow(0.0001, (float)l+1);
						if(m->cells[cell].soils[l].rcvl>m->cells[cell].soils[l+1].rcvl)
						{
							ConsumedLitter1 = EarthWormActivity * m->cells[cell].soils[l].rcvl;
							m->cells[cell].soils[l].rcvl -= ConsumedLitter1;
							m->cells[cell].soils[l+1].rcvl += ConsumedLitter1;
						}

						if(m->cells[cell].soils[l].rcl>m->cells[cell].soils[l+1].rcl)
						{
							ConsumedLitter2 = EarthWormActivity * m->cells[cell].soils[l].rcl;
							m->cells[cell].soils[l].rcl -= ConsumedLitter2;
							m->cells[cell].soils[l+1].rcl += ConsumedLitter2;
						}

						if(m->cells[cell].soils[l].rcr>m->cells[cell].soils[l+1].rcr)
						{
							ConsumedLitter3 = EarthWormActivity * m->cells[cell].soils[l].rcr;
							m->cells[cell].soils[l].rcr -= ConsumedLitter3;
							m->cells[cell].soils[l+1].rcr+= ConsumedLitter3;
						}
					}
				}
				// Recover layer lables
				m->cells[cell].soils[l].initialOrganicC = m->cells[cell].soils[l].CRB1 + m->cells[cell].soils[l].CRB2 + m->cells[cell].soils[l].crhl+ m->cells[cell].soils[l].crhr;
				//DOC consumption in soil
				if(l< m->cells[cell].soils_count)//&&doc[l]>0.0)
				{
					TB = m->cells[cell].soils[l].CRB1+m->cells[cell].soils[l].CRB2;
					BAC = m->cells[cell].soils[l].soilTemp/(40.0+m->cells[cell].soils[l].soilTemp) * TB/(500.0+TB);
					BAC = Maximum(0.0, Minimum(1.0, BAC));
					dDOC = BAC * m->cells[cell].soils[l].doc;
					m->cells[cell].soils[l].co2 += dDOC;
					m->cells[cell].soils[l].doc -= dDOC;

					m->cells[cell].soils[l].day_O2 += dDOC;
					if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
				}
				else
				{
					if(l<=m->cells[cell].soils_count+3) //feedlots
					{
						TB = m->cells[cell].soils[l].CRB1+m->cells[cell].soils[l].CRB2;
						BAC =m->cells[cell].soils[l].soilTemp/(40.0+m->cells[cell].soils[l].soilTemp) * TB/(500.0+TB);
						BAC = Maximum(0.0, Minimum(1.0, BAC));
						dDOC = BAC * m->cells[cell].soils[l].doc;
						m->cells[cell].soils[l].co2 += dDOC;
						m->cells[cell].soils[l].doc -= dDOC;
					}
				}

				// Daily profile accumulation
				m->cells[cell].wrcvl += m->cells[cell].soils[l].rcvl;
				m->cells[cell].wrcl += m->cells[cell].soils[l].rcl;
				m->cells[cell]. wrcr +=m->cells[cell].soils[l].rcr;
				m->cells[cell].wcrb += (m->cells[cell].soils[l].CRB1 + m->cells[cell].soils[l].CRB2);
				//				m->cells[cell].wcrhl += m->cells[cell].soils[l].crhl;
				//				m->cells[cell].wcrhr  = m->cells[cell].soils[l].crhr);
				m->cells[cell].whumus +=m->cells[cell].soils[l].dphum;
				//m->cells[cell].wFreezedoc += m->cells[cell].soils[l].FreezeDOC;

				m->cells[cell].soils[l].soc =m->cells[cell].soils[l].rcvl + m->cells[cell].soils[l].rcl + m->cells[cell].soils[l].rcr + m->cells[cell].soils[l].CRB1 + m->cells[cell].soils[l].CRB2 +
						m->cells[cell].soils[l].crhl + m->cells[cell].soils[l].crhr + m->cells[cell].soils[l].dphum;

				m->cells[cell].wtcavai += m->cells[cell].soils[l].doc;
				m->cells[cell].w_no2 += m->cells[cell].soils[l].no2;
				m->cells[cell].w_no3 += m->cells[cell].soils[l].no3;
				m->cells[cell].w_nh4 +=m->cells[cell].soils[l].nh4;
				m->cells[cell].w_nh3 += m->cells[cell].soils[l].nh3;
				m->cells[cell].day_clayn += m->cells[cell].soils[l].clay_nh4;

				//		if(m->cells[cell].flood_flag==1)
				//		{
				//			m->cells[cell].soils[l].n2 += m->cells[cell].soils[l].no*0.1;
				//			m->cells[cell].soils[l].no*=0.9;
				//			m->cells[cell].soils[l].n2 += m->cells[cell].soils[l].n2o*0.1;
				//			m->cells[cell].soils[l].n2o*=0.9;
				//		}
				m->cells[cell].wpool_no += m->cells[cell].soils[l].no;
				m->cells[cell].w_n2o += m->cells[cell].soils[l].n2o;
				//m->cells[cell].day_frostMicrobe += m->cells[cell].soils[l].frost_doc;

				m->cells[cell].leach_u = 0.0;
				m->cells[cell].day_no2 += (m->cells[cell].flux_no2);
				m->cells[cell].flux_no2 = 0.0;

				//day_minen += (sumn + fb_nh4 + fh_nh4);

				if ( day == 31 &&month == DECEMBER && l==m->cells[cell].soils_count -1)
				{

					wrnvl = m->cells[cell].wrcvl / g_soil->values[SOIL_RCNRVL];
					wrnl = m->cells[cell].wrcl / g_soil->values[SOIL_RCNRL];
					wrnr =m->cells[cell].wrcr / g_soil->values[SOIL_RCNRR];
					wnrb = m->cells[cell].wcrb /g_soil->values[SOIL_RCNB];
					wnrh = m->cells[cell].wcrh / g_soil->values[SOIL_RCNH];
					whumusn =m->cells[cell].whumus / g_soil->values[SOIL_RCNM];

					//todo m->cells[cell].End_SOC = m->cells[cell].wrcvl+m->cells[cell].wrcl+m->cells[cell].wrcr+m->cells[cell].wcrb+m->cells[cell].wcrh+m->cells[cell].whumus;


					//input_C = m->cells[cell].yr_addmc + m->cells[cell].yr_addtc + m->cells[cell].yr_addrc +m->cells[cell].yr_weedc;
					//output_C =m->cells[cell].yr_soil_co2 + m->cells[cell].yr_ch4 + m->cells[cell].yr_leach_hu;

					//delta = (IniSOC + input_C) - (EndSOC + output_C);
					//
					//de = delta / m->cells[cell].soils_count;	//q

					m->cells[cell].End_SON = wrnvl + wrnl + wrnr + wnrb + wnrh + whumusn;// + delta/rcnm;

				}
			}

			if(m->cells[cell].out_flow>0.0)
			{
				fw=m->cells[cell].out_flow * 1.0;//0.01
				fw = Minimum(0.9, Maximum(0.0, fw));
				m->cells[cell].runoff_N += fw * m->cells[cell].soils[l].no3;
				m->cells[cell].soils[l].no3 *= (1.0 - fw);
			}
			//dInertC = m->cells[cell].soils[l].inert_C;
			m->cells[cell].soils[l].inert_C = .049 * pow(m->cells[cell].soils[l].soc, 1.139);
			//decomposition of inert organic matter assumed as 1 x1000
			if( m->cells[cell].soils[l].inert_C>0.0)
			{
				m->cells[cell].soils[l].co2 += m->cells[cell].soils[l].inert_C*0.001;
				m->cells[cell].soils[l].inert_C *= 0.999;
				m->cells[cell].soils[l].day_O2 +=  m->cells[cell].soils[l].inert_C *0.001;
				if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
			}
			//	if(l==m->cells[cell].soils_count)
			//		//			P_adsorption(&m->cells[cell].soils[l].LabP, &m->cells[cell].soils[l].AdsP,&m->cells[cell].soils[l].soilMoisture, &m->cells[cell].soils[l].sts, &m->cells[cell].soils[l].mmm);//P adsorption
			//	{

			//			/* P adsorption */
			//			//Langmuir Equation
			//
			//			for(ll=0; ll<m->cells[cell].soils_count; ll++)
			//			{
			//				TP = m->cells[cell].soils[ll].LabP + m->cells[cell].soils[ll].AdsP;//kg P/layer
			//
			//				if(m->cells[cell].soils[ll].LabP<=0.0&& m->cells[cell].soils[ll].AdsP>0.0)
			//				{
			//					m->cells[cell].soils[ll].LabP = m->cells[cell].soils[ll].AdsP * 0.001;
			//					m->cells[cell].soils[ll].AdsP *= 0.999;
			//				}
			//				VVw = g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY]* 10000.0 * m->cells[cell].soils[ll].sts * m->cells[cell].soils[ll].soilMoisture * 1000.0; //water L/ha in layer l
			//				PinW = (m->cells[cell].soils[ll].LabP * 1000000.0) / VVw;//mg P/L in liquid
			//
			//				//Langmuir equation
			//				PinS = adsPmax * (adsPK * PinW) / (1.0 + adsPK * PinW) - adsPo;//mg P/kg soil, 2->100; 50-600 mg P/kg soil
			//
			//				dLabP = (PinW / 1000000.0) * VVw;//kg P/layer soil pore water;
			//				dAdsP = PinS / 1000000.0 * m->cells[cell].soils[ll].mmm;//kg P/layer soil
			//				TTP = dLabP + dAdsP;
			//
			//				if(TTP<=0.0)
			//				{
			//					m->cells[cell].soils[ll].AdsP = 0.0;
			//					m->cells[cell].soils[ll].LabP = 0.0;
			//				}
			//				else
			//				{
			//					m->cells[cell].soils[ll].AdsP = TP * (dAdsP / TTP);
			//					m->cells[cell].soils[ll].LabP = TP - m->cells[cell].soils[ll].AdsP;
			//				}
			//				//				m->cells[cell].day_co2 += co2[l];
			//				//							co2[l] = 0.0;
			//			}

			//}
			m->cells[cell].day_clay_N = 0.0;
			for( vv=0; vv<m->cells[cell].soils_count; vv++)
				m->cells[cell].day_clay_N += m->cells[cell].soils[vv].clay_nh4;

			m->cells[cell].wsoc = m->cells[cell].wrcvl + m->cells[cell].wrcl + m->cells[cell].wrcr + m->cells[cell].wcrb + m->cells[cell].wcrh + m->cells[cell].whumus;
			/*
	m->cells[cell].dsno3 = 0.0;
	m->cells[cell].dsnh4 = 0.0;
	m->cells[cell].dsnh3 = 0.0;
	m->cells[cell].ds_doc = 0.0;


	//calculate the total NO3, NH4, NH3 in 0-10cm
	for ( l = 0; l <(int)(0.1 / g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY]); l++ )
	{
		m->cells[cell].dsno3 += m->cells[cell].soils[l].no3;
		m->cells[cell].dsnh4 += m->cells[cell].soils[l].nh4;
		m->cells[cell].dsnh3 += m->cells[cell].soils[l].nh3;
		//m->cells[cell].day_dsurea += urea;
		m->cells[cell].ds_doc += m->cells[cell].soils[l].doc;
	}
	//calculate the total NO3, NH4, NH3 in 10 - 20cm
	for ( l = (int)(0.1 / g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY])+1; l < (int)(0.2 / g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY]); l++ )
	{
		m->cells[cell].dsno3b += m->cells[cell].soils[l].no3;
		m->cells[cell].dsnh4b += m->cells[cell].soils[l].nh4;
	}
	//calculate the total NO3, NH4, NH3 in 20 - 30cm
	for ( l = (int)(0.2 / g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY])+1; l < (int)(0.3 / g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY]); l++ )
	{
		m->cells[cell].dsno3c += m->cells[cell].soils[l].no3;
		m->cells[cell].dsnh4c += m->cells[cell].soils[l].nh4;
	}
	//calculate the total NO3, NH4, NH3 in 30 - 40cm
	for ( l = (int)(0.3 / g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY])+1; l <(int)(0.4 / g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY]); l++ )
	{
		m->cells[cell].dsno3d += m->cells[cell].soils[l].no3;
		m->cells[cell].dsnh4d += m->cells[cell].soils[l].nh4;
	}
	//calculate the total NO3, NH4, NH3 in 40 - 50cm
	for ( l = (int)(0.4 / g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY])+1; l < (int)(0.5 / g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY]); l++ )
	{
		m->cells[cell].dsno3e += m->cells[cell].soils[l].no3;
		m->cells[cell].dsnh4e += m->cells[cell].soils[l].nh4;
	}
	//calculate the total NO3, NH4, NH3 in 50+cm
	for ( l = (int)(0.5 / g_soil->values[SOIL_HYDRAULIC_CONDUCTIVITY])+1; l < m->cells[cell].soils_count; l++ )
	{
		m->cells[cell].dsno3f += m->cells[cell].soils[l].no3;
		m->cells[cell].dsnh4f += m->cells[cell].soils[l].nh4;
	}
			 */
		}
	}
}

void soilCEC(CELL *const c)
{
	int i;
	for (i = 0; i < c->soils_count; i++ )
	{
		//check it was clay
		double xx = g_soil->values[SOIL_CLAY_PERC] /100 * 100.0;// + (double)exp(soc[i]/m*1000.0-20.0);
		//if(xx>100.0) xx=100.0;
		c->soils[i].CEC= 1.0802 * xx + 14.442;	//meq/100 g soil
		c->soils[i].CEC =c->soils[i].CEC * 14.0 / 100000.0 *  c->bulk_density;	 //assumed m as bulk density//meq/100 g soil -> kg N/ha/layer
		//CEC[i] = CEC[i] * 0.001; //fraction used for NH4
	}
	c->base_clay_N = 0.01 *c->soils[0].CEC;
	c->max_clay_N = 2.0 * c->base_clay_N;
}


void get_av_year_temperature(CELL *const c, int years, int month, int day, const MET_DATA *const met )
{
	double tempSoilTemp;
	double monthDays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

	c->temp_avet = 0;
	//get permission to include it in main line 1200, getting rid of the following double loop
	for (month = 0; month < MONTHS; month++)
	{
		for (day = 0; day < monthDays[month]; day++)
		{
			tempSoilTemp = met[month].d[day].tsoil;
			c->temp_avet += tempSoilTemp;
		}
	}
	c->temp_avet /= 365;
}

void soil_temperature(CELL *const c, int years, int month, int day, const MET_DATA *const met )
{
	int l, n, qq;
	double K[120], Z[120], C[120], outQ[120],outAir, Kave, dQ;
	double Org, Min, Vwater, Vmin, Vorg, Vsoil, TM, Csoil, Ksoil, a,dQQ, tempSoilTemp, outBottom;
		double fsl;
		double adjT;
	//double Mleaf;



	qq =c->soils_count;//q;//(int)(0.3/h);
	//
	//	Mleaf = (Grain_Wt[1] + Leaf_Wt[1] + Stem_Wt[1])/.58/1000.0;//ton try matter/ha
	//
	//	bcv1 = Mleaf / (Mleaf + (double)exp(7.563 - .0001297 * Mleaf));
	//	bcv2 = (snow_pack * 100.0 / (snow_pack * 100.0 + (double)exp(2.303 - .2197 * snow_pack * 100.0)));

	//c->albedo = albedo(bcv1, bcv2);

	if ( years == 0 &&day == 0 && month == JANUARY ) c->previousSoilT = met[month].d[day].tavg;

	adjT = (met[month].d[day].tavg+met[month].d[day].tmax)*0.5;
	//double adjT = air_temp;



	//adjT = adjT / (double)pow((1.0+lai),0.2); //albedo * OldT + (1.0 - albedo) * adjT;
	//adjT = albedo * OldT + (1.0 - albedo) * adjT;

	if(c->snow_pack==0.0)//&&surface_litter<=2000.0)
	{
		fsl = (c->soils[0].rcvl + c->soils[0].rcvl + c->soils[0].rcvl)/1000.0;
		//fsl = c->litter / 1000.0;//-0.1097*(double)log(surface_litter+0.0000001) + 1.3143;
		fsl = Minimum(0.5, Maximum(0.0, fsl));

		c->soilSurfaceT = c->previousSoilT * fsl + adjT * (1.0 - fsl);//min_temp * (1.0 - fsl);//
		//Surf_Temp = min_temp;//OldT * fsl + min_temp * (1.0 - fsl);
	}
	else
	{
		double SnowDepth = c->snow_pack*2000.0; //cm, snow pack thicknes
		//Surf_Temp = 0.0 + adjT / (10.0 + SnowDepth);//
		//Surf_Temp = max(0.0, Surf_Temp);
		c->soilSurfaceT = adjT / (1.0 + SnowDepth);//

	}

	c->previousSoilT = c->soilSurfaceT;
	c->soils[0].soilTemp = c->soilSurfaceT;

}


