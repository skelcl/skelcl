// Predefined macros: NORTH, SOUTH, WEST, EAST, corresponding to
// respective border region sizes.

// Type of input and output data.
typedef float DATA_T;


// "Simple" kernel. Returns the average of all neighbouring element values.
DATA_T simple(input_matrix_t *img) {
        // Number of neighbouring elements.
        int numElements = (NORTH + SOUTH + 1) * (WEST + EAST + 1);
        DATA_T sum = 0;

        // Loop over all neighbouring elements.
        for (int y = -NORTH; y <= SOUTH; y++) {
                for (int x = -WEST; x <= EAST; x++) {
                        // Sum values of neighbouring elements.
                        sum += getData(img, y, x);
                }
        }

        // If/then/else branch:
        DATA_T out = (int)sum % 2 ? sum : 0;

        return out;
}

// "Complex" kernel. Performs lots of trigonometric heavy lifting.
DATA_T complex(input_matrix_t *img) {
        DATA_T sum = 0;

        // Iterate over all except outer neighbouring elements.
        for (int y = -NORTH + 1; y < SOUTH; y++) {
                for (int x = -WEST + 1; x < EAST; x++) {
                        // Do *some* computation on values.
                        DATA_T a = sin((float)getData(img, -1, 0));
                        DATA_T b = native_sin((float)getData(img, 0, 1) * a);
                        sum += getData(img, y, x) * a * (b / b);
                }
        }

        DATA_T out = 0;
        // Loop over horizontal region.
        for (int i = EAST; i >= -WEST; i--) {
                // DO *some* computation on values.
                sum *= cos((DATA_T)((int)getData(img, 0, i) % i) + sqrt((float)getData(img, 0, 0)));
                out += sinpi((float)sum);
                out /= sum;
        }

        return out;
}
