#include "scene.h"
#include <iostream>
#include <fstream>
#include <string>

scene::scene()
{
	obj_num = 0;
}

scene::scene(int TestScene)
{
	obj_num = 0;
	load_scene(TestScene);
}

scene::~scene()
{
	
}

void scene::load_scene(int test_scene)
{
	ifstream ifs;
	string	token;

	if (test_scene == 1)
		ifs.open("Scene\\TestScene1\\Scene1.scene");
	else if (test_scene == 2)
		ifs.open("Scene\\TestScene2\\Scene2.scene");
	else if (test_scene == 3)
		ifs.open("Scene\\TestScene3\\Scene3.scene");
	else
		cout << "File not found";


	while (ifs >> token)
	{
		if (token != "model")
			break;

		ifs >> token;

		object[obj_num] = new mesh(token.c_str());

		ifs >> scale_value[obj_num].x >> scale_value[obj_num].y >> scale_value[obj_num].z;
		ifs >> angle[obj_num] >> rotation_axis_vector[obj_num].x >> rotation_axis_vector[obj_num].y >> rotation_axis_vector[obj_num].z;
		ifs >> transfer_vector[obj_num].x >> transfer_vector[obj_num].y >> transfer_vector[obj_num].z;

		obj_num++;
	}

	ifs.close();
	
}
