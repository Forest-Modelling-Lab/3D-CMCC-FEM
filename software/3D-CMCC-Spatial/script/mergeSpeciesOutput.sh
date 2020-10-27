#!/bin/bash
### File information summary  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
# Description:    mergeSpeciesOutput.sh bash shell script
#                 Processing chain to merge different output images from several 3D-CMCC-Spatial outputs into one multi-species packet
# Author:         Alessandro Candini - candini@meeo.it
# Version:        0.1
# Copyright:      MEEO S.R.L. - www.meeo.it 
# Changelog:      2013-01-08 - version 0.1
#                     - First Release
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  File information summary }

### Global functions definitions  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
usage(){
	echo "Usage: ${0} ZIP_PACKAGES"
	echo "       - ZIP_PACKAGES is a mandatory array of 3D-CMCC-Spatial output zip packages to be merged into one multi-species package."
	echo ""
	echo "Run example: ${0} MADONIE_30m_2002-2020_20130107000001_output.zip MADONIE_30m_2002-2020_20130107000002_output.zip MADONIE_30m_2002-2020_20130107000007_output.zip MADONIE_30m_2002-2020_20130107000007_output.zip"
	echo ""
	exit 40
}

log() {
	echo -en "$(date +"%Y-%m-%d %H:%M:%S") - ${1}"
}

YEARS=(2002 2003 2004 2005 2006 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017 2018 2019 2020)
IMG1BAND=(AGB BGB MAI CAI GPP_sum NPP_sum)
IMG12BANDS=(GPP NPP)
PAR="-q -co COMPRESS=LZW -of GTiff -ot Float32"
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global functions definitions }

### Checking input arguments and configurations - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
# Check input parameters
ZIP_LIST=()
if [ -z "${1}" ] || [ -z "${2}" ] ; then # If I have no input packages, exit
	echo "Put at least 2 input packages to merge together."
	usage
else
	for PKG in "${@}" ; do
		if [ ! -f "${PKG}" ] ; then
    		echo "One of the input package does not exist or has a wrong path."
    		echo ""
    		usage
    	else
    		ZIP_LIST+=("${PKG}")
    	fi
	done
fi
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Checking input arguments and configurations }

# Working directory with pid of the process as name
WKDIR="dir"$$""

SIGN="$( basename "${ZIP_LIST[0]}" | cut -f '4' -d '_' | cut -f '1' -d '.' )"
NEWSIGN="${SIGN:0:13}0"

GENERIC_INDIR="$( echo "${ZIP_LIST[0]}" | sed s/${SIGN}/${SIGN:0:13}\*/ | cut -f '1' -d '*' )"
GENERIC_INDIR="${GENERIC_INDIR}*"

OUTDIR="$( echo "${ZIP_LIST[0]}" | sed s/${SIGN}/${NEWSIGN}/ | cut -f '1' -d '.' )"
OUT="${WKDIR}/${OUTDIR}"

mkdir "${WKDIR}"
if [ ! -d "${WKDIR}" ] ; then
	echo "Input parameters error: Invalid working directory"
	exit 40
fi

log "Decompressing input packages..."
for ZIP in ${ZIP_LIST[@]} ; do
	unzip "${ZIP}" -d "${WKDIR}" &>/dev/null
	if [ "$?" -ne "0" ] ; then
		echo "Invalid or corrupted data: cannot decompress input dataset"
		exit 10
	fi
done
echo "done"

IN_LIST=($(ls "${WKDIR}"))

TMPOUT="${WKDIR}/tmp_output"
mkdir "${TMPOUT}"
if [ ! -d "${TMPOUT}" ] ; then
	echo "Cannot create temporary output directory"
	exit 40
fi

for Y in ${YEARS[@]} ; do
	# Loop on monoband images
	for IMG in ${IMG1BAND[@]} ; do
		log "Merging ${IMG}_${Y}.tif..."
		IMAGE="${IMG}_${Y}.tif"
		IMAGES_TO_MERGE=($( find ${WKDIR}/${GENERIC_INDIR} -type f -name ${IMAGE} ))
		gdal_merge.py ${PAR} -n 0 ${IMAGES_TO_MERGE[@]} -o ${TMPOUT}/${IMAGE}
		echo "done"
	done
	
	# Loop on multiband images
	for IMG in ${IMG12BANDS[@]} ; do
		log "Merging ${IMG}_${Y}.tif..."
		IMAGE="${IMG}_${Y}.tif"
		IMAGES_TO_MERGE=($( find ${WKDIR}/${GENERIC_INDIR} -type f -name ${IMAGE} ))
		gdal_merge.py ${PAR} -n 0 ${IMAGES_TO_MERGE[@]} -o ${TMPOUT}/${IMAGE}
		echo "done"
	done
done

for I in ${IN_LIST[@]} ; do
	CURDIR="${WKDIR}/${I}"
	log "Copy of mask images for dataset ${I}..."
	cp ${CURDIR}/*_mask.tif -t ${TMPOUT} &>/dev/null
	if [ "$?" -ne "0" ] ; then
		echo "failed"
		exit 10
	fi
	echo "done"
done

log "Renaming ${TMPOUT} into ${OUT}..."
mv ${TMPOUT} ${OUT} &>/dev/null
if [ "$?" -ne "0" ] ; then
	echo "failed"
	exit 10
fi
echo "done"

log "Compressing merged dataset into working dir..."
cd ${WKDIR}
zip -r ${OUTDIR} ${OUTDIR} &>/dev/null
if [ "$?" -ne "0" ] ; then
	echo "failed"
	exit 34
fi
cd - > /dev/null
echo "done"

log "Moving output package out from working dir..."
mv ${WKDIR}/${OUTDIR}.zip . &>/dev/null
if [ "$?" -ne "0" ] ; then
	echo "failed"
	exit 34
fi
echo "done"

log "Removing working dir..."
rm -fr ${WKDIR} &>/dev/null
if [ "$?" -ne "0" ] ; then
	echo "failed"
	exit 34
fi
echo "done"

exit 0