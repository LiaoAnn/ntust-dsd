#include "robddTable.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <pla_file_name> <dot_file_name>" << endl;
        return 1;
    }

    LoadPlaFile(argv[1]);
    InitTable();
    RemoveRedundantNodes();
    OutputDotFile(argv[2]);

    return 0;
}