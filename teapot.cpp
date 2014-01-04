/**
 * Comments:
 * This program was written in Linux.
 */

#define _USE_MATH_DEFINES

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <GL/glut.h>
#include "readppm.h"
#include "vectorfunctions.h"

using namespace std;

/**
 * OpenGL callback functions
 */
void init();
void display();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);

/**
 * Additional helper functions
 */
void parseFile();
void createNormals();
unsigned char * flipImage(unsigned char * image, int width, int height);

/**
 * Global constants
 */
const int COORDS = 3;
int textureWidth = 1024;
int textureHeight = 1024;
int environmentWidth = 256;
int environmentHeight = 256;

/**
 * Global variables
 */
vector< vector< double > > vertices;
vector< vector< double > > faces;
vector< vector< double > > vertexNormals;
unsigned char * texture;
unsigned char * environment;
bool useTexture = true;
bool useVertices = true;
double zpos = 0;
double xangle = 0;
double yangle = 0;

/**
 * The start point of the program
 * @param argc
 * 	The number of arguments passed to the program
 * @param argv
 * 	The list of arguments
 * @return
 * 	0
 */
int main(int argc, char ** argv)
{
	// Initialize an OpenGL window
	glutInit(&argc, (char**) argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return 0;
}

/**
 * Initializes OpenGL, the teapot file, the texture file, and the environment map file
 */
void init()
{
	// Read in the teapot
	parseFile();
	createNormals();

	// Read in the texture file
	texture = readPPM("texture.ppm", &textureWidth, &textureHeight);
	texture = flipImage(texture, textureWidth, textureHeight);

	// Read in the environment file
	environment = readPPM("environment.ppm", &environmentWidth, &environmentHeight);
	environment = flipImage(environment, environmentWidth, environmentHeight);

	// Initialize Perspective Projection in OpenGL
	glClearColor(0.5, 0.5, 1.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(70.f, 1.0, 0.001f, 30.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Create lighting and shadows
	GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat lpos[] = {10.0, 10.0, 10.0, 0.0};
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	
	// Enable the Z-buffer to remove hidden surfaces
	glEnable(GL_DEPTH_TEST);
}

/**
 * Display callback function to be used with OpenGL
 */
void display()
{
	if (useTexture)
	{
		// Create the texture map
		GLuint textureImage;
		glGenTextures(1, &textureImage);
		glBindTexture(GL_TEXTURE_2D, textureImage);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glEnable(GL_TEXTURE_2D);
	}
	else
	{
		// Create the environment map
		GLuint environmentImage;
		glGenTextures(1, &environmentImage);
		glBindTexture(GL_TEXTURE_2D, environmentImage);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, environmentWidth, environmentHeight, GL_RGB, GL_UNSIGNED_BYTE, environment);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glEnable(GL_TEXTURE_2D);
	}

	// Create the lookat vector
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.f, 2.f, 5.f, 0.f, 1.f, 0.f, 0.f, 1.f, 0.f);

	if (useTexture)
	{
		// Load as a texture that rotates with the object
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	}
	else
	{
		// Load as an environment that remains static when the object rotates
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	}

	// Create texture coordinates
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);

	// Apply matrix multiplication for any movement done by the user
	glTranslated(0.0, 0.0, zpos);
	glRotated(xangle, 1.0, 0.0, 0.0);
	glRotated(yangle, 0.0, 1.0, 0.0);

	glColor3d(1.0, 1.0, 1.0);
	if (useVertices)
	{
		glBegin(GL_TRIANGLES);
		for (int i = 0; i < (int) faces.size(); ++i)
		{
			// Select the vertices that make up the face
			vector< double > vertex1 = vertices[(int) faces[i][0]];
			vector< double > normal1 = vertexNormals[(int) faces[i][0]];
			vector< double > vertex2 = vertices[(int) faces[i][1]];
			vector< double > normal2 = vertexNormals[(int) faces[i][1]];
			vector< double > vertex3 = vertices[(int) faces[i][2]];
			vector< double > normal3 = vertexNormals[(int) faces[i][2]];

			// Draw the vertices
			glNormal3d(normal1[0], normal1[1], normal1[2]);
			glVertex3d(vertex1[0], vertex1[1], vertex1[2]);
			glNormal3d(normal2[0], normal2[1], normal2[2]);
			glVertex3d(vertex2[0], vertex2[1], vertex2[2]);
			glNormal3d(normal3[0], normal3[1], normal3[2]);
			glVertex3d(vertex3[0], vertex3[1], vertex3[2]);
		}
		glEnd();
	}
	else
	{
		// Create the default glut teapot
		glutSolidTeapot(1.f);
	}

	// Disable Texture Coordinates
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);

	// Enable double buffering
	glutSwapBuffers();
	glFlush();
	glutPostRedisplay();
}

/**
 * Reshap callback function used by OpenGL
 * @param w
 * 	The width of the viewport
 * @param h
 * 	The height of the viewport
 */
void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	float fAspect = ((float)w)/h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.f, fAspect, 0.001f, 30.f);
}

/**
 * keyboard callback function used by OpenGL
 * @param key
 * 	The pressed key
 * @param x
 * 	The x coordinate of the mouse
 * @param y
 * 	The y coordinate of the mouse
 */
void keyboard(unsigned char key, int x, int y)
{
	// If Esc is pressed, exit
	if (key == 27)
	{
		exit(0);
	}

	// If w is pressed, rotate clockwise about the x-axis
	if (key == 'w' || key == 'W')
	{
		xangle -= 2;
	}

	// If a is pressed, rotate clockwise about the y-axis
	if (key == 'a' || key == 'A')
	{
		yangle -= 2;
	}

	// If s is pressed, rotate counter-clockwise about the x-axis
	if (key == 's' || key == 'S')
	{
		xangle += 2;
	}

	// If d is pressed, rotate counter-clockwise about the y-axis
	if (key == 'd' || key == 'D')
	{
		yangle += 2;
	}

	// If z is pressed, move towards the teapot
	if (key == 'z' || key == 'Z')
	{
		zpos += .5;
	}

	// If x is pressed, move away from the teapot
	if (key == 'x' || key == 'X')
	{
		zpos -= .5;
	}

	// If f is pressed, swap the texture map and the environment map
	if (key == 'f' || key == 'F')
	{
		useTexture = !useTexture;
	}

	// If v is pressed, swap the loaded teapot with the default glut teapot
	if (key == 'v' || key == 'V')
	{
		useVertices = !useVertices;
	}
}

/**
 * Reads in the teapot.obj file and initializes the vertices and faces arrays
 */
void parseFile()
{
	string filename = "teapot.obj";
	ifstream fin(filename.c_str());
	string line = "";	

	// Open the file
	if (fin.is_open())
	{
		while (fin.good())
		{
			getline(fin, line);
			
			istringstream input(line, istringstream::in);

			string type;
			input >> type;

			// Read in a vertex
			if (type == "v")
			{
				vector< double > vertex;
				for (int i = 0; i < 3; ++i)
				{
					double value;
					input >> value;
					vertex.push_back(value);
				}
				vertices.push_back(vertex);
			}
			// Read in a face
			else if (type == "f")
			{
				vector< double > face;
				for (int i = 0; i < 3; ++i)
				{
					double value;
					input >> value;
					face.push_back(value - 1);
				}
				faces.push_back(face);
			}
		}
	}
	else
	{
		cout << "Error: unable to open " << filename << "." << endl;
	}

	// Close the file
	fin.close();
}

/**
 * creates face normals for each face, and then uses them to create vertex normals for each vertex
 */
void createNormals()
{
	// Initialize the vertex normal array
	for (int i = 0; i < (int) vertices.size(); ++i)
	{
		vector< double > temp;
		for (int j = 0; j < COORDS; ++j)
		{
			temp.push_back(0.0);
		}
		vertexNormals.push_back(temp);
	}

	for (int i = 0; i < (int) faces.size(); ++i)
	{
		// Initialize the faceNormal vector
		vector< double > faceNormal;
		for (int j = 0; j < COORDS; ++j)
		{
			faceNormal.push_back(0);
		}

		// Find the face normal
		vector< double > a;
		vector< double > b;
		a = subtract(vertices[(int) faces[i][1]], vertices[(int) faces[i][0]]);
		b = subtract(vertices[(int) faces[i][2]], vertices[(int) faces[i][0]]);
		faceNormal = crossProduct(a, b);

		// Increase the vertex normal of each vertex
		// in the face by the face normal
		for (int j = 0; j < COORDS; ++j)
		{
			vector< double > vertexNormal = vertexNormals[(int) faces[i][j]];
			vector< double > newNormalA = add(vertexNormal, faceNormal);
			vector< double > newNormalB = subtract(vertexNormal, faceNormal);

			if (length(newNormalA) >= length(newNormalB))
			{
				vertexNormals[(int) faces[i][j]] = newNormalA;
			}
			else
			{
				vertexNormals[(int) faces[i][j]] = newNormalB;
			}
		}
	}

	// Normalize the vertex normals
	for (int i = 0; i < (int) vertexNormals.size(); ++i)
	{
		vertexNormals[i] = normalize(vertexNormals[i]);
	}
}

/**
 * Flips an image vertically. This must be done because image files have (0, 0) as the top-left point, and OpenGL has (0, 0) as the bottom-left point.
 * @param image
 * 	The image to be flipped
 * @param width
 * 	The width of the image
 * @param height
 * 	The height of the image
 * @return
 * 	The flipped image
 */
unsigned char * flipImage(unsigned char * image, int width, int height)
{
	const int RGB = 3;
	unsigned char imageArray[width][height][RGB];

	// Convert the 1-D input array into a 3-D array
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			for (int k = 0; k < RGB; ++k)
			{
				imageArray[i][j][k] = image[i*RGB + j*width*RGB + k];
			}
		}
	}

	// Flip the image
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height/2; ++j)
		{
			for (int k = 0; k < RGB; ++k)
			{
				unsigned char temp = imageArray[i][j][k];
				imageArray[i][j][k] = imageArray[i][height - j - 1][k];
				imageArray[i][height - j - 1][k] = temp;
			}
		}
	}

	// Convert the 3-D array into a 1-D array
	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			for (int k = 0; k < RGB; ++k)
			{
				image[i*RGB + j*width*RGB + k] = imageArray[i][j][k];
			}
		}
	}

	return image;
}
