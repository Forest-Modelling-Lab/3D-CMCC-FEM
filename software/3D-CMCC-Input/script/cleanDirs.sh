#!/bin/bash

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
# Working directories
WK_00="$( dirname ${0} )/../working/00_Filters"
WK_01="$( dirname ${0} )/../working/01_Y_planted"
WK_02="$( dirname ${0} )/../working/02_Species"
WK_03="$( dirname ${0} )/../working/03_Phenology"
WK_04="$( dirname ${0} )/../working/04_Management"
WK_05="$( dirname ${0} )/../working/05_N_cell"
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

OUT_DIRS=(${OUT_00} ${OUT_01} ${OUT_02} ${OUT_03} ${OUT_04} ${OUT_05} ${OUT_06} ${OUT_07} ${OUT_08} ${OUT_09} ${OUT_10} ${OUT_11} ${OUT_12} ${OUT_13} ${OUT_14} ${OUT_15} ${OUT_16})
WK_DIRS=(${WK_00} ${WK_01} ${WK_02} ${WK_03} ${WK_04} ${WK_05} ${WK_06} ${WK_07} ${WK_08} ${WK_09} ${WK_10} ${WK_11} ${WK_12} ${WK_13} ${WK_14} ${WK_15} ${WK_16})


usage(){
	echo "Usage: ${0} [wk|out|all]"
	echo "       - Clean working and/or output directories for UNCEM input image creation project."
	echo "       - Accepted input values are:"
	echo "             wk: clean only working directories"
	echo "             out: clean only output directories"
	echo "             all: clean working and output directories"
	echo ""
	echo "Run example: ${0} all"
	exit 40
}

clean_wk(){
	echo "Deleting content of working directories..."
	for DIR in "${WK_DIRS[@]}" ; do
		rm -fr ${DIR}/*
	done
	echo "...done"
}

clean_out(){
	echo "Deleting content of output directories..."
	for DIR in "${OUT_DIRS[@]}" ; do
		rm -fr ${DIR}/*
	done
	echo "...done"
}


if [ -z "${1}" ] ; then # If I have no input parameters...
	usage
else
	if [ "${1}" == "wk" ] ; then
		clean_wk
	elif [ "${1}" == "out" ] ; then
		clean_out
	elif [ "${1}" == "all" ] ; then
		clean_wk
		clean_out
	else
		usage
	fi
fi

exit 0
