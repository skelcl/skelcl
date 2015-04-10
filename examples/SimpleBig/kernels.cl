typedef float DATA_T;

DATA_T simple(input_matrix_t *img) {
        DATA_T sum = 0;

        for (int y = -NORTH; y <= SOUTH; y++) {
                for (int x = -WEST; x <= EAST; x++) {
                        sum += getData(img, y, x);
                }
        }

        DATA_T out = (int)sum % 2 ? sum : 0;

        return out;
}


DATA_T complex(input_matrix_t *img) {
        DATA_T sum = 0;

        for (int y = -NORTH + 1; y < SOUTH; y++) {
                for (int x = -WEST + 1; x < EAST; x++) {
                        DATA_T a = sin(getData(img, -1, 0));
                        DATA_T b = native_sin(getData(img, 0, 1) * a);
                        sum += getData(img, y, x) * a * (b / b);
                }
        }

        DATA_T out = 0;
        for (int i = EAST; i >= -WEST; i--) {
                sum *= cos((DATA_T)((int)getData(img, 0, i) % i) + sqrt(getData(img, 0, 0)));
                out += sinpi(sum);
                out /= sum;
        }

        return out;
}
