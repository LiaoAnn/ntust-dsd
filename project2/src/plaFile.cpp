#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include "term.cpp"

using namespace std;

class PLAFile
{
public:
    int numInputs;
    int numOutputs;
    vector<string> inputLabels;
    vector<string> outputLabels;
    int numProductTerms;
    vector<Term> terms;

    void load(const string &filename)
    {
        ifstream file(filename);

        if (!file.is_open())
        {
            throw runtime_error("Failed to open file");
        }

        string line;
        while (getline(file, line))
        {
            if (line[0] == '.')
            {
                istringstream iss(line);
                string directive, temp;
                iss >> directive;

                if (directive == ".i")
                    iss >> numInputs;
                else if (directive == ".o")
                    iss >> numOutputs;
                else if (directive == ".ilb")
                {
                    while (iss >> temp)
                        inputLabels.push_back(temp);
                }
                else if (directive == ".ob")
                {
                    while (iss >> temp)
                        outputLabels.push_back(temp);
                }
                else if (directive == ".p")
                    iss >> numProductTerms;
            }
            else
            {
                // Parsing product terms
                string in, out;
                istringstream iss(line);
                iss >> in >> out;
                terms.push_back(Term(in, out));
            }
        }
    }

    // for testing
    void print() const
    {
        cout << "Number of inputs: " << numInputs << endl;
        cout << "Number of outputs: " << numOutputs << endl;
        cout << "Input labels: ";
        for (const string &label : inputLabels)
        {
            cout << label << " ";
        }
        cout << endl;
        cout << "Output labels: ";
        for (const string &label : outputLabels)
        {
            cout << label << " ";
        }
        cout << endl;

        // print product terms
        cout << "Product terms: " << endl;
        for (const auto &term : terms)
        {
            cout << term.input << " " << term.output << endl;
        }
    }
};
