#pragma once
#include "histogram_distribution.h"

namespace ad {

class StatisticalTest {
public:
    static double anderson_darling_statistic(const HistogramDistribution& a, const HistogramDistribution& b);
    static double threshold(size_t sample_size, double alpha = 0.05);
};
}
