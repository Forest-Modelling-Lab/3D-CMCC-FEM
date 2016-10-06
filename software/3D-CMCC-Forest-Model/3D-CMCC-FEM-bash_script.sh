#!/bin/bash
# Developed by Alessio Collalti (alessio.collalti@cmcc.it)
# starting date: 5 October 2016


CMCC="3D_CMCC_Forest_Model"
VERSION="v.5.2.2"
PROJECT="ISIMIP"

echo "**************************************"
echo "$CMCC $VERSION script for $PROJECT run"
echo "**************************************"

executable="3D_CMCC_Forest_Model"

#input paths
SITE_PATH=input/
PARAMETERIZATION_PATH=input/parameterization

#output_paths
export OUTPUT_PATH=output/

#run execution
model_run[0]=Debug
model_run[1]=Release

#declare sites
SITEs[0]=Soroe
SITEs[1]=Kroof
SITEs[2]=Peitz
SITEs[3]=All

#declare climate
CLIMATEs[0]=Historical
CLIMATEs[1]=Scenario
CLIMATEs[2]=All

#declare historical
HYSTs[0]=CLIMATE
HYSTs[1]=PRINCETON
HYSTs[2]=WATCH
HYSTs[3]=GSWP3
HYSTs[4]=WATCH-WFDEI
HYSTs[5]=All

#declare GCMs
GCMs[0]=GCM1
GCMs[1]=GCM2
GCMs[2]=GCM3
GCMs[3]=GCM4
GCMs[4]=GCM5
GCMs[5]=All

#declare RCPs
RCPs[0]=rcp8p5
RCPs[1]=rcp6p0
RCPs[2]=rcp4p5
RCPs[3]=rcp2p6
RCPs[4]=All

#declare Management
MANs[0]=on
MANs[1]=off
MANs[2]=All

#declare CO2 enrichment
CO2s[0]=on
CO2s[1]=off
CO2s[2]=All

#current data
today=`date +%Y-%m-%d`

########################################################################################################
#Debug or Release run
echo 'available model run'
for (( i = 0 ; i < ${#model_run[@]} ; ++i )) ; do
      echo -"${model_run[i]}"
done

echo "which model version do you want to use?"

#ask which run use	
read run

echo "model run '$run'"

match=no

#check if red run merge with available runs
for (( i = 0 ; i <= ${#model_run[@]} ; ++i )) ; do

    if [ "${run,,}" = "${model_run[$i],,}" ] ; then
    	match=yes
		run=${model_run[$i]}
     fi
done

#exit if none run merge with run list
if [ "$match" == "no" ] ; then
      echo $run "doesn't match with sites list exit program!"
exit
fi

#search in debug folder debug executable
if [ "$run" == "${model_run[0]}" ] ; then
	
	#find folder at first............
	if [ -d "Debug" ] ; then

		cd Debug
		
		#check if executable exists
		if [ -x "$executable" ] ; then
			
			folder_run=Debug
			
			cd ..
			
		else
			echo "$executable executable doesn't exist"
			exit
		fi	
	else
		echo "Debug folder doens't exist"
		exit
	fi
#search in release folder debug executable
else	
	#find folder at first............
	if [ -d "Release" ] ; then
		
		cd Release
		
		#check if executable exists
		if [ -x "$executable" ] ; then
			
			folder_run=Release
			
			cd..
		else
			echo "$executable executable doesn't exist"
			exit
		fi	
	else
		echo "Release folder doens't exist"
		exit
	fi
fi

#########################################################################################################
#log available sites
echo 'available sites:'
for (( i = 0 ; i < ${#SITEs[@]} ; ++i )) ; do
      echo -"${SITEs[i]}"
done

echo "which site do you want to simulate?"

#ask which site use
read site

echo "site to simulate '$site'"

match=no

#check if red site merge with available sites
for (( i = 0 ; i <= ${#SITEs[@]} ; ++i )) ; do

    if [ "${site,,}" = "${SITEs[$i],,}" ] ; then
    	match=yes
	site=${SITEs[$i]}
     fi
done

#exit if none site name merge with site list
if [ "$match" == "no" ] ; then
      echo $site "doesn't match with sites list exit program!"
exit
fi

#for counter
if [ "$site" == 'All' ] ; then
	sites_counter=${#SITEs[@]} 
else
	sites_counter=1
fi


#########################################################################################################
#log available CLIMATEs
echo 'available CLIMATEs:'
for (( i = 0 ; i < ${#CLIMATEs[@]} ; ++i )) ; do
      echo -"${CLIMATEs[i]}"
done

echo "which CLIMATEs do you want to use for '$site'?"

#ask which CLIMATEs to use
read climate

echo "CLIMATEs to use '$climate'"

match=no

#check if red climate merge with available CLIMATEs
for (( i = 0 ; i < ${#CLIMATEs[@]}  ; ++i )) ; do

	if [ "${climate,,}" == "${CLIMATEs[$i],,}" ] ; then
	match=yes     
	climate=${CLIMATEs[$i]} 
	fi
done

#exit if none climate name merge with CLIMATEs list
if [ "$match" == "no" ] ; then
      echo $climate "doesn't match with CLIMATEs list exit program!"
exit
fi

#for counter
if [ "$climate" == 'All' ] ; then
	clim_counter=${#CLIMATEs[@]} 
else
	clim_counter=1
fi
#########################################################################################################
#IT CURRENTLY RUNS ONLY FOR SCENARIOS AND FOR ALL GCMs AND ALL RCPs

#Historical
if [ "$climate" == "${CLIMATEs[0]}" ] ; then
	
	#log available Historical
	echo 'available Historical:'
	for (( i = 0 ; i < ${#HYSTs[@]} ; ++i )) ; do
		echo -"${HYSTs[i]}"
		done

	echo "which Historical do you want to use for '$site'?"

	#ask which HYSTs to use
	read Hist

	echo "Historical to use '$Hist'"

	match=no

	#check if red Hist merge with available HYSTs
	for (( i = 0 ; i < ${#HYSTs[@]} ; ++i )) ; do

		if [ "${Hist,,}" == "${HYSTs[$i],,}" ] ; then
		match=yes  
		Hist=${HISTs[$i]}   
		fi
		done

	#exit if none Hist merge with HYSTs list
	if [ "$match" == "no" ] ; then
		echo $Hist "doesn't match with Historical list exit program!"
	exit
	fi
	
	#for counter
	if [ "$Hist" == 'All' ] ; then
		hist_counter=${#HYSTs[@]} 
	else
		hist_counter=1
	fi
fi

#****************************************************************************

#Scenario
if [ "$climate" == "${CLIMATEs[1]}" ] ; then
	
	#log available GCMs
	echo 'available GCMs:'
	for (( i = 0 ; i < ${#GCMs[@]} ; ++i )) ; do
			echo -"${GCMs[i]}"
	done

	echo "which GCMs do you want to use for '$site'?"
	
	#ask which GCM to use
	read gcm
	
	echo "gcm to use '$gcm'"
	
	match=no
	
	#check if red gcm merge with available GCMs
	for (( i = 0 ; i < ${#GCMs[@]} ; ++i )) ; do

		if [ "${gcm,,}" == "${GCMs[$i],,}" ] ; then
		match=yes
		gcm=${GCMs[$i]}
		fi
	done

		#exit if none gcm merge with GCMs list
	if [ "$match" == "no" ] ; then
		echo $gcm "doesn't match with GCMs list exit program!"
		exit
	fi
	
	#for counter
	if [ "$gcm" == 'All' ] ; then
		gcm_counter=${#GCMs[@]} 
	else
		gcm_counter=1
	fi
	
	#*****************************************************************

	#log available RCPs
	echo 'available RCPs:'
	for (( i = 0 ; i < ${#RCPs[@]} ; ++i )) ; do
			echo -"${RCPs[i]}"
	done

	echo "which RCPs do you want to use for '$site' and '$gcm'?"

	#ask which rcp to use
	read rcp

	echo "RCPs to use '$rcp'"

	match=no

	#check if red rcp merge with available RCPs
	for (( i = 0 ; i < ${#RCPs[@]} ; ++i )) ; do

		if [ "${rcp,,}" == "${RCPs[$i],,}" ] ; then
		match=yes
		rcp=${RCPs[$i]}
		fi
	done

		#exit if none rcp name merge with RCPs list
	if [ "$match" == "no" ] ; then
			echo $rcp "doesn't match with RCPs list exit program!"
	exit
	fi
	
	#for counter
	if [ "$rcp" == 'All' ] ; then
		rcp_counter=${#RCPs[@]} 
	else
		rcp_counter=1
	fi
fi

#########################################################################################################

#log available Management
echo 'available management'
for (( i = 0 ; i < ${#MANs[@]} ; ++i )) ; do
      echo -"${MANs[i]}"
done

echo "Management on or off for '$site' and '$gcm' and '$rcp'?"

#ask if use management
read management

echo "Management is '$management'"

match=no

#check if red management options merge with available management
for (( i = 0 ; i < ${#MANs[@]} ; ++i )) ; do

	if [ "${management,,}" == "${MANs[$i],,}" ] ; then
	match=yes
	management=${MANs[$i]}
	fi
done

#exit if none management options merge with management list
if [ "$match" == "no" ] ; then
      echo $management "doesn't match with Management possibilities list exit program!"
exit
fi

#for counter
if [ "$management" == 'All' ] ; then
	man_counter=${#RCPs[@]} 
else
	man_counter=1
fi

#########################################################################################################
#log available CO2
echo 'CO2 enrichment on or off?:'
for (( i = 0 ; i < ${#CO2s[@]} ; ++i )) ; do
      echo -"${CO2s[i]}"
done

echo "CO2 enrichment on or off for '$site' and '$gcm' and '$rcp' and Management '$management'?"

#ask if use CO2 enrichment
read co2

echo "CO2 enrichment is '$co2'"

match=no

#check if red CO2 options merge with available CO2 list 
for (( i = 0 ; i < ${#CO2s[@]} ; ++i )) ; do

	if [ "${co2,,}" == "${CO2s[$i],,}" ] ; then
	match=yes      
	co2=${CO2[$i]}
	fi
done

#exit if none CO2 options merge with CO2 list
if [ "$match" == "no" ] ; then
      echo $co2 "doesn't match with CO2 enrichment possibilities list exit program!"
exit
fi

#for counter
if [ "$co2" == 'All' ] ; then
	co2_counter=${#CO2s[@]} 
else
	co2_counter=1
fi

#########################################################################################################
#to complete
#if [ "$climate" == "${CLIMATEs[1]}"] ; then
		#MET_PATH=ISIMIP/Historical/"$hist"_1960-2001.txt
		#SOIL_PATH=ISIMIP/Historical/"$site"_soil_"$hist"_ISIMIP.txt
		#CO2_PATH=ISIMIP/CO2/CO2_historical_1901_2012.txt
#fi
#if [ "$climate" == "${CLIMATEs[2]}"] ; then
		#MET_PATH=ISIMIP/"$gcm"/"$gcm"_hist_"$rcp"_1960_2099.txt
		#SOIL_PATH=ISIMIP/"$gcm"/"$site"_soil_"$rcp"_ISIMIP.txt
		#CO2_PATH=ISIMIP/CO2/CO2_"$rcp"_1950_2099.txt
#fi

for (( i = 0 ; i < $site_counter; ++i )) ; do
for (( i = 0 ; i < $clim_counter ; ++i )) ; do
for (( i = 0 ; i < $gcm_counter ; ++i )) ; do
for (( i = 0 ; i < $rcp_counter ; ++i )) ; do
for (( i = 0 ; i < $man_counter ; ++i )) ; do
for (( i = 0 ; i < $co2_counter ; ++i )) ; do
	
	#add site name to current paths
	SITE_PATH=input/$site
	OUTPUT_PATH=output/$site
	=ISIMIP/"$site"_stand_1960_ISIMIP.txt
	TOPO_PATH=ISIMIP/"$site"_topo_ISIMIP.txt

	#add management and co2 to setting path
	SETTING_PATH=ISIMIP/"$site"_settings_ISIMIP_Manag-"$management"_CO2-"$co2".txt

	#add gcm and rcp to meteo co2 and soil path
	MET_PATH=ISIMIP/"$gcm"/"$gcm"_hist_"$rcp"_1960_2099.txt
	SOIL_PATH=ISIMIP/"$gcm"/"$site"_soil_"$rcp"_ISIMIP.txt
	CO2_PATH=ISIMIP/CO2/CO2_"$rcp"_1950_2099.txt

	#goes to executable folder
	cd $folder_run

	cp 3D_CMCC_Forest_Model ../

	cd ..

	#log arguments paths
	echo "*****************************"
	echo "$CMCC $VERSION-$PROJECT arguments"
	echo -i $SITE_PATH
	echo -p $PARAMETERIZATION_PATH
	echo -d $STAND_PATH
	echo -s $SOIL_PATH
	echo -t $TOPO_PATH
	echo -m $MET_PATH
	echo -k $CO2_PATH
	echo -c $SETTING_PATH
	echo "*****************************"

	#add paths and arguments to executable and run
	./3D_CMCC_Forest_Model -i $SITE_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH 
done
done
done
done
done
done


#delete copied executable from current directory
rm 3D_CMCC_Forest_Model

echo "script finished!"

#exit program
exit