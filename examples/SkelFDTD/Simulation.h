#pragma once
#include "SkelCL/SkelCL.h"
#include "SkelCL/Matrix.h"
#include "SkelCL/Map.h"
#include "SkelCL/Stencil.h"
#include <vector>

typedef struct {	
			float x;	
			float y;	
			float z;	
			float w;	
} data_t;				

using namespace skelcl;

class Simulation
{
public:

	Simulation(const size_t size);

	virtual ~Simulation();

	void initialize();

	void run();

private:

	bool readRandomMediaFile(const char*);

	struct Parameters
	{
		float c;
		float pi;
		float hbar;
		float mu0;
		float eps0;
		float log_2;
		float c_mu0;
		int abs_cell_size;
		int array_size;
		float tau32;
		float tau21;
		float tau10;
		float dx;
		float dt;
		float dx_r;
		float dt_r;
		float omega_a;
		float omega_a2;
		float T2;
		float c3;						// c³
		float Nges;
		float sqrt_mu0_eps0;
		float sqrt_eps0_mu0;
		float grid_size;
		float resolution;
		float time_end;
		float abs_size;
		float Pr;
		int time_steps;
	} _p;

	Matrix<data_t> E; 
	Matrix<data_t> H;  
	Matrix<data_t> N;

	std::vector<data_t> _particles;

	Map<data_t(data_t)>* motionEquation;
	Stencil<data_t(data_t)>* eFieldEquation;
	Stencil<data_t(data_t)>* hFieldEquation;

};

