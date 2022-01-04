# MMV

This project was made for the Virtual World Modeling course dispensed by Eric Galin during my second year of Master at Université Lyon 1.

### How to Compile

##### Linux

Once downloaded, install premake4 or 5, go to this project's root folder and use the command ```premake(4/5) --file=./premake4.lua gmake``` to build the makefiles for all the projects, and then run the "make HeightField" to build the project's sources.

##### Windows (for visual studio 2019)

Go to the extern/visual folder and copy the bin folder to gkit2light's root folder.

Then open the powershell, go to your the project's root folder and execute the "./extern/premake5.exe --file=./premake4.lua vs2019".

### How to use

##### Terrain Erosion
Once compiled, you can run the project simply by giving the path to a grayscale image as a command line argument, you'll find some in the data/terrain folder.

While the program is running, you'll see at the top right all the erosion commands, but some command aren't listed due to viewing comfort issues.

Pressing D will save a grayscale image of the terrain slope (you can also directly apply the slope in blue false color by pressing e), pressing F will save a grayscale image of the terrain's draining area using the steepest neighbor method(press r to apply it in green false color), and last but not least, pressing G will save the terrain's draining area using the normalized neighbor dispatch method (press t to apply it directly in green false color).

##### Roads calculations
While the program is running, you can press y to search for a path between two random points, or you can press h to enter those points yourself in the command line.

### What i did

* Generate a terrain based on a Heightmap.
* Implemented stream power erosion.
* Implemented hillslope erosion.
* Implemented debris erosion.
* Implemented Dijkstra's algorithm.
* Implemented a parametrable road generation cost function based on distance between points and slope limitation.

### TODO:

* Make the program more user friendly (Rework road functions and maybe use imgui to create a graphical interface).
* Review code and clean up/optimize if necessary.