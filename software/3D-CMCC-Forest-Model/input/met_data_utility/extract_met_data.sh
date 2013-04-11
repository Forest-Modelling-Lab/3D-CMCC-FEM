#!/bin/bash 

#create folder of downloaded data
# mkdir downloaded_files

#TO DO scarica i file dal sito ucea
# wget http://old.politicheagricole.it/ucea/forniture/index3.htm

cd CRA_MET_DATA




echo "choose a site and write (in capital letters)"
read  \site 
echo "site choiced :" "$site"

echo "chose a variable between: TMAX, TMIN, RAD, PREC, UMI"
read \variable

if  [[ "${variable}" == "TMAX" ]] || 
    [[ "${variable}" == "TMIN" ]] || 
    [[ "${variable}" == "RAD" ]] || 
    [[ "${variable}" == "PREC" ]] || 
    [[ "${variable}" == "UMI" ]] ; then
    cd "${variable}"
else
  echo ''"$variable"' is a bad choice!!!'
  exit 0
fi



filesToParse=($(find -type f -name '*.gz'))

#control file
if [[ -a EXTRACTED_"$site".txt ]] ; then
  echo 'file EXTRACTED_'"$site"'.txt still exist '
  echo 'delete old  OR create a new EXTRACTED_'"$site"'.txt '
  rm  EXTRACTED_"$site_$variable".txt
else
  echo 'file doesnt exist'
  echo 'create  EXTRACTED_'"$site"_"$variable"'.txt '
fi

for I in ${filesToParse[@]} ; do
	gunzip -dc ${I} | grep  "$site" >> EXTRACTED_"$site"_"$variable".txt
done




#control files
if [[ -s EXTRACTED_"$site"_"$variable".txt ]] ; then
  echo 'file EXTRACTED_'"$site"_"$variable"'.txt is valid'
  else
  echo 'file EXTRACTED_'"$site"_"$variable"'.txt is empty'
fi

#control if files contain NO DATA
echo 'control no data values...'
cat EXTRACTED_"$site"_"$variable".txt | sed 's/--/-9999/g' > EXTRACTED_"$site"_"$variable".txt.tmp
mv EXTRACTED_"$site"_"$variable".txt.tmp EXTRACTED_"$site"_"$variable".txt



mv  EXTRACTED_"$site"_"$variable".txt .. 


exit 0
