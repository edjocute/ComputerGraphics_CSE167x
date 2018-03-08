#include <iostream>
#include "Transform.h"

using namespace std;
#include "scene.h"
#include <FreeImage.h>
#include <omp.h>

//Main rendering function
film Raytrace::render() {
      Scene scene = Scene(); //preprocess scene information
      Image = film(h,matrix(w)); //set up blank canvas
      #pragma omp parallel for
      for (int i=0; i<h; i++){
         for (int j=0; j<w; j++){
		
            Ray ray = generateRay(scene,i,j);
	    IntersectionInfo info = Intersect(ray);
	    int nbounce=0;
            Image[i][j] = FindColor(info,nbounce);
         }
      }
      return Image;
}


// Generate ray given (i,j) index
Ray Raytrace::generateRay(Scene s, int i, int j){
   Ray ray;
   double fw = (double) w;
   double fh = (double) h;
   double alpha = tan(s.fovyr/2.) *fw/fh * (j+0.5-fw/2)/(fw/2);
   double beta = tan(s.fovyr/2.) * (fh/2-i-0.5)/(fh/2);

   vec3 dir = alpha*s.uvec + beta*s.vvec - s.wvec;
   ray.pos = eye; 
   ray.dir = glm::normalize(dir);
   return ray;
}

// For a given ray, we:
//  - test for shadowing
//  - find primary color
//  - trace reflected rays and add their colors recursively
// To terminate recursion, we pass with this function nbounce
// to track the number of reflections that have occured and 
// terminate recursion when nbounce < maxdepth
vec3 Raytrace::FindColor(IntersectionInfo info, int &nbounce){
   nbounce++;
   vec3 color=vec3(0.0f);

   if (info.hitobject >= 0){
      object *obj = &objects[info.hitobject];
      IntersectionInfo shadow;
      Ray shadowray, reflectedray;
      vec3 pos = info.pos;
      vec3 lightdir, tempc;
      double d, att = 1.;

      color += obj->ambient + obj->emission;
      vec3 normal = info.normal;
      for (int n = 0; n < numused; n++){
	 vec3 lightcolor = lights[n].color;
	 if (lights[n].dir){
            lightdir = lights[n].xyz; //directional light
	    att = 1.;
	 } else {
	    lightdir = lights[n].xyz - pos; //point light
	    d = glm::distance(lights[n].xyz,pos);
	    att = attenuation[0]+attenuation[1]*d+attenuation[2]*pow(d,2);
	 }
	 lightdir= glm::normalize(lightdir);

	 shadowray.dir = lightdir;
	 shadowray.pos = pos+0.001*normal;
	 shadow = Intersect(shadowray);
	 //For point source, the light can be in between objects,
	 //so check only obstructions between light source and object.
	 //Check if hit object is in front (d<-1) or behind (d>0) of light
	 if (lights[n].dir) {
		 d = -1.; //doesn't apply for direction src
	 } else if (!lights[n].dir) {
	     d=glm::distance(shadowray.pos,shadow.pos) -
		 glm::distance(shadowray.pos,lights[n].xyz);
	 }
	 if (shadow.hitobject < 0 || d > 0) {
	   //the eye direction is given by the ray starting position
	   //which is required for reflected rays	
	   vec3 eyedirn = glm::normalize(info.rpos-pos);
	   vec3 halfvec = glm::normalize(lightdir+eyedirn);
	   double nDotL = dot(normal,lightdir);
	   double nDotH = dot(normal,halfvec);
           tempc = obj->diffuse * lightcolor * max(nDotL, 0.0);
	   tempc += obj->specular * lightcolor * pow(max(nDotH,0.0),obj->shininess);
	   tempc /= att;
	   }
	   color += tempc;
      }

      //reflected ray:
      reflectedray.pos = pos+0.001*normal;
      reflectedray.dir = glm::reflect(info.rdir,normal);
      info = Intersect(reflectedray);
      while (nbounce <= maxdepth){
         color+=obj->specular*FindColor(info,nbounce);
      }
   return color;
   }
   else return vec3(0.);
}

vec3 Raytrace::findNormal(IntersectionInfo info){
   int obj = info.hitobject;
   vec3 normal;
   if (objects[obj].type == sphere){
      return glm::normalize( info.pos- objects[obj].pos);
   }
   else if (objects[obj].type == triangle){
      return objects[obj].normal;
      }
}

void Raytrace::saveImage(){
  FreeImage_Initialise();
  //FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, w, h, w * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, true);
  FIBITMAP *bitmap  = FreeImage_Allocate(w,h,24) ;
  RGBQUAD  color;
  float m = 1.;
  if (!bitmap) exit(1) ;
  for (int i=0; i<h; i++){
    for (int j=0;  j<w; j++){
      color.rgbRed = min(Image[h-i-1][j][0],m) * 255.0 ;
      color.rgbGreen = min(Image[h-i-1][j][1],m) * 255.0;
      color.rgbBlue  = min(Image[h-i-1][j][2],m) * 255.0 ;
      FreeImage_SetPixelColor(bitmap, j, i ,&color) ;
    }
  }
  FreeImage_Save(FIF_PNG, bitmap, outfile.c_str(), 0);
  FreeImage_DeInitialise();
}

//Test ray-sphere intersection
double Raytrace::IntersectSphere(Ray r, object sph){
  vec3 dir = r.dir;
  vec3 pos = r.pos;

  double t = -1;
  double a = dot(dir,dir);
  double b = 2*dot(dir,pos-sph.pos);
  double c = dot(pos-sph.pos,pos-sph.pos)-pow(sph.size,2);
  double det = pow(b,2) - 4*a*c;

  if (det < 0){
     return -1;
  }
  else if (det < 1e-10){
     t = -b/2./a;
  }
  else{
     double t1 = (-b + sqrt(det))/2./a;
     double t2 = (-b - sqrt(det))/2./a;
     //cout << t1 << " " << t2 << "\n";
     
     if ((t1 >= 0) and (t2 >= 0)){
	t = min(t1,t2);
     }
     else if ((t1 >= 0) and (t2 <= 0)){
	t = t1;
     }
     else if ((t1 <= 0) and (t2 >= 0)){
	t = t2;
     }
  }
  if ((t>=r.t_min) & (t<=r.t_max)) return t;
  else return -1;
}


//Test ray-triangle intersection
double Raytrace::IntersectTriangle(Ray r, object tri){
   double t = -1;
   vec3 A = (tri.vecpos[0]);
   vec3 B = (tri.vecpos[1]);
   vec3 C = (tri.vecpos[2]);

   vec3 normal = glm::cross(C-A,B-A);
   normal = glm::normalize(normal);

   //Check if ray and normal are parallel
   if (fabs(dot(normal,r.dir)) < 1e-6) return -1;

   /* Check ray-plane intersection first */
   t = dot(A-r.pos,normal)/dot(r.dir,normal);
   if (t < 0) return -1;

   vec3 P = r.pos + t * r.dir; //intersection pt with plane

   //1st edge
   vec3 edge = B-A;
   vec3 pv = P-A;
   vec3 CR = glm::cross(pv,edge);
   if (dot(normal,CR) < 0) {
	return -1;
   }

   edge = C-B;
   pv = P-B;
   CR = glm::cross(pv,edge);
   if (dot(normal,CR) < 0){
	   return -1;
   }

   edge = A-C;
   pv = P-C;
   CR = glm::cross(pv,edge);
   if (dot(normal,CR) < 0){
	   return -1;
   }
   return t;
}

// Loop through objects and test intersection depend on whether
// object is triangle or sphere.
// For spheres, we:
//  - inverse transform the ray
//  - test for intersection, find intersection point and normal in
//    transformed frame
//  - transform intersection point and normal back into normal frame
// Returns information about intersection needed for color,
// shadowing and reflection
IntersectionInfo Raytrace::Intersect(Ray ray) { 
  IntersectionInfo info;
  vec3 normal;
  Ray rayt;

  double mindist = 1e10; 
  int hitobject = -1; 
  double t = -1;
  for (int i = 0; i<numobjects; i++){  // Find closest intersection; test all objects
     info.rdir = ray.dir;
     info.rpos = ray.pos;
     object *obj = &(objects[i]);
     if (obj->type == triangle) {
        t = IntersectTriangle(ray, objects[i]); 
     } else if (obj->type == sphere) {
  	rayt.dir = (mat3(obj->invtransform)*ray.dir);
  	rayt.pos = vec3(obj->invtransform*vec4(ray.pos,1.0));
	t = IntersectSphere(rayt, objects[i]);
     }
     if (t > 0 && t < mindist){  // closer than previous closest object 
        mindist = t; 
     	hitobject = i; 
     }
  }
 
  if (mindist > ray.t_min && mindist < ray.t_max){
     info.hitobject=hitobject;
     info.t=mindist;
     object *obj = &(objects[hitobject]);
     if (obj->type == sphere) {
        rayt.dir = (mat3(obj->invtransform)*ray.dir);
	rayt.pos = vec3(obj->invtransform*vec4(ray.pos,1.0));
        info.pos=rayt.pos+mindist*rayt.dir;

	normal = normalize( info.pos- objects[hitobject].pos);
	info.normal =normalize(mat3(transpose(obj->invtransform))*normal);
        info.pos = vec3(obj->transform*vec4(info.pos,1.0));
     } else {
        info.pos=ray.pos+mindist*ray.dir;
        info.normal = objects[hitobject].normal;
     }
  } else 
     info.hitobject=-1;
     info.t=-1;
  return info;
}

//Calculate vectors of coordinate frame
void Scene::createCoordinateframe() {
	cout << "creating coordinate frame" << "\n";
	fovyr = glm::radians(fovy);
	fovxr = (double) w / (double) h * fovyr;
        wvec = glm::normalize(eyeinit - center);
        uvec = glm::cross(upinit,wvec);
        uvec = glm::normalize(uvec);
        vvec = glm::cross(wvec,uvec);
	//cout << fovx << " " << fovy << "\n";
	cout << glm::to_string(eyeinit) << "\n";
	cout << glm::to_string(center) << "\n";
	cout << glm::to_string(upinit) << "\n";
	cout << glm::to_string(wvec) << "\n";
	cout << glm::to_string(uvec) << "\n";
	cout << glm::to_string(vvec) << "\n";
}


//Takes into account transformations by:
// - Transforming vertices of triangles directly and storing the normal
// - Storing the inverse transformation to minimize calculation
//For spheres, transformation is done during intersection test
void Scene::transformTriangles(){
   vec3 normal;
   vec4 A[3];
   for (int i = 0; i < numobjects; i++){
      object *obj = &objects[i];
      if (obj->type == triangle){
         //cout << "transforming\n";
	 for (int j = 0; j < 3; j++){
            A[j] = vec4(vertices[obj->vert[j]].pos,1.0);
	    A[j] = obj->transform * A[j];
	    (obj->vecpos)[j] = vec3(A[j]);
	 }

         normal = glm::cross(vec3(A[2])-vec3(A[0]),vec3(A[1])-vec3(A[0]));
         normal = glm::normalize(normal);
         obj->normal = -normal;
	 //cout << "norm:" << to_string(obj->normal) << "\n";
      }
      else {
         obj->invtransform = inverse(obj->transform);
      }
   }
}
