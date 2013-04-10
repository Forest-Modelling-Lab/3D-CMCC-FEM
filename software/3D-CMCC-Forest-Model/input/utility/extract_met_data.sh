#!/bin/bash 

#create folder of downloaded data
# mkdir downloaded_files

#TO DO scarica i file dal sito ucea
# wget http://old.politicheagricole.it/ucea/forniture/index3.htm

cd CRA_MET_DATA

filesToParse=($(find -type f -name '*.gz'))


echo "choose a site and write (in capital letters)"
read  \site 
echo "site choiced :" "$site"

//echo "chose a variable"
//read \variable


#control file
if [[ -a EXTRACTED_"$site".txt ]] ; then
  echo 'file EXTRACTED_'"$site"'.txt still exist '
  echo 'delete old  and create a new EXTRACTED_'"$site"'.txt '
  rm  EXTRACTED_"$site".txt
else
  echo 'file doesnt exist'
  echo 'create  EXTRACTED_'"$site"'.txt '
fi

for I in ${filesToParse[@]} ; do
	gunzip -dc ${I} | grep  "$site" >> EXTRACTED_"$site".txt
done




#control row
if [[ -s EXTRACTED_"$site".txt ]] ; then
  echo 'file EXTRACTED_'"$site"'.txt is valid'
  else
  echo 'file EXTRACTED_'"$site"'.txt is empty'
fi



mv  EXTRACTED_"$site".txt .. 


exit 0
