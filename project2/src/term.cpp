#pragma once
#include <string>
#include <vector>

using namespace std;

class Term
{
public:
    string input;
    string output;
    vector<int> minterms;
    bool checked;

    Term(const string &input, const string &output)
    {
        this->input = input;
        this->output = output;
        this->minterms = vector<int>();
        if (!inputContainsDontCare())
            this->minterms.push_back(getMinitermByInput());
    }

    Term(const string &input, const string &output, const vector<int> &minterms)
    {
        this->input = input;
        this->output = output;
        this->minterms = minterms;
    }

    void setChecked()
    {
        this->checked = true;
    }

    bool outputContainsDontCare() const
    {
        return output.find('-') != string::npos;
    }

    bool inputContainsDontCare() const
    {
        return input.find('-') != string::npos;
    }

    bool operator==(const Term &other) const
    {
        return input == other.input && output == other.output;
    }

    bool operator<(const Term &other) const
    {
        return input < other.input || (input == other.input && output < other.output);
    }

    int getMinitermByInput() const
    {
        int miniterm = 0;
        for (int i = 0; i < input.size(); i++)
        {
            if (input[i] == '1')
            {
                miniterm += 1 << (input.size() - i - 1);
            }
        }
        return miniterm;
    }
};
