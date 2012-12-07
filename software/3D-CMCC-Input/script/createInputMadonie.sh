#!/bin/bash
### File information summary  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
# Description:    createInputMadonie.sh bash shell script
#                 Processing chain to create input images for Forest Scenarios Evolution project (ForSE).
# Author:         Alessandro Candini - candini@meeo.it
# Version:        0.1
# Copyright:      MEEO S.R.L. - www.meeo.it 
# How it works:   It takes...
# Changelog:      2012-12-05 - version 0.1
#                     - First Release
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  File information summary }

### Global variables definitions  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
VERSION="0.1"
SCRIPT_NAME="${0:2:-3}"
AOI="Parco delle Madonie (Sicily)"
SITE="MADONIE"
MODULES=(remap applyMask calcAverage multiplyImgPx getLAI getVPD createImg mergeImg specFromMaxPerc copyGeoref reduceToBinaryMask)
IMG_ALL=(Filters Y_planted Species Phenology Management N_cell AvDBH Height Wf Wrc Ws SolarRad Avg_Temp VPD Precip LAI Soil Packet)
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
IN_01="$( dirname ${0} )/../input/01_Y_planted"
IN_02="$( dirname ${0} )/../input/02_Species"
IN_03="$( dirname ${0} )/../input/03_Phenology"
IN_04="$( dirname ${0} )/../input/04_Management"
IN_05="$( dirname ${0} )/../input/05_N_cell"
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
IN_17="$( dirname ${0} )/../input/17_Packet"
# Output directories
OUT_00="$( dirname ${0} )/../output/00_Filters"
OUT_01="$( dirname ${0} )/../output/01_Y_planted"
OUT_02="$( dirname ${0} )/../output/02_Species"
OUT_03="$( dirname ${0} )/../output/03_Phenology"
OUT_04="$( dirname ${0} )/../output/04_Management"
OUT_05="$( dirname ${0} )/../output/05_N_cell"
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
OUT_17="$( dirname ${0} )/../output/17_Packet"
# Working directories
WK_00="$( dirname ${0} )/../working/00_Filters"
WK_01="$( dirname ${0} )/../working/01_Y_planted"
WK_02="$( dirname ${0} )/../working/02_Species"
WK_03="$( dirname ${0} )/../working/03_Phenology"
WK_04="$( dirname ${0} )/../working/04_Management"
WK_06="$( dirname ${0} )/../working/05_N_cell"
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
WK_17="$( dirname ${0} )/../working/17_Packet"

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
# Lat-lon extents
WORLD="-180 90 180 -90"
SICILY="13 39 15 37"

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
		OUTPUT_08="${WK_00}/Madonie_${SPECIES_ID[${IDX}]}_${IDX}.tif"
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
		
		IDX="1"
		MSG="Get a binary mask of ${FILTER_01}"
		MASK_1="${WK_00}/${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/reduceToBinaryMask -i ${FILTER_01} -o ${MASK_1} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="2"
		MSG="Get a binary mask of ${FILTER_02}"
		MASK_2="${WK_00}/${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/reduceToBinaryMask -i ${FILTER_02} -o ${MASK_2} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="6"
		MSG="Get a binary mask of ${FILTER_06}"
		MASK_6="${WK_00}/${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/reduceToBinaryMask -i ${FILTER_06} -o ${MASK_6} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		IDX="7"
		MSG="Get a binary mask of ${FILTER_07}"
		MASK_7="${WK_00}/${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
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
		OUTPUT_03="${WK_00}/Madonie_dem.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/remap -i ${OUTPUT_02} -o ${OUTPUT_03} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		M="-0.0064"
		MSG="Performing DEM rescaling"
		DEM_SCALED="${WK_00}/Madonie_dem_scaled.tif"
		log "${MSG} ...\n"
		gdal_calc.py -A ${OUTPUT_03} --outfile=${DEM_SCALED} --calc="(${M})*(A+2)" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		log "End creating dem file .....\n"
		
		MSG="Copy masks, empty band and scaled DEM into output dir"
		log "${MSG} ...\n"
		cp ${MASK_D} ${MASK_E} ${MASK_TOT} ${OUTPUT_11} ${DEM_SCALED} ${MASK_1} ${MASK_2} ${MASK_6} ${MASK_7} -t ${OUT_00}
		check "${MSG} failed.\n"

		clean "${WK_00}"

		log "### .......stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Filters execution }

### Y_planted execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Y_planted" ] ; then
    	log "### { Start creating ${IMG} images.... ###\n"
    	
    	SPECIES_MASK_NAMES=(Undefined null null null null null null null)
    	SPECIES_MASK=(Undefined null null null null null null null)
    	for IDX in "${SPECIES_ID_PRESENT[@]}" ; do
    		SPECIES_MASK_NAMES[${IDX}]="${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
    		SPECIES_MASK[${IDX}]="${WK_01}/${SPECIES_MASK_NAMES[${IDX}]}"
    		
    		MSG="Copy ${SPECIES_ID[${IDX}]}_${IDX}_mask.tif from ${OUT_00}"
			log "${MSG} ...\n"
			cp ${OUT_00}/${SPECIES_MASK_NAMES[${IDX}]} -t ${WK_01}
			check "${MSG} failed.\n"    	
    	done
    	
    	NAME_EMPTY_BAND="empty_geo.tif"
    	EMPTY_BAND="${WK_01}/${NAME_EMPTY_BAND}"
    	
		MSG="Copy empty band from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_EMPTY_BAND} -t ${WK_01}
		check "${MSG} failed.\n"
		
		PROCESSED_SPECIES=()
		for IDX in "${SPECIES_ID_PRESENT[@]}" ; do
			MSG="Create a monoband image with every pixel set to ${Y_PLANTED_ID[${IDX}]}"
			OUTPUT_01="${WK_01}/${IMG}_every_px_${IDX}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/createImg -x ${SIZEX} -y ${SIZEY} -b 1 -t float -v ${Y_PLANTED_ID[${IDX}]} -c -n "${OUTPUT_01}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
		
			MSG="Set georeference to ${OUTPUT_01}"
			OUTPUT_02="${WK_01}/${IMG}_every_px_geo_${IDX}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/copyGeoref -i ${EMPTY_BAND} ${OUTPUT_01} -o ${OUTPUT_02} &>> "${LOGFILE}"
			check "${MSG} failed.\n"

			MSG="Apply mask to ${OUTPUT_02}"
			OUTPUT_03="${WK_01}/${IMG}_masked_${IDX}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/applyMask -i ${OUTPUT_02} -m ${SPECIES_MASK[${IDX}]} -o ${OUTPUT_03} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			PROCESSED_SPECIES+=("${OUTPUT_03}")
		done
		
		MSG="Merge of different species ${IMG}"
		OUTPUT_04="${WK_01}/${IMG}_b1.tif"
		log "${MSG} ...\n"
		gdal_merge.py ${PAR_01} -n 0 ${PROCESSED_SPECIES[@]} -o ${OUTPUT_04} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		UNITY="Planting_year"
		METADATA="SITE=${SITE},UNITY=${UNITY},ID=Y_PLANTED,${SPECIES_ID[1]}=${Y_PLANTED_ID[1]},${SPECIES_ID[2]}=${Y_PLANTED_ID[2]},${SPECIES_ID[6]}=${Y_PLANTED_ID[6]},${SPECIES_ID[7]}=${Y_PLANTED_ID[7]}"
		MSG="Create multiband ${IMG} image"
		INPUT_01=(${OUTPUT_04} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND})
		OUTPUT_05="${WK_01}/${IMG}.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/mergeImg -b ${#INPUT_01[@]} -i ${INPUT_01[@]} -o ${OUTPUT_05} -m "${METADATA}" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Copy ${IMG} into ${OUT_01}"
		log "${MSG} ...\n"
		cp ${OUTPUT_05} -t ${OUT_01}
		check "${MSG} failed.\n"
		
		clean "${WK_01}"
    
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
		
		METADATA="SITE=${SITE},ID=SPECIE,-9999=${SPECIES_ID[0]},1=${SPECIES_ID[1]},2=${SPECIES_ID[2]},3=${SPECIES_ID[3]},4=${SPECIES_ID[4]},5=${SPECIES_ID[5]},6=${SPECIES_ID[6]},7=${SPECIES_ID[7]}"
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

### N_cell execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "N_cell" ] ; then
    	log "### { Start creating ${IMG} images........ ###\n"
    	
    	SPECIES_MASK_NAMES=(Undefined null null null null null null null)
    	SPECIES_MASK=(Undefined null null null null null null null)
    	for IDX in "${SPECIES_ID_PRESENT[@]}" ; do
    		SPECIES_MASK_NAMES[${IDX}]="${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
    		SPECIES_MASK[${IDX}]="${WK_06}/${SPECIES_MASK_NAMES[${IDX}]}"
    		
    		MSG="Copy ${SPECIES_ID[${IDX}]}_${IDX}_mask.tif from ${OUT_00}"
			log "${MSG} ...\n"
			cp ${OUT_00}/${SPECIES_MASK_NAMES[${IDX}]} -t ${WK_06}
			check "${MSG} failed.\n"    	
    	done
    	
    	NAME_EMPTY_BAND="empty_geo.tif"
    	EMPTY_BAND="${WK_06}/${NAME_EMPTY_BAND}"
    	
		MSG="Copy empty band from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_EMPTY_BAND} -t ${WK_06}
		check "${MSG} failed.\n"
		
		PROCESSED_SPECIES=()
		for IDX in "${SPECIES_ID_PRESENT[@]}" ; do
			MSG="Create a monoband image with every pixel set to ${N_CELL_ID[${IDX}]}"
			OUTPUT_01="${WK_06}/${IMG}_every_px_${IDX}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/createImg -x ${SIZEX} -y ${SIZEY} -b 1 -t float -v ${N_CELL_ID[${IDX}]} -c -n "${OUTPUT_01}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
		
			MSG="Set georeference to ${OUTPUT_01}"
			OUTPUT_02="${WK_06}/${IMG}_every_px_geo_${IDX}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/copyGeoref -i ${EMPTY_BAND} ${OUTPUT_01} -o ${OUTPUT_02} &>> "${LOGFILE}"
			check "${MSG} failed.\n"

			MSG="Apply mask to ${OUTPUT_02}"
			OUTPUT_03="${WK_06}/${IMG}_masked_${IDX}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/applyMask -i ${OUTPUT_02} -m ${SPECIES_MASK[${IDX}]} -o ${OUTPUT_03} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			PROCESSED_SPECIES+=("${OUTPUT_03}")
		done
		
		MSG="Merge of different species ${IMG}"
		OUTPUT_04="${WK_06}/${IMG}_b1.tif"
		log "${MSG} ...\n"
		gdal_merge.py ${PAR_01} -n 0 ${PROCESSED_SPECIES[@]} -o ${OUTPUT_04} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		UNITY="Number_of_trees_per_cell"
		METADATA="SITE=${SITE},UNITY=${UNITY},ID=N_CELL,${SPECIES_ID[1]}=${N_CELL_ID[1]},${SPECIES_ID[2]}=${N_CELL_ID[2]},${SPECIES_ID[6]}=${N_CELL_ID[6]},${SPECIES_ID[7]}=${N_CELL_ID[7]}"
		MSG="Create multiband ${IMG} image"
		INPUT_01=(${OUTPUT_04} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND})
		OUTPUT_05="${WK_06}/${IMG}.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/mergeImg -b ${#INPUT_01[@]} -i ${INPUT_01[@]} -o ${OUTPUT_05} -m "${METADATA}" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Copy ${IMG} into ${OUT_05}"
		log "${MSG} ...\n"
		cp ${OUTPUT_05} -t ${OUT_05}
		check "${MSG} failed.\n"
		
		clean "${WK_06}"
		
    	log "### .........stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - N_cell execution }

### AvDBH execution - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "AvDBH" ] ; then
    	log "### { Start creating ${IMG} images........ ###\n"
    	
    	SPECIES_MASK_NAMES=(Undefined null null null null null null null)
    	SPECIES_MASK=(Undefined null null null null null null null)
    	for IDX in "${SPECIES_ID_PRESENT[@]}" ; do
    		SPECIES_MASK_NAMES[${IDX}]="${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
    		SPECIES_MASK[${IDX}]="${WK_06}/${SPECIES_MASK_NAMES[${IDX}]}"
    		
    		MSG="Copy ${SPECIES_ID[${IDX}]}_${IDX}_mask.tif from ${OUT_00}"
			log "${MSG} ...\n"
			cp ${OUT_00}/${SPECIES_MASK_NAMES[${IDX}]} -t ${WK_06}
			check "${MSG} failed.\n"    	
    	done
    	
    	NAME_EMPTY_BAND="empty_geo.tif"
    	EMPTY_BAND="${WK_06}/${NAME_EMPTY_BAND}"
    	
		MSG="Copy empty band from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_EMPTY_BAND} -t ${WK_06}
		check "${MSG} failed.\n"
		
		PROCESSED_SPECIES=()
		for IDX in "${SPECIES_ID_PRESENT[@]}" ; do
			MSG="Create a monoband image with every pixel set to ${AVDBH_ID[${IDX}]}"
			OUTPUT_01="${WK_06}/${IMG}_every_px_${IDX}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/createImg -x ${SIZEX} -y ${SIZEY} -b 1 -t float -v ${AVDBH_ID[${IDX}]} -c -n "${OUTPUT_01}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
		
			MSG="Set georeference to ${OUTPUT_01}"
			OUTPUT_02="${WK_06}/${IMG}_every_px_geo_${IDX}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/copyGeoref -i ${EMPTY_BAND} ${OUTPUT_01} -o ${OUTPUT_02} &>> "${LOGFILE}"
			check "${MSG} failed.\n"

			MSG="Apply mask to ${OUTPUT_02}"
			OUTPUT_03="${WK_06}/${IMG}_masked_${IDX}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/applyMask -i ${OUTPUT_02} -m ${SPECIES_MASK[${IDX}]} -o ${OUTPUT_03} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			PROCESSED_SPECIES+=("${OUTPUT_03}")
		done
		
		MSG="Merge of different species ${IMG}"
		OUTPUT_04="${WK_06}/${IMG}_b1.tif"
		log "${MSG} ...\n"
		gdal_merge.py ${PAR_01} -n 0 ${PROCESSED_SPECIES[@]} -o ${OUTPUT_04} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		UNITY="cm"
		METADATA="SITE=${SITE},UNITY=${UNITY},ID=AVDBH,${SPECIES_ID[1]}=${AVDBH_ID[1]},${SPECIES_ID[2]}=${AVDBH_ID[2]},${SPECIES_ID[6]}=${AVDBH_ID[6]},${SPECIES_ID[7]}=${AVDBH_ID[7]}"
		MSG="Create multiband ${IMG} image"
		INPUT_01=(${OUTPUT_04} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND})
		OUTPUT_05="${WK_06}/${IMG}.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/mergeImg -b ${#INPUT_01[@]} -i ${INPUT_01[@]} -o ${OUTPUT_05} -m "${METADATA}" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Copy ${IMG} into ${OUT_06}"
		log "${MSG} ...\n"
		cp ${OUTPUT_05} -t ${OUT_06}
		check "${MSG} failed.\n"
		
		clean "${WK_06}"
		
    	log "### .........stop creating ${IMG} images } ###\n"
    fi
done
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - AvDBH execution }

### Height execution  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
for IMG in "${IMG_SELECTED[@]}" ; do
	if [ "${IMG}" == "Height" ] ; then
    	log "### { Start creating ${IMG} images....... ###\n"
    	
    	SPECIES_MASK_NAMES=(Undefined null null null null null null null)
    	SPECIES_MASK=(Undefined null null null null null null null)
    	for IDX in "${SPECIES_ID_PRESENT[@]}" ; do
    		SPECIES_MASK_NAMES[${IDX}]="${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
    		SPECIES_MASK[${IDX}]="${WK_07}/${SPECIES_MASK_NAMES[${IDX}]}"
    		
    		MSG="Copy ${SPECIES_ID[${IDX}]}_${IDX}_mask.tif from ${OUT_00}"
			log "${MSG} ...\n"
			cp ${OUT_00}/${SPECIES_MASK_NAMES[${IDX}]} -t ${WK_07}
			check "${MSG} failed.\n"    	
    	done
    	
    	NAME_EMPTY_BAND="empty_geo.tif"
    	EMPTY_BAND="${WK_07}/${NAME_EMPTY_BAND}"
    	
		MSG="Copy empty band from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_EMPTY_BAND} -t ${WK_07}
		check "${MSG} failed.\n"
		
		PROCESSED_SPECIES=()
		for IDX in "${SPECIES_ID_PRESENT[@]}" ; do
			MSG="Create a monoband image with every pixel set to ${HEIGHT_ID[${IDX}]}"
			OUTPUT_01="${WK_07}/${IMG}_every_px_${IDX}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/createImg -x ${SIZEX} -y ${SIZEY} -b 1 -t float -v ${HEIGHT_ID[${IDX}]} -c -n "${OUTPUT_01}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
		
			MSG="Set georeference to ${OUTPUT_01}"
			OUTPUT_02="${WK_07}/${IMG}_every_px_geo_${IDX}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/copyGeoref -i ${EMPTY_BAND} ${OUTPUT_01} -o ${OUTPUT_02} &>> "${LOGFILE}"
			check "${MSG} failed.\n"

			MSG="Apply mask to ${OUTPUT_02}"
			OUTPUT_03="${WK_07}/${IMG}_masked_${IDX}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/applyMask -i ${OUTPUT_02} -m ${SPECIES_MASK[${IDX}]} -o ${OUTPUT_03} &>> "${LOGFILE}"
			check "${MSG} failed.\n"
			
			PROCESSED_SPECIES+=("${OUTPUT_03}")
		done
		
		MSG="Merge of different species ${IMG}"
		OUTPUT_04="${WK_07}/${IMG}_b1.tif"
		log "${MSG} ...\n"
		gdal_merge.py ${PAR_01} -n 0 ${PROCESSED_SPECIES[@]} -o ${OUTPUT_04} &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		UNITY="m"
		METADATA="SITE=${SITE},UNITY=${UNITY},ID=HEIGHT,${SPECIES_ID[1]}=${HEIGHT_ID[1]},${SPECIES_ID[2]}=${HEIGHT_ID[2]},${SPECIES_ID[6]}=${HEIGHT_ID[6]},${SPECIES_ID[7]}=${HEIGHT_ID[7]}"
		MSG="Create multiband ${IMG} image"
		INPUT_01=(${OUTPUT_04} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND} ${EMPTY_BAND})
		OUTPUT_05="${WK_07}/${IMG}.tif"
		log "${MSG} ...\n"
		${BIN_DIR}/mergeImg -b ${#INPUT_01[@]} -i ${INPUT_01[@]} -o ${OUTPUT_05} -m "${METADATA}" &>> "${LOGFILE}"
		check "${MSG} failed.\n"
		
		MSG="Copy ${IMG} into ${OUT_07}"
		log "${MSG} ...\n"
		cp ${OUTPUT_05} -t ${OUT_07}
		check "${MSG} failed.\n"
		
		clean "${WK_07}"
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
    	
    	# Average Temperature pixel value ( T(a) )
		# T(a) = m * (Z+2) + T(o)
		# m = -0.0064 
		# Z = DEM pixel value
		# T(o) = scale_factor * (mod_value - add_offset) - 273.15
		# scale_factor = 0.01
		# mod_value = MOD08_M3 observed value (layer 917, "retrieved temperature profile mean", band 20)
		# add_offset = -15000
		#										 
		# T(a)= (-0.0064 * (Z+2)) + (0.01*(mod_value + 15000) - 273.15)

		M="-0.0064"
		SCALE_FACTOR="0.01"
		ADD_OFFSET="-15000"
		AVG_TEMP_FORMULA="T(a)=(${M}*(Z+2))+(${SCALE_FACTOR}*(mod_value-(${ADD_OFFSET}))-273.15)"
		log "${AVG_TEMP_FORMULA}\n"
    	
    	NAME_MASK_TOT="Total_mask.tif"
		MSG="Copy filter from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_MASK_TOT} -t ${WK_12}
		check "${MSG} failed.\n"
		
		NAME_DEM_SCALED="Madonie_dem_scaled.tif"
		MSG="Copy DEM from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_DEM_SCALED} -t ${WK_12}
		check "${MSG} failed.\n"
		
    	MASK_TOT="${WK_12}/${NAME_MASK_TOT}"
    	DEM_SCALED="${WK_12}/${NAME_DEM_SCALED}"
		
		UNITY="Celsius degrees"
		METADATA="SITE=${SITE},VALUES=AVG_TEMPERATURE,UNITY_OF_MEASURE=${UNITY}"
		for YYYY in ${YEARS_PROC[@]} ; do
			MONTHS=()
			JULIAN_DAYS=($( ls ${IN_00}/${YYYY} ))
			for JJ in ${JULIAN_DAYS[@]} ; do
				HDF="$( find ${IN_00}/${YYYY}/${JJ} -type f )"
				# Delete zeros in front of julian day: "001" --> "1" or "091" --> "91"
				if [ "${JJ:0:1}" == "0" ] && [ "${JJ:1:1}" == "0" ] ; then
					JJ="${JJ:2:1}"
				elif [ "${JJ:0:1}" == "0" ] && [ "${JJ:1:1}" != "0" ] ; then
					JJ="${JJ:1:2}"
				fi
				MM=$( date -d "`date +${YYYY} `-01-01 +$(( ${JJ} - 1 ))days" +%m )
				DATE="${YYYY}${MM}"
				
				SUBDATASET="HDF4_EOS:EOS_GRID:\"${HDF}\":mod08:Retrieved_Temperature_Profile_Mean_Mean"
				
				MSG="Extract ${IMG} subdataset"
				OUTPUT_05="${WK_12}/${IMG}_global_latlon-${DATE}.tif"
				log "${MSG} ...\n"
				gdal_translate ${PAR_01} ${SUBDATASET} ${OUTPUT_05} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				BAND="20"
				MSG="Extract band ${BAND} for ${DATE}"
				OUTPUT_06="${WK_12}/${IMG}_latlon-${DATE}_b${BAND}.tif"
				log "${MSG} ...\n"
				gdal_translate  ${PAR_01} -b ${BAND} -a_srs "${PROJ_LONGLAT}" -a_ullr ${WORLD} ${OUTPUT_05} ${OUTPUT_06} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Cut on Sicily for ${DATE}"
				OUTPUT_07="${WK_12}/${IMG}_latlon-${DATE}_b${BAND}_cut.tif"
				log "${MSG} ...\n"
				gdal_translate  ${PAR_01} -projwin ${SICILY} ${OUTPUT_06} ${OUTPUT_07} &>> "${LOGFILE}"
				check "${MSG} failed.\n"				
				
				# Check if I have null values (-9999) inside 2x2 image extracted
				findNull "${OUTPUT_07}"				
				if [ "${?}" == "1" ] ; then
					log "Found a null value (-9999) in band ${BAND}. Trying with previous band.\n"
					BAND="19"
					MSG="Extract band ${BAND} for ${DATE}"
					OUTPUT_06="${WK_12}/${IMG}_latlon-${DATE}_b${BAND}.tif"
					log "${MSG} ...\n"
					gdal_translate  ${PAR_01} -b ${BAND} -a_srs "${PROJ_LONGLAT}" -a_ullr ${WORLD} ${OUTPUT_05} ${OUTPUT_06} &>> "${LOGFILE}"
					check "${MSG} failed.\n"
				
					MSG="Cut on Sicily for ${DATE}"
					OUTPUT_07="${WK_12}/${IMG}_latlon-${DATE}_b${BAND}_cut.tif"
					log "${MSG} ...\n"
					gdal_translate  ${PAR_01} -projwin ${SICILY} ${OUTPUT_06} ${OUTPUT_07} &>> "${LOGFILE}"
					check "${MSG} failed.\n"
					
					findNull "${OUTPUT_07}"				
					if [ "${?}" == "1" ] ; then
						log "Found a null value (-9999) in band ${BAND}. Exiting\n"
						return 0
					fi
				fi		
				
				MSG="Conversion of tiff projection from longlat to UTM for ${DATE}"
				OUTPUT_08="${WK_12}/${IMG}_utm_30m-${DATE}.tif"
				log "${MSG} ...\n"
				gdalwarp ${PAR_01} -t_srs "${PROJ}" -tr ${RES} -${RES} ${OUTPUT_07} ${OUTPUT_08} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Remap and cut UTM geotiff image for ${DATE}"
				OUTPUT_09="${WK_12}/${IMG}_remapped-${DATE}.tif"
				log "${MSG} ...\n"
				${BIN_DIR}/remap -i ${OUTPUT_08} -o ${OUTPUT_09} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Performing second part of ${AVG_TEMP_FORMULA}"
				OUTPUT_10="${WK_12}/${IMG}_${DATE}.tif"
				log "${MSG} ...\n"
				gdal_calc.py -A ${DEM_SCALED} -B ${OUTPUT_09} --outfile=${OUTPUT_10} --calc="(A+(${SCALE_FACTOR}*(B-(${ADD_OFFSET}))-273.15))" &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Mask ${IMG} for ${DATE}"
				OUTPUT_11="${WK_12}/${IMG}_${DATE}_masked.tif"
				log "${MSG} ...\n"
				${BIN_DIR}/applyMask -i ${OUTPUT_10} -m ${MASK_TOT} -o ${OUTPUT_11} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MONTHS+=("${WK_12}/${IMG}_${DATE}_masked.tif")
			done
			
			MSG="Create multiband ${IMG} image for ${YYYY}"
			OUTPUT_12="${WK_12}/${IMG}_${YYYY}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/mergeImg -b ${#MONTHS[@]} -i ${MONTHS[@]} -o ${OUTPUT_12} -m "${METADATA}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
		
			MSG="Copy ${IMG} for ${YYYY} into ${OUT_12}"
			log "${MSG} ...\n"
			cp ${OUTPUT_12} -t ${OUT_12}
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
		# T_min(o) = scale_factor * (T_min_mod_value - add_offset) - 273.15
		# T_max(o) = scale_factor * (T_max_mod_value - add_offset) - 273.15
		# scale_factor = 0.01
		# T_min_mod_value = MOD08_M3 observed value (layer 919, "Retrieved_Temperature_Profile_Mean_Min", band 20)
		# T_max_mod_value = MOD08_M3 observed value (layer 920, "Retrieved_Temperature_Profile_Mean_Max", band 20)
		# add_offset = -15000
		
		P1="0.6108"
		P2="17.27"
		P3="237.3"
		SCALE_FACTOR="0.01"
		ADD_OFFSET="-15000"

		VPD_FORMULA="VPD=((e(T_max)+e(T_min))/2)-(e(T_min))"
		log "${AVG_TEMP_FORMULA}\n"
    	
    	NAME_MASK_TOT="Total_mask.tif"
		MSG="Copy filter from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_MASK_TOT} -t ${WK_13}
		check "${MSG} failed.\n"
		
		NAME_DEM_SCALED="Madonie_dem_scaled.tif"
		MSG="Copy DEM from ${OUT_00}"
		log "${MSG} ...\n"
		cp ${OUT_00}/${NAME_DEM_SCALED} -t ${WK_13}
		check "${MSG} failed.\n"
		
    	MASK_TOT="${WK_13}/${NAME_MASK_TOT}"
    	DEM_SCALED="${WK_13}/${NAME_DEM_SCALED}"    	
		
		UNITY="kPa"
		METADATA="SITE=${SITE},VALUES=VPD,UNITY_OF_MEASURE=${UNITY}"
		for YYYY in ${YEARS_PROC[@]} ; do
			MONTHS=()
			JULIAN_DAYS=($( ls ${IN_00}/${YYYY} ))
			for JJ in ${JULIAN_DAYS[@]} ; do
				HDF="$( find ${IN_00}/${YYYY}/${JJ} -type f )"
				# Delete zeros in front of julian day: "001" --> "1" or "091" --> "91"
				if [ "${JJ:0:1}" == "0" ] && [ "${JJ:1:1}" == "0" ] ; then
					JJ="${JJ:2:1}"
				elif [ "${JJ:0:1}" == "0" ] && [ "${JJ:1:1}" != "0" ] ; then
					JJ="${JJ:1:2}"
				fi
				MM=$( date -d "`date +${YYYY} `-01-01 +$(( ${JJ} - 1 ))days" +%m )
				DATE="${YYYY}${MM}"
				
				SUBDATASET1="HDF4_EOS:EOS_GRID:\"${HDF}\":mod08:Retrieved_Temperature_Profile_Mean_Min"
				SUBDATASET2="HDF4_EOS:EOS_GRID:\"${HDF}\":mod08:Retrieved_Temperature_Profile_Mean_Max"
				
				MSG="Extract T_min subdataset for ${DATE}"
				OUTPUT_01="${WK_13}/${IMG}_T_min_global_latlon-${DATE}.tif"
				log "${MSG} ...\n"
				gdal_translate ${PAR_01} ${PAR_02} ${SUBDATASET1} ${OUTPUT_01} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Extract T_max subdataset for ${DATE}"
				OUTPUT_02="${WK_13}/${IMG}_T_max_global_latlon-${DATE}.tif"
				log "${MSG} ...\n"
				gdal_translate ${PAR_01} ${PAR_02} ${SUBDATASET2} ${OUTPUT_02} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				# T_min ------------------------------------------------------------------------
				BAND1="20"
				MSG="Extract band ${BAND1} for ${DATE}"
				OUTPUT_03="${WK_13}/${IMG}_T_min_latlon-${DATE}_b${BAND1}.tif"
				log "${MSG} ...\n"
				gdal_translate  ${PAR_01} -b ${BAND1} -a_srs "${PROJ_LONGLAT}" -a_ullr ${WORLD} ${OUTPUT_01} ${OUTPUT_03} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Cut T_min on Sicily for ${DATE}"
				OUTPUT_04="${WK_13}/${IMG}_T_min_latlon-${DATE}_b${BAND1}_cut.tif"
				log "${MSG} ...\n"
				gdal_translate  ${PAR_01} -projwin ${SICILY} ${OUTPUT_03} ${OUTPUT_04} &>> "${LOGFILE}"
				check "${MSG} failed.\n"				
				
				# Check if I have null values (-9999) inside 2x2 image extracted
				findNull "${OUTPUT_04}"				
				if [ "${?}" == "1" ] ; then
					log "Found a null value (-9999) in band ${BAND1}. Trying with previous band.\n"
					BAND1="19"
					MSG="Extract band ${BAND1} for ${DATE}"
					OUTPUT_03="${WK_13}/${IMG}_T_min_latlon-${DATE}_b${BAND1}.tif"
					log "${MSG} ...\n"
					gdal_translate  ${PAR_01} -b ${BAND1} -a_srs "${PROJ_LONGLAT}" -a_ullr ${WORLD} ${OUTPUT_01} ${OUTPUT_03} &>> "${LOGFILE}"
					check "${MSG} failed.\n"
				
					MSG="Cut T_min on Sicily for ${DATE}"
					OUTPUT_04="${WK_13}/${IMG}_T_min_latlon-${DATE}_b${BAND1}_cut.tif"
					log "${MSG} ...\n"
					gdal_translate  ${PAR_01} -projwin ${SICILY} ${OUTPUT_03} ${OUTPUT_04} &>> "${LOGFILE}"
					check "${MSG} failed.\n"
					
					findNull "${OUTPUT_04}"				
					if [ "${?}" == "1" ] ; then
						log "Found a null value (-9999) in band ${BAND1}. Exiting\n"
						return 0
					fi
				fi
				# T_max ------------------------------------------------------------------------
				BAND2="20"
				MSG="Extract band ${BAND2} for ${DATE}"
				OUTPUT_05="${WK_13}/${IMG}_T_max_latlon-${DATE}_b${BAND2}.tif"
				log "${MSG} ...\n"
				gdal_translate  ${PAR_01} -b ${BAND2} -a_srs "${PROJ_LONGLAT}" -a_ullr ${WORLD} ${OUTPUT_02} ${OUTPUT_05} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Cut T_max on Sicily for ${DATE}"
				OUTPUT_06="${WK_13}/${IMG}_T_max_latlon-${DATE}_b${BAND2}_cut.tif"
				log "${MSG} ...\n"
				gdal_translate  ${PAR_01} -projwin ${SICILY} ${OUTPUT_05} ${OUTPUT_06} &>> "${LOGFILE}"
				check "${MSG} failed.\n"				
				
				# Check if I have null values (-9999) inside 2x2 image extracted
				findNull "${OUTPUT_06}"				
				if [ "${?}" == "1" ] ; then
					log "Found a null value (-9999) in band ${BAND2}. Trying with previous band.\n"
					BAND2="19"
					MSG="Extract band ${BAND2} for ${DATE}"
					OUTPUT_05="${WK_13}/${IMG}_T_max_latlon-${DATE}_b${BAND2}.tif"
					log "${MSG} ...\n"
					gdal_translate  ${PAR_01} -b ${BAND2} -a_srs "${PROJ_LONGLAT}" -a_ullr ${WORLD} ${OUTPUT_02} ${OUTPUT_05} &>> "${LOGFILE}"
					check "${MSG} failed.\n"
				
					MSG="Cut T_max on Sicily for ${DATE}"
					OUTPUT_06="${WK_13}/${IMG}_T_max_latlon-${DATE}_b${BAND2}_cut.tif"
					log "${MSG} ...\n"
					gdal_translate  ${PAR_01} -projwin ${SICILY} ${OUTPUT_05} ${OUTPUT_06} &>> "${LOGFILE}"
					check "${MSG} failed.\n"
					
					findNull "${OUTPUT_06}"				
					if [ "${?}" == "1" ] ; then
						log "Found a null value (-9999) in band ${BAND2}. Exiting\n"
						return 0
					fi
				fi
				# ------------------------------------------------------------------------------
				
				MSG="Conversion of tiff projection from longlat to UTM (T_min, ${DATE})"
				OUTPUT_07="${WK_13}/${IMG}_T_min_utm_30m-${DATE}.tif"
				log "${MSG} ...\n"
				gdalwarp ${PAR_01} -t_srs "${PROJ}" -tr ${RES} -${RES} ${OUTPUT_04} ${OUTPUT_07} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Conversion of tiff projection from longlat to UTM (T_max, ${DATE})"
				OUTPUT_08="${WK_13}/${IMG}_T_max_utm_30m-${DATE}.tif"
				log "${MSG} ...\n"
				gdalwarp ${PAR_01} -t_srs "${PROJ}" -tr ${RES} -${RES} ${OUTPUT_06} ${OUTPUT_08} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Remap and cut UTM geotiff image for T_min, ${DATE}"
				OUTPUT_09="${WK_13}/${IMG}_T_min_remapped-${DATE}.tif"
				log "${MSG} ...\n"
				${BIN_DIR}/remap -i ${OUTPUT_07} -o ${OUTPUT_09} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Remap and cut UTM geotiff image for T_max, ${DATE}"
				OUTPUT_10="${WK_13}/${IMG}_T_max_remapped-${DATE}.tif"
				log "${MSG} ...\n"
				${BIN_DIR}/remap -i ${OUTPUT_08} -o ${OUTPUT_10} -s ${RES} -m -l ${UL_LAT} ${UL_LON} -e ${SIZEX}x${SIZEY} -w 5x5 &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Getting T_min for ${DATE}"
				T_MIN="${WK_13}/${IMG}_T_min_${DATE}.tif"
				log "${MSG} ...\n"
				gdal_calc.py -A ${DEM_SCALED} -B ${OUTPUT_09} --outfile=${T_MIN} --calc="(A+(${SCALE_FACTOR}*(B-(${ADD_OFFSET}))-273.15))" &>> "${LOGFILE}"
				check "${MSG} failed.\n"

				MSG="Getting T_max for ${DATE}"
				T_MAX="${WK_13}/${IMG}_T_max_${DATE}.tif"
				log "${MSG} ...\n"
				gdal_calc.py -A ${DEM_SCALED} -B ${OUTPUT_10} --outfile=${T_MAX} --calc="(A+(${SCALE_FACTOR}*(B-(${ADD_OFFSET}))-273.15))" &>> "${LOGFILE}"
				check "${MSG} failed.\n"				
								
				MSG="Getting ${IMG} for ${DATE}"
				OUTPUT_11="${WK_13}/${IMG}_${DATE}.tif"
				log "${MSG} ...\n"
				${BIN_DIR}/getVPD -min ${T_MIN} -max ${T_MAX} -o ${OUTPUT_11} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MSG="Mask ${IMG}"
				OUTPUT_12="${WK_13}/${IMG}_${DATE}_masked.tif"
				log "${MSG} ...\n"
				${BIN_DIR}/applyMask -i ${OUTPUT_11} -m ${MASK_TOT} -o ${OUTPUT_12} &>> "${LOGFILE}"
				check "${MSG} failed.\n"
				
				MONTHS+=("${OUTPUT_12}")
			done
			
			MSG="Create multiband ${IMG} image for ${YYYY}"
			OUTPUT_13="${WK_13}/${IMG}_${YYYY}.tif"
			log "${MSG} ...\n"
			${BIN_DIR}/mergeImg -b ${#MONTHS[@]} -i ${MONTHS[@]} -o ${OUTPUT_13} -m "${METADATA}" &>> "${LOGFILE}"
			check "${MSG} failed.\n"
		
			MSG="Copy ${IMG} into ${OUT_13}"
			log "${MSG} ...\n"
			cp ${OUTPUT_13} -t ${OUT_13}
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
    	
    	NAME_MASK_TOT="Total_mask.tif"
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
		INPUT_02+=("$( ls ${WK_16}/*argilla*30m_masked.tif)" "$( ls ${WK_16}/*limo*30m_masked.tif)" "$( ls ${WK_16}/*sabbia*30m_masked.tif)" "$( ls ${WK_16}/*densita*30m_masked.tif)" "$( ls ${WK_16}/*profondita*30m_masked.tif)")
		
		METADATA="SITE=${SITE},VALUE=SOIL,BAND1=clay (%),BAND2=silt (%),BAND3=sand (%),BAND4=density (g/cm3),BAND5=soil depth (cm)"
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