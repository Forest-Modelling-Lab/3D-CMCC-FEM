#!/bin/bash 
# MEEO - www.meeo.it
# Developed by Alessandro Candini (candini@meeo.it)

PROGNAME="$( basename ${0} )"
BIN="../$( dirname ${0} )/bin"
CMCC="3D_CMCC_Forest_Model"
MODULES=(getInputCMCC wrapCMCC "${CMCC}"  getOutputCMCC mergeImg)

usage() {
	echo "Usage: ${PROGNAME} INPUTDATASET RESOLUTION OUTPUTDIR [PREFIX]"
	echo " - INPUTDATASET is the tar.bz2 of the dataset to be processed, given with absolute path."
	echo " - REOLUTION must be 10 or 100 and means at which resolution the CMCC model has to work (in meters)."
	echo " - OUTPUTDIR is the output directory given with absolute path; be shure that it exists."
	echo " - PREFIX is an optional parameter to set the beginning of output filename."
	exit 1
}

[ -z "${1}" ] && usage
[ -z "${2}" ] && usage
[ -z "${3}" ] && usage

OUTPUTDIR="${3}"
if [ ! -d "${OUTPUTDIR}" ] ; then
	echo "Input parameters error: Invalid output directory"
	exit 2
fi
# If the output dir terminates with a "/", delete it
SLASH=$( echo ${OUTPUTDIR:`expr ${#OUTPUTDIR} - 1`} )
if [ ${SLASH} == "/"  ] ; then
	OUTPUTDIR=$( echo ${OUTPUTDIR} | sed s/.$// )
fi

# Working directory with pid of the process as name
SUBDIR="dir"$$""

LOGFILE="${OUTPUTDIR}/${CMCC}-${2}m.log"
log() {
	echo -e "$(date +"%Y-%m-%d %H:%M:%S") - ${1}" >> "${LOGFILE}"
}
log "${PROGNAME} started"

for I in ${MODULES[@]} ; do
	if [ ! -x "${BIN}/${I}" ] ; then
		echo "Module ${I} is missing. Exiting."
		exit 51
	fi
done

INPUTDATASET="${1}"
# Check if input dataset archive exists
if [ ! -f "${INPUTDATASET}" ] ; then
	log "Input dataset not found..."
	echo "Input dataset not found..."
	exit 2
fi

DATASETNAME="$( basename "${INPUTDATASET}" | cut -f '1' -d '.' )"

RESOLUTION="${2}"
if [ ${RESOLUTION} -ne "10" ] && [ ${RESOLUTION} -ne "100" ] ; then
	log "Input parameters error: resolution (2nd parameter) must be 10 or 100"
	echo "Input parameters error: resolution (2nd parameter) must be 10 or 100"
	exit 2
fi

PREFIX="${4}"

# Exporting boost libraries needed by some module
export LD_LIBRARY_PATH=/home/sistema/lib:${LD_LIBRARY_PATH}

# Real working directory (outputDir/pidNamedDir)
WORKDIR="${OUTPUTDIR}/${SUBDIR}"
WORK_IN="${WORKDIR}/01_getInputCMCC"
WORK_CMCC="${WORKDIR}/02_${CMCC}"
WORK_OUT="${WORKDIR}/03_getOutputCMCC"

mkdir "${WORKDIR}"
if [ ! -d "${WORKDIR}" ] ; then
	log "Invalid working directory."
	echo "Input parameters error: Invalid working directory"
	exit 40
fi

mkdir "${WORK_IN}"
if [ ! -d "${WORK_IN}" ] ; then
	log "Invalid getInputCMCC working directory."
	echo "Input parameters error: Invalid getInputCMCC working directory"
	exit 40
fi

mkdir "${WORK_CMCC}"
if [ ! -d "${WORK_CMCC}" ] ; then
	log "Invalid wrapCMCC working directory."
	echo "Input parameters error: Invalid wrapCMCC working directory"
	exit 40
fi

mkdir "${WORK_OUT}"
if [ ! -d "${WORK_OUT}" ] ; then
	log "Invalid getOutputCMCC working directory."
	echo "Input parameters error: Invalid getOutputCMCC working directory"
	exit 40
fi

# Input directory
INPUTDATASETDIR="${WORKDIR}/${DATASETNAME}"

log "Decompressing input dataset into working dir..."
tar -C ${WORKDIR} -jxf ${INPUTDATASET}
if [ "$?" -ne "0" ] ; then
	log "Cannot decompress input dataset"
	echo "Invalid or corrupted data: cannot decompress input dataset"
	exit 10
fi
log "...done"

LOCATION="$( ls ${INPUTDATASETDIR} )"

# :::::::::::::::::::::: #
# getInputCMCC execution #
# :::::::::::::::::::::: #
IMG_SPEC="\
${INPUTDATASETDIR}/${LOCATION}/images/Y_planted.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Species.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Phenology.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Management.tif \
${INPUTDATASETDIR}/${LOCATION}/images/NumHa.tif \
${INPUTDATASETDIR}/${LOCATION}/images/AvDBH.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Height.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Wf.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Wr.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Ws.tif"

IMG_CLIM="\
${INPUTDATASETDIR}/${LOCATION}/images/SolarRad_2007.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Avg_Temp_2007.tif \
${INPUTDATASETDIR}/${LOCATION}/images/VPD_2007.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Precip_2007.tif \
${INPUTDATASETDIR}/${LOCATION}/images/LAI_2007.tif \
${INPUTDATASETDIR}/${LOCATION}/images/SolarRad_2008.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Avg_Temp_2008.tif \
${INPUTDATASETDIR}/${LOCATION}/images/VPD_2008.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Precip_2008.tif \
${INPUTDATASETDIR}/${LOCATION}/images/LAI_2008.tif \
${INPUTDATASETDIR}/${LOCATION}/images/SolarRad_2009.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Avg_Temp_2009.tif \
${INPUTDATASETDIR}/${LOCATION}/images/VPD_2009.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Precip_2009.tif \
${INPUTDATASETDIR}/${LOCATION}/images/LAI_2009.tif \
${INPUTDATASETDIR}/${LOCATION}/images/SolarRad_2010.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Avg_Temp_2010.tif \
${INPUTDATASETDIR}/${LOCATION}/images/VPD_2010.tif \
${INPUTDATASETDIR}/${LOCATION}/images/Precip_2010.tif \
${INPUTDATASETDIR}/${LOCATION}/images/LAI_2010.tif"

YEARS=(2007 2008 2009 2010)

# Create the year string
for STR in ${YEARS[@]} ; do
	YEARS_STR=${YEARS_STR},${STR}
done

# Delete comma in front of first year
YEARS_STR=${YEARS_STR:1:${#YEARS_STR}}

log "Starting execution of getInputCMCC..."
${BIN}/getInputCMCC -p ${IMG_SPEC} -y ${YEARS_STR} -c ${IMG_CLIM} -o ${WORK_IN}
if [ "$?" -ne "0" ] ; then
	log "Execution of getInputCMCC failed"
	echo "Invalid or corrupted data: execution of getInputCMCC failed"
	exit 10
fi
log "...getInputCMCC exited succesfully."

# :::::::::::::::::: #
# wrapCMCC execution #
# :::::::::::::::::: #

FIRST_SPEC_FILE=$( find ${WORK_IN} -type f | grep '_spec' | head -n 1 )
NUM_PX=$( cat ${FIRST_SPEC_FILE} | wc -l )

SPEC_FILES=( $( find ${WORK_IN} -type f | grep '_spec' | sort ) )
YEAR_FILES=( $( find ${WORK_IN} -type f | grep '[0-9][0-9][0-9][0-9]_b' | sort ) )

for SPEC in ${SPEC_FILES[@]} ; do
	SPEC_STR=${SPEC_STR},${SPEC}
done

for YEAR in ${YEAR_FILES[@]} ; do
	YEAR_STR=${YEAR_STR},${YEAR}
done

SPEC_STR=${SPEC_STR:1:${#SPEC_STR}}
YEAR_STR=${YEAR_STR:1:${#YEAR_STR}}

SITE="${INPUTDATASETDIR}/${LOCATION}/txt/site.txt"

if [ ! -f "${SITE}" ] ; then
    log "Site file for CMCC not found..."
    echo "Site file for CMCC not found..."
    exit 2
fi

# In this case 3D-CMCC is launched in spatial version
VERSION="s"

log "Starting execution of wrapCMCC..."
${BIN}/wrapCMCC -p ${NUM_PX} -y ${YEARS_STR} -yf ${YEAR_STR} -sf ${SPEC_STR} -e "${BIN}/${CMCC}" -i "${INPUTDATASETDIR}/${LOCATION}/txt" -s "${SITE}" -r "${RESOLUTION}" -ve "${VERSION}" -o ${WORK_CMCC} 1> /dev/null
if [ "$?" -ne "0" ] ; then
	log "Execution of wrapCMCC failed"
	echo "Invalid or corrupted data: execution of wrapCMCC failed"
	exit 10
fi

log "...wrapCMCC exited succesfully."

# :::::::::::::::::::::::::::::::::::: #
# getOutputCMCC and mergeImg execution #
# :::::::::::::::::::::::::::::::::::: #

TEMPLATE="${INPUTDATASETDIR}/${LOCATION}/images/AvDBH.tif"

# Production of NPP, GPP, ABG, BGB, MAI and CAI output images
for Y in ${YEARS[@]} ; do
	MONTHS_NPP=""
	for B in b01 b02 b03 b04 b05 b06 b07 b08 b09 b10 b11 b12 ; do
		log "Starting execution of getOutputCMCC for NPP on year ${Y}, band ${B}..."
		${BIN}/getOutputCMCC -t ${TEMPLATE} -i ${WORK_CMCC}/${Y}_${B}_NPP_Good_Points.txt -o ${WORK_OUT}/${Y}_${B}_NPP.tif
		if [ "$?" -ne "0" ] ; then
			log "Execution of getOutputCMCC failed"
			echo "Invalid or corrupted data: execution of getOutputCMCC failed"
			exit 10
		fi
		log "...getOutputCMCC exited succesfully."
		MONTHS_NPP="${MONTHS_NPP} ${WORK_OUT}/${Y}_${B}_NPP.tif"
	done
	unset LD_LIBRARY_PATH
	log "Starting execution of mergeImg for NPP, year ${Y}..."
	${BIN}/mergeImg -b 12 -i ${MONTHS_NPP} -o ${WORK_OUT}/NPP_${Y}.tif -m VALUE=NPP,YEAR=${Y},SITE=PNMS,SPECIE=FAGUSSYLVATICA
	if [ "$?" -ne "0" ] ; then
		log "Execution of mergeImg for NPP, year ${Y} failed"
		echo "Invalid or corrupted data: execution of mergeImg for NPP, year ${Y} failed"
		exit 10
	fi
	log "...mergeImg exited succesfully."
	rm ${MONTHS_NPP}

	export LD_LIBRARY_PATH=/home/sistema/lib:${LD_LIBRARY_PATH}
	MONTHS_GPP=""
	for B in b01 b02 b03 b04 b05 b06 b07 b08 b09 b10 b11 b12 ; do
		log "Starting execution of getOutputCMCC for GPP on year ${Y}, band ${B}..."
		${BIN}/getOutputCMCC -t ${TEMPLATE} -i ${WORK_CMCC}/${Y}_${B}_GPP_Good_Points.txt -o ${WORK_OUT}/${Y}_${B}_GPP.tif
		if [ "$?" -ne "0" ] ; then
			log "Execution of getOutputCMCC failed"
			echo "Invalid or corrupted data: execution of getOutputCMCC failed"
			exit 10
		fi
		log "...getOutputCMCC exited succesfully."
		MONTHS_GPP="${MONTHS_GPP} ${WORK_OUT}/${Y}_${B}_GPP.tif"
	done
	unset LD_LIBRARY_PATH
	log "Starting execution of mergeImg for GPP, year ${Y}..."
	${BIN}/mergeImg -b 12 -i ${MONTHS_GPP} -o ${WORK_OUT}/GPP_${Y}.tif -m VALUE=GPP,YEAR=${Y},SITE=PNMS,SPECIE=FAGUSSYLVATICA
	if [ "$?" -ne "0" ] ; then
		log "Execution of mergeImg for GPP, year ${Y} failed"
		echo "Invalid or corrupted data: execution of mergeImg for GPP, year ${Y} failed"
		exit 10
	fi
	log "...mergeImg exited succesfully."
	rm ${MONTHS_GPP}

	export LD_LIBRARY_PATH=/home/sistema/lib:${LD_LIBRARY_PATH}
	log "Starting execution of getOutputCMCC for AGB on year ${Y}..."
	${BIN}/getOutputCMCC -t ${TEMPLATE} -i ${WORK_CMCC}/${Y}_AGB_Good_Points.txt -o ${WORK_OUT}/AGB_${Y}.tif
	if [ "$?" -ne "0" ] ; then
		log "Execution of getOutputCMCC failed"
		echo "Invalid or corrupted data: execution of getOutputCMCC failed"
		exit 10
	fi
	log "...getOutputCMCC exited succesfully."

	log "Starting execution of getOutputCMCC for BGB on year ${Y}..."
	${BIN}/getOutputCMCC -t ${TEMPLATE} -i ${WORK_CMCC}/${Y}_BGB_Good_Points.txt -o ${WORK_OUT}/BGB_${Y}.tif
	if [ "$?" -ne "0" ] ; then
		log "Execution of getOutputCMCC failed"
		echo "Invalid or corrupted data: execution of getOutputCMCC failed"
		exit 10
	fi
	log "...getOutputCMCC exited succesfully."
	
	log "Starting execution of getOutputCMCC for MAI on year ${Y}..."
	${BIN}/getOutputCMCC -t ${TEMPLATE} -i ${WORK_CMCC}/${Y}_MAI_Good_Points.txt -o ${WORK_OUT}/MAI_${Y}.tif
	if [ "$?" -ne "0" ] ; then
		log "Execution of getOutputCMCC failed"
		echo "Invalid or corrupted data: execution of getOutputCMCC failed"
		exit 10
	fi
	log "...getOutputCMCC exited succesfully."
	
	log "Starting execution of getOutputCMCC for CAI on year ${Y}..."
	${BIN}/getOutputCMCC -t ${TEMPLATE} -i ${WORK_CMCC}/${Y}_CAI_Good_Points.txt -o ${WORK_OUT}/CAI_${Y}.tif
	if [ "$?" -ne "0" ] ; then
		log "Execution of getOutputCMCC failed"
		echo "Invalid or corrupted data: execution of getOutputCMCC failed"
		exit 10
	fi
	log "...getOutputCMCC exited succesfully."
done

OUTDATASETNAME="${PREFIX}${LOCATION}_${RESOLUTION}m_2007-2010_out"

OUTPUTDATASETDIR="${OUTPUTDIR}/${OUTDATASETNAME}"
mkdir "${OUTPUTDATASETDIR}"
if [ ! -d "${OUTPUTDATASETDIR}" ] ; then
	log "Invalid output dataset directory."
	echo "Cannot create ${OUTPUTDATASETDIR}"
	exit 34
fi

log "Moving ${WORK_OUT} content into ${OUTPUTDATASETDIR}..."
mv -f ${WORK_OUT}/* ${OUTPUTDATASETDIR}
if [ "$?" -ne "0" ] ; then
	log "Moving of ${WORK_OUT} content into ${OUTPUTDATASETDIR} failed"
	echo "Cannot move ${WORK_OUT} content into ${OUTPUTDATASETDIR}"
	exit 34
fi
log "...move succesful"

log "Removing working dir..."
rm -r ${WORKDIR}
if [ "$?" -ne "0" ] ; then
	log "Cannot remove working directory"
	echo "Cannot remove working directory"
	exit 34
fi
log "...done"

log "Compressing output dataset into output dir..."
cd ${OUTPUTDIR}
tar -jcf ${OUTDATASETNAME}.tar.bz2 ${OUTDATASETNAME}
if [ "$?" -ne "0" ] ; then
	log "Cannot compress output dataset"
	echo "Cannot compress output dataset"
	exit 34
fi
cd - > /dev/null
log "...done"

log "Removing output dataset dir..."
rm -r ${OUTPUTDATASETDIR}
if [ "$?" -ne "0" ] ; then
	log "Cannot remove output dataset directory"
	echo "Cannot remove output dataset directory"
	exit 34
fi
log "...done"

log "${PROGNAME} successfully ended."

# Synthetic outputs for KEO purposes
echo "<synthetic name=\"output_dataset\" value=\"${OUTPUTDIR}/${OUTDATASETNAME}.tar.bz2\"/>"
echo "<synthetic name=\"logFile\" value=\"${OUTPUTDIR}/${CMCC}-${RESOLUTION}m.log\"/>" 

exit 0
