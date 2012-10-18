bin_PROGRAMS = 3D_CMCC_Forest_Model

3D_CMCC_Forest_Model_SOURCES = allometry.c crowding-competition.c light.c modifiers.c sort.c biomass_increment.c dataset.c light_dominance.c monthly_partitioning-allocation.c sp_v_monthly_partitioning-allocation.c canopy_cover.c dendrometry.c litterfall.c mortality.c structure.c establishment.c log.c parameters.c treemodel.c canopy_interception.c garbage.c main.c partitioning-allocation.c turnover.c canopy_transpiration.c G-Function.c management.c peak_lai.c utility.c clearcut.c lai.c matrix.c photosynthesis.c vpd.c common.c layer_cover.c met_data.c soil_evaporation.c common.h compiler.h types.h

check_SCRIPTS = test_1.sh

TESTS = $(check_SCRIPTS)

test_1.sh:
	echo "./3D_CMCC_Forest_Model -i ../input -o ../output/output.txt -d ../input/1526_962_input.txt -m ../input/1526_962_2007.txt,../input/1526_962_2008.txt -s ../input/site.txt -r 10" > test_1.sh
	chmod +x test_1.sh

CLEANFILES = $(check_SCRIPTS)
