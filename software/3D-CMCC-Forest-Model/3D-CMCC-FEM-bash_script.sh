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
site[0]=All
site[1]=Soroe
site[2]=Kroof
site[3]=Peitz

#declare climate
CLIMATE[0]=All
CLIMATE[1]=Historical
CLIMATE[2]=Scenario

#declare historical
HYSTs[0]=All
HYSTs[1]=CLIMATE
HYSTs[2]=PRINCETON
HYSTs[3]=WATCH
HYSTs[4]=GSWP3
HYSTs[5]=WATCH-WFDEI

#declare GCMs
GCMs[0]=All
GCMs[1]=GCM1
GCMs[2]=GCM2
GCMs[3]=GCM3
GCMs[4]=GCM4
GCMs[5]=GCM5

#declare RCPs
RCPs[0]=All
RCPs[1]=rcp8p5
RCPs[2]=rcp6p0
RCPs[3]=rcp4p5
RCPs[4]=rcp2p6

#declare Management
Man[0]=All
Man[1]=on
Man[2]=off

#declare CO2 enrichment
CO2[0]=All
CO2[1]=on
CO2[2]=off

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
	
	
	
fi
exit 



#########################################################################################################
#log available sites
echo 'available sites:'
for (( i = 0 ; i < ${#site[@]} ; ++i )) ; do
      echo -"${site[i]}"
done

echo "which site do you want to simulate?"

#ask which site use
read site_name

echo "site to simulate '$site_name'"

match=no

#check if red site merge with available sites
for (( i = 0 ; i <= ${#site[@]} ; ++i )) ; do

    if [ "${site_name,,}" = "${site[$i],,}" ] ; then
    	match=yes
		site_name=${site[$i]}
     fi
done

#exit if none site name merge with site list
if [ "$match" == "no" ] ; then
      echo $site_name "doesn't match with sites list exit program!"
exit
fi

#check for available simulation years


#########################################################################################################
#log available CLIMATE
echo 'available CLIMATE:'
for (( i = 0 ; i < ${#CLIMATE[@]} ; ++i )) ; do
      echo -"${CLIMATE[i]}"
done

echo "which CLIMATE do you want to use for '$site_name'?"

#ask which CLIMATE to use
read Climate

echo "CLIMATE to use '$Climate'"

match=no

#check if red GCM merge with available CLIMATE
for (( i = 0 ; i < ${#CLIMATE[@]}  ; ++i )) ; do

	if [ "${Climate,,}" == "${CLIMATE[$i],,}" ] ; then
	match=yes     
	Climate=${CLIMATE[$i]} 
	fi
done

#exit if none RCP name merge with RCP list
if [ "$match" == "no" ] ; then
      echo $Climate "doesn't match with CLIMATE list exit program!"
exit
fi

#########################################################################################################
#All climates
if [ "$Climate" == "${CLIMATE[0]}" ] ; then
	echo 'All climate'
	exit
fi

#Historical
if [ "$Climate" == "${CLIMATE[1]}" ] ; then
	
		#log available Historical
		echo 'available Historical:'
		for (( i = 0 ; i < ${#HYSTs[@]} ; ++i )) ; do
				echo -"${HYSTs[i]}"
		done

		echo "which Historical do you want to use for '$site_name'?"

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
fi
#Scenario
if [ "$Climate" == "${CLIMATE[2]}" ] ; then
		#log available GCMs
		echo 'available GCMs:'
		for (( i = 0 ; i < ${#GCMs[@]} ; ++i )) ; do
				echo -"${GCMs[i]}"
		done

		echo "which GCMs do you want to use for '$site_name'?"

		#ask which GCM to use
		read GCM

		echo "GCM to use '$GCM'"

		match=no

		#check if red GCM merge with available GCMs
		for (( i = 0 ; i < ${#GCMs[@]} ; ++i )) ; do

			  if [ "${GCM,,}" == "${GCMs[$i],,}" ] ; then
				match=yes
				GCM=${GCMs[$i]}
			  fi
		done

		#exit if none GCM merge with GCMs list
		if [ "$match" == "no" ] ; then
				echo $GCM "doesn't match with GCMs list exit program!"
		exit
		fi

		#log available RCPs
		echo 'available RCPs:'
		for (( i = 0 ; i < ${#RCPs[@]} ; ++i )) ; do
				echo -"${RCPs[i]}"
		done

		echo "which RCPs do you want to use for '$site_name' and '$GCM'?"

		#ask which RCP to use
		read RCP

		echo "RCPs to use '$RCP'"

		match=no

		#check if red GCM merge with available GCMs
		for (( i = 0 ; i < ${#RCPs[@]} ; ++i )) ; do

			  if [ "${RCP,,}" == "${RCPs[$i],,}" ] ; then
				match=yes
				RCP=${RCPs[$i]}
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
for (( i = 0 ; i < ${#Man[@]} ; ++i )) ; do
      echo -"${Man[i]}"
done

echo "Management on or off for '$site_name' and '$GCM' and '$RCP'?"

#ask if use management
read Management

echo "Management is '$Management'"

match=no

#check if red management options merge with available management
for (( i = 0 ; i < ${#Man[@]} ; ++i )) ; do

     if [ "${Management,,}" == "${Man[$i],,}" ] ; then
      match=yes
    Management=${Man[$i]}
     fi
done

#exit if none management options merge with management list
if [ "$match" == "no" ] ; then
      echo $Man "doesn't match with Management possibilities list exit program!"
exit
fi

#########################################################################################################
#log available CO2
echo 'CO2 enrichment on or off?:'
for (( i = 0 ; i < ${#CO2[@]} ; ++i )) ; do
      echo -"${CO2[i]}"
done

echo "CO2 enrichment on or off for '$site_name' and '$GCM' and '$RCP' and Management '$Management'?"

#ask if use CO2 enrichment
read CO2enrich

echo "CO2 enrichment is '$CO2enrich'"

match=no

#check if red CO2 options merge with available CO2 list 
for (( i = 0 ; i < ${#CO2[@]} ; ++i )) ; do

     if [ "${CO2enrich,,}" == "${CO2[$i],,}" ] ; then
      match=yes      
    CO2enrich=${CO2[$i]}
     fi
done

#exit if none CO2 options merge with CO2 list
if [ "$match" == "no" ] ; then
      echo $CO2enrich "doesn't match with CO2 enrichment possibilities list exit program!"
exit
fi

#########################################################################################################
#to complete
if [ "$Climate" == "${CLIMATE[1]}"] ; then
		MET_PATH=ISIMIP/Historical/"$Hist"_1960-2001.txt
		SOIL_PATH=ISIMIP/Historical/"$site_name"_soil_"$Hist"_ISIMIP.txt
		CO2_PATH=ISIMIP/CO2/CO2_historical_1901_2012.txt
fi
if [ "$Climate" == "${CLIMATE[2]}"] ; then
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


#log input folder path
echo "path $site_name input data for $PROJECT project: $SITE_PATH"

#go to debug folder to execute debug run for site
cd Debug

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

#add paths and arguments to executable
./3D_CMCC_Forest_Model -i $SITE_PATH -p $PARAMETERIZATION_PATH -d $STAND_PATH -m $MET_PATH -s $SOIL_PATH -t $TOPO_PATH -c $SETTING_PATH -k $CO2_PATH 

#delete copied executable from current directory
rm 3D_CMCC_Forest_Model

echo "script finished!"

#exit program
exit