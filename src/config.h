
#include <string>
#include <iostream>
#include <fstream>

class ConfigReader {

 public:
  ConfigReader();
  ~ConfigReader();

  void readConfigFile(std::string filePath);
  std::string getFontPath();
  std::string getCharset();
  //  string getExceptedFeatures();

 private:
  std::string fontPath;
  std::string charSet;
};
