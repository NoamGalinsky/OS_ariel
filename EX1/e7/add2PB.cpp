
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        cerr << "Usage: "
             << argv[0]
             << " <name> <phone>"
             << endl;
        return 1;
    }

    string name = "";

    for (int i = 1; i < argc - 1; i++)
    {
        name += argv[i];

        if (i != argc - 2)
        {
            name += " ";
        }
    }

    string phone = argv[argc - 1];

    ofstream phonebook("phonebook.txt", ios::app);

    if (!phonebook)
    {
        cerr << "Failed to open phonebook.txt" << endl;
        return 1;
    }

    phonebook << name << "," << phone << endl;

    phonebook.close();

    return 0;
}
