#!/bin/bash
### File information summary  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
# Description:    createInputFutureClimMadonie.sh bash shell script
#                 Processing chain to create input images for Forest Scenarios Evolution project (ForSE).
# Author:         Alessandro Candini - candini@meeo.it
# Version:        0.1
# Copyright:      MEEO S.R.L. - www.meeo.it 
# How it works:   It takes...
# Changelog:      2012-12-11 - version 0.1
#                     - First Release
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  File information summary }

### Global variables definitions  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
VERSION="0.1"
SCRIPT_NAME="${0:2:-3}"
AOI="Parco delle Madonie (Sicily)"
SITE="MADONIE"
PREF="Madonie"
SUBDIR="ClimFuture"
MODULES=(remap applyMask calcAverage multiplyImgPx getLAI getVPD createImg mergeImg specFromMaxPerc copyGeoref reduceToBinaryMask)
IMG_ALL=(Filters SolarRad Avg_Temp VPD Precip LAI Packet)
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
SPECIES_ID_PRESENT=(1 2 6 7)

# Some parameters values per specie:
Y_PLANTED_ID=(Undefined 1981 1963 null null null 1991 1986)
N_CELL_ID=(Undefined 5 49 null null null 12 11)
AVDBH_ID=(Undefined 18.33 26.697 null null null 12.387 18.473)
HEIGHT_ID=(Undefined 12.79 12.427 null null null 7.04 7.0)

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
IN_11="$( dirname ${0} )/../input/11_SolarRad/${SUBDIR}"
IN_12="$( dirname ${0} )/../input/12_Avg_Temp/${SUBDIR}"
IN_13="$( dirname ${0} )/../input/13_VPD/${SUBDIR}"
IN_14="$( dirname ${0} )/../input/14_Precip/${SUBDIR}"
IN_15="$( dirname ${0} )/../input/15_LAI"
IN_17="$( dirname ${0} )/../input/17_Packet"
# Output directories
OUT_00="$( dirname ${0} )/../output/00_Filters"
OUT_11="$( dirname ${0} )/../output/11_SolarRad"
OUT_12="$( dirname ${0} )/../output/12_Avg_Temp"
OUT_13="$( dirname ${0} )/../output/13_VPD"
OUT_14="$( dirname ${0} )/../output/14_Precip"
OUT_15="$( dirname ${0} )/../output/15_LAI"
OUT_17="$( dirname ${0} )/../output/17_Packet"
# Working directories
WK_00="$( dirname ${0} )/../working/00_Filters"
WK_11="$( dirname ${0} )/../working/11_SolarRad"
WK_12="$( dirname ${0} )/../working/12_Avg_Temp"
WK_13="$( dirname ${0} )/../working/13_VPD"
WK_14="$( dirname ${0} )/../working/14_Precip"
WK_15="$( dirname ${0} )/../working/15_LAI"
WK_17="$( dirname ${0} )/../working/17_Packet"

# Output geotiff size:
SIZEX="1286"
SIZEY="1160"

# Textual files extents (lat,lon and x,y values)
SIZEX_EUROPE="258"
SIZEY_EUROPE="228"
UL_EUR_LONGITUDE="-11"
UL_EUR_LATITUDE="72"
LR_EUR_LONGITUDE="32"
LR_EUR_LATITUDE="34"
START_X="150"
END_X="152"
START_Y="23"
END_Y="25"
UL_LONGITUDE="13.8333333333"
UL_LATITUDE="38.1666666666"
LR_LONGITUDE="14.3333333333"
LR_LATITUDE="37.6666666666"

# Output geotiff resolution:
RES="30"
# Output geotiff Upper Left and Lower Right point coordinates:
UL_LON="399333.291887304978445"
UL_LAT="4211980.487859229557216"
LR_LON="437913.291887304978445"
LR_LAT="4177180.487859229557216"
AOI_ZONE="33S"
# Geotiff projections (proj4 definitions from spatialreference.org):
PROJ="+proj=utm +zone=33 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
PROJ_32="+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
PROJ_LONGLAT="+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs"
PROJ_3004="+proj=tmerc +lat_0=0 +lon_0=15 +k=0.9996 +x_0=2520000 +y_0=0 +ellps=intl +units=m +no_defs"
PAR_01="-q -co COMPRESS=LZW -of GTiff"
PAR_02="-ot Float32"
PAR_03="-q -of XYZ"
PAR_05="${PAR_01} ${PAR_02} -separate"
# Lat-lon extents
WORLD="-180 90 180 -90"
SICILY="13 39 15 37"

# Temporal coverage
YEARS_PROC=(2010 2011 2012 2013 2014 2015 2016 2017 2018 2019 2020)
FIRST_YEAR="${YEARS_PROC[0]}"
LAST_YEAR="${YEARS_PROC[$((${#YEARS_PROC[@]}-1))]}"
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
	echo "Run example: ${0} Phenology LAI Wrc N_cell"
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

findNull(){
	MISSING="0"
	for X in 0 1 ; do
		for Y in 0 1 ; do
			VAL=$( gdallocationinfo -valonly "${1}" ${X} ${Y} )
			if [ "${VAL}" == "-9999" ] ; then
				MISSING="1"
			fi
		done
	done
	return ${MISSING}
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
		OUTPUT_03="${WK_00}/${SPECIES_ID[${IDX}]}_${IDX}_remapped.tif"
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
		OUTPUT_05="${WK_00}/${SPECIES_ID[${IDX}]}_${IDX}_remapped.tif"
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
		OUTPUT_07="${WK_00}/${SPECIES_ID[${IDX}]}_${IDX}_remapped.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/remap -i ${OUTPUT_06} -o ${OUTPUT_07} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="7"
		MSG="Conversion of Madonie shapefile into GeoTiff (${SPECIES_ID[${IDX}]})"
		OUTPUT_08="${WK_00}/${PREF}_${SPECIES_ID[${IDX}]}_${IDX}.tif"
		log "${MSG} ...\n"
		gdal_rasterize ${PAR_01} ${PAR_02} -sql 'SELECT * FROM CFRS_ParcoMadonie_utm WHERE COD_CATEG="LE" OR COD_CATEG="SU"' -burn ${IDX} -tr ${RES} -${RES} ${OUTPUT_01} ${OUTPUT_08} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Remap and cut UTM geotiff image"
		OUTPUT_09="${WK_00}/${SPECIES_ID[${IDX}]}_${IDX}_remapped.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/remap -i ${OUTPUT_08} -o ${OUTPUT_09} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		FILTER_01="${OUTPUT_03}"
		FILTER_02="${OUTPUT_05}"
		FILTER_06="${OUTPUT_07}"
		FILTER_07="${OUTPUT_09}"
		
		MSG="Merge of deciduous species"
		FILTER_D="${WK_00}/${PREF}_deciduous_filter.tif"
		log "${MSG} ...\n"
		gdal_merge.py ${PAR_01} -n 0 ${FILTER_01} ${FILTER_02} ${FILTER_06} -o ${FILTER_D} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		FILTER_E="${FILTER_07}"
		
		MSG="Get a binary mask of type GDT_Byte"
		MASK_D="${WK_00}/${PREF}_deciduous_mask.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/reduceToBinaryMask -i ${FILTER_D} -o ${MASK_D} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Get a binary mask of type GDT_Byte"
		MASK_E="${WK_00}/${PREF}_evergreen_mask.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/reduceToBinaryMask -i ${FILTER_E} -o ${MASK_E} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Get a total mask"
		MASK_TOT="${WK_00}/${PREF}_total_mask.tif"
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
		
		IDX="1"
		MSG="Get a binary mask of ${FILTER_01}"
		MASK_1="${WK_00}/${PREF}_${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/reduceToBinaryMask -i ${FILTER_01} -o ${MASK_1} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="2"
		MSG="Get a binary mask of ${FILTER_02}"
		MASK_2="${WK_00}/${PREF}_${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/reduceToBinaryMask -i ${FILTER_02} -o ${MASK_2} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="6"
		MSG="Get a binary mask of ${FILTER_06}"
		MASK_6="${WK_00}/${PREF}_${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/reduceToBinaryMask -i ${FILTER_06} -o ${MASK_6} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="7"
		MSG="Get a binary mask of ${FILTER_07}"
		MASK_7="${WK_00}/${PREF}_${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/reduceToBinaryMask -i ${FILTER_07} -o ${MASK_7} &>> "${LOGFILE}"
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

		log "Start creating dem file ...\n"
		for INPUT_01 in $( ls ${IN_00}/*.zip ) ; do
    		MSG="Unzipping DEM files"
			log "${MSG} ...\n"
			unzip ${INPUT_01} -d ${WK_00} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
		
			MSG="Removing useless stuff"
			log "${MSG} ...\n"
			rm -f ${WK_00}/*.pdf ${WK_00}/*_num.tif
			check "${MSG} failed.\n"
    	done
    	
		MSG="Merge adjacent dem files"
		INPUT_02=($( ls ${WK_00}/*_dem.tif ))
		OUTPUT_01="${WK_00}/Sicily_dem.tif"
		log "${MSG} ...\n"
		gdal_merge.py ${PAR_01} ${PAR_02} ${INPUT_02[@]} -o ${OUTPUT_01} &>> "${LOGFILE}"
    	check "${MSG} failed.\n"
    	
    	MSG="Conversion of tiff projection from longlat to UTM"
		OUTPUT_02="${WK_00}/Sicily_dem_no_remap.tif"
		log "${MSG} ...\n"
		gdalwarp ${PAR_01} -t_srs "${PROJ}" -tr ${RES} -${RES} ${OUTPUT_01} ${OUTPUT_02} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Remap and cut UTM geotiff image"
		OUTPUT_03="${WK_00}/${PREF}_dem.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/remap -i ${OUTPUT_02} -o ${OUTPUT_03} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		M="-0.0064"
		MSG="Performing DEM rescaling"
		DEM_SCALED="${WK_00}/${PREF}_dem_scaled.tif"
		log "${MSG} ...\n"
		gdal_calc.py -A ${OUTPUT_03} --outfile=${DEM_SCALED} --calc="(${M})*(A+2)" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		log "End creating dem file .....\n"
		
		MSG="Copy masks, empty band and scaled DEM into output dir"
		log "${MSG} ...\n"
		cp ${MASK_D} ${MASK_E} ${MASK_TOT} ${OUTPUT_11} ${DEM_SCALED} ${MASK_1} ${MASK_2} ${MASK_6} ${MASK_7} -t ${OUT_00}
		check "${MSG} failed.\n"
		
		MSG="Copy masks into Species output dir (put into packet for testing/tuning purposes)"
		log "${MSG} ...\n"
		cp ${MASK_D} ${MASK_E} ${MASK_TOT} ${MASK_1} ${MASK_2} ${MASK_6} ${MASK_7} -t ${OUT_02}
		check "${MSG} failed.\n"

		clean "${WK_00}"

		log "### .......stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Filters execution }

### SolarRad execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "SolarRad" ] ; then
    	log "### { Start creating ${IMG} images..... ###\n"
		
		YEARS=(2002 2003 2004 2005 2006 2007 2008 2009)
		MONTHS_SR=(1 2 3 4 5 6 7 8 9 10 11 12)
		
		for Y in ${YEARS[@]} ; do
			for M in ${MONTHS_SR[@]} ; do
				INPUT_01="${IN_11}/${IMG}_${Y}.tif"
				OUTPUT_01="${WK_11}/${IMG}_${Y}_${M}.tif"
				gdal_translate ${PAR_01} -b ${M} ${INPUT_01} ${OUTPUT_01} &>> "${LOGFILE}"
			done
		done

		SR_FINAL=()
		for M in ${MONTHS_SR[@]} ; do
			INPUT_01="-A ${WK_11}/${IMG}_2002_${M}.tif -B ${WK_11}/${IMG}_2003_${M}.tif -C ${WK_11}/${IMG}_2004_${M}.tif -D ${WK_11}/${IMG}_2005_${M}.tif -E ${WK_11}/${IMG}_2006_${M}.tif -F ${WK_11}/${IMG}_2007_${M}.tif -G ${WK_11}/${IMG}_2008_${M}.tif -H ${WK_11}/${IMG}_2009_${M}.tif"
			OUTPUT_01="${WK_11}/${IMG}_${M}.tif"
			gdal_calc.py ${INPUT_01} --outfile=${OUTPUT_01} --calc="(A+B+C+D+E+F+G+H)/${#YEARS[@]}" &>/dev/null
			SR_FINAL+=("${OUTPUT_01}")
		done
		
		UNITY="MJ/m2/day"
		METADATA="-mo SITE=${SITE} -mo VALUES=SOLAR_RADIATION -mo UNITY_OF_MEASURE=${UNITY}"
		
		MSG="Create merged ${IMG} image for ${YEARS_PROC[0]}"
		OUTPUT_10="${WK_11}/${IMG}_${YEARS_PROC[0]}_merged.tif"
		log "${MSG} ...\n"
		gdal_merge.py ${PAR_05} ${SR_FINAL[@]} -o ${OUTPUT_10}  &>> "${LOGFILE}"
		check "${MSG} failed.\n"
			
		MSG="Add metadata to ${IMG} image and compress it for ${YYYY}"
		OUTPUT_11="${WK_11}/${IMG}_${YEARS_PROC[0]}.tif"
		log "${MSG} ...\n"
		gdal_translate ${PAR_01} ${METADATA} ${OUTPUT_10} ${OUTPUT_11} &>> "${LOGFILE}" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		OUTPUT_12="${WK_11}/${IMG}_${YEARS_PROC[1]}.tif"
		OUTPUT_13="${WK_11}/${IMG}_${YEARS_PROC[2]}.tif"
		OUTPUT_14="${WK_11}/${IMG}_${YEARS_PROC[3]}.tif"
		OUTPUT_15="${WK_11}/${IMG}_${YEARS_PROC[4]}.tif"
		OUTPUT_16="${WK_11}/${IMG}_${YEARS_PROC[5]}.tif"
		OUTPUT_17="${WK_11}/${IMG}_${YEARS_PROC[6]}.tif"
		OUTPUT_18="${WK_11}/${IMG}_${YEARS_PROC[7]}.tif"
		OUTPUT_19="${WK_11}/${IMG}_${YEARS_PROC[8]}.tif"
		OUTPUT_20="${WK_11}/${IMG}_${YEARS_PROC[9]}.tif"
		OUTPUT_21="${WK_11}/${IMG}_${YEARS_PROC[10]}.tif"
		
		cp ${OUTPUT_11} ${OUTPUT_12}
		cp ${OUTPUT_11} ${OUTPUT_13}
		cp ${OUTPUT_11} ${OUTPUT_14}
		cp ${OUTPUT_11} ${OUTPUT_15}
		cp ${OUTPUT_11} ${OUTPUT_16}
		cp ${OUTPUT_11} ${OUTPUT_17}
		cp ${OUTPUT_11} ${OUTPUT_18}
		cp ${OUTPUT_11} ${OUTPUT_19}
		cp ${OUTPUT_11} ${OUTPUT_20}
		cp ${OUTPUT_11} ${OUTPUT_21}		
		
		MSG="Copy ${IMG} into ${OUT_11}"
		log "${MSG} ...\n"
		cp ${OUTPUT_11} ${OUTPUT_12} ${OUTPUT_13} ${OUTPUT_14} ${OUTPUT_15} ${OUTPUT_16} ${OUTPUT_17} ${OUTPUT_18} ${OUTPUT_19} ${OUTPUT_20} ${OUTPUT_21} -t ${OUT_11}
		check "${MSG} failed.\n"
				
		clean "${WK_11}"
		
    	log "### ......stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  SolarRad execution }

### Avg_Temp execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Avg_Temp" ] ; then
    	log "### { Start creating ${IMG} images..... ###\n"
    	
    	# Average Temperature pixel value ( T(a) )
		# T(a) = m * (Z+2) + T(o)
		# m = -0.0064 
		# Z = DEM pixel value
		# T(o) = pixel value
    	
    	NAME_MASK_TOT="${PREF}_total_mask.tif"
		MSG="Copy filter from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_MASK_TOT} -t ${WK_12}
		check "${MSG} failed.\n"
		
		NAME_DEM_SCALED="${PREF}_dem_scaled.tif"
		MSG="Copy DEM from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_DEM_SCALED} -t ${WK_12}
		check "${MSG} failed.\n"
		
    	MASK_TOT="${WK_12}/${NAME_MASK_TOT}"
    	DEM_SCALED="${WK_12}/${NAME_DEM_SCALED}"
    	
#    	INPUT_01=$( ls ${IN_12}/*.tmp )
#    	MISSING=$(  cat ${INPUT_01} | grep -w "Missing=[ \t]*[0-9]*" | awk -F"=" '{ print $5 }' | tr -d ']' ) 
#    	MULTI=$(    cat ${INPUT_01} | grep -w "Multi=[ \t]*[0-9]*"   | awk -F"=" '{ print $4 }' | tr -d '] [Missing' )
#    	
#    	PX_COORDS=()
#		for (( X=${START_X}; X<=${END_X}; X++ )) ; do
#			for (( Y=${START_Y}; Y<=${END_Y}; Y++ )) ; do
#				PX_COORDS+=("${X},${Y}")
#			done
#		done
#    	
#    	for YYYY in "${YEARS_PROC[@]}" ; do
#			for MM in "${MONTHS_PROC[@]}" ; do
#			
#				MSG="Create an empty monoband image for ${YYYY}-${MM}"
#				OUTPUT_01="${WK_12}/Europe_empty_${YYYY}${MM}.tif"
#				log "${MSG} ...\n"
#				${BIN_DIR}/createImg -x ${SIZEX_EUROPE} -y ${SIZEY_EUROPE} -b 1 -t float -v 0 -c -n "${OUTPUT_01}" &>> "${LOGFILE}"
#				check "${MSG} failed.\n" 
#		
#				MSG="Conversion of GeoTiff to a textual file for ${YYYY}-${MM}"
#				OUTPUT_02="${WK_12}/${IMG}_${YYYY}${MM}.txt"
#				log "${MSG} ...\n"
#				gdal_translate ${PAR_03} ${OUTPUT_01} ${OUTPUT_02} &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#				
#				MSG="Remove empty GeoTiff for ${YYYY}-${MM}"
#				log "${MSG} ...\n"
#				rm ${OUTPUT_01} &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#
#			done
#		done
#		
#		for P in ${PX_COORDS[@]} ; do
#			X_COORD=$( echo ${P} | cut -d ',' -f '1' )
#			Y_COORD=$( echo ${P} | cut -d ',' -f '2' )
#			LINE_NUM=$( cat ${INPUT_01} | grep -wn "[ \t]*${X_COORD},[ \t]*${Y_COORD}" | cut -d ':' -f '1' )
#			if [ "${LINE_NUM}" != "" ] ; then # If this cell exists in the dataset
#				PREC="${LINE_NUM}"
#				J="1"
#				for YYYY in "${YEARS_PROC[@]}" ; do
#					IDX=$( echo "${LINE_NUM}+${J}" | bc )
#					YEAR_DATA=( $( sed -n -e "${IDX},${IDX}p" ${INPUT_01} ) )
#					K="0"
#					for MM in "${MONTHS_PROC[@]}" ; do
#						ORIG_VAL="${YEAR_DATA[${K}]}"
#						if [ "${ORIG_VAL}" == "${MISSING}" ] ; then
#							ORIG_VAL="0"
#						fi
#						SCALED_VAL=$( echo ${ORIG_VAL}*${MULTI} | bc | sed 's/^\./0./' )
#						# Gdal indexes starts from 0, not from 1 and in textual files there is the center of the pixel (i.e.: 0,0 --> 0.5,0.5)
#						# Gdal starts from UL, not from LL as textual files
#						NEW_X_COORD=$( echo "(${X_COORD}-1)+0.5" | bc | sed 's/^\./0./' )
#						NEW_Y_COORD=$( echo "((${SIZEY_EUROPE}-1)-(${Y_COORD}-1))+0.5" | bc | sed 's/^\./0./' )
#						
#						MSG="Change textual file in cell ${X_COORD}, ${Y_COORD} (${NEW_X_COORD}, ${NEW_Y_COORD}) content for ${YYYY}-${MM}"
#						OUTPUT_03="${WK_12}/${IMG}_${YYYY}${MM}.txt"
#						log "${MSG} ...\n"
#						sed -i 's/\<'${NEW_X_COORD}' '${NEW_Y_COORD}' 0\>/'${NEW_X_COORD}' '${NEW_Y_COORD}' '${SCALED_VAL}'/' "${OUTPUT_03}" &>> "${LOGFILE}"
#						check "${MSG} failed.\n"
#						
#						let "K += 1"
#					done
#					let "J += 1"
#				done
#			else # If this cell is missing from the dataset take the previous values
#				J="1"
#				for YYYY in "${YEARS_PROC[@]}" ; do
#					IDX=$( echo "${PREC}+${J}" | bc )
#					YEAR_DATA=( $( sed -n -e "${IDX},${IDX}p" ${INPUT_01} ) )
#					K="0"
#					for MM in "${MONTHS_PROC[@]}" ; do
#						ORIG_VAL="${YEAR_DATA[${K}]}"
#						if [ "${ORIG_VAL}" == "${MISSING}" ] ; then
#							ORIG_VAL="0"
#						fi
#						SCALED_VAL=$( echo ${ORIG_VAL}*${MULTI} | bc | sed 's/^\./0./' )
#						# Gdal indexes starts from 0, not from 1 and in textual files there is the center of the pixel (i.e.: 0,0 --> 0.5,0.5)
#						# Gdal starts from UL, not from LL as textual files
#						NEW_X_COORD=$( echo "(${X_COORD}-1)+0.5" | bc | sed 's/^\./0./' )
#						NEW_Y_COORD=$( echo "((${SIZEY_EUROPE}-1)-(${Y_COORD}-1))+0.5" | bc | sed 's/^\./0./' )
#						
#						MSG="Change textual file in cell ${X_COORD}, ${Y_COORD} (${NEW_X_COORD}, ${NEW_Y_COORD}) content for ${YYYY}-${MM}"
#						OUTPUT_03="${WK_12}/${IMG}_${YYYY}${MM}.txt"
#						log "${MSG} ...\n"
#						sed -i 's/\<'${NEW_X_COORD}' '${NEW_Y_COORD}' 0\>/'${NEW_X_COORD}' '${NEW_Y_COORD}' '${SCALED_VAL}'/' "${OUTPUT_03}" &>> "${LOGFILE}"
#						check "${MSG} failed.\n"
#						
#						let "K += 1"
#					done
#					let "J += 1"
#				done
#			fi
#		done

		cp ~/Desktop/${IMG}_tmp/* ${WK_12}
		
		UNITY="kPa"
		METADATA="-mo SITE=${SITE} -mo VALUES=AVERAGE_TEMPERATURE -mo UNITY_OF_MEASURE=${UNITY}"
		for YYYY in "${YEARS_PROC[@]}" ; do
			MONTHS=()
			for MM in "${MONTHS_PROC[@]}" ; do
				MSG="Reconvert textual files into GeoTiff content for ${YYYY}-${MM}"
				INPUT_02="${WK_12}/${IMG}_${YYYY}${MM}.txt"
				OUTPUT_04="${WK_12}/${IMG}_${YYYY}${MM}_latlon.tif"
				log "${MSG} ...\n"
				gdal_translate ${PAR_01} -a_srs "${PROJ_LONGLAT}" -a_ullr ${UL_EUR_LONGITUDE} ${UL_EUR_LATITUDE} ${LR_EUR_LONGITUDE} ${LR_EUR_LATITUDE} ${INPUT_02} ${OUTPUT_04} &>> "${LOGFILE}" &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Take a sub window of GeoTiff ${YYYY}-${MM}"
				OUTPUT_05="${WK_12}/${IMG}_${YYYY}${MM}_latlon_cut.tif"
				log "${MSG} ...\n"
				gdal_translate ${PAR_01} -a_srs "${PROJ_LONGLAT}" -projwin ${UL_LONGITUDE} ${UL_LATITUDE} ${LR_LONGITUDE} ${LR_LATITUDE} ${OUTPUT_04} ${OUTPUT_05} &>> "${LOGFILE}" &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Conversion of tiff projection from longlat to UTM for ${YYYY}-${MM}"
				OUTPUT_06="${WK_12}/${IMG}_${YYYY}${MM}_utm.tif"
				log "${MSG} ...\n"
				gdalwarp ${PAR_01} -t_srs "${PROJ}" -tr ${RES} -${RES} ${OUTPUT_05} ${OUTPUT_06} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Remap and cut UTM geotiff image for ${YYYY}-${MM}"
				OUTPUT_07="${WK_12}/${IMG}_${YYYY}${MM}_remap.tif"
				log "${MSG} ...\n"
				${BIN_DIR}/remap -i ${OUTPUT_06} -o ${OUTPUT_07} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Applying scaled dem to pixel value for ${YYYY}-${MM}"
				OUTPUT_08="${WK_12}/${IMG}_${YYYY}${MM}_dem.tif"
				log "${MSG} ...\n"
				gdal_calc.py -A ${DEM_SCALED} -B ${OUTPUT_07} --outfile=${OUTPUT_08} --calc="(A+B)" &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Apply mask to ${OUTPUT_08}"
				OUTPUT_09="${WK_12}/${IMG}_${YYYY}${MM}_mask.tif"
				log "${MSG} ...\n"
				gdal_calc.py -A ${OUTPUT_08} -B ${MASK_TOT} --outfile=${OUTPUT_09} --calc="(A*B)" &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MONTHS+=("${OUTPUT_09}")
			done
			
			MSG="Create merged ${IMG} image for ${YYYY}"
			OUTPUT_10="${WK_12}/${IMG}_${YYYY}_merged.tif"
			log "${MSG} ...\n"
			gdal_merge.py ${PAR_05} ${MONTHS[@]} -o ${OUTPUT_10}  &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			MSG="Add metadata to ${IMG} image and compress it for ${YYYY}"
			OUTPUT_11="${WK_12}/${IMG}_${YYYY}.tif"
			log "${MSG} ...\n"
			gdal_translate ${PAR_01} ${METADATA} ${OUTPUT_10} ${OUTPUT_11} &>> "${LOGFILE}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			MSG="Copy ${IMG} into ${OUT_12}"
			log "${MSG} ...\n"
			cp ${OUTPUT_11} -t ${OUT_12}
			check "${MSG} failed.\n"
			
		done

		clean "${WK_12}"

    	log "### ......stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Avg_Temp execution }

### VPD execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "VPD" ] ; then
    	log "### { Start creating ${IMG} images.......... ###\n"
    	
    	# VPD pixel value ( VPD )
		# VPD = e(s) - e(a)
		# e(s)= (e(T_max(a)) + e(T_min(a))) / 2
		# e(T_max(a)) = 0.6108 * e^((17.27 * T_max(a))/(T_max(a) + 237.3))
		# e(T_min(a)) = 0.6108 * e^((17.27 * T_min(a))/(T_min(a) + 237.3))
		# e = 2.71828182845904523536028747135266249775724709369995... (Napier's constant)
		# e(a) = e(T_min(a))
		# T_min(a) = m * (Z+2) + T_min(o)
		# T_max(a) = m * (Z+2) + T_max(o)
		# m = -0.0064 
		# Z = DEM pixel value
		# T_min(o) = pixel value
		# T_max(o) = pixel value
		
		P1="0.6108"
		P2="17.27"
		P3="237.3"


		VPD_FORMULA="VPD=((e(T_max)+e(T_min))/2)-(e(T_min))"
		log "${AVG_TEMP_FORMULA}\n"
    	
    	NAME_MASK_TOT="${PREF}_total_mask.tif"
		MSG="Copy filter from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_MASK_TOT} -t ${WK_13}
		check "${MSG} failed.\n"
		
		NAME_DEM_SCALED="${PREF}_dem_scaled.tif"
		MSG="Copy DEM from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_DEM_SCALED} -t ${WK_13}
		check "${MSG} failed.\n"
		
    	MASK_TOT="${WK_13}/${NAME_MASK_TOT}"
    	DEM_SCALED="${WK_13}/${NAME_DEM_SCALED}"    	
		
    	INPUT_01=$( ls ${IN_13}/*.tmp )
    	MISSING=$(  cat ${INPUT_01} | grep -w "Missing=[ \t]*[0-9]*" | awk -F"=" '{ print $5 }' | tr -d ']' ) 
    	MULTI=$(    cat ${INPUT_01} | grep -w "Multi=[ \t]*[0-9]*"   | awk -F"=" '{ print $4 }' | tr -d '] [Missing' )
    	
    	PX_COORDS=()
		for (( X=${START_X}; X<=${END_X}; X++ )) ; do
			for (( Y=${START_Y}; Y<=${END_Y}; Y++ )) ; do
				PX_COORDS+=("${X},${Y}")
			done
		done
    	
#    	for YYYY in "${YEARS_PROC[@]}" ; do
#			for MM in "${MONTHS_PROC[@]}" ; do
#			
#				MSG="Create an empty monoband image for ${YYYY}-${MM}"
#				OUTPUT_01="${WK_13}/Europe_empty_${YYYY}${MM}.tif"
#				log "${MSG} ...\n"
#				${BIN_DIR}/createImg -x ${SIZEX_EUROPE} -y ${SIZEY_EUROPE} -b 1 -t float -v 0 -c -n "${OUTPUT_01}" &>> "${LOGFILE}"
#				check "${MSG} failed.\n" 
#		
#				MSG="Conversion of Avg_Temp GeoTiff to a textual file for ${YYYY}-${MM}"
#				OUTPUT_02="${WK_13}/Avg_Temp_${YYYY}${MM}.txt"
#				log "${MSG} ...\n"
#				gdal_translate ${PAR_03} ${OUTPUT_01} ${OUTPUT_02} &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#
#				MSG="Conversion of Temp_Range GeoTiff to a textual file for ${YYYY}-${MM}"
#				OUTPUT_03="${WK_13}/Temp_Range_${YYYY}${MM}.txt"
#				log "${MSG} ...\n"
#				gdal_translate ${PAR_03} ${OUTPUT_01} ${OUTPUT_03} &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#
#				MSG="Conversion of Temp_min GeoTiff to a textual file for ${YYYY}-${MM}"
#				OUTPUT_04="${WK_13}/Temp_min_${YYYY}${MM}.txt"
#				log "${MSG} ...\n"
#				gdal_translate ${PAR_03} ${OUTPUT_01} ${OUTPUT_04} &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#				
#				MSG="Conversion of Temp_max GeoTiff to a textual file for ${YYYY}-${MM}"
#				OUTPUT_05="${WK_13}/Temp_max_${YYYY}${MM}.txt"
#				log "${MSG} ...\n"
#				gdal_translate ${PAR_03} ${OUTPUT_01} ${OUTPUT_05} &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#
#				MSG="Remove empty GeoTiff for ${YYYY}-${MM}"
#				log "${MSG} ...\n"
#				rm ${OUTPUT_01} &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#
#			done
#		done
#		
#		for P in ${PX_COORDS[@]} ; do
#			X_COORD=$( echo ${P} | cut -d ',' -f '1' )
#			Y_COORD=$( echo ${P} | cut -d ',' -f '2' )
#			LINE_NUM=$( cat ${INPUT_01} | grep -wn "[ \t]*${X_COORD},[ \t]*${Y_COORD}" | cut -d ':' -f '1' )
#			if [ "${LINE_NUM}" != "" ] ; then # If this cell exists in the dataset
#				PREC="${LINE_NUM}"
#				J="1"
#				for YYYY in "${YEARS_PROC[@]}" ; do
#					IDX=$( echo "${LINE_NUM}+${J}" | bc )
#					YEAR_DATA=( $( sed -n -e "${IDX},${IDX}p" ${INPUT_01} ) )
#					K="0"
#					for MM in "${MONTHS_PROC[@]}" ; do
#						ORIG_VAL="${YEAR_DATA[${K}]}"
#						if [ "${ORIG_VAL}" == "${MISSING}" ] ; then
#							ORIG_VAL="0"
#						fi
#						SCALED_VAL=$( echo ${ORIG_VAL}*${MULTI} | bc | sed 's/^\./0./' )
#						# Gdal indexes starts from 0, not from 1 and in textual files there is the center of the pixel (i.e.: 0,0 --> 0.5,0.5)
#						# Gdal starts from UL, not from LL as textual files
#						NEW_X_COORD=$( echo "(${X_COORD}-1)+0.5" | bc | sed 's/^\./0./' )
#						NEW_Y_COORD=$( echo "((${SIZEY_EUROPE}-1)-(${Y_COORD}-1))+0.5" | bc | sed 's/^\./0./' )
#						
#						MSG="Change textual file in cell ${X_COORD}, ${Y_COORD} (${NEW_X_COORD}, ${NEW_Y_COORD}) content for ${YYYY}-${MM}"
#						OUTPUT_03="${WK_13}/Avg_Temp_${YYYY}${MM}.txt"
#						log "${MSG} ...\n"
#						sed -i 's/\<'${NEW_X_COORD}' '${NEW_Y_COORD}' 0\>/'${NEW_X_COORD}' '${NEW_Y_COORD}' '${SCALED_VAL}'/' "${OUTPUT_03}" &>> "${LOGFILE}"
#						check "${MSG} failed.\n"
#						
#						let "K += 1"
#					done
#					let "J += 1"
#				done
#			else # If this cell is missing from the dataset take the previous values
#				J="1"
#				for YYYY in "${YEARS_PROC[@]}" ; do
#					IDX=$( echo "${PREC}+${J}" | bc )
#					YEAR_DATA=( $( sed -n -e "${IDX},${IDX}p" ${INPUT_01} ) )
#					K="0"
#					for MM in "${MONTHS_PROC[@]}" ; do
#						ORIG_VAL="${YEAR_DATA[${K}]}"
#						if [ "${ORIG_VAL}" == "${MISSING}" ] ; then
#							ORIG_VAL="0"
#						fi
#						SCALED_VAL=$( echo ${ORIG_VAL}*${MULTI} | bc | sed 's/^\./0./' )
#						# Gdal indexes starts from 0, not from 1 and in textual files there is the center of the pixel (i.e.: 0,0 --> 0.5,0.5)
#						# Gdal starts from UL, not from LL as textual files
#						NEW_X_COORD=$( echo "(${X_COORD}-1)+0.5" | bc | sed 's/^\./0./' )
#						NEW_Y_COORD=$( echo "((${SIZEY_EUROPE}-1)-(${Y_COORD}-1))+0.5" | bc | sed 's/^\./0./' )
#						
#						MSG="Change textual file in cell ${X_COORD}, ${Y_COORD} (${NEW_X_COORD}, ${NEW_Y_COORD}) content for ${YYYY}-${MM}"
#						OUTPUT_03="${WK_13}/Avg_Temp_${YYYY}${MM}.txt"
#						log "${MSG} ...\n"
#						sed -i 's/\<'${NEW_X_COORD}' '${NEW_Y_COORD}' 0\>/'${NEW_X_COORD}' '${NEW_Y_COORD}' '${SCALED_VAL}'/' "${OUTPUT_03}" &>> "${LOGFILE}"
#						check "${MSG} failed.\n"
#						
#						let "K += 1"
#					done
#					let "J += 1"
#				done
#			fi
#		done
#
#    	INPUT_01=$( ls ${IN_13}/*.dtr )
#    	MISSING=$(  cat ${INPUT_01} | grep -w "Missing=[ \t]*[0-9]*" | awk -F"=" '{ print $5 }' | tr -d ']' ) 
#    	MULTI=$(    cat ${INPUT_01} | grep -w "Multi=[ \t]*[0-9]*"   | awk -F"=" '{ print $4 }' | tr -d '] [Missing' )
#    	
#    	PX_COORDS=()
#		for (( X=${START_X}; X<=${END_X}; X++ )) ; do
#			for (( Y=${START_Y}; Y<=${END_Y}; Y++ )) ; do
#				PX_COORDS+=("${X},${Y}")
#			done
#		done
#		
#		for P in ${PX_COORDS[@]} ; do
#			X_COORD=$( echo ${P} | cut -d ',' -f '1' )
#			Y_COORD=$( echo ${P} | cut -d ',' -f '2' )
#			LINE_NUM=$( cat ${INPUT_01} | grep -wn "[ \t]*${X_COORD},[ \t]*${Y_COORD}" | cut -d ':' -f '1' )
#			if [ "${LINE_NUM}" != "" ] ; then # If this cell exists in the dataset
#				PREC="${LINE_NUM}"
#				J="1"
#				for YYYY in "${YEARS_PROC[@]}" ; do
#					IDX=$( echo "${LINE_NUM}+${J}" | bc )
#					YEAR_DATA=( $( sed -n -e "${IDX},${IDX}p" ${INPUT_01} ) )
#					K="0"
#					for MM in "${MONTHS_PROC[@]}" ; do
#						ORIG_VAL="${YEAR_DATA[${K}]}"
#						if [ "${ORIG_VAL}" == "${MISSING}" ] ; then
#							ORIG_VAL="0"
#						fi
#						SCALED_VAL=$( echo ${ORIG_VAL}*${MULTI} | bc | sed 's/^\./0./' )
#						# Gdal indexes starts from 0, not from 1 and in textual files there is the center of the pixel (i.e.: 0,0 --> 0.5,0.5)
#						# Gdal starts from UL, not from LL as textual files
#						NEW_X_COORD=$( echo "(${X_COORD}-1)+0.5" | bc | sed 's/^\./0./' )
#						NEW_Y_COORD=$( echo "((${SIZEY_EUROPE}-1)-(${Y_COORD}-1))+0.5" | bc | sed 's/^\./0./' )
#						
#						MSG="Change textual file in cell ${X_COORD}, ${Y_COORD} (${NEW_X_COORD}, ${NEW_Y_COORD}) content for ${YYYY}-${MM}"
#						OUTPUT_03="${WK_13}/Temp_Range_${YYYY}${MM}.txt"
#						log "${MSG} ...\n"
#						sed -i 's/\<'${NEW_X_COORD}' '${NEW_Y_COORD}' 0\>/'${NEW_X_COORD}' '${NEW_Y_COORD}' '${SCALED_VAL}'/' "${OUTPUT_03}" &>> "${LOGFILE}"
#						check "${MSG} failed.\n"
#						
#						let "K += 1"
#					done
#					let "J += 1"
#				done
#			else # If this cell is missing from the dataset take the previous values
#				J="1"
#				for YYYY in "${YEARS_PROC[@]}" ; do
#					IDX=$( echo "${PREC}+${J}" | bc )
#					YEAR_DATA=( $( sed -n -e "${IDX},${IDX}p" ${INPUT_01} ) )
#					K="0"
#					for MM in "${MONTHS_PROC[@]}" ; do
#						ORIG_VAL="${YEAR_DATA[${K}]}"
#						if [ "${ORIG_VAL}" == "${MISSING}" ] ; then
#							ORIG_VAL="0"
#						fi
#						SCALED_VAL=$( echo ${ORIG_VAL}*${MULTI} | bc | sed 's/^\./0./' )
#						# Gdal indexes starts from 0, not from 1 and in textual files there is the center of the pixel (i.e.: 0,0 --> 0.5,0.5)
#						# Gdal starts from UL, not from LL as textual files
#						NEW_X_COORD=$( echo "(${X_COORD}-1)+0.5" | bc | sed 's/^\./0./' )
#						NEW_Y_COORD=$( echo "((${SIZEY_EUROPE}-1)-(${Y_COORD}-1))+0.5" | bc | sed 's/^\./0./' )
#						
#						MSG="Change textual file in cell ${X_COORD}, ${Y_COORD} (${NEW_X_COORD}, ${NEW_Y_COORD}) content for ${YYYY}-${MM}"
#						OUTPUT_03="${WK_13}/Temp_Range_${YYYY}${MM}.txt"
#						log "${MSG} ...\n"
#						sed -i 's/\<'${NEW_X_COORD}' '${NEW_Y_COORD}' 0\>/'${NEW_X_COORD}' '${NEW_Y_COORD}' '${SCALED_VAL}'/' "${OUTPUT_03}" &>> "${LOGFILE}"
#						check "${MSG} failed.\n"
#						
#						let "K += 1"
#					done
#					let "J += 1"
#				done
#			fi
#		done
#
#		TEMPLATE_TXT="${WK_13}/Avg_Temp_201001.txt"
#		
#		for P in ${PX_COORDS[@]} ; do
#			X_COORD=$( echo ${P} | cut -d ',' -f '1' )
#			Y_COORD=$( echo ${P} | cut -d ',' -f '2' )
#			NEW_X_COORD=$( echo "(${X_COORD}-1)+0.5" | bc | sed 's/^\./0./' )
#			NEW_Y_COORD=$( echo "((${SIZEY_EUROPE}-1)-(${Y_COORD}-1))+0.5" | bc | sed 's/^\./0./' )
#			LINE_NUM=$( cat ${TEMPLATE_TXT} | grep -wn "[ \t]*${NEW_X_COORD}[ \t]*${NEW_Y_COORD}" | cut -d ':' -f '1' )
#
#			for YYYY in "${YEARS_PROC[@]}" ; do
#				for MM in "${MONTHS_PROC[@]}" ; do
#					IDX="${LINE_NUM}"
#					INPUT_02="${WK_13}/Avg_Temp_${YYYY}${MM}.txt"
#					INPUT_03="${WK_13}/Temp_Range_${YYYY}${MM}.txt"
#					
#					AVG_TEMP=$( sed -n -e "${LINE_NUM},${LINE_NUM}p" ${INPUT_02} | awk -F" " '{ print $3 }' )
#					RANGE=$( sed -n -e "${LINE_NUM},${LINE_NUM}p" ${INPUT_03} | awk -F" " '{ print $3 }' )
#					
#					HALF_RANGE=$( echo ${RANGE}*0.5 | bc | sed 's/^\./0./' )
#					
#					MIN_TEMP=$( echo ${AVG_TEMP}-${HALF_RANGE} | bc | sed 's/^\./0./' )
#					MAX_TEMP=$( echo ${AVG_TEMP}+${HALF_RANGE} | bc | sed 's/^\./0./' )
#					
#					MSG="Change Temp_min textual file in cell ${NEW_X_COORD}, ${NEW_Y_COORD} content for ${YYYY}-${MM}"
#					OUTPUT_03="${WK_13}/Temp_min_${YYYY}${MM}.txt"
#					log "${MSG} ...\n"
#					sed -i 's/\<'${NEW_X_COORD}' '${NEW_Y_COORD}' 0\>/'${NEW_X_COORD}' '${NEW_Y_COORD}' '${MIN_TEMP}'/' "${OUTPUT_03}" &>> "${LOGFILE}"
#					check "${MSG} failed.\n"
#				
#					MSG="Change Temp_max textual file in cell ${NEW_X_COORD}, ${NEW_Y_COORD} content for ${YYYY}-${MM}"
#					OUTPUT_04="${WK_13}/Temp_max_${YYYY}${MM}.txt"
#					log "${MSG} ...\n"
#					sed -i 's/\<'${NEW_X_COORD}' '${NEW_Y_COORD}' 0\>/'${NEW_X_COORD}' '${NEW_Y_COORD}' '${MAX_TEMP}'/' "${OUTPUT_04}" &>> "${LOGFILE}"
#					check "${MSG} failed.\n"
#				
#				done
#			done
#		done
#		

		cp ~/Desktop/${IMG}_tmp/VPD_[0-9]*.tif ${WK_13}
				
		UNITY="kPa"
		METADATA="-mo SITE=${SITE} -mo VALUES=VPD -mo UNITY_OF_MEASURE=${UNITY}"
		
		for YYYY in "${YEARS_PROC[@]}" ; do
			MONTHS=()
			for MM in "${MONTHS_PROC[@]}" ; do
				
#				VAR="Temp_min"
#				MSG="Reconvert ${VAR} textual files into GeoTiff content for ${YYYY}-${MM}"
#				INPUT_05="${WK_13}/${VAR}_${YYYY}${MM}.txt"
#				OUTPUT_05="${WK_13}/${VAR}_${YYYY}${MM}_latlon.tif"
#				log "${MSG} ...\n"
#				gdal_translate ${PAR_01} -a_srs "${PROJ_LONGLAT}" -a_ullr ${UL_EUR_LONGITUDE} ${UL_EUR_LATITUDE} ${LR_EUR_LONGITUDE} ${LR_EUR_LATITUDE} ${INPUT_05} ${OUTPUT_05} &>> "${LOGFILE}" &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#				
#				MSG="Take a sub window of GeoTiff ${VAR} for ${YYYY}-${MM}"
#				OUTPUT_06="${WK_13}/${VAR}_${YYYY}${MM}_latlon_cut.tif"
#				log "${MSG} ...\n"
#				gdal_translate ${PAR_01} -a_srs "${PROJ_LONGLAT}" -projwin ${UL_LONGITUDE} ${UL_LATITUDE} ${LR_LONGITUDE} ${LR_LATITUDE} ${OUTPUT_05} ${OUTPUT_06} &>> "${LOGFILE}" &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#				
#				MSG="Conversion of tiff projection from longlat to UTM for ${VAR} ${YYYY}-${MM}"
#				OUTPUT_07="${WK_13}/${VAR}_${YYYY}${MM}_utm.tif"
#				log "${MSG} ...\n"
#				gdalwarp ${PAR_01} -t_srs "${PROJ}" -tr ${RES} -${RES} ${OUTPUT_06} ${OUTPUT_07} &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#				
#				MSG="Remap and cut UTM geotiff ${VAR} image for ${YYYY}-${MM}"
#				OUTPUT_08="${WK_13}/${VAR}_${YYYY}${MM}_remap.tif"
#				log "${MSG} ...\n"
#				${BIN_DIR}/remap -i ${OUTPUT_07} -o ${OUTPUT_08} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#				
#				# ------------------------------------------------------------------------------------
#				
#				VAR="Temp_max"
#				MSG="Reconvert ${VAR} textual files into GeoTiff content for ${YYYY}-${MM}"
#				INPUT_05="${WK_13}/${VAR}_${YYYY}${MM}.txt"
#				OUTPUT_05="${WK_13}/${VAR}_${YYYY}${MM}_latlon.tif"
#				log "${MSG} ...\n"
#				gdal_translate ${PAR_01} -a_srs "${PROJ_LONGLAT}" -a_ullr ${UL_EUR_LONGITUDE} ${UL_EUR_LATITUDE} ${LR_EUR_LONGITUDE} ${LR_EUR_LATITUDE} ${INPUT_05} ${OUTPUT_05} &>> "${LOGFILE}" &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#				
#				MSG="Take a sub window of GeoTiff ${VAR} for ${YYYY}-${MM}"
#				OUTPUT_06="${WK_13}/${VAR}_${YYYY}${MM}_latlon_cut.tif"
#				log "${MSG} ...\n"
#				gdal_translate ${PAR_01} -a_srs "${PROJ_LONGLAT}" -projwin ${UL_LONGITUDE} ${UL_LATITUDE} ${LR_LONGITUDE} ${LR_LATITUDE} ${OUTPUT_05} ${OUTPUT_06} &>> "${LOGFILE}" &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#				
#				MSG="Conversion of tiff projection from longlat to UTM for ${VAR} ${YYYY}-${MM}"
#				OUTPUT_07="${WK_13}/${VAR}_${YYYY}${MM}_utm.tif"
#				log "${MSG} ...\n"
#				gdalwarp ${PAR_01} -t_srs "${PROJ}" -tr ${RES} -${RES} ${OUTPUT_06} ${OUTPUT_07} &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#				
#				MSG="Remap and cut UTM geotiff ${VAR} image for ${YYYY}-${MM}"
#				OUTPUT_09="${WK_13}/${VAR}_${YYYY}${MM}_remap.tif"
#				log "${MSG} ...\n"
#				${BIN_DIR}/remap -i ${OUTPUT_07} -o ${OUTPUT_09} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#
#				MSG="Getting T_min for ${YYYY}${MM}"
#				T_MIN="${WK_13}/${IMG}_T_min_${YYYY}${MM}.tif"
#				log "${MSG} ...\n"
#				gdal_calc.py -A ${DEM_SCALED} -B ${OUTPUT_08} --outfile=${T_MIN} --calc="(A+B)" &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#				
#				MSG="Getting T_max for ${YYYY}${MM}"
#				T_MAX="${WK_13}/${IMG}_T_max_${YYYY}${MM}.tif"
#				log "${MSG} ...\n"
#				gdal_calc.py -A ${DEM_SCALED} -B ${OUTPUT_09} --outfile=${T_MAX} --calc="(A+B)" &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#
#				MSG="Getting ${IMG} for ${YYYY}${MM}"
				OUTPUT_11="${WK_13}/${IMG}_${YYYY}${MM}.tif"
#				log "${MSG} ...\n"
#				${BIN_DIR}/getVPD -min ${T_MIN} -max ${T_MAX} -o ${OUTPUT_11} &>> "${LOGFILE}"
#				check "${MSG} failed.\n"

				MSG="Apply mask to ${OUTPUT_11}"
				OUTPUT_12="${WK_13}/${IMG}_${YYYY}${MM}_mask.tif"
				log "${MSG} ...\n"
				gdal_calc.py -A ${OUTPUT_11} -B ${MASK_TOT} --outfile=${OUTPUT_12} --calc="(A*B)" &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MONTHS+=("${OUTPUT_12}")
			done
			
			MSG="Create merged ${IMG} image for ${YYYY}"
			OUTPUT_13="${WK_13}/${IMG}_${YYYY}_merged.tif"
			log "${MSG} ...\n"
			gdal_merge.py ${PAR_05} ${MONTHS[@]} -o ${OUTPUT_13}  &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			MSG="Add metadata to ${IMG} image and compress it for ${YYYY}"
			OUTPUT_14="${WK_13}/${IMG}_${YYYY}.tif"
			log "${MSG} ...\n"
			gdal_translate ${PAR_01} ${METADATA} ${OUTPUT_13} ${OUTPUT_14} &>> "${LOGFILE}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			MSG="Copy ${IMG} into ${OUT_13}"
			log "${MSG} ...\n"
			cp ${OUTPUT_14} -t ${OUT_13}
			check "${MSG} failed.\n"
		done

		clean "${WK_13}"
    	
    	log "### ...........stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - VPD execution }

### Precip execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Precip" ] ; then
    	log "### { Start creating ${IMG} images....... ###\n"
    	
    	NAME_MASK_TOT="${PREF}_total_mask.tif"
		MSG="Copy filter from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_MASK_TOT} -t ${WK_14}
		check "${MSG} failed.\n"
		
		MASK_TOT="${WK_14}/${NAME_MASK_TOT}"
    	
#    	INPUT_01=$( ls ${IN_14}/*.pre )
#    	MISSING=$(  cat ${INPUT_01} | grep -w "Missing=[ \t]*[0-9]*" | awk -F"=" '{ print $5 }' | tr -d ']' ) 
#    	MULTI=$(    cat ${INPUT_01} | grep -w "Multi=[ \t]*[0-9]*"   | awk -F"=" '{ print $4 }' | tr -d '] [Missing' )
#    	
#    	PX_COORDS=()
#		for (( X=${START_X}; X<=${END_X}; X++ )) ; do
#			for (( Y=${START_Y}; Y<=${END_Y}; Y++ )) ; do
#				PX_COORDS+=("${X},${Y}")
#			done
#		done
#    	
#    	for YYYY in "${YEARS_PROC[@]}" ; do
#			for MM in "${MONTHS_PROC[@]}" ; do
#			
#				MSG="Create an empty monoband image for ${YYYY}-${MM}"
#				OUTPUT_01="${WK_14}/Europe_empty_${YYYY}${MM}.tif"
#				log "${MSG} ...\n"
#				${BIN_DIR}/createImg -x ${SIZEX_EUROPE} -y ${SIZEY_EUROPE} -b 1 -t float -v 0 -c -n "${OUTPUT_01}" &>> "${LOGFILE}"
#				check "${MSG} failed.\n" 
#		
#				MSG="Conversion of GeoTiff to a textual file for ${YYYY}-${MM}"
#				OUTPUT_02="${WK_14}/${IMG}_${YYYY}${MM}.txt"
#				log "${MSG} ...\n"
#				gdal_translate ${PAR_03} ${OUTPUT_01} ${OUTPUT_02} &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#				
#				MSG="Remove empty GeoTiff for ${YYYY}-${MM}"
#				log "${MSG} ...\n"
#				rm ${OUTPUT_01} &>> "${LOGFILE}"
#				check "${MSG} failed.\n"
#
#			done
#		done
#		
#		for P in ${PX_COORDS[@]} ; do
#			X_COORD=$( echo ${P} | cut -d ',' -f '1' )
#			Y_COORD=$( echo ${P} | cut -d ',' -f '2' )
#			LINE_NUM=$( cat ${INPUT_01} | grep -wn "[ \t]*${X_COORD},[ \t]*${Y_COORD}" | cut -d ':' -f '1' )
#			if [ "${LINE_NUM}" != "" ] ; then # If this cell exists in the dataset
#				PREC="${LINE_NUM}"
#				J="1"
#				for YYYY in "${YEARS_PROC[@]}" ; do
#					IDX=$( echo "${LINE_NUM}+${J}" | bc )
#					YEAR_DATA=( $( sed -n -e "${IDX},${IDX}p" ${INPUT_01} ) )
#					K="0"
#					for MM in "${MONTHS_PROC[@]}" ; do
#						ORIG_VAL="${YEAR_DATA[${K}]}"
#						if [ "${ORIG_VAL}" == "${MISSING}" ] ; then
#							ORIG_VAL="0"
#						fi
#						SCALED_VAL=$( echo ${ORIG_VAL}*${MULTI} | bc | sed 's/^\./0./' )
#						# Gdal indexes starts from 0, not from 1 and in textual files there is the center of the pixel (i.e.: 0,0 --> 0.5,0.5)
#						# Gdal starts from UL, not from LL as textual files
#						NEW_X_COORD=$( echo "(${X_COORD}-1)+0.5" | bc | sed 's/^\./0./' )
#						NEW_Y_COORD=$( echo "((${SIZEY_EUROPE}-1)-(${Y_COORD}-1))+0.5" | bc | sed 's/^\./0./' )
#						
#						MSG="Change textual file in cell ${X_COORD}, ${Y_COORD} (${NEW_X_COORD}, ${NEW_Y_COORD}) content for ${YYYY}-${MM}"
#						OUTPUT_03="${WK_14}/${IMG}_${YYYY}${MM}.txt"
#						log "${MSG} ...\n"
#						sed -i 's/\<'${NEW_X_COORD}' '${NEW_Y_COORD}' 0\>/'${NEW_X_COORD}' '${NEW_Y_COORD}' '${SCALED_VAL}'/' "${OUTPUT_03}" &>> "${LOGFILE}"
#						check "${MSG} failed.\n"
#						
#						let "K += 1"
#					done
#					let "J += 1"
#				done
#			else # If this cell is missing from the dataset take the previous values
#				J="1"
#				for YYYY in "${YEARS_PROC[@]}" ; do
#					IDX=$( echo "${PREC}+${J}" | bc )
#					YEAR_DATA=( $( sed -n -e "${IDX},${IDX}p" ${INPUT_01} ) )
#					K="0"
#					for MM in "${MONTHS_PROC[@]}" ; do
#						ORIG_VAL="${YEAR_DATA[${K}]}"
#						if [ "${ORIG_VAL}" == "${MISSING}" ] ; then
#							ORIG_VAL="0"
#						fi
#						SCALED_VAL=$( echo ${ORIG_VAL}*${MULTI} | bc | sed 's/^\./0./' )
#						# Gdal indexes starts from 0, not from 1 and in textual files there is the center of the pixel (i.e.: 0,0 --> 0.5,0.5)
#						# Gdal starts from UL, not from LL as textual files
#						NEW_X_COORD=$( echo "(${X_COORD}-1)+0.5" | bc | sed 's/^\./0./' )
#						NEW_Y_COORD=$( echo "((${SIZEY_EUROPE}-1)-(${Y_COORD}-1))+0.5" | bc | sed 's/^\./0./' )
#						
#						MSG="Change textual file in cell ${X_COORD}, ${Y_COORD} (${NEW_X_COORD}, ${NEW_Y_COORD}) content for ${YYYY}-${MM}"
#						OUTPUT_03="${WK_14}/${IMG}_${YYYY}${MM}.txt"
#						log "${MSG} ...\n"
#						sed -i 's/\<'${NEW_X_COORD}' '${NEW_Y_COORD}' 0\>/'${NEW_X_COORD}' '${NEW_Y_COORD}' '${SCALED_VAL}'/' "${OUTPUT_03}" &>> "${LOGFILE}"
#						check "${MSG} failed.\n"
#						
#						let "K += 1"
#					done
#					let "J += 1"
#				done
#			fi
#		done

		cp ~/Desktop/${IMG}_tmp/* ${WK_14}
		
		UNITY="mm/month"
		METADATA="-mo SITE=${SITE} -mo VALUES=PRECIPITATIONS -mo UNITY_OF_MEASURE=${UNITY}"
		for YYYY in "${YEARS_PROC[@]}" ; do
			MONTHS=()
			for MM in "${MONTHS_PROC[@]}" ; do
				MSG="Reconvert textual files into GeoTiff content for ${YYYY}-${MM}"
				INPUT_02="${WK_14}/${IMG}_${YYYY}${MM}.txt"
				OUTPUT_04="${WK_14}/${IMG}_${YYYY}${MM}_latlon.tif"
				log "${MSG} ...\n"
				gdal_translate ${PAR_01} -a_srs "${PROJ_LONGLAT}" -a_ullr ${UL_EUR_LONGITUDE} ${UL_EUR_LATITUDE} ${LR_EUR_LONGITUDE} ${LR_EUR_LATITUDE} ${INPUT_02} ${OUTPUT_04} &>> "${LOGFILE}" &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Take a sub window of GeoTiff ${YYYY}-${MM}"
				OUTPUT_05="${WK_14}/${IMG}_${YYYY}${MM}_latlon_cut.tif"
				log "${MSG} ...\n"
				gdal_translate ${PAR_01} -a_srs "${PROJ_LONGLAT}" -projwin ${UL_LONGITUDE} ${UL_LATITUDE} ${LR_LONGITUDE} ${LR_LATITUDE} ${OUTPUT_04} ${OUTPUT_05} &>> "${LOGFILE}" &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Conversion of tiff projection from longlat to UTM for ${YYYY}-${MM}"
				OUTPUT_06="${WK_14}/${IMG}_${YYYY}${MM}_utm.tif"
				log "${MSG} ...\n"
				gdalwarp ${PAR_01} -t_srs "${PROJ}" -tr ${RES} -${RES} ${OUTPUT_05} ${OUTPUT_06} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Remap and cut UTM geotiff image for ${YYYY}-${MM}"
				OUTPUT_07="${WK_14}/${IMG}_${YYYY}${MM}_remap.tif"
				log "${MSG} ...\n"
				${BIN_DIR}/remap -i ${OUTPUT_06} -o ${OUTPUT_07} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Apply mask to ${OUTPUT_07}"
				OUTPUT_08="${WK_14}/${IMG}_${YYYY}${MM}_mask.tif"
				log "${MSG} ...\n"
				gdal_calc.py -A ${OUTPUT_07} -B ${MASK_TOT} --outfile=${OUTPUT_08} --calc="(A*B)" &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MONTHS+=("${OUTPUT_08}")
			done
			
			MSG="Create merged ${IMG} image for ${YYYY}"
			OUTPUT_09="${WK_14}/${IMG}_${YYYY}_merged.tif"
			log "${MSG} ...\n"
			gdal_merge.py ${PAR_05} ${MONTHS[@]} -o ${OUTPUT_09}  &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			MSG="Add metadata to ${IMG} image and compress it for ${YYYY}"
			OUTPUT_10="${WK_14}/${IMG}_${YYYY}.tif"
			log "${MSG} ...\n"
			gdal_translate ${PAR_01} ${METADATA} ${OUTPUT_09} ${OUTPUT_10} &>> "${LOGFILE}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			MSG="Copy ${IMG} into ${OUT_14}"
			log "${MSG} ...\n"
			cp ${OUTPUT_10} -t ${OUT_14}
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
    	
    	NAME_MASK_TOT="${PREF}_total_mask.tif"
		MSG="Copy filter from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_MASK_TOT} -t ${WK_15}
		check "${MSG} failed.\n"
		
    	MASK_TOT="${WK_15}/${NAME_MASK_TOT}"
    	
    	UNITY="m^2/m^2 (foliage_area/soil_area)"
    	METADATA="SITE=${SITE},VALUES=LAI,UNITY_OF_MEASURE=${UNITY}"
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
					
					MSG="Mask ${IMG}"
					OUTPUT_05="${WK_15}/${IMG}_${YEAR}${MONTH}_masked.tif"
					log "${MSG} ...\n"
					${BIN_DIR}/applyMask -i ${OUTPUT_04} -m ${MASK_TOT} -o ${OUTPUT_05} &>> "${LOGFILE}"
					check "${MSG} failed.\n"
					
					LAI_MONTHS+=("${OUTPUT_05}")
			
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
			
			MSG="Mask ${IMG}"
			OUTPUT_05="${WK_15}/${IMG}_${YEAR}${MONTH}_masked.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/applyMask -i ${OUTPUT_04} -m ${MASK_TOT} -o ${OUTPUT_05} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			LAI_MONTHS+=("${OUTPUT_05}")
			
			OUTPUT_06="${WK_15}/LAI_${YEAR}.tif"
			MSG="Get multiband LAI image"
			log "${MSG} ...\n"
			${BIN_DIR}/mergeImg -b ${#LAI_MONTHS[@]} -i "${LAI_MONTHS[@]}" -o ${OUTPUT_06} -m "${METADATA}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			MSG="Copy LAI multiband image into output dir"
			log "${MSG} ...\n"
			cp ${OUTPUT_06} ${OUT_15}
			check "${MSG} failed.\n"
		done
		
    	clean "${WK_15}"

    	log "### ...........stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - LAI execution }

### Packet execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Packet" ] ; then
    	log "### { Start creating compressed package .......... ###\n"
    		
    	OUT_DIRS=(${OUT_01} ${OUT_02} ${OUT_03} ${OUT_04} ${OUT_05} ${OUT_06} ${OUT_07} ${OUT_08} ${OUT_09} ${OUT_10} ${OUT_11} ${OUT_12} ${OUT_13} ${OUT_14} ${OUT_15} ${OUT_16})
    	
    	FIRST_YEAR="${YEARS_PROC[0]}"
    	LAST_YEAR="${YEARS_PROC[$((${#YEARS_PROC[@]}-1))]}"
    	
    	NOW=$( date +"%Y%m%d%H%M%S" )
    	
    	PKG_DIR_NAME="${SITE}_${RES}m_${FIRST_YEAR}-${LAST_YEAR}_${NOW}_input"
    	PKG_DIR="${WK_17}/${PKG_DIR_NAME}"
    	PKG_IMG_DIR="${PKG_DIR}/${SITE}/images"
    	PKG_TXT_DIR="${PKG_DIR}/${SITE}/txt"
    	
    	MSG="Creating package directories"
		log "${MSG} ...\n"
    	mkdir -p ${PKG_IMG_DIR} ${PKG_TXT_DIR}
    	check "${MSG} failed.\n"
    	
    	for DIR in "${OUT_DIRS[@]}" ; do
    		MSG="Copy ${DIR} files into ${PKG_IMG_DIR}"
			log "${MSG} ...\n"
			cp ${DIR}/* -t ${PKG_IMG_DIR} 2>/dev/null || :
			check "${MSG} failed.\n"
		done
		
		CMCC_PROJECT="$( dirname ${0} )/../../3D-CMCC-Forest-Model"
		SETTINGS="${CMCC_PROJECT}/input/settings.txt"
		
		MSG="Copy ${DIR} files into ${PKG_IMG_DIR}"
		log "${MSG} ...\n"
		cp ${SETTINGS} -t ${PKG_TXT_DIR} 2>/dev/null || :
		check "${MSG} failed.\n"
		
		for IDX in "${SPECIES_ID_PRESENT[@]}" ; do
			MSG="Copy specie ${IDX} file into ${PKG_TXT_DIR}"
			SPECIE_FILE_NAME="${CMCC_PROJECT}/input/${SPECIES_ID[${IDX}]}.txt"
			log "${MSG} ...\n"
			cp ${SPECIE_FILE_NAME} -t ${PKG_TXT_DIR} 2>/dev/null || :
			check "${MSG} failed.\n"
    	done
		
		MSG="Creating ${PKG_DIR_NAME}.zip"
		log "${MSG} ...\n"
		cd ${WK_17}
		zip -r ${PKG_DIR_NAME} ${PKG_DIR_NAME} &>/dev/null
		cd - &>/dev/null
		check "${MSG} failed.\n"
		
		MSG="Copy ${PKG_DIR_NAME}.zip into ${OUT_17}"
		log "${MSG} ...\n"
		cp ${PKG_DIR}.zip -t ${OUT_17}
		check "${MSG} failed.\n"
		
		SPATIAL_PROJECT="$( dirname ${0} )/../../3D-CMCC-Spatial"
		MSG="Copy ${PKG_DIR_NAME}.zip into spatial input directory"
		log "${MSG} ...\n"
		cp ${PKG_DIR}.zip -t ${SPATIAL_PROJECT}/input/
		check "${MSG} failed.\n"

		clean "${WK_17}"
		
    	log "### ............stop creating compressed package } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Packet execution }

exit 0