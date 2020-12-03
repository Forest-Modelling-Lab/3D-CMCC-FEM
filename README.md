# 3D-CMCC-FEM  
# *(Three Dimensional - Coupled Model Carbon Cycle - Forest Ecosystem Model)*

![FML_4](https://user-images.githubusercontent.com/27204956/99875955-c8bfea80-2bf3-11eb-821c-c02a068990d4.jpg)

**3D-CMCC-FEM *(Three Dimensional - Coupled Model Carbon Cycle - Forest Ecosystem Model)* repository**

LICENSE:

Copyright (c) 2020, Forest Modelling Laboratory – 3D-CMCC-FEM  
All rights reserved


The **3D-CMCC-FEM** is basically a research tool which is freely available ***only*** for non-commercial use. We have developed the **3D-CMCC-FEM** code relying solely on open source components, in order to facilitate its use and further development by others. 

The **3D-CMCC-FEM** is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. The **3D-CMCC-FEM** code is released under the GNU General Public Licence (GPL). See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see http://www.gnu.org/licenses/gpl.html.

This page contains all the code releases developed over the time on the open source distribution of the computer simulation forest model **3D-CMCC-FEM**. The model has been developed and is maintained by the **Forest Modelling Laboratory** at the National Research Council of Italy, Institute for Agricultural and Forestry Systems in the Mediterranean (CNR-ISAFOM), Perugia, and at the University of Tuscia, Department of Innovation in Biological, Agro-food and Forest Systems (UNITUS-DIBAF), Viterbo.  

All source code and documents provided here are subject to copyright (c) by the CNR-ISAFOM and UNITUS-DIBAF.     

In case you have copied and/or modified the **3D-CMCC-FEM** code overall, even in small parts of it, you may not publish data from it using the name **3D-CMCC-FEM** or any **3D-CMCC-FEM** variants unless you have either coordinated your usage and their changes with the developers listed below, or publish enough details about your changes so that they could be replicated.

The **3D-CMCC-FEM** has been developed by:  
Alessio Collalti    
Alessio Ribeca  
Carlo Trotta  
Daniela Dalmonech  
Gina Marano  

*Forest Ecology Laboratory* at the National Research Council of Italy (CNR),Institute for Agricultural and Forestry Systems in the Mediterranean (ISAFOM), Via della Madonna Alta, 128, 06128 - Perugia (PG), Italy, and Tuscia University (UNITUS), Department for innovation in biological, agro-food and forest systems (DIBAF), Via S. Camillo de Lellis, snc 01100 - Viterbo, Italy.   

*DISCLAIMER*  
CNR and UNITUS accepts no responsibility for the use of the **3D-CMCC-FEM** in the form supplied or as subsequently modified by third parties. CNR and UNITUS disclaims liability for all losses,damages and costs incurred by any person as a result of relying on this software. Use of this software assumes agreement to this condition of use. Removal of this statement violates the spirit in which **3D-CMCC-FEM** was released by CNR and UNITUS.

For any further information please contact us:  
-Alessio Collalti (Lab. Head and Model Principal Investigator): alessio.collalti@cnr.it  
-Carlo Trotta: trottacarlo@unitus.it  
-Daniela Dalmonech: daniela.dalmonech@gmail.com  
-Alessio Ribeca: a.ribeca@unitus.it  
-Gina Marano: gina.marano@unina.it  
-The model: 3d-cmcc-fem@gmail.com  
-The lab: forest.modelling.lab@gmail.com  

or look at the **3D-CMCC-FEM** web-site:https://www.forest-modelling-lab.com/the-3d-cmcc-model

For references on **3D-CMCC-FEM**:

1. *"Sviluppo di un modello ecologico-forestale per foreste a struttura complessa"*. Collalti A., Valentini R.. *PhD Thesis*, University of Tuscia, 2011

2. *“A process-based model to simulate growth in forests with complex structure: Evaluation and use of 3D-CMCC Forest Ecosystem Model in a deciduous forest in Central Italy“*. Collalti A., Perugini L., Santini M., Chiti T., Nolè A., Matteucci G., Valentini R., *Ecological Modelling*, 272, 362– 378, https://doi.org/10.1016/j.ecolmodel.2013.09.016, 2014.

3. *”Validation of 3D-CMCC Forest Ecosystem Model (v.5.1) against eddy covariance data for ten European forest sites”*, A. Collalti, S. Marconi, A. Ibrom, C. Trotta, A. Anav, E. D'Andrea, G. Matteucci, L. Montagnani, B. Gielen, I. Mammarella, T. Grünwald, A. Knohl, F. Berninger, Y. Zhao, R. Valentini, and M. Santini. *Geoscientific Model Development*, 9:479–504, https://doi.org/10.5194/gmd-9-479-2016, 2016.

4.  *“Protocollo di simulazione, calibrazione e validazione del modello 3D-CMCC-CNR FEM: il caso studio del bacino altamente strumentato del Bonis in Calabria”*, A. Collalti,  Biondo C., Buttafuoco G., Maesano M., Caloiero T., Lucà F., Pellicone G., Ricca N., Salvati R., Veltri A., Scarascia Mugnozza. G., Matteucci G.; *Forest@*, 14:247-256, https://doi.org/10.3832/efor2368-014, 2017.

5. *“The role of Respiration in estimation of the net Carbon cycle: coupling soil Carbon dynamics and canopy turnover in a novel version of 3D-CMCC Forest Ecosystem Model”*, Marconi S., Chiti T., Valentini R., Collalti A.. *Forests*, 8:220, https://doi.org/10.3390/f8060220, 2017.

6. *“Climate change mitigation by forests: a case study on the role of management on carbon dynamics of a pine forest in South Italy”* Pellicone G., Scarascia-Mugnozza G., Matteucci G., Collalti. A. *PhD Thesis*, University of Tuscia, 2018.

7. *“Thinning can reduce losses in carbon use efficiency and carbon stocks in managed forests under warmer climate”. A. Collalti , C. Trotta, T. Keenan, A. Ibrom, B. Bond-Lamberty, B. R. Grote, S. Vicca, C.P.O. Reyer, M. Migliavacca, F. Veroustraete, A. Anav, M. Campioli, E. Scoccimarro, E. Grieco, A. Cescatti, G. Matteucci. *Journal of Advances in  Modelling Earth Systems*, 10(10):2427-2452, https://doi.org/10.1029/2018MS001275, 2018.

8. *“The sensitivity of the forest carbon budget shifts between different parameters and processes along stand development and climate change”*, Collalti A., Thornton P. E., Cescatti A., Rita A., Nolè A., Borghetti M., Trotta C., Ciais P., Matteucci G..*Ecological Applications*, 29(2):1-18, https://doi.org/10.1002/eap.1837, 2019.

9. *“Forest carbon allocation modelling under climate change”*, Merganičová K., Merganič J., Lehtonen A., Vacchiano G., Ostrogović Sever M.Z., Augustynczik A.L.D., Grote R., Kyselova I., Mäkelä A., Yousefpour R., Krejza J., Collalti A., Reyer C.P.O.. *Tree Physiology*, 39:1937–1960, https://doi.org/10.1093/treephys/tpz105, 2019.

10. *“Plant respiration: Controlled by photosynthesis or biomass?”*, Collalti A., Tjoelker M.G., Hoch G., Mäkelä A., Guidolotti G., Heskel M., Petit G., Ryan M.G., Battipaglia G., Matteucci G., Prentice I.C.. *Global Change Biology*, 26: 1739–1753, https://doi.org/10.1111/gcb.14857, 2020.

see also at: https://www.forest-modelling-lab.com/publications
