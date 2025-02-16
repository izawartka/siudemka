@echo off
echo # Deleting old files...
del /Q siudemka.bmd > nul
del /Q ..\bin\assets\models\siudemka.bmd > nul
del /Q ..\bin\assets\textures\siudemka\* > nul

echo # Converting the model...
py convert-json-sgebmd.py siudemka-model-source\siudemka.json --output siudemka.bmd

echo # Copying new files...
move siudemka.bmd ..\bin\assets\models\siudemka.bmd > nul
copy siudemka-model-source\*.png ..\bin\assets\textures\siudemka\ > nul

echo Done!
pause