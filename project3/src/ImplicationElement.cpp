#pragma once
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class ImplicationElement
{
private:
    bool canMerge;

public:
    vector<string> implication; // ["A->B", "C->D"]
    ImplicationElement()
    {
        implication = vector<string>();
        this->canMerge = true;
    }

    ImplicationElement(vector<string> implication)
    {
        this->implication = implication;
        this->canMerge = true;
    }

    vector<string> GetImplication()
    {
        return this->implication;
    }

    bool CanMerge()
    {
        return this->canMerge;
    }

    void PushImplication(string implication)
    {
        // check if implication already exists
        for (string imp : this->implication)
        {
            if (imp == implication)
            {
                return;
            }
        }

        this->implication.push_back(implication);
    }

    void SetCanMerge(bool canMerge)
    {
        this->canMerge = canMerge;
    }

    bool operator==(const ImplicationElement &other) const
    {
        return this->implication == other.implication;
    }

    bool operator!=(const ImplicationElement &other) const
    {
        return !(*this == other);
    }
};