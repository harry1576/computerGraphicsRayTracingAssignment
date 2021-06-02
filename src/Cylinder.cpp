/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Sphere's intersection method.  The input is a ray (pos, dir). 
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float a = (dir.x * dir.x) + (dir.z * dir.z);
    float b = 2*(dir.x * (posn.x - center.x) + (dir.z*(posn.z-center.z)));
	float c = (posn.x - center.x) * (posn.x-center.x) + (posn.z-center.z) * (posn.z-center.z) - (radius*radius); 
	
	
	float delta = (b*b) - 4*(a*c);
   
	if(fabs(delta) < 0.001) return -1.0; 
    if(delta < 0.0) return -1.0;
	
	
    float t1 = (-b - sqrt(delta))/(2*a);
    float t2 = (-b + sqrt(delta))/(2*a);
	float smallest;
	float biggest;
	
	if(t1 > t2){smallest = t2; biggest = t1;}
	else{smallest = t1; biggest = t2;}

	
	if(center.y + height >= posn.y + (dir.y * smallest) && center.y <= posn.y + (dir.y * smallest) ){return smallest;}
	
	else if(center.y + height >= posn.y + (dir.y * biggest) && center.y <= posn.y + (dir.y * biggest) && posn.y >= center.y + height){return (center.y + height - posn.y)/dir.y;}
	


	else{ return -1;} 


	
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
	glm::vec3 n;
	n.x = p.x - center.x;
	n.y = 0;
	n.z = p.z - center.z;
    
    n = glm::normalize(n);
    return n;
}

