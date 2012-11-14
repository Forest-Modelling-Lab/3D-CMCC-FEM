#!/bin/bash

for I in $( ls *.asc ) ; do
	OUT1=$( echo ${I} | sed s/.asc/.tif/ )
	OUT2=$( echo ${I} | sed s/.asc/_latlon.tif/ )
	OUT3=$( echo ${I} | sed s/.asc/_cut.tif/ )
	OUT4=$( echo ${I} | sed s/.asc/_shifted.tif/ )
	OUT5=$( echo ${I} | sed s/.asc/_Madonie_30m_first.tif/ )
	OUT6=$( echo ${I} | sed s/.asc/_Madonie_30m.tif/ )

	gdal_translate -co COMPRESS=LZW -of GTiff -a_srs '+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs' ${I} ${OUT1} ;
	gdalwarp -co COMPRESS=LZW -of GTiff -t_srs '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs' -tr 0.00045266691956530711 0.00045266691956530711 ${OUT1} ${OUT2} ;
	gdal_translate -co COMPRESS=LZW -of GTiff -a_srs '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs' -projwin 13.855128040667733 38.051144527880616 14.2937623 37.7383517 ${OUT2} ${OUT3} ;
	gdal_translate -co COMPRESS=LZW -of GTiff -a_ullr 13.855128040667733 38.051144527880616 14.2937623 37.7383517 ${OUT3} ${OUT4} ;
	gdalwarp -co COMPRESS=LZW -of GTiff -t_srs '+proj=utm +zone=33 +ellps=WGS84 +datum=WGS84 +units=m +no_defs' -tr 30 -30 ${OUT4} ${OUT5} ;
	gdal_translate -co COMPRESS=LZW -srcwin 7 4 1286 1160 -a_ullr 399333.291887304978445 4211980.487859229557216 437913.292 4177180.488 ${OUT5} ${OUT6} ;

	rm -v ${OUT1} ${OUT2} ${OUT3} ${OUT4} ${OUT5} ;
done

exit 0
