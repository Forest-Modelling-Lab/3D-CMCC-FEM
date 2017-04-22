#!/bin/bash
# Developed by:
# Alessio Collalti (alessio.collalti@cmcc.it)
# Alessio Ribeca (alessio.ribeca@cmcc.it)
# starting date: 13 November 2016

MODEL="3D-CMCC-CNR FEM"
VERSION="v.5.3.2"
PROJECT="ISIMIP"

echo "***************************************************************"
echo "* $MODEL $VERSION script for PAPER runs in "$OSTYPE" *"
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
SITEs=(Soroe Hyytiala All)

#declare GCMs or Repeated
GCMs=(GCM1 GCM2 GCM3 GCM4 GCM5 All)

#declare RCPs
RCPs=(rcp0p0 rcp2p6 rcp4p5 rcp6p0 rcp8p5 All)

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

#for unix
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
				
				cd ..
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

#copy netcdf dll (only for windows build)
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

#assign all available site folders to site array
#SITEs=(*)

cd ..

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
	
		#for counter
	if [ "$site" == 'All' ] ; then
		site_counter=${#SITEs[@]}
		let "site_counter-=1"
	else
		site_counter=1
	fi


#########################################################################################################
	
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

function CC_run {
	for (( b = 0 ; b < $site_counter ; ++b )) ; do
		for (( c = 0 ; c < $gcm_counter ; ++c )) ; do
			for (( d = 0 ; d < $rcp_counter ; ++d )) ; do
				for (( e = 0 ; e < $man_counter ; ++e )) ; do
					for (( f = 0 ; f < $co2_counter ; ++f )) ; do
					
					if (( $site_counter > 1 )) ; then site=${SITEs[$b]}; fi
					if (( $gcm_counter  > 1 )) ; then gcm=${GCMs[$c]}; fi
					if (( $rcp_counter  > 1 )) ; then rcp=${RCPs[$d]}; fi
					if (( $man_counter  > 1 )) ; then management=${MANs[$e]}; fi
					if (( $co2_counter  > 1 )) ; then co2=${CO2s[$f]}; fi
					
					echo "multi run"
					echo 'running for' "$site"
					echo 'running for' "$climate"
					echo 'running for' "$gcm"
					echo 'running for' "$rcp"
					echo 'running with management =' "$management" 
					echo 'running with co2 =' "$co2"
										
					#add site name to current paths
					SITE_PATH=input/"$site"
					OUTPUT_PATH=output/"$site"
					STAND_PATH=PAPER/"$site"_stand_ISIMIP.txt
					TOPO_PATH=PAPER/"$site"_topo_ISIMIP.txt
		
					SETTING_PATH=PAPER/2A/"$site"_settings_ISIMIP_Manag-"$management"_CO2-"$co2".txt
				
					#add gcm and rcp to meteo co2 and soil path
					MET_PATH=PAPER/2A/"$gcm"/"$gcm"_"$rcp".txt
					SOIL_PATH=PAPER/"$site"_soil_ISIMIP.txt
					CO2_PATH=PAPER/2A/CO2/CO2_"$rcp"_1950_2099.txt
									
					#add paths and arguments to executable and run
					$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH #&
					
					#log arguments paths
					echo "*****************************"
					echo "$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH"
					echo "$MODEL $VERSION-PAPER arguments:"
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

#NO CLIMATE CHANGE SIMULATIONS + TRANSIENT CO2
function no_CC_CO2_run {
	for (( b = 0 ; b < $site_counter ; ++b )) ; do
		for (( c = 0 ; c < $gcm_counter ; ++c )) ; do
			for (( d = 0 ; d < $rcp_counter ; ++d )) ; do
				for (( e = 0 ; e < $man_counter ; ++e )) ; do
					
					if (( $site_counter > 1 )) ; then site=${SITEs[$b]}; fi
					if (( $gcm_counter  > 1 )) ; then gcm=${GCMs[$c]}; fi
					if (( $rcp_counter  > 1 )) ; then rcp=${RCPs[$d]}; fi
					if (( $man_counter  > 1 )) ; then management=${MANs[$e]}; fi
					if (( $co2_counter  > 1 )) ; then co2=${CO2s[$f]}; fi
					
					echo "multi run"
					echo 'running for' "$site"
					echo 'running for' "$climate"
					echo 'running for' "$gcm"
					echo 'running for' "$rcp"
					echo 'running with management =' "$management" 
					echo 'running with co2 =' "$co2"
										
					#add site name to current paths
					SITE_PATH=input/"$site"
					OUTPUT_PATH=output/"$site"
					STAND_PATH=PAPER/"$site"_stand_ISIMIP.txt
					TOPO_PATH=PAPER/"$site"_topo_ISIMIP.txt
		
					SETTING_PATH=PAPER/2A/"$site"_settings_ISIMIP_Manag-"$management"_CO2-on.txt
				
					#add gcm and rcp to meteo co2 and soil path
					MET_PATH=PAPER/2A/"$gcm"/"$gcm"_rcp0p0.txt
					SOIL_PATH=PAPER/"$site"_soil_ISIMIP.txt
					CO2_PATH=PAPER/2A/CO2/CO2_"$rcp"_1950_2099.txt
									
					#add paths and arguments to executable and run
					$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH #&
					
					#log arguments paths
					echo "*****************************"
					echo "$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH"
					echo "$MODEL $VERSION-PAPER arguments:"
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
}

#simulation with climate change
CC_run

#simulation with NO climate change (rcp0.0) but transient [CO2]
#no_CC_CO2_run

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
ELAPSED=`echo "scale=4; ($END - $START) / 1000000000" | bc`
ELAPSED_min=`echo "scale=4; $ELAPSED / 60" | bc`
ELAPSED_hou=`echo "scale=4; $ELAPSED / 3600" | bc`
echo "elapsed time in bash script:" $ELAPSED "secs" $ELAPSED_min "min" $ELAPSED_hou "hours"

#fi

echo "script finished!"

#exit program
exit
