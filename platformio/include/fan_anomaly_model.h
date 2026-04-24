#pragma once
#include <cstdarg>
namespace Eloquent {
    namespace ML {
        namespace Port {
            class OneClassSVM {
                public:
                    /**
                    * Predict class for features vector
                    */
                    int predict(float *x) {
                        float kernels[16] = { 0 };
                        kernels[0] = compute_kernel(x,   3.379  , 3.3258 );
                        kernels[1] = compute_kernel(x,   3.355  , 3.3382 );
                        kernels[2] = compute_kernel(x,   3.378  , 3.344 );
                        kernels[3] = compute_kernel(x,   3.391  , 3.35 );
                        kernels[4] = compute_kernel(x,   3.373  , 3.3546 );
                        kernels[5] = compute_kernel(x,   3.35  , 3.3536 );
                        kernels[6] = compute_kernel(x,   3.389  , 3.2794 );
                        kernels[7] = compute_kernel(x,   3.365  , 3.2886 );
                        kernels[8] = compute_kernel(x,   2.62  , 2.7158 );
                        kernels[9] = compute_kernel(x,   2.633  , 2.7118 );
                        kernels[10] = compute_kernel(x,   2.638  , 2.6768 );
                        kernels[11] = compute_kernel(x,   2.656  , 2.6764 );
                        kernels[12] = compute_kernel(x,   2.621  , 2.694 );
                        kernels[13] = compute_kernel(x,   2.651  , 2.7098 );
                        kernels[14] = compute_kernel(x,   2.637  , 2.7008 );
                        kernels[15] = compute_kernel(x,   2.669  , 2.675 );
                        float decision = -14.992961466312 - ( + kernels[0]   + kernels[1]   + kernels[2]   + kernels[3]   + kernels[4]   + kernels[5]   + kernels[6]   + kernels[7] * 0.0  + kernels[8]   + kernels[9]   + kernels[10]   + kernels[11]   + kernels[12]   + kernels[13]   + kernels[14]   + kernels[15]  );

                        return decision > 0 ? 0 : 1;
                    }

                protected:
                    /**
                    * Compute kernel between feature vector and support vector.
                    * Kernel type: rbf
                    */
                    float compute_kernel(float *x, ...) {
                        va_list w;
                        va_start(w, 2);
                        float kernel = 0.0;

                        for (uint16_t i = 0; i < 2; i++) {
                            kernel += pow(x[i] - va_arg(w, double), 2);
                        }

                        return exp(-0.001 * kernel);
                    }
                };
            }
        }
    }