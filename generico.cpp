#include <test/trajectory.h>
#include <iostream>
#include <numeric>
#include <ros/ros.h>
#include <geometry_msgs/PoseStamped.h>
#include <vector>
#include <algorithm>
#include <functional>

using namespace std;

//calcola i coefficienti del polinomio passante per n punti.

void Trajectory::set_coeff(Eigen::VectorXd x, Eigen::VectorXd y, Eigen::VectorXd z, Eigen::VectorXd t)
{
  
int row_degree=n+4;
int column_degree=n+4;  // ho n punti + 4 condizioni (2 acc e due vel) -1. grado polinomio (n+3)
int i=0; //indicatore righe
int j=0;  //indicatore colonne

// vettore che tiene conto delle posizioni,velocità e accelerazioni



//Definisco la matrice di Vandermonde 

MatrixXd V(row_degree,column_degree);


// parti della matrice con valori numerici

 for(i=0; i<n ; i++){

  V(i,0)=1;
     
  }
  
for(i=n; i<n+4 ; i++){

  V(i,0)=0;
     
  } 
  
for(i=n; i<n+2; i++){
 
  V(i,1)=1;
  
  }   
  
 for(i=n+2; i<n+4 ; i++){
 
  V(i,1)=0;
  
  }    
 
 for(i=n+2; i<n+4 ; i++){
 
  V(i,3)=2;
  
  }    
 
// Matrice relativa alle posizioni.

 for(i=0; i< n; i++){
   
   for(j=1; j < n+4; j++){ 
  
   } 
  V(i,j)=pow(t(i),j);
 } 
  
//Matrice relativa alle velocità
  
  for(i=n; i< n+2; i++){
   
   for(j=2; j < n+4; j++){ 
  
   } 
  V(i,j)=j*pow(t(i-n),j-1);
 } 
   
// Matrice relativa alle accelerazioni 
  
  for(i=n+2; i<n+4; i++){
   
   for(j=3; j < n+4; j++){ 
  
   } 
  V(i,j)=j*(j-1)*pow(t(i-n-2),j-2);
 } 
 
// calcolo dei coef del polinomio

cout << "Determinante matrice di Vandermonde " << V.determinant() << endl;
cout << "Inversa della matrice\n" << V.inverse() << endl;
   
coeff_x=V.inverse*x;
coeff_y=V.inverse*y;
coeff_z=V.inverse*z;
   
   
}

// calcola la posizione tramite coefficienti calcolati dalla set.

void Trajectory::get_points(double* x, double* y, double* z, double t)
{
  
  x = std::inner_product(std::begin(coeff_x),std::end(coeff_x), t.begin(), 0);
  y = std::inner_product(std::begin(coeff_y),std::end(coeff_y), t.begin(), 0);   
  z = std::inner_product(std::begin(coeff_z),std::end(coeff_z), t.begin(), 0);

}


struct Waypoint{
  
  double t;
  
} waypoint[3];


int main(int argc, char **argv)
{
  
Trajectory  traj;

traj.n=3;

double posx;
double posy;
double posz;
double t_i;
  
Eigen::VectorXd pf={0,0,0};
Eigen::VectorXd ps={0,0,5};
Eigen::VectorXd pt={2,0,5};
Eigen::VectorXd tempo(3);


Eigen::VectorXd x_vect={pf(0),ps(0),pt(0),0,0,0};
Eigen::VectorXd y_vect={pf(1),ps(1),pt(1),0,0,0};
Eigen::VectorXd z_vect={pf(2),ps(2),pt(2),0,0,0};


   ros::init(argc, argv, "trajectory_node");
   ros::NodeHandle nh;
  
   ROS_INFO("Test_info");
    
   ros::Publisher local_pos_pub = nh.advertise<geometry_msgs::PoseStamped>
            ("mavros/setpoint_position/local", 10);
    
   ros::Rate rate(20.0);

   geometry_msgs::PoseStamped pose;
    
   ros::Time last_request = ros::Time::now();
   double secs =last_request.toSec();
   
   waypoint.time[0]=secs;
   
   ros::Duration d;
   double d_secs = d.toSec();
   d_secs=0.1;
   
   waypoint.time[1]=secs+d_secs;
  
   traj.set_coeff(x_vect,y_vect,z_vect,tempo);
   traj.get_points(posx,posy,posz,t_i);
  
   
    
  
   while(ros::ok()){
     
   
    
  local_pos_pub.publish(pose); //pubblica nel topic
	
	ros::spinOnce();
        rate.sleep();
    }
    
   
   
   
    return 0;
 }
   

