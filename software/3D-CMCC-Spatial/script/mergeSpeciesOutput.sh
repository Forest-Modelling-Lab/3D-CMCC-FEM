#!/bin/bash
### File information summary  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
# Description:    mergeSpeciesOutput.sh bash shell script
#                 Processing chain to merge different output images from several 3D-CMCC-Spatial outputs into one multi-species packet
# Author:         Alessandro Candini - candini@meeo.it
# Version:        0.1
# Copyright:      MEEO S.R.L. - www.meeo.it 
# Changelog:      2013-01-08 - version 0.1
#                     - First Release
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  File information summary }

### Global functions definitions  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
usage(){
	echo "Usage: ${0} ZIP_PACKAGES"
	echo "       - ZIP_PACKAGES is a mandatory array of 3D-CMCC-Spatial output zip packages to be merged into one multi-species package."
	echo ""
	echo "Run example: ${0} MADONIE_30m_2002-2020_20130107000001_output.zip MADONIE_30m_2002-2020_20130107000002_output.zip MADONIE_30m_2002-2020_20130107000007_output.zip MADONIE_30m_2002-2020_20130107000007_output.zip"
	echo ""
	exit 40
}
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  Global functions definitions }

### Checking input arguments and configurations - - - - - - - - - - - - - - - - - - - - - - - - - - - - - {
# Check input parameters
if [ -z "${1}" ] ; then # If I have no input packages, exit
	echo "Put at least 2 input packages to merge together."
	usage
else
	for PKG in "${@}" ; do
		if [ ! -f "${PKG}" ] ; then
    		echo "One of the input package does not exist or has a wrong path."
    		echo ""
    		usage
    	fi
	done
fi
### - - - - - - - - - - - - - - - - - - - - - - - - - - - - - Checking input arguments and configurations }


exit 0