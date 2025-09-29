
#include <iostream>
#include <vector>

const int SMALL_nCr[9][9] = {
//   m = 0, 1, 2, 3, 4, 5, 6, 7, 8
        {1, 0, 0, 0, 0, 0, 0, 0, 0},  // capacity = 0;
        {1, 1, 0, 0, 0, 0, 0, 0, 0},  // capacity = 1;
        {1, 2, 1, 0, 0, 0, 0, 0, 0},  // capacity = 2;
        {1, 3, 3, 1, 0, 0, 0, 0, 0},  // capacity = 3;
        {1, 4, 6, 4, 1, 0, 0, 0, 0},  // capacity = 4;
        {1, 5,10,10, 5, 1, 0, 0, 0},  // capacity = 5;
        {1, 6,15,20,15, 6, 1, 0, 0},  // capacity = 6;
        {1, 7,21,35,35,21, 7, 1, 0},  // capacity = 7;
        {1, 8,28,56,70,56,28, 8, 1},  // capacity = 8;
};

double C(int n, int r) {
    if (r < 0 || r > n) return 0;
    if (r > n-r) r = n-r;
    double ans = 1;
    for (int i = 1; i <= r; ++i) {
        ans *= n - r + i;
        ans /= i;
    }
    return ans;
}

// GPT-4.1 written code
// Generate all combinations of size r from pool starting at index start
void combine(const std::vector<int>& pool, int r, int start, std::vector<int>& current, std::vector<std::vector<int>>& results) {
    if (current.size() == r) {
        results.push_back(current);
        return;
    }
    for (int i = start; i < pool.size(); ++i) {
        current.push_back(pool[i]);
        combine(pool, r, i + 1, current, results);
        current.pop_back();
    }
}

// Recursive function to generate all combined combinations from all pools
void combineAllPools(const std::vector<std::vector<int>>& N,
                     const std::vector<int>& R,
                     int poolIndex,
                     std::vector<int>& currentCombination,
                     std::vector<std::vector<int>>& allCombinations) {
    // Base case: if all pools processed, add the combination to results
    if (poolIndex == N.size()) {
        allCombinations.push_back(currentCombination);
        return;
    }

    // Get all combinations for the current pool
    std::vector<std::vector<int>> poolCombinations;
    std::vector<int> temp;
    combine(N[poolIndex], R[poolIndex], 0, temp, poolCombinations);

    // For each combination from current pool, append it and recurse for next pool
    for (const auto& comb : poolCombinations) {
        currentCombination.insert(currentCombination.end(), comb.begin(), comb.end());
        combineAllPools(N, R, poolIndex + 1, currentCombination, allCombinations);
        currentCombination.erase(currentCombination.end() - comb.size(), currentCombination.end());
    }
}

std::vector<std::vector<int>> getCombinations(const std::vector<std::vector<int>>& N, const std::vector<int>& R) {
    std::vector<int> currentCombination;
    std::vector<std::vector<int>> allCombinations;
    combineAllPools(N, R, 0, currentCombination, allCombinations);
    return allCombinations;
}
// End of GPT-4.1 written code



