#include <cmath>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <istream>
#include <strstream>
#include <fstream>
#include <chrono>

#include "Simulation.h"
#include "SkelCL/detail/Padding.h" // FIXME: includes to detail shold not be necessary
#include "SkelCL/detail/Distribution.h"

#define __sq(x) ((x)*(x))
#define CL_CONSTANT(x) #x << " " << x

skelcl::detail::RegisterCommonDefinition
  data_t_def("typedef struct { float x; float y; float z; float w; } data_t; ");

// physical constants
const float __c = 299792458.0;
const float __pi = 3.14159265358979;
const float __hbar = 6.62606896e-34 / (2.0*__pi);
const float __mu0 = 4.0 * __pi * 1.0e-7;
const float __eps0 = 1.0 / (__c*__c*__mu0);
const float __log_2 = log(2.0);
const float __c_mu0 = __c * __mu0;
const float sqrt_eps0_mu0 = sqrt(__eps0 / __mu0);
const float sqrt_mu0_eps0 = sqrt(__mu0 / __eps0);

void read_string(std::ifstream* ifs, std::istrstream* idss, char* buffer, char* str)
{
	std::istrstream iss(buffer, 1024);
	do
	{
		ifs->getline(buffer, 1024);
	} while (buffer[0] == '#');

	for (int i = 0; i != 1024 && buffer[i] != '\0'; ++i)
		if (buffer[i] == '\n' || buffer[i] == '\r')
			buffer[i] = '\0';

	strcpy(str, buffer);
}

data_t atof4(char* str)
{
	data_t ret = { 0.0f, 0.0f, 0.0f, 0.0f };
	char* temp = str;
	int values = 0;
	bool start = false;

	for (int i = 0; str[i] != '\0'; ++i)
	{
		if (str[i] == '[' || str[i] == '|' || str[i] == ']')
		{
			if (!start)
			{
				temp = str + i + 1;
				start = true;
			}
			else
			{
				float x, y, z, w;
				str[i] = '\0';
				switch (values)
				{
				case 0:
					ret.x = atof(temp);
					break;
				case 1:
					ret.y = atof(temp);
					break;
				case 2:
					ret.z = atof(temp);
					break;
				case 3:
					ret.w = atof(temp);
					break;
				default:
					return ret;
				}
				values++;
				temp = str + i + 1;
			}
		}
	}
	return ret;

}

std::ostream& operator<<(std::ostream& os, data_t a)
{
	os << std::scientific << "{" << a.x << "f," << a.y << "f," << a.z << "f," << a.w << "f}";

	return os;
}

using namespace skelcl;

Simulation::Simulation(const size_t size)
: E(MatrixSize(size, size)), H(MatrixSize(size, size)), N(MatrixSize(size, size))
{

	float gain_f = 6e+14f;
	float T2 = 2.18e-14f;
	float Nges = 3.311e24f;
	float tau32 = 1e-13f;
	float tau21 = 1e-10f;
	float tau10 = 1e-12f;
	float eps_r = 4.0f;
	float eps_b = 1.0f;
	float Pr = _p.Pr = 1e13f;

	int src_x = 50;
	int src_y = 50;

	int resolution = _p.resolution = 100; // FIXME: read a jobfile for dynamic values
	float abs_size = _p.abs_size = 0.32;
	float grid_size = _p.grid_size = size / _p.resolution;
	int array_size = _p.array_size = size;
	int array_size_2  = size / 2;
	int abs_cell_size = _p.abs_cell_size = _p.abs_size * _p.resolution;
	float time_end = _p.time_end = 200;

	float c = _p.c = __c;
	float pi = _p.pi = __pi;
	float hbar = _p.hbar = __hbar;
	float mu0 = _p.mu0 = __mu0;
	float eps0 = _p.eps0 = __eps0;
	float log_2 = _p.log_2 = __log_2;
	float c_mu0 = _p.c_mu0 = __c_mu0;
	_p.sqrt_eps0_mu0 = sqrt_eps0_mu0;
	_p.sqrt_mu0_eps0 = sqrt_mu0_eps0;
	float c3 = _p.c3 = pow(__c, 3);
	float dx = _p.dx = 1.0 / _p.resolution;
	float dt = _p.dt = 0.5 * _p.dx;

	_p.time_steps = (int)_p.time_end / _p.dt;

	_p.dx_r = _p.dx * 1e-6; //dx in m
	float dt_r = _p.dt_r = 0.5 * _p.dx_r / __c;

	float omega_a = _p.omega_a = gain_f * 2.0 * __pi;
	float omega_a2 = _p.omega_a2 = pow(_p.omega_a, 2);

	float k = (6.0f * pi * eps0 * c_mu0 * c3) / (omega_a2 * tau21);
	float a1 = sqrt_mu0_eps0 / (_p.hbar * omega_a) * _p.dt_r;
	float a2 = (1.0f - (1.0f / tau21 + 2.0f / T2))* _p.dt_r;
	float dt_tau32 = _p.dt_r / tau32;
	float dt_tau21 = _p.dt_r / tau21;
	float dt_tau10 = _p.dt_r / tau10;
	int smoothGrade = 1;


	data_t zero = { 0.f, 0.f, 0.f, 0.f };


	std::stringstream ss;

	std::string define("#define ");

	ss << std::scientific << define << CL_CONSTANT(Pr) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(eps_r) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(eps_b) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(abs_cell_size) << "\n";
	ss << std::scientific << define << CL_CONSTANT(array_size) << "\n";
	ss << std::scientific << define << CL_CONSTANT(array_size_2) << "\n";
	ss << std::scientific << define << CL_CONSTANT(log_2) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(pi) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(dt_r) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(k) << "f" << "\n";

	ss << std::scientific << define << CL_CONSTANT(a1) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(a2) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(omega_a2) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(Nges) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(dt_tau10) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(dt_tau32) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(dt_tau21) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(sqrt_eps0_mu0) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(c) << "f" << "\n";
	ss << std::scientific << define << CL_CONSTANT(src_x) << "\n";
	ss << std::scientific << define << CL_CONSTANT(src_y) << "\n";

	std::string code;
	std::ifstream file("./Kernels.cl", std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		file.seekg(0, std::ios::end);
		code.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(&code[0], code.size());
		file.close();
	}

	std::string map_in = ss.str() + code;


	std::ifstream file_s("./Stencils.cl", std::ios::in | std::ios::binary | std::ios::ate);
	std::string code2;
	if (file_s.is_open())
	{
		file_s.seekg(0, std::ios::end);
		code2.resize(file_s.tellg());
		file_s.seekg(0, std::ios::beg);
		file_s.read(&code2[0], code2.size());
		file_s.close();
	}
	std::string stc_in = ss.str() + code2;
	std::cout << map_in;
	motionEquation = new Map<data_t(data_t)>(map_in, "motionEquation");
	eFieldEquation = new Stencil<data_t(data_t)>(stc_in, 1u, 1u, 0u, 0u, detail::Padding::NEUTRAL, zero, "eFieldEquation");
	hFieldEquation = new Stencil<data_t(data_t)>(stc_in, 0u, 0u, 1u, 1u, detail::Padding::NEUTRAL, zero, "hFieldEquation");
}

Simulation::~Simulation()
{

	delete motionEquation;
	delete eFieldEquation;
	delete hFieldEquation;
}

void Simulation::initialize()
{
	//Map<data_t(data_t)> zeroMem(R"(data_t zero(data_t x)
	//								{
	//									data_t zero = { 0.0f, 0.0f, 0.0f, 0.0f };
	//									return zero;
	//								})", "zero");


	//E = zeroMem(E);
	//H = zeroMem(H);
	//N = zeroMem(N);

	std::cout << std::scientific;

	readRandomMediaFile("data/job.rmf");
	LOG_INFO("loaded: ", _particles.size(), " particles");
	int smoothGrade = 1;
	for (int idx_y = 0; idx_y != E.rowCount(); ++idx_y)
	{
		for (int idx_x = 0; idx_x != E.columnCount(); ++idx_x)
		{
			float x = (float)idx_y / (float)_p.resolution;
			float y = (float)idx_x / (float)_p.resolution;
			data_t sphere = { 0.0f, 0.0f, 0.0f, 0.0f };
			int particle = -1;
			float smoothSteps = 2.0f * smoothGrade + 1.0f;
			float step = 1.0f / (_p.resolution * smoothSteps);
			int hits = 0;
			for (int i = 0; i != _particles.size(); ++i)
			{
				sphere = _particles[i];

				if (__sq(sphere.x - x) + __sq(sphere.y - y) - __sq(sphere.w) <= 0.0f)
				{
					particle = i;
					for (int l = (-1 * smoothGrade); l <= smoothGrade; ++l)
					{
						for (int m = (-1 * smoothGrade); m <= smoothGrade; ++m)
						{
							if (__sq(sphere.x - (x + l * step)) + __sq(sphere.y - (y + m * step)) - __sq(sphere.w) <= 0.0f)
							{
								hits++;
							}
						}
					}
				}
			}
			if (particle >= 0)
			{
				N[idx_x][idx_y].w = _p.Pr;
			}
		}
	}

	LOG_DEBUG("INITIALIZED");
}

void Simulation::run()
{
	auto dev = skelcl::detail::globalDeviceList.front();
	LOG_INFO("computing FDTD with ", _p.time_steps, " iterations");
	
	for (int t = 0; t != _p.time_steps; ++t)
	{

		dev->wait();
		auto gstart = std::chrono::high_resolution_clock::now();
		auto start = std::chrono::high_resolution_clock::now();
		(*motionEquation)(out(N), N, out(E));
		dev->wait();
		auto end = std::chrono::high_resolution_clock::now();
		LOG_INFO("rate_eq:", std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6);


		dev->wait();
		start = std::chrono::high_resolution_clock::now();
		(*eFieldEquation)(1, out(E), out(E), H, E, t * _p.dt);
		dev->wait();
		end = std::chrono::high_resolution_clock::now();
		LOG_INFO("efield_eq:", std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6);

		dev->wait();
		start = std::chrono::high_resolution_clock::now();
		(*hFieldEquation)(1, out(H), out(H), E, H);
		dev->wait();
		end = std::chrono::high_resolution_clock::now();
		auto gend = std::chrono::high_resolution_clock::now();
		LOG_INFO("hfield_eq:", std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e6);
		
		LOG_INFO("sum:", std::chrono::duration_cast<std::chrono::nanoseconds>(gend - gstart).count() / 1e6);

		if (t == 100) break;
	}
	
	LOG_INFO("N = ", N[58][51]);
	LOG_INFO("E = ", E[40][50]);
	LOG_INFO("E = ", E[50][40]);
	LOG_INFO("E = ", E[50][50]);

	//LOG_INFO("FDTD finished in ", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(), "s");
}

bool Simulation::readRandomMediaFile(const char* filename)
{
	char buffer[1024];
	char str[1024];
	char* temp;
	std::ifstream ifs(filename);
	
	std::istrstream iss(buffer, 1024);
	if (ifs.is_open())
	{
		while (true)
		{ 
			read_string(&ifs, &iss, buffer, str);
			if (str[0] == 'p') // its a positon 
			{
				_particles.push_back(atof4(str + 3));
			}
			else
			{
				if (str[0] == 'e')
					break;
			}
		}
		ifs.close();
		return true;
	}
	else
		return false;
}
