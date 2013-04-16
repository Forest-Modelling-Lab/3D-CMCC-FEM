#!/bin/bash 
# Authors: Alessio Collalti, Alessandro Candini

#create folder of downloaded data
# mkdir downloaded_files

#TO DO scarica i file dal sito ucea
# wget http://old.politicheagricole.it/ucea/forniture/index3.htm

BASE_DIR="input/CRA_MET_DATA"
OUT_DIR="output"

echo "Choose a site and write (in capital letters, i.e.: CASTEL DI SANGRO)"
read  \SITE
# On filenames, blanks are substituted with underscores: CASTEL DI SANGRO --> CASTEL_DI_SANGRO
SITENAME=$(echo "${SITE}" | sed 's/\ /_/g')

echo "Chose a variable among: TMAX, TMIN, RAD, PREC, UMI"
read \VARIABLE

if  [[ "${VARIABLE}" == "TMAX" ]] || 
    [[ "${VARIABLE}" == "TMIN" ]] || 
    [[ "${VARIABLE}" == "RAD" ]] || 
    [[ "${VARIABLE}" == "PREC" ]] || 
    [[ "${VARIABLE}" == "UMI" ]] ; then
    IN_DIR="${BASE_DIR}/${VARIABLE}"
else
    echo "${VARIABLE} is a bad choice!!!"
    exit 0
fi

#Marconi: added sort -- version command, to be sure listfiles are sorted BEFORE being read
LISTFILES=($((find "${IN_DIR}" -type f -name '*.gz') | sort --version-sort) )

# Check if file already exists
for I in ${LISTFILES[@]} ; do
    TMPYEAR=$(basename "${I}" | cut -d _ -f 5)
    YEAR="${TMPYEAR:0:4}"
    FILENAME=EXTRACTED_"${SITENAME}"_"${VARIABLE}"_"${YEAR}".txt
    FILEPATH="${OUT_DIR}"/"${FILENAME}"
    if [[ -a "${FILEPATH}" ]] ; then
        echo "File ${FILEPATH} already exists: deleting it"
        rm -v ${FILEPATH}
    else
        echo "File ${FILEPATH} doesn't exist: creating it"
    fi
done

for I in ${LISTFILES[@]} ; do
    TMPYEAR=$(basename "${I}" | cut -d _ -f 5)
    YEAR="${TMPYEAR:0:4}"
    FILENAME=EXTRACTED_"${SITENAME}"_"${VARIABLE}"_"${YEAR}".txt
    FILEPATH="${OUT_DIR}"/"${FILENAME}"

    # Decompress files (grep on site with blanks)
    # If files are already present they will be over written
    echo "${I}"
    gunzip -dc ${I} | grep "${SITE}" >> "${FILEPATH}"
    if [ "${?}" -ne "0" ] ; then
        echo "--> ERROR: File ${I} is corrupted! :-("
        # exit 1
    fi   
    
    #Marconi: check if there's some months lacking: 
		COUNTER=$(gunzip -dc ${I} | grep -c "${SITE}")
		if [[ "${COUNTER}" == "0" ]] ; then
  		echo "--> ERROR: no ${SITE} data found for ${I}! "
    fi
	
    # Check if file are equal to or bigger than zero
    if [[ -s "${FILEPATH}" ]] ; then
        echo "File ${FILEPATH} is bigger than 0 bytes"
    else
        echo "file ${FILEPATH} is empty"
    fi

    echo -ne 'Check if file contains NO DATA\n\n'
    cat "${FILEPATH}" | sed 's/--/-9999/g' > "${FILEPATH}".tmp
    mv "${FILEPATH}".tmp "${FILEPATH}"

done

exit 0
