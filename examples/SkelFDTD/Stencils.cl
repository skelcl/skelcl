#define idx_x (get_global_id(1))
#define idx_y (get_global_id(0))

#define __sq(x) ((x)*(x))
#define __cu(x) ((x)*(x)*(x))

#define fx pml(idx_y+1, abs_cell_size, array_size_2, 0.25f)
#define fy pml(idx_x+1, abs_cell_size, array_size_2, 0.25f)  
#define gx pml(idx_y+1, abs_cell_size, array_size_2, 0.33f)
#define gy pml(idx_x+1, abs_cell_size, array_size_2, 0.33f)


data_t pml(int i, int acs, int as, float scale)
{
	//		  1		   2	    3	   unused
	data_t ret = { 0.0f, 1.0f, 1.0f, 0.0f };
	if (i > as)
		i = (as * 2) - 1 - i;

	if (i >= acs)
		return ret;

	float xnum = acs - i;
	float xn = 0;

	xnum = (xnum - 0.5f) / acs;
	xn = scale * xnum * xnum * xnum;
	ret.x = xn;
	ret.y = 1.0f / (1.0f + xn);
	ret.z = (1.0f - xn) / (1.0f + xn);

	return ret;
}

float jsrc(float t)
{
	if ((t < (3.0f)))
		return 1000.f * exp(-2.0f * log_2 * __sq((t - 0.0f) / 0.5f)) * sin(2.0f * pi * 2.0f * t);

	return 0.0f;
}


//	   y - 1		 x - 1
data_t eFieldEquation(input_matrix_t* mH, data_t_matrix_t mE, float t)
{
	data_t E = get(mE, idx_x, idx_y);

	data_t cx = gx;
	data_t cy = gy;

	data_t H  = getData(mH, 0, 0);
	data_t Ha = getData(mH, 0, 1);
	data_t Hc = getData(mH, -1, 0);

	E.y = cx.z * cy.z * E.y + cx.y * cy.y * 0.5f * (H.y - Hc.y - H.x + Ha.x);

	if ((idx_x == src_x) && (idx_y == src_y))
		E.y += sqrt_eps0_mu0 * jsrc(t);

	float eps = eps_b;
	if ((fx.x + fy.x) == 0.0f)
	{
		if (E.w != 0.0f)
			eps = eps_r; // in particle
	}

	E.z = (E.y - c * E.x) * (1.0f / eps);

	return E;
}


data_t hFieldEquation(input_matrix_t* mE, data_t_matrix_t mH)
{
	data_t H = get(mH, idx_x, idx_y);

	data_t E  = getData(mE, 0, 0);
	data_t Ea = getData(mE, 0, -1);
	data_t Ec = getData(mE, 1, 0);

	float2 curl_e = { 0.0f, 0.0f };

	data_t cx = fx;
	data_t cy = fy;

	curl_e.x = E.z - Ea.z;
	curl_e.y = Ec.z - E.z;

	if (cx.x > 0.0f)
	{
		H.z = H.z + cx.x * curl_e.x;
	}
	if (cy.x > 0.0f)
	{
		H.w = H.w + cy.x * curl_e.y;
	}

	H.x = cy.z * H.x + cy.y * 0.5f * (curl_e.x + H.z);
	H.y = cx.z * H.y + cx.y * 0.5f * (curl_e.y + H.w);
	return H;
}
