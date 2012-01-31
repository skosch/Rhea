
#include "config.h"
using namespace std;


ConfigReader::ConfigReader() {

}

void ConfigReader::readConfigFile(string filePath) {
  ifstream f;
  f.open(filePath.c_str());
  if(f.is_open()) {
    getline(f, fontPath);
    f.close();
  } else {
    cout << "Unable to read config file, sorry." << endl;
  }
}

string ConfigReader::getFontPath() {
  return fontPath;
}
