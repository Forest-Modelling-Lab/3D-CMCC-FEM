#!/bin/bash -x

#create folder of downloaded data
# mkdir downloaded_files

#TO DO scarica i file dal sito ucea
# wget http://old.politicheagricole.it/ucea/forniture/index3.htm

cd input/CRA_MET_DATA

echo "choose a site and write (in capital letters)"
read  \site 
echo "site choiced : ${site}"

echo "chose a variable among: TMAX, TMIN, RAD, PREC, UMI"
read \variable

if  [[ "${variable}" == "TMAX" ]] || 
    [[ "${variable}" == "TMIN" ]] || 
    [[ "${variable}" == "RAD" ]] || 
    [[ "${variable}" == "PREC" ]] || 
    [[ "${variable}" == "UMI" ]] ; then
    cd "${variable}"
else
    echo "${variable} is a bad choice!!!"
    exit 0
fi

filesToParse=($(find -type f -name '*.gz'))

for I in ${filesToParse[@]} ; do
    yearTmp=$(echo "${I}" | cut -d _ -f 5)
    year="${yearTmp:0:4}"
    fileName=EXTRACTED_"${site}"_"${variable}"_"${year}".txt

    #control file
    if [[ -a "${fileName}" ]] ; then
        echo "file ${fileName} still exist"
        echo "delete old  OR create a new ${fileName}"
        rm  ${fileName}
    else
        echo "file doesnt exist"
        echo "create${fileName}"
    fi

    gunzip -dc ${I} | grep  "${site}" >> "${fileName}" 

    #control files
    if [[ -s "${fileName}" ]] ; then
        echo "file ${fileName} is valid"
    else
        echo "file ${fileName} is empty"
    fi

    #control if files contain NO DATA
    echo 'control no data values...'
    cat "${fileName}" | sed 's/--/-9999/g' > "${fileName}".tmp
    mv "${fileName}".tmp "${fileName}"

    
done





exit 0
