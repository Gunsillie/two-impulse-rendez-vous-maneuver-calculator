/************************************************
* TWO-IMPULSE RENDEZ-VOUS TRAJECTORY CALCULATOR *
* Author: Romain Pessia                         *
* Date: 2017/10/24                              *
* License: GNU GPLv3 (see repo)                 *
*************************************************/
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

int main(int argc, char* argv[])
{
	//__________DEFINING VARIABLES__________

    // Desired time of impact, in seconds from t=0
	double T = 3600.;

    // Desired step between two pieces of output data, in seconds
	double time_step = 60.;

	// Note: the user can give either of the two following variables a value of 0
	// Altitude of the target wrt. Earth, in meters
	double altitude_target = 400000.; 
    // Angular speed of the target around the Earth in its circular motion, in meters per second
	double w = 0.;                

	// Initial coordinates of the chaser wrt. the target, in meters
	double x0 = 100000.;
	double y0 = 100000.;
	double z0 = 100000.;

	// Gravitational parameter of the Earth, in m^3.s^(-2)
	double mu = 3.986*pow(10, 14);

	// Radius of the Earth, in meters
	double earth_radius = 6371000.;

	// __________CHECKING USER INPUT__________
	try
	{
		if (argc == 7)
		{
			T  = stod(argv[0]);
			time_step = stod(argv[1]);
			altitude_target = stod(argv[2]);
			w  = stod(argv[3]);
			x0 = stod(argv[4]);
			y0 = stod(argv[5]);
			z0 = stod(argv[6]);
		}
		else if (argc > 1)
		{
			throw("Wrong number of arguments (expected 7)");
		}

		if (T <= 0 || w < 0 || altitude_target < 0)
		{
			throw("Invalid value of w, T or altitude_target");
		}
	}
	catch (string error_message)
	{
		cout << "ERROR: " << error_message;
		return 1;
	}
	if (altitude_target == 0.)
	{
		// Calculating the altitude of the target from its angular speed
		altitude_target = pow(pow(w, 2) / mu, 1. / 3.) - earth_radius;
	}
	else
	{
		// Calculating the angular speed of the target from its altitude
		w = pow((3.986*pow(10, 14)) / pow(altitude_target + earth_radius, 3), 0.5);
	}

	// Calculating the matrices for the linearized Clohessy–Wiltshire equations
	double A[3][3] = { {1, 0, 6 * (sin(w*T) - w*T)},
					   {0, cos(w*T), 0},
					   {0, 0, 4 - 3 * cos(w*T)} };

	double B[3][3] = { { (4 / w)*sin(w*T) - 3 * T, 0, (2 / w)*(cos(w*T) - 1) },
						{0, (1 / w)*sin(w*T), 0},
						{(2 / w)*(1 - cos(w*T)) , 0, (1 / w)*sin(w*T) } };
	double determinant =
		+ B[0][0]*(B[1][1]*B[2][2] - B[2][1]*B[1][2])
		- B[0][1]*(B[1][0]*B[2][2] - B[1][2]*B[2][0])
		+ B[0][2]*(B[1][0]*B[2][1] - B[1][1]*B[2][0]);
	try
	{
		if (abs(determinant) < pow(10, -6))
		{
			throw("Could not find a solution for the initial velocity of the chaser.");
		}
	}
	catch (string error_message)
	{
		cout << error_message;
		return 1;
	}
	double invdet = 1 / determinant;
	double invB[3][3];
	invB[0][0] = (B[1][1]*B[2][2] - B[2][1]*B[1][2]) * invdet;
	invB[0][1] = (B[0][2]*B[2][1] - B[0][1]*B[2][2]) * invdet;
	invB[0][2] = (B[0][1]*B[1][2] - B[0][2]*B[1][1]) * invdet;
	invB[1][0] = (B[1][2]*B[2][0] - B[1][0]*B[2][2]) * invdet;
	invB[1][1] = (B[0][0]*B[2][2] - B[0][2]*B[2][0]) * invdet;
	invB[1][2] = (B[1][0]*B[0][2] - B[0][0]*B[1][2]) * invdet;
	invB[2][0] = (B[1][0]*B[2][1] - B[2][0]*B[1][1]) * invdet;
	invB[2][1] = (B[2][0]*B[0][1] - B[0][0]*B[2][1]) * invdet;
	invB[2][2] = (B[0][0]*B[1][1] - B[1][0]*B[0][1]) * invdet;

	// Calculating the initial velocity of the chaser
	double v0x =    - (invB[0][0] * A[0][0] + invB[0][1] * A[1][0] + invB[0][2] * A[2][0])*x0
				    - (invB[0][0] * A[0][1] + invB[0][1] * A[1][1] + invB[0][2] * A[2][1])*y0
					- (invB[0][0] * A[0][2] + invB[0][1] * A[1][2] + invB[0][2] * A[2][2])*z0;
	double v0y =    - (invB[1][0] * A[0][0] + invB[1][1] * A[1][0] + invB[1][2] * A[2][0])*x0
					- (invB[1][0] * A[0][1] + invB[1][1] * A[1][1] + invB[1][2] * A[2][1])*y0
					- (invB[1][0] * A[0][2] + invB[1][1] * A[1][2] + invB[1][2] * A[2][2])*z0;
	double v0z =    - (invB[2][0] * A[0][0] + invB[2][1] * A[1][0] + invB[2][2] * A[2][0])*x0
					- (invB[2][0] * A[0][1] + invB[2][1] * A[1][1] + invB[2][2] * A[2][1])*y0
			  		- (invB[2][0] * A[0][2] + invB[2][1] * A[1][2] + invB[2][2] * A[2][2])*z0;

	// Calculating the coordinates of the chaser at each time step from time 0 to time T
	uint32_t num_samples = static_cast<uint32_t>(T / time_step); 
	cout << "The number of samples is " << num_samples;
	vector<vector<double>> points;
	for (uint32_t i = 1; i < num_samples; ++i)// The sample at t=0 is not considered here
	{
		double Ai[3][3] = { { 1, 0, 6 * (sin(w*time_step*i) - w*time_step*i) },
		{ 0, cos(w*time_step*i), 0 },
		{ 0, 0, 4 - 3 * cos(w*time_step*i) } };

		double Bi[3][3] = { { (4 / w)*sin(w*time_step*i) - 3 * time_step*i, 0, (2 / w)*(cos(w*time_step*i) - 1) },
		{ 0, (1 / w)*sin(w*time_step*i), 0 },
		{ (2 / w)*(1 - cos(w*time_step*i)) , 0, (1 / w)*sin(w*time_step*i) } };
		double xi = Ai[0][0] * x0  + Ai[0][1] * y0  + Ai[0][2] * z0
				  + Bi[0][0] * v0x + Bi[0][1] * v0y + Bi[0][2] * v0z;
		double yi = Ai[1][0] * x0  + Ai[1][1] * y0  + Ai[1][2] * z0
				  + Bi[1][0] * v0x + Bi[1][1] * v0y + Bi[1][2] * v0z;
		double zi = Ai[2][0] * x0  + Ai[2][1] * y0  + Ai[2][2] * z0
				  + Bi[2][0] * v0x + Bi[2][1] * v0y + Bi[2][2] * v0z;
		vector<double> point = { xi, yi, zi };
		points.push_back(point);
	}

	//__________EXPORTING DATA__________

	// All (x,z) points
	ofstream output_file_x_z;
	output_file_x_z.open("output_tikz_x_z.txt");
	output_file_x_z << "(" << x0 << "," << z0 << ")";
	for (uint32_t i = 0; i < num_samples - 1; ++i)
	{
		output_file_x_z << "(" << points[i][0] << "," << points[i][2] << ")";
	}
	output_file_x_z << "(" << 0 << "," << 0 << ")" << endl;
	output_file_x_z.close();

	// All (x,y,z) points, for 3D plots
	ofstream output_file_x_y_z;
	output_file_x_y_z.open("output_tikz_x_y_z.txt");
	output_file_x_y_z << "(" << x0 << "," << y0 << "," << z0 << ")";
	for (uint32_t i = 0; i < num_samples - 1; ++i)	
	{
		output_file_x_y_z << "(" << points[i][0] << "," << points[i][1] << "," << points[i][2] << ")";
	}
	output_file_x_y_z << "(" << 0 << "," << 0 << "," << 0 << ")" << endl;
	output_file_x_y_z.close();

	// All (t, x(t)) points
	ofstream output_file_x_t;
	output_file_x_t.open("output_tikz_x_t.txt");
	output_file_x_t << "(" << 0 << "," << x0 << ")";
	for (uint32_t i = 0; i < num_samples - 1; ++i)
	{
		output_file_x_t << "(" << (i+1) * time_step << "," << points[i][0] << ")";
	}
	output_file_x_t << "(" << T << "," << 0 << ")" << endl;
	output_file_x_t.close();

	// All (t, y(t)) points
	ofstream output_file_y_t;
	output_file_y_t.open("output_tikz_y_t.txt");
	output_file_y_t << "(" << 0 << "," << y0 << ")";
	for (uint32_t i = 0; i < num_samples - 1; ++i)
	{
		output_file_y_t << "(" << (i + 1) * time_step << "," << points[i][1] << ")";
	}
	output_file_y_t << "(" << T << "," << 0 << ")" << endl;
	output_file_y_t.close();

	// All (t, z(t)) points
	ofstream output_file_z_t;
	output_file_z_t.open("output_tikz_z_t.txt");
	output_file_z_t << "(" << 0 << "," << z0 << ")";
	for (uint32_t i = 0; i < num_samples - 1; ++i)
	{
		output_file_z_t << "(" << (i + 1) * time_step << "," << points[i][2] << ")";
	}
	output_file_z_t << "(" << T << "," << 0 << ")" << endl;
	output_file_z_t.close();

    return 0;
}