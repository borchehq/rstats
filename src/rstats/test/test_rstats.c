#ifdef NDEBUG
#undef NDEBUG
#endif

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>

#include "rstats.h"

#define LENGTH_ARRAY 1000
#define ITERATIONS_TEST 100


void fill_random(double *array, size_t length, double min, double max) {
    for(size_t i = 0; i < length; i++) {
        array[i] = min + (max - min) * rand() / (double) RAND_MAX;
    }
}

void test_rstats_mean() {
    for(size_t k = 0; k < ITERATIONS_TEST; k++) {
        double buffer[2] = {0.0};
        double x[LENGTH_ARRAY] = {0.0};
        double weights[LENGTH_ARRAY] = {0.0};
        double mean = 0.0;
        double sum_weights = 0.0;
        double comp = 0.0;
        double sum_comp = 0.0;
        double sum_weights_comp = 0.0;

        fill_random(x, LENGTH_ARRAY, 0.0, 1.0);
        fill_random(weights, LENGTH_ARRAY, 1e-5, 1.0);

        for(size_t i = 0; i < LENGTH_ARRAY; i++) {
            rstats_mean(x[i], weights[i], buffer);
            rstats_mean_finalize(&mean, buffer);
            sum_comp += weights[i] * x[i];
            sum_weights_comp += weights[i];
            comp = sum_comp / sum_weights_comp;
            assert(fabs(mean - comp) < 1e-7);
        }
    }
}

void test_rstats_variance() {
    for(size_t k = 0; k < ITERATIONS_TEST; k++) {
        double x[LENGTH_ARRAY] = {0.0};
        double weights[LENGTH_ARRAY] = {0.0};
        double buffer[3] = {0.0};
        double results[2] = {0.0};
        double comp = 0.0;
        double variance = 0.0;
        double sum_comp = 0.0;
        double sum_weights_comp = 0.0;
        double comp_v = 0.0;

        fill_random(x, LENGTH_ARRAY, 0.0, 1.0);
        fill_random(weights, LENGTH_ARRAY, 1e-5, 1.0);

        for(size_t i = 0; i < LENGTH_ARRAY; i++) {
            rstats_variance(x[i], weights[i], buffer);
            rstats_variance_finalize(results, buffer);
            sum_comp += weights[i] * x[i];
            sum_weights_comp += weights[i];
            comp = sum_comp / sum_weights_comp;
            for(size_t j = 0; j < i + 1; j++) {
                comp_v += weights[j] * (x[j] - comp) * (x[j] - comp);
            }
            comp_v /= sum_weights_comp;
            assert(fabs(results[0] - comp) < 1e-7);
            assert(fabs(results[1] - comp_v) < 1e-7);
            comp_v = 0;
        }
    }
}

void test_rstats_wskewness() {
    for(size_t k = 0; k < ITERATIONS_TEST; k++) {
        double x[LENGTH_ARRAY] = {0.0};
        double weights[LENGTH_ARRAY] = {0.0};
        double buffer[4] = {0.0};
        double results[3] = {0.0};
        double sum_weights_comp = 0.0;
        double skewness_comp = 0.0;
        double sum_comp = 0.0;
        double comp = 0.0;
        double variance_comp = 0.0;
        
        fill_random(x, LENGTH_ARRAY, 0.0, 1.0);
        fill_random(weights, LENGTH_ARRAY, 1e-5, 1.0);

        for(size_t i = 0; i < LENGTH_ARRAY; i++) {
            rstats_skewness(x[i], weights[i], buffer);
            rstats_skewness_finalize(results, buffer);
            sum_comp += weights[i] * x[i];
            sum_weights_comp += weights[i];
            comp = sum_comp / sum_weights_comp;
            for(size_t j = 0; j < i + 1; j++) {
                skewness_comp += weights[j] * (x[j] - comp) * (x[j] - comp)
                * (x[j] - comp);
                variance_comp += weights[j] * (x[j] - comp) * (x[j] - comp);
            }
            skewness_comp /= sum_weights_comp;
            variance_comp /= sum_weights_comp;
            assert(fabs(results[0] - comp) < 1e-7);
            assert(fabs(results[1] - variance_comp) < 1e-7);
            if(i > 0) {
                skewness_comp = skewness_comp / pow(variance_comp, 1.5);
                assert(fabs(results[2] - skewness_comp) < 1e-7);
            }
            skewness_comp = 0;
            variance_comp = 0;
        }
    }
}

void test_rstats_kurtosis() {
    for(size_t k = 0; k < ITERATIONS_TEST; k++) {
        double x[LENGTH_ARRAY] = {0.0};
        double weights[LENGTH_ARRAY] = {0.0};
        double buffer[5] = {0.0};
        double results[4] = {0.0};
        double kurtosis = 0;
        double sum_weights_comp = 0;
        double kurtosis_comp = 0;
        double skewness_comp = 0;
        double sum_comp = 0;
        double comp = 0;
        double variance_comp = 0;

        fill_random(x, LENGTH_ARRAY, 0.0, 1.0);
        fill_random(weights, LENGTH_ARRAY, 1e-5, 1.0);

        for(size_t i = 0; i < LENGTH_ARRAY; i++) {
            rstats_kurtosis(x[i], weights[i], buffer);
            rstats_kurtosis_finalize(results, buffer);
            sum_comp += weights[i] * x[i];
            sum_weights_comp += weights[i];
            comp = sum_comp / sum_weights_comp;
            for(size_t j = 0; j < i + 1; j++) {
                kurtosis_comp += weights[j] * (x[j] - comp) * (x[j] - comp) *
                (x[j] - comp) * (x[j] - comp);
                skewness_comp += weights[j] * (x[j] - comp) * (x[j] - comp)
                * (x[j] - comp);
                variance_comp += weights[j] * (x[j] - comp) * (x[j] - comp);
            }
            kurtosis_comp /= sum_weights_comp;
            skewness_comp /= sum_weights_comp;
            variance_comp /= sum_weights_comp;
            assert(fabs(results[0] - comp) < 1e-7);
            assert(fabs(results[1] - variance_comp) < 1e-7);
            if(i > 0) {
                skewness_comp = skewness_comp / pow(variance_comp, 1.5);
                assert(fabs(results[2] - skewness_comp) < 1e-7);
                assert(fabs(results[3] - kurtosis_comp / 
                pow(variance_comp, 2.0)) < 1e-5);
            }
            kurtosis_comp = 0.0;
            skewness_comp = 0.0;
            variance_comp = 0.0;
        }
    }
}

void test_central_moment() {
    for(size_t m = 0; m < ITERATIONS_TEST; m++) {
        double x[LENGTH_ARRAY] = {0.0};
        double weights[LENGTH_ARRAY] = {0.0};
        uint64_t p = 10;
        double mass = 0.0;
        double mean_cmp = 0.0;
        double results[15 + 2] = {0.0};
        double results_standardized[15 + 2] = {0.0};
        double buffer[15 + 1] = {0.0};
        double wmoments_comp[15 + 1] = {0.0};
        double sum_weights = 0.0;
        double sum_weights_comp = 0.0;

        fill_random(x, LENGTH_ARRAY, 0.0, 1.0);
        fill_random(weights, LENGTH_ARRAY, 1e-5, 1.0);

        for(size_t i = 0; i < LENGTH_ARRAY; i++) {
            rstats_central_moment(x[i], weights[i], buffer, p);
            rstats_central_moment_finalize(results, buffer, p, false);
            rstats_central_moment_finalize(results_standardized, buffer, p,
            true);
            // Calculate mean.
            mass += weights[i] * x[i];
            sum_weights_comp += weights[i];
            mean_cmp = mass / sum_weights_comp;
            assert(fabs(results[p + 1] - mean_cmp) < 1e-7);
            for(size_t k = 0; k < p; k++) {
                for(size_t j = 0; j < i + 1; j++) {
                    wmoments_comp[k] += weights[j] * pow(x[j] - mean_cmp, k);
                    if(k != 2) {
                        wmoments_comp[2] += 
                        weights[j] * pow(x[j] - mean_cmp, 2);
                    }
                }
                double moment = wmoments_comp[k] / sum_weights_comp;
                double moment_2 = wmoments_comp[2] / sum_weights_comp;
                double norm = pow(sqrt(moment_2), k);
                double std_centr_moment = moment / norm;        
                assert(fabs((results[k] - moment)) < 1e-5);
                assert(i == 0 || fabs(results_standardized[k] - 
                std_centr_moment) < 1e-5);
                wmoments_comp[k] = 0.0;
                wmoments_comp[2] = 0.0;
            }
        }
    }
}

void test_rstats_max() {
    double x[10] = {4, 8, 12, 16, 23, 45, 678, 123, 2.0123, M_PI};
    double max = DBL_MIN;
    double max_val;

    for(size_t i = 0; i < 10; i++) {
        rstats_max(x[i], &max);
    }
    assert(678 == max);
}


void test_rstats_min() {
    double x[10] = {4, 8, 12, 16, 23, 45, 678, 123, 2.0123, M_PI};
    double min = DBL_MAX;
    double min_val;

    for(size_t i = 0; i < 10; i++) {
        rstats_min(x[i], &min);
    }
    assert(2.0123 == min);
}

int main(int argc, char const *argv[]) {
    srand(111111);
    printf("[i] Testing rstats_mean()...\n");
    test_rstats_mean();
    printf("[i] Testing rstats_variance()...\n");
    test_rstats_variance();
    printf("[i] Testing rstats_wskewness()...\n");
    test_rstats_wskewness();
    printf("[i] Testing rstats_kurtosis()...\n");
    test_rstats_kurtosis();
    printf("[i] Testing rstats_max()...\n");
    test_rstats_max();
    printf("[i] Testing rstats_min()...\n");
    test_rstats_min();
    printf("[i] Testing central_moment...\n");
    test_central_moment();
    return 0;
}