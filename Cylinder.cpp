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
	
	
	float t;
    float t1 = (-b - sqrt(delta))/(2*a);
    float t2 = (-b + sqrt(delta))/(2*a);
   
    if(fabs(t1) < 0.001 )
    {
        if (t2 > 0) t = t2;
        else t1 = -1.0;
    }
    if(fabs(t2) < 0.001 ) t2 = -1.0;
	
	if(t1 == -1 && t2 == -1)
	{ return -1;}
	
	if(t1 > t2)
	{
		if(posn.y + (dir.y * t1) > height)
			{t = -1;}
			else{ t = t1;}
		
	}
	else
	{		if(posn.y + (dir.y * t2) > height)
			{t = -1;}
			else{ t = t2;}
	}
	
	

	
	
	
	return t;
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

