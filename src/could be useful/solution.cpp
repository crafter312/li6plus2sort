#include "solution.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


using namespace std;

float solution::angle()
{
  float XYZ2 = pow(Xpos , 2) + pow(Ypos, 2) + pow(distTarget,2);
  theta = acos(distTarget/sqrt(XYZ2));
  phi = atan2(Ypos , Xpos);

  return theta;
}
//********************************************************
void solution::getMomentum()
{
  momentum = Kinematics.getMomentum(Ekin,mass);
  Mvect[0] = momentum*sin(theta)*cos(phi);
  Mvect[1] = momentum*sin(theta)*sin(phi);
  Mvect[2] = momentum*cos(theta);
  

  //scale = 1 einstein, 0 for newton
  energyTot = Ekin*Kinematics.scale + mass;
}
