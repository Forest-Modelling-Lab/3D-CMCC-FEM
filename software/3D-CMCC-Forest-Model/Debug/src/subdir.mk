################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/C-assimilation.c \
../src/C-fluxes.c \
../src/C-partitioning-allocation.c \
../src/G-Function.c \
../src/N-stock.c \
../src/allometry.c \
../src/aut_respiration.c \
../src/biomass.c \
../src/canopy_cover.c \
../src/canopy_evapotranspiration.c \
../src/canopy_interception.c \
../src/canopy_transpiration.c \
../src/common.c \
../src/cropmodel.c \
../src/cropmodel_daily.c \
../src/crowding-competition.c \
../src/dataset.c \
../src/dendrometry.c \
../src/establishment.c \
../src/evapotranspiration.c \
../src/garbage.c \
../src/initialization_biomass_data.c \
../src/initialization_site_data.c \
../src/io.c \
../src/lai.c \
../src/light.c \
../src/litterfall.c \
../src/log.c \
../src/main.c \
../src/management.c \
../src/matrix.c \
../src/met_data.c \
../src/modifiers.c \
../src/mortality.c \
../src/parameters.c \
../src/peak_lai.c \
../src/phenology.c \
../src/photosynthesis.c \
../src/renovation.c \
../src/soil_evaporation.c \
../src/soil_respiration.c \
../src/soil_water_balance.c \
../src/soilmodel.c \
../src/structure.c \
../src/treemodel.c \
../src/treemodel_daily.c \
../src/turnover.c \
../src/utility.c \
../src/wue.c 

OBJS += \
./src/C-assimilation.o \
./src/C-fluxes.o \
./src/C-partitioning-allocation.o \
./src/G-Function.o \
./src/N-stock.o \
./src/allometry.o \
./src/aut_respiration.o \
./src/biomass.o \
./src/canopy_cover.o \
./src/canopy_evapotranspiration.o \
./src/canopy_interception.o \
./src/canopy_transpiration.o \
./src/common.o \
./src/cropmodel.o \
./src/cropmodel_daily.o \
./src/crowding-competition.o \
./src/dataset.o \
./src/dendrometry.o \
./src/establishment.o \
./src/evapotranspiration.o \
./src/garbage.o \
./src/initialization_biomass_data.o \
./src/initialization_site_data.o \
./src/io.o \
./src/lai.o \
./src/light.o \
./src/litterfall.o \
./src/log.o \
./src/main.o \
./src/management.o \
./src/matrix.o \
./src/met_data.o \
./src/modifiers.o \
./src/mortality.o \
./src/parameters.o \
./src/peak_lai.o \
./src/phenology.o \
./src/photosynthesis.o \
./src/renovation.o \
./src/soil_evaporation.o \
./src/soil_respiration.o \
./src/soil_water_balance.o \
./src/soilmodel.o \
./src/structure.o \
./src/treemodel.o \
./src/treemodel_daily.o \
./src/turnover.o \
./src/utility.o \
./src/wue.o 

C_DEPS += \
./src/C-assimilation.d \
./src/C-fluxes.d \
./src/C-partitioning-allocation.d \
./src/G-Function.d \
./src/N-stock.d \
./src/allometry.d \
./src/aut_respiration.d \
./src/biomass.d \
./src/canopy_cover.d \
./src/canopy_evapotranspiration.d \
./src/canopy_interception.d \
./src/canopy_transpiration.d \
./src/common.d \
./src/cropmodel.d \
./src/cropmodel_daily.d \
./src/crowding-competition.d \
./src/dataset.d \
./src/dendrometry.d \
./src/establishment.d \
./src/evapotranspiration.d \
./src/garbage.d \
./src/initialization_biomass_data.d \
./src/initialization_site_data.d \
./src/io.d \
./src/lai.d \
./src/light.d \
./src/litterfall.d \
./src/log.d \
./src/main.d \
./src/management.d \
./src/matrix.d \
./src/met_data.d \
./src/modifiers.d \
./src/mortality.d \
./src/parameters.d \
./src/peak_lai.d \
./src/phenology.d \
./src/photosynthesis.d \
./src/renovation.d \
./src/soil_evaporation.d \
./src/soil_respiration.d \
./src/soil_water_balance.d \
./src/soilmodel.d \
./src/structure.d \
./src/treemodel.d \
./src/treemodel_daily.d \
./src/turnover.d \
./src/utility.d \
./src/wue.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


