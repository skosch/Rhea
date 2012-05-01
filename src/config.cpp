
#include "config.h"
using namespace std;


ConfigReader::ConfigReader() {

}

void ConfigReader::readConfigFile(string filePath) {
  ifstream f;
  string tmp_ppem;
  f.open(filePath.c_str());
  if(f.is_open()) {
    getline(f, fontDesc);
    getline(f, tmp_ppem);
    ppem = atoi(tmp_ppem.c_str());
    getline(f, charSet);
    f.close();
  } else {
    cout << "Unable to read config file, sorry." << endl;
  }
}

string ConfigReader::getFontDesc() {
  return fontDesc;
}

string ConfigReader::getCharset() {
  return charSet;
}

int ConfigReader::getPPEM() {
  return ppem;
}
