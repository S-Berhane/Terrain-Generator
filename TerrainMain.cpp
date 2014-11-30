/**
CompSci 3GC3 Assignment 3: Terrain Modelling
By: Simon Berhane
Email: s.berhane23@gmail.com
Features:
Toggle W to alternate between wireframe, filledframe, and both.
Toggle M to alternate between perspective and orthogonal display.
Change the lineAlg boolean parameter to alternate between using the fault algorithm(true) and a modified circle algorithm(false).
The color mode is set to be similar to a forest, where green and blue are at the bottom and gray and white are at the top.
Hit R to reset using new height values.
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>
#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif
using namespace std;
/**
Begin: Global variables
*/

//will give you n-1 number of squares
int lengthZ = 80;
int widthX = 80;
//the number of line algorithm iterations
int lineIterations = 50;
//specifies the camera's eye
float pos[] = {0, 1, 0};
float rot[] = {0, 0, 0};

//values for the camera in perspective and orthogonal mode
float perspCamPos[] = {-80,80,-90};
float orthoCamPos[] = {5,5,0};
bool orthoMode = true;
bool perspMode = false;

bool reset = false;
bool lineAlg = true;

/**
End: Global Variables
*/

/**
Returns a random float between 2 numbers.
*/
float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}
/**
Creates a 3D vector representing the points on the terrain. The first
index contains the number of rows, the second index contains the number of points in
the row. The third index contains the specific x,y,z value of the point in the
row.
*/
std::vector<vector<vector<float> > >createPoints(int lengthZ, int widthX)
{
    std::vector<vector<vector<float> > > pointer;
    //scaling factor for the quad
    int quadSize = 1;
    for(int i = 0;i<lengthZ;i++){
        std::vector<vector<float> > miniPointer;
        pointer.push_back(miniPointer);

        for(int j = 0;j<widthX;j++){

            std::vector<float> miniMiniPointer;
            pointer[i].push_back(miniMiniPointer);
            //set the values of each point
            pointer[i][j].push_back(i*quadSize-.5*widthX);
            pointer[i][j].push_back(8);
            pointer[i][j].push_back(j*quadSize-.5*lengthZ);
        }
    }
    return pointer;
}


/**
Creates a random line, which can either cut the terrain horizontally
or vertically. The terrain is initially created with zero height, so the plane is
created across the x-z axis.
To cut the terrain horizontally, one point from the first column of the terrain,
and one point from the last column of the terrain are randomly selected.
To cut the terrain vertically, one point from the first row of the terrain,
and one point from the last row of the terrain are randomly selected.
Then the line connecting the two points is calculated.
What gets returned is a vector containing the slope and intercept of the line.
*/
std::vector<float> randomLine(int length, int width, vector<vector<vector<float> > > points)
{
    int p1Index,p2Index;
    std::vector<float> p1,p2;
    float x = RandomFloat(0,10);
    //vertical line
    if(x<5){
        //point 1
        p1Index = RandomFloat(0,width);
        p1 = points[0][p1Index];
        //point 2
        p2Index = RandomFloat(0,width);
        p2 = points[length-1][p2Index];
    }
    //horizontal line
    if(x>=5){
        //alternate point 1
        p1Index = RandomFloat(0,length);
        p1 = points[p1Index][0];
        //alternate point 2
        p2Index = RandomFloat(0,length);
        p2 = points[p2Index][width-1];
    }
    float slope = (p2[2] - p1[2])/(p2[0]-p1[0]);
    //rearrange y=mx+b to get intercept b=y-mx
    float intercept = p2[2] - slope*p2[0];

    std::vector<float> line(2);
    line[0] = slope;
    line[1] = intercept;
    return line;
}
/**
Creates a 2D vector, consisting of the height at each point in the array.
The first index represents the row, the second index represents a point's
height value. This implementation of generating a height map uses the fault
algorithm.
This method also contains a slightly different implementation of the circle algorithm
Reference:http://www.lighthouse3d.com/opengl/terrain/index.php?impdetails
*/
std::vector<vector<float> > heightMap2(int length, int width, int numofLines, vector<vector<vector<float> > > points,bool lineAlg)
{
    float v,a,b,c,d,temp1,temp2;
    int circleCenterX;
    int circleCenterZ;
    float distFromCenter;
    float terrainCircleSize;
    float maxHeightVariation=.5;
    float pd;
    std::vector<vector<float> > heightRow(length,vector<float>(width));
    std::vector<vector<float> > lines;
    float increment = 1;
    float circleIncrement = 4;
    d = sqrt(width*width + length*length);

    for(int k = 0;k<numofLines;k++){
        if(lineAlg){
        v = rand();
        a = sin(v);
        b = cos(v);
        c = RandomFloat(-d/2,d/2);
        }
        else{
            circleCenterX = RandomFloat(0,width-1);
            circleCenterZ = RandomFloat(0,length-1);
            terrainCircleSize = RandomFloat(0,4);
        }
        for (int i = 0;i<length;i++){

            std::vector<float> heightPoints;
            heightRow.push_back(heightPoints);

            for(int j = 0;j<width;j++){
                if(lineAlg == true){
                    if (a*points[i][j][2] + b*points[i][j][0] - c > 0){
                        heightRow[i][j] += increment;
                    }
                    else{
                        heightRow[i][j] -= increment;
                    }
                }
                else{
                //alteration of the circle algorithm
                temp1 = points[i][j][2] - points[circleCenterZ][circleCenterX][2];
                temp2 = points[i][j][0] - points[circleCenterZ][circleCenterX][0];
                pd = sqrt(temp1*temp1+temp2*temp2);
                if (fabsf(pd) <= terrainCircleSize)
                    heightRow[i][j]+= circleIncrement;
                }
            }
        }
    }
    return heightRow;
}
/**
This is an alternative method of generating a heightmap. The difference here
is that the lines are created so that each cut will make the same corner of the terrain
always have a positive height, and can create a unique scene representing a single mountain with
a river under it.

*/
std::vector<vector<float> > heightMap(int length, int width, int numOfLines, vector<vector<vector<float> > > points)
{
    std::vector<vector<float> > heightRow(length,vector<float>(width));

    std::vector<vector<float> > lines;
    float x = RandomFloat(0,10);
    float increment = .2;

    for (int i = 0;i<numOfLines;i++){
        std::vector<float> line = randomLine(lengthZ, widthX,points);
        lines.push_back(line);
    }
    for (int i = 0;i<length;i++){

        std::vector<float> heightPoints;
        heightRow.push_back(heightPoints);

        for(int j = 0;j<width;j++){
            //need to iterate for each line equation
            for(int k = 0;k<numOfLines;k++){
                //equation of line y=mx+b. since checking each row, check each x.
                if (x<5){
                    //because Points[i][j][0] = j, just using j will suffice, and improve performance
                    if (j < (points[i][j][0]-lines[k][1])/lines[k][0]){
                        //heightRow[i].push_back(heightRow[i]);
                        heightRow[i][j] += increment;
                    }
                    else{
                        heightRow[i][j] -= increment;
                    }
                }
                if (x>=5){
                    if (j < (points[i][j][0]-lines[k][1])/lines[k][0]){
                        //heightRow[i].push_back(heightRow[i]);
                        heightRow[i][j] += increment;
                    }
                    else{
                        heightRow[i][j] -= increment;
                    }
                }
            }
        }
    }
    return heightRow;
}


/**
Returns the normalized cross product of two vectors.
Reference:http://www.lighthouse3d.com/opengl/terrain/index.php3?normals
*/
std::vector<float> normalizedVector(vector<float> a, vector<float> b)
{
    //remember the first indice of a point is the z value, the second is the y, the third is the x.
    std::vector<float> v1(3);
    std::vector<float> v2(3);
    for(int i = 0;i<a.size();i++){
        v1[i] = a[i];
        v2[i] = b[i];
    }

    float vx = v1[1]*v2[0] - v1[0]*v2[1];
    float vy = v1[0]*v2[2] - v1[2]*v2[0];
    float vz = v1[2]*v2[1] - v1[1]*v2[2];

    float length = sqrt(vx*vx+vy*vy+vz*vz);
    std::vector<float> normalizedV(3);
    normalizedV[0] = vz/length;
    normalizedV[1] = vy/length;
    normalizedV[2] = vx/length;
    return normalizedV;
}
/**
Creates a 3D array containing the normal vectors for each point.

*/
std::vector<vector<vector<float> > > vectorNormals(int length, int width, vector<vector<vector<float> > > points)
{
    vector<float> normVector(3);
    vector<float> normVector1(3);
    vector<float> normVector2(3);
    vector<float> normVector3(3);
    vector<float> normVector4(3);
    vector<float> sum(3);
    float normLength;
    std::vector<vector<vector<float> > > normals(length);
    for(int i = 0;i<length;i++){

        std::vector<vector<float> > rows(width);
        normals[i] = rows;

        for(int j = 0;j<width;j++){

            std::vector<float> eachNormal(3);
            normals[i][j] = eachNormal;
            //corners
            if(i==0 && j==0){
                normVector = normalizedVector(points[i+1][j],points[i][j+1]);
                normals[i][j][0] = normVector[0];
                normals[i][j][1] = normVector[1];
                normals[i][j][2] = normVector[2];
            }
            else if(i==0 && j==width-1){
                normVector = normalizedVector(points[i+1][j],points[i][j-1]);
                normals[i][j][0] = normVector[0];
                normals[i][j][1] = normVector[1];
                normals[i][j][2] = normVector[2];
            }
            else if(i==length-1 && j==0){
                normVector = normalizedVector(points[i-1][j],points[i][j+1]);
                normals[i][j][0] = normVector[0];
                normals[i][j][1] = normVector[1];
                normals[i][j][2] = normVector[2];
            }

            else if(i==length-1 && j==width-1){
                normVector = normalizedVector(points[i-1][j],points[i][j-1]);
                normals[i][j][0] = normVector[0];
                normals[i][j][1] = normVector[1];
                normals[i][j][2] = normVector[2];
            }
            //edges
            else if(i==0 && j>0 && j<width-1){
                normVector1 = normalizedVector(points[i][j-1], points[i+1][j]);
                normVector2 = normalizedVector(points[i+1][j], points[i][j+1]);

                sum[0] = normVector1[0]+normVector2[0];
                sum[1] = normVector1[1]+normVector2[1];
                sum[2] = normVector1[2]+normVector2[2];
                normLength = sqrt(sum[0]*sum[0]+sum[1]*sum[1]+sum[2]*sum[2]);

                normals[i][j][0] = (sum[0]/normLength);
                normals[i][j][1] = (sum[1]/normLength);
                normals[i][j][2] = (sum[2]/normLength);
            }
            else if(i==length-1 && j>0 && j<width-1){
                normVector3 = normalizedVector(points[i][j+1], points[i-1][j]);
                normVector4 = normalizedVector(points[i-1][j], points[i][j-1]);

                sum[0] = normVector3[0]+normVector4[0];
                sum[1] = normVector3[1]+normVector4[1];
                sum[2] = normVector3[2]+normVector4[2];
                normLength = sqrt(sum[0]*sum[0]+sum[1]*sum[1]+sum[2]*sum[2]);

                normals[i][j][0] = (sum[0]/normLength);
                normals[i][j][1] = (sum[1]/normLength);
                normals[i][j][2] = (sum[2]/normLength);
            }
            else if(j==0 && i>0 && i<length-1){
                normVector2 = normalizedVector(points[i+1][j], points[i][j+1]);
                normVector3 = normalizedVector(points[i][j+1], points[i-1][j]);
                sum[0] = normVector2[0]+normVector3[0];
                sum[1] = normVector2[1]+normVector3[1];
                sum[2] = normVector2[2]+normVector3[2];
                normLength = sqrt(sum[0]*sum[0]+sum[1]*sum[1]+sum[2]*sum[2]);

                normals[i][j][0] = (sum[0]/normLength);
                normals[i][j][1] = (sum[1]/normLength);
                normals[i][j][2] = (sum[2]/normLength);
            }
            else if(j==width-1 && i>0 && i<length-1){
                normVector4 = normalizedVector(points[i-1][j], points[i][j-1]);
                normVector1 = normalizedVector(points[i][j-1], points[i+1][j]);
                sum[0] = normVector4[0]+normVector1[0];
                sum[1] = normVector4[1]+normVector1[1];
                sum[2] = normVector4[2]+normVector1[2];
                normLength = sqrt(sum[0]*sum[0]+sum[1]*sum[1]+sum[2]*sum[2]);

                normals[i][j][0] = (sum[0]/normLength);
                normals[i][j][1] = (sum[1]/normLength);
                normals[i][j][2] = (sum[2]/normLength);
            }
            //the rest
            else if(i>0 && i<length-1 && j>0 && j<width-1){

                normVector1 = normalizedVector(points[i][j-1], points[i+1][j]);
                normVector2 = normalizedVector(points[i+1][j], points[i][j+1]);
                normVector3 = normalizedVector(points[i][j+1], points[i-1][j]);
                normVector4 = normalizedVector(points[i-1][j], points[i][j-1]);

                sum[0] = normVector1[0]+normVector2[0]+normVector3[0]+normVector4[0];
                sum[1] = normVector1[1]+normVector2[1]+normVector3[1]+normVector4[1];
                sum[2] = normVector1[2]+normVector2[2]+normVector3[2]+normVector4[2];
                normLength = sqrt(sum[0]*sum[0]+sum[1]*sum[1]+sum[2]*sum[2]);

                normals[i][j][0] = (sum[0]/normLength);
                normals[i][j][1] = (sum[1]/normLength);
                normals[i][j][2] = (sum[2]/normLength);

            }
        }
    }
    return normals;

}
/**
Begin: Global Variables
*/

int wireFrameMode = 0;

std::vector<vector<float> > pointer1;
std::vector<vector<float> > pointer2;
std::vector<float> heightPointer1;
std::vector<float> heightPointer2;
std::vector<vector<float> > normalPointer1;
std::vector<vector<float> > normalPointer2;
std::vector<vector<vector<float> > > points = createPoints(lengthZ,widthX);
std::vector<vector<float> > heightVector = heightMap2(lengthZ,widthX,lineIterations,points,lineAlg);
std::vector<vector<vector<float> > > normalVectors = vectorNormals(lengthZ,widthX,points);
//std::vector<vector<float> > heightVector = heightMap(lengthZ,widthX,lineIterations,points);
/**
End: Global Variables
*/


/**
Utilizing the above, here is where the terrain gets created.
*/
void drawTerrain(int length, int width,vector<vector<vector<float> > > points, bool color)
{
    float y;

    for(int i = 0;i<length;i++){
        glBegin(GL_QUAD_STRIP);
            for (int j = 0;j<width;j++){
                y = heightVector[i][j];
                if(y<0){glColor3f(0,0,1);}
                if(y==0){glColor3f(.2,.8,.2);}
                if(0<y && y<=2){glColor3f(.2,.2,.2);}
                if(2<y && y<=4){glColor3f(.4,.4,.4);}
                if(4<y && y<=4){glColor3f(.5,.5,.5);}
                if(6<y && y<=9){glColor3f(.6,.6,.6);}
                if(9<y && y<=12){glColor3f(.7,.7,.7);}
                if(12<y && y<=15){glColor3f(.8,.8,.8);}
                if(15<y && y<=18){glColor3f(.85,.85,.85);}
                if(18<y && y<=21){glColor3f(.9,.9,.9);}
                if(21<y){glColor3f(1,1,1);}
                if(wireFrameMode==1 || color == true) glColor3f(0,1,0);

                if(i == 0){
                    //here the ordering matters. Please refer to the attached doc to understand whats going on
                    pointer1 = points[i];
                    pointer2 = points[i+1];
                    heightPointer1 = heightVector[i];
                    heightPointer2 = heightVector[i+1];
                    normalPointer1 = normalVectors[i];
                    normalPointer2 = normalVectors[i+1];

                    glNormal3f(normalPointer2[j][2],normalPointer2[j][1],normalPointer2[j][0]);
                    glVertex3f(pointer2[j][2],heightPointer2[j],pointer2[j][0]);

                    glNormal3f(normalPointer1[j][2],normalPointer1[j][1],normalPointer1[j][0]);
                    glVertex3f(pointer1[j][2],heightPointer1[j],pointer1[j][0]);
                }
                else{
                    if(i%2 == 1){
                        pointer1 = points[i];
                        heightPointer1 = heightVector[i];
                        normalPointer1 = normalVectors[i];

                        glNormal3f(normalPointer2[j][2],normalPointer2[j][1],normalPointer2[j][0]);
                        glVertex3f(pointer2[j][2],heightPointer2[j],pointer2[j][0]);

                        glNormal3f(normalPointer1[j][2],normalPointer1[j][1],normalPointer1[j][0]);
                        glVertex3f(pointer1[j][2],heightPointer1[j],pointer1[j][0]);
                        }
                    else{
                        pointer2 = points[i];
                        heightPointer2 = heightVector[i];
                        normalPointer2 = normalVectors[i];

                        glNormal3f(normalPointer1[j][2],normalPointer1[j][1],normalPointer1[j][0]);
                        glVertex3f(pointer1[j][2],heightPointer1[j],pointer1[j][0]);

                        glNormal3f(normalPointer2[j][2],normalPointer2[j][1],normalPointer2[j][0]);
                        glVertex3f(pointer2[j][2],heightPointer2[j],pointer2[j][0]);
                    }
                }
            }
        glEnd();
    }
}

void drawAxis()
{
	glBegin(GL_LINES);
	glColor3f(1, 0, 1);
	glVertex3f(-100,0,0);
	glColor3f(1,1,1);
	glVertex3f(100,0,0);
	glColor3f(0,1,0);
	glVertex3f(0,-100,0);
	glColor3f(1,1,1);
	glVertex3f(0,100,0);
	glColor3f(0,0,1);
	glVertex3f(0,0,-100);
	glColor3f(1,1,1);
	glVertex3f(0,0,100);
	glEnd();
}

/**
Setting to change the camera to orthogonal display mode or perspective display mode.
*/
void orthoDisplay()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-100, 100, -100, 100, -100, 100);
}
void perspectiveDisplay()
{
/**
This is where everything gets created for display.
*/

    //change to projection matrix mode, set the extents of our viewing volume
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluPerspective(50, 1, 1,200);

}
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);

    if(perspMode == true){
        perspectiveDisplay();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(perspCamPos[0], perspCamPos[1], perspCamPos[2], 0,0,0, 0,30,0);
    }
    if(orthoMode == true){
        orthoDisplay();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        gluLookAt(orthoCamPos[0], orthoCamPos[1], orthoCamPos[2], 0,0,0, 0,30,0);
    }
    glRotatef(rot[1], 0, 1, 0);


    //all lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
    GLfloat amb[4] = {1,1,1,1};
    GLfloat dif[4] = {1,0,0,1};
    GLfloat spec[4] = {0,0, 1.0, 1.0};
    float m_amb[] = {0.33,0.22,0.03,1.0};
    float m_dif[] = {0.78,0.57,0.11,1.0};
    float m_spec[] = {0.99, 0.91, 0.81, 1.0};
    float shiny = 27;

    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,m_amb);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,m_dif);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,m_spec);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_AMBIENT_AND_DIFFUSE);


    //enable backface culling
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    drawAxis();
    if(wireFrameMode==0){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawTerrain(lengthZ,widthX,points,false);
    }
    if(wireFrameMode==1){
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        drawTerrain(lengthZ,widthX,points,false);
    }
    if(wireFrameMode==2){
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        drawTerrain(lengthZ,widthX,points,true);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        drawTerrain(lengthZ,widthX,points,false);

    }
	glutSwapBuffers();

}
void reshape(int w, int h)
{

    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    //the current matrix specifies the projection transformations and subsequent
    //transformation calls affect the proj matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80.0,2.0,1.5,20.0); //determines the shape of the viewing volume
    //the succeeding transformations now affect the modelview matrix instead of proj matrix
    glMatrixMode(GL_MODELVIEW);
}
void keyb(unsigned char key, int x, int y)
{
	switch (key)
	{
		case 'q':
		case 27:
			exit (0);
			break;
		case 'w':
		case 'W':
            if(wireFrameMode==0){
                wireFrameMode=1;
            }
            else if(wireFrameMode==1){
                wireFrameMode=2;
            }
            else if(wireFrameMode==2){
                wireFrameMode=0;
            }
            break;
        case 'm':
        case 'M':
            if (perspMode == true)
            {
                perspMode = false;
                orthoMode = true;
            }
            else if (orthoMode == true)
            {
                orthoMode = false;
                perspMode = true;
            }
            break;
        case 'a':
        case 'A':
            if(lineAlg == true){
                lineAlg = false;
            }
            else if(lineAlg == false){
                lineAlg = true;
            }
            break;
        case 'r':
        case 'R':
            heightVector = heightMap2(lengthZ,widthX,lineIterations,points,lineAlg);
            normalVectors = vectorNormals(lengthZ,widthX,points);
	}
	glutPostRedisplay();
}
void special(int key, int x, int y)
{
	/* arrow key presses move the camera */
	switch(key)
	{
		case GLUT_KEY_LEFT:
			if (orthoMode == true)orthoCamPos[0] -= 0.5;
			else if (perspMode == true) perspCamPos[0] -= 0.5;
			break;

		case GLUT_KEY_RIGHT:
			if (orthoMode == true)orthoCamPos[0] += 0.5;
			else if (perspMode == true) perspCamPos[0] += 0.5;
			break;

		case GLUT_KEY_UP:
			if (orthoMode == true)orthoCamPos[2] -= 0.5;
			else if (perspMode == true) perspCamPos[2] -= 0.5;
			break;

		case GLUT_KEY_DOWN:
			if (orthoMode == true)orthoCamPos[2] += 0.5;
			else if (perspMode == true) perspCamPos[2] += 0.5;
			break;

		case GLUT_KEY_HOME:
			if (orthoMode == true)orthoCamPos[1] += 0.5;
			else if (perspMode == true) perspCamPos[1] += 0.5;
			break;

		case GLUT_KEY_END:
			if (orthoMode == true)orthoCamPos[1] -= 0.5;
			else if (perspMode == true) perspCamPos[1] -= 0.5;
			break;

	}
	glutPostRedisplay();
}
void init(void)
{

}

/* main function - program entry point */
int main(int argc, char** argv)
{
    //glut initialization stuff:
	// set the window size, display mode, and create the window
	glutInit(&argc, argv);
	glutInitWindowSize(900, 900);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("BasicTerrain");
	//enable Z buffer test, otherwise things appear in the order they're drawn
	glEnable(GL_DEPTH_TEST);

    //loadTerrain loads the 3D terrain
	//register glut callbacks for keyboard and display function
	glutKeyboardFunc(keyb);
	glutSpecialFunc(special);

	glutDisplayFunc(display);
	//glutIdleFunc(idle);
	glutReshapeFunc(reshape);

	//start the program!
	glutMainLoop();

	return 0;
}
