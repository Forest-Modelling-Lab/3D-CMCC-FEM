#!/bin/bash

#gdal_translate -co COMPRESS=LZW -of GTiff -a_srs '+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs' CORINE_3211.asc CORINE_3211.tif
#gdalwarp -co COMPRESS=LZW -of GTiff -t_srs '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs' -tr 0.00045266691956530711 0.00045266691956530711 CORINE_3211.tif CORINE_3211_latlon.tif
#gdal_translate -co COMPRESS=LZW -of GTiff -a_srs '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs' -projwin 13.855128040667733 38.051144527880616 14.2937623 37.7383517 CORINE_3211_latlon.tif CORINE_3211_cut.tif
#gdal_translate -co COMPRESS=LZW -of GTiff -a_ullr 13.855128040667733 38.051144527880616 14.2937623 37.7383517 CORINE_3211_cut.tif CORINE_3211_shifted.tif
#gdalwarp -co COMPRESS=LZW -of GTiff -t_srs '+proj=utm +zone=33 +ellps=WGS84 +datum=WGS84 +units=m +no_defs' -tr 30 -30 CORINE_3211_shifted.tif CORINE_3211_Madonie.tif


for I in $( ls *.asc ) ; do
	O=$( echo ${I} | sed s/.asc/.tif/ )
	gdal_translate -co COMPRESS=LZW -of GTiff -a_srs '+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs' ${I} ${O} ;
done

for I in $( ls *.tif) ; do
	O=$( echo ${I} | sed s/.tif/_latlon.tif/ )
	gdalwarp -co COMPRESS=LZW -of GTiff -t_srs '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs' -tr 0.00045266691956530711 0.00045266691956530711 ${I} ${O} ;
	rm -v ${I} ;
done

for I in $( ls *_latlon.tif) ; do
	O=$( echo ${I} | sed s/_latlon.tif/_cut.tif/ )
	gdal_translate -co COMPRESS=LZW -of GTiff -a_srs '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs' -projwin 13.855128040667733 38.051144527880616 14.2937623 37.7383517 ${I} ${O} ;
	rm -v ${I} ;
done

for I in $( ls *_cut.tif) ; do
	O=$( echo ${I} | sed s/_cut.tif/_shifted.tif/ )
	gdal_translate -co COMPRESS=LZW -of GTiff -a_ullr 13.855128040667733 38.051144527880616 14.2937623 37.7383517 ${I} ${O} ;
	rm -v ${I} ;
done

for I in $( ls *_shifted.tif) ; do
	O=$( echo ${I} | sed s/_shifted.tif/_Madonie_30m.tif/ )
	gdalwarp -co COMPRESS=LZW -of GTiff -t_srs '+proj=utm +zone=33 +ellps=WGS84 +datum=WGS84 +units=m +no_defs' -tr 30 -30 ${I} ${O} ;
	rm -v ${I} ;
done

exit 0
