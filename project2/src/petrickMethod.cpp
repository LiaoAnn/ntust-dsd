#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "term.cpp"

using namespace std;

class PetrickMethod
{
private:
    vector<vector<Term>> combinations;

public:
    vector<Term> simplify(const vector<Term> &primeImplicants,
                          const vector<vector<bool>> &coverageMatrix)
    {
        // Generating Petrick's Table
        vector<vector<Term>> petrickTable;
        for (int column = 0; column < coverageMatrix[0].size(); column++)
        {
            vector<Term> currentColumn;
            for (int row = 0; row < coverageMatrix.size(); row++)
            {
                if (coverageMatrix[row][column])
                {
                    currentColumn.push_back(primeImplicants[row]);
                }
            }
            petrickTable.push_back(currentColumn);
        }

        // Generating all combinations
        gerneateAllCombinations(petrickTable, vector<Term>());

        // Removing duplicates
        for (int i = 0; i < combinations.size(); i++)
        {
            vector<Term> curr = combinations[i];
            for (int j = i + 1; j < combinations.size(); j++)
            {
                if (curr.size() == combinations[j].size())
                {
                    bool isDuplicate = true;
                    for (int k = 0; k < curr.size(); k++)
                    {
                        if (find(combinations[j].begin(), combinations[j].end(), curr[k]) == combinations[j].end())
                        {
                            isDuplicate = false;
                            break;
                        }
                    }
                    if (isDuplicate)
                    {
                        combinations.erase(combinations.begin() + j);
                        j--;
                    }
                }
            }
        }

        // Finding the minimum combination
        int minSize = 1000000;
        vector<Term> minCombination;
        for (int i = 0; i < combinations.size(); i++)
        {
            if (combinations[i].size() < minSize)
            {
                minSize = combinations[i].size();
                minCombination = combinations[i];
            }
        }

        return minCombination;
    }

    void gerneateAllCombinations(const vector<vector<Term>> petrickTable, vector<Term> curr, int currIndex = 0)
    {
        if (currIndex == petrickTable.size())
        {
            combinations.push_back(curr);
            return;
        }
        for (int i = 0; i < petrickTable[currIndex].size(); i++)
        {
            vector<Term> newCurr = curr;
            if (find(newCurr.begin(), newCurr.end(), petrickTable[currIndex][i]) == newCurr.end())
                newCurr.push_back(petrickTable[currIndex][i]);
            gerneateAllCombinations(petrickTable, newCurr, currIndex + 1);
        }
    }

    int getNumberOfLiterals()
    {
        return combinations.size();
    }
};
