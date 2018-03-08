/*****************************************************************************/
// File adapted from HW2 to read input information
/*****************************************************************************/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include "Transform.h" 
using namespace std;
#include "variables.h" 
#include "readfile.h"

// You may not need to use the following two functions, but it is provided
// here for convenience

// The function below applies the appropriate transform to a 4-vector
void matransform(stack<mat4> &transfstack, float* values) 
{
    mat4 transform = transfstack.top(); 
    vec4 valvec = vec4(values[0],values[1],values[2],values[3]); 
    vec4 newval = transform * valvec; 
    for (int i = 0; i < 4; i++) values[i] = newval[i]; 
}

void rightmultiply(const mat4 & M, stack<mat4> &transfstack) 
{
    mat4 &T = transfstack.top(); 
    T = T * M; 
}

// Function to read the input data values
bool readvals(stringstream &s, const int numvals, float* values) 
{
    for (int i = 0; i < numvals; i++) {
        s >> values[i]; 
        if (s.fail()) {
            cout << "Failed reading value " << i << " will skip\n"; 
            return false;
        }
    }
	return true; 
}

void readfile(const char* filename) 
{
    string str, cmd; 
    ifstream in;
    in.open(filename); 
    if (in.is_open()) {
	std::cout << "Reading file " << filename << "\n";
        stack <mat4> transfstack; 
        transfstack.push(mat4(1.0));  // identity
        
	// Set some defaults
	ambient=vec3(0.2);
	attenuation=vec3(0.);
	attenuation[0]=1.0;
	outfile = "save.png";
	maxdepth = 5;

        getline (in, str); 
        while (in) {
            if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
                // Rule out comment and blank lines 

                stringstream s(str);
                s >> cmd; 
                int i; 
                float values[10]; // Position and color for light, colors for others
                                    // Up to 10 params for cameras.  
                bool validinput; // Validity of input 

                // Process the light, add it to database.
                // Lighting Command
                if (cmd == "directional" || cmd == "point") {
			//cout << "Light number: " << numused << "\n";
                    	if (numused == numLights) { // No more Lights 
                        cerr << "Reached Maximum Number of Lights " << numused << " Will ignore further lights\n";
			}
			else {
                        validinput = readvals(s, 6, values); // Position/color for lts.
                        if (validinput) {
			   light * lit = &(lights[numused]);
			   for (int i = 0; i < 3; i++) {
			      (lit->xyz)[i]=values[i];
			      (lit->color)[i]=values[i+3];
			   }
			   if (cmd == "directional"){
				   lit->dir = 1;
			   }
			   else{
				   lit->dir = 0;
			   }
			   ++numused; 
                        }
                    }
                }
		else if (cmd == "output") {
		   s >> outfile;
		}

                // Material Commands 
                else if (cmd == "ambient") {
                    validinput = readvals(s, 3, values);
                    if (validinput) {
                        for (int i = 0; i < 3; i++) {
                            ambient[i] = values[i];
                        }
                    }
                } else if (cmd == "diffuse") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        for (int i = 0; i < 3; i++) {
                            diffuse[i] = values[i]; 
                        }
                    }
                } else if (cmd == "specular") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        for (int i = 0; i < 3; i++) {
                            specular[i] = values[i]; 
                        }
                    }
                } else if (cmd == "emission") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        for (int i = 0; i < 3; i++) {
                            emission[i] = values[i]; 
                        }
                    }
                } else if (cmd == "shininess") {
                    validinput = readvals(s, 1, values); 
                    if (validinput) {
                        shininess = values[0]; 
                    }
                } else if (cmd == "attenuation") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
			for (int i = 0; i < 3; i++){
                           attenuation[i] = values[i]; 
			}
                    }
                } else if (cmd == "size") {
                    validinput = readvals(s,2,values); 
                    if (validinput) { 
                        w = (int) values[0]; h = (int) values[1]; 
                    } 
                } else if (cmd == "camera") {
                    validinput = readvals(s,10,values); 
		    // 10 values eye cen up fov
                    if (validinput) {
			eyeinit = vec3(values[0], values[1], values[2]);
			center = vec3(values[3], values[4], values[5]);
			upinit = vec3(values[6], values[7], values[8]);
			fovy = values[9];
                    }
                } else if (cmd == "maxdepth") {
		    validinput = readvals(s, 1, values);
                    if (validinput) {
                        maxdepth = values[0];
                    }
		}

		//Geometry
		else if (cmd == "maxverts"){
			validinput = readvals(s,1, values);
			if (validinput) {
				maxverts = values[0];
			}
		}
		else if (cmd == "vertex"){
			if (numverts == maxvertices) { 
                        	cerr << "Reached Maximum Number of Vertices " << numverts << " Will ignore further vertices\n";
                   	}
		       	else {
                           validinput = readvals(s, 3, values); 
			   if (validinput) {
				vertex * ver = &(vertices[numverts]);
			   	for (int i=0;i<3;i++){
					(ver->pos)[i] = values[i];
				}
			   }	
			}
			++numverts;
		}
		else if (cmd == "tri" || cmd == "sphere"){
		   if (numobjects == maxobjects) { 
                      cerr << "Reached Maximum Number of Objects " << numobjects << " Will ignore further objects\n";
                   }
		   else {
                      if (cmd == "tri") validinput = readvals(s, 3, values); 
		      else if(cmd == "sphere") validinput = readvals(s, 4, values);
		      if (validinput) {
		         object *obj = &(objects[numobjects]);
			 for (i = 0; i < 3; i++) {
                                (obj->ambient)[i] = ambient[i];
                                (obj->diffuse)[i] = diffuse[i];
                                (obj->specular)[i] = specular[i];
                                (obj->emission)[i] = emission[i];
                         }
                         obj->shininess = shininess;
			 obj->transform = transfstack.top();

			 if (cmd == "tri") {
			    obj->type = triangle;
			    for (int i=0;i<3;i++){
			       (obj->vert)[i] = values[i];
			    }	
			 } else if (cmd == "sphere") {
			    obj->type = sphere;
			    for (int i=0;i<3;i++){
                               (obj->pos)[i] = values[i];
                            }
			    obj->size = values[3];
			}
			++numobjects;
		     }
		   }
		}

                else if (cmd == "translate") {
                    validinput = readvals(s,3,values); 
                    if (validinput) {
			rightmultiply(Transform::translate(values[0],values[1],values[2]), transfstack);
                    }
                }
                else if (cmd == "scale") {
                    validinput = readvals(s,3,values); 
                    if (validinput) {
			rightmultiply(Transform::scale(values[0],values[1],values[2]), transfstack);
                    }
                }
                else if (cmd == "rotate") {
                    validinput = readvals(s,4,values); 
                    if (validinput) {
			vec3 axist(values[0], values[1], values[2]);
			mat3 R = Transform::rotate(values[3], axist);
			mat4 R4(1.0f);
			for (int i = 0; i < 3; i++) 
			    R4[i] = vec4(R[i], 0.0f);
			rightmultiply(R4, transfstack);
                    }
                }

                // basic push/pop code for matrix stacks
                else if (cmd == "pushTransform") {
                    transfstack.push(transfstack.top()); 
                } else if (cmd == "popTransform") {
                    if (transfstack.size() <= 1) {
                        cerr << "Stack has no elements.  Cannot Pop\n"; 
                    } else {
                        transfstack.pop(); 
                    }
                }
                else {
                    cerr << "Unknown Command: " << cmd << " Skipping \n"; 
                }
            }
            getline (in, str); 
        }

        // Set up initial position for eye, up and amount
        eye = eyeinit; 
        up = upinit; 

    } else {
        cerr << "Unable to Open Input Data File " << filename << "\n"; 
        throw 2; 
    }
}
