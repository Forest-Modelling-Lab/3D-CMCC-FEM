#!/bin/bash
### File information summary  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
# Description:    createInputForSE.sh bash shell script
#                 Processing chain to create input images for Forest Scenarios Evolution project (ForSE).
# Author:         Alessandro Candini - candini@meeo.it
# Version:        0.1
# Copyright:      MEEO S.R.L. - www.meeo.it 
# How it works:   It takes...
# Launch example: ./createInputForSE.sh
# Changelog:      2012-11-06 - version 0.1
#                     - First Release
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  File information summary }

### Global variables definitions  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
VERSION="0.1"
BIN_DIR="$( dirname ${0} )/../bin"
MODULES=(remap getLAI applyMask)
IMG_ALL=(Y_planted Species Phenology Management NumHa AvDBH Height Wf Wrc Ws SolarRad Avg_Temp VPD Precip LAI)
IMG_SELECTED=()

# DEBUG="n" --> clean the current working directory
# DEBUG="y" --> do not clean the current working directory
DEBUG="n"
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global variables definitions }

### Global functions definitions  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
usage(){
	echo ""
	echo "Usage: ${0} [IMG_TO_PROCESS]"
	echo "       - IMG_TO_PROCESS is an optional array to define which images to process. If omitted, every image will be created."
	echo "       - Accepted values for the array are (every other value it is ignored):"
	echo "             Y_planted, Species, Phenology, Management, NumHa, AvDBH, Height, Wf, Wrc, Ws, SolarRad, Avg_Temp, VPD, Precip, LAI"
	echo "       - NOTE: Remeber to set up and fill correctly the BIN_DIR, directory where the C modules are stored."
	echo ""
	exit 40
}

log() {
	echo -en "$(date +"%Y-%m-%d %H:%M:%S") - ${1}" >> "${LOGFILE}"
}

error() {
	# Copy the log file (with reported error) into output directory
	cp "${LOGFILE}" "${WORK_DIR}/${LOGFILENAME}.err"
	if [ ${?} -ne "0" ] ; then
		echo "Copy of logfile into "${OUTPUT_DIR}" failed\n"
		exit 4
	fi

	# Clean input dir
	if [ ${DEBUG} == "n" ] ; then
		rm -r ${WORK_DIR}/*
		if [ ${?} -ne "0" ] ; then
			echo "Cleaning "${WORK_DIR}" failed\n"
			exit 4
		fi
	fi

	exit 4
}
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global functions definitions }

### Checking input arguments and configurations - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
if [ ! -d "${BIN_DIR}" ]; then
	usage
fi

for I in ${MODULES[@]} ; do
	if [ ! -x "${BIN_DIR}/${I}" ] ; then
		echo "Module ${I} is missing. Exiting."
		exit 4
	fi
done

# Check if DEBUG is setted to "y" or "n": no other values accepted
if [ ${DEBUG} != "y" ] && [ ${DEBUG} != "n" ] ; then
	usage
fi

# Check input parameters
if [ -z ${1} ] ; then # If I have no input parameters, create every image
	for IMG in "${IMG_ALL[@]}" ; do
		IMG_SELECTED+=("${IMG}")
	done
else
	for PARAM in "${@}" ; do
		for IMG in "${IMG_ALL[@]}" ; do
			if [ "${IMG}" == "${PARAM}" ] ; then
    			IMG_SELECTED+=("${PARAM}")
    		fi
		done
	done
fi
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Checking input arguments and configurations }

### Y_planted execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Y_planted" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Y_planted execution }

### Species execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Species" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Species execution }

### Phenology execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Phenology" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Phenology execution }

### Management execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Management" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Management execution }

### NumHa execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "NumHa" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - NumHa execution }

### AvDBH execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "AvDBH" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - AvDBH execution }

### Height execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Height" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Height execution }

### Wf execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Wf" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Wf execution }

### Wrc execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Wrc" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Wrc execution }

### Ws execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Ws" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Ws execution }

### SolarRad execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "SolarRad" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  SolarRad execution }

### Avg_Temp execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Avg_Temp" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Avg_Temp execution }

### VPD execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "VPD" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - VPD execution }

### Precip execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Precip" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Precip execution }

### LAI execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "LAI" ] ; then
    	echo "Go ${IMG}!"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - LAI execution }

exit 0