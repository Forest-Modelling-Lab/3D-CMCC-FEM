function longwave = longwave_from_metdata(Tair,Rg,Rg_pot,VPD,missval,dt)
% calculates longwave radiation from Vapour Pressure and Air Temp. and Radiation
%-------------------------------------------------------------------------------
% implemented by Sönke Zaehle (szaehle@bgc-jena.mpg.de)
% MPI for Biogeochemistry, Jena Germany
%
% DISCLAIMER:
% This software comes with absolutely no warrenty whatsoever wrt to its performance
% or scientific correctness. Any error associated with results produced from
% this routine is not in the responsibility of the author of this routine
%-------------------------------------------------------------------------------
% Input:
%   Tair Air temperature [°C]
%   Rg Shortwave downward radiation [W m-2]
%   Rg_pot Potential shortwave downward radiation [W m-2]
%   VP  Vapour Pressure [Pa]
% Output:
%   longwave Longwave downward radiation [W m-2]
%-------------------------------------------------------------------------------
% Calculation of Longwave follows the JSBACH algorithm
% Downward long wave radiation flux "R_d" [W/m^2] is according to [1],[2] computed by
% (1) R_d = r_cloud * epsA * sigma * T^4,
% where "T" is the air temperature [K] (in the bottom layer?), sigma = 5.6703e-8 W/(m^2 K^4) is the Stefan-Boltzmann constant,
% "epsA" is the emissivity of the cloudless atmosphere given by
% (2) epsA = epsA0 * (e_A/T)^(1/7).
% Here "e_vap" is the vapor pressure in [Pa] from above, and epsA0 = 0.64 (units: [(K/Pa)^(1/7)]) a constant (see [2]).
% The factor "r_cloud" in (1) corrects for clouds and is given by
% (3) r_cloud = 1+0.22*n^2,
% where "n" is the cloudcover.
% [1] W. Knorr, "Satellite remote sensing and modelling of the global CO2 exchange of land vegetation", Examensarbeit 49,
% (Max Planck Institut fuer Meteorologie, Hamburg, 1998).
% [2] W. Brutsaert, "Evaporation into the Atmosphere", (Reidel, Dordrecht, 1982), pp. 299.
% Calculation of Vapour Pressure follows Monteith & Unsworth 2008, page 11f [3]

%% Constants
% longwave constants
sigma      = 5.6696e-8; % Stefan-Boltzmann constant [W/(m^2 K^4)]
T0         = 273.15;    % zero degrees Celsius expressed by Kelvin [K]
% VPD constants
eps        = 0.622;     % mixing ratio of wet and dry air
% T parameters eq2.27 from [3]
Tstar      = T0;
Tstroke    = 36;
estar      = 611;       % in Pa =.611 kPa
A          = 17.27;

%% Conversion to K
T=Tair+T0;

%% Determine whether we are dealing with daily or hourly values
if ~exist('dt', 'var')
    nstep=length(Tair);
    if(rem(nstep,365)==0)||(rem(nstep,366)==0)
        dt=1;
    elseif(rem(nstep,17520)==0)||(rem(nstep,17568)==0)
        dt=48;
    else
        error('longwave_from_metdata: unknown timestep for estimating longwave')
    end
end

%% Backcalculate cloudcover
fpar = max(0.,min(1,Rg./Rg_pot));
fpar(Rg_pot==0) = NaN;

%% Night-time values equal day-time average
if (dt > 1 ) 
     for i = 1:(length(fpar)/dt)
        pos=find(isnan(fpar((i-1)*dt+(1:dt))));
        fpar((i-1)*dt+pos)=nanmean(fpar((i-1)*dt+(1:dt)));
     end
else
     disp('longwave_from_metdata: Warning: daily not checked')
     for i = 1:(length(fpar)/dt)
        if(isnan(fpar(i)))
           a=max(length(Tair),min(1,i+(-2:2)-1));
           fpar(i)=nanmean(fpar(a));
        end
     end
end

%% Remaining gaps equal yearly average
pos=find(isnan(fpar));
if ( pos > 0 ) 
   fpar(pos)=nanmean(fpar);
   disp(['filling ' num2str(length(pos)) ' values with annual mean fpar'])
end

%% Cloud cover and cloud correction factor Eq. (3)
cloud_cover  = max(0.,min(1.,(1.0 - (fpar - 0.5) / 0.4)));
r_cloud = 1. + 0.22 .* cloud_cover .^ 2;                        

%% Saturation and actual Vapour pressure [3], and associated  emissivity Eq. (2)
esat=estar*exp(A*((T-Tstar)./(T-Tstroke)));
VP = esat - VPD;%';
VP(VP<0) = 1e-8;
epsA = 0.64 .* (VP ./ T) .^ 0.14285714;    
    
%% Longwave radiation flux downward Eq. (1)
%longwave = r_cloud' .* epsA .* sigma .* T .^ 4;	
longwave = r_cloud .* epsA .* sigma .* T .^ 4;	
% mark NaN as no data and check for plausible range
longwave(isnan(longwave))=missval;
longwave(longwave<10.)=missval;
longwave(longwave>1000.)=missval;
