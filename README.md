# HeightMaps, Erosion and Roads

This project was made for the Virtual World Modeling course dispensed by Eric Galin during my second year of Master at Université Lyon 1.

![Pres terrain](https://user-images.githubusercontent.com/50468590/148096732-1aa787f8-87f5-4ad0-936e-fe45f8453ffe.png)

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

![terrain generation](https://user-images.githubusercontent.com/50468590/148097611-f7709717-2479-466c-a37b-d071fc02089e.png)

![terrain base](https://user-images.githubusercontent.com/50468590/148097667-4aef8f27-fc91-4afb-947b-0119b190c21e.png)

* Implemented stream power erosion.

![terrain stream](https://user-images.githubusercontent.com/50468590/148096891-bdcf2a5f-2e22-42c0-85fe-54e9f94fff57.png)
* Implemented hillslope erosion.

![Terrain hillslope](https://user-images.githubusercontent.com/50468590/148097712-bbdce838-ac02-42b2-afd1-ac769a25c821.png)

* Implemented debris erosion.

![terrain debris](https://user-images.githubusercontent.com/50468590/148097722-e39d7985-3e2c-4dca-a367-f679e56924f9.png)

* Implemented Dijkstra's algorithm.
* Implemented a parametrable road generation cost function based on distance between points and slope limitation.
  * Minizing distance:
  
![Route distance](https://user-images.githubusercontent.com/50468590/148096997-8f211dd0-9cf0-4ac3-9791-787f70a761de.png)
  * Minimizing distance while avoiding steep slopes:

![Route pente](https://user-images.githubusercontent.com/50468590/148097739-a67ea824-a61a-48f2-8cc5-ca4f9d79dac6.png)


### TODO:

* Make the program more user friendly (Rework road functions and maybe use imgui to create a graphical interface).
* Review code and clean up/optimize if necessary.
