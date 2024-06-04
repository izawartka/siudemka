# siudemka
## Sample project for the RZUF3 framework

This app displays a prerendered locomotive model loaded from a tilemap set using the SGE (Siudemka Graphics Engine) module. You can interact with the model using some of the built-in RZUF functionalities. Just download and try it out yourself.

### RZUF3
You can find the framework repository [here](https://github.com/izawartka/rzuf3).

### Binaries
The latest binaries (currently only for Windows) are available in the Releases section on the right. Just download the zip file, extract it, and run `siudemka.exe`.

### Dependencies
RZUF3 requires the following libraries: `SDL2`, `SDL2_image`, `SDL2_ttf`, `spdlog`, `yaml-cpp`. For installation on Windows, it's best to use vcpkg. If CMake has trouble finding them, ensure the appropriate path is set in the second line of `CMakeLists.txt`.

### Author
masuo / izawartka
