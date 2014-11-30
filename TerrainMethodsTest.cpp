#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
using namespace std;
float RandomFloat(float a, float b) {
    float random = ((float) rand()) / (float) RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}
//will give you n-1 number of squares
int lengthZ = 10;
int widthX = 10;

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

            pointer[i][j].push_back(i*quadSize);
            pointer[i][j].push_back(10);
            pointer[i][j].push_back(j*quadSize);

        }
    }
    return pointer;
}

bool wireFrameMode = false;
std::vector<vector<vector<float> > > points = createPoints(lengthZ,widthX);
std::vector<vector<float> > pointer1;
std::vector<vector<float> > pointer2;

std::vector<float> randomLine(int length, int width)
{
    //point 1
    int p1Index = RandomFloat(0,width);
    std::vector<float> p1 = points[0][p1Index];

    int p2Index = RandomFloat(0,width);
    std:vector<float> p2 = points[length-1][p2Index];

    float slope = (p2[2] - p1[2])/(p2[0]-p1[0]);
    //rearrange y=mx+b to get intercept b=y-mx
    float intercept = p2[2] - slope*p2[0];

    std::vector<float> line(2);
    line[0] = slope;
    line[1] = intercept;
    return line;
}
std::vector<vector<float> > heightMap(int length, int width, int numOfLines)
{
    std::vector<vector<float> > heightRow;
    std::vector<float> line1 = randomLine(length,width);

    std::vector<vector<float> > lines;
    for (int i = 0;i<numOfLines;i++){
        std::vector<float> line = randomLine(lengthZ, widthX);
        lines.push_back(line);
    }
    float increment = .1;
    for (int i = 0;i<length;i++){

        std::vector<float> heightPoints;
        heightRow.push_back(heightPoints);

        for(int j = 0;j<width;j++){
            //need to iterate for each line equation
            //equation of line y=mx+b. since checking each row, check each x.
            if (j < (i-line1[1])/line1[0]){
                heightRow[i].push_back(55);
            }
            else{
                heightRow[i].push_back(3);
            }
        }
    }
    return heightRow;
}

std::vector<float> normalizedVector(vector<float> a, vector<float> b)
{
    //remember the first indice of a point is the z value, the second is the y, the third is the x.
    std::vector<float> v1(3);
    std::vector<float> v2(3);
    for(int i = 0;i<3;i++){
        v1[i] = a[i];
        v2[i] = b[i];
        printf("%f\n",v1[i]);
        printf("%f\n",v2[i]);
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
std::vector<vector<vector<float> > > vectorNormals(int length, int width)
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
std::vector<vector<vector<float> > > normalVectors = vectorNormals(lengthZ,widthX);


int main(int argc, char** argv)
{

    //create the pointer, set it to the first row
    std::vector<vector<float> > pointer1 = points[4];
    //create the array of heights for each row
    std::vector<vector<float> > heightVector = heightMap(lengthZ,widthX,1);
    //assign a pointer for the rows of heights, set it to the first row
    std::vector<float> heightPointer1 = heightVector[1];
    //assign a second pointer for the rows of heights
    std::vector<float> heightPointer2;
    //create a random line cutting the grid
    std::vector<float> line1 = randomLine(lengthZ,widthX);
    int numOfLines = 10;
    std::vector<vector<float> > lines;

    std::vector<float> a(3);
    a[0] = 6;
    a[1] = 4;
    a[2] = 1;
    printf("%f, %f, %f",a[0],a[1],a[2]);
    std::vector<float> b(3);
    b[0] = 2;
    b[1] = 2;
    b[2] = 2;
    std::vector<float> c = normalizedVector(a,b);
    printf("x: %f, y: %f, z: %f",c[0],c[1],c[2]);
    for (int i = 0;i<numOfLines;i++){
        //printf("line %d slope %f, intercept %f\n",i,lines[i][0],lines[i][1]);
    }
    for (int i = 0;i<pointer1.size();i++){
        //printf("%f %f %f\n",pointer1[i][0],pointer1[i][1],pointer1[i][2]);
    }
    //printf("slope is %f, intercept is %f\n",line1[0],line1[1]);
    for (int j = 0;j<lengthZ;j++){
        for(int k = 0;k<widthX;k++){
            //printf("point %d, %d height: %f\n",j,k,heightVector[j][k]);
        }
    }

    return 0;
}
