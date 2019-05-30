/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer
* See Lab07.pdf for details.
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include "Cylinder.h"
#include "Cone.h"
#include "TextureBMP.h"
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

const float WIDTH = 20.0;
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float phong = 10;
const float eta = 1.02;
float transparency = 0.4;

float fogstart = -10;
float fogfinish = -60;
float fogmax = 400.0f;
bool fog = false;


//TextureBMP texture1;

TextureBMP texture1 = TextureBMP((char*)"jupiter.bmp");

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
    glm::vec3 backgroundCol(0);

    glm::vec3 light(45, 40, -5);
    glm::vec3 ambientCol(0.2);   //Ambient color of light

    glm::vec3 objectColor(0);
    glm::vec3 materialCol(0);
    glm::vec3 throughcol(0);
    glm::vec3 fogEffect(0);


    ray.closestPt(sceneObjects);        //Compute the closest point of intersetion of objects with the ray


    if(ray.xindex == -1) {return backgroundCol;}      //If there is no intersection return background colour

    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);
    glm::vec3 lightVector = glm::normalize(light -ray.xpt);

    glm::vec3 lightVector2 = light -ray.xpt;

    float lDotn = glm::dot(normalVector,lightVector);

    materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour


   // if(ray.xpt.y > 0){std::cout << ray_y << endl;}

    if(ray.xindex == 1)
    {
        int ray_x = ((ray.xpt.x)/4)+10;
        int ray_z = -(ray.xpt.z/10);
        //std::cout << ray_z << endl;

        if(ray_z % 2 == 0 && ray_x % 2 == 0)
        {
            materialCol = glm::vec3(0.6, 0.4, 0.4);
        }
        else if(ray_z % 2 == 1 &&ray_x % 2 == 1)
        {
            materialCol = glm::vec3(0.6, 0.4, 0.4);

        }
        else
        {
            materialCol = glm::vec3(0.8, 0.8, 0.85);
        }
    }


    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
    Ray shadow(ray.xpt, lightVector);
    shadow.closestPt(sceneObjects);


    float rDotv= glm::dot(reflVector, normalVector);
    float specularTerm;


    if(rDotv < 0)
    {
        specularTerm= 0.0;
    }
    else
    {
        specularTerm= pow(rDotv, phong);
    }

    if ((shadow.xindex > -1 && shadow.xdist <  glm::length(lightVector2))|| lDotn <= 0)
    {
        if(shadow.xindex == 9 || shadow.xindex == 8)
        {
        objectColor =  ambientCol*materialCol  * 3.0f ;
        }
        else{
            objectColor =  ambientCol*materialCol;
            }
    }
    else
    {
        objectColor = ambientCol*materialCol + lDotn*materialCol + specularTerm * glm::vec3(1);
    }


    if(ray.xindex == 0 && step < MAX_STEPS)
    {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1); //Recursion!
        objectColor += (0.8f*reflectedCol);
    }

    if(ray.xindex == 9) // refraction
    {

        glm::vec3 refracteddir = glm::refract(ray.dir, normalVector,1.0f/eta);
        Ray refrRay(ray.xpt, refracteddir);
        refrRay.closestPt(sceneObjects);

        glm::vec3 normalVector2 = sceneObjects[refrRay.xindex]->normal(refrRay.xpt);
        glm::vec3 refracDir2 = glm::refract(refracteddir, -normalVector2, eta);
        Ray refracRay2(refrRay.xpt,refracDir2);
        refracRay2.closestPt(sceneObjects);

        glm::vec3 refracCol2 = trace(refracRay2,1);
        objectColor = objectColor * transparency + refracCol2*(1-transparency); //transparent object

    }




    if(ray.xindex == 8)
    {

        Ray throughRay(ray.xpt, ray.dir);
        throughRay.closestPt(sceneObjects);


        if(throughRay.xindex == 8)
        {
            Ray throughRay2(throughRay.xpt, throughRay.dir);
            throughRay2.closestPt(sceneObjects);
            if(throughRay2.xindex == -1){return backgroundCol;}
            else{ objectColor = trace(throughRay2,1) + (objectColor * 0.25f);}
        }
        else
        {

        if(throughRay.xindex == -1){return backgroundCol;}
        else{objectColor = trace(throughRay,1) + (objectColor * 0.25f);}
        }



    }
    
    if(ray.xindex == 12)
    {
		glm::vec3 centerPoint(-10.0, 5.0, -80.0);
		glm::vec3 UnitVectorToSphereOrigin = glm::normalize(ray.xpt - centerPoint); 
		
		float s = 0.5f + atan2f(UnitVectorToSphereOrigin.z,UnitVectorToSphereOrigin.x)/(2 * M_PI);
		float t = 0.5f - asin(UnitVectorToSphereOrigin.y)/M_PI;
		
		objectColor = texture1.getColorAt(s, t);		

	}
    
    

    if(fog){
        float fogDist = -ray.xpt.z - fogstart;
        objectColor.x += (fogDist/fogmax)*(1 - objectColor.x);
        objectColor.y += (fogDist/fogmax)*(1 - objectColor.y);
        objectColor.z += (fogDist/fogmax)*(1 - objectColor.z);
        return objectColor;

    }

    return objectColor;





}




//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
    float xp, yp;  //grid point
    float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
    float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

    glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glBegin(GL_QUADS);  //Each cell is a quad.

    for(int i = 0; i < NUMDIV; i++)     //For each grid point xp, yp
    {
        xp = XMIN + i*cellX;
        for(int j = 0; j < NUMDIV; j++)
        {
            yp = YMIN + j*cellY;

            glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);  //direction of the primary ray

            Ray ray = Ray(eye, dir);        //Create a ray originating from the camera in the direction 'dir'
            ray.normalize();                //Normalize the direction of the ray to a unit vector
            glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value

            glColor3f(col.r, col.g, col.b);
            glVertex2f(xp, yp);             //Draw each cell with its color value
            glVertex2f(xp+cellX, yp);
            glVertex2f(xp+cellX, yp+cellY);
            glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}


//---This function initializes the scene -------------------------------------------
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);

    //-- Create a pointer to a sphere object
    Sphere *sphere1 = new Sphere(glm::vec3(10.0, -5.0, -150.0), 15.0, glm::vec3(0, 0, 1));
    //--Add the above to the list of scene objects.
    sceneObjects.push_back(sphere1);

    Plane *plane = new Plane (glm::vec3(-40., -20, -40),
    //Point A
    glm::vec3(40., -20, -40),
    //Point B
    glm::vec3(40., -20, -500),
    //Point C
    glm::vec3(-40., -20, -500),
    //Point D
    glm::vec3(0.5, 0.5, 0));
    //Colour 1

    //Colour 1
    //Point D
    sceneObjects.push_back(plane);
    

   float angle = 0.55;
   glm::vec3 Box1Position = glm::vec3(19.5 , -17.5, -115.0);
   
   glm::vec3 backUR = (0.5f * glm::vec3((5.0 * cos(angle)) + (5.0 * sin(angle)), 5.0 , (-5.0 * sin(angle)) + (5.0 * cos(angle)))) + Box1Position; // back upper right
   glm::vec3 backUL = (0.5f * glm::vec3((-5.0 * cos(angle)) + (5.0 * sin(angle)), 5.0 , (5.0 * sin(angle)) + (5.0 * cos(angle)))) + Box1Position; // back upper right
   glm::vec3 backBL = (0.5f * glm::vec3((-5.0 * cos(angle)) + (5.0 * sin(angle)), -5.0 , (5.0 * sin(angle)) + (5.0 * cos(angle)))) + Box1Position;  // back upper right
   glm::vec3 backBR = (0.5f * glm::vec3((5.0 * cos(angle)) + (5.0 * sin(angle)), -5.0 , (-5.0 * sin(angle)) + (5.0 * cos(angle)))) + Box1Position;  // back upper right
   glm::vec3 frontUR = (0.5f * glm::vec3((5.0 * cos(angle)) + (-5.0 * sin(angle)), 5.0 , (-5.0 * sin(angle)) + (-5.0 * cos(angle)))) + Box1Position;  // back upper right
   glm::vec3 frontUL = (0.5f * glm::vec3((-5.0 * cos(angle)) + (-5.0 * sin(angle)), 5.0 , (5.0 * sin(angle)) + (-5.0 * cos(angle)))) + Box1Position;  // back upper right
   glm::vec3 frontBL = (0.5f * glm::vec3((-5.0 * cos(angle)) + (-5.0 * sin(angle)), -5.0 , (5.0 * sin(angle)) + (-5.0 * cos(angle)))) + Box1Position;  // back upper right
   glm::vec3 frontBR = (0.5f * glm::vec3((5.0 * cos(angle)) + (-5.0 * sin(angle)), -5.0 , (-5.0 * sin(angle)) + (-5.0 * cos(angle)))) + Box1Position;  // back upper right


    Plane *squarebottom = new Plane (
    backBL,
    //Point A
    frontBL,
    //Point B
    frontBR,
    //Point C
    backBR,
    //Point D
    //Colour 1
    glm::vec3(0.0, 1.0, 0.0));
    //Colour 1
    //Point D
    sceneObjects.push_back(squarebottom);

    Plane *squaretop = new Plane (
    backUL,
    //Point A
    frontUL,
    //Point B
    frontUR,
    //Point C
    backUR,
    //Colour 1
    glm::vec3(0.0, 1.0, 0.0));
    //Colour 1
    //Point D
    sceneObjects.push_back(squaretop);

    Plane *squareback = new Plane (
    backUR,
    //Point A
    backUL,
    //Point B
    backBL,
    //Point C
    backBR,
    //Point D
    //Colour 1
    glm::vec3(0.0, 1.0, 0.0));
    //Colour 1
    //Point D
    sceneObjects.push_back(squareback);

    Plane *squarefront = new Plane (
    frontBL,
    //Point A
    frontBR,
    //Point B
    frontUR,
    //Point C
    frontUL,
    //Point D
    //Colour 1
    glm::vec3(0.0, 1.0, 0.0));
    //Colour 1
    //Point D
    sceneObjects.push_back(squarefront);


    Plane *squareleft = new Plane (
    backBL,
    //Point A
    frontBL,
    //Point B
    frontUL,
    //Point C
    backUL,
    //Point D
    //Colour 1
    glm::vec3(0.0, 1.0, 0.0));
    //Colour 1
    //Point D
    sceneObjects.push_back(squareleft);

    Plane *squareright = new Plane (
    backBR,
    //Point A
    frontBR,
    //Point B
    frontUR,
    //Point C
    backUR,
    //Colour 1
    glm::vec3(0.0, 1.0, 0.0));
    //Colour 1
    //Point D
    sceneObjects.push_back(squareright);



    Sphere *spheretransparent = new Sphere(glm::vec3(8.0, -15.0, -85.0), 5.0, glm::vec3(0.5, 1, 0));
    //--Add the above to the list of scene objects.
    sceneObjects.push_back(spheretransparent);

    Sphere *sphererefract = new Sphere(glm::vec3(1.0, -14.0, -105.0), 6, glm::vec3(0.5, 0.1, 0.4));
    //--Add the above to the list of scene objects.
    sceneObjects.push_back(sphererefract);

    Cylinder *cyclinder = new Cylinder(glm::vec3(-10, -20.0, -145.0),2.5,15.0, glm::vec3(0.8, 0.1, 0.6));
    //--Add the above to the list of scene objects.
    sceneObjects.push_back(cyclinder);
    
    Cone *Cone1 = new Cone(glm::vec3(-7.0, -20.0, -90.0),7.0,8.0, glm::vec3(0.95, 0.66, 0.25));
    //--Add the above to the list of scene objects.
    sceneObjects.push_back(Cone1);
    
    Sphere *picturesphere = new Sphere(glm::vec3(-10.0, 5.0, -80.0), 4, glm::vec3(0.5, 0.1, 0.4));
    //--Add the above to the list of scene objects.
    sceneObjects.push_back(picturesphere);
    
    
    
    
   float angle2 = 0.0;
   glm::vec3 Box1Position2 = glm::vec3(-18.0 , -17.5, -125.0);
   
   glm::vec3 backUR2 = (0.8f * glm::vec3((5.0 * cos(angle2)) + (5.0 * sin(angle2)) + (5*tan(-0.6)), 5.0 , (-5.0 * sin(angle2)) + (5.0 * cos(angle2)))) + Box1Position2; // back upper right
   glm::vec3 backUL2 = (0.8f * glm::vec3((-5.0 * cos(angle2)) + (5.0 * sin(angle2)) + (5*tan(-0.6)), 5.0 , (5.0 * sin(angle2)) + (5.0 * cos(angle2)))) + Box1Position2; // back upper right
   glm::vec3 backBL2 = (0.8f * glm::vec3((-5.0 * cos(angle2)) + (5.0 * sin(angle2)), -5.0 , (5.0 * sin(angle2)) + (5.0 * cos(angle2)))) + Box1Position2;  // back upper right
   glm::vec3 backBR2 = (0.8f * glm::vec3((5.0 * cos(angle2)) + (5.0 * sin(angle2)), -5.0 , (-5.0 * sin(angle2)) + (5.0 * cos(angle2)))) + Box1Position2;  // back upper right
   glm::vec3 frontUR2 = (0.8f * glm::vec3((5.0 * cos(angle2)) + (-5.0 * sin(angle2)) + (5*tan(-0.6)), 5.0 , (-5.0 * sin(angle2)) + (-5.0 * cos(angle2)))) + Box1Position2;  // back upper right
   glm::vec3 frontUL2 = (0.8f * glm::vec3((-5.0 * cos(angle2)) + (-5.0 * sin(angle2)) + (5*tan(-0.6)), 5.0 , (5.0 * sin(angle2)) + (-5.0 * cos(angle2)))) + Box1Position2;  // back upper right
   glm::vec3 frontBL2 = (0.8f * glm::vec3((-5.0 * cos(angle2)) + (-5.0 * sin(angle2)), -5.0 , (5.0 * sin(angle2)) + (-5.0 * cos(angle2)))) + Box1Position2;  // back upper right
   glm::vec3 frontBR2 = (0.8f * glm::vec3((5.0 * cos(angle2)) + (-5.0 * sin(angle2)), -5.0 , (-5.0 * sin(angle2)) + (-5.0 * cos(angle2)))) + Box1Position2;  // back upper right


    Plane *squarebottom2 = new Plane (
    backBL2,
    //Point A
    frontBL2,
    //Point B
    frontBR2,
    //Point C
    backBR2,
    //Point D
    //Colour 1
    glm::vec3(0.956, 0.85, 0.258));
    //Colour 1
    //Point D
    sceneObjects.push_back(squarebottom2);

    Plane *squaretop2 = new Plane (
    backUL2,
    //Point A
    frontUL2,
    //Point B
    frontUR2,
    //Point C
    backUR2,
    //Colour 1
    glm::vec3(0.956, 0.85, 0.258));
    //Colour 1
    //Point D
    sceneObjects.push_back(squaretop2);

    Plane *squareback2 = new Plane (
    backUR2,
    //Point A
    backUL2,
    //Point B
    backBL2,
    //Point C
    backBR2,
    //Point D
    //Colour 1
    glm::vec3(0.956, 0.85, 0.258));
    //Colour 1
    //Point D
    sceneObjects.push_back(squareback2);

    Plane *squarefront2 = new Plane (
    frontBL2,
    //Point A
    frontBR2,
    //Point B
    frontUR2,
    //Point C
    frontUL2,
    //Point D
    //Colour 1
    glm::vec3(0.956, 0.85, 0.258));
    //Colour 1
    //Point D
    sceneObjects.push_back(squarefront2);


    Plane *squareleft2 = new Plane (
    backBL2,
    //Point A
    frontBL2,
    //Point B
    frontUL2,
    //Point C
    backUL2,
    //Point D
    //Colour 1
    glm::vec3(0.956, 0.85, 0.258));
    //Colour 1
    //Point D
    sceneObjects.push_back(squareleft2);

    Plane *squareright2 = new Plane (
    backBR2,
    //Point A
    frontBR2,
    //Point B
    frontUR2,
    //Point C
    backUR2,
    //Colour 1
    glm::vec3(0.956, 0.85, 0.258));
    //Colour 1
    //Point D
    sceneObjects.push_back(squareright2);
    
    
    
    
    


}


void keyBoard (unsigned char key, int x, int y)
{
    if (key == 'f')
    {
			fog = true;
			glutPostRedisplay();
    }
    else if (key == 'c')
    {
			fog = false;
			glutPostRedisplay();
    }
    

}




int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer hdo27");
    

    glutDisplayFunc(display);
    initialize();
	glutKeyboardFunc(keyBoard);


    glutMainLoop();
    return 0;
}
