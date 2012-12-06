#!/bin/bash

rm -fr 3D-CMCC-Forest-Model 3D-CMCC-Spatial wrapCMCC mergeImg

svn co https://svn.services.meeo.it/forSE/trunk/software/3D-CMCC-Spatial

svn co https://svn.services.meeo.it/forSE/trunk/software/3D-CMCC-Forest-Model
cd 3D-CMCC-Forest-Model
autoreconf -i
./configure
make
cp src/3D_CMCC_Forest_Model ../3D-CMCC-Spatial/bin
cd -

svn co https://svn.services.meeo.it/sistema-Utilities/trunk/wrapCMCC
cd wrapCMCC/src
make
cp getInputCMCC wrapCMCC getOutputCMCC -t ../../3D-CMCC-Spatial/bin
cd -

svn co https://svn.services.meeo.it/sistema-Utilities/trunk/mergeImg
cd mergeImg/src
make
cp mergeImg -t ../../3D-CMCC-Spatial/bin
cd -

exit 0
