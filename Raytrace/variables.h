/*****************************************************************************/
/* This is the program skeleton for homework 2 in CSE167 by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This is the basic include file for the global variables in the program.  
// Since all files need access to it, we define EXTERN as either blank or 
// extern, depending on if included in the main program or not.  

#include <vector>

#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif

EXTERN int amount; // The amount of rotation for each arrow press
EXTERN vec3 eye; // The (regularly updated) vector coordinates of the eye 
EXTERN vec3 up;  // The (regularly updated) vector coordinates of the up 

/*#ifdef MAINPROGRAM 
vec3 eyeinit(0.0,0.0,5.0) ; // Initial eye position, also for resets
vec3 upinit(0.0,1.0,0.0) ; // Initial up position, also for resets
vec3 center(0.0,0.0,0.0) ; // Center look at point 
int w = 500, h = 500 ; // width and height 
float fovy = 90.0 ; // For field of view
#else*/ 
EXTERN vec3 eyeinit ; 
EXTERN vec3 upinit ; 
EXTERN vec3 center ; 
EXTERN int w, h ; 
EXTERN float fovy ;
EXTERN string outfile;
EXTERN int maxdepth;
//#endif 

//EXTERN mat4 projection, modelview; // The mvp matrices
static enum {view, translate, scale} transop ; // which operation to transform 
enum shape {triangle, sphere} ;
EXTERN float sx, sy ; // the scale in x and y 
EXTERN float tx, ty ; // the translation in x and y

// Lighting parameter array, similar to that in the fragment shader
const int numLights = 10 ; 
EXTERN int numused ;                     // How many lights are used 
EXTERN struct light {
  vec3 xyz;
  vec3 color;
  bool dir;
} lights[numLights];


// Materials (read from file) 
// With multiple objects, these are colors for each.
EXTERN vec3 ambient; 
EXTERN vec3 diffuse; 
EXTERN vec3 specular; 
EXTERN vec3 emission; 
EXTERN float shininess; 
EXTERN vec3 attenuation;   

// For multiple objects, read from a file.  
const int maxobjects = 100000 ; 
EXTERN int numobjects ; 

//Geometry
const int maxvertices= 51000;
EXTERN int maxverts;
EXTERN int numverts;
EXTERN struct vertex {
  vec3 pos;
} vertices[maxvertices];
EXTERN struct object {
  shape type;
  vec3 ambient;
  vec3 diffuse; 
  vec3 specular;
  vec3 emission; 
  float shininess;
  mat4 transform;
  mat4 invtransform;

  //for spheres
  vec3 pos;
  float size;

  //for triangles
  int vert[3];
  vec3 vecpos[3];
  vec3 normal;
} objects[maxobjects];
