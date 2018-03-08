#include <vector>
#include <string>
#define GLM_SWIZZLE_XYZ
#include <glm/glm.hpp>
#include "variables.h"


typedef std::vector<double> vec;
typedef std::vector<vec3> matrix;
typedef std::vector<matrix> film;

struct Ray{ 
   vec3 pos,dir; 
   const float t_min=0,t_max=200; 
};

struct IntersectionInfo {
   int hitobject;
   double t;
   vec3 pos;
   vec3 rdir;
   vec3 rpos;
   vec3 normal;
};

class Scene{
   public:
      double fovxr,fovyr;
      vec3 wvec, uvec ,vvec;


   private:
      void createCoordinateframe();
      void transformTriangles();

   public:
      Scene(){
         createCoordinateframe();
	 transformTriangles();
      };
};

class Raytrace{
   film Image;

   Ray generateRay(Scene s, int i,int j);
   vec3 FindColor(IntersectionInfo info, int &nbounce);
   vec3 findNormal(IntersectionInfo info);
   double IntersectSphere(Ray r, object sph);
   double IntersectTriangle(Ray r, object tri);
   IntersectionInfo Intersect(Ray r);

   public:
      film render();
      void saveImage();
      //Raytrace(){render();};
};
