# siudemka
## Sample project for the RZUF3 framework

This app displays a prerendered locomotive model loaded from a tilemap set using the SGE (Siudemka Graphics Engine) module. You can interact with the model using some of the built-in RZUF functionalities. Just download and try it out yourself.

### RZUF3
You can find the framework repository [here](https://github.com/izawartka/rzuf3).

### Binaries
The latest binaries are available in the Releases section on the right. Just download the zip file appropriate for your system (Windows x64 or Linux x64), extract it, and run `siudemka.exe` (or `siudemka` if you are on Linux).

### Keybindings
`LEFT SHIFT` - toggle between normal view and model-only view

### Dependencies
RZUF3 requires the following libraries: `SDL2`, `SDL2_image`, `SDL2_ttf`, `spdlog`. For installation on Windows, it's best to use vcpkg. If CMake has trouble finding them, ensure the appropriate path is set in the second line of `CMakeLists.txt`.

### Author
masuo / izawartka
