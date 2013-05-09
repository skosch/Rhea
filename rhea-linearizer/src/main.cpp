// compile with something like clang++ -I/usr/include/freetype2 -lfreetype main.ccp config.cpp FT.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <cstdlib> // for rand()
#include <ctime> // for time()
#include <map>

#include "rhea_config.h"
#include "letter.h"
#include "pair.h"

using namespace std;

void run_pair_force_matrix(vector<vector<int>> *matrix, vector<Pair*> *pairs) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(256);
    vector<int>(256,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < pairs->size(); i++) {
    pairs->at(i)->fillPairPolynomialForceMatrix(matrix, i);
    //pairs->at(i)->fillPairEdgeForceMatrix(matrix, i, 10,10,10);
    //pairs->at(i)->fillPairForceMatrix(matrix, i);
    cout << "\r" << (int) (100*i/pairs->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
} 

/****************** CSV HANDLING ********************/

class CVSRow
{
  public:
    std::string const& operator[](std::size_t index) const
    {
      return m_data[index];
    }
    std::size_t size() const
    {
      return m_data.size();
    }
    void readNextRow(std::istream& str)
    {
      std::string         line;
      std::getline(str,line);

      std::stringstream   lineStream(line);
      std::string         cell;

      m_data.clear();
      while(std::getline(lineStream,cell,','))
      {
        m_data.push_back(cell);
      }
    }
  private:
    std::vector<std::string>    m_data;
};
std::istream& operator>>(std::istream& str,CVSRow& data)
{
  data.readNextRow(str);
  return str;
}


/******************** MAIN.CPP ********************/

int main(int argc, char* argv[]) {

  // Say hello
  cout << "Hi. Welcome to the Rhea Spacing Analyzer, version " << string(rhea_VERSION_MAJOR) << "." << rhea_VERSION_MINOR << "!" << endl;


  cout << "Letters 0% analyzed.";
  vector<Letter*> letters;
  vector<Pair*> pairs;
  
  // open rasterizer file
  if(argc!=2) {
    cout << "No rasterizer CSV file given." << endl;
    return 1;
  }

  string filename(argv[1]);
  ifstream csvfile(filename);

  map<char, Letter*> letterDict;

  CVSRow row;
  while(csvfile >> row)
  {
    if(row[0].length() == 1) {
      // letters
      vector<bool> content;
      for(int i=0; i<row[4].length(); i++) {
        content.push_back(row[4][i] == '1' ? true : false);
      }
      Letter* l = new Letter(row[0][0], content, atoi(row[1].c_str()),
          atoi(row[2].c_str()), atoi(row[3].c_str()), 0, 0);
      letterDict[row[0][0]] = l;

    } else if(row[0].length() == 2) {
      // pairs
      // map char to letter
      pairs.push_back(new Pair(letterDict[(char)row[0][0]],
            letterDict[(char)row[0][1]], atoi(row[1].c_str())));
    }
  }

  // Create Pair Objects
  for(int i = 0; i < letters.size(); i++) {
    for(int j = 0; j < letters.size(); j++) {
      pairs.push_back(new Pair(letters[i], letters[j], 4));
    }
  }



  cerr << "Now creating array" << endl;
  std::vector<std::vector<int>> matrix;

  matrix.resize(letters.size()*letters.size());
  cout << "Matrix length: " << matrix.size() << endl;
  run_pair_force_matrix(&matrix, &pairs);

  cout << "Now putting into files" << endl;
  ofstream outdata;
  outdata.open("/tmp/edgetraining");
  if(!outdata) {
    cerr << "Error: Could not create output file.\n";
    exit(1);
  }
  for(int i = 0; i < matrix.size(); i++) {
    for(int j = 0; j < matrix.at(1).size(); j++) {
      outdata << matrix[i][j] << ",";
    }
    outdata << endl;
  }
  outdata.close();
  cout << "Done. Goodbye." << endl;
  return 0;
}
