#pragma once
#include <iostream>
#include <string>

using namespace std;

class State
{
private:
    string next;
    string input;
    string output;

public:
    State(string next, string input, string output)
    {
        this->next = next;
        this->input = input;
        this->output = output;
    }

    string GetNext()
    {
        return this->next;
    }

    string GetInput()
    {
        return this->input;
    }

    string GetOutput()
    {
        return this->output;
    }

    void SetNext(string next)
    {
        this->next = next;
    }

    bool operator==(const State &other) const
    {
        return this->next == other.next && this->input == other.input && this->output == other.output;
    }

    bool operator!=(const State &other) const
    {
        return !(*this == other);
    }
};