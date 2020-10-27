@echo off
3D-CMCC-Forest-Model.exe -i parameterization -o output/debug_output -b output/daily_output -f output/monthly_output -e output/annual_output -d DKSoroe/6_inputs.txt -m DKSoroe/6_climate.lst -s DKSoroe/Soroe_site.txt -c DKSoroe/Soroe_settings.txt -n output/soil_output/soil_output > results.txt
pause
