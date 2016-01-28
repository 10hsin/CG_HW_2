#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "glew.h"
#include "glut.h"
using namespace std;

class light
{
	public:
		// variable
		float light_position[4];
		float light_ambient[4];
		float light_diffuse[4];
		float light_specular[4];
		GLenum	gl_light[8];
		float ambient[3];
		
		int testscene = 3;

		// function
		light();
		light(int);
		~light();
		void load_light(int test_scene);
		void load_ambient();
		void load_normal();
};