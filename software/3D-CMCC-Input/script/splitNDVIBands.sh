#!/bin/bash

DIRS=( Campania-Titerno Sicilia-Madonie )
PAR_01="-q -co COMPRESS=LZW -of GTiff -ot Float32"

# MEA pattern: SENSOR_PRODUCT_DATE.TIME.TEMPORALRESOLUTION_*.tif
# Example:     LANDSAT_NDVI_19991215.000000.1d.tif
SENSOR="LANDSAT"
PRODUCT="NDVI"
TIME="000000"
TEMPORALRESOLUTION="1d"


for DIRECTORY in ${DIRS[@]} ; do
	for INPUT_01 in $( ls ${DIRECTORY}/LayerDates* ) ; do
    	INPUT_02="$( echo ${INPUT_01} | sed s/LayerDates_// | sed s/$/.tif/ )"

    	IDX="1"
    	while read LINE; do
			DATE=$( echo ${LINE:0:10} | tr -d '-' )
    		OUTPUT_01="${DIRECTORY}/${SENSOR}_${PRODUCT}_${DATE}.${TIME}.${TEMPORALRESOLUTION}.tif"
			echo -ne "Producing ${OUTPUT_01} (band ${IDX})..."
			gdal_translate ${PAR_01} -b ${IDX} ${INPUT_02} ${OUTPUT_01}
			echo "done"
			IDX=$(( ${IDX} + 1 ));
		done < "${INPUT_01}"
	done
done



exit 0
