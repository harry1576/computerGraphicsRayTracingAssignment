/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Plane.h"
#include <math.h>


/**
* Checks if a point pt is inside the current polygon
* Implement a point inclusion test using
* member variables a, b, c, d.
*/
bool Plane::isInside(glm::vec3 pt)
{
    glm::vec3 n = normal(pt);

    glm::vec3 UaCrossVa = glm::vec3(0);
    glm::vec3 UbCrossVb = glm::vec3(0);
    glm::vec3 UcCrossVc = glm::vec3(0);
    glm::vec3 UdCrossVd = glm::vec3(0);

    UaCrossVa = glm::cross((b-a),(pt-a));
    UbCrossVb = glm::cross((c-b),(pt-b));
    UcCrossVc = glm::cross((d-c),(pt-c));
    UdCrossVd = glm::cross((a-d),(pt-d));

    int parameter1 = glm::dot(UaCrossVa,n);
    int parameter2 = glm::dot(UbCrossVb,n);
    int parameter3 = glm::dot(UcCrossVc,n);
    int parameter4 = glm::dot(UdCrossVd,n);


    if(parameter1 > 0 && parameter2 > 0 && parameter3 > 0 && parameter4 > 0)
    {
        return true;
    }
    else
    {
		return false;
    }


    //=== Complete this function ====

    return true;
}

/**
* Plane's intersection method.  The input is a ray (pos, dir).
*/
float Plane::intersect(glm::vec3 posn, glm::vec3 dir)
{
    glm::vec3 n = normal(posn);
    glm::vec3 vdif = a - posn;
    float vdotn = glm::dot(dir, n);
    if(fabs(vdotn) < 1.e-4) return -1;
    float t = glm::dot(vdif, n)/vdotn;
    if(fabs(t) < 0.0001) return -1;
    glm::vec3 q = posn + dir*t;
    if(isInside(q)) return t;
    else return -1;
}

/**
* Returns the unit normal vector at a given point.
* Compute the plane's normal vector using
* member variables a, b, c, d.
* The parameter pt is a dummy variable and is not used.
*/
glm::vec3 Plane::normal(glm::vec3 pt)
{
    glm::vec3 n = glm::vec3(0);


     n = glm::cross((b-a),(d-a));
     n = glm::normalize(n);
    //=== Complete this function ==== 

    return n;
}

int getColourAt(float s, float t)
{
	
	return 0;
};




