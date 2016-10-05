#!/bin/bash
# Developed by Alessio Collalti (alessio.collalti@cmcc.it)
# starting date: 5 October 2016


CMCC="3D_CMCC_Forest_Model"
VERSION="v.5.2.2"
PROJECT="ISIMIP"

echo "**************************************"
echo "$CMCC $VERSION script for $PROJECT run"
echo "**************************************"

#input paths
SITE_PATH=input/
PARAMETERIZATION_PATH=input/parameterization

#output_paths
export OUTPUT_PATH=output/


#run execution
model_debug_run="3D_CMCC_Forest_Model"
model_release_run="3D_CMCC_Forest_Model"

#declare sites
site[0]=Soroe
site[1]=Kroof
site[2]=Peitz

#declare climate
CLIMATE[0]=Historical
CLIMATE[1]=Scenario

#declare historical
HYSTs[0]=CLIMATE
HYSTs[1]=PRINCETON
HYSTs[2]=WATCH
HYSTs[3]=GSWP3
HYSTs[4]=WATCH-WFDEI

#declare GCMs
GCMs[0]=GCM1
GCMs[1]=GCM2
GCMs[2]=GCM3
GCMs[3]=GCM4
GCMs[4]=GCM5

#declare RCPs
RCPs[0]=rcp8p5
RCPs[1]=rcp6p0
RCPs[2]=rcp4p5
RCPs[3]=rcp2p6

#declare Management
Man[0]=on
Man[1]=off

#declare CO2 enrichment
CO2[0]=on
CO2[1]=off



#########################################################################################################
#log available sites
echo 'available sites:'
for (( i = 0 ; i <= 2 ; ++i )) ; do
      echo -"${site[i]}"
done

echo "which site do you want to simulate (case sensitive)?"

#ask which site use
read site_name

echo "site to simulate '$site_name'"

match=no

#check if red site merge with available sites
for (( i = 0 ; i <= 2 ; ++i )) ; do

     if [ "$site_name" == "${site[$i]}" ] ; then
      match=yes      
     fi
done

#exit if none site name merge with site list
if [ "$match" == "no" ] ; then
      echo $site_name "doesn't match with sites list exit program!"
exit
fi

#########################################################################################################
#log available CLIMATE
echo 'available CLIMATE:'
for (( i = 0 ; i <= 1 ; ++i )) ; do
      echo -"${CLIMATE[i]}"
done

echo "which CLIMATE do you want to use for '$site_name' (case sensitive)?"

#ask which CLIMATE to use
read Climate

echo "CLIMATE to use '$Climate'"

match=no

#check if red GCM merge with available CLIMATE
for (( i = 0 ; i <= 1 ; ++i )) ; do

     if [ "$Climate" == "${CLIMATE[$i]}" ] ; then
      match=yes      
     fi
done

#exit if none RCP name merge with RCP list
if [ "$match" == "no" ] ; then
      echo $Climate "doesn't match with CLIMATE list exit program!"
exit
fi

#assign value to case depending on climate
if [ "$Climate" == "Historical" ] ; then
		case=0
else
		case=1
fi

#########################################################################################################
if [ "$case" == 0 ] ; then
		#log available Historical
		echo 'available Historical:'
		for (( i = 0 ; i <= 4 ; ++i )) ; do
				echo -"${HYSTs[i]}"
		done

		echo "which Historical do you want to use for '$site_name' (case sensitive)?"

		#ask which HYSTs to use
		read Hist

		echo "Historical to use '$Hist'"

		match=no

		#check if red Hist merge with available HYSTs
		for (( i = 0 ; i <= 4 ; ++i )) ; do

			  if [ "$Hist" == "${HYSTs[$i]}" ] ; then
				match=yes     
			  fi
		done

		#exit if none Hist merge with HYSTs list
		if [ "$match" == "no" ] ; then
				echo $Hist "doesn't match with Historical list exit program!"
		exit
		fi
else
		#log available GCMs
		echo 'available GCMs:'
		for (( i = 0 ; i <= 4 ; ++i )) ; do
				echo -"${GCMs[i]}"
		done

		echo "which GCMs do you want to use for '$site_name' (case sensitive)?"

		#ask which GCM to use
		read GCM

		echo "GCM to use '$GCM'"

		match=no

		#check if red GCM merge with available GCMs
		for (( i = 0 ; i <= 4 ; ++i )) ; do

			  if [ "$GCM" == "${GCMs[$i]}" ] ; then
				match=yes     
			  fi
		done

		#exit if none GCM merge with GCM list
		if [ "$match" == "no" ] ; then
				echo $GCM "doesn't match with GCMs list exit program!"
		exit
		fi

		#log available RCPs
		echo 'available RCPs:'
		for (( i = 0 ; i <= 3 ; ++i )) ; do
				echo -"${RCPs[i]}"
		done

		echo "which RCPs do you want to use for '$site_name' and '$GCM' (case sensitive)?"

		#ask which RCP to use
		read RCP

		echo "RCPs to use '$RCP'"

		match=no

		#check if red GCM merge with available GCMs
		for (( i = 0 ; i <= 3 ; ++i )) ; do

			  if [ "$RCP" == "${RCPs[$i]}" ] ; then
				match=yes      
			  fi
		done

		#exit if none RCP name merge with RCP list
		if [ "$match" == "no" ] ; then
				echo $RCP "doesn't match with RCPs list exit program!"
		exit
		fi
fi

#########################################################################################################
#log available Management
echo 'Management on or off?:'
for (( i = 0 ; i <= 1 ; ++i )) ; do
      echo -"${Man[i]}"
done

echo "Management on or off for '$site_name' and '$GCM' and '$RCP' (case sensitive)?"

#ask if use management
read Management

echo "Management is '$Management'"

match=no

#check if red GCM merge with available GCMs
for (( i = 0 ; i <= 1 ; ++i )) ; do

     if [ "$Management" == "${Man[$i]}" ] ; then
      match=yes      
     fi
done

#exit if none RCP name merge with RCP list
if [ "$match" == "no" ] ; then
      echo $Man "doesn't match with Management possibilities list exit program!"
exit
fi

#########################################################################################################
#log available CO2
echo 'CO2 enrichment on or off?:'
for (( i = 0 ; i <= 1 ; ++i )) ; do
      echo -"${CO2[i]}"
done

echo "CO2 enrichment on or off for '$site_name' and '$GCM' and '$RCP' and Management '$Management'(case sensitive)?"

#ask if use management
read CO2enrich

echo "CO2 enrichment is '$CO2enrich'"

match=no

#check if red GCM merge with available GCMs
for (( i = 0 ; i <= 1 ; ++i )) ; do

     if [ "$CO2enrich" == "${CO2[$i]}" ] ; then
      match=yes      
     fi
done

#exit if none RCP name merge with RCP list
if [ "$match" == "no" ] ; then
      echo $CO2enrich "doesn't match with CO2 enrichment possibilities list exit program!"
exit
fi

#########################################################################################################

if [ "$case" == 0 ] ; then
		MET_PATH=ISIMIP/Historical/"$Hist"_1960-2001.txt
		SOIL_PATH=ISIMIP/Historical/"$site_name"_soil_"$Hist"_ISIMIP.txt
		CO2_PATH=ISIMIP/CO2/CO2_historical_1901_2012.txt
else
		MET_PATH=ISIMIP/"$GCM"/"$GCM"_hist_"$RCP"_1960_2099.txt
		SOIL_PATH=ISIMIP/"$GCM"/"$site_name"_soil_"$RCP"_ISIMIP.txt
		CO2_PATH=ISIMIP/CO2/CO2_"$RCP"_1950_2099.txt
fi

#add site name to current paths
SITE_PATH=input/$site_name
OUTPUT_PATH=output/$site_name
STAND_PATH=ISIMIP/"$site_name"_stand_1960_ISIMIP.txt
TOPO_PATH=ISIMIP/"$site_name"_topo_ISIMIP.txt

#setting path
SETTING_PATH=ISIMIP/"$site_name"_settings_ISIMIP_Manag-"$Management"_CO2-"$CO2enrich".txt

DEBUG_OUTPUT=$OUTPUT_PATH/debug_output/debug_output
DAILY_OUTPUT=$OUTPUT_PATH/daily_output/daily_output
MONTHLY_OUTPUT=$OUTPUT_PATH/monthly_output/monthly_output
ANNUAL_OUTPUT=$OUTPUT_PATH/annual_output/annual_output
SOIL_OUTPUT=$OUTPUT_PATH/soil_output/soil_output

#log input folder path
echo "path $site_name input data for $PROJECT project: $SITE_PATH"

#go to debug folder to execute debug run for site
cd Debug

cp 3D_CMCC_Forest_Model ../

cd ..

#log argumets paths
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
echo -o $DEBUG_OUTPUT
echo -b $DAILY_OUTPUT
echo -f $MONTHLY_OUTPUT
echo -e $ANNUAL_OUTPUT
echo -n $SOIL_OUTPUT
echo "*****************************"

#add paths and arguments to executable
./3D_CMCC_Forest_Model  -i $SITE_PATH -p $PARAMETERIZATION_PATH -o $DEBUG_OUTPUT -b $DAILY_OUTPUT -e $ANNUAL_OUTPUT -f $MONTHLY_OUTPUT -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH -n $SOIL_OUTPUT
