#include <sstream>
#include <string>
#include <iostream>
using namespace std;

int intFromArg(const char* arg, const char* argname, int defaultval=-1, bool check=true) {
  if(!check) return 0;
  int result;
  if(arg == NULL) {
    if(defaultval > -1) return defaultval;
    cout << "No " << argname << " given. Exiting." << endl;
    exit(1);
  }
  istringstream converter(arg);
  if((converter >> result).fail()) {
    if(defaultval > -1) return defaultval;
    cout << "Could not convert " << argname << " to integer. Exiting." << endl;
  }
  converter.clear();
  return result;
}
