
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>

class ConfigReader {

 public:
  ConfigReader();
  ~ConfigReader();

  void readConfigFile(std::string filePath);
  std::string getFontDesc();
  std::string getCharset();
  int getPPEM();
  //  string getExceptedFeatures();

 private:
  std::string fontDesc;
  int ppem;
  std::string charSet;
};
