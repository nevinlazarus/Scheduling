#ifndef STEP
#define STEP
#include <vector>
#include <cmath>

inline double myMean(std::vector<double> vals) {
  double sum = 0.0;
  for (auto v : vals) {
    sum += v;
  }
  return sum / vals.size();
}

inline double myStandardDeviation(std::vector<double> vals) {
  double sum = 0.0;
  double mean = myMean(vals);
  for (auto v : vals) {
    sum += pow(v - mean, 2);
  }
  return sum / (vals.size() - 1);
}

#endif
