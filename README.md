To compile, type:
make all

To run, type:
./roth15_mp3

This program was created to complete MP3 of CS 418 at UIUC for 3 credit hours. It will read in an input file of a teapot containing only vertex coordinates and faces.  It will then computer the face normals and vertex normals of the teapot. It then creates lighting and shadows on the teapot. It will then load a texture onto the teapot, and can be switched to instead load an environment map on the teapot. This program was created in Linux (specifically Fedora) and is not guaranteed to run on Windows or Mac OSX.

Controls:
W - Rotate the teapot clockwise around the X-axis
S - Rotate the teapot counter-clockwise around the X-axis
A - Rotate the teapot clockwise around the Y-axis
D - Rotate the teapot counter-clockwise around the Y-axis
Z - Move closer to the teapot
X - Move away from the teapot
F - Switch between texture mapping and environment mapping
V - Switch between the teapot loaded from the file, and the default glut teapot
Esc - exits the program

Bugs:
Currently, lighting is loaded properly on start, but when the user switches to environment mapping the lighting fails and does not reload when the user switches back to texture mapping.
