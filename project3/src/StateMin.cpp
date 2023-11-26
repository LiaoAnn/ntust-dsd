#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "KissTool.cpp"
#include "State.cpp"
#include "ImplicationElement.cpp"

using namespace std;

class StateMin
{
private:
    KissTool *tool;
    string dot_file;

public:
    StateMin(KissTool *tool, string dot_file)
    {
        this->tool = tool;
        this->dot_file = dot_file;
    }

    void Minimize()
    {
        // init state map
        map<string, vector<State>> stateMap;
        vector<vector<string>> stateTable = tool->table;
        for (vector<string> row : stateTable)
        {
            string input = row[0];
            string from = row[1];
            string to = row[2];
            string output = row[3];
            State state(to, input, output);
            if (stateMap.find(from) == stateMap.end())
            {
                vector<State> states;
                stateMap[from] = states;
            }
            stateMap[from].push_back(state);
        }

        map<string, int> stateIndexMap;
        int index = 0;
        for (auto const &state : stateMap)
        {
            string from = state.first;
            stateIndexMap[from] = index;
            index++;
        }
        auto getKeyByIndex = [&](int index)
        {
            for (auto const &state : stateIndexMap)
            {
                if (state.second == index)
                    return state.first;
            }
            return string();
        };

        // init implication table
        vector<vector<ImplicationElement>> implicantionTable;
        int mapSize = stateMap.size();
        for (int i = 0; i < mapSize; i++)
        {
            vector<ImplicationElement> implicationRow;
            for (int j = 0; j < mapSize; j++)
            {
                implicationRow.push_back(ImplicationElement());
            }
            implicantionTable.push_back(implicationRow);
        }

        // set element's implications
        for (auto const &state : stateMap)
        {
            string from = state.first;
            vector<State> states = state.second;
            for (auto const &otherState : stateMap)
            {
                bool allOutputSame = true;
                string otherFrom = otherState.first;
                vector<State> otherStates = otherState.second;
                if (from == otherFrom)
                    continue;

                // check output all same
                for (State state : states)
                {
                    for (State otherState : otherStates)
                    {
                        if (state.GetInput() == otherState.GetInput() &&
                            state.GetOutput() != otherState.GetOutput())
                        {
                            allOutputSame = false;
                            break;
                        }
                    }
                    if (!allOutputSame)
                        break;
                }

                for (State state : states)
                {
                    for (State otherState : otherStates)
                    {
                        if (state.GetInput() == otherState.GetInput())
                        {
                            int row = stateIndexMap[from];
                            int col = stateIndexMap[otherFrom];
                            if (!implicantionTable[row][col].CanMerge())
                                continue;

                            if (allOutputSame)
                            {
                                implicantionTable[row][col].PushImplication(otherState.GetNext() + "->" + state.GetNext());
                                implicantionTable[col][row].PushImplication(state.GetNext() + "->" + otherState.GetNext());
                            }
                            else
                            {
                                implicantionTable[row][col].SetCanMerge(false);
                                implicantionTable[col][row].SetCanMerge(false);
                            }
                        }
                    }
                }
            }
        }

        bool changed = true;
        while (changed)
        {
            changed = false;
            for (int i = 0; i < mapSize; i++)
            {
                for (int j = 0; j < mapSize; j++)
                {
                    if (i == j || !implicantionTable[i][j].CanMerge())
                        continue;

                    vector<string> implication = implicantionTable[i][j].GetImplication();
                    for (string imp : implication)
                    {
                        string from = imp.substr(0, imp.find("->"));
                        string to = imp.substr(imp.find("->") + 2);
                        int fromIndex = stateIndexMap[from];
                        int toIndex = stateIndexMap[to];
                        if (!implicantionTable[fromIndex][toIndex].CanMerge())
                        {
                            implicantionTable[i][j].SetCanMerge(false);
                            implicantionTable[j][i].SetCanMerge(false);
                            changed = true;
                        }
                    }
                }
            }
        }

        changed = true;
        map<string, string> changedMap;
        while (changed)
        {
            changed = false;
            int canMergeRow = -1;
            int canMergeCol = -1;
            ImplicationElement canMergeElement;
            for (int i = 0; i < mapSize; i++)
            {
                for (int j = 0; j < mapSize; j++)
                {
                    if (i == j)
                        continue;
                    if (implicantionTable[i][j].CanMerge())
                    {
                        canMergeRow = i;
                        canMergeCol = j;
                        canMergeElement = implicantionTable[i][j];
                        break;
                    }
                }
                if (canMergeRow != -1 && canMergeCol != -1)
                    break;
            }

            if (canMergeRow == -1 || canMergeCol == -1)
                break;

            changed = true;
            int deletedIndex = max<int>(canMergeRow, canMergeCol);
            int keptIndex = min<int>(canMergeRow, canMergeCol);
            implicantionTable[deletedIndex][keptIndex].SetCanMerge(false);
            implicantionTable[keptIndex][deletedIndex].SetCanMerge(false);
            string deletdeKey = getKeyByIndex(deletedIndex);
            string keptKey = getKeyByIndex(keptIndex);
            changedMap[deletdeKey] = keptKey;
            for (int i = 0; i < mapSize; i++)
            {
                implicantionTable[deletedIndex][i].SetCanMerge(false);
                implicantionTable[i][deletedIndex].SetCanMerge(false);
            }
        }

        for (auto changed : changedMap)
        {
            string from = changed.first;
            string to = changed.second;
            stateMap.erase(from);
            for (auto &state : stateMap)
            {
                for (auto &otherState : state.second)
                {
                    if (otherState.GetNext() == from)
                    {
                        otherState.SetNext(to);
                    }
                }
            }
            if (tool->resetState == from)
            {
                tool->resetState = to;
            }

            for (int i = 0; i < tool->variables.size(); i++)
            {
                if (tool->variables[i] == from)
                {
                    tool->variables.erase(tool->variables.begin() + i);
                    break;
                }
            }
        }

        // generate new table for kiss tool
        vector<vector<string>> newTable;
        for (auto const &state : stateMap)
        {
            string from = state.first;
            vector<State> states = state.second;
            for (State state : states)
            {
                vector<string> row;
                row.push_back(state.GetInput());
                row.push_back(from);
                row.push_back(state.GetNext());
                row.push_back(state.GetOutput());
                newTable.push_back(row);
            }
        }

        tool->table = newTable;
    }
};