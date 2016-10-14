#!/bin/bash
# Developed by:
# Alessio Collalti (alessio.collalti@cmcc.it)
# Alessio Ribeca (alessio.ribeca@cmcc.it)
# starting date: 5 October 2016

#note: it currently works for just one case or all


MODEL="3D-CMCC-CNR FEM"
VERSION="v.5.2.2"
PROJECT="ISIMIP"

echo "***************************************************************"
echo "* $MODEL $VERSION script for $PROJECT runs in "$OSTYPE" *"
echo "***************************************************************"

launch="./"
if [[ "$OSTYPE" == "linux-gnu" ]]; then
	executable="3D_CMCC_Forest_Model"
elif [[ "$OSTYPE" == "cygwin" ]]; then
	executable="3D-CMCC-Forest-Model"
fi

#input paths
SITE_PATH=input/
PARAMETERIZATION_PATH=input/parameterization

#output_paths
export OUTPUT_PATH=output/

#run execution
model_run=(Debug Release)

#declare sites
SITEs=(Soroe Kroof Peitz)

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

#for *nix
if [[ "$OSTYPE" == "linux-gnu" ]]; then
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
				echo "$executable executable doesn't exist (exit)"
				exit
			fi	
		else
			echo "Debug folder doesn't exist (exit)"
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
				echo "$executable executable doesn't exist (exit)"
				exit
			fi	
		else
			echo "Release folder doesn't exist (exit)"
			exit
		fi
	fi
# for windows
elif [[ "$OSTYPE" == "cygwin" ]]; then
	folder_run=bin
	
	#windows stuff
	if [ "$run" == "${model_run[0]}" ] ; then
		suffix="_debug.exe" # very lame
		
	else
		suffix=".exe" # very lame
	fi
	executable="$executable$suffix"
	
	#check bin folder
	if [ -d $folder_run ] ; then

		cd $folder_run
	
		#check if executable exists
		if [ -x "$executable" ] ; then		
			cd ..
		else
			echo "$executable doesn't exist (exit)"
			exit
		fi	
	else
		echo "Executable folder doesn't exist (exit)"
		exit
	fi
fi

#go to executable directory
cd $folder_run

#copy to previous directory executable
cp $executable ../

#copy netcdf dll
#only for windows build
if [[ "$OSTYPE" == "cygwin" ]]; then
	netcdf_dll="netcdf.dll"
	#check if dll exists
	if [ -x "$netcdf_dll" ] ; then		
		cp $netcdf_dll ../
	else
		echo "$netcdf_dll doesn't exist (exit)"
		exit
	fi
fi

#back to previous directory
cd ..

#########################################################################################################
#list of available folder sites
cd input

#list
ls -d *

cd ..
#########################################################################################################
#log available sites
echo 'available sites to be simulated:'
for (( i = 0 ; i < ${#SITEs[@]} ; ++i )) ; do
	echo -"${SITEs[i]}"
done

#ask which site use
echo "which site do you want to simulate?"
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

#########################################################################################################
#log available Input data
#assign for a single site a starting year

cd input 
	
echo "$site available stand initialization year(s):"

cd "$site"

#check if is an ISIMIP project
if [ -d "$PROJECT" ] ; then

	cd "$PROJECT"
	
	#find among *.txt files occurrence for "stand" and "year"
	#and put in array
	YEARs=($(find *.txt | ( grep "stand" | sed -e s/[^0-9]//g )))
	cd ../../..
else
	
	#find among *.txt files occurrence for "stand" and "year"
	#and put in array
	YEARs=($(find *.txt | ( grep "stand" | sed -e s/[^0-9]//g )))
	cd ../..
fi

for (( i = 0 ; i < ${#YEARs[@]} ; ++i )) ; do
	echo -"${YEARs[i]}"
done

#########################################################################################################
#log available year data
#ask which year use
match=no
echo "which is the starting year for "$site" to simulate?"
while :
	do
	read year
	for (( i = 0 ; i < ${#YEARs[@]} ; ++i )) ; do
		if [ "${year,,}" = "${YEARs[$i],,}" ] ; then
			match=yes
			year=${YEARs[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi
	
	echo "'$year' doesn't match with year list. please rewrite it."
done
	
echo "starting year for '$site' = '$year'"

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
	let "clim_counter-=1"
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
		let "hist_counter-=1"
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
#########################################################################################################
#########################################################################################################

# compute elapsed time
START=`date +%s%N`

echo 'running for' "$site"


function single_run {
	
	echo "single run"
	echo 'running for' "$climate"
	echo 'running for' "$gcm"
	echo 'running for' "$rcp"
	echo 'running with management =' "$management" 
	echo 'running with co2 =' "$co2"
	
	#add site name to current paths
	SITE_PATH=input/"$site"
	OUTPUT_PATH=output/"$site"
	STAND_PATH=ISIMIP/"$site"_stand_"$year"_"$PROJECT".txt
	TOPO_PATH=ISIMIP/"$site"_topo_"$PROJECT".txt

	#add management and co2 to setting path
	SETTING_PATH=ISIMIP/"$site"_settings_"$PROJECT"_Manag-"$management"_CO2-"$co2".txt

	#add gcm and rcp to meteo co2 and soil path
	MET_PATH=ISIMIP/"$gcm"/"$gcm"_hist_"$rcp"_"$year"_2099.txt
	SOIL_PATH=ISIMIP/"$gcm"/"$site"_soil_"$rcp"_"$PROJECT".txt
	CO2_PATH=ISIMIP/CO2/CO2_"$rcp"_1950_2099.txt
	
	#add paths and arguments to executable and run
	$launch$executable" -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH"
	
	#log arguments paths
	echo "*****************************"
	echo "$MODEL $VERSION-$PROJECT arguments:"
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
}

function multi_run {
	for (( b = 0 ; b < $clim_counter ; ++b )) ; do
		for (( c = 0 ; c < $gcm_counter ; ++c )) ; do
			for (( d = 0 ; d < $rcp_counter ; ++d )) ; do
				for (( e = 0 ; e < $man_counter ; ++e )) ; do
					for (( f = 0 ; f < $co2_counter ; ++f )) ; do
					
					if (( $clim_counter > 1 )) ; then climate=${CLIMATEs[$b]}; fi
					if (( $gcm_counter > 1 )) ; then gcm=${GCMs[$c]}; fi
					if (( $rcp_counter > 1)) ; then rcp=${RCPs[$d]}; fi				
					if (( $man_counter > 1 )) ; then management=${MANs[$e]}; fi
					if (( $co2_counter > 1 )) ; then co2=${CO2s[$f]}; fi
					
					echo "multi run"
					echo 'running for' "$climate"
					echo 'running for' "$gcm"
					echo 'running for' "$rcp"
					echo 'running with management =' "$management" 
					echo 'running with co2 =' "$co2"
										
					#add site name to current paths
					SITE_PATH=input/"$site"
					OUTPUT_PATH=output/"$site"
					STAND_PATH=ISIMIP/"$site"_stand_"$year"_"$PROJECT".txt
					TOPO_PATH=ISIMIP/"$site"_topo_"$PROJECT".txt
				
					#add management and co2 to setting path
					SETTING_PATH=ISIMIP/"$site"_settings_"$PROJECT"_Manag-"$management"_CO2-"$co2".txt
				
					#add gcm and rcp to meteo co2 and soil path
					MET_PATH=ISIMIP/"$gcm"/"$gcm"_hist_"$rcp"_"$year"_2099.txt
					SOIL_PATH=ISIMIP/"$gcm"/"$site"_soil_"$rcp"_"$PROJECT".txt
					CO2_PATH=ISIMIP/CO2/CO2_"$rcp"_1950_2099.txt
									
					#add paths and arguments to executable and run
					$launch$executable" -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH"
					
					#log arguments paths
					echo "*****************************"
					echo "$MODEL $VERSION-$PROJECT arguments:"
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
}

if [ "$gcm_counter" == 1 ] && [ "$rcp_counter" == 1 ] && [ "$man_counter" == 1 ] && [ "$co2_counter" == 1 ] ; then
	#launch single run
	single_run
else
	#launch multi run
	multi_run
fi

#delete copied executable from current directory
echo "...removing executable from project directory"
rm $executable

#remove netcdf dll
#only for windows build
if [[ "$OSTYPE" == "cygwin" ]]; then
	echo "...removing netcdf dll from project directory"
	rm -f $netcdf_dll
fi

#log elapsed time
#if [[ "$OSTYPE" == "linux-gnu" ]]; then
END=`date +%s%N`
ELAPSED=`echo "scale=2; ($END - $START) / 1000000000" | bc`
echo "elapsed time in bash script = $ELAPSED secs"
#fi

echo "script finished!"

#exit program
exit
