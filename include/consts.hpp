#pragma once
#include <vector>

using DataPoint = std::vector<double>;

// parameters of VP tree
namespace VP_CONST {
/** if A_SUBSET_CARDINALITY + B_SUBSET_CARDINALITY is larger than size of
 * dataset then no random subset is used but real variance of distances is
 * calculated */
/** Cardinality of subset A in random distance variance calculation */
const int A_SUBSET_CARDINALITY = 2;
/** Cardinality of subset B in random distance variance calculation */
const int B_SUBSET_CARDINALITY = 2;

/** Parameter of Minkowski distance calculation */
const int MINKOWSKI_PARAM = 2;

}  // namespace VP_CONST
