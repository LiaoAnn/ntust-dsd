#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include "KissTool.cpp"

using namespace std;

class DotGenerator
{
private:
    KissTool *tool;
    string dot_file;

public:
    DotGenerator(KissTool *tool, string dot_file)
    {
        this->tool = tool;
        this->dot_file = dot_file;
    }

    void GenerateDot()
    {
        ofstream fout(dot_file);

        fout << "digraph STG {" << endl;
        fout << '\t' << "rankdir=LR;" << endl;
        fout << '\t' << "INIT [shape=point];" << endl;

        for (string variable : tool->variables)
        {
            fout << '\t' << variable << " [label=\"" << variable << "\"];" << endl;
        }

        fout << endl;
        fout << '\t' << "INIT -> " << tool->resetState << ";" << endl;

        map<string, vector<string>> sameNext;

        for (int i = 0; i < tool->GetTableSize(); i++)
        {
            string input = tool->table[i][0];
            string from = tool->table[i][1];
            string to = tool->table[i][2];
            string output = tool->table[i][3];

            string key = from + "->" + to;
            string value = input + "/" + output;
            sameNext[key].push_back(value);
        }

        for (auto const &state : sameNext)
        {
            string from = state.first.substr(0, state.first.find("->"));
            string to = state.first.substr(state.first.find("->") + 2);
            fout << '\t' << from << " -> " << to << " [label=\"";
            for (int i = 0; i < state.second.size(); i++)
            {
                fout << state.second[i];
                if (i != state.second.size() - 1)
                    fout << ", ";
            }
            fout << "\"];" << endl;
        }

        fout << "}" << endl;
    }
};