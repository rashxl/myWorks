/*
 * maths.cpp
 *
 *  Created on: Apr 4, 2017
 *      Author: rash
 */

#include <maya/MItMeshVertex.h>
#include <maya/MMatrix.h>
#include <maya/MPoint.h>
#include <maya/MGlobal.h>
//#include <cmath>
#include <Windows.h>
//namespace std;


#ifndef EPSILON
#define EPSILON 0.0001
#endif

#ifndef RADTODEG
#define RADTODEG 57.29577951
#endif

#ifndef DEGTORAD
#define DEGTORAD 0.01745329
#endif



// define constants

#ifndef PI
#define PI 3.14159265
#endif

#ifndef HALFPI
#define HALFPI 1.570795
#endif

#ifndef TWOPI
#define TWOPI 6.28318531
#endif


// ################################################################################################
// ############################################ MATH PROCS ########################################
// ################################################################################################


/*
 * noZero() - DIVIDE BY ZERO CHECK.
 */
float noZero(double value){
	if(value == 0){
		return 0.0001;
	}else{
	return value;
	}
}


// ---------------------------------------------------------------------------

/*
 * degreeToRadians() - converting degrees to radians
 * 					   radians = ( degrees * pi ) / 180
 */
float degreeToRadians(float &deg){
	return (deg*PI)/180;
}

// ---------------------------------------------------------------------------

/*
 * toRad() - converting degrees to radians
 * 			 degrees=( radians * 180 ) / pi
 */
float radiansToDegree(float &rad){
	return (rad*180)/PI;
}

// ---------------------------------------------------------------------------

/*
 * dotP() - DOT PRODUCT.
 */
float dotP(MVector A, MVector B){
	return A.x*B.x+A.y*B.y+A.z*B.z;
}

// ---------------------------------------------------------------------------

/*
 * crossProd() - cross product.
 */
MVector crossProd(MVector a, MVector b){
	a.normalize();
	b.normalize();
	MVector result;
	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
	return result;
}

// ---------------------------------------------------------------------------


/*
 * cartesianToSpherical() - Convert an XYZ value into a spherical coord.
 */

void cartesianToSpherical(const double &x, const double &y, const double &z,
						double &r, double &phi, double &theta)
{
	r = sqrt( x*x + y*y + z*z) ;
	//azimuth
	phi = atan2( x , z ) ;
	theta = acos( y / r );
}


//void cartesianToSpherical(const double &x, const double &y, const double &z,
//						double &r, double &phi, double &theta)
//{
//	r = sqrt( x*x + y*y + z*z) ;
//	//azimuth
//	phi = atan( y / x ) ;
//	theta = acos( z / (sqrt(r)) );
//}
//
//
//void cartesianToSphericalNew(const double &x, const double &y, const double &z,
//						double &r, double &phi, double &theta)
//{
//	r = sqrt( x*x + y*y + z*z) ;
//	//azimuth
//	phi = atan2( y, z ) ;
//	theta = acos( x / r );
//}


// ---------------------------------------------------------------------------

/*
 * sphericalToCartesian() - Convert an spherical value into a cart coord.
 */

void sphericalToCartesian(const double &r, const double &phi, const double &theta,
						double &x, double &y, double &z)
{
	x = r * sin(theta) * sin(phi) ;
	z = r * sin(theta) * cos(phi) ;
	y = r * cos(theta) ;
}


//void sphericalToCartesian(const double &r, const double &phi, const double &theta,
//						double &x, double &y, double &z)
//{
//	x = r * sin(theta) * cos(phi) ;
//	y = r * sin(theta) * sin(phi) ;
//	z = r * cos(theta) ;
//}
//
//void sphericalToCartesianNew(const double &r, const double &phi, const double &theta,
//						double &x, double &y, double &z)
//{
//	x = r * cos(theta) ;
//	y = r * sin(theta);
//	z = y * cos(phi) ;
//	y *= sin(phi);
//}

// ---------------------------------------------------------------------------


/*
 * cartesianToCylindrical() - Convert an XYZ value into a cylindrical coord.
 */
void cartesianToCylindrical(const double &x, const double &y, const double &z,
						double &r, double &theta, double &zc)
{
	r = sqrt( x*x + y*y) ;
	theta = atan( y / x ) ;
	zc = z ;
}

// ---------------------------------------------------------------------------

/*
 * cylindricalToCartesian() - Convert an cyl value into a cart coord.
 */
void cylindricalToCartesian(const double &r, const double &theta, const double &zc,
						double &x, double &y, double &z)
{
	x = r * cos(theta) ;
	y = r * sin(theta) ;
	z = zc ;
}

// ---------------------------------------------------------------------------


/*
 * cartesianToPolar() - Convert an XY value into a polar coord.
 */
void cartesianToPolar(const double &x, const double &y,
						double &r, double &theta)
{
	r = sqrt( x*x + y*y) ;
	theta = atan( y / x ) ;
}

// ---------------------------------------------------------------------------

/*
 * polarToCartesian() - Convert an polar value into a cart coord.
 */
void polarToCartesian(const double &r, const double &theta,
						double &x, double &y)
{
	x = r * cos(theta) ;
	y = r * sin(theta) ;
}

// ---------------------------------------------------------------------------

/*
 * arclength() - Given radius and degrees apart, get arclen.
 */
double arclength(const double &r, const double &theta)
{
	return (r * theta) ;
}

// ---------------------------------------------------------------------------

/*
 * cartesianToSphericalUV() - x,y,z should be a normalize vector.
 */
void cartesianToSphericalUV(const double &x, const double &y, const double &z,
							double &u, double &v)
{
	// Make X go from 0-1 as we go ALL the way around.  ie: instead of just pure
	// X being -1 to 1.  We want to change to so -1=1 on right side,
	// And if z > 0  then we use x as 0-1 and if <= -1 to 0
	//
	double dx ;
	if (z >= 0)
		{
		dx = (x + 1.0) / 2.0 ;	// Convert from -1...1  to 0...1
		}
	else
		{
		dx = ((x + 1.0) / 2.0) * -1.0 ;	// Convert from 1...-1  to -1...0
		}

	u = asin( dx ) / PI + 0.5 ;
	v = asin( y ) / PI + 0.5 ;

}


// ---------------------------------------------------------------------------

/*
 * dist2D() - Computes distance/length of a 2D vector
 */
double dist2D(const double &x, const double &y)
{
	return ( sqrt(x*x + y*y) );
}

// ---------------------------------------------------------------------------

/*
 * smoothStep() - Take a value from 0-1 and instead of having it be linear,
 *			make it ease out and then in from the 0 and 1 locations.
 */
double smoothStep(const double &dVal)
{
	// x^2*(3-2x)
	double dRet = dVal * dVal * (3.0 - (2.0 * dVal) );
	return dRet ;
}

// ---------------------------------------------------------------------------

/*
 * smoothGaussian() - Take a value from 0-1 and instead of having it be linear,
 *			make it ease out and then in from the 0 and 1 locations.  This has more of a longer
 *			ease than a smoothstep and so a faster falloff in the middle.
 */
double smoothGaussian(const double &dVal)
{
	// 1.0 - exp( (-1 * x^2 ) / (2*sigma^2) )

	double dSigma = 1.0 ;		// Must be >0.   As drops to zero, higher value one lasts longer.

	double dRet = (1.0 - exp( -1.0 * (dVal*dVal) * 10.0  / (2.0 * dSigma*dSigma)  ) );	// RBF
	return dRet ;
}

// ---------------------------------------------------------------------------

/*
 * getAngle() - take two vectors, and give angle
 */
float getAngle(MVector A, MVector B){
	A.normalize();
	B.normalize();
	return acos(dotP(A,B));

}

// ---------------------------------------------------------------------------

/*
 * softnessMax() - Take three arguments, first arg a value, second max of first value and third scale of softness.
 * 					This func. give softnes max value
 */
float softnessMax(float val, float max, float softness){

	softness /= (sin(45*PI/180) * tan(22.5*PI/180));

	if(val > (max+(softness * tan(22.5*PI/180) ))){
		return max;
	}else if(val < (max-(softness * (sin(45*PI/180) - tan(22.5*PI/180) )))){
		return val;
	}else{
		return max + (softness * cos((asin((tan(22.5*PI/180) + ((max - val)/softness)))*180/PI)*PI/180)) - softness;
	}
}

// ---------------------------------------------------------------------------

/*
 * softnessMin() - Take three arguments, first arg a value, second max of first value and third scale of softness.
 * 					This func. give softnes min value
 */
float softnessMin(float val, float min, float softness){

	softness /= (sin(45*PI/180) * tan(22.5*PI/180));

	if(val < (min-(softness * tan(22.5*PI/180) ))){
		return min;
	}else if(val > (min+(softness * (sin(45*PI/180) - tan(22.5*PI/180) )))){
		return val;
	}else{
		return  min + (softness * -cos((asin((((min - val)/softness - tan(22.5*PI/180))))*180/PI)*PI/180)) + softness;
	}
}

// ---------------------------------------------------------------------------

/*
 * clamping() - Take three arguments, first arg a value, second max of first value and third min of value.
 * 					This func. give a clamping value
 */
float clamping(float val, float max, float min){

	return std::fmax(min, std::fmin(val, max));

}

// ---------------------------------------------------------------------------

/*
 * rescale() - Take five arguments, 1- arg a value, 2- old min of value, 3- old max of value, 4- new min value 5- new max value.
 * 					This func. give a clamping value between new Min and new Max value based of old value.
 */
float rescale(float value, float oldMin, float oldMax, float newMin, float newMax){

	return newMin + (((value - oldMin) / (oldMax - oldMin))*(newMax - newMin));

}


// ---------------------------------------------------------------------------

/*
 * GetDistByTwoMat() - Take two arguments, 1- Matrix 2- matrix.
 * 					This func. give a distance between two matrixes.
 */
float GetDistByTwoMat(MMatrix matA, MMatrix matB){
	return std::sqrt(std::pow(matA[3][0] - matB[3][0], 2)+std::pow(matA[3][1] - matB[3][1], 2)+std::pow(matA[3][2] - matB[3][2], 2));
}


// ---------------------------------------------------------------------------

/*
 * bezier3DMPoint() - bezier3D curve
 */

void bezier3DMPoint (MPoint& p1, MPoint& p2, MPoint& p3, MPoint& p4, double t, MPoint& outPoint) {
	double r = 1-t;
	outPoint.x = r*r*r*p1.x + 3*t*r*r*p2.x + 3*t*t*r*p3.x + t*t*t*p4.x;
	outPoint.y = r*r*r*p1.y + 3*t*r*r*p2.y + 3*t*t*r*p3.y + t*t*t*p4.y;
	outPoint.z = r*r*r*p1.z + 3*t*r*r*p2.z + 3*t*t*r*p3.z + t*t*t*p4.z;
}

// --------------------------------------------------------------------------



//----------------------------------------------------------------------------

/*
 * Barycentric() - Compute barycentric coordinates (u, v, w) for
 * 				   point p with respect to triangle (a, b, c).
 */
void Barycentric(MPoint p, MPoint a, MPoint b, MPoint c, float &u, float &v, float &w)
{
    MVector v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = dotP(v0, v0);
    float d01 = dotP(v0, v1);
    float d11 = dotP(v1, v1);
    float d20 = dotP(v2, v0);
    float d21 = dotP(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;
}


// ---------------------------------------------------------------------------

//void BarycentricB(MPoint p, MPoint a, MPoint b, MPoint c, MVector normal, float &u, float &v, float &w)
//{
//    MVector v01 = (b - a), v02 = (c - a), v03 = (b - p), v04 = (c - p), v05 = (a - p);
//	float areaABC = dotP( normal, crossProd( v01, v02 )  ) ;
//	float areaPBC = dotP( normal, crossProd( v03, v04 )  ) ;
//	float areaPCA = dotP( normal, crossProd( v04, v05 )  ) ;
//
//	u = areaPBC / areaABC ; // alpha
//	v = areaPCA / areaABC ; // beta
//	w = 1.0f - u - v ; // gamma
//}
//
//
//bool BarycentricB(MPoint p, MPoint a, MPoint b, MPoint c, float &u, float &v, float &w){
//
//	MVector uV = b-a;
//	MVector vV = c-a;
//	MVector wV = p-a;
//
//    MVector vCrossW = crossProd(vV, wV);
//    MVector vCrossU = crossProd(vV, uV);
//
//    // Test sign of r
//    if (dotP(vCrossW, vCrossU) < 0)
//        return false;
//
//    MVector uCrossW = crossProd(uV, wV);
//    MVector uCrossV = crossProd(uV, vV);
//
//    // Test sign of t
//    if (dotP(uCrossW, uCrossV) < 0)
//        return false;
//
//    float denom = uCrossV.length();
//    v = vCrossW.length() / denom;
//    u = uCrossW.length() / denom;
//    w = 1.0f - u - v;
//}

/*
void cartesianToSpherical(const double &x, const double &y, const double &z,
						double &r, double &phi, double &theta) ;
void sphericalToCartesian(const double &r, const double &phi, const double &theta,
						double &x, double &y, double &z) ;

void cartesianToCylindrical(const double &x, const double &y, const double &z,
						double &r, double &theta, double &zc) ;
void cylindricalToCartesian(const double &r, const double &theta, const double &zc,
						double &x, double &y, double &z) ;

void cartesianToPolar(const double &x, const double &y,
						double &r, double &theta) ;
void polarToCartesian(const double &r, const double &theta,
						double &x, double &y) ;

double arclength(const double &r, const double &theta) ;

void cartesianToSphericalUV(const double &x, const double &y, const double &z,
							double &u, double &v) ;

inline double dist2D(const double &x, const double &y) ;

 */
