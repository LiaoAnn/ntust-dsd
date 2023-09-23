#pragma once
#include <cmath>
#include <vector>
#include <algorithm>
#include "item.cpp"
#include "plaFile.cpp"

using namespace std;

bool getValByIndex(int index, vector<pair<string, string>> terms);

vector<Item> table;
PLAFile plaFile;

void LoadPlaFile(const string &filename)
{
    plaFile.load(filename);
}

void InitTable()
{
    int runTimes = (1 << plaFile.numInputs) + 1;
    for (int i = 0; i < runTimes; i++)
    {
        // init 0 and 1
        if (i == 0)
        {
            table.push_back(Item(i, '0'));
            continue;
        }
        else if (i == runTimes - 1)
        {
            table.push_back(Item(i, '1'));
            continue;
        }

        int variableIndex = floor(log2(i));
        char v = *(plaFile.inputLabels[variableIndex].c_str());
        int falseBranch = i * 2;
        int trueBranch = i * 2 + 1;

        // terminal node
        if (variableIndex == floor(log2(runTimes - 1) - 1))
        {
            falseBranch = getValByIndex(falseBranch, plaFile.terms) ? runTimes - 1 : 0;
            trueBranch = getValByIndex(trueBranch, plaFile.terms) ? runTimes - 1 : 0;
        }
        table.push_back(Item(i, v, falseBranch, trueBranch));
    }
}

void RemoveRedundantNodes()
{
    bool isChanged = true;
    while (isChanged)
    {
        isChanged = false;

        // remove same nodes
        for (int i = 0; i < table.size(); i++)
        {
            vector<int> sameValIndexs;
            const Item &currItem = table[i];
            for (int j = i + 1; j < table.size(); j++)
            {
                const Item &nextItem = table[j];
                if (currItem.variable == nextItem.variable && currItem.elseEdge == nextItem.elseEdge && currItem.thenEdge == nextItem.thenEdge)
                {
                    sameValIndexs.push_back(j);
                }
            }

            if (sameValIndexs.size())
                isChanged = true;
            // update table
            for (int index : sameValIndexs)
            {
                for (int j = 0; j < table.size(); j++)
                {
                    if (table[j].elseEdge == table[index].index)
                        table[j].elseEdge = currItem.index;
                    if (table[j].thenEdge == table[index].index)
                        table[j].thenEdge = currItem.index;
                }
            }
            // remove same indexs
            for (int j = 0; j < sameValIndexs.size(); j++)
            {
                table.erase(table.begin() + sameValIndexs[j] - j);
            }
        }

        // when thenEdge == elseEdge, remove this node
        for (int i = 0; i < table.size(); i++)
        {
            const Item &currItem = table[i];
            if (currItem.elseEdge == currItem.thenEdge && currItem.elseEdge != -1)
            {
                isChanged = true;
                for (int j = 0; j < table.size(); j++)
                {
                    if (table[j].elseEdge == currItem.index)
                        table[j].elseEdge = currItem.elseEdge;
                    if (table[j].thenEdge == currItem.index)
                        table[j].thenEdge = currItem.elseEdge;
                }
                table.erase(table.begin() + i);
                break;
            }
        }
    }
}

void OutputDotFile(const string &filename)
{
    ofstream dotFile(filename);
    dotFile << "digraph ROBDD {" << endl;
    vector<int> sameLevel;
    char cache;
    for (int i = 0; i < table.size(); i++)
    {
        const Item &item = table[i];
        if (item.variable == '0' || item.variable == '1')
            continue;
        if (sameLevel.size() == 0)
        {
            cache = item.variable;
            sameLevel.push_back(item.index);
        }
        else if (cache != item.variable)
        {
            dotFile << "  "
                    << "{rank=same";
            for (int index : sameLevel)
            {
                dotFile << " " << index;
            }
            dotFile << "}" << endl;
            cache = item.variable;
            sameLevel.clear();
            sameLevel.push_back(item.index);
        }
        else
        {
            sameLevel.push_back(item.index);
        }
    }
    dotFile << "  "
            << "{rank=same";
    for (int index : sameLevel)
    {
        dotFile << " " << index;
    }
    dotFile << "}" << endl;
    dotFile << endl;
    for (int i = 0; i < table.size(); i++)
    {
        const Item &item = table[i];
        if (item.variable == '0' || item.variable == '1')
        {
            dotFile << "  " << item.index << " [label=" << item.variable << ",shape=bot]" << endl;
        }
        else
        {
            dotFile << "  " << item.index << " [label=\"" << item.variable << "\"]" << endl;
        }
    }
    dotFile << endl;
    for (int i = 0; i < table.size(); i++)
    {
        const Item &item = table[i];
        if (item.variable == '0' || item.variable == '1')
            continue;
        dotFile << "  " << item.index << " -> " << item.elseEdge << " [label=\"0\",style=dotted]" << endl;
        dotFile << "  " << item.index << " -> " << item.thenEdge << " [label=\"1\",style=solid]" << endl;
    }
    dotFile << "}" << endl;
}

bool getValByIndex(int index, vector<pair<string, string>> terms)
{
    // get binary representation of index
    vector<int> result; // true -> 1, false -> 0
    while (index != 1)
    {
        result.push_back(index % 2);
        index /= 2;
    }
    reverse(result.begin(), result.end());

    // get value by terms
    bool res = false;
    for (auto term : terms)
    {
        bool termResult = true;
        for (int i = 0; i < term.first.size(); i++)
        {
            if (term.first[i] == '-')
                continue;
            else if (term.first[i] == '0' && result[i] == 1)
            {
                termResult = false;
                break;
            }
            else if (term.first[i] == '1' && result[i] == 0)
            {
                termResult = false;
                break;
            }
        }
        if (termResult)
        {
            res = true;
            break;
        }
    }

    return res;
}
