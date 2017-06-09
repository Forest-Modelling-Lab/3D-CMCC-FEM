#!/bin/bash
# Developed by:
# Alessio Collalti (alessio.collalti@cmcc.it)
# Alessio Ribeca (alessio.ribeca@cmcc.it)
# starting date: 13 November 2016

MODEL="3D-CMCC-CNR FEM"
VERSION="v.5.3.3"
PROJECT="ISIMIP"

echo "***************************************************************"
echo "* $MODEL $VERSION script for ISIMIP runs in "$OSTYPE" *"
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
SITEs=(Bily_Kriz Collelongo Hyytiala Kroof LeBray Peitz Solling_beech Solling_spruce Soroe All)

#experiments
EXPs=(LOCAL FT 2A 2B 2BLBC 2Bpico 2BLBCpico)

#declare ESMs or Repeated
ESMs=(ESM1 ESM2 ESM3 ESM4 ESM5 ESM6 ESM7 ESM8 ESM9 ESM10 All)

#declare RCPs
RCPs=(rcp2p6 rcp4p5 rcp6p0 rcp8p5 All)

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
	
#for counter
if [ "$site" == 'All' ] ; then
	site_counter=${#SITEs[@]}
	let "site_counter-=1"
else
	site_counter=1
fi
echo "site = '$site'"

#########################################################################################################
#log available experiments
echo 'available experiments to be simulated:'
for (( i = 0 ; i < ${#EXPs[@]} ; ++i )) ; do
	echo -"${EXPs[i]}"
done

#ask which experiment use
echo "which ISIMIP experiments?"
match=no

while :
	do
		read exp
			for (( i = 0 ; i <= ${#EXPs[@]} ; ++i )) ; do
		if [ "${exp,,}" = "${EXPs[$i],,}" ] ; then
			match=yes
			exp=${EXPs[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi
	
	echo "'$exp' doesn't match with experiment list. please rewrite it."
	
done
	
#for counter
if [ "$exp" == 'All' ] ; then
		exp_counter=${#EXPs[@]}
		let "exp_counter-=1"
	else
		exp_counter=1
fi
echo "experiment = '$exp'"

#########################################################################################################

if [ "${exp,,}" != "${EXPs[$1],,}" ] ; then
			
	#log available ESMs
	echo 'available ESMs:'
	for (( i = 0 ; i < ${#ESMs[@]} ; ++i )) ; do
			echo -"${ESMs[i]}"
	done

	echo "which ESMs do you want to use for '$site'?"
				
	#ask which ESMs use
	match=no
	while :
	do
		read esm
		for (( i = 0 ; i <= ${#ESMs[@]} ; ++i )) ; do
			if [ "${esm,,}" = "${ESMs[$i],,}" ] ; then
				match=yes
				esm=${ESMs[$i]}
			fi
		done
		if [ "$match" == "yes" ] ; then
			break;
		fi
	
		echo "'$esm' doesn't match with ESMs list. please rewrite it."
	done
	
	#for counter
	if [ "$esm" == 'All' ] ; then
		esm_counter=${#ESMs[@]}
		let "esm_counter-=1"
	else
		esm_counter=1
	fi
	echo "esm = '$esm'"
	
#########################################################################################################

	#log available RCPs
	echo 'available RCPs:'
	for (( i = 0 ; i < ${#RCPs[@]} ; ++i )) ; do
			echo -"${RCPs[i]}"
	done

	echo "which RCPs do you want to use for '$site' and '$esm'?"
		
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
	echo "rcp = '$rcp'"
fi


#########################################################################################################

	#log available Management
echo 'available management'
for (( i = 0 ; i < ${#MANs[@]} ; ++i )) ; do
	echo -"${MANs[i]}"
done

echo "Management on or off for '$site' and '$esm' and '$rcp'?"

	#ask which run use
match=no
while :
	do
		
	read man
	for (( i = 0 ; i <= ${#MANs[@]} ; ++i )) ; do
		if [ "${man,,}" = "${MANs[$i],,}" ] ; then
			match=yes
			man=${MANs[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi

	echo "'$man' doesn't match with MANs list. please rewrite it."
	done

	#for counter
if [ "$man" == 'All' ] ; then
	man_counter=${#MANs[@]} 
	let "man_counter-=1"
else
man_counter=1
fi
echo "management = '$man'"

#########################################################################################################

if [ "${exp,,}" != "${EXPs[$1],,}" ] ; then
	
	#log available CO2
	echo 'CO2 enrichment on or off?:'
	for (( i = 0 ; i < ${#CO2s[@]} ; ++i )) ; do
		echo -"${CO2s[i]}"
	done
	
	echo "CO2 enrichment on or off for '$site' and '$esm' and '$rcp' and Management '$man'?"
	
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
fi

echo "co2 = '$co2'"

#########################################################################################################

# compute elapsed time
START=`date +%s%N`

#########################################################################################################
############################################## FAST TRACK ###############################################
#########################################################################################################

function FT_runs {
	for (( b = 0 ; b < $site_counter ; ++b )) ; do
		for (( c = 0 ; c < $esm_counter ; ++c )) ; do
			for (( d = 0 ; d < $rcp_counter ; ++d )) ; do
				for (( e = 0 ; e < $man_counter ; ++e )) ; do
					for (( f = 0 ; f < $co2_counter ; ++f )) ; do
					
					if (( $site_counter > 1 )) ; then site=${SITEs[$b]}; fi
					if (( $esm_counter  > 1 )) ; then esm=${ESMs[$c]}; fi
					if (( $rcp_counter  > 1 )) ; then rcp=${RCPs[$d]}; fi
					if (( $man_counter  > 1 )) ; then man=${MANs[$e]}; fi
					if (( $co2_counter  > 1 )) ; then co2=${CO2s[$f]}; fi
					
					echo "*****************************"
					echo 'running for' "$exp"
					echo 'running for' "$site"
					echo 'running for' "$climate"
					echo 'running for' "$esm"
					echo 'running for' "$rcp"
					echo 'running with management =' "$man" 
					echo 'running with co2 =' "$co2"
					echo "*****************************"
										
					#add site name to current paths
					SITE_PATH=input/"$site"
					OUTPUT_PATH=output/ISIMIP_OUTPUT/FT/"$site"
					STAND_PATH=ISIMIP/"$site"_stand_ISIMIP.txt
					TOPO_PATH=ISIMIP/"$site"_topo_ISIMIP.txt
		
					SETTING_PATH=ISIMIP/FT/"$site"_settings_ISIMIP_Manag-"$man"_CO2-"$co2".txt
				
					#add esm and rcp to meteo co2 and soil path
					MET_PATH=ISIMIP/FT/"$esm"/"$esm"_"$rcp".txt
					SOIL_PATH=ISIMIP/"$site"_soil_ISIMIP.txt
					CO2_PATH=ISIMIP/CO2/CO2_"$rcp".txt
					
					#log arguments paths
					echo "*****************************"
					echo "$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH"
					echo "$MODEL $VERSION-ISIMIP arguments:"
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
					
					#add paths and arguments to executable and run
					$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH #&

					done
				done
			done
		done
	done
}
#########################################################################################################
############################################## LOCAL ####################################################
#########################################################################################################

function LOCAL_runs {
	for (( b = 0 ; b < $site_counter ; ++b )) ; do
		for (( e = 0 ; e < $man_counter ; ++e )) ; do
			
			if (( $site_counter > 1 )) ; then site=${SITEs[$b]}; fi
			if (( $man_counter  > 1 )) ; then man=${MANs[$e]}; fi
			
			echo "*****************************"
			echo 'running for' "$exp"
			echo 'running for' "$site"
			echo 'running with management =' "$man" 
			echo "*****************************"
								
			#add site name to current paths
			SITE_PATH=input/"$site"
			OUTPUT_PATH=output/ISIMIP_OUTPUT/LOCAL/"$site"
			STAND_PATH=ISIMIP/"$site"_stand_ISIMIP.txt
			TOPO_PATH=ISIMIP/"$site"_topo_ISIMIP.txt

			SETTING_PATH=ISIMIP/LOCAL/"$site"_settings_ISIMIP_Manag-"$man"_CO2-on.txt
		
			#add esm and rcp to meteo co2 and soil path
			MET_PATH=ISIMIP/LOCAL/hist.txt
			SOIL_PATH=ISIMIP/"$site"_soil_ISIMIP.txt
			CO2_PATH=ISIMIP/CO2/CO2_hist.txt
							
			#log arguments paths
			echo "*****************************"
			echo "$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH"
			echo "$MODEL $VERSION-ISIMIP arguments:"
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
			
			#add paths and arguments to executable and run
			$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH #&
	
		done		
	done	
}
#########################################################################################################
############################################### 2A ######################################################
#########################################################################################################

function 2A_runs {
	for (( b = 0 ; b < $site_counter ; ++b )) ; do
		for (( c = 0 ; c < $esm_counter ; ++c )) ; do
			for (( e = 0 ; e < $man_counter ; ++e )) ; do
				
				if (( $site_counter > 1 )) ; then site=${SITEs[$b]}; fi
				if (( $esm_counter  > 1 )) ; then esm=${ESMs[$c]}; fi
				if (( $man_counter  > 1 )) ; then man=${MANs[$e]}; fi
				
				echo "*****************************"
				echo 'running for' "$exp"
				echo 'running for' "$site"
				echo 'running for' "$climate"
				echo 'running for' "$esm"
				echo 'running with management =' "$man" 
				echo "*****************************"
									
				#add site name to current paths
				SITE_PATH=input/"$site"
				OUTPUT_PATH=output/ISIMIP_OUTPUT/2A/"$site"
				STAND_PATH=ISIMIP/"$site"_stand_ISIMIP.txt
				TOPO_PATH=ISIMIP/"$site"_topo_ISIMIP.txt
	
				SETTING_PATH=ISIMIP/2A/"$site"_settings_ISIMIP_Manag-"$man"_CO2-on.txt
			
				#add esm and rcp to meteo co2 and soil path
				MET_PATH=ISIMIP/2A/"$esm"/"$esm"_hist.txt
				SOIL_PATH=ISIMIP/"$site"_soil_ISIMIP.txt
				CO2_PATH=ISIMIP/CO2/CO2_hist.txt
				
				#log arguments paths
				echo "*****************************"
				echo "$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH"
				echo "$MODEL $VERSION-ISIMIP arguments:"
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
				
				#add paths and arguments to executable and run
				$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH #&

			done
		done
	done

}
#########################################################################################################
############################################### 2B ######################################################
#########################################################################################################

function 2B_runs {
	for (( b = 0 ; b < $site_counter ; ++b )) ; do
		for (( c = 0 ; c < $esm_counter ; ++c )) ; do
			for (( d = 0 ; d < $rcp_counter ; ++d )) ; do
				for (( e = 0 ; e < $man_counter ; ++e )) ; do
					for (( f = 0 ; f < $co2_counter ; ++f )) ; do
						
						if (( $site_counter > 1 )) ; then site=${SITEs[$b]}; fi
						if (( $esm_counter  > 1 )) ; then esm=${ESMs[$c]}; fi
						if (( $rcp_counter  > 1 )) ; then rcp=${RCPs[$d]}; fi
						if (( $man_counter  > 1 )) ; then man=${MANs[$e]}; fi
						if (( $co2_counter  > 1 )) ; then co2=${CO2s[$f]}; fi
						
						echo "*****************************"
						echo 'running for' "$exp"
						echo 'running for' "$site"
						echo 'running for' "$climate"
						echo 'running for' "$esm"
						echo 'running for' "$rcp"
						echo 'running with management =' "$man" 
						echo 'running with co2 =' "$co2"
						echo "*****************************"
											
						#add site name to current paths
						SITE_PATH=input/"$site"
						OUTPUT_PATH=output/ISIMIP_OUTPUT/2B/"$site"
						STAND_PATH=ISIMIP/"$site"_stand_ISIMIP.txt
						TOPO_PATH=ISIMIP/"$site"_topo_ISIMIP.txt
			
						SETTING_PATH=ISIMIP/2B/"$site"_settings_ISIMIP_Manag-"$man"_CO2-"$co2".txt
					
						#add esm and rcp to meteo co2 and soil path
						MET_PATH=ISIMIP/2B/"$esm"/"$esm"_"$rcp".txt
						SOIL_PATH=ISIMIP/"$site"_soil_ISIMIP.txt
						CO2_PATH=ISIMIP/CO2/CO2_"$rcp".txt
										
						#add paths and arguments to executable and run
						$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH #&
						
						#log arguments paths
						echo "*****************************"
						echo "$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH"
						echo "$MODEL $VERSION-ISIMIP arguments:"
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
############################################### 2B pico ###################################################
function 2Bpico_runs {
	for (( b = 0 ; b < $site_counter ; ++b )) ; do
		for (( c = 0 ; c < $esm_counter ; ++c )) ; do
			for (( e = 0 ; e < $man_counter ; ++e )) ; do
				
				if (( $site_counter > 1 )) ; then site=${SITEs[$b]}; fi
				if (( $esm_counter  > 1 )) ; then esm=${ESMs[$c]}; fi
				if (( $man_counter  > 1 )) ; then man=${MANs[$e]}; fi
					
				echo "*****************************"
				echo 'running for' "$exp"
				echo 'running for' "$site"
				echo 'running for' "$climate"
				echo 'running for' "$esm"
				echo 'running with management =' "$man" 
				echo "*****************************"
									
				#add site name to current paths
				SITE_PATH=input/"$site"
				OUTPUT_PATH=output/ISIMIP_OUTPUT/2Bpico/"$site"
				STAND_PATH=ISIMIP/"$site"_stand_ISIMIP.txt
				TOPO_PATH=ISIMIP/"$site"_topo_ISIMIP.txt
	
				SETTING_PATH=ISIMIP/2Bpico/"$site"_settings_ISIMIP_Manag-"$man"_CO2-off.txt
			
				#add esm and rcp to meteo co2 and soil path
				MET_PATH=ISIMIP/2Bpico/"$esm"/"$esm"_pico.txt
				SOIL_PATH=ISIMIP/"$site"_soil_ISIMIP.txt
				CO2_PATH=ISIMIP/CO2/CO2_hist.txt
				
				#log arguments paths
				echo "*****************************"
				echo "$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH"
				echo "$MODEL $VERSION-ISIMIP arguments:"
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
				
				#add paths and arguments to executable and run
				$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH #&

			done
		done
	done
}
#########################################################################################################
############################################## 2BLB #####################################################
#########################################################################################################
#
function 2BLBC_runs {
	for (( b = 0 ; b < $site_counter ; ++b )) ; do
		for (( c = 0 ; c < $esm_counter ; ++c )) ; do
			for (( d = 0 ; d < $rcp_counter ; ++d )) ; do
				for (( e = 0 ; e < $man_counter ; ++e )) ; do
					for (( f = 0 ; f < $co2_counter ; ++f )) ; do
					
					if (( $site_counter > 1 )) ; then site=${SITEs[$b]}; fi
					if (( $esm_counter  > 1 )) ; then esm=${ESMs[$c]}; fi
					if (( $rcp_counter  > 1 )) ; then rcp=${RCPs[$d]}; fi
					if (( $man_counter  > 1 )) ; then man=${MANs[$e]}; fi
					if (( $co2_counter  > 1 )) ; then co2=${CO2s[$f]}; fi
					
					echo "*****************************"
					echo 'running for' "$exp"
					echo 'running for' "$site"
					echo 'running for' "$climate"
					echo 'running for' "$esm"
					echo 'running for' "$rcp"
					echo 'running with management =' "$man" 
					echo 'running with co2 =' "$co2"
					echo "*****************************"
										
					#add site name to current paths
					SITE_PATH=input/"$site"
					OUTPUT_PATH=output/ISIMIP_OUTPUT/2BLBC/"$site"
					STAND_PATH=ISIMIP/"$site"_stand_ISIMIP.txt
					TOPO_PATH=ISIMIP/"$site"_topo_ISIMIP.txt
		
					SETTING_PATH=ISIMIP/2BLBC/"$site"_settings_ISIMIP_Manag-"$man"_CO2-"$co2".txt
				
					#add esm and rcp to meteo co2 and soil path
					MET_PATH=ISIMIP/2BLBC/"$esm"/"$esm"_"$rcp".txt
					SOIL_PATH=ISIMIP/"$site"_soil_ISIMIP.txt
					CO2_PATH=ISIMIP/CO2/CO2_"$rcp".txt
					
					#log arguments paths
					echo "*****************************"
					echo "$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH"
					echo "$MODEL $VERSION-ISIMIP arguments:"
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
					
					#add paths and arguments to executable and run
					$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH #&

					done
				done
			done
		done
	done
}
############################################### 2BLBC pico ###################################################
function 2BLBCpico_runs {
	for (( b = 0 ; b < $site_counter ; ++b )) ; do
		for (( c = 0 ; c < $esm_counter ; ++c )) ; do
			for (( e = 0 ; e < $man_counter ; ++e )) ; do
				
				if (( $site_counter > 1 )) ; then site=${SITEs[$b]}; fi
				if (( $esm_counter  > 1 )) ; then esm=${ESMs[$c]}; fi
				if (( $man_counter  > 1 )) ; then man=${MANs[$e]}; fi

					
					echo "*****************************"
				echo 'running for' "$exp"
				echo 'running for' "$site"
				echo 'running for' "$climate"
				echo 'running for' "$esm"
				echo 'running with management =' "$man" 
				echo "*****************************"
									
				#add site name to current paths
				SITE_PATH=input/"$site"
				OUTPUT_PATH=output/ISIMIP_OUTPUT/2BLBCpico/"$site"
				STAND_PATH=ISIMIP/"$site"_stand_ISIMIP.txt
				TOPO_PATH=ISIMIP/"$site"_topo_ISIMIP.txt
	
				SETTING_PATH=ISIMIP/2BLBCpico/"$site"_settings_ISIMIP_Manag-"$man"_CO2-off.txt
			
				#add esm and rcp to meteo co2 and soil path
				MET_PATH=ISIMIP/2BLBCpico/"$esm"/"$esm"_pico.txt
				SOIL_PATH=ISIMIP/"$site"_soil_ISIMIP.txt
				CO2_PATH=ISIMIP/CO2/CO2_hist.txt
				
				#log arguments paths
				echo "*****************************"
				echo "$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH"
				echo "$MODEL $VERSION-ISIMIP arguments:"
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
				
				#add paths and arguments to executable and run
				$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH #&

			done
		done
	done
}
##################################################################################################


for (( e = 0 ; e < $exp_counter ; ++e )) ; do
	
	if (( $exp_counter > 1 )) ; then exp=${EXPs[$e]}; fi
	
	#1 simulations for 'LOCAL' experiments
	#LOCAL_runs
	
	#2 simulation for 'FAST TRACK' experimet
	#FT_runs
	
	#3  simulation for '2A' experiment
	#2A_runs
	
	#3  simulation for '2B' experiment
	#2B_runs

	#4 simulation for '2Bpico' experiment
	#2Bpico_runs
	
	#5  simulation for '2BLBC' experiment
	#2BLBC_runs
	
	#6  simulation for '2BLBCpico' experiment
	2BLBCpico_runs
done

#########################################################################################################

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
