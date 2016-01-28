#include "light.h"
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

light::light()
{

}
light::light(int TestScene)
{
	gl_light[0] = GL_LIGHT0;
	gl_light[1] = GL_LIGHT1;
	gl_light[2] = GL_LIGHT2;
	gl_light[3] = GL_LIGHT3;
	gl_light[4] = GL_LIGHT4;
	gl_light[5] = GL_LIGHT5;
	gl_light[6] = GL_LIGHT6;
	gl_light[7] = GL_LIGHT7;
	load_light(TestScene);
}


light::~light()
{
	
}

void light::load_light(int test_scene)
{
	ifstream ifs;
	string	token;

	if (test_scene == 1)
		ifs.open("Scene\\TestScene1\\Scene1.light");
	else if (test_scene == 2)
		ifs.open("Scene\\TestScene2\\Scene2.light");
	else if (test_scene == 3)
		ifs.open("Scene\\TestScene3\\Scene3.light");
	else
		cout << "File not found";
	
	int i = 0;
	float temp_x, temp_y, temp_z;
	
	while (ifs >> token && token == "light" && i<8)
	{
		ifs >> temp_x >> temp_y >> temp_z;
		light_position[0] = temp_x;
		light_position[1] = temp_y;
		light_position[2] = temp_z;
		light_position[3] = 1.0f;
		
		ifs >> temp_x >> temp_y >> temp_z;
		light_ambient[0] = temp_x;
		light_ambient[1] = temp_y;
		light_ambient[2] = temp_z;
		light_ambient[3] = 1.0f;
		
		ifs >> temp_x >> temp_y >> temp_z;
		light_diffuse[0] = temp_x;
		light_diffuse[1] = temp_y;
		light_diffuse[2] = temp_z;
		light_diffuse[3] = 1.0f;
		
		ifs >> temp_x >> temp_y >> temp_z;
		light_specular[0] = temp_x;
		light_specular[1] = temp_y;
		light_specular[2] = temp_z;
		light_specular[3] = 1.0f;
	}
	
	ifs >> temp_x >> temp_y >> temp_z;
	ambient[0] = temp_x;
	ambient[1] = temp_y;
	ambient[2] = temp_z;
};


void light::load_ambient()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);	
	glEnable(GL_LIGHTING);

	float a[] = { 0, 0, 0, 0 };
	float b[] = { 0, 0, 0, 0 };

	glEnable(gl_light[0]);
	glLightfv(gl_light[0], GL_POSITION, light_position);
	glLightfv(gl_light[0], GL_DIFFUSE,	a);
	glLightfv(gl_light[0], GL_SPECULAR, b);
	glLightfv(gl_light[0], GL_AMBIENT,	light_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
}

void light::load_normal()
{
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);	
	glEnable(GL_LIGHTING);

	glEnable(gl_light[0]);
	glLightfv(gl_light[0], GL_POSITION, light_position);
	glLightfv(gl_light[0], GL_DIFFUSE,	light_diffuse);
	glLightfv(gl_light[0], GL_SPECULAR, light_specular);
	glLightfv(gl_light[0], GL_AMBIENT,	light_ambient);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
}