# terrainGenerator
A terrain generator made for an advanced graphics assignment using C++ and openGL.

## Note: this project will require Microsoft Visual Studio to build and run.
In case errors occur, or building/running is not desired, sample outputs can be found in .png files in the root directory:
1. capture1.PNG shows a sample output from running the executable with no options.
2. capture2.PNG shows a sample output from running the executable with the following command: terrainGenerator 6 6 513 513 1 2 4 1
3. wireFrameMode.PNG shows a closer view of capture1, but in wireframe mode so that all of the triangles are on display.

## Project Features:
1. Fractal height map generation using the diamond-square algorithm
2. Converting the height map into a 3D terrain with OpenGL using vertex, index, and normal buffers 
3. Some simple vertex and fragment shaders to assign colours depending on height.

## Running/Using the program:
To run the program, simply type “terrainGenerator” into a command prompt within the build directory (x64\Debug in project folder). This runs the program with a default set of input parameters that I have found to consistently look quite good (by this, I mean that it will generally show a solid variety of landscape that works well with the height levels I have implemented). Alternatively, you can add 4 command line arguments that are integers in the following order: x_size, y_size, x_res, y_res. This will allow you to specify the sizing and resolution of the field but default to an initial height map of size 2x2 that I have chosen which generally looks good. If you add an “r” or “random” argument after those sizes and resolutions, the initial height map will instead be randomly generated. Finally, instead of adding the “random” argument you can write 4 more integers in series, which will set the values for an initial 2x2 height map (the corners). Some examples of these, in order from top of this paragraph to bottom are as follows:
1. terrainGenerator
2. terrainGenerator 6 6 513 513
3. terrainGenerator 6 6 513 513 random
4. terrainGenerator 6 6 513 513 1 4 2 1
Please note that resolution inputs must be (2^n) +1 for the algorithm to work properly.

Once the window has opened with the 3D terrain, the asdw keys can be used to move around, the mouse can be used to look around, and the shift/spacebar buttons can be used to go down and up respectively.