#pragma once
#include "plaFile.cpp"
#include "term.cpp"
#include "petrickMethod.cpp"
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <iterator>
#include <iostream>

using namespace std;

class QuineMcCluskey
{
public:
    PLAFile plaFile;
    vector<Term> terms;
    vector<Term> primeImplicants;
    vector<Term> essentialPrimeImplicants;
    set<Term> allTerms;

    QuineMcCluskey(const PLAFile &plaFile)
    {
        this->plaFile = plaFile;
        for (auto &term : plaFile.terms)
        {
            terms.push_back(term);
        }
    }

    void generateAllCombinations()
    {
        for (int i = 0; i < terms.size(); i++)
        {
            const Term &term = terms[i];
            Term outputTerm(term.input, term.output);
            generateCombination(0, term, outputTerm);
        }
    }

    void generateCombination(int index, const Term &inputTerm, Term outputTerm)
    {
        if (index == inputTerm.input.size())
        {
            allTerms.insert(outputTerm);
            return;
        }

        if (inputTerm.input[index] == '-')
        {
            string temp = outputTerm.input;
            temp[index] = '0';
            Term newOutputTerm(temp, outputTerm.output);
            generateCombination(index + 1, inputTerm, newOutputTerm);
            temp[index] = '1';
            newOutputTerm = Term(temp, outputTerm.output);
            generateCombination(index + 1, inputTerm, newOutputTerm);
        }
        else
        {
            outputTerm.input[index] = inputTerm.input[index];
            generateCombination(index + 1, inputTerm, outputTerm);
        }
    }

    void simplify()
    {
        const vector<Term> terms(allTerms.begin(), allTerms.end());

        // init primeImplicantChart
        vector<vector<Term>> primeImplicantChart;
        for (int i = 0; i < plaFile.numInputs + 1; i++)
        {
            vector<Term> temp;
            primeImplicantChart.push_back(temp);
        }
        for (int i = 0; i < terms.size(); i++)
        {
            const Term &term = terms[i];
            int trueCount = 0;
            for (int j = 0; j < term.input.length(); j++)
            {
                if (term.input[j] == '1')
                {
                    trueCount += 1;
                }
            }
            primeImplicantChart[trueCount].push_back(term);
        }

        for (int i = 0; i < plaFile.numInputs; i++)
        {
            for (int currTrueCount = 0; currTrueCount < primeImplicantChart.size() - 1; currTrueCount++)
            {
                // remove same terms in primeImplicantChart
                sort(primeImplicantChart[currTrueCount].begin(), primeImplicantChart[currTrueCount].end());
                primeImplicantChart[currTrueCount].erase(unique(primeImplicantChart[currTrueCount].begin(), primeImplicantChart[currTrueCount].end()), primeImplicantChart[currTrueCount].end());

                vector<Term> &currTerms = primeImplicantChart[currTrueCount];
                vector<Term> &nextTerms = primeImplicantChart[currTrueCount + 1];
                const int currTermsSize = currTerms.size();
                const int nextTermsSize = nextTerms.size();
                for (int curr = 0; curr < currTermsSize; curr++)
                {
                    for (int next = 0; next < nextTermsSize; next++)
                    {
                        const Term &currTerm = currTerms[curr];
                        const Term &nextTerm = nextTerms[next];

                        // check both output are don't care
                        if (currTerm.output == "-" && nextTerm.output == "-")
                            continue;

                        // check if two terms are different by only one bit
                        int differentCount = 0;
                        int differentIndex = -1;
                        for (int termIndex = 0; termIndex < currTerm.input.size(); termIndex++)
                        {
                            if ((currTerm.input[termIndex] == '-' && nextTerm.input[termIndex] != '-') || (nextTerm.input[termIndex] == '-' && currTerm.input[termIndex] != '-'))
                            {
                                differentCount = -1;
                                break;
                            }
                            if (currTerm.input[termIndex] == '-' || nextTerm.input[termIndex] == '-')
                                continue;
                            if (currTerm.input[termIndex] == nextTerm.input[termIndex])
                                continue;
                            differentCount += 1;
                            differentIndex = termIndex;
                        }
                        if (differentCount != 1)
                        {
                            continue;
                        }

                        // create new term
                        string newTermInput = currTerm.input;
                        newTermInput[differentIndex] = '-';
                        vector<int> newTermMinterms;
                        newTermMinterms.insert(newTermMinterms.end(), currTerm.minterms.begin(), currTerm.minterms.end());
                        newTermMinterms.insert(newTermMinterms.end(), nextTerm.minterms.begin(), nextTerm.minterms.end());
                        Term newTerm = Term(newTermInput, "1", newTermMinterms);
                        currTerms.push_back(newTerm);
                        currTerms[curr].setChecked();
                        nextTerms[next].setChecked();
                    }
                }
            }

            // remove checked terms and output don't care terms
            for (int layer = 0; layer < primeImplicantChart.size(); layer++)
            {
                for (int l = 0; l < primeImplicantChart[layer].size(); l++)
                {
                    if (primeImplicantChart[layer][l].checked || primeImplicantChart[layer][l].outputContainsDontCare())
                    {
                        primeImplicantChart[layer].erase(primeImplicantChart[layer].begin() + l);
                        l--;
                    }
                }
            }
        }

        // get primeImplicants
        for (int layer = 0; layer < primeImplicantChart.size(); layer++)
        {
            for (int l = 0; l < primeImplicantChart[layer].size(); l++)
            {
                primeImplicants.push_back(primeImplicantChart[layer][l]);
            }
        }
    }

    void getEssentialPrimeImplicants()
    {
        vector<int> outputDontCare;
        for (int i = 0; i < terms.size(); i++)
        {
            if (terms[i].output == "-")
            {
                outputDontCare.push_back(terms[i].getMinitermByInput());
            }
        }

        // create primeImplicantMatrix
        vector<vector<bool>> primeImplicantMatrix;
        const vector<Term> terms(allTerms.begin(), allTerms.end());
        for (int i = 0; i < primeImplicants.size(); i++)
        {
            const Term &pi = primeImplicants[i];
            vector<bool> temp;
            for (int j = 0; j < terms.size(); j++)
            {
                const Term &term = terms[j];
                if (find(outputDontCare.begin(), outputDontCare.end(), term.getMinitermByInput()) != outputDontCare.end())
                {
                    temp.push_back(false);
                    continue;
                }
                temp.push_back(find(pi.minterms.begin(), pi.minterms.end(), term.getMinitermByInput()) != pi.minterms.end());
            }
            primeImplicantMatrix.push_back(temp);
        }

        // get essential prime implicants
        for (int col = 0; col < primeImplicantMatrix[0].size(); col++)
        {
            int count = 0;
            int rowIndex = -1;
            for (int row = 0; row < primeImplicantMatrix.size(); row++)
            {
                if (primeImplicantMatrix[row][col])
                {
                    count += 1;
                    rowIndex = row;
                }
            }
            if (count == 1 && find(essentialPrimeImplicants.begin(), essentialPrimeImplicants.end(), primeImplicants[rowIndex]) == essentialPrimeImplicants.end())
                essentialPrimeImplicants.push_back(primeImplicants[rowIndex]);
        }

        // convert miniterm to allTerms index using labmda
        auto minitermToColIndex = [&](int miniterm)
        {
            for (int i = 0; i < terms.size(); i++)
            {
                if (terms[i].getMinitermByInput() == miniterm)
                    return i;
            }
            return -1;
        };

        // set primeImplicantChart to false from essential prime implicants's miniterms
        for (Term essentialTerm : essentialPrimeImplicants)
        {
            for (int miniterm : essentialTerm.minterms)
            {
                int colIndex = minitermToColIndex(miniterm);
                for (int row = 0; row < primeImplicantMatrix.size(); row++)
                {
                    primeImplicantMatrix[row][colIndex] = false;
                }
            }
        }

        // clean primeImplicantMatrix and primeImplicants
        for (int i = 0; i < primeImplicants.size(); i++)
        {
            if (find(essentialPrimeImplicants.begin(), essentialPrimeImplicants.end(), primeImplicants[i]) != essentialPrimeImplicants.end())
            {
                primeImplicants.erase(primeImplicants.begin() + i);
                primeImplicantMatrix.erase(primeImplicantMatrix.begin() + i);
                i--;
            }
        }

        // remove all false rows
        for (int row = 0; row < primeImplicantMatrix.size(); row++)
        {
            bool allFalse = true;
            for (int col = 0; col < primeImplicantMatrix[0].size(); col++)
            {
                if (primeImplicantMatrix[row][col])
                {
                    allFalse = false;
                    break;
                }
            }
            if (allFalse)
            {
                primeImplicantMatrix.erase(primeImplicantMatrix.begin() + row);
                primeImplicants.erase(primeImplicants.begin() + row);
                row--;
            }
        }

        // get trueColIndexs and trueRowIndexs
        set<int> trueColIndexs;
        set<int> trueRowIndexs;
        for (int col = 0; col < primeImplicantMatrix[0].size(); col++)
        {
            for (int row = 0; row < primeImplicantMatrix.size(); row++)
            {
                if (primeImplicantMatrix[row][col])
                {
                    trueColIndexs.insert(col);
                    trueRowIndexs.insert(row);
                }
            }
        }

        if (trueColIndexs.size() == 0)
        {
            cout << "Total number of terms: " << essentialPrimeImplicants.size() << endl;
            cout << "Total number of literals: " << 0 << endl;
            return;
        }

        // create converageMatrix for Petrick's Method
        vector<int> trueColIndexVector(trueColIndexs.begin(), trueColIndexs.end());
        vector<int> trueRowIndexVector(trueRowIndexs.begin(), trueRowIndexs.end());
        vector<vector<bool>> coverageMatrix;
        for (int rowIndex : trueRowIndexVector)
        {
            vector<bool> temp;
            for (int colIndex : trueColIndexVector)
            {
                temp.push_back(primeImplicantMatrix[rowIndex][colIndex]);
            }
            coverageMatrix.push_back(temp);
        }

        // get others prime implicants using Petrick's Method
        PetrickMethod petrickMethod;
        vector<Term> petrickResult = petrickMethod.simplify(primeImplicants, coverageMatrix);
        for (Term term : petrickResult)
        {
            essentialPrimeImplicants.push_back(term);
        }

        cout << "Total number of terms: " << essentialPrimeImplicants.size() << endl;
        cout << "Total number of literals: " << petrickMethod.getNumberOfLiterals() << endl;
    }

    void outputPlaFile(const string &filename)
    {
        ofstream file(filename);

        if (!file.is_open())
        {
            throw runtime_error("Failed to open file");
        }

        file << ".i " << plaFile.numInputs << endl;
        file << ".o " << plaFile.numOutputs << endl;
        file << ".ilb";
        for (string label : plaFile.inputLabels)
        {
            file << " " << label;
        }
        file << endl;
        file << ".ob";
        for (string label : plaFile.outputLabels)
        {
            file << " " << label;
        }
        file << endl;
        file << ".p " << essentialPrimeImplicants.size() << endl;
        for (Term term : essentialPrimeImplicants)
        {
            file << term.input << " " << term.output << endl;
        }
        file << ".e" << endl;

        file.close();
    }

    // for testing
    void printPrimeImplicants() const
    {
        for (auto &term : primeImplicants)
        {
            cout << term.input << " " << term.output << endl;
        }
    }
};