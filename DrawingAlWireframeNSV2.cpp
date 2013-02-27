
#include "Angel.h"
#include <string.h>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

#define MAX_VERT 20000 
point4 data[MAX_VERT];
point4 colors[MAX_VERT];
int dataSize = 0;  // how many vertices


mat4 CTM;

// shader variable locations
GLuint programID;
GLuint bufferID;
GLuint vPositionLoc;
GLuint vColorLoc;
GLuint ctmID;

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

			    // ONLY USING THE FIRST 3 ... this code must be improved
				v1 = v[0];
				v2 = v[1];
				v3 = v[2];

				// generate a random color
				float r = rand()%256/255.0, g = rand()%256/255.0 , b = rand()%256/255.0; 
				if (dataSize+3 > MAX_VERT) {
					printf("Arrays are not big enough!\n");
					system("PAUSE");
	                exit(1);
				}
				// filling in 3 vertices to form a triangle
				// also 3 colors, one for each vertex
				// corresponding colors and vertices are at the same index in each array 
				colors[dataSize] = color4(r,g,b,1);
				data[dataSize++] = vertices[v1];
				colors[dataSize] = color4(r,g,b,1);
				data[dataSize++] = vertices[v2];
				colors[dataSize] = color4(r,g,b,1);
				data[dataSize++] = vertices[v3];
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
    readFile();
    glClearColor(1.0, 1.0, 1.0, 1.0); /* white background */
}

void display()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(yRot,0,1,0);
	glScalef(0.3, 0.3, -0.3);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

    for (int v = 0; v < dataSize; v += 3) {
		glBegin(GL_POLYGON);
		glColor3fv(colors[v]);
		glVertex3fv(data[v]);
		glColor3fv(colors[v+1]);
		glVertex3fv(data[v+1]);
		glColor3fv(colors[v+2]);
		glVertex3fv(data[v+2]);
		glEnd();
	}

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
    glutCreateWindow("Defective Al Capone - No Shaders");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(mykey);
	glutSpecialFunc(myspecialkey);

    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}
