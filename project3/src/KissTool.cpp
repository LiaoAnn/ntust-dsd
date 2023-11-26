#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

class KissTool
{
private:
    string kiss_in;
    string kiss_out;

public:
    int inputNum;
    int outputNum;
    int stateNum;
    vector<vector<string>> table;
    string resetState;
    vector<string> variables;

    KissTool(string kiss_in, string kiss_out)
    {
        this->kiss_in = kiss_in;
        this->kiss_out = kiss_out;
    }

    void ReadKiss()
    {
        ifstream fin(kiss_in);
        string line;

        while (getline(fin, line))
        {
            if (line[0] == '.')
            {
                if (line == ".start_kiss")
                    continue;

                if (line[1] == 'i')
                    inputNum = stoi(line.substr(3));
                else if (line[1] == 'o')
                    outputNum = stoi(line.substr(3));
                else if (line[1] == 's')
                    stateNum = stoi(line.substr(3));
                else if (line[1] == 'r')
                    resetState = line.substr(3);
                else if (line == ".end_kiss")
                    break;
            }
            else
            {
                vector<string> row;
                string input, from, to, output;
                istringstream iss(line);

                iss >> input >> from >> to >> output;
                row.push_back(input);
                row.push_back(from);
                row.push_back(to);
                row.push_back(output);
                table.push_back(row);

                for (int i = 0; i < inputNum; i++)
                {
                    if (find(variables.begin(), variables.end(), from) == variables.end())
                        variables.push_back(from);
                    if (find(variables.begin(), variables.end(), to) == variables.end())
                        variables.push_back(to);
                }
            }
        }
    }

    void WriteKiss()
    {
        ofstream fout(kiss_out);

        fout << ".start_kiss" << endl;
        fout << ".i " << inputNum << endl;
        fout << ".o " << outputNum << endl;
        fout << ".s " << variables.size() << endl;
        fout << ".r " << resetState << endl;
        fout << ".p " << GetTableSize() << endl;

        for (vector<string> row : table)
        {
            for (int i = 0; i < row.size(); i++)
            {
                fout << row[i];
                if (i != row.size() - 1)
                    fout << " ";
            }
            fout << endl;
        }

        fout << ".end_kiss" << endl;
    }

    int GetTableSize()
    {
        return table.size();
    }
};