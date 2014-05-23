#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "types.h"
#include "constants.h"

void soil_initialization(CELL *const c)
{
	double litterSOC, CRB, Thc;
	int l;
	double srh = .16;
	//c->soils_count = 1;
	for (l =0; l < c->soils_count; l++)
	{
		//taken from Chiti 2010,
		//ratios taken from DNDC default (userGuide)
		//			Soil_porosity            0.451000
		//actually, it should be site->inSOC * site->humuFract * site->sd * 1000 * 10000 * Maximum(0, Minimum(0.01 / site->porosity, site->soil_depth));
//		c->soils[l].dphum = site->inSOC * site->humuFract * site->bulk_dens * 1000 * 10000 * site->soil_depth;
//		c->soils[l].initialOrganicC = site->inSOC * site->humaFract * site->bulk_dens * 1000 * 10000 * site->soil_depth;
//		litterSOC = site->inSOC * site->litFract * site->bulk_dens * 1000 * 10000 * site->soil_depth;
//		c->soils[l].inert_C = site->inSOC * site->humaFract * site->bulk_dens * 1000 * 10000 * site->soil_depth;
		c->soil_ph = 5.6;

		//gC/m-2 profile-1
//		c->soils[l].dphum = site->inSOC * site->humuFract * 1000; // * site->soil_depth;
//		c->soils[l].initialOrganicC = site->inSOC * site->humaFract * 1000; // * site->soil_depth;
		litterSOC = site->inSOC * site->litFract; // * site->soil_depth;
		c->soils[l].initialOrganicC = site->inSOC - litterSOC;
//		c->soils[l].inert_C = site->inSOC * site->humaFract * 1000; // * site->soil_depth;
		CRB = RBO * c->soils[l].initialOrganicC;
		c->soils[l].CRB1 = CRB * SRB;
		c->soils[l].CRB2 = CRB * (1.0 - SRB);
		Thc = c->soils[l].initialOrganicC - CRB;
		c->soils[l].crhl = Thc * srh;
		c->soils[l].crhr = Thc - c->soils[l].crhl;
		c->soils[l].soilDepth = site->soil_depth;
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



		c->soils[l].soc = site->inSOC; // ???
		c->soils[l].rcvl = litterSOC *.005;
		c->soils[l].rcr= litterSOC *.045;
		c->soils[l].rcl= litterSOC *.95;
//		c->soils[l].crhl = c->soils[l].initialOrganicC;	//interpreted
//		c->soils[l].crhr = c->soils[l].dphum;	//interpreted
		c->soils[l].clay_nh4 = site->soil_nh4 * 0.3 * site->bulk_dens ;
		c->soils[l].nh4 = site->soil_nh4 * 0.7 * 10*site->bulk_dens ;
		c->soils[l].no3 = site->soil_no3 *10*site->bulk_dens ; //bulk_dens as mass
		c->litter = litterSOC;
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
	//site->hydraulicConductivity = 0.063; // tab 5. Katie Price 2010
	//	c->till_fact;
	//	c->tilq;
	//	Soil_NO3(-)(mgN/kg)       0.50000
	//	Soil_NH4(+)(mgN/kg)       0.05000
}


void tree_leaves_fall(MATRIX *const m, int const cell)
{
	int height;
	int soil;
	float  RR1, RR2, RR3;
	float param1, param2, param3, param4, param5, param6;
	//dInertC = 0.0;
	float AddN;
	//float leaf_falln, leaf_fallc;
	float AddC1, AddC2, AddC3 = 0; //AddC, AddCN;
	double leafFall, leaf_falln, day_nup;
	int   l, age,species,bVeg;
	AddN = m->cells[cell].AddC / m->cells[cell].AddCN;
	for (height = m->cells[cell].heights_count -1; height >=0; height --)
	{
		for ( age = m->cells[cell].heights[height].ages_count - 1 ; age >= 0 ; age-- )
		{
			for (species = 0; species < m->cells[cell].heights[height].ages[age].species_count; species++)
			{
				if (m->cells[cell].heights[height].ages[age].species[species].counter[VEG_UNVEG] ==1)
				{
					//vegetative period: the boolean variable is set to 1 (true) and it is sufficient to considered the top soil as not bare (exiting the nested for loop)
					day_nup += m->cells[cell].heights[height].ages[age].species[species].value[VEG_UNVEG];
					leafFall +=m->cells[cell].leafLittering + m->cells[cell].fineRootLittering+m->cells[cell].coarseRootLittering+m->cells[cell].stemLittering;
					bVeg = 1;
					break;
				}else
					// unvegetative period; in this case the nested loop goes on, to check if there is an height, age or species class in vegetative period
				{
					//in case of a bare topspoil, the maximum factor is divided by 1.8 to represent the reduced evapotranspiration from bare soil
				}
			}if(bVeg ==1) break;
		}if(bVeg==1) break;
	}
	//if ( crop[1] == 17 )

	//	leaf_falln = day_nup[1] * leaf[1];
	//	leaf_fallc = leaf_falln * leafcn[1];
	//    AddC = leaf_fallc;
	//	AddCN = leafcn[1];

	leaf_falln = day_nup * leafFall;
	/* #22-1, Add new Litter into soil litter pools */
	if ( m->cells[cell].AddCN >= site->rcnrvl && m->cells[cell].AddCN < site->rcnrl )
	{
		RR1 = site->rcnrvl;
		RR2 = site->rcnrl;
		m->cells[cell].AddC2 = (AddN - m->cells[cell].AddC/RR1) / (1.0/RR2 - 1.0/RR1);
		m->cells[cell].AddC1 = (m->cells[cell].AddC - m->cells[cell].AddC2);
		m->cells[cell].AddC3 = 0.0;

	}
	if ( m->cells[cell].AddCN >= site->rcnrl && m->cells[cell].AddCN <= site->rcnrr )
	{
		RR2 = site->rcnrl;
		RR3 = site->rcnrr;
		param3 = (1.0 / m->cells[cell].AddCN);
		param4 = (1.0 / RR2);
		param1 = param3 - param4;
		param5 = (1.0 / RR3);
		param6 = (1.0 / RR2);
		param2 = param5 - param6;
		m->cells[cell].AddC3 = (float)(m->cells[cell].AddC * (param1 / param2));
		m->cells[cell].AddC2 = (float)(m->cells[cell].AddC - m->cells[cell].AddC3);
		m->cells[cell].AddC1 = (float)0.0;
	}
	if ( m->cells[cell].AddCN < site->rcnrvl )
	{
		float ActN = m->cells[cell].AddC/site->rcnrvl;
		m->cells[cell].AddC3 = 0.0;
		m->cells[cell].AddC2 = 0.0;
		m->cells[cell].AddC1 = m->cells[cell].AddC;
		//c->soils[0].nh4 += (c->AddC / (c->AddCN+0.0000001) - ActN);	//fixSergio [0] or better a for each layer?
		for(soil = 0; soil < m->cells[cell].soils_count; soil ++) {m->cells[cell].soils[soil].nh4 += (m->cells[cell].AddC / (m->cells[cell].AddCN+0.0000001) - ActN);}
		m->cells[cell].AddCN = site->rcnrvl;
	}
	if ( m->cells[cell].AddCN > site->rcnrr )
	{
		//float ActC = AddC/ AddCN * rcnrr;
		m->cells[cell].AddC3 = m->cells[cell].AddC;//ActC;
		m->cells[cell].AddC2 = 0.0;
		m->cells[cell].AddC1 = 0.0;

	}

	if (m->cells[cell].AddC1 < 0.0) m->cells[cell].AddC1 = 0.0;
	if (m->cells[cell].AddC2 < 0.0) m->cells[cell].AddC2 = 0.0;
	if (m->cells[cell].AddC3 < 0.0) m->cells[cell].AddC3 = 0.0;
	for ( l = 0; l <= 1; l++ )
	{
		m->cells[cell].soils[l].rcvl += (AddC1 * 0.5);
		m->cells[cell].soils[l].rcl += (AddC2 * 0.5);
		m->cells[cell].soils[l].rcr += (AddC3 * 0.5);
	}

	//		yr_addleafc = yr_addleafc + AddC1 +
	//				AddC2 + AddC3;
	//		yr_addleafn = yr_addleafn +
	//				AddC1 / rcnrvl +
	//				AddC2 / rcnrl +
	//				AddC3 / rcnrr;
	AddC1 = 0.0;
	AddC2 = 0.0;
	AddC3 = 0.0;

	/* leaf_falln = 0;
	leaf_fallc = 0; */
}


void get_av_year_temperature(CELL *const c, int years, int month, int day, const MET_DATA *const met )
{
	double tempSoilTemp;
	c->temp_avet = 0;
	//get permission to include it in main line 1200, getting rid of the following double loop
	for (month = 0; month < MONTHS; month++)
	{
		for (day = 0; day < 365; day++)
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
	double K[120], Z[120], C[120], outQ[120], Kave, dQ;
	double Org, Min, Vwater, Vmin, Vorg, Vsoil, TM, Csoil, Ksoil, a,dQQ, tempSoilTemp;
	//double Mleaf;



	qq =c->soils_count;//q;//(int)(0.3/h);
	//
	//	Mleaf = (Grain_Wt[1] + Leaf_Wt[1] + Stem_Wt[1])/.58/1000.0;//ton try matter/ha
	//
	//	bcv1 = Mleaf / (Mleaf + (double)exp(7.563 - .0001297 * Mleaf));
	//	bcv2 = (snow_pack * 100.0 / (snow_pack * 100.0 + (double)exp(2.303 - .2197 * snow_pack * 100.0)));

	//c->albedo = albedo(bcv1, bcv2);

	if ( years == 0 &&day == 0 && month == JANUARY ) c->previousSoilT = met[month].d[day].tavg;

	double adjT = (met[month].d[day].tavg+met[month].d[day].tmax)*0.5;
	//double adjT = air_temp;

	double fsl;

	//adjT = adjT / (double)pow((1.0+lai),0.2); //albedo * OldT + (1.0 - albedo) * adjT;
	//adjT = albedo * OldT + (1.0 - albedo) * adjT;

	if(c->snow==0.0)//&&surface_litter<=2000.0)
	{
		fsl = c->litter / 1000.0;//-0.1097*(double)log(surface_litter+0.0000001) + 1.3143;
		fsl = Minimum(0.5, Maximum(0.0, fsl));

		c->soilSurfaceT = c->previousSoilT * fsl + adjT * (1.0 - fsl);//min_temp * (1.0 - fsl);//
		//Surf_Temp = min_temp;//OldT * fsl + min_temp * (1.0 - fsl);
	}
	else
	{
		double SnowDepth = c->snow*2000.0; //cm, snow pack thicknes
		//Surf_Temp = 0.0 + adjT / (10.0 + SnowDepth);//
		//Surf_Temp = max(0.0, Surf_Temp);
		c->soilSurfaceT = adjT / (1.0 + SnowDepth);//

	}

	c->previousSoilT = c->soilSurfaceT;

	//Volumatric heat capacity, J/(layer*K), and conductivity
	for (l=0; l<qq; l++)
	{
		/*
    if(temp[l]<=0.0 && air_temp> 0.0 && rainfall>0.0 )
    {
       int rain_layer = (int)(rainfall/0.005);
       if(l<=rain_layer) temp[l] = air_temp;
    }
		 */
		c->soils[l].waterContent = c->water_to_soil;
		Org = c->soils[l].rcvl + c->soils[l].rcl+ c->soils[l].rcr + c->soils[l].CRB1 +
				c->soils[l].CRB2 + c->soils[l].crhl+c->soils[l].crhr + c->soils[l].dphum; //kg C/layer/ha

		Min = c->soils[l].mmm;
		Vwater = c->soils[l].waterContent * 10000.0; //m water/layer -> m3 water/layer/ha

		Vmin = Min / (1000 * site->bulk_dens); //volume of mineral matter, m3/layer/ha
		Vorg = Org / 0.58 / (1000 * 0.4); //volume of organic matter, m3/layer/ha
		Vsoil = Vmin + Vorg; //volume of soil, m3/layer/ha

		TM = Org + Min;

		Csoil = 4.0 * (3450000.0 * Vorg / Vsoil + 2350000.0 * Vmin / Vsoil); //J/(m3*K)
		//if(st[1]>=12) Csoil *= 2.0;

		C[l] = 0.5 * (Vsoil * Csoil + Vwater * 4180000.0); // J/(layer*K)


		Ksoil = (0.0025 * Org / TM + 0.029 * Min / TM) * 10.0;

		K[l] = 86400.0 * ((1.0 - c->soils[l].sts) * Ksoil + c->soils[l].sts * c->soils[l].soilMoisture * 0.0057);//
		//J/(cm*day*K)

		//		//Depth of layer, cm
		//		//Z[l] = 100.0 * l * h[l]; // cm
		//		if(l==0) Z[l] = site->hydraulicConductivity * 100.0;
		//		else Z[l] = Z[l-1] + site->hydraulicConductivity* 100.0;

	}

	//Heat flux from layer L-1 down to L
	for (n=0; n<200; n++)
	{
		for (l=0; l<qq; l++)
		{
			a = 5000.0; //10000
			if (l==0)
			{
				double F_snow = 100.0 * c->snow;
				if (F_snow < 1.0) F_snow = 1.0;

				//heat transfer between air and first layer

				if(c->snow > 0.0)
					outQ[l-1] = a * K[l] * (c->soilSurfaceT - c->soils[l].soilTemp) / Z[l] / F_snow;//J/(cm2*day)
				else
					outQ[l-1] = a * K[l] * (c->soilSurfaceT - c->soils[l].soilTemp) / Z[l];
			}

			if (l<qq)
			{
				Kave = (K[l] + K[l+1]) / 2.0;
				//heat from layer l to l+1
				outQ[l] = a * Kave * (c->soils[l].soilTemp - c->soils[l +1].soilTemp) / (Z[l+1] - Z[l]);
			}
			else
			{
				//heat from bottom layer to deeper soil
				outQ[l] = a * K[l] * (c->soils[l].soilTemp - c->yr_avet) / (200.0 - Z[l]);	//yr_ave_tj
			}

			dQ = outQ[l-1] - outQ[l];

			double dT = dQ / C[l];

			c->soils[l].soilTemp += dT;

		} // l loop

	} // n

	for(l=0; l<qq; l++)
	{
		double pQ1, pQ2, d_ice=0;
		double ps = site->hydraulicConductivity*c->soils[l].sts;
		double max_water = ps * 10000.0;//ps

		Vwater =c->soils[l].waterContent * 10000.0;  // m water -> m3 water/layer/ha

		if (c->soils[l].soilTemp<=0.0) //freeze
		{
			if(Vwater>0.0&& c->soils[l].ice/10000.0<ps)
			{
				pQ1 = -c->soils[l].soilTemp * C[l]; //J/layer, heat release when T->0
				pQ2 = Vwater * (4100000.0 - 2100000.0); //J/layer, water heat capacity

				if (pQ1 > pQ2) //totally freeze
				{
					dQQ = pQ2; //J/layer/ha
					c->soils[l].soilTemp += (dQQ / C[l]);
					d_ice = Vwater; //m3 water/lay/ha
					Vwater = 0.0;

					c->soils[l].ice += d_ice;//m3 water/lay/ha
					c->soils[l].waterContent = 0.0; //Vwater / 10000.0; // m water/layer

					//day_ice += d_ice/10000.0;
					//if(day_ice<0.0) day_ice = 0.0;

				}
				else //partially freeze
				{
					dQQ = pQ1;
					c->soils[l].soilTemp = 0.0;
					d_ice = dQQ / (4100000.0 - 2100000.0);

					if(d_ice > Vwater)
						d_ice = Vwater;//m3 water/lay/ha

					Vwater -= d_ice;

					c->soils[l].ice += d_ice; //m3 water/lay/ha

					c->soils[l].waterContent -= (d_ice / 10000.0); // m water/layer
					//day_ice += d_ice/10000.0;// m water/layer
					//if(day_ice<0.0) day_ice = 0.0;
				}

				if(c->soils[l].ice/10000.0 > ps) c->soils[l].ice = ps * 10000.0;
			}
			else
			{
				d_ice = 0.0;
			}

		}
		else  //thawing
		{
			if (c->soils[l].ice>0.0&&c->soils[l].soilTemp>0.0)
			{
				pQ1 = c->soils[l].soilTemp * C[l] * 0.05;//0.1;//0.05;
				//pQ2 = 2.0 * ice[l] * (4100000.0 - 2100000.0);
				pQ2 = c->soils[l].ice * (4100000.0 - 2100000.0);

				if (pQ1 > pQ2) //totally thawing
				{
					dQQ = pQ2;
					c->soils[l].soilTemp -= dQQ / C[l];
					d_ice = c->soils[l].ice;

					Vwater += d_ice;
					c->soils[l].ice = 0.0;
					c->soils[l].waterContent += d_ice / 10000.0; // m water/layer

					//day_ice -= d_ice/10000.0;
					//if(day_ice<0.0) day_ice = 0.0;
				}
				else //partially thawing
				{
					dQQ = pQ1;
					c->soils[l].soilTemp = 0.0;
					//d_ice = -0.5 * pQ1 / (4100000.0 - 2100000.0);
					d_ice = pQ1 / (4100000.0 - 2100000.0);
					if(d_ice > c->soils[l].ice) d_ice = c->soils[l].ice;
					Vwater += d_ice;
					c->soils[l].ice -= d_ice;// m3 water/layer/ha

					c->soils[l].waterContent += d_ice / 10000.0; // m water/layer
					//day_ice -= d_ice/10000.0;
					//if(day_ice<0.0) day_ice = 0.0;
				}
				if(c->soils[l].ice>max_water)
					c->soils[l].ice= max_water;
			}
			else
			{
				d_ice = 0.0;
			}

		}

		if (c->soils[l].ice < 0.0) c->soils[l].ice = 0.0;

		if (c->soils[l].waterContent < 0.0) c->soils[l].waterContent  = 0.0;

		//c->total_ice += (c->soils[l].ice/10000.0); //m water for profile

	}//layer loop
}

//double ttn;

void soil_dndc_sgm(MATRIX *const m, const YOS *const yos, const int years, const int month, const int day, const int years_of_simulation)
{
	MET_DATA *met;
	double  clayc, krh, hrh, DDRF;
	double drcvl,DRCB,lit_co2, DRCB1, DRCB2, sumn, fb_nh4, fb_co2, fh_nh4, fh_co2;
	int    cell, l,ll, vv;
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
			tree_leaves_fall(m, cell);
		}
		if (day == 0 && month == JANUARY)
		{
			//reinitialize data
			//get_av_year_temperature(&m->cells[cell], years, month, day, met);
		}
		//soil_temperature(&m->cells[cell], years, month, day, met);

		//effect of clay adsorption: according to Zhang no 2.3026
		clayc = (double)(log(.14 / (site->clay_perc/100)) / 2.3026 + 1);
		krh = (double).16 * clayc;
		hrh = (double).006 * clayc;

		//limiting factors f(clay%)
		if(krh>1.0) krh=1.0;
		if(hrh>1.0) hrh=1.0;
		for ( l = 0; l < m->cells[cell].soils_count;  l++ )
		{
			Fl=1.0;
			Fl = 0.6 * (double)pow(l, -0.1366);
			if(Fl>1.3) Fl = 1.3;
			if(Fl<0.0) Fl = 0.0;

			m->cells[cell].soils[l].soilTemp = met[month].d[day].tsoil;
			//DRF is defined as a constant (0.03)
			//this considered the imapct of soil clay and microbio
			//activity in each layer

			DDRF = (DRF - 0.02 * site->clay_perc/100) * Fl;	   // * pow(m->cells[cell].MicrobioIndex, 0.1);
			if ( DDRF < 0.0 ) DDRF = 0.0;

			//		if(m->cells[cell].TTT>0.0)		//&& m->cells[cell].soils[l].soil_ph  != m->cells[cell].soils[l].sph)
			//		{
			//
			//			Frain = 0.5; //20.0 * rainfall;
			//			if(Frain>1.0) Frain=1.0;
			//
			//			delta_ph = 6.7 * (met[month].d[day].rain / 0.05) + m->cells[cell].soil_ph * 1000.0;
			//			delta_ph = 6.7 * Frain + m->cells[cell].soil_ph * 1000.0;
			//			delta_ph /= (1000.0+Frain);
			//
			//			dev_ph = (m->cells[cell].soils[l].sph - m->cells[cell].soil_ph) * Frain;
			//
			//			m->cells[cell].soils[l].sph -= dev_ph;
			//		}
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
			//RFM=Minimum(1.0, Maximum(0.0, RFM));

			if(m->cells[cell].soils[l].ice >0.0) RFM = (0.9 - m->cells[cell].soils[l].soilMoisture) * 0.05;//0.015;//
			RFM=Minimum(1.0, Maximum(0.0, RFM));

			// Redefine daily microbes/humads
			m->cells[cell].soils[l].soc = m->cells[cell].soils[l].rcvl + m->cells[cell].soils[l].rcl + m->cells[cell].soils[l].rcr + m->cells[cell].soils[l].CRB1 +
					m->cells[cell].soils[l].CRB2 + m->cells[cell].soils[l].crhl +m->cells[cell].soils[l].crhr + m->cells[cell].soils[l].dphum;

			if(day == 0 && month == 0 && years == 0)
			{
				ActSOC = m->cells[cell].soils[l].CRB1 + m->cells[cell].soils[l].CRB2 + m->cells[cell].soils[l].crhl + m->cells[cell].soils[l].crhr;
				if(ActSOC<=0.0) Fhc = 0.0;
				else Fhc = m->cells[cell].soils[l].crhl / (m->cells[cell].soils[l].crhl + m->cells[cell].soils[l].crhr);
				CRB = 0.01 * ActSOC;
				m->cells[cell].soils[l].CRB1 = CRB * SRB;
				m->cells[cell].soils[l].CRB2 = CRB * (1.0 - SRB);
				Thc = ActSOC - CRB;
				m->cells[cell].soils[l].crhl = Thc * Fhc;
				m->cells[cell].soils[l].crhr = Thc - m->cells[cell].soils[l].crhl;
				new_ActSOC = m->cells[cell].soils[l].CRB1 + m->cells[cell].soils[l].CRB2 + m->cells[cell].soils[l].crhl +
						m->cells[cell].soils[l].crhr;
				if(new_ActSOC!=ActSOC)
				{
					dActSOC = new_ActSOC - ActSOC;
					m->cells[cell].soils[l].crhr -= dActSOC;
				}
			}

			m->cells[cell].soils[l].no3 += m->cells[cell].soils[l].no2;
			m->cells[cell].soils[l].no2 = 0.0;
			// Decomposition of surface litter
			if ( l ==0)
			{
				if(m->cells[cell].litter >0.0&& m->cells[cell].soils[l].soilTemp>0.0)
				{
					Ftw=  m->cells[cell].soils[l].soilTemp * 0.001;//0.005

					//Ftw = temp[1] / 40.0 * day_wfps[1];
					Ftw = Maximum(0.0, Minimum(Ftw, 0.5));

					dlitter = m->cells[cell].litter * Ftw;
					m->cells[cell].soils[l].co2 += dlitter;
					m->cells[cell].soils[l].day_O2 += dlitter;
					if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
					m->cells[cell].soils[l].nh4 += (dlitter / site->rcnrvl);
					m->cells[cell].litter -= dlitter;
					if ( m->cells[cell].litter <= 0.0000001 ) m->cells[cell].litter = 0.0;
				}
			}

			// Decomposition of very labile litter
			if (l>=m->cells[cell].tilq) f_till_fact = 1.0;// + 3
			else  f_till_fact = m->cells[cell].till_fact;

			/* potential CO2 from rcvl decomposition */
			r_RFM = Maximum(0.0, Minimum(1.0, RFM));//max(0.7, min(1.0, RFM));
			Frcvl = 2.0 * r_RFM * KRCVL * DDRF * site->DClitter * f_till_fact;//* f_till_fact
			if(Frcvl>1.0)
				Frcvl=1.0;

			m->cells[cell].soils[l].drcvl = Frcvl * m->cells[cell].soils[l].rcvl;
			// microbes formed from rcvl decomposition
			m->cells[cell].soils[l].DRCB1 = (m->cells[cell].soils[l].drcvl) * EFFRB;
			sum_drcvl = (m->cells[cell].soils[l].drcvl) + (m->cells[cell].soils[l].DRCB1);
			m->cells[cell].soils[l].rcvl -= sum_drcvl;
			m->cells[cell].soils[l].nh4+= (sum_drcvl / site->rcnrvl);
			m->cells[cell].soils[l].co2+= m->cells[cell].soils[l].drcvl;
			m->cells[cell].soils[l].day_O2+= m->cells[cell].soils[l].drcvl;

			if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
			m->cells[cell].soils[l].CRB1 += ((m->cells[cell].soils[l].DRCB1) * SRB);
			m->cells[cell].soils[l].CRB2+= ((m->cells[cell].soils[l].DRCB1) * (1.0 - SRB));
			m->cells[cell].soils[l].nh4 -= (m->cells[cell].soils[l].DRCB1 / site->rcnb);

			// N demand for microbes formation
			//DRNB1 = (*DRCB1) / rcnb;
			// suplas N
			//FreeN1 = sum_drcvl / rcnrvl - DRNB1;

			m->cells[cell].day_C_mine += m->cells[cell].soils[l].drcvl;
			m->cells[cell].day_N_mine += m->cells[cell].soils[l].drcvl/site->rcnrvl;//1(sum_drcvl / rcnrvl);
			m->cells[cell].day_N_assim += (m->cells[cell].soils[l].DRCB1 / site->rcnb);

			//----------------------------------------------------------------------------
			// Decomposition of labile litter
			//if ( l > (tilq + 3) ) f_till_fact = 1.0;
			if (l>=m->cells[cell].tilq) f_till_fact = 1.0;// + 3
			else  f_till_fact = m->cells[cell].till_fact;

			/* potential CO2 from rcl decomposition */
			Frcl = 2.0*RFM * KRCL * DDRF * site->DClitter * f_till_fact;//* f_till_fact
			if(Frcl>1.0)
				Frcl=1.0;

			p_drcl = m->cells[cell].soils[l].rcl * Frcl;

			/* potential microbe formation from rcl decomposition */
			p_DRCB2 = p_drcl * EFFRB;

			/* potential total rcl decomposition */
			total_drcl = p_drcl + p_DRCB2;

			/* N release from potential total rcl decomposition */
			PTAN = total_drcl / site->rcnrl;

			/* potential N demand for microbial formation */
			PDN = p_DRCB2 / site->rcnb;

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
			m->cells[cell].day_N_mine += m->cells[cell].soils[l].litco22 / site->rcnrl;//2(soils[l].litco22+*DRCB2) / rcnrl; //gross N mineralization
			m->cells[cell].day_N_assim += m->cells[cell].soils[l].DRCB2 / site->rcnb;//N assimilation

			if(l<=(int)(0.15/site->hydraulicConductivity))
			{
				Ftill=0.01;
				FungiLitterDecomposition = m->cells[cell].soils[l].rcl * Ftill * (RFM);
				FungiN = FungiLitterDecomposition / site->rcnrl;
				FungiMicrobe = FungiN * site->rcnb;
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
			TN1 = m->cells[cell].soils[l].rcr/site->rcnrr + (m->cells[cell].soils[l].CRB1+m->cells[cell].soils[l].CRB2)/site->rcnb + m->cells[cell].soils[l].no3 +m->cells[cell].soils[l].nh4 ;
			if (l>=m->cells[cell].tilq) f_till_fact = 1.0;// + 3
			else  f_till_fact = m->cells[cell].till_fact;

			/* potential CO2 from rcr decomposition */
			Frcr = 2.0*RFM * KRCR * DDRF * site->DClitter * f_till_fact;//

			if(Frcr>1.0) Frcr=1.0;
			//sergio if(l==q+1) Frcr *= 1000.0;
			Frcr = Minimum(1.0, Maximum(0.0, Frcr));

			p_drcr = Frcr *m->cells[cell].soils[l].rcr ;

			/* potential microbe formation from rcr decomposition */
			p_DRCB3 = p_drcr * EFFRB;

			/* potential total rcr decomposition */
			total_drcr = p_drcr + p_DRCB3;

			/* N release from potential total rcr decomposition */
			PTAN = total_drcr / site->rcnrr;

			/* potential N demand for microbe formation */
			PDN = p_DRCB3 / site->rcnb;

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

			TN2 = m->cells[cell].soils[l].rcr /site->rcnrr +
					(m->cells[cell].soils[l].CRB1 +m->cells[cell].soils[l].CRB2 )/site->rcnb +
					m->cells[cell].soils[l].no3 +m->cells[cell].soils[l].nh4 ;

			dTN = Maximum(TN2 - TN1, 0);

			m->cells[cell].soils[l].nh4  -= dTN;
			if(m->cells[cell].soils[l].nh4 <0.0) m->cells[cell].soils[l].nh4  = 0.0;

			m->cells[cell].day_C_mine += m->cells[cell].soils[l].litco23;//net C release
			m->cells[cell].day_N_mine += m->cells[cell].soils[l].litco23 / site->rcnrr;//3(soils[l].litco23+DRCB3) / rcnrl; //gross N mineralization
			m->cells[cell].day_N_assim += DRCB3 / site->rcnb;//N assimilation

			drcra = p_drcr;

			lit_co2 = drcvl + m->cells[cell].soils[l].litco22 + m->cells[cell].soils[l].litco23;//!!!

			sumn = drcvl / site->rcnrvl + m->cells[cell].soils[l].litco22 / site->rcnrl + drcra / site->rcnrr;   /* not accurate */

			m->cells[cell].soils[l].litco22 = 0.0;
			drcvl = 0.0;
			m->cells[cell].soils[l].litco23 = 0.0;

			if(l<=(int)(0.15/site->hydraulicConductivity))
			{

				Ftill2=0.0001;//0.0005
				FungiLitterDecomposition2 = m->cells[cell].soils[l].rcr  * Ftill * (RFM);
				FungiN2 = FungiLitterDecomposition2 / site->rcnrr;
				FungiMicrobe2 = FungiN2 * site->rcnb;
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
			if (l>=m->cells[cell].tilq) f_till_fact = 1.0;// + 3
			else  f_till_fact = m->cells[cell].till_fact;

			TN1 = (m->cells[cell].soils[l].CRB1+m->cells[cell].soils[l].CRB2)/site->rcnb +
					m->cells[cell].soils[l].nh4 + m->cells[cell].soils[l].crhr/site->rcnh2;

			/* microbe decomposition */
			DCRB1 = RFM * KRB * DDRF * f_till_fact * m->cells[cell].soils[l].CRB1 * Fo * site->DChumads;
			DCRB2 = RFM * HRB * DDRF * f_till_fact * m->cells[cell].soils[l].CRB2 * Fo * site->DChumads;
			DCRB = DCRB1 + DCRB2;

			m->cells[cell].soils[l].CRB1 -= (DCRB1);
			m->cells[cell].soils[l].CRB2 -= (DCRB2);

			m->cells[cell].soils[l].nh4 += (DCRB / site->rcnb);

			/* efficiency */
			FBBC = DCRB * EFFAC;	//    EFFAC = 0.2;

			/* new microbe */
			m->cells[cell].soils[l].CRB1 += (FBBC * SRB);
			m->cells[cell].soils[l].CRB2 += (FBBC * (1.0 - SRB));

			m->cells[cell].soils[l].nh4 -= (FBBC / site->rcnb);
			m->cells[cell].day_N_assim += (FBBC / site->rcnb);

			m->cells[cell].soils[l].dcbavai = FBBC;

			/* new resistant humads from microbe decomposition */
			FBHC = DCRB * EFFNO;
			m->cells[cell].soils[l].crhr += FBHC;

			m->cells[cell].soils[l].nh4 -= (FBHC / site->rcnh2);
			m->cells[cell].day_N_assim += (FBHC / site->rcnh);


			/* CO2 from net microbe decomposition */
			fb_co2 = DCRB - FBBC - FBHC;
			m->cells[cell].soils[l].co2 += fb_co2;
			m->cells[cell].soils[l].day_O2 += fb_co2;
			if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;
			m->cells[cell].day_C_mine += fb_co2;
			m->cells[cell].day_N_mine += fb_co2/ site->rcnb;//4

			/* NH4 from net microbe decomposition */
			fb_nh4 = (fb_co2) / site->rcnb;


			TN2 = (m->cells[cell].soils[l].CRB1+m->cells[cell].soils[l].CRB2)/site->rcnb +
					m->cells[cell].soils[l].nh4 + m->cells[cell].soils[l].crhr/site->rcnh2;
			dTN = TN2 - TN1;
			m->cells[cell].soils[l].nh4 -= dTN;
			m->cells[cell].soils[l].nh4 = Maximum(0.0, m->cells[cell].soils[l].nh4);




			// Decomposition of humads (this function has error source)
			//if ( l > (tilq + 3) ) f_till_fact = 1.0;
			if (l>=m->cells[cell].tilq) f_till_fact = 1.0;// + 3
			else  f_till_fact =m->cells[cell].till_fact;

			// humads decomposition */
			Fclay = 0.5 * pow( site->clay_perc /100 , -0.471);

			k1 = 0.8 * RFM * hrh * DDRF * f_till_fact * Fclay * site->DChumads;//1.0
			k2 = 0.8 * RFM * krh * DDRF * f_till_fact * Fclay * site->DChumads;//1.0

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

			TN1 = m->cells[cell].soils[l].crhr /site->rcnh2 + m->cells[cell].soils[l].crhl /
					site->rcnh +m->cells[cell].soils[l].nh4  +
					(m->cells[cell].soils[l].CRB1 +m->cells[cell].soils[l].CRB2 )/site->rcnb +
					m->cells[cell].soils[l].dphum /site->rcnm;//new humads

			m->cells[cell].soils[l].crhr  -= dchr;
			m->cells[cell].soils[l].crhl  -= dchl;

			crhr_co2 = dchr * 0.5;
			m->cells[cell].soils[l].co2  += crhr_co2;
			m->cells[cell].soils[l].day_O2  += crhr_co2;
			if(m->cells[cell].soils[l].day_O2 <0.0) m->cells[cell].soils[l].day_O2 =0.0;
			m->cells[cell].soils[l].nh4  += (crhr_co2 / site->rcnh2);

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
			m->cells[cell].soils[l].nh4  += (crhl_co2 / site->rcnh);
			// efficiency */
			dhbcl = dchl * 0.15;
			// new microbe
			m->cells[cell].soils[l].CRB1  += (dhbcl * SRB);
			m->cells[cell].soils[l].CRB2  += (dhbcl * (1.0 - SRB));
			nhuml = dchl - (dhbcl + crhl_co2);
			m->cells[cell].soils[l].dphum  += nhuml;

			TN2 = m->cells[cell].soils[l].crhr /site->rcnh2 + m->cells[cell].soils[l].crhl /
					site->rcnh + m->cells[cell].soils[l].nh4  + (m->cells[cell].soils[l].CRB1 +
							m->cells[cell].soils[l].CRB2 )/site->rcnb + m->cells[cell].soils[l].dphum /site->rcnm;
			dTN = TN2 - TN1;
			m->cells[cell].soils[l].nh4  -= dTN;

			m->cells[cell].soils[l].nh4  = Maximum(0.0, m->cells[cell].soils[l].nh4 );

			humad_co2 = crhr_co2 + crhl_co2;
			m->cells[cell].day_C_mine += humad_co2;
			m->cells[cell].day_N_mine += (crhl_co2/site->rcnh + crhr_co2/site->rcnh2);

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
//				site->clay_perc= 0.01;
//				//previously soilDepth[l]
//				m->cells[cell].soils[l].soilDepth = 0.01;
//			}

			//if(st[l]<12)
			Fclay = 0.1793 * pow(site->clay_perc /100, -0.471) * 20.0;

			Fclay=Maximum(0.0, Minimum(1.0, Fclay));

			//if(st==12) Fclay *= 0.1;
			//fixSergio being a single layer, assumed the value at depth .5
			//Fdepth = (double)pow(10.0, (-20.0 * m->cells[cell].soils[l].soilDepth) + 1.0);
			Fdepth = (double)pow(10.0, (-20.0 * .5) + 1.0);
			Fdepth=Maximum(0.0, Minimum(1.0, Fdepth));

			Ftemp = m->cells[cell].soils[l].soilTemp / 25.0;
			Ftemp = Maximum(1.0, Minimum(0.001, Ftemp));

			d_factor = 0.00001 * RFM * DDRF * f_till_fact * Fclay * Fdepth * site->DChumus * Ftemp;//0.0008

			if(d_factor < 0.0) d_factor = 0.0;

			d_humus = m->cells[cell].soils[l].dphum* d_factor;

			TN1 = m->cells[cell].soils[l].dphum/site->rcnm + m->cells[cell].soils[l].nh4;

			m->cells[cell].soils[l].dphum -= d_humus;
			if(m->cells[cell].soils[l].dphum < 0.0)
			{
				m->cells[cell].soils[l].dphum = 0.0;
				d_humus = 0.0;
			}

			m->cells[cell].soils[l].nh4 += (d_humus / site->rcnm);
			m->cells[cell].soils[l].co2 += d_humus;
			m->cells[cell].soils[l].day_O2 += d_humus;
			if(m->cells[cell].soils[l].day_O2<0.0) m->cells[cell].soils[l].day_O2=0.0;

			m->cells[cell].day_N_mine += (d_humus / site->rcnm);//6

			m->cells[cell].day_C_mine += d_humus;

			TN2 = m->cells[cell].soils[l].dphum/site->rcnm + m->cells[cell].soils[l].nh4;

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
			hydrogen = (double)pow(10.0, -site->);//mol/L
			hydroxide = Kw / hydrogen;//mol/L

			//suspect its porosity
			V_water = m->cells[cell].soils[l].soilMoisture * site->hydraulicConductivity * 1000.0;//liter water/m in layer l

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
					Fwi = .5;	//sergio debug
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

				lbcn = site->base_clay_N * (double)pow(0.5, (double)l);
				lmcn = site->max_clay_N * (double)pow(0.5, (double)l);

				if(m->cells[cell].soils[l].clay_nh4 > lbcn) active_clay_nh4 = m->cells[cell].soils[l].clay_nh4 - lbcn;
				else active_clay_nh4 = 0.0;

				totalN = m->cells[cell].soils[l].nh4 + active_clay_nh4;

				FIXRATE = 0.5 * (7.2733*pow(site->clay_perc /100, 3.0) - 11.22*pow(site->clay_perc /100, 2.0) + 5.7198*site->clay_perc/100 + 0.0263);//0.99

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
					if(l+1<(int)(0.2/site->hydraulicConductivity))
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
				if(l<=m->cells[cell].soils_count)//&&doc[l]>0.0)
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
				m->cells[cell].wcrh += (m->cells[cell].soils[l].crhl+ m->cells[cell].soils[l].crhr);
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

					wrnvl = m->cells[cell].wrcvl / site->rcnrvl;
					wrnl = m->cells[cell].wrcl / site->rcnrl;
					wrnr =m->cells[cell].wrcr / site->rcnrr;
					wnrb = m->cells[cell].wcrb /site->rcnb;
					wnrh = m->cells[cell].wcrh / site->rcnh;
					whumusn =m->cells[cell].whumus / site->rcnm;

					//todo m->cells[cell].End_SOC = m->cells[cell].wrcvl+m->cells[cell].wrcl+m->cells[cell].wrcr+m->cells[cell].wcrb+m->cells[cell].wcrh+m->cells[cell].whumus;


					//input_C = m->cells[cell].yr_addmc + m->cells[cell].yr_addtc + m->cells[cell].yr_addrc +m->cells[cell].yr_weedc;
					//output_C =m->cells[cell].yr_soil_co2 + m->cells[cell].yr_ch4 + m->cells[cell].yr_leach_hu;

					//delta = (IniSOC + input_C) - (EndSOC + output_C);
					//
					//de = delta / m->cells[cell].soils_count;	//q

					m->cells[cell].End_SON = wrnvl + wrnl + wrnr + wnrb + wnrh + whumusn;// + delta/rcnm;

				}
			}

			if(m->cells[cell].runoff>0.0)
			{
				fw=m->cells[cell].runoff * 1.0;//0.01
				fw = Minimum(0.9, Maximum(0.0, fw));
				m->cells[cell].runoff_N += fw * m->cells[cell].soils[l].no3;
				m->cells[cell].soils[l].no3 *= (1.0 - fw);
			}

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

			/* P adsorption */
			//Langmuir Equation

			for(l=0; ll<m->cells[cell].soils_count; ll++)
			{
				TP = m->cells[cell].soils[ll].LabP + m->cells[cell].soils[ll].AdsP;//kg P/layer

				if(m->cells[cell].soils[ll].LabP<=0.0&& m->cells[cell].soils[ll].AdsP>0.0)
				{
					m->cells[cell].soils[ll].LabP = m->cells[cell].soils[ll].AdsP * 0.001;
					m->cells[cell].soils[ll].AdsP *= 0.999;
				}
				VVw = site->hydraulicConductivity* 10000.0 * m->cells[cell].soils[ll].sts * m->cells[cell].soils[ll].soilMoisture * 1000.0; //water L/ha in layer l
				PinW = (m->cells[cell].soils[ll].LabP * 1000000.0) / VVw;//mg P/L in liquid

				//Langmuir equation
				PinS = adsPmax * (adsPK * PinW) / (1.0 + adsPK * PinW) - adsPo;//mg P/kg soil, 2->100; 50-600 mg P/kg soil

				dLabP = (PinW / 1000000.0) * VVw;//kg P/layer soil pore water;
				dAdsP = PinS / 1000000.0 * m->cells[cell].soils[ll].mmm;//kg P/layer soil
				TTP = dLabP + dAdsP;

				if(TTP<=0.0)
				{
					m->cells[cell].soils[ll].AdsP = 0.0;
					m->cells[cell].soils[ll].LabP = 0.0;
				}
				else
				{
					m->cells[cell].soils[ll].AdsP = TP * (dAdsP / TTP);
					m->cells[cell].soils[ll].LabP = TP - m->cells[cell].soils[ll].AdsP;
				}
			}

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
	for ( l = 0; l <(int)(0.1 / site->hydraulicConductivity); l++ )
	{
		m->cells[cell].dsno3 += m->cells[cell].soils[l].no3;
		m->cells[cell].dsnh4 += m->cells[cell].soils[l].nh4;
		m->cells[cell].dsnh3 += m->cells[cell].soils[l].nh3;
		//m->cells[cell].day_dsurea += urea;
		m->cells[cell].ds_doc += m->cells[cell].soils[l].doc;
	}
	//calculate the total NO3, NH4, NH3 in 10 - 20cm
	for ( l = (int)(0.1 / site->hydraulicConductivity)+1; l < (int)(0.2 / site->hydraulicConductivity); l++ )
	{
		m->cells[cell].dsno3b += m->cells[cell].soils[l].no3;
		m->cells[cell].dsnh4b += m->cells[cell].soils[l].nh4;
	}
	//calculate the total NO3, NH4, NH3 in 20 - 30cm
	for ( l = (int)(0.2 / site->hydraulicConductivity)+1; l < (int)(0.3 / site->hydraulicConductivity); l++ )
	{
		m->cells[cell].dsno3c += m->cells[cell].soils[l].no3;
		m->cells[cell].dsnh4c += m->cells[cell].soils[l].nh4;
	}
	//calculate the total NO3, NH4, NH3 in 30 - 40cm
	for ( l = (int)(0.3 / site->hydraulicConductivity)+1; l <(int)(0.4 / site->hydraulicConductivity); l++ )
	{
		m->cells[cell].dsno3d += m->cells[cell].soils[l].no3;
		m->cells[cell].dsnh4d += m->cells[cell].soils[l].nh4;
	}
	//calculate the total NO3, NH4, NH3 in 40 - 50cm
	for ( l = (int)(0.4 / site->hydraulicConductivity)+1; l < (int)(0.5 / site->hydraulicConductivity); l++ )
	{
		m->cells[cell].dsno3e += m->cells[cell].soils[l].no3;
		m->cells[cell].dsnh4e += m->cells[cell].soils[l].nh4;
	}
	//calculate the total NO3, NH4, NH3 in 50+cm
	for ( l = (int)(0.5 / site->hydraulicConductivity)+1; l < m->cells[cell].soils_count; l++ )
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
	int soil;
	for (soil = 0; soil< c->soils_count; soil++ )
	{
		//check it was clay
		double xx = site->clay_perc /100 * 100.0;// + (double)exp(soc[i]/m*1000.0-20.0);
		//if(xx>100.0) xx=100.0;
		c->soils[soil].CEC= 1.0802 * xx + 14.442;	//meq/100 g soil
		c->soils[soil].CEC =c->soils[soil].CEC * 14.0 / 100000.0 * site->bulk_dens;	 //assumed m as bulk density//meq/100 g soil -> kg N/ha/layer
		//CEC[i] = CEC[i] * 0.001; //fraction used for NH4
	}
	c->base_clay_N = 0.01 *c->soils[0].CEC;
	c->max_clay_N = 2.0 * c->base_clay_N;
}



