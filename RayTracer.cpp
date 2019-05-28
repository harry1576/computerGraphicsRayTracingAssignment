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


vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ----------------------------------
//   Computes the colour value obtained by tracing a ray and finding its
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
    glm::vec3 backgroundCol(0);
    glm::vec3 light(10, 40, -3);
    glm::vec3 ambientCol(0.2);   //Ambient color of light

    glm::vec3 objectColor(0);
    glm::vec3 materialCol(0);
    glm::vec3 throughcol(0);

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
            materialCol = glm::vec3(0.5, 0.15, 0.15);
        }
        else if(ray_z % 2 == 1 &&ray_x % 2 == 1)
        {
            materialCol = glm::vec3(0.6, 0.15, 0.15);

        }
        else
        {
            materialCol = glm::vec3(0.0, 0.15, 0.15);
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
        objectColor =  ambientCol*materialCol ;
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
		return objectColor;
        
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
			else{ objectColor = trace(throughRay2,1) + objectColor * 0.3f;}
		}
		else
		{
		
		if(throughRay.xindex == -1){return backgroundCol;}
		else{objectColor = trace(throughRay,1) + objectColor * 0.4f;}
		}
		
		

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
    Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -150.0), 15.0, glm::vec3(0, 0, 1));
    //--Add the above to the list of scene objects.
    sceneObjects.push_back(sphere1);

    Plane *plane = new Plane (glm::vec3(-20., -20, -40),
    //Point A
    glm::vec3(20., -20, -40),
    //Point B
    glm::vec3(20., -20, -200),
    //Point C
    glm::vec3(-20., -20, -200),
    //Point D
    glm::vec3(0.5, 0.5, 0));
    //Colour 1

    //Colour 1
    //Point D
    sceneObjects.push_back(plane);

    glm::vec3 backUR = glm::vec3(15.0, -10.0, -115.0); // back upper right
    glm::vec3 backUL = glm::vec3(10.0, -10.0, -115.0); // back upper left
    glm::vec3 backBL = glm::vec3(10.0, -15.0, -115.0); // back bottom left
    glm::vec3 backBR = glm::vec3(15.0, -15.0, -115.0); // back bottom right
    glm::vec3 frontUR = glm::vec3(15.0, -10.0, -105.0);
    glm::vec3 frontUL = glm::vec3(10.0, -10.0, -105.0);
    glm::vec3 frontBL = glm::vec3(10.0, -15.0, -105.0);//
    glm::vec3 frontBR = glm::vec3(15.0, -15.0, -105.0);//

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



    Sphere *spheretransparent = new Sphere(glm::vec3(-8.0, -15.0, -85.0), 3.0, glm::vec3(0.5, 1, 0));
    //--Add the above to the list of scene objects.
    sceneObjects.push_back(spheretransparent);
    
    Sphere *sphererefract = new Sphere(glm::vec3(5.0, -15.0, -85.0), 5.0, glm::vec3(0.5, 0.1, 0.4));
    //--Add the above to the list of scene objects.
    sceneObjects.push_back(sphererefract);
    
    
    Cylinder *cyclinder = new Cylinder(glm::vec3(8.0, -15.0, -100.0),2.0,10.0, glm::vec3(0.5, 0.1, 0.4));
    //--Add the above to the list of scene objects.
    sceneObjects.push_back(cyclinder);

}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer hdo27");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
