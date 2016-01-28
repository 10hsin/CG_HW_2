//*************************************************
// Library & Header 
//*************************************************
#include "mesh.h"
#include "glew.h"
#include "glut.h"
#include "scene.h"
#include "view.h"
#include "light.h"
#include <fstream>
#include <iostream>
#include <string>
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdio>
#include "Vector3D.h"
using namespace std;
using namespace MathTool;


//*************************************************
// Function Declaration & Global Variable
//*************************************************
#define TestScene 3

void	display();
void	keyboard(unsigned char, int, int);
void	mouse(int, int, int, int);
void	reshape(GLsizei, GLsizei);
bool	is_front(Vector3D obj_vec[3]);
void	draw_shadow_volume();
void	draw_scene();

int		obj_id = 0;
int		x_pos = -1, y_pos = -1;
int		windowSize[2];
float	flag = 0;

scene	s(TestScene);
view	v(TestScene);
light	l(TestScene);


//*************************************************
// Function Implementation
//*************************************************

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(v.viewport_width, v.viewport_height);
	glutInitWindowPosition(v.viewport_x, v.viewport_y);
	glutInitDisplayMode(GL_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ACCUM);
	glutCreateWindow("HW2");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMainLoop();

	return 0;
}


//*************************************************
// vertex -> ModelView Matrix -> Projection Matrix 
// -> Perspective Division -> vView Transformation
//*************************************************

void display()
{
	//setup
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_ACCUM_BUFFER_BIT);
	glClearDepth(1.0f);
	glClearAccum(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// viewport transformation
	glViewport(v.viewport_x, v.viewport_y, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(v.fovy, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], v.dnear, v.dfar);

	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(v.eye.x, v.eye.y, v.eye.z,	// eye
		v.vat.x, v.vat.y, v.vat.z,  // center
		v.vup.x, v.vup.y, v.vup.z); // up

	// four direction of camera
	Vector3D temp;
	temp.Set(v.vup.x, v.vup.y, v.vup.z);
	temp.Normalize();
	float dir_1[] = { temp.x / 4, temp.y / 4, temp.z / 4 };
	float dir_2[] = { -temp.x / 4, -temp.y / 4, -temp.z / 4 };

	Vector3D a, b;
	a.Set(v.eye.x - v.vat.x, v.eye.y - v.vat.y, v.eye.z - v.vat.z);
	b.Set(v.eye.x - v.vup.x, v.eye.y - v.vup.y, v.eye.z - v.vup.z);

	temp = a.UnitCross(b);
	float dir_3[] = { temp.x / 4, temp.y / 4, temp.z / 4 };
	float dir_4[] = { -temp.x / 4, -temp.y / 4, -temp.z / 4 };
	

	Vector3D focus (v.eye.x - v.vat.x, v.eye.y - v.vat.y, v.eye.z - v.vat.z);
	focus.Normalize();

	for (int i = 0; i < 17; i++)
	{
		//************** CLEAR BUFFER **************//
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glLoadIdentity();

		//************** MOVE CAMERA **************//
		if (i % 4 == 0)
			gluLookAt(	v.eye.x + (i / 4)*dir_1[0],		v.eye.y + (i / 4)*dir_1[1],		v.eye.z + (i / 4)*dir_1[2],
						v.vat.x + flag*focus.x,			v.vat.y + flag*focus.y,			v.vat.z + flag*focus.z,
						v.vup.x,						v.vup.y,						v.vup.z);

		if (i % 4 == 1)
			gluLookAt(	v.eye.x + (i/4 + 1)*dir_2[0],	v.eye.y + (i/4 + 1)*dir_2[1],	v.eye.z + (i/4 + 1)*dir_2[2],
						v.vat.x + flag*focus.x,			v.vat.y + flag*focus.y,			v.vat.z + flag*focus.z,
						v.vup.x,						v.vup.y,						v.vup.z);

		if (i % 4 == 2)
			gluLookAt(	v.eye.x + (i/4 + 1)*dir_3[0],	v.eye.y + (i/4 + 1)*dir_3[1],	v.eye.z + (i/4 + 1)*dir_3[2],
						v.vat.x + flag*focus.x,			v.vat.y + flag*focus.y,			v.vat.z + flag*focus.z,
						v.vup.x,						v.vup.y,						v.vup.z);

		if (i % 4 == 3)
			gluLookAt(	v.eye.x + (i/4 + 1)*dir_4[0],	v.eye.y + (i/4 + 1)*dir_4[1],	v.eye.z + (i/4 + 1)*dir_4[2],
						v.vat.x + flag*focus.x,			v.vat.y + flag*focus.y,			v.vat.z + flag*focus.z,
						v.vup.x,						v.vup.y,						v.vup.z); 


		//************** DRAW SHADOW **************//
		// pass 1 : draw scene tith ambient only
		l.load_ambient();
		draw_scene();

		// mask color and depth
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glDepthMask(GL_FALSE);
		glClearStencil(0);
		glEnable(GL_CULL_FACE);
		glEnable(GL_STENCIL_TEST);

		// pass 2 : back-face culling
		glStencilFunc(GL_ALWAYS, 0, ~0);
		glCullFace(GL_BACK);
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		draw_shadow_volume();

		// pass 3 : front-face culling
		glStencilFunc(GL_ALWAYS, 0, ~0);
		glCullFace(GL_FRONT);
		glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
		draw_shadow_volume();

		// pass 4 : draw scene
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glDepthMask(GL_TRUE);
		glDisable(GL_CULL_FACE);
		glStencilFunc(GL_EQUAL, 0, ~0);

		l.load_normal();
		draw_scene();

		//************** ACCU BUFFER **************//
		glAccum(GL_ACCUM, 1 / (float)17);
	}
	glAccum(GL_RETURN, 1);
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

//*************************************************
// rotate matrix along y
// cos 0 -sin 0
// 0   1   0  0
// sin 0   cos 0
// 0   0   0   1
//*************************************************

void keyboard(unsigned char key, int x, int y)
{
	float x_rotate, z_rotate;
	switch (key)
	{
	case 'w':
		v.fovy += 1;
		break;

	case 's':
		v.fovy -= 1;
		break;

	case 'd':
		x_rotate = v.eye.x - v.vat.x;
		z_rotate = v.eye.z - v.vat.z;
		v.eye.x = x_rotate*cos(-0.5) + z_rotate*sin(-0.5);
		v.eye.z = z_rotate*cos(-0.5) - x_rotate*sin(-0.5);
		break;

	case 'a':
		x_rotate = v.eye.x - v.vat.x;
		z_rotate = v.eye.z - v.vat.z;
		v.eye.x = x_rotate*cos(0.5) + z_rotate*sin(0.5);
		v.eye.z = z_rotate*cos(0.5) - x_rotate*sin(0.5);
		break;

	case '1':
		l.light_position[0] += 1;
		break;

	case '2':
		l.light_position[0] -= 1;
		break;

	case '3':
		l.light_position[1] += 1;
		break;

	case '4':
		l.light_position[1] -= 1;
		break;

	case '5':
		l.light_position[2] += 1;
		break;

	case '6':
		l.light_position[2] -= 1;
		break;

	case 'q':
		flag += 3;
		break;

	case 'e':
		flag -= 3;
		break;

	default:
		obj_id = 0;
		flag = 0;
		break;
	}

	glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && obj_id != 0)
	{
		if (state == GLUT_DOWN)
		{
			x_pos = x;
			y_pos = y;
		}
		else if (state == GLUT_UP)
		{
			l.light_position[0] += (x - x_pos) / 50;
			l.light_position[1] -= (y - y_pos) / 50;
			glutPostRedisplay();
		}
	}
}

bool is_front(Vector3D obj_vec[3])
{
	Vector3D light_vector, a, b, cross_result;

	a = obj_vec[0] - obj_vec[1];
	b = obj_vec[1] - obj_vec[2];
	cross_result = a.Cross(b);

	light_vector.x = l.light_position[0] - obj_vec[0].x;
	light_vector.y = l.light_position[1] - obj_vec[0].y;
	light_vector.z = l.light_position[2] - obj_vec[0].z;

	return (cross_result.Dot(light_vector) >= 0) ? true : false;
}

void draw_shadow_volume()
{
	for (int k = 0; k < s.obj_num; k++)
	{
		glPushMatrix();					//put the current matrix into stack (modelview)
		glTranslatef(s.transfer_vector[k].x, s.transfer_vector[k].y, s.transfer_vector[k].z);
		glRotatef(s.angle[k], s.rotation_axis_vector[k].x, s.rotation_axis_vector[k].y, s.rotation_axis_vector[k].z);
		glScalef(s.scale_value[k].x, s.scale_value[k].y, s.scale_value[k].z);

		int lastMaterial = -1;
		for (size_t i = 0; i < s.object[k]->fTotal; ++i)
		{
			// set material property if this face used different material
			if (lastMaterial != s.object[k]->faceList[i].m)
			{
				lastMaterial = (int)s.object[k]->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT, s.object[k]->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, s.object[k]->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, s.object[k]->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &s.object[k]->mList[lastMaterial].Ns);
			}

			Vector3D tri_obj[3], tri_far[3];
			Vector3D light_vector(l.light_position[0], l.light_position[1], l.light_position[2]);

			for (int j = 0; j < 3; j++)
				tri_obj[j] = s.object[k]->vList[s.object[k]->faceList[i][j].v].ptr;

			for (int i = 0; i < 3; i++)
				tri_far[i] = tri_obj[i] + (tri_obj[i] - light_vector) * 5;

			if (is_front(tri_obj))
				for (int i = 0; i < 3; i++)
				{
					glBegin(GL_POLYGON);
					glVertex3fv(tri_obj[i].entries);
					glVertex3fv(tri_far[i].entries);
					glVertex3fv(tri_far[(i + 1) % 3].entries);
					glVertex3fv(tri_obj[(i + 1) % 3].entries);
					glEnd();
				}
		}
		glPopMatrix();
	}
}

void draw_scene()
{
	for (int k = 0; k < s.obj_num; k++)
	{
		glPushMatrix();					//put the current matrix into stack (modelview)
		glTranslatef(s.transfer_vector[k].x, s.transfer_vector[k].y, s.transfer_vector[k].z);
		glRotatef(s.angle[k], s.rotation_axis_vector[k].x, s.rotation_axis_vector[k].y, s.rotation_axis_vector[k].z);
		glScalef(s.scale_value[k].x, s.scale_value[k].y, s.scale_value[k].z);

		int lastMaterial = -1;
		for (size_t i = 0; i < s.object[k]->fTotal; ++i)
		{
			// set material property if this face used different material
			if (lastMaterial != s.object[k]->faceList[i].m)
			{
				lastMaterial = (int)s.object[k]->faceList[i].m;
				glMaterialfv(GL_FRONT, GL_AMBIENT, s.object[k]->mList[lastMaterial].Ka);
				glMaterialfv(GL_FRONT, GL_DIFFUSE, s.object[k]->mList[lastMaterial].Kd);
				glMaterialfv(GL_FRONT, GL_SPECULAR, s.object[k]->mList[lastMaterial].Ks);
				glMaterialfv(GL_FRONT, GL_SHININESS, &s.object[k]->mList[lastMaterial].Ns);
			}

			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j < 3; ++j)
			{
				glNormal3fv(s.object[k]->nList[s.object[k]->faceList[i][j].n].ptr);
				glVertex3fv(s.object[k]->vList[s.object[k]->faceList[i][j].v].ptr);
			}
			glEnd();
		}
		glPopMatrix();
	}
}



