#include <iostream>
#include <string>
#include "plaFile.cpp"
#include "quineMcCluskey.cpp"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: " << argv[0] << " <input_pla_file> <output_pla_file>" << endl;
        return 1;
    }

    const string inputFilename = argv[1];
    const string outputFilename = argv[2];

    PLAFile plaFile;
    plaFile.load(inputFilename);

    QuineMcCluskey qm = QuineMcCluskey(plaFile);
    qm.generateAllCombinations();
    qm.simplify();
    qm.getEssentialPrimeImplicants();
    qm.outputPlaFile(outputFilename);

    cout << "Total number of terms: " << qm.getTotalNumberOfTerms() << endl;
    cout << "Total number of literals: " << qm.getTotalNumberOfLiterals() << endl;

    return 0;
}