/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The sphere class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Sphere's intersection method.  The input is a ray (pos, dir). 
*/
float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{	
	//float R = (radius/height)*(height - center.y - posn.y);
	
	float D = powf(radius/height,2);
	
	float A = posn.x - center.x;
	float B = posn.z - center.z;
	float C = height - posn.y + center.y;
	
	
    float a = (dir.x * dir.x) + (dir.z * dir.z) - (D * (dir.y * dir.y));
    float b = (2 * A * dir.x) + (2 * B * dir.z) + (2 * D * C * dir.y)  ;
	float c = powf(A,2) + powf(B,2) - D * powf(C,2) ; 
  
	float delta = (b*b) - 4*(a*c);
   
	if(fabs(delta) < 0.001) return -1.0; 
    if(delta < 0.0) return -1.0;
	
	
    float t1 = (-b - sqrt(delta))/(2*a);
    float t2 = (-b + sqrt(delta))/(2*a);
	float smallest;
	
	
	if(t1 > t2){smallest = t2;}
	else{smallest = t1;}
	
	if(center.y + height > posn.y + (dir.y * smallest) && center.y < posn.y + (dir.y * smallest) ){return smallest;}
	else{ return -1;} 

	
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the sphere.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
	glm::vec3 n;
	
	float theta = atan(radius/height);
	float alpha = atan((p.x-center.x)/(p.z-center.z));
	
	n.x = sin(alpha) * cos(theta);
	n.y = sin(theta);
	n.z = cos(alpha) * cos(theta);

    n = glm::normalize(n);
    return n;
}

