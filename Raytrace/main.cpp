#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include "Transform.h"

using namespace std;
#define MAINPROGRAM
#include "scene.h"
#include "readfile.h" // prototypes for readfile.cpp

int main(int argc, char* argv[]) {
  if ( argc < 2 ){
    cout<<"usage: "<< argv[0] <<" <filename>\n";
    return 1;
  }

  readfile(argv[1]) ;
  cout << "Ok" << "\n";
  for (int i=0;i<numused;i++){
     cout << "Light " << i << "\n";
     cout << "  pos:"+glm::to_string(lights[i].xyz) << "\n";
     cout << "  color:"+glm::to_string(lights[i].color) << "\n";
  }
  /*for (int i=0;i<numobjects;i++){
      cout << "Object " << i << " : " << objects[i].type << ".";
      if (objects[i].type == triangle) {
         for (int j=0;j<3;j++){
            cout << objects[i].vert[j] << "";
         }
      } else if (objects[i].type == sphere) {
	  for (int j=0;j<3;j++){
            cout << objects[i].pos[j] << " ";
          }
	  cout << objects[i].size;
      }
         cout << "\n";
	 cout << "  amb:"+glm::to_string(objects[i].ambient) << "\n";
	 cout << "  dif:"+glm::to_string(objects[i].diffuse) << "\n";
	 cout << "  spe:"+glm::to_string(objects[i].specular) << "\n";
	 //cout << "  tra:"+glm::to_string(objects[i].transform) << "\n";
  }*/
  cout << "h=" << h << " ,w=" << w << "\n";

  Raytrace R = Raytrace();
  film I = R.render();
  R.saveImage();

  return 0;
}
