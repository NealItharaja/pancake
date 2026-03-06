#pragma once
#include "histogram_distribution.h"

namespace ks {

class StatisticalTest {
public:
    static double ks_statistic(const HistogramDistribution& a, const HistogramDistribution& b);
    static double threshold(size_t sample_size, double alpha = 0.05);
};
}
