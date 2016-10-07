#!/bin/bash
# Developed by Alessio Collalti (alessio.collalti@cmcc.it)
# corcazzo...modified by Alessio Ribeca (alessio.ribeca@cmcc.it)
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
model_run=(Debug Release)

#declare sites
SITEs=(Soroe Kroof Peitz All)

#declare climate
CLIMATEs=(Historical Scenario All)

#declare historical
HYSTs=(CLIMATE PRINCETON WATCH GSWP3 WATCH-WFDEI All)

#declare GCMs
GCMs=(GCM1 GCM2 GCM3 GCM4 GCM5 All)

#declare RCPs
RCPs=(rcp8p5 rcp6p0 rcp4p5 rcp2p6 All)

#declare Management
MANs=(on off All)

#declare CO2 enrichment
CO2s=(on off All)

#current date
today=`date +%Y-%m-%d`

########################################################################################################
#Debug or Release run
echo 'available model run'
for (( i = 0 ; i < ${#model_run[@]} ; ++i )) ; do
      echo -"${model_run[i]}"
done

echo "which model version do you want to use?"

#ask which run use
match=no
while :
	do
	read run
	for (( i = 0 ; i <= ${#model_run[@]} ; ++i )) ; do
		if [ "${run,,}" = "${model_run[$i],,}" ] ; then
			match=yes
			run=${model_run[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi
	
	echo "'$run' doesn't match with model run list. please rewrite it."
done


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
match=no
while :
	do
	read site
	for (( i = 0 ; i <= ${#SITEs[@]} ; ++i )) ; do
		if [ "${site,,}" = "${SITEs[$i],,}" ] ; then
			match=yes
			site=${SITEs[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi
	
	echo "'$site' doesn't match with site list. please rewrite it."
done


#for counter
if [ "$site" == 'All' ] ; then
	site_counter=${#SITEs[@]} 
else
	site_counter=1
fi

#########################################################################################################
#log available CLIMATEs
echo 'available CLIMATEs:'
for (( i = 0 ; i < ${#CLIMATEs[@]} ; ++i )) ; do
      echo -"${CLIMATEs[i]}"
done

echo "which CLIMATEs do you want to use for '$site'?"
	
#ask which climate use
match=no
while :
	do
	read climate
	for (( i = 0 ; i <= ${#CLIMATEs[@]} ; ++i )) ; do
		if [ "${climate,,}" = "${CLIMATEs[$i],,}" ] ; then
			match=yes
			climate=${CLIMATEs[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi
	
	echo "'$climate' doesn't match with climate list. please rewrite it."
done

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
		
	#ask which historical use
	match=no
	while :
	do
	read Hist
	for (( i = 0 ; i <= ${#HYSTs[@]} ; ++i )) ; do
		if [ "${Hist,,}" = "${HYSTs[$i],,}" ] ; then
			match=yes
			Hist=${HYSTs[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi
	
	echo "'$Hist' doesn't match with climate list. please rewrite it."
	done
	
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
		
	#ask which GCMs use
	match=no
	while :
	do
	read gcm
	for (( i = 0 ; i <= ${#GCMs[@]} ; ++i )) ; do
		if [ "${gcm,,}" = "${GCMs[$i],,}" ] ; then
			match=yes
			gcm=${GCMs[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi
	
	echo "'$gcm' doesn't match with GCMs list. please rewrite it."
	done
	
	#for counter
	if [ "$gcm" == 'All' ] ; then
		gcm_counter=${#GCMs[@]}
		let "gcm_counter-=1"
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
		
	#ask which RCPs use
	match=no
	while :
	do
	read rcp
	for (( i = 0 ; i <= ${#RCPs[@]} ; ++i )) ; do
		if [ "${rcp,,}" = "${RCPs[$i],,}" ] ; then
			match=yes
			rcp=${RCPs[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi
	
	echo "'$rcp' doesn't match with RCPs list. please rewrite it."
	done
	
	#for counter
	if [ "$rcp" == 'All' ] ; then
		rcp_counter=${#RCPs[@]} 
		let "rcp_counter-=1"
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

	#ask which run use
	match=no
	while :
	do
	read management
	for (( i = 0 ; i <= ${#MANs[@]} ; ++i )) ; do
		if [ "${management,,}" = "${MANs[$i],,}" ] ; then
			match=yes
			management=${MANs[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi
	
	echo "'$management' doesn't match with MANs list. please rewrite it."
	done

#for counter
if [ "$management" == 'All' ] ; then
	man_counter=${#MANs[@]} 
	let "man_counter-=1"
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

	#ask which co2 use
	match=no
	while :
	do
	read co2
	for (( i = 0 ; i <= ${#CO2s[@]} ; ++i )) ; do
		if [ "${co2,,}" = "${CO2s[$i],,}" ] ; then
			match=yes
			co2=${CO2s[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi
	
	echo "'$co2' doesn't match with CO2s list. please rewrite it."
	done

#for counter
if [ "$co2" == 'All' ] ; then
	co2_counter=${#CO2s[@]} 
	let "co2_counter-=1"
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

for (( a = 0 ; a < $site_counter ; ++a )) ; do
	site=${SITEs[$a]}
	echo 'running for' "$site" 'site_counter' "$site_counter"
						
	for (( b = 0 ; b < $clim_counter ; ++b )) ; do
		climate=${CLIMATEs[$i]}
		echo 'running for' "$climate" 'clim_counter' "$clim_counter"

		for (( c = 0 ; c < $gcm_counter ; ++c )) ; do
			gcm=${GCMs[$c]}
			echo 'running for' "$gcm" 'gcm_counter' "$gcm_counter"
						
			for (( d = 0 ; d < $rcp_counter ; ++d )) ; do
				rcp=${RCPs[$d]}
				echo 'running for' "$rcp"'rcp_counter' "$rcp_counter"
						
				for (( e = 0 ; e < $man_counter ; ++e )) ; do
						management=${MANs[$e]}
						echo 'running with management =' "$management"
						
					for (( f = 0 ; f < $co2_counter ; ++f )) ; do
						co2=${CO2s[$f]}
						echo 'running with co2 =' "$co2"
						
						#add site name to current paths
						SITE_PATH=input/$site
						OUTPUT_PATH=output/$site
						STAND_PATH=ISIMIP/"$site"_stand_1960_ISIMIP.txt
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
					
						#add paths and arguments to executable and run
						#./3D_CMCC_Forest_Model -i $SITE_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH -o $OUTPUT_PATH
						
						#log arguments paths
						echo "*****************************"
						echo "$CMCC $VERSION-$PROJECT arguments"
						echo "-i" $SITE_PATH
						echo "-p" $PARAMETERIZATION_PATH
						echo "-d" $STAND_PATH
						echo "-s" $SOIL_PATH
						echo "-t" $TOPO_PATH
						echo "-m" $MET_PATH
						echo "-k" $CO2_PATH
						echo "-c" $SETTING_PATH
						echo "-o" $OUTPUT_PATH
						echo "*****************************"
done
done
done
done
done
done

#delete copied executable from current directory
echo "...removing executable from project directory"
rm 3D_CMCC_Forest_Model

echo "script finished!"

#exit program
exit