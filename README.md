# trees-lib
An implementation of mathematical morphology hierarchies (current: min/max, ToS, alpha, omega) and some processing algorithms (filtering, MSER...)

# developing in CodeBlocks
The project was developed using CodeBlocks. To open the code in CodeBlocks, simply open Trees.cbp to start.

# compiling using make
As the project was developed in CodeBlocks, the makefile was not written by hand for the project. It was generated using 'cbp2make' tool, which translates a CodeBlocks project into a makefile.
https://sourceforge.net/projects/cbp2make/
As the makefile might not be updated as regularly as the rest of the project, in case of any problems please download cbp2make and generate a new makefile.

# documentation
Documenting the code is on-going (about 70% of the code is documented - most of the classes, and about half the methods). Doxygen documentation with the project.

# main.cpp
Currently the main file is a complete mess and contains bits of code under developement, or procedures to test certain concepts. Any concepts that will finally be kept will be eventually re-implemented within the correct structure.

# other requirements
The project compiles following C++11 standard (the CodeBlocks should correctly handle this). It also relies on OpenCV for providing basic structures (e.g. Point, Mat (matrix) etc)
