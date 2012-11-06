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

# DEBUG="n" --> clean the current working directory
# DEBUG="y" --> do not clean the current working directory
DEBUG="n"
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global variables definitions }

### Global functions definitions  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
usage(){
	echo ""
	echo "Usage: ${0}"
	#echo "	- INPUTDIR is the directory in which the image is supposed to be"
	#echo "	- IMAGE is the zipped AVNIR2 L1B2G image to process"
	#echo "Remeber to set up correctly the BIN_DIR, directory where the C modules are stored"
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
if [ ! -d "${BIN_DIR}" ] || [ -z ${1} ] || [ -z ${2} ] || [ ! -d ${1} ] ; then
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
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Checking input arguments and configurations }
