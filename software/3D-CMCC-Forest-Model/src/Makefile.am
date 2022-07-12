bin_PROGRAMS = 3D_CMCC_Forest_Model

3D_CMCC_Forest_Model_SOURCES = \
        allometry.c\
        CN-allocation.c\
        litter_nitrogen_balance.c\
        print_output.c\
        CN-balance.c\
        logger.c\
        regeneration.c\
        aut_respiration.c\
        common.c\
        main.c\
        remove_tree_class.c\
        biomass.c\
        settings.c\
                    compare.c\
                    snow.c\
cue.c\
soil_carbon_balance.c\
canopy_cover.c\
decomposition.c\
management.c\
soil_evaporation.c\
               dendrometry.c\
               matrix.c\
               soil_model.c\
canopy_evapotranspiration.c\
establishment.c\
met_data.c\
soil_model_check_balance.c\
  evapotranspiration.c\
  meteo.c\
  soil_nitrogen_balance.c\
canopy_interception.c\
fluxes.c\
modifiers.c\
soil_radiation_sw_band.c\
canopy_net_radiation.c\
g-function.c\
mortality.c\
soil_respiration.c\
canopy_radiation_lw_band.c\
soil_settings.c\
canopy_radiation_sw_band.c\
heat_fluxes.c\
new_forest_tree_class.c\
soil_water_balance.c\
canopy_sensible_heat.c\
het_respiration.c\
output.c\
nc.c\
structure.c\
canopy_temperature.c\
initialization.c\
peak_lai.c\
topo.c\
C-assimilation.c\
lai.c\
Penman_Monteith.c\
tree_model.c\
C-deciduous-partitioning.c\
leaf_fall.c\
phenology.c\
tree_model_check_balance.c\
cell_model.c\
litter_carbon_balance.c\
photosynthesis2.c\
turnover.c\
cell_model_check_balance.c\
littering.c\
photosynthesis3.c\
utility.c\
C-evergreen-partitioning.c\
litter_model.c\
photosynthesis.c\
water_balance.c\
C-fruit-partitioning.c\
litter_model_check_balance.c\
print.c\
wue.c



check_SCRIPTS = test_1.sh

TESTS = $(check_SCRIPTS)

test_1.sh:
	echo "./3D_CMCC_Forest_Model -i ../input -o ../output/output.txt -d ../input/1526_962_input.txt -m ../input/1526_962_2007.txt,../input/1526_962_2008.txt -s ../input/site.txt -c ../input/settings.txt" > test_1.sh
	chmod +x test_1.sh

CLEANFILES = $(check_SCRIPTS)
