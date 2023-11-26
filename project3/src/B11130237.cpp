#include <iostream>
#include <string>
#include "KissTool.cpp"
#include "DotGenerator.cpp"
#include "StateMin.cpp"

using namespace std;

int main(int argc, char const *argv[])
{
    if (argc != 4)
    {
        cout << "Usage: " << argv[0] << " <kiss_in> <kiss_out> <dot_file>" << endl;
        return 1;
    }

    string kiss_in = argv[1];
    string kiss_out = argv[2];
    string dot_file = argv[3];

    KissTool *tool = new KissTool(kiss_in, kiss_out);
    tool->ReadKiss();

    DotGenerator *inputGenerator = new DotGenerator(tool, "input.dot");
    inputGenerator->GenerateDot();

    StateMin *stateMin = new StateMin(tool, dot_file);
    stateMin->Minimize();
    tool->WriteKiss();

    DotGenerator *outputGenerator = new DotGenerator(tool, dot_file);
    outputGenerator->GenerateDot();

    return 0;
}