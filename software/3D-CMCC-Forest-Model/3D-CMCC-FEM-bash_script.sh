#!/bin/bash
# Developed by:
# Alessio Collalti (alessio.collalti@cmcc.it)
# Alessio Ribeca (alessio.ribeca@cmcc.it)
# starting date: 14 October 2016

MODEL="3D-CMCC-CNR FEM"
VERSION="v.5.2.2"

echo "****************************************************"
echo "* $MODEL $VERSION script runs in "$OSTYPE" *"
echo "****************************************************"

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
SITEs=(*)

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

#########################################################################################################
#log available Input data
#assign for a single site a starting year

cd input 
	
echo "$site available stand initialization year(s):"

cd "$site"

# stand data 
if [ -d stand ] ; then

	cd stand
	
	#find among *.txt files occurrence for "stand" and "year" and put in array
	STAND_YEARs=($(find *.txt | ( grep "stand" | sed -e s/[^0-9]//g )))
	cd ..
else	
	#find among *.txt files occurrence for "stand" and "year" and put in array
	STAND_YEARs=($(find *.txt | ( grep "stand" | sed -e s/[^0-9]//g )))
fi

for (( i = 0 ; i < ${#STAND_YEARs[@]} ; ++i )) ; do
	echo -"${STAND_YEARs[i]}"
done

#log available year data ask which year to use
match=no
echo "which is the starting year for "$site" to simulate?"
while :
	do
	read stand_year
	for (( i = 0 ; i < ${#STAND_YEARs[@]} ; ++i )) ; do
		if [ "${stand_year,,}" = "${STAND_YEARs[$i],,}" ] ; then
			match=yes
			stand_year=${STAND_YEARs[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi
	
	echo "$stand_year doesn't match with year list. please rewrite it."
done
	
echo "starting year for $site = $stand_year"

###################################################################################

#meteo data
cd meteo

echo "$site available meteo data:"

if [ -d meteo ] ; then

	cd meteo
	
	#find among *.txt files occurrence for "stand" and "year" and put in array
	METEO_YEARs=($(find *.txt | ( grep "meteo" | sed -e s/[^0-9]//g )))
	cd ..
else	
	#find among *.txt files occurrence for "stand" and "year" and put in array
	METEO_YEARs=($(find *.txt | ( grep "meteo" | sed -e s/[^0-9]//g )))
fi

for (( i = 0 ; i < ${#METEO_YEARs[@]} ; ++i )) ; do
	echo -"${METEO_YEARs[i]}"
done

#log available meteo year data ask which year to use
match=no
echo "which is the starting meteo year for "$site" to simulate?"
while :
	do
	read meteo_year
	for (( i = 0 ; i < ${#METEO_YEARs[@]} ; ++i )) ; do
		if [ "${meteo_year,,}" = "${METEO_YEARs[$i],,}" ] ; then
			match=yes
			meteo_year=${METEO_YEARs[$i]}
		fi
	done
	if [ "$match" == "yes" ] ; then
		break;
	fi
	
	echo "$meteo_year doesn't match with meteo year list. please rewrite it."
done
	
echo "starting year for $site init year $stand_year = $meteo_year"

#check if stand year matches with meteo data
if [ $stand_year != $meteo_year ] ; then
	echo "$stand_year doesn't match with $meteo_year (exit)"
	exit
fi

#########################################################################################################
#########################################################################################################
#########################################################################################################

# compute elapsed time
START=`date +%s%N`

#backforward to correct dir
cd ../../..

echo "running for $site"

function single_run {
	
	echo "single run"

	
	#add site name to current paths
	SITE_PATH=input/"$site"
	OUTPUT_PATH=output/"$site"
	STAND_PATH=stand/"$site"_stand_"$stand_year".txt
	TOPO_PATH=stand/"$site"_topo.txt

	#add setting path
	SETTING_PATH="$site"_settings.txt

	#add gcm and rcp to meteo co2 and soil path
	MET_PATH=meteo/"$site"_meteo_"$meteo_year".txt
	SOIL_PATH=stand/"$site"_soil.txt
	
	#add paths and arguments to executable and run
	$launch$executable -i $SITE_PATH -o $OUTPUT_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH 
	
	#log arguments paths
	echo "*****************************"
	echo "$MODEL $VERSION-$PROJECT arguments:"
	echo "-i" $SITE_PATH
	echo "-p" $PARAMETERIZATION_PATH
	echo "-d" $STAND_PATH
	echo "-s" $SOIL_PATH
	echo "-t" $TOPO_PATH
	echo "-m" $MET_PATH
	echo "-c" $SETTING_PATH
	echo "-o" $OUTPUT_PATH
	echo "*****************************"
}

#launch single run
single_run

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
