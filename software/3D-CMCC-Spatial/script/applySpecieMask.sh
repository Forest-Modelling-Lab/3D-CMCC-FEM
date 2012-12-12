#!/bin/bash

# Species identification numbers: 
# 0 = "-9999" (Undefined)
# 1 = "Castaneasativa"
# 2 = "Fagussylvatica"
# 3 = "Ostryacarpinifolia" 
# 4 = "Pinusnigra"
# 5 = "Quercuscerris"
# 6 = "quercus_deciduous" (Q. cerris, Q. robur, Q. pubescens, Q. petreae)
# 7 = "quercus_evergreen" (Q. ilex, Q. suber)
SPECIES_ID=(Undefined Castaneasativa Fagussylvatica Ostryacarpinifolia Pinusnigra Quercuscerris quercus_deciduous quercus_evergreen)
SPECIES_ID_PRESENT=(1 2 6 7)
PREF="Madonie"
YEARS=(2002 2003 2004 2005 2006 2007 2008 2009)

for IDX in "${SPECIES_ID_PRESENT[@]}" ; do
	SPECIES_MASK_NAMES[${IDX}]="${PREF}_${SPECIES_ID[${IDX}]}_${IDX}_mask.tif"
  	SPECIES_MASK[${IDX}]="${SPECIES_MASK_NAMES[${IDX}]}"		
done

for X in GPP NPP ; do
	for Y in ${YEARS[@]} ; do
		for Z in ${SPECIES_MASK[@]} ; do
			IMG="${X}_sum_${Y}.tif"
			MSK="${Z}"
			POSTFIX="${MSK:${#PREF}}"
			OUT_TMP="${X}_sum_${Y}${POSTFIX}"
			OUT=$( echo ${OUT_TMP} | sed s/_mask// )
			gdal_calc.py -A ${IMG} -B ${MSK} --outfile=${OUT} --calc="(A*B)"
		done
	done
done

exit 0
