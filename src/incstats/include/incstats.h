#ifndef INCSTATS_H
#define INCSTATS_H

// This is needed for compilation with MSVC under Windows.
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <math.h>
#include <stdint.h>
#include <stdbool.h>


/**
 * @brief Computes the power of a number.
 *
 * This function calculates \( x^y \) (x raised to the power y) for a given 
 * double x and an unsigned 64-bit integer y.
 * 
 * @param x The base value of type double.
 * @param y The exponent value of type uint64_t. If y is 0, the function 
 * returns 1.0.
 * @return The result of \( x^y \). If y is 0, the function returns 1.0.
 *
 */
inline double incstats_pow(double x, uint64_t y) {
    double result = x;

    if(y == 0) {
        return 1.0;
    }
    for(uint64_t i = 0; i < y - 1; i++) {
        result *= x;
    }

    return result;
}

/**
 * @brief Computes the factorial of a number.
 *
 * This function calculates the factorial of a given unsigned 64-bit integer n.
 * 
 * @param n The value for which the factorial is to be calculated.
 * @return The factorial of n. If n is 0 or 1, the function returns
 * 1.
 *
 */
inline uint64_t factorial(uint64_t n) {
    uint64_t result = 1;
    for(uint64_t i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

/**
 * @brief Computes the binomial coefficient (n choose k).
 *
 * This function calculates the binomial coefficient, also known as
 * "n choose k". It is calculated using the formula: 
 * \f$ \binom{n}{k} = \frac{n!}{k!(n-k)!} \f$
 * 
 * @param n The total number of elements in the set.
 * @param k The number of elements to choose from the set.
 * @return The binomial coefficient (n choose k).
 */
inline uint64_t n_choose_k(uint64_t n, uint64_t k) {
    uint64_t result = 1;
    k = k > n-k ? n-k : k;
    for(uint64_t j = 1; j <= k; j++, n--) {
        if(n % j == 0) {
            result *= n / j;
        }
        else if(result % j == 0) {
            result = result / j * n;
        }
        else {
            result = result * n / j;
        }
    }
    return result;
}


/**
 * @brief Updates the running mean of a dataset.
 *
 * This function updates the running mean of a dataset using a new value `x` 
 * with weight `w`.
 * 
 * @param x The new value to incorporate into the running mean.
 * @param w The weight of the new value `x`.
 * @param buffer A pointer to a double array of length 2.
 * 
 * @note This function shall be used in conjunction with incstats_mean_finalize.
 * The `buffer` array is expected to be initialized to 0 before use.
 */
inline void incstats_mean(double x, double w, double *buffer) {
    buffer[0] += w;
    buffer[1] = buffer[1] + w / buffer[0] * (x - buffer[1]);
}

/**
 * @brief Finalizes the computation of the running mean.
 *
 * This function finalizes the computation of the running mean.
 * 
 * @param mean A pointer to a double where the final mean value will be stored.
 * @param buffer A pointer to a double array of length 2 containing the results
 * generated by incstats_mean.
 * 
 * @note This function should be used to finalize the results obtained by 
 * `incstats_mean`. This call is non-destructive, allowing multiple calls to 
 * the same buffer.
 */
inline void incstats_mean_finalize(double *mean, double *buffer) {
    *mean = buffer[1];
}

/**
 * @brief Updates the running mean and variance of a dataset.
 *
 * This function updates the running mean and variance of a dataset using a new 
 * value `x` with weight `w`.
 * 
 * @param x The new value to incorporate into the running statistics.
 * @param w The weight of the new value `x`.
 * @param buffer A pointer to a double array of length 3.
 * 
 * @note The results shall be finalized by incstats_variance_finalize.
 * The `buffer` array is expected to be initialized to 0 before use.
 */
inline void incstats_variance(double x, double w, double *buffer) {
    double new_mean;

    buffer[0] += w;
    new_mean = buffer[1] + w / buffer[0] * (x - buffer[1]);
    buffer[2] = buffer[2] + w * (x - buffer[1]) * (x - new_mean);
    buffer[1] = new_mean;
}

/**
 * @brief Finalizes the computation of the running mean and variance.
 *
 * This function finalizes the computation of the running mean and variance.
 * 
 * @param results A pointer to an array of length 2 where the final mean and 
 * variance values will be stored:
 *                - `results[0]` will store the final mean value.
 *                - `results[1]` will store the final variance value.
 * @param buffer A pointer to a double array of length 3.
 * 
 * @note This function should be used to finalize the results obtained by 
 * `incstats_variance`. This call is non-destructive, allowing multiple calls to 
 * the same buffer.
 */
inline void incstats_variance_finalize(double *results, double *buffer) {
    results[0] = buffer[1];
    results[1] = buffer[2] / buffer[0];
}

/**
 * @brief Updates the running mean, variance, and skewness of a dataset.
 *
 * This function updates the running mean, variance, and skewness of a dataset
 * using a new value `x` with weight `w`.
 * 
 * @param x The new value to incorporate into the running statistics.
 * @param w The weight of the new value `x`.
 * @param buffer A pointer to a double array of length 4.
 * 
 * @note This function updates the skewness incrementally and should be called 
 * with each new data point. The `buffer` array is expected to be initialized 
 * to 0 before use.
 */
inline void incstats_skewness(double x, double w, double *buffer) {
    double new_sum_w = buffer[0] + w;
    buffer[3] = buffer[3] + 3 * (buffer[2]) * (-w * (x - buffer[1]) /
                    new_sum_w) + buffer[0] * incstats_pow(-w * (x - buffer[1]) / 
                    new_sum_w, 3) + w * incstats_pow(buffer[0] * 
                    (x - buffer[1]) / new_sum_w, 3);
    buffer[2] = buffer[2] + buffer[0] * incstats_pow(-w * (x - buffer[1]) / 
                    new_sum_w, 2) + w * incstats_pow(buffer[0] * 
                    (x - buffer[1]) / new_sum_w, 2);
    buffer[1] = buffer[1] + w / new_sum_w * (x - buffer[1]);
    buffer[0] = new_sum_w;
}

/**
 * @brief Finalizes the computation of the running mean, variance, and 
 * skewness.
 *
 * This function finalizes the computation of the running mean, variance.
 * 
 * @param results A pointer to an array of length 3 where the final mean, 
 * variance, and skewness values will be stored:
 *                - `results[0]` will store the final mean value.
 *                - `results[1]` will store the final variance value.
 *                - `results[2]` will store the final skewness value.
 * @param buffer A pointer to a double array of length 4.
 * 
 * @note This function should be used to finalize the results obtained by 
 * `incstats_skewness`. This call is non-destructive, 
 * allowing multiple calls to the same buffer.
 */
inline void incstats_skewness_finalize(double *results, double *buffer) {
    results[0] = buffer[1];
    results[1] = buffer[2] / buffer[0];
    results[2] = (buffer[3] / buffer[0]) / ((buffer[2] / buffer[0]) 
                 * sqrt(buffer[2] / buffer[0]));
}

/**
 * @brief Updates the running mean, variance, skewness, and kurtosis of a 
 * dataset.
 *
 * This function updates the running mean, variance, skewness, and 
 * kurtosis of a dataset using a new value `x` with weight `w`.
 * 
 * @param x The new value to incorporate into the running statistics.
 * @param w The weight of the new value `x`.
 * @param buffer A pointer to a double array of length 5.
 * 
 * @note This function updates the kurtosis incrementally and should be 
 * called with each new data point. The `buffer` array is expected to be 
 * initialized to 0 before use.
 */
inline void incstats_kurtosis(double x, double w, double *buffer) {
    double new_sum_w = buffer[0] + w;
    buffer[4] = buffer[4] + 4.0 * (buffer[3]) * 
                (-w * (x - buffer[1]) / new_sum_w) + 6.0 * (buffer[2]) * 
                incstats_pow((-w * (x  - buffer[1]) / new_sum_w), 2) + buffer[0]
                * incstats_pow((-w * (x  - buffer[1]) / new_sum_w), 4) + w * 
                incstats_pow((buffer[0] * (x  - buffer[1]) / new_sum_w), 4);
    buffer[3] = buffer[3] + 3 * (buffer[2]) * (-w * (x - buffer[1]) /
                    new_sum_w) + buffer[0] * incstats_pow(-w * (x - buffer[1]) / 
                    new_sum_w, 3) + w * incstats_pow(buffer[0] * 
                    (x - buffer[1]) / new_sum_w, 3);
    buffer[2] = buffer[2] + buffer[0] * incstats_pow(-w * (x - buffer[1]) / 
                    new_sum_w, 2) + w * incstats_pow(buffer[0] * 
                    (x - buffer[1]) / new_sum_w, 2);
    buffer[1] = buffer[1] + w / new_sum_w * (x - buffer[1]);
    buffer[0] = new_sum_w;
}

/**
 * @brief Finalizes the computation of the running mean, variance, skewness, 
 * and kurtosis.
 *
 * This function finalizes the computation of the running mean, variance, 
 * skewness, and kurtosis by copying the current values from the buffer to 
 * the provided results array.
 * 
 * @param results A pointer to an array length 4 where the final mean,
 *  variance, skewness, and kurtosis values will be stored:
 *                - `results[0]` will store the final mean value.
 *                - `results[1]` will store the final variance value.
 *                - `results[2]` will store the final skewness value.
 *                - `results[3]` will store the final kurtosis value.
 * @param buffer A pointer to a double array of length 5.
 * 
 * @note This function should be used to finalize the results obtained by 
 * `incstats_kurtosis`. This call is non-destructive, allowing multiple calls to
 * the same buffer.
 */
inline void incstats_kurtosis_finalize(double *results, double *buffer) {
    results[0] = buffer[1];
    results[1] = buffer[2] / buffer[0];
    results[2] = (buffer[3] / buffer[0]) / ((buffer[2] / buffer[0]) 
                 * sqrt(buffer[2] / buffer[0]));
    results[3] = (buffer[4] / buffer[0]) / 
                 incstats_pow((buffer[2] / buffer[0]), 2);
}

/**
 * @brief Updates the running central moments of a dataset.
 *
 * This function updates the running central moments of a dataset using a 
 * new value `x` with weight `w`.
 * The central moments are updated up to the specified order `p`.
 * 
 * @param x The new value to incorporate into the running statistics.
 * @param w The weight of the new value `x`.
 * @param buffer A pointer to an array of doubles of length p + 1.
 * @param p The order of the highest central moment to update.
 * 
 * @note This function updates the central moments incrementally and should be
 * called with each new data point. The `buffer` array is expected to be 
 * initialized to 0 before use.
 */
inline void incstats_central_moment(double x, double w, double *buffer, uint64_t p) {
    double new_sum_w = buffer[0] + w;

    for(uint64_t i = p; i > 1; i--) {
        double tmp = 0.0;
        for(uint64_t k = i - 2; k > 0; k--) {
           tmp += n_choose_k(i, k) * buffer[i - k] * 
                  incstats_pow(-w * (x - buffer[1]) / new_sum_w, k);  
        }
        buffer[i] = buffer[i] + tmp +
                    buffer[0] * incstats_pow(-w * (x - buffer[1]) / 
                    new_sum_w, i) + w * incstats_pow(buffer[0] * 
                    (x - buffer[1]) / new_sum_w, i);
    }
    buffer[1] = buffer[1] + w / new_sum_w * (x - buffer[1]);
    buffer[0] = new_sum_w;
}

/**
 * @brief Finalizes the computation of the running central moments.
 *
 * This function finalizes the computation of the running central moments by 
 * copying the current mean and central moments from the buffer to the provided
 * results array.
 * 
 * @param results A pointer to an array of doubles where the final mean and 
 * central moments will be stored:
 *                - `results[p + 1]` will store the final mean value.
 *                - `results[0]` to `results[p]` will store the final central 
 *                  moments from the 0th to the p-th order.
 * results must point to an array of length p + 2.
 * @param buffer A pointer to an array of doubles used in 
 * `incstats_central_moment` to calculate the running statistics.
 * buffer must point to an array of length max(2, p + 1).
 * @param p The order of the highest central moment to finalize.
 * 
 * @note This function should be used to finalize the results obtained by 
 * `incstats_central_moment`. This call is non-destructive, allowing multiple 
 * calls to the same buffer.
 */
inline void incstats_central_moment_finalize(double *results, double *buffer, 
uint64_t p, bool standardize) {
    results[0] = 1.0;
    results[1] = 0.0;
    for(uint64_t i = 2; i < p + 1; i++) {
        results[i] = buffer[i] / buffer[0];
    }
    if(standardize) {
        double variance = results[2];
        for(uint64_t i = 0; i < p + 1; i++) {
            results[i] = results[i] / incstats_pow(sqrt(variance), i);
        }
    }
    results[p + 1] = buffer[1]; // Mean.
}

/**
 * @brief Updates the maximum value of a dataset.
 *
 * This function updates the maximum value of a dataset by comparing the new 
 * value `x` with the current maximum value stored at the location pointed to by
 * `max`.
 * If `x` is greater than the current maximum, the maximum value is updated to 
 * `x`.
 * 
 * @param x The new value to compare with the current maximum.
 * @param max A pointer to a double that stores the current maximum value. 
 * If `x` is greater than the value pointed to by `max`, it will be updated to
 * `x`.
 */
inline void incstats_max(double x, double *max) {
    if(*max < x) {
        *max = x;
    }
}

/**
 * @brief Updates the minimum value of a dataset.
 *
 * This function updates the minimum value of a dataset by comparing the new 
 * value `x` with the current minimum value stored at the location pointed to
 * by `min`.
 * If `x` is less than the current minimum, the minimum value is updated to `x`.
 * 
 * @param x The new value to compare with the current minimum.
 * @param min A pointer to a double that stores the current minimum value. If 
 * `x` is less than the value pointed to by `min`, it will be updated to `x`.
 */
inline void incstats_min(double x, double *min) {
    if(*min > x) {
        *min = x;
    }
}

#endif