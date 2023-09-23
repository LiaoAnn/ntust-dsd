#pragma once

using namespace std;

class Item
{
public:
    int index;
    char variable;
    int elseEdge = -1; // to false branch
    int thenEdge = -1; // to true branch
    Item(int index, char variable) : index(index), variable(variable){};
    Item(int index, char variable, int elseEdge, int thenEdge) : index(index), variable(variable), elseEdge(elseEdge), thenEdge(thenEdge){};
};