#!/bin/bash

for I in $( ls *.asc ) ; do
	OUT1=$( echo ${I} | sed s/.asc/.tif/ )
	OUT2=$( echo ${I} | sed s/.asc/_zone33.tif/ )
	OUT3=$( echo ${I} | sed s/.asc/_Madonie_30m.tif/ )

	gdal_translate -co COMPRESS=LZW -of GTiff -a_srs '+proj=utm +zone=32 +ellps=WGS84 +datum=WGS84 +units=m +no_defs' ${I} ${OUT1} ;
	gdalwarp -co COMPRESS=LZW -of GTiff -t_srs '+proj=utm +zone=33 +ellps=WGS84 +datum=WGS84 +units=m +no_defs' ${OUT1} ${OUT2} ;
	./remap -i ${OUT2} -o ${OUT3} -s 30 -m -l 4211980.487859229557216 399333.291887304978445 -e 1286x1160 -w 5x5 ;

	rm -v ${OUT1} ${OUT2} ;
done

exit 0
