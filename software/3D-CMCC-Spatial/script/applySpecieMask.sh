#!/bin/bash

YEARS=(2002 2003 2004 2005 2006 2007 2008 2009 2010 2011 2012 2013 2014 2015 2016 2017 2018 2019 2020)
MONTHS=(1 2 3 4 5 6 7 8 9 10 11 12)

MASK="Titerno-Tammaro_quercus_evergreen_7_mask.tif"
EMPTY="empty_good.tif"
PAR="-q -co COMPRESS=LZW -of GTiff -ot Float32 -separate"

IMG5BANDS=(AvDBH Height Y_planted Management N_cell Phenology Species)
IMG12BANDS=(SolarRad Avg_Temp VPD Precip LAI)

gdal_translate -b 2 ${IMG5BANDS[0]}.tif ${EMPTY}

for S in ${IMG5BANDS[@]} ; do
	gdal_calc.py -A ${S}.tif -B ${MASK} --outfile=${S}_new.tif --calc="(A*B)"
	gdal_merge.py ${PAR} ${S}_new.tif ${EMPTY} ${EMPTY} ${EMPTY} ${EMPTY} -o ${S}_good.tif
	rm ${S}_new.tif
done

for C in ${IMG12BANDS[@]} ; do
	for Y in ${YEARS[@]} ; do
		BANDS=()
		for M in ${MONTHS[@]} ; do
			IN="${C}_${Y}.tif"
			OUT="${C}_${Y}_b${M}.tif"			
			gdal_calc.py -A ${IN} --A_band=${M} -B ${MASK} --outfile=${OUT} --calc="(A*B)"
			BANDS+=("${OUT}")
		done
		gdal_merge.py ${PAR} ${BANDS[@]} -o ${C}_${Y}_good.tif
		rm ${BANDS[@]}
	done
done

BANDS=()
for B in 1 2 3 4 5 ; do
	IN="Soil.tif"
	OUT="Soil_b${B}.tif"			
	gdal_calc.py -A ${IN} --A_band=${B} -B ${MASK} --outfile=${OUT} --calc="(A*B)"
	BANDS+=("${OUT}")
done
gdal_merge.py ${PAR} ${BANDS[@]} -o Soil_good.tif
rm ${BANDS[@]}

rm ${EMPTY}

OLD=()
NEW=()
for S in ${IMG5BANDS[@]} ; do
	OLD+=("${S}.tif")
	NEW+=("${S}_good.tif")
done

for S in ${IMG12BANDS[@]} ; do
	for Y in ${YEARS[@]} ; do
		OLD+=("${S}_${Y}.tif")
		NEW+=("${S}_${Y}_good.tif")
	done
done

OLD+=("Soil.tif")
NEW+=("Soil_good.tif")

rm ${OLD[@]}

for N in ${NEW[@]} ; do
	#echo "mv ${N} $( echo ${N} | sed s/_good// )"
	mv ${N} $( echo ${N} | sed s/_good// )
done

exit 0
