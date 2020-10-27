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
	echo " - REOLUTION must be within 10 and 100 and means at which resolution the CMCC model has to work (in meters)."
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


for I in ${MODULES[@]} ; do
	if [ ! -x "${BIN}/${I}" ] ; then
		echo "Module ${I} is missing. Exiting."
		exit 51
	fi
done

INPUTDATASET="${1}"
# Check if input dataset archive exists
if [ ! -f "${INPUTDATASET}" ] ; then
	echo "Input dataset not found..."
	exit 2
fi

DATASETNAME="$( basename "${INPUTDATASET}" | cut -f '1' -d '.' )"
SIGN="$( basename "${INPUTDATASET}" | cut -f '4' -d '_' | cut -f '1' -d '.' )"

RESOLUTION="${2}"
if [ ${RESOLUTION} -ne "10" ] && [ ${RESOLUTION} -ne "100" ] && [ ${RESOLUTION} -ne "30" ] ; then
	echo "Input parameters error: resolution (2nd parameter) must be 10, 30 or 100"
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
	echo "Input parameters error: Invalid working directory"
	exit 40
fi

mkdir "${WORK_IN}"
if [ ! -d "${WORK_IN}" ] ; then
	echo "Input parameters error: Invalid getInputCMCC working directory"
	exit 40
fi

mkdir "${WORK_CMCC}"
if [ ! -d "${WORK_CMCC}" ] ; then
	echo "Input parameters error: Invalid wrapCMCC working directory"
	exit 40
fi

mkdir "${WORK_OUT}"
if [ ! -d "${WORK_OUT}" ] ; then
	echo "Input parameters error: Invalid getOutputCMCC working directory"
	exit 40
fi

# Input directory
INPUTDATASETDIR="${WORKDIR}/${DATASETNAME}"


unzip ${INPUTDATASET} -d ${WORKDIR} &>/dev/null
if [ "$?" -ne "0" ] ; then
	echo "Invalid or corrupted data: cannot decompress input dataset"
	exit 10
fi

LOCATION="$( ls ${INPUTDATASETDIR} )"

LOGFILE="${OUTPUTDIR}/${LOCATION}-${SIGN}-${2}m.log"
log() {
	echo -en "$(date +"%Y-%m-%d %H:%M:%S") - ${1}" | tee -a "${LOGFILE}"
}
log "${PROGNAME} started\n"

# Soil parameters for wrapCMCC ----------------------------------- #
LOC1="MADONIE"
LOC2="TITERNO-TAMMARO"

if [ "${LOCATION}" == "${LOC1}" ] ; then
	SITENAME="Parco delle Madonie (Sicily) - Italy"
	LAT="37.53"
	LONG="14.20"
	Y_VAL="0.49"
	INITIALLITTER="2"
	MIN_FRAC_MAXASW="0.1"
	FR="0.8"
	FN0="0.5"
	FNN="0.5"
	M0="0.2"
elif [ "${LOCATION}" == "${LOC2}" ] ; then
	SITENAME="Comunità montana del Titerno ed alto Tammaro (Benevento, Campania) - Italy"
	LAT="41.20"
	LONG="14.36"
	Y_VAL="0.49"
	INITIALLITTER="2"
	MIN_FRAC_MAXASW="0.1"
	FR="0.8"
	FN0="0.5"
	FNN="0.5"
	M0="0.2"
else
	log "${LOCATION} is an unknown location: exiting."
	exit 40
fi

SITE_PARAMS="-lat ${LAT} -lon ${LONG} -yval ${Y_VAL} -l ${INITIALLITTER} -asw ${MIN_FRAC_MAXASW} -fr ${FR} -fn0 ${FN0} -fnn ${FNN} -m0 ${M0}"
# ---------------------------------------------------------------- #

# :::::::::::::::::::::: #
# getInputCMCC execution #
# :::::::::::::::::::::: #
IMG_PATH="${INPUTDATASETDIR}/${LOCATION}/images"
IMG_SPEC_NAMES=(Y_planted.tif Species.tif Phenology.tif Management.tif N_cell.tif AvDBH.tif Height.tif)
IMG_SPEC=()
for NAME in ${IMG_SPEC_NAMES[@]} ; do
	IMG_SPEC+=("${IMG_PATH}/${NAME}")
done

YEARS=(2002 2003 2004 2005 2006 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017 2018 2019 2020)
IMG_CLIM_NAMES=(SolarRad Avg_Temp VPD Precip LAI)
IMG_CLIM=()
for YYYY in ${YEARS[@]} ; do
	for NAME in ${IMG_CLIM_NAMES[@]} ; do
		IMG_CLIM+=("${IMG_PATH}/${NAME}_${YYYY}.tif")
	done
done


# Create the year string
for STR in ${YEARS[@]} ; do
	YEARS_STR=${YEARS_STR},${STR}
done

# Delete comma in front of first year
YEARS_STR=${YEARS_STR:1:${#YEARS_STR}}

# Soil input image
SOIL_IMG="${IMG_PATH}/Soil.tif"

log "Starting execution of getInputCMCC...\n"
${BIN}/getInputCMCC -n ${#IMG_SPEC[@]} -p ${IMG_SPEC[@]} -y ${YEARS_STR} -c ${IMG_CLIM[@]} -s ${SOIL_IMG} -o ${WORK_IN}
#cp /home/candini/Desktop/getInputCMCC_temp/* ${WORK_IN}
if [ "$?" -ne "0" ] ; then
	log "Execution of getInputCMCC failed\n"
	exit 10
fi
log "...getInputCMCC exited succesfully.\n"

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

SOIL="${WORK_IN}/Soil.txt"
SETTINGS="${INPUTDATASETDIR}/${LOCATION}/txt/settings.txt"

log "Starting execution of wrapCMCC...\n"
${BIN}/wrapCMCC -p ${NUM_PX} -y ${YEARS_STR} -yf ${YEAR_STR} -sf ${SPEC_STR} -e "${BIN}/${CMCC}" -i "${INPUTDATASETDIR}/${LOCATION}/txt" -s "${SOIL}" -c "${SETTINGS}" -o ${WORK_CMCC} -sname "${SITENAME}" ${SITE_PARAMS} 1> /dev/null
#cp /home/candini/Desktop/wrapCMCC_temp/* ${WORK_CMCC}
if [ "$?" -ne "0" ] ; then
	log "Execution of wrapCMCC failed\n"
	exit 10
fi


log "...wrapCMCC exited succesfully.\n"

# :::::::::::::::::::::::::::::::::::: #
# getOutputCMCC and mergeImg execution #
# :::::::::::::::::::::::::::::::::::: #
#cp /home/candini/Desktop/getOutputCMCC_temp/* ${WORK_OUT}
TEMPLATE="${INPUTDATASETDIR}/${LOCATION}/images/AvDBH.tif"

# Production of GPP, NPP, ABG, BGB, MAI and CAI output images
for Y in ${YEARS[@]} ; do
	export LD_LIBRARY_PATH=/home/sistema/lib:${LD_LIBRARY_PATH}
	MONTHS_NPP=""
	log "Starting execution of getOutputCMCC for NPP on year ${Y}...\n"
	for B in b01 b02 b03 b04 b05 b06 b07 b08 b09 b10 b11 b12 ; do
		${BIN}/getOutputCMCC -t ${TEMPLATE} -i ${WORK_CMCC}/${Y}_${B}_NPP_Good_Points.txt -o ${WORK_OUT}/${Y}_${B}_NPP.tif
		if [ "$?" -ne "0" ] ; then
			log "Execution of getOutputCMCC failed\n"
			exit 10
		fi
		MONTHS_NPP="${MONTHS_NPP} ${WORK_OUT}/${Y}_${B}_NPP.tif"
	done
	log "...getOutputCMCC exited succesfully.\n"
	unset LD_LIBRARY_PATH
	log "Starting execution of mergeImg for NPP, year ${Y}...\n"
	${BIN}/mergeImg -b 12 -i ${MONTHS_NPP} -o ${WORK_OUT}/NPP_${Y}.tif -m VALUE=NPP,YEAR=${Y},SITE=${LOCATION}
	if [ "$?" -ne "0" ] ; then
		log "Execution of mergeImg for NPP, year ${Y} failed\n"
		exit 10
	fi
	log "...mergeImg exited succesfully.\n"
	rm ${MONTHS_NPP}
	
	INPUT_01="${WORK_OUT}/NPP_${Y}.tif"
	INPUT_02="-A ${INPUT_01} --A_band=1 -B ${INPUT_01} --B_band=2 -C ${INPUT_01} --C_band=3 -D ${INPUT_01} --D_band=4 -E ${INPUT_01} --E_band=5 -F ${INPUT_01} --F_band=6 -G ${INPUT_01} --G_band=7 -H ${INPUT_01} --H_band=8 -I ${INPUT_01} --I_band=9 -J ${INPUT_01} --J_band=10 -K ${INPUT_01} --K_band=11 -L ${INPUT_01} --L_band=12"
	gdal_calc.py ${INPUT_02} --outfile=${WORK_OUT}/NPP_sum_${Y}.tif --calc="(A+B+C+D+E+F+G+H+I+J+K+L)" &>/dev/null

	# ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

	export LD_LIBRARY_PATH=/home/sistema/lib:${LD_LIBRARY_PATH}
	MONTHS_GPP=""
	log "Starting execution of getOutputCMCC for GPP on year ${Y}...\n"
	for B in b01 b02 b03 b04 b05 b06 b07 b08 b09 b10 b11 b12 ; do
		${BIN}/getOutputCMCC -t ${TEMPLATE} -i ${WORK_CMCC}/${Y}_${B}_GPP_Good_Points.txt -o ${WORK_OUT}/${Y}_${B}_GPP.tif
		if [ "$?" -ne "0" ] ; then
			log "Execution of getOutputCMCC failed\n"
			exit 10
		fi
		MONTHS_GPP="${MONTHS_GPP} ${WORK_OUT}/${Y}_${B}_GPP.tif"
	done
	log "...getOutputCMCC exited succesfully.\n"
	unset LD_LIBRARY_PATH
	log "Starting execution of mergeImg for GPP, year ${Y}...\n"
	${BIN}/mergeImg -b 12 -i ${MONTHS_GPP} -o ${WORK_OUT}/GPP_${Y}.tif -m VALUE=GPP,YEAR=${Y},SITE=${LOCATION}
	if [ "$?" -ne "0" ] ; then
		log "Execution of mergeImg for GPP, year ${Y} failed\n"
		exit 10
	fi
	log "...mergeImg exited succesfully.\n"
	rm ${MONTHS_GPP}
	
	INPUT_01="${WORK_OUT}/GPP_${Y}.tif"
	INPUT_02="-A ${INPUT_01} --A_band=1 -B ${INPUT_01} --B_band=2 -C ${INPUT_01} --C_band=3 -D ${INPUT_01} --D_band=4 -E ${INPUT_01} --E_band=5 -F ${INPUT_01} --F_band=6 -G ${INPUT_01} --G_band=7 -H ${INPUT_01} --H_band=8 -I ${INPUT_01} --I_band=9 -J ${INPUT_01} --J_band=10 -K ${INPUT_01} --K_band=11 -L ${INPUT_01} --L_band=12"
	gdal_calc.py ${INPUT_02} --outfile=${WORK_OUT}/GPP_sum_${Y}.tif --calc="(A+B+C+D+E+F+G+H+I+J+K+L)" &>/dev/null

	export LD_LIBRARY_PATH=/home/sistema/lib:${LD_LIBRARY_PATH}
	log "Starting execution of getOutputCMCC for AGB on year ${Y}...\n"
	${BIN}/getOutputCMCC -t ${TEMPLATE} -i ${WORK_CMCC}/${Y}_AGB_Good_Points.txt -o ${WORK_OUT}/AGB_${Y}.tif
	if [ "$?" -ne "0" ] ; then
		log "Execution of getOutputCMCC failed\n"
		exit 10
	fi
	log "...getOutputCMCC exited succesfully.\n"

	log "Starting execution of getOutputCMCC for BGB on year ${Y}...\n"
	${BIN}/getOutputCMCC -t ${TEMPLATE} -i ${WORK_CMCC}/${Y}_BGB_Good_Points.txt -o ${WORK_OUT}/BGB_${Y}.tif
	if [ "$?" -ne "0" ] ; then
		log "Execution of getOutputCMCC failed\n"
		exit 10
	fi
	log "...getOutputCMCC exited succesfully.\n"
	
	log "Starting execution of getOutputCMCC for MAI on year ${Y}...\n"
	${BIN}/getOutputCMCC -t ${TEMPLATE} -i ${WORK_CMCC}/${Y}_MAI_Good_Points.txt -o ${WORK_OUT}/MAI_${Y}.tif
	if [ "$?" -ne "0" ] ; then
		log "Execution of getOutputCMCC failed\n"
		exit 10
	fi
	log "...getOutputCMCC exited succesfully.\n"
	
	log "Starting execution of getOutputCMCC for CAI on year ${Y}...\n"
	${BIN}/getOutputCMCC -t ${TEMPLATE} -i ${WORK_CMCC}/${Y}_CAI_Good_Points.txt -o ${WORK_OUT}/CAI_${Y}.tif
	if [ "$?" -ne "0" ] ; then
		log "Execution of getOutputCMCC failed\n"
		exit 10
	fi
	log "...getOutputCMCC exited succesfully.\n"
done

FIRST_YEAR="${YEARS[0]}"
LAST_YEAR="${YEARS[$((${#YEARS[@]}-1))]}"

OUTDATASETNAME="${PREFIX}${LOCATION}_${RESOLUTION}m_${FIRST_YEAR}-${LAST_YEAR}_${SIGN}_output"

OUTPUTDATASETDIR="${OUTPUTDIR}/${OUTDATASETNAME}"
mkdir "${OUTPUTDATASETDIR}"
if [ ! -d "${OUTPUTDATASETDIR}" ] ; then
	log "Invalid output dataset directory.\n"
	exit 34
fi

log "Moving ${WORK_OUT} content into ${OUTPUTDATASETDIR}...\n"
mv -f ${WORK_OUT}/* ${OUTPUTDATASETDIR}
if [ "$?" -ne "0" ] ; then
	log "Moving of ${WORK_OUT} content into ${OUTPUTDATASETDIR} failed\n"
	exit 34
fi
log "...move succesful\n"

log "Moving masks to ${OUTPUTDATASETDIR}\n"
mv -f ${IMG_PATH}/*_mask.tif ${OUTPUTDATASETDIR}
if [ "$?" -ne "0" ] ; then
	log "Moving of masks into ${OUTPUTDATASETDIR} failed\n"
	exit 34
fi
log "...move succesful\n"

log "Removing working dir...\n"
rm -r ${WORKDIR}
if [ "$?" -ne "0" ] ; then
	log "Cannot remove working directory\n"
	exit 34
fi
log "...done\n"

log "Compressing output dataset into output dir...\n"
cd ${OUTPUTDIR}
zip -r ${OUTDATASETNAME} ${OUTDATASETNAME} &>/dev/null
if [ "$?" -ne "0" ] ; then
	log "Cannot compress output dataset\n"
	exit 34
fi
cd - > /dev/null
log "...done\n"

log "Removing output dataset dir...\n"
rm -r ${OUTPUTDATASETDIR}
if [ "$?" -ne "0" ] ; then
	log "Cannot remove output dataset directory\n"
	exit 34
fi
log "...done\n"

log "${PROGNAME} successfully ended.\n"

log "Produced ${OUTPUTDIR}/${OUTDATASETNAME}.zip\n"
log "Produced ${LOGFILE}\n" 
log "${PROGNAME} successfully ended.\n"

exit 0