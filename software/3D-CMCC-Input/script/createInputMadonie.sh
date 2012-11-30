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
SCRIPT_NAME="${0:2:-3}"
AOI="Parco delle Madonie (Sicily)"
SITE="MADONIE"
MODULES=(remap applyMask calcAverage multiplyImgPx getLAI createImg mergeImg specFromMaxPerc copyGeoref reduceToBinaryMask)
IMG_ALL=(Filters Y_planted Species Phenology Management NumHa AvDBH Height Wf Wrc Ws SolarRad Avg_Temp VPD Precip LAI Soil)
IMG_SELECTED=()

# Species identification numbers: 
# 0 = "-9999" (Undefined)
# 1 = "Castaneasativa"
# 2 = "Fagussylvatica"
# 3 = "Ostryacarpinifolia" 
# 4 = "Pinusnigra"
# 5 = "Quercuscerris"
# 6 = "quercus_deciduous" (Q. cerris, Q. robur, Q. pubescens, Q. petreae)
# 7 = "quercus_evergreen" (Q. ilex, Q. suber)
SPECIES_ID=(Undefined Castaneasativa Fagussylvatica Ostryacarpinifolia Pinusnigra Quercuscerris quercus_deciduous quercus_evergreen)

# Phenology identification numbers:
# 0 = "-9999" (Undefined)
# 1 = "D"     (Deciduous)
# 2 = "E"     (Evergreen)
PHENOLOGY_ID=(Undefined Deciduous Evergreen)

# Management identification numbers:
# 0 = "-9999" (Undefined)
# 1 = "T"     (Timber)
# 2 = "C"     (Coppice)
MANAGEMENT_ID=(Undefined Timber Coppice)

BIN_DIR="$( dirname ${0} )/../bin"
OUTPUT_DIR="$( dirname ${0} )/../output"
# Input directories
IN_00="$( dirname ${0} )/../input/00_Filters"
IN_01="$( dirname ${0} )/../input/01_Y_planted"
IN_02="$( dirname ${0} )/../input/02_Species"
IN_03="$( dirname ${0} )/../input/03_Phenology"
IN_04="$( dirname ${0} )/../input/04_Management"
IN_05="$( dirname ${0} )/../input/05_NumHa"
IN_06="$( dirname ${0} )/../input/06_AvDBH"
IN_07="$( dirname ${0} )/../input/07_Height"
IN_08="$( dirname ${0} )/../input/08_Wf"
IN_09="$( dirname ${0} )/../input/09_Wrc"
IN_10="$( dirname ${0} )/../input/10_Ws"
IN_11="$( dirname ${0} )/../input/11_SolarRad"
IN_12="$( dirname ${0} )/../input/12_Avg_Temp"
IN_13="$( dirname ${0} )/../input/13_VPD"
IN_14="$( dirname ${0} )/../input/14_Precip"
IN_15="$( dirname ${0} )/../input/15_LAI"
IN_16="$( dirname ${0} )/../input/16_Soil"
# Output directories
OUT_00="$( dirname ${0} )/../output/00_Filters"
OUT_01="$( dirname ${0} )/../output/01_Y_planted"
OUT_02="$( dirname ${0} )/../output/02_Species"
OUT_03="$( dirname ${0} )/../output/03_Phenology"
OUT_04="$( dirname ${0} )/../output/04_Management"
OUT_05="$( dirname ${0} )/../output/05_NumHa"
OUT_06="$( dirname ${0} )/../output/06_AvDBH"
OUT_07="$( dirname ${0} )/../output/07_Height"
OUT_08="$( dirname ${0} )/../output/08_Wf"
OUT_09="$( dirname ${0} )/../output/09_Wrc"
OUT_10="$( dirname ${0} )/../output/10_Ws"
OUT_11="$( dirname ${0} )/../output/11_SolarRad"
OUT_12="$( dirname ${0} )/../output/12_Avg_Temp"
OUT_13="$( dirname ${0} )/../output/13_VPD"
OUT_14="$( dirname ${0} )/../output/14_Precip"
OUT_15="$( dirname ${0} )/../output/15_LAI"
OUT_16="$( dirname ${0} )/../output/16_Soil"
# Working directories
WK_00="$( dirname ${0} )/../working/00_Filters"
WK_01="$( dirname ${0} )/../working/01_Y_planted"
WK_02="$( dirname ${0} )/../working/02_Species"
WK_03="$( dirname ${0} )/../working/03_Phenology"
WK_04="$( dirname ${0} )/../working/04_Management"
WK_05="$( dirname ${0} )/../working/05_NumHa"
WK_06="$( dirname ${0} )/../working/06_AvDBH"
WK_07="$( dirname ${0} )/../working/07_Height"
WK_08="$( dirname ${0} )/../working/08_Wf"
WK_09="$( dirname ${0} )/../working/09_Wrc"
WK_10="$( dirname ${0} )/../working/10_Ws"
WK_11="$( dirname ${0} )/../working/11_SolarRad"
WK_12="$( dirname ${0} )/../working/12_Avg_Temp"
WK_13="$( dirname ${0} )/../working/13_VPD"
WK_14="$( dirname ${0} )/../working/14_Precip"
WK_15="$( dirname ${0} )/../working/15_LAI"
WK_16="$( dirname ${0} )/../working/16_Soil"

# Output geotiff size:
SIZEX="1286"
SIZEY="1160"
# Output geotiff resolution:
RES="30"
# Output geotiff Upper Left and Lower Right point coordinates:
UL_LON="399333.291887304978445"
UL_LAT="4211980.487859229557216"
LR_LON="437913.291887304978445"
LR_LAT="4177180.487859229557216"
# Geotiff projections (proj4 definitions from spatialreference.org):
PROJ="+proj=utm +zone=33 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
PROJ_32="+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
PROJ_LONGLAT="+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs"
PROJ_3004="+proj=tmerc +lat_0=0 +lon_0=15 +k=0.9996 +x_0=2520000 +y_0=0 +ellps=intl +units=m +no_defs"
PAR_01="-q -co COMPRESS=LZW -of GTiff"
PAR_02="-ot Float32"

# Temporal coverage
YEARS_PROC=(2002 2003 2004 2005 2006 2007 2008 2009)
MONTHS_PROC=(01 02 03 04 05 06 07 08 09 10 11 12)

# DEBUG="n" --> clean the current working directory
# DEBUG="y" --> do not clean the current working directory
DEBUG="y"
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global variables definitions }

### Global functions definitions  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
usage(){
	echo "Usage: ${0} [IMG_TO_PROCESS]"
	echo "       - IMG_TO_PROCESS is an optional array to define which images to process. If omitted, every image will be created."
	echo "       - Accepted values for the array are (every other value will be ignored):"
	echo "             ${IMG_ALL[@]}"
	echo "       - NOTE: Remeber to set up and fill correctly the BIN_DIR, directory where the C modules are stored."
	echo ""
	echo "Run example: ${0} Phenology LAI Wrc NumHa"
	echo ""
	exit 40
}

log() {
	echo -en "$(date +"%Y-%m-%d %H:%M:%S") - ${1}" | tee -a "${LOGFILE}"
}

check() {
	if [ ${?} -ne "0" ] ; then
		log "ERROR: ${1}.\n"
	else
		log "... done.\n"
	fi
}

clean() {
	if [ ${DEBUG} == "n" ] ; then
		MSG="Cleaning working directory ${1}"
		log "${MSG} ...\n"
		rm -r ${1}/*
		check "${MSG} failed.\n"
	fi
}

leapYear(){
	if [ $[${1} % 400] -eq "0" ]; then
		# This is a leap year: February has 29 days.
			return 1
	elif [ $[${1} % 4] -eq "0" ]; then
    	if [ $[${1} % 100] -ne "0" ]; then
        	# This is a leap year: February has 29 days.
			return 1
        else
        	# This is not a leap year: February has 28 days.
			return 0
        fi
	else
		# This is not a leap year: February has 28 days.
		return 0
	fi
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

### Pre-execution settings- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
# Processing log filename with and without path
LOGFILENAME="${SCRIPT_NAME}.log"
LOGFILE="${OUTPUT_DIR}/${LOGFILENAME}"

# Remove logfile if already existent
if [ -f "${LOGFILE}" ] ; then
	rm -f "${LOGFILE}"
	if [ ${?} -ne "0" ] ; then
		echo "Removing of existing "${LOGFILE}" failed."
		exit 1
	fi
fi

log "Starting data processing for ForSE project (version ${VERSION})\n"
log "Area Of Interest: ${AOI}\n"
log "\n"
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Pre-execution settings }

### Filters execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Filters" ] ; then
		log "### { Start creating ${IMG} images...... ###\n"
		
		#MSG="Conversion of AOI shapefile into tiff"
		#INPUT_01="${IN_00}/Sicily.shp"
		#OUTPUT_01="${WK_00}/Sicily.tif"
		#log "${MSG} ...\n"
		#gdal_rasterize ${PAR_01} ${PAR_02} -burn 255 -tr 0.00045266691956530711 0.00045266691956530711 ${INPUT_01} ${OUTPUT_01} &>> "${LOGFILE}"
		#check "${MSG} failed on ${INPUT_01}.\n"
		#MSG="Conversion of tiff projection from longlat to UTM"
		#OUTPUT_02="${WK_00}/Madonie_no_remap.tif"
		#log "${MSG} ...\n"
		#gdalwarp ${PAR_01} -t_srs "${PROJ}" -tr ${RES} -${RES} ${OUTPUT_01} ${OUTPUT_02} &>> "${LOGFILE}"
		#check "${MSG} failed on ${OUTPUT_01}.\n"
		#MSG="Remap and cut UTM geotiff image"
		#OUTPUT_03="${WK_00}/Madonie.tif"
		#log "${MSG} ...\n"
		#${BIN_DIR}/remap -i ${OUTPUT_02} -o ${OUTPUT_03} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
		#check "${MSG} failed on ${OUTPUT_02}.\n"

		MSG="Conversion of shapefile georef"
		INPUT_01="${IN_00}/CFRS_ParcoMadonie.shp"
		OUTPUT_01="${WK_00}/CFRS_ParcoMadonie_utm.shp"
		log "${MSG} ...\n"
		ogr2ogr -a_srs "${PROJ_3004}" -t_srs "${PROJ}" ${OUTPUT_01} ${INPUT_01} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="1"
		MSG="Conversion of Madonie shapefile into GeoTiff (${SPECIES_ID[${IDX}]})"
		OUTPUT_02="${WK_00}/Madonie_${SPECIES_ID[${IDX}]}_${IDX}.tif"
		log "${MSG} ...\n"
		gdal_rasterize ${PAR_01} ${PAR_02} -sql 'SELECT * FROM CFRS_ParcoMadonie_utm WHERE COD_CATEG="CA"' -burn ${IDX} -tr ${RES} -${RES} ${OUTPUT_01} ${OUTPUT_02} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Remap and cut UTM geotiff image"
		OUTPUT_03="${WK_00}/${SPECIES_ID[${IDX}]}_remapped.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/remap -i ${OUTPUT_02} -o ${OUTPUT_03} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="2"
		MSG="Conversion of Madonie shapefile into GeoTiff (${SPECIES_ID[${IDX}]})"
		OUTPUT_04="${WK_00}/Madonie_${SPECIES_ID[${IDX}]}_${IDX}.tif"
		log "${MSG} ...\n"
		gdal_rasterize ${PAR_01} ${PAR_02} -sql 'SELECT * FROM CFRS_ParcoMadonie_utm WHERE COD_CATEG="FA"' -burn ${IDX} -tr ${RES} -${RES} ${OUTPUT_01} ${OUTPUT_04} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Remap and cut UTM geotiff image"
		OUTPUT_05="${WK_00}/${SPECIES_ID[${IDX}]}_remapped.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/remap -i ${OUTPUT_04} -o ${OUTPUT_05} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="6"
		MSG="Conversion of Madonie shapefile into GeoTiff (${SPECIES_ID[${IDX}]})"
		OUTPUT_06="${WK_00}/Madonie_${SPECIES_ID[${IDX}]}_${IDX}.tif"
		log "${MSG} ...\n"
		gdal_rasterize ${PAR_01} ${PAR_02} -sql 'SELECT * FROM CFRS_ParcoMadonie_utm WHERE COD_CATEG="QU" OR COD_CATEG="CE"' -burn ${IDX} -tr ${RES} -${RES} ${OUTPUT_01} ${OUTPUT_06} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Remap and cut UTM geotiff image"
		OUTPUT_07="${WK_00}/${SPECIES_ID[${IDX}]}_remapped.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/remap -i ${OUTPUT_06} -o ${OUTPUT_07} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="7"
		MSG="Conversion of Madonie shapefile into GeoTiff (${SPECIES_ID[${IDX}]})"
		OUTPUT_08="${WK_00}/Madonie_${SPECIES_ID[${IDX}]}_${IDX}.tif"
		log "${MSG} ...\n"
		gdal_rasterize ${PAR_01} ${PAR_02} -sql 'SELECT * FROM CFRS_ParcoMadonie_utm WHERE COD_CATEG="LE" OR COD_CATEG="SU"' -burn ${IDX} -tr ${RES} -${RES} ${OUTPUT_01} ${OUTPUT_08} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Remap and cut UTM geotiff image"
		OUTPUT_09="${WK_00}/${SPECIES_ID[${IDX}]}_remapped.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/remap -i ${OUTPUT_08} -o ${OUTPUT_09} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		FILTER_01="${OUTPUT_03}"
		FILTER_02="${OUTPUT_05}"
		FILTER_06="${OUTPUT_07}"
		FILTER_07="${OUTPUT_09}"
		
		MSG="Merge of deciduous species"
		FILTER_D="${WK_00}/Deciduous_filter.tif"
		log "${MSG} ...\n"
		gdal_merge.py ${PAR_01} -n 0 ${FILTER_01} ${FILTER_02} ${FILTER_06} -o ${FILTER_D} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		FILTER_E="${FILTER_07}"
		
		MSG="Get a binary mask of type GDT_Byte"
		MASK_D="${WK_00}/Deciduous_mask.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/reduceToBinaryMask -i ${FILTER_D} -o ${MASK_D} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Get a binary mask of type GDT_Byte"
		MASK_E="${WK_00}/Evergreen_mask.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/reduceToBinaryMask -i ${FILTER_E} -o ${MASK_E} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Get a total mask"
		MASK_TOT="${WK_00}/Total_mask.tif"
		log "${MSG} ...\n"
		gdal_merge.py ${PAR_01} -n 0 ${MASK_D} ${MASK_E} -o ${MASK_TOT} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Create an empty monoband image"
		OUTPUT_10="${WK_00}/empty.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/createImg -x ${SIZEX} -y ${SIZEY} -b 1 -t float -v 0 -c -n "${OUTPUT_10}" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Set georeference to empty band"
		OUTPUT_11="${WK_00}/empty_geo.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/copyGeoref -i ${FILTER_E} ${OUTPUT_10} -o ${OUTPUT_11} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Copy masks and empty band into output dir"
		log "${MSG} ...\n"
		cp ${MASK_D} ${MASK_E} ${MASK_TOT} ${OUTPUT_11} -t ${OUT_00}
		check "${MSG} failed.\n"
		
#		MSG="Merge of multiple species"
#		OUTPUT_10="${WK_00}/Madonie_species.tif"
#		log "${MSG} ...\n"
#		gdal_merge.py ${PAR_01} -n 0 -separate ${OUTPUT_03} ${OUTPUT_05} ${OUTPUT_07} ${OUTPUT_09} -o ${OUTPUT_10} &>> "${LOGFILE}"
#		check "${MSG} failed.\n"
#		
#		MSG="Adding metadata to ${OUTPUT_10}"
#		METADATA="-mo SITE=${SITE} -mo ID=SPECIE -mo -9999=${SPECIES_ID[0]} -mo 1=${SPECIES_ID[1]} -mo 2=${SPECIES_ID[2]} -mo 3=${SPECIES_ID[3]} -mo 4=${SPECIES_ID[4]} -mo 5=${SPECIES_ID[5]} -mo 6=${SPECIES_ID[6]} -mo 7=${SPECIES_ID[7]} -mo BAND1=${SPECIES_ID[1]} -mo BAND2=${SPECIES_ID[2]} -mo BAND3=${SPECIES_ID[6]} -mo BAND4=${SPECIES_ID[7]}"
#		OUTPUT_11="${WK_00}/Madonie_species_filter.tif"
#		log "${MSG} ...\n"
#		gdal_translate ${PAR_01} ${METADATA} ${OUTPUT_10} ${OUTPUT_11} &>> "${LOGFILE}"
#		check "${MSG} failed.\n"
#		
#		MSG="Copy ${OUTPUT_11} into output dir"
#		log "${MSG} ...\n"
#		cp ${OUTPUT_11} ${OUT_00}
#		check "${MSG} failed.\n"

		clean "${WK_00}"

		log "### .......stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Filters execution }

### Y_planted execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Y_planted" ] ; then
    	log "### { Start creating ${IMG} images.... ###\n"
    	log "### .....stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Y_planted execution }

### Species execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Species" ] ; then
    	log "### { Start creating ${IMG} images...... ###\n"
    	
#   INPUT_03=()
#	for INPUT_02 in $( ls ${IN_02}/*.asc ) ; do
#		MSG="Conversion from ASCII corine format into geotiff of ${INPUT_02}"
#		OUTPUT_04="${WK_02}/$( basename $( echo ${INPUT_02} | sed s/.asc/.tif/ ) )"
#		log "${MSG} ...\n"
#		gdal_translate ${PAR_01} -a_srs "${PROJ_32}" ${INPUT_02} ${OUTPUT_04} &>> "${LOGFILE}"
#	check "${MSG} failed on ${INPUT_02}.\n"
#
#	MSG="Changing zone from 32 to 33 of ${OUTPUT_04}"
#	OUTPUT_05="${WK_02}/$( basename $( echo ${INPUT_02} | sed s/.asc/_zone33.tif/ ) )"
#	log "${MSG} ...\n"
#	gdalwarp ${PAR_01} -t_srs "${PROJ}" ${OUTPUT_04} ${OUTPUT_05} &>> "${LOGFILE}"
#	check "${MSG} failed on ${OUTPUT_04}.\n"
#
#	MSG="Remap of UTM geotiff image"
#	OUTPUT_06="${WK_02}/$( basename $( echo ${INPUT_02} | sed s/.asc/_Madonie_30m.tif/ ) )"
#	log "${MSG} ...\n"	
#			${BIN_DIR}/remap -i ${OUTPUT_05} -o ${OUTPUT_06} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
#			check "${MSG} failed on ${OUTPUT_05}.\n"
#	
#			INPUT_03+=("${OUTPUT_06}")
#		done
#		
#		MSG="Merge different corine images"
#		log "${MSG} ...\n"
#		OUTPUT_07="${WK_02}/Species_corine_one_band.tif"	
#		${BIN_DIR}/specFromMaxPerc -b ${#INPUT_03[@]} -i ${INPUT_03[@]} -v 7,6,2 -o ${OUTPUT_07} &>> "${LOGFILE}"
#		check "${MSG} failed.\n"
#		
#		MSG="Create an empty monoband image"
#		OUTPUT_08="${WK_02}/empty.tif"
#		log "${MSG} ...\n"
#		${BIN_DIR}/createImg -x ${SIZEX} -y ${SIZEY} -b 1 -t float -v 0 -c -n "${OUTPUT_08}" &>> "${LOGFILE}"
#		check "${MSG} failed.\n"
#		
#		MSG="Give to empty image a proper georeference"
#		OUTPUT_09="${WK_02}/empty_with_georef.tif"
#		log "${MSG} ...\n"
#		gdal_translate ${PAR_01} -a_srs "${PROJ}" -a_ullr ${UL_LON} ${UL_LAT} ${LR_LON} ${LR_LAT} ${OUTPUT_08} ${OUTPUT_09} &>> "${LOGFILE}"
#		check "${MSG} failed.\n"
#
#		METADATA="SITE=${SITE},ID=SPECIE,-9999=${SPECIES_ID[0]},1=${SPECIES_ID[1]},2=${SPECIES_ID[2]},3=${SPECIES_ID[3]},4=${SPECIES_ID[4]},5=${SPECIES_ID[5]},6=${SPECIES_ID[6]},7=${SPECIES_ID[7]}"
#		MSG="Create multiband ${IMG} image"
#		INPUT_04=(${OUTPUT_07} ${OUTPUT_09} ${OUTPUT_09} ${OUTPUT_09} ${OUTPUT_09})
#		OUTPUT_10="${WK_02}/Species_corine.tif"
#		log "${MSG} ...\n"
#		${BIN_DIR}/mergeImg -b ${#INPUT_04[@]} -i ${INPUT_04[@]} -o ${OUTPUT_10} -m "${METADATA}" &>> "${LOGFILE}"
#		check "${MSG} failed.\n"
#		
#		MSG="Copy ${OUTPUT_10} into output dir"
#		log "${MSG} ...\n"
#		cp ${OUTPUT_10} ${OUT_02}
#		check "${MSG} failed.\n"
		
		# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

		MSG="Conversion of shapefile georef"
		INPUT_01="${IN_02}/CFRS_ParcoMadonie.shp"
		OUTPUT_01="${WK_02}/CFRS_ParcoMadonie_utm.shp"
		log "${MSG} ...\n"
		ogr2ogr -a_srs "${PROJ_3004}" -t_srs "${PROJ}" ${OUTPUT_01} ${INPUT_01} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="1"
		MSG="Conversion of Madonie shapefile into GeoTiff (${SPECIES_ID[${IDX}]})"
		OUTPUT_02="${WK_02}/Madonie_${SPECIES_ID[${IDX}]}_${IDX}.tif"
		log "${MSG} ...\n"
		gdal_rasterize ${PAR_01} ${PAR_02} -sql 'SELECT * FROM CFRS_ParcoMadonie_utm WHERE COD_CATEG="CA"' -burn ${IDX} -tr ${RES} -${RES} ${OUTPUT_01} ${OUTPUT_02} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="2"
		MSG="Conversion of Madonie shapefile into GeoTiff (${SPECIES_ID[${IDX}]})"
		OUTPUT_03="${WK_02}/Madonie_${SPECIES_ID[${IDX}]}_${IDX}.tif"
		log "${MSG} ...\n"
		gdal_rasterize ${PAR_01} ${PAR_02} -sql 'SELECT * FROM CFRS_ParcoMadonie_utm WHERE COD_CATEG="FA"' -burn ${IDX} -tr ${RES} -${RES} ${OUTPUT_01} ${OUTPUT_03} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="6"
		MSG="Conversion of Madonie shapefile into GeoTiff (${SPECIES_ID[${IDX}]})"
		OUTPUT_04="${WK_02}/Madonie_${SPECIES_ID[${IDX}]}_${IDX}.tif"
		log "${MSG} ...\n"
		gdal_rasterize ${PAR_01} ${PAR_02} -sql 'SELECT * FROM CFRS_ParcoMadonie_utm WHERE COD_CATEG="QU" OR COD_CATEG="CE"' -burn ${IDX} -tr ${RES} -${RES} ${OUTPUT_01} ${OUTPUT_04} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="7"
		MSG="Conversion of Madonie shapefile into GeoTiff (${SPECIES_ID[${IDX}]})"
		OUTPUT_05="${WK_02}/Madonie_${SPECIES_ID[${IDX}]}_${IDX}.tif"
		log "${MSG} ...\n"
		gdal_rasterize ${PAR_01} ${PAR_02} -sql 'SELECT * FROM CFRS_ParcoMadonie_utm WHERE COD_CATEG="LE" OR COD_CATEG="SU"' -burn ${IDX} -tr ${RES} -${RES} ${OUTPUT_01} ${OUTPUT_05} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Merge of multiple species"
		OUTPUT_06="${WK_02}/Madonie_species.tif"
		log "${MSG} ...\n"
		gdal_merge.py ${PAR_01} -n 0 ${OUTPUT_02} ${OUTPUT_03} ${OUTPUT_04} ${OUTPUT_05} -o ${OUTPUT_06} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Remap and cut UTM geotiff image"
		OUTPUT_07="${WK_02}/Species_carta_forestale_one_band.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/remap -i ${OUTPUT_06} -o ${OUTPUT_07} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
		check "${MSG} failed on ${OUTPUT_02}.\n"	
		
    	NAME_EMPTY_BAND="empty_geo.tif"
    	EMPTY_BAND="${WK_02}/${NAME_EMPTY_BAND}"
		MSG="Copy empty band from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_EMPTY_BAND} -t ${WK_02}
		check "${MSG} failed.\n"
		
		MSG="Create multiband ${IMG} image"
		INPUT_02=(${OUTPUT_07} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND})
		OUTPUT_08="${WK_02}/${IMG}.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/mergeImg -b ${#INPUT_02[@]} -i ${INPUT_02[@]} -o ${OUTPUT_08} -m "${METADATA}" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Copy ${OUTPUT_08} into output dir"
		log "${MSG} ...\n"
		cp ${OUTPUT_08} ${OUT_02}
		check "${MSG} failed.\n"
		
		clean "${WK_02}"
    	
    	log "### .......stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Species execution }

### Phenology execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Phenology" ] ; then
    	log "### { Start creating ${IMG} images.... ###\n"
    	
		NAME_MASK_D="Deciduous_mask.tif"
    	NAME_MASK_E="Evergreen_mask.tif"
    	NAME_MASK_TOT="Total_mask.tif"
    	NAME_EMPTY_BAND="empty_geo.tif"
		MSG="Copy filters and empty band from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_MASK_D} ${OUT_00}/${NAME_MASK_E} ${OUT_00}/${NAME_MASK_TOT} ${OUT_00}/${NAME_EMPTY_BAND} -t ${WK_03}
		check "${MSG} failed.\n"
		
		MASK_D="${WK_03}/${NAME_MASK_D}"
    	MASK_E="${WK_03}/${NAME_MASK_E}"
    	MASK_TOT="${WK_03}/${NAME_MASK_TOT}"
    	EMPTY_BAND="${WK_03}/${NAME_EMPTY_BAND}"
    	
    	IDX="1"
		MSG="Create a monoband image with every pixel set to ${IDX} (${PHENOLOGY_ID[${IDX}]})"
		OUTPUT_01="${WK_03}/Every_px_deciduous.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/createImg -x ${SIZEX} -y ${SIZEY} -b 1 -t float -v ${IDX} -c -n "${OUTPUT_01}" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="2"
		MSG="Create a monoband image with every pixel set to ${IDX} (${PHENOLOGY_ID[${IDX}]})"
		OUTPUT_02="${WK_03}/Every_px_evergreen.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/createImg -x ${SIZEX} -y ${SIZEY} -b 1 -t float -v ${IDX} -c -n "${OUTPUT_02}" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Set georeference to ${OUTPUT_01}"
		OUTPUT_03="${WK_03}/Every_px_deciduous_geo.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/copyGeoref -i ${MASK_D} ${OUTPUT_01} -o ${OUTPUT_03} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Set georeference to ${OUTPUT_02}"
		OUTPUT_04="${WK_03}/Every_px_evergreen_geo.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/copyGeoref -i ${MASK_D} ${OUTPUT_02} -o ${OUTPUT_04} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Get deciduous pixels"
		OUTPUT_05="${WK_03}/Deciduous.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/applyMask -i ${OUTPUT_03} -m ${MASK_D} -o ${OUTPUT_05} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Get evergreen pixels"
		OUTPUT_06="${WK_03}/Evergreen.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/applyMask -i ${OUTPUT_04} -m ${MASK_E} -o ${OUTPUT_06} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Merge of different phenologies"
		OUTPUT_07="${WK_03}/Phenology_b1.tif"
		log "${MSG} ...\n"
		gdal_merge.py ${PAR_01} -n 0 ${OUTPUT_05} ${OUTPUT_06} -o ${OUTPUT_07} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		METADATA="SITE=${SITE},ID=PHENOLOGY,-9999=${PHENOLOGY_ID[0]},1=${PHENOLOGY_ID[1]},2=${PHENOLOGY_ID[2]}"
		MSG="Create multiband ${IMG} image"
		INPUT_01=(${OUTPUT_07} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND})
		OUTPUT_09="${WK_03}/${IMG}.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/mergeImg -b ${#INPUT_01[@]} -i ${INPUT_01[@]} -o ${OUTPUT_09} -m "${METADATA}" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Copy ${IMG} into ${OUT_03}"
		log "${MSG} ...\n"
		cp ${OUTPUT_09} -t ${OUT_03}
		check "${MSG} failed.\n"
		
		clean "${WK_03}"
    	
    	log "### .....stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Phenology execution }

### Management execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Management" ] ; then
    	log "### { Start creating ${IMG} images... ###\n"
    	
    	NAME_MASK_TOT="Total_mask.tif"
    	NAME_EMPTY_BAND="empty_geo.tif"
		MSG="Copy filters and empty band from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_MASK_TOT} ${OUT_00}/${NAME_EMPTY_BAND} -t ${WK_04}
		check "${MSG} failed.\n"
		
    	MASK_TOT="${WK_04}/${NAME_MASK_TOT}"
    	EMPTY_BAND="${WK_04}/${NAME_EMPTY_BAND}"
    	
    	IDX="1"
		MSG="Create a monoband image with every pixel set to ${IDX} (${MANAGEMENT_ID[${IDX}]})"
		OUTPUT_01="${WK_04}/Every_px_timber.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/createImg -x ${SIZEX} -y ${SIZEY} -b 1 -t float -v ${IDX} -c -n "${OUTPUT_01}" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Set georeference to ${OUTPUT_01}"
		OUTPUT_02="${WK_04}/Every_px_timber_geo.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/copyGeoref -i ${MASK_TOT} ${OUTPUT_01} -o ${OUTPUT_02} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Get timber pixels"
		OUTPUT_03="${WK_04}/Timber.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/applyMask -i ${OUTPUT_02} -m ${MASK_TOT} -o ${OUTPUT_03} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		METADATA="SITE=${SITE},ID=MANAGEMENT,-9999=${MANAGEMENT_ID[0]},1=${MANAGEMENT_ID[1]},2=${MANAGEMENT_ID[2]}"
		MSG="Create multiband ${IMG} image"
		INPUT_01=(${OUTPUT_03} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND})
		OUTPUT_04="${WK_04}/${IMG}.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/mergeImg -b ${#INPUT_01[@]} -i ${INPUT_01[@]} -o ${OUTPUT_04} -m "${METADATA}" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Copy ${IMG} into ${OUT_04}"
		log "${MSG} ...\n"
		cp ${OUTPUT_04} -t ${OUT_04}
		check "${MSG} failed.\n"
		
		clean "${WK_04}"
    	
    	log "### ....stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Management execution }

### NumHa execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "NumHa" ] ; then
    	log "### { Start creating ${IMG} images........ ###\n"
    	log "### .........stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - NumHa execution }

### AvDBH execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "AvDBH" ] ; then
    	log "### { Start creating ${IMG} images........ ###\n"
    	log "### .........stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - AvDBH execution }

### Height execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Height" ] ; then
    	log "### { Start creating ${IMG} images....... ###\n"
    	log "### ........stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Height execution }

### Wf execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Wf" ] ; then
    	log "### { Start creating ${IMG} images........... ###\n"
    	log "### ............stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Wf execution }

### Wrc execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Wrc" ] ; then
    	log "### { Start creating ${IMG} images.......... ###\n"
    	log "### ...........stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Wrc execution }

### Ws execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Ws" ] ; then
    	log "### { Start creating ${IMG} images........... ###\n"
    	log "### ............stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Ws execution }

### SolarRad execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "SolarRad" ] ; then
    	log "### { Start creating ${IMG} images..... ###\n"
    	
    	NAME_MASK_TOT="Total_mask.tif"
		MSG="Copy filter from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_MASK_TOT} -t ${WK_11}
		check "${MSG} failed.\n"
		
    	MASK_TOT="${WK_11}/${NAME_MASK_TOT}"
    	
    	UNITY="MJ/m2/day"
    	for INPUT_01 in $( ls ${IN_11}/*_rad_Nx*.hdf ) ; do
    		CURR_PROJ=$( gdalinfo HDF4_SDS:UNKNOWN:"${INPUT_01}":6 | grep SRS | cut -d '=' -f '2' )
    		UL_LONGITUDE=$( gdalinfo -stats HDF4_SDS:UNKNOWN:"${INPUT_01}":1 | grep STATISTICS_MINIMUM | cut -d '=' -f '2' )
    		LR_LONGITUDE=$( gdalinfo -stats HDF4_SDS:UNKNOWN:"${INPUT_01}":1 | grep STATISTICS_MAXIMUM | cut -d '=' -f '2' )
    		UL_LATITUDE=$(  gdalinfo -stats HDF4_SDS:UNKNOWN:"${INPUT_01}":2 | grep STATISTICS_MAXIMUM | cut -d '=' -f '2' )
    		LR_LATITUDE=$(  gdalinfo -stats HDF4_SDS:UNKNOWN:"${INPUT_01}":2 | grep STATISTICS_MINIMUM | cut -d '=' -f '2' )
    		
    		# UL and LR are inverted because the original image is upside down and mirrored: a flip is needed!
    		MSG="Extract solar radiation from hdf"
    		INPUT_02="HDF4_SDS:UNKNOWN:\"${INPUT_01}\":6" # swgdn subdataset
			OUTPUT_01="${WK_11}/$( basename $( echo ${INPUT_01} | sed s/.SUB.hdf/.tif/ | sed s/MERRA301.prod.assim.tavgM_2d_rad_Nx./swgdn-/) )"
			log "${MSG} ...\n"
			gdal_translate ${PAR_01} -a_srs "${CURR_PROJ}" -a_ullr ${UL_LONGITUDE} ${LR_LATITUDE} ${LR_LONGITUDE} ${UL_LATITUDE} ${INPUT_02} ${OUTPUT_01} &>> "${LOGFILE}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			DATE=$( basename ${OUTPUT_01} | cut -d '-' -f '2' | cut -d '.' -f '1' )
			MONTH="${DATE:4:2}"
			YEAR="${DATE:0:4}"
			
			MSG="Remap of UTM geotiff image"
			OUTPUT_02="$( echo ${OUTPUT_01} | sed s/.tif/-remapped/ )"
			log "${MSG} ...\n"	
			${BIN_DIR}/remap -i ${OUTPUT_01} -q -u -o ${OUTPUT_02} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			MSG="Get the subwindow needed"
    		INPUT_03="$( ls ${WK_11}/*${DATE}-remapped_33S.tif )" # Get only tile at zone 33S
			OUTPUT_03="$( echo ${INPUT_03} | sed s/-remapped_33S.tif/-warped.tif/ )"
			log "${MSG} ...\n"
			gdalwarp ${PAR_01} -t_srs "${PROJ}" -tr ${RES} -${RES} -te ${UL_LON} ${LR_LAT} ${LR_LON} ${UL_LAT} ${INPUT_03} ${OUTPUT_03} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			MSG="Mask ${IMG}"
			OUTPUT_04="$( echo ${OUTPUT_03} | sed s/-warped.tif/-masked.tif/ )"
			log "${MSG} ...\n"
			${BIN_DIR}/applyMask -i ${OUTPUT_03} -m ${MASK_TOT} -o ${OUTPUT_04} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			OUTPUT_05="$( echo ${OUTPUT_04} | sed s/-masked.tif/-final.tif/ )"
			VALUE_SOLAR_RAD="0.0864"
			MSG="Conversion from W/m2 to ${UNITY}"
			log "${MSG} ...\n"
			${BIN_DIR}/multiplyImgPx -i ${OUTPUT_04} -v "${VALUE_SOLAR_RAD}" -o ${OUTPUT_05} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
		done
		
		# Create output years images
		METADATA="SITE=${SITE},VALUES=SOLAR_RADIATION,UNITY_OF_MEASURE=${UNITY}"
		for YYYY in "${YEARS_PROC[@]}" ; do
			MONTHS=()
			for MM in "${MONTHS_PROC[@]}" ; do
				INPUT_04=$( ls ${WK_11}/*${YYYY}${MM}-final.tif )
				MONTHS+=("${INPUT_04}")				
			done
			
			MSG="Create multiband ${IMG} image"
			OUTPUT_06="${WK_11}/${IMG}_${YYYY}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/mergeImg -b ${#MONTHS[@]} -i ${MONTHS[@]} -o ${OUTPUT_06} -m "${METADATA}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
		
			MSG="Copy ${IMG} into ${OUT_11}"
			log "${MSG} ...\n"
			cp ${OUTPUT_06} -t ${OUT_11}
			check "${MSG} failed.\n"
		done
		
		clean "${WK_11}"
		
    	log "### ......stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  SolarRad execution }

### Avg_Temp execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Avg_Temp" ] ; then
    	log "### { Start creating ${IMG} images..... ###\n"
    	
    	# MODIS ID layer 916: Retrieved Temperature profile mean, band 20
    	# T(a)= m * (Z+2) + T
		# m è una costante = -0.0064
		# Z = valore del DEM
		# T è la temperatura della banda 20 ottenuta dalla formula sopra: T= factor_scale * (valore - add_offset) - 273.15

    	log "### ......stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Avg_Temp execution }

### VPD execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "VPD" ] ; then
    	log "### { Start creating ${IMG} images.......... ###\n"
    	
    	# layers 918 e 919 delle MOD08_M3
		# una volta che abbiamo la umidità relativa:
		# VPD = 0.6108 * e^( (17.27 * Tavg)/(237.3 + Tavg) ) * (1 - (RH/100))
    	
    	
    	log "### ...........stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - VPD execution }

### Precip execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Precip" ] ; then
    	log "### { Start creating ${IMG} images....... ###\n"
    	
    	NAME_MASK_TOT="Total_mask.tif"
		MSG="Copy filter from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_MASK_TOT} -t ${WK_14}
		check "${MSG} failed.\n"
		
    	MASK_TOT="${WK_14}/${NAME_MASK_TOT}"
    	
    	UNITY="mm/month"
    	for INPUT_01 in $( ls ${IN_14}/*.xml ) ; do
			DATE=$( cat ${INPUT_01} | grep 'RangeBeginningDate' | cut -f2 -d '>' | cut -f1 -d '<' )
			MONTH=${DATE:5:2}
			YEAR=${DATE:0:4}
			DATE_SHORT="${DATE:2:2}${DATE:5:2}${DATE:8:2}"
			INPUT_02=$( ls ${IN_14}/*${DATE_SHORT}*.nc)
	
			# Original precipitations data are mm/hh
			OUTPUT_01="${WK_14}/${IMG}_0_25_mm-hh_${DATE}.tif"
			MSG="Extraction of precipitations subdataset from ${INPUT_02}"
			log "${MSG} ...\n"
			gdal_translate ${PAR_01} -a_srs "${PROJ_LONGLAT}" NETCDF:"${INPUT_02}":pcp ${OUTPUT_01} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			# Data conversion from mm/hh into mm/month
			if [ "${MONTH}" == "11" ] || [ "${MONTH}" == "04" ] || [ "${MONTH}" == "06" ] || [ "${MONTH}" == "09" ] ; then
				HOURS_IN_MONTH=720.0 # 30*24=720 
			elif [ "${MONTH}" == "01" ] || [ "${MONTH}" == "03" ] || [ "${MONTH}" == "05" ] || [ "${MONTH}" == "07" ] || [ "${MONTH}" == "08" ] || [ "${MONTH}" == "10" ] || [ "${MONTH}" == "12" ] ; then
				HOURS_IN_MONTH=744.0 # 31*24=744
			elif [ "${MONTH}" == "02" ] ; then
				leapYear "${YEAR}"
				LEAP="${?}"
				if [ "${LEAP}" -eq "1" ] ; then
					HOURS_IN_MONTH=696.0 # 29*24=696
				else
					HOURS_IN_MONTH=672.0 # 28*24=672
				fi
			fi
			
			OUTPUT_02="${WK_14}/${IMG}_0_25_mm-${YEAR}${MONTH}.tif"
			MSG="Divide every pixel value for number of hours in month"
			log "${MSG} ...\n"
			${BIN_DIR}/multiplyImgPx -i ${OUTPUT_01} -v "${HOURS_IN_MONTH}" -o ${OUTPUT_02} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			MSG="Conversion of tiff projection from longlat to UTM"
			OUTPUT_03="${WK_14}/${IMG}_${YEAR}${MONTH}_utm.tif"
			log "${MSG} ...\n"
			gdalwarp ${PAR_01} -t_srs "${PROJ}" -tr ${RES} -${RES} ${OUTPUT_02} ${OUTPUT_03} &>> "${LOGFILE}"
			check "${MSG} failed on ${OUTPUT_02}.\n"
			
			MSG="Remap and cut UTM geotiff image"
			OUTPUT_04="${WK_14}/${IMG}_${YEAR}${MONTH}_remapped.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/remap -i ${OUTPUT_03} -o ${OUTPUT_04} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
			check "${MSG} failed on ${OUTPUT_03}.\n"
			
			MSG="Mask ${IMG}"
			OUTPUT_05="${WK_14}/${IMG}_${YEAR}${MONTH}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/applyMask -i ${OUTPUT_04} -m ${MASK_TOT} -o ${OUTPUT_05} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
    	done
    	
    	# Create output years images
		METADATA="SITE=${SITE},VALUES=PRECIPITATIONS,UNITY_OF_MEASURE=${UNITY}"
		for YYYY in "${YEARS_PROC[@]}" ; do
			MONTHS=()
			for MM in "${MONTHS_PROC[@]}" ; do
				INPUT_03=$( ls ${WK_14}/${IMG}_${YYYY}${MM}.tif )
				MONTHS+=("${INPUT_03}")				
			done
			
			MSG="Create multiband ${IMG} image"
			OUTPUT_06="${WK_14}/${IMG}_${YYYY}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/mergeImg -b ${#MONTHS[@]} -i ${MONTHS[@]} -o ${OUTPUT_06} -m "${METADATA}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
		
			MSG="Copy ${IMG} into ${OUT_14}"
			log "${MSG} ...\n"
			cp ${OUTPUT_06} -t ${OUT_14}
			check "${MSG} failed.\n"
		done
    	
    	clean "${WK_14}"
    	
    	log "### ........stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Precip execution }

### LAI execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "LAI" ] ; then
    	log "### { Start creating ${IMG} images.......... ###\n"
    	
    	#for INPUT_01 in $( ls ${IN_15}/LayerDates_NDVI_YearlyLambda500_year2000 ) ; do
		for INPUT_01 in $( ls ${IN_15}/LayerDates* ) ; do
    		INPUT_02="$( echo ${INPUT_01} | sed s/LayerDates_// | sed s/$/.tif/ )"

    		IDX="1"
    		MONTH_PREV="01"
    		SAME_MONTH_IMG=()
    		LAI_MONTHS=()
    		while read LINE; do
    			DATE=${LINE:0:10}
    			OUTPUT_01="${WK_15}/NDVI_Lambda500_${DATE}.tif"
				MSG="Extraction of band ${IDX} from ${INPUT_02}"
				log "${MSG} ...\n"
    			gdal_translate ${PAR_01} -b ${IDX} ${INPUT_02} ${OUTPUT_01}
				check "${MSG} failed.\n"
    			
    			MONTH=${LINE:5:2}
    			YEAR=${LINE:0:4}
    			
    			if [ "${MONTH}" == "${MONTH_PREV}" ] ; then
    				SAME_MONTH_IMG+=("${OUTPUT_01}")
				else
					MSG="Getting average for month ${MONTH_PREV}"
					OUTPUT_02="${WK_15}/NDVI_Lambda500_Averaged_${YEAR}-${MONTH_PREV}.tif"
					log "${MSG} ...\n"	
					${BIN_DIR}/calcAverage -n ${#SAME_MONTH_IMG[@]} -i "${SAME_MONTH_IMG[@]}" -o ${OUTPUT_02} &>> "${LOGFILE}"
					check "${MSG} failed.\n"
					
					OUTPUT_03="${WK_15}/NDVI_${YEAR}-${MONTH_PREV}.tif"
					MSG="Divide every pixel value per 10000"
					log "${MSG} ...\n"
					${BIN_DIR}/multiplyImgPx -i ${OUTPUT_02} -v 0.0001 -o ${OUTPUT_03} &>> "${LOGFILE}"
					check "${MSG} failed.\n"
					
					# For December, January, February and March put LAI to 0.0 if its value is < 1.0
					if [ "${MONTH_PREV}" == "01" ] || [ "${MONTH_PREV}" == "02" ] || [ "${MONTH_PREV}" == "03" ] ; then
						THRESHOLD="-t 1.0"
					else
						THRESHOLD=""
					fi
					
					OUTPUT_04="${WK_15}/LAI_${YEAR}-${MONTH_PREV}.tif"
					MSG="Get LAI from NDVI"
					log "${MSG} ...\n"
					${BIN_DIR}/getLAI -i ${OUTPUT_03} -o ${OUTPUT_04} ${THRESHOLD} &>> "${LOGFILE}"
					check "${MSG} failed.\n"
					
					LAI_MONTHS+=("${OUTPUT_04}")
			
					SAME_MONTH_IMG=()
					SAME_MONTH_IMG+=("${OUTPUT_01}")
    			fi
				MONTH_PREV=${MONTH}
    			IDX=$(( ${IDX} + 1 ));
			done < "${INPUT_01}"
			# Work on last month (December)
			MSG="Getting average for month ${MONTH}"
			OUTPUT_02="${WK_15}/NDVI_Lambda500_Averaged_${YEAR}-${MONTH}.tif"
			log "${MSG} ...\n"	
			${BIN_DIR}/calcAverage -n ${#SAME_MONTH_IMG[@]} -i "${SAME_MONTH_IMG[@]}" -o ${OUTPUT_02} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			OUTPUT_03="${WK_15}/NDVI_${YEAR}-${MONTH}.tif"
			MSG="Divide every pixel value per 10000"
			log "${MSG} ...\n"
			${BIN_DIR}/multiplyImgPx -i ${OUTPUT_02} -v 0.0001 -o ${OUTPUT_03} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			OUTPUT_04="${WK_15}/LAI_${YEAR}-${MONTH}.tif"
			MSG="Get LAI from NDVI"
			log "${MSG} ...\n"
			${BIN_DIR}/getLAI -i ${OUTPUT_03} -o ${OUTPUT_04} -t 1.0 &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			LAI_MONTHS+=("${OUTPUT_04}")
			
			OUTPUT_05="${WK_15}/LAI_${YEAR}.tif"
			MSG="Get multiband LAI image"
			log "${MSG} ...\n"
			${BIN_DIR}/mergeImg -b ${#LAI_MONTHS[@]} -i "${LAI_MONTHS[@]}" -o ${OUTPUT_05} -m VALUE=LAI,YEAR=${YEAR},SITE=${SITE}
			check "${MSG} failed.\n"
			
			MSG="Copy LAI multiband image into output dir"
			log "${MSG} ...\n"
			cp ${OUTPUT_05} ${OUT_15}
			check "${MSG} failed.\n"
		done
		
    	clean "${WK_15}"

    	log "### ...........stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - LAI execution }

### Soil execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Soil" ] ; then
    	log "### { Start creating ${IMG} images......... ###\n"
    	
    	NAME_MASK_TOT="Total_mask.tif"
		MSG="Copy filter from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_MASK_TOT} -t ${WK_16}
		check "${MSG} failed.\n"
		
    	MASK_TOT="${WK_16}/${NAME_MASK_TOT}"
    	
    	for INPUT_01 in $( ls ${IN_16}/*.asc ) ; do
			MSG="Conversion from ASCII corine format into geotiff of ${INPUT_01}"
			OUTPUT_01="${WK_16}/$( basename $( echo ${INPUT_01} | sed s/.asc/.tif/ ) )"
			log "${MSG} ...\n"
			gdal_translate ${PAR_01} -a_srs "${PROJ_32}" ${INPUT_01} ${OUTPUT_01} &>> "${LOGFILE}"
			check "${MSG} failed on ${INPUT_01}.\n"
	
			MSG="Changing zone from 32 to 33 of ${OUTPUT_01}"
			OUTPUT_02="${WK_16}/$( basename $( echo ${INPUT_01} | sed s/.asc/_zone33.tif/ ) )"
			log "${MSG} ...\n"
			gdalwarp ${PAR_01} -t_srs "${PROJ}" ${OUTPUT_01} ${OUTPUT_02} &>> "${LOGFILE}"
			check "${MSG} failed on ${OUTPUT_01}.\n"

			MSG="Remap of UTM geotiff image"
			OUTPUT_03="${WK_16}/$( basename $( echo ${INPUT_01} | sed s/.asc/_30m.tif/ ) )"
			log "${MSG} ...\n"	
			${BIN_DIR}/remap -i ${OUTPUT_02} -o ${OUTPUT_03} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
			check "${MSG} failed on ${OUTPUT_02}.\n"
			
			MSG="Mask ${IMG}"
			OUTPUT_04="${WK_16}/$( basename $( echo ${INPUT_01} | sed s/.asc/_30m_masked.tif/ ) )"
			log "${MSG} ...\n"
			${BIN_DIR}/applyMask -i ${OUTPUT_03} -m ${MASK_TOT} -o ${OUTPUT_04} &>> "${LOGFILE}"
			check "${MSG} failed.\n"	
		done
		
		# Prepare layers for multiband image
		INPUT_02+=("$( ls ${WK_16}/*argilla*30m_masked.tif)" "$( ls ${WK_16}/*limo*30m_masked.tif)" "$( ls ${WK_16}/*sabbia*30m_masked.tif)" "$( ls ${WK_16}/*densita*30m_masked.tif)")
		
		METADATA="SITE=${SITE},VALUE=SOIL,BAND1=clay (%),BAND2=silt (%),BAND3=sand (%),BAND4=density (g/cm3)"
		MSG="Create multiband ${IMG} image"
		OUTPUT_05="${WK_16}/${IMG}.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/mergeImg -b ${#INPUT_02[@]} -i ${INPUT_02[@]} -o ${OUTPUT_05} -m "${METADATA}"
		check "${MSG} failed.\n"
		
		MSG="Copy ${IMG} into ${OUT_16}"
		log "${MSG} ...\n"
		cp ${OUTPUT_05} -t ${OUT_16}
		check "${MSG} failed.\n"

		clean "${WK_16}"
    	
    	log "### ..........stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Soil execution }

exit 0