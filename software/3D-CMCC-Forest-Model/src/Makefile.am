bin_PROGRAMS = 3D_CMCC_Forest_Model

3D_CMCC_Forest_Model_SOURCES = \
	allometry.c\
	biomass.c\
	canopy_cover.c\
	canopy_evapotranspiration.c\
	canopy_transpiration.c\
	clearcut.c\
	common.c\
	common.h\
	compiler.h\
	crowding-competition.c\
	dataset.c\
	dendrometry.c\
	establishment.c\
	garbage.c\
	G-Function.c\
	initialization_biomass_data.c\
	initialization_site_data.c\
	lai.c\
	layer_cover.c\
	light.c\
	litterfall.c\
	log.c\
	main.c\
	management.c\
	matrix.c\
	met_data.c\
	modifiers.c\
	monthly_partitioning-allocation.c\
	mortality.c\
	parameters.c\
	partitioning-allocation.c\
	peak_lai.c\
	photosynthesis.c\
	renovation.c\
	soil_evaporation.c\
	soil_water_balance.c\
	sort.c\
	structure.c\
	treemodel.c\
	turnover.c\
	types.h\
	utility.c\
	vpd.c\
	wue.c

check_SCRIPTS = test_1.sh

TESTS = $(check_SCRIPTS)

test_1.sh:
	echo "./3D_CMCC_Forest_Model -i ../input -o ../output/output.txt -d ../input/1526_962_input.txt -m ../input/1526_962_2007.txt,../input/1526_962_2008.txt -s ../input/site.txt -c ../input/settings.txt" > test_1.sh
	chmod +x test_1.sh

CLEANFILES = $(check_SCRIPTS)
