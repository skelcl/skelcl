#define idx_x (get_global_id(0))
#define idx_y (get_global_id(1))


data_t motionEquation(data_t N, data_t_matrix_t mE)
{
	if (N.w > 0.0f)
	{
		data_t E = get(mE, idx_y, idx_x);

		float N0 = (Nges - (N.x + N.y + N.z));
		N.x = (1.0f - dt_tau32)  * N.x + ((N0 < 0.0f) ? 0.0f : N0) * (N.w * dt_r);
		N.y = (1.0f - dt_tau21)  * N.y + N.x * dt_tau32 + a1 * (E.z * E.w);
		N.z = (1.0f - dt_tau10)  * N.z + N.y * dt_tau21 - a1 * (E.z * E.w);

		E.w = a2 * E.w - omega_a2 * E.x * dt_r + k * (N.z - N.y) * E.z * dt_r;

		E.x = E.x + E.w * dt_r;

		set(mE, idx_y, idx_x, E);
	}
	return N;
}


