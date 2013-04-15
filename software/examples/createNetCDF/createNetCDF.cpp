#include "gdal_priv.h"
#include "cpl_string.h"

int main (int argc, char *argv[])
{
    int sizeX  = 512,
        sizeY  = 512,
        nBands = 1,
        i;

    const char *pszFormat = "netCDF";

    GDALDataset *destDataset = NULL;
    GDALRasterBand *destBand;
    GDALDriver *poDriver;
    GDALAllRegister();

    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);

    destDataset = poDriver->Create("netcdf_example.nc", sizeX, sizeY, nBands, GDT_Float64, NULL);
    destBand  = destDataset->GetRasterBand(1);

    double* raster = (double*)malloc(sizeof(double)*sizeX*sizeY);

    for(i=0; i<sizeX*sizeY; i++)
        raster[i] = 0;
    
    destBand->RasterIO(GF_Write, 0, 0, sizeX, sizeY, raster, sizeX, sizeY, GDT_Float64, 0, 0);

    GDALClose((GDALDatasetH) destDataset);

    return EXIT_SUCCESS;
}
