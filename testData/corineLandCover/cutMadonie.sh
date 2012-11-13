#!/bin/bash

#gdal_translate -co COMPRESS=LZW -of GTiff -a_srs '+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs' CORINE_3211.asc CORINE_3211.tif
#gdalwarp -co COMPRESS=LZW -of GTiff -t_srs '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs' -tr 0.00045266691956530711 0.00045266691956530711 CORINE_3211.tif CORINE_3211_latlon.tif
#gdal_translate -co COMPRESS=LZW -of GTiff -a_srs '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs' -projwin 13.855128040667733 38.051144527880616 14.2937623 37.7383517 CORINE_3211_latlon.tif CORINE_3211_cut.tif
#gdal_translate -co COMPRESS=LZW -of GTiff -a_ullr 13.855128040667733 38.051144527880616 14.2937623 37.7383517 CORINE_3211_cut.tif CORINE_3211_shifted.tif
#gdalwarp -co COMPRESS=LZW -of GTiff -t_srs '+proj=utm +zone=33 +ellps=WGS84 +datum=WGS84 +units=m +no_defs' -tr 30 -30 CORINE_3211_shifted.tif CORINE_3211_Madonie.tif


for I in $( ls *.asc ) ; do
	OUT1=$( echo ${I} | sed s/.asc/.tif/ )
	OUT2=$( echo ${I} | sed s/.asc/_latlon.tif/ )
	OUT3=$( echo ${I} | sed s/.asc/_cut.tif/ )
	OUT4=$( echo ${I} | sed s/.asc/_shifted.tif/ )
	OUT5=$( echo ${I} | sed s/.asc/_Madonie_30m.tif/ )

	gdal_translate -co COMPRESS=LZW -of GTiff -a_srs '+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs' ${I} ${OUT1} ;
	gdalwarp -co COMPRESS=LZW -of GTiff -t_srs '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs' -tr 0.00045266691956530711 0.00045266691956530711 ${OUT1} ${OUT2} ;
	gdal_translate -co COMPRESS=LZW -of GTiff -a_srs '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs' -projwin 13.855128040667733 38.051144527880616 14.2937623 37.7383517 ${OUT2} ${OUT3} ;
	gdal_translate -co COMPRESS=LZW -of GTiff -a_ullr 13.855128040667733 38.051144527880616 14.2937623 37.7383517 ${OUT3} ${OUT4} ;
	gdalwarp -co COMPRESS=LZW -of GTiff -t_srs '+proj=utm +zone=33 +ellps=WGS84 +datum=WGS84 +units=m +no_defs' -tr 30 -30 ${OUT4} ${OUT5} ;

	rm -v ${OUT1} ${OUT2} ${OUT3} ${OUT4} ;
done

exit 0
