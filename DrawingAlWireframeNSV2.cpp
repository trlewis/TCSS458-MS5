

#include "Angel.h"
#include <string.h>

#include <iostream>

#include "materials.hpp"

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define MAX_VERT 35000//20000
point4 data[MAX_VERT];
point4 colors[MAX_VERT];
int dataSize = 0;  // how many vertices

point4 light_position( 0.0, 0.0, 1.0, 0.0 );
mat4 CTM;

// shader variable locations
GLuint programID;
GLuint bufferID;
GLuint vPositionLoc;
GLuint vColorLoc;
GLuint ctmID;

//list of materials
MaterialList mtll;

float yRot = 0.0;


int parseVertString(char * verticesString, int v[]) {
	char token[20];
	int vCount = 0;
	int t = 0;
	for (int s = 0; verticesString[s] != '\n'; s++) {
		if (verticesString[s] != ' ') {
			token[t++] = verticesString[s];
		} else if (t > 0) {
			token[t++] = 0;
			v[vCount++] = atoi(token);
			t = 0;
		}
	}

	if (t > 0) {
		token[t++] = 0;
		v[vCount++] = atoi(token);
	}
	return vCount;
}

void readFile() {
	Material m;
	bool readfirstg = false;

    char fileName[50];
    char str1[300];
    char str2[300];
    FILE *input;
    float x,y,z;
    int v1,v2,v3;

    vec4 vertices[4000];
    int numVertices = 1;   // .obj input file numbers the first vertex starting at index 1

	int facetCount = 0;

	printf("Enter the name of the input file (no blanks): ");
	gets(fileName);
	input = fopen(fileName, "r+");

    if (input == NULL)
        perror ("Error opening file");
    else {
        while(feof(input)== 0){
            fscanf(input,"%s",str1);
			if(strcmp(str1,"mtllib")==0) {
				fscanf(input,"%s",str1);
				mtll = MaterialList(str1);
			}
			if(strcmp(str1,"g")==0) {
				if(readfirstg) {
					fscanf(input,"%s",str1);
					m = mtll.getMaterial(str1);
				}
				else
					readfirstg = true;
			}
			if (strcmp(str1,"v")==0) {  // lines starting with v  =  a vertex 
				fscanf(input,"%f %f %f", &x, &y, &z);
				vertices[numVertices++] = vec4(x,y,z,1);
			} else if (strcmp(str1,"f")==0) {  // lines starting with f = a facet
				facetCount++;

				// Code upgrade
				// The number of vertices that follow "f" is variable
				// so here is some code to gather all the integers into an array
				char verticesString[200];
				fgets(verticesString, 200, input);

				int v[50];
				int vCount = parseVertString(verticesString,v);

				if (dataSize+3 > MAX_VERT) {
					printf("Arrays are not big enough!\n");
					system("PAUSE");
	                exit(1);
				}

				for(int i = 0 ; i < vCount-1 ; i++) {
					colors[dataSize] = m.diffuse;
					data[dataSize++] = vertices[v[0]];
					colors[dataSize] = m.diffuse;
					data[dataSize++] = vertices[v[i]];
					colors[dataSize] = m.diffuse;
					data[dataSize++] = vertices[v[i+1]];
				}
			} else {  // line began with something else - ignore for now
				fscanf(input, "%[^\n]%*c", str2);
				//printf("Junk line : %s %s\n", str1, str2);
			}
		}
	}
	printf("Facet count = %d\n",facetCount);
}

void init()
{
	// setting light properties
	color4 light_diffuse( 0.4, 0.4, 0.4, 1.0 );
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);

	glEnable( GL_DEPTH_TEST );

    readFile();
    glClearColor(1.0, 1.0, 1.0, 1.0); // white background
}

void tri(int i) {
	vec4 v1 = data[i];
	vec4 v2 = data[i+1];
	vec4 v3 = data[i+2];
	vec4 normal = normalize(cross(v3-v1,v2-v1));

	glMaterialfv(GL_FRONT, GL_DIFFUSE, colors[i]);

	glBegin(GL_POLYGON);
	glNormal3f(normal.x,normal.y,normal.z);
	glVertex3fv(v1);
	glVertex3fv(v2);
	glVertex3fv(v3);
	glEnd();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(yRot,0,1,0);
	glScalef(0.3, 0.3, -0.3);

    for (int v = 0; v < dataSize; v += 3)
		tri(v);

    glutSwapBuffers();
}

void myspecialkey(int key, int mousex, int mousey)
{

    if(key == 100) {
        yRot += 1;
        if (yRot > 360.0 ) {
	        yRot -= 360.0;
	    }
	}
    if(key == 102) {
        yRot -= 1;
        if (yRot < -360.0 ) {
	        yRot += 360.0;
	    }
	}

    glutPostRedisplay();
}

void mykey(unsigned char key, int mousex, int mousey)
{
    float dr = 3.14159/180.0*5.0;
    if(key=='q'||key=='Q') exit(0);

    glutPostRedisplay();
}

void reshape(GLsizei ww, GLsizei hh)
{
    glViewport(0,0, ww, hh);
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 512);
    glutCreateWindow("Non-Defective Al Capone - No Shaders (Travis Lewis)");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(mykey);
	glutSpecialFunc(myspecialkey);

    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}

