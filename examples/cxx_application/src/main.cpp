#include <cxx_library/add.h>

#include <iostream>
#include <string>

using std::cout;
using std::clog;
using std::endl;
using std::stoi;

int main(int argc, char* argv[])
{
    if (argc < 3) {
        clog << "usage: " << argv[0] << " {first number} {second number}" << endl;
        return 1;
    }

    int first_number = stoi(argv[1]);
    int second_number = stoi(argv[2]);
    int result = add(first_number, second_number);

    cout << first_number << " + " << second_number << " = " << result << endl;
        
    return 0;
}
