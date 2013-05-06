// compile with something like clang++ -I/usr/include/freetype2 -lfreetype main.ccp config.cpp FT.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
<<<<<<< HEAD
#include <array>
#include <cstdlib> // for rand()
#include <ctime> // for time()
=======
#include <tr1/array>
>>>>>>> 701459771fe1c31d59ebbac6d1106b4c72b6d5b6

#include "rhea_config.h"
#include "config.h"
#include "FT.h"
#include "letter.h"
#include "pair.h"
#include "triplet.h"

using namespace std;

/**
 * print a force coefficient matrix for triplets.
 */
void run_triplet_force_matrix(vector<vector<int>> *matrix, vector<Triplet*> *triplets) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(2000);
    vector<int>(2000,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < triplets->size(); i++) {
    triplets->at(i)->fillTripletForceMatrix(matrix, i);
    cout << "\r" << (int) (100*i/triplets->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

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

/**
 * Try and create a huge homogenous matrix to establish
 * how the y-rows would have to be weighted for the 3rd moment to work 
 * best for triplets.
 */
void run_yweight_matrix(vector<vector<int>> *matrix, vector<Triplet*> *triplets) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(300);
    vector<int>(300,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < triplets->size(); i++) {
    triplets->at(i)->fillTripletYWeightMatrix(matrix, i);
    cout << "\r" << (int) (100*i/triplets->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

/**
 * Try and create a huge homogenous matrix to establish
 * whether or not there's a force law we can find out about.
 * Didn't work because not enough equations (matrix not full-rank)
 */
void run_dxdy_limited(vector<vector<int>> *matrix, vector<Pair*> *pairs) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(65*55);
    vector<int>(65*55,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < pairs->size(); i++) {
    pairs->at(i)->fillMatrix_dxdy_limited(matrix, 65, 55, i);
    cout << "\r" << (int) (100*i/pairs->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

/**
 * Just like the above, but tries to limit/vary the y-coordinate
 * to try and get more unique equations. Failed miserably.
 */
void run_ydxdy_limited(vector<vector<int>> *matrix,  vector<Pair*> *pairs) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(20*10*10);
    vector<int>(20*10*10,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < pairs->size(); i++) {
    pairs->at(i)->fillMatrix_ydxdy_limited(matrix, 20, 10, 10, i);
    cout << "\r" << (int) (100*i/pairs->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

/**
 * This tries to test whether Kindersley's thesis is true for pairs,
 * not just for triplets (do mathematical centers align with moment
 * centers?) ... needs to try more crazy moment types
 */

void run_pair_feature_analysis(vector<vector<int>> *matrix,  vector<Pair*> *pairs) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(3);
    vector<int>(3,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < pairs->size(); i++) {
    pairs->at(i)->findPairFeatures(matrix, i);
    cout << "\r" << (int) (100*i/pairs->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

/**
 * This runs a whole host of analyses on the pair to see if there's 
 * relationships between any of them. The number of features analyzed
 * corresponds to the number of columns in the matrix.
 */
void run_pair_proxistem_analysis(vector<vector<int>> *matrix,  vector<Pair*> *pairs) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(8);
    vector<int>(8,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < pairs->size(); i++) {
    pairs->at(i)->proxiStemAnalysis(matrix, i);
    cout << "\r" << (int) (100*i/pairs->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

/**
 * This reads out distances between letter pairs, averages, deviations etc.
 */
void run_pair_contourvar_analysis(vector<vector<int>> *matrix,  vector<Pair*> *pairs) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(4);
    vector<int>(4,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < pairs->size(); i++) {
    pairs->at(i)->fillMatrix_contourvar(matrix, i);
    cout << "\r" << (int) (100*i/pairs->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

/**
 * This runs analyses like [math center] and [third moment center]
 * on triplets of letters, to find out if there's any relation between them
 */
void run_triplet_feature_analysis(vector<vector<int>> *matrix,  vector<Triplet*> *triplets) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(4);
    vector<int>(4,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < triplets->size(); i++) {
    triplets->at(i)->findTripletFeatures(matrix, i);
    cout << "\r" << (int) (100*i/triplets->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

/**
 * This runs analyses like [math center] and [third moment center]
 * on single letters, to find out if there's any relation between them
 */
void run_letter_feature_analysis(vector<vector<int>> *matrix, vector<Letter*> *letters) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(4);
    vector<int>(4,0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < letters->size(); i++) {
    letters->at(i)->findLetterFeatures(matrix, i);
    cout << "\r" << (int) (100*i/letters->size()) << "% done." << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
}

/**
 * This lets the pair objects fill a matrix that is then used
 * to run lp_solve or least squares, to find the approximate
 * bubble shapes around the letters.
 */
void  run_pair_bubble_matrix_fill(vector<vector<vector<int>>> *bubblematrix, vector<vector<int>> *distancematrix, vector<Pair*> *pairs, vector<Letter*> *letters) {
   
    for(int i=0; i<bubblematrix->size(); i++) {
      bubblematrix->at(i).resize(pairs->size()); // 26^2 equations for height y
      for(int j=0; j<bubblematrix->at(0).size(); j++) {
	bubblematrix->at(i).at(j).resize(52); // 52 columns
	vector<int>(52,0).swap(bubblematrix->at(i).at(j));
      }
    }
    for(int i=0; i<distancematrix->size(); i++) {
      distancematrix->at(i).resize(pairs->size());
      vector<int>(pairs->size(),0).swap(distancematrix->at(i));
    }

    cout << "Now filling the bubble matrix:" << endl;
    cout << "0% done." << flush;
    for(int i = 0; i < pairs->size(); i++) {
      pairs->at(i)->fillBubbleMatrix(bubblematrix, distancematrix, i);
      cout << "\r" << (int) (100*i/pairs->size()) << "% done." << flush;
    }
    cout << "\nDone. Now optimizing for biggest bubbles:\n";

    // since lp_solve has retarded semantics, just output as a file
    // and then use read_LP

    for(int y = 0; y < pairs->at(0)->getlLetter()->xheight; y++) {

      ofstream lpfile;
      string filename;
      std::stringstream filename_builder;
      filename_builder << "lp_" << y;
      filename = filename_builder.str();

      lpfile.open(filename);
      if(!lpfile) {
	cerr << "Error: Could not create output file " << filename << ".\n";
	exit(1);
      }
    
      // a_l a_r b_l b_r ... z_r;
      for(int i=0; i<26; i++) {
	lpfile << (char)(i+97) << "_l " << (char)(i+97) << "_r ";
      }
      lpfile << ";" << endl;

      // all greater than 0
      for(int i=0; i<26; i++) {
	lpfile << (char)(i+97) << "_l >=0;\n" << (char)(i+97) << "_r >=0;\n";
      }

      // b_r + f_l <= 32
      for(int i=0; i<pairs->size(); i++) {
	for(int j=0; j<26; j++) {
	  if(bubblematrix->at(y)[i][2*j]) {
	    lpfile << (char)(j+97) << "_l+";
	  }
	  if(bubblematrix->at(y)[i][2*j+1]) {
	    lpfile << (char)(j+97) << "_r+";
	  }
	}
	lpfile << "0 <=" << distancematrix->at(y)[i] << ";" << endl;
      }
      lpfile << endl;
      lpfile.close();
      
      // solve filename
      char* filename_noconst = const_cast<char*> (filename.c_str());
      lprec* lp = read_LP(filename_noconst, 2, "lp_bubble_model");
      if(lp == NULL) cout << "Couldn't read model :(\n";
      solve(lp);
      
      /* variable values */
      REAL *row = (REAL *) malloc(52 * sizeof(*row));
      get_variables(lp, row);
      for(int j = 0; j < 52; j++) {
	//	printf("%s: %f\n", get_col_name(lp, j + 1), row[j]);
	char* colname = get_col_name(lp, j+1);
	int letterno = ((int)colname[0])-97;

	
	if (colname[2]=='l') {
	  letters->at(letterno)->setBubble(y, row[j], true);
	} else if (colname[2]=='r') {
	  letters->at(letterno)->setBubble(y, row[j], false);
	}
      }

      free(row);

      delete_lp(lp);
    } // end for all y
    
    

  }


/* run_pair_svm_horizontaldistances(&matrix, &pairs);
 * This function looks at every pair and generates training and testing data for SVM testing.
 * The features are the horizontal distance between two letters at height y \in [0, xheight]
 */

void  run_pair_svm_horizontaldistances(std::vector<std::string>* trainingmatrix, std::vector<std::string> *testingmatrix, vector<Pair*> *pairs) {
	srand(time(0));
	int r = 0;

	cout << "Working with " << pairs->size() << " pairs." << endl;
	for(int p=0; p<pairs->size(); p++) {
		stringstream data_row;
		Pair* pp = ((Pair*)(*pairs)[p]);

		// create too close, too far, and just right
		// just right first. First, put the pair ID, "g" for "good" and a "1" for good
		data_row << pp->getlLetter()->getLetterChar() << pp->getrLetter()->getLetterChar() << "g,0 ";
		// now the actual data
		for(int y=0; y<pp->getlLetter()->xheight; y++) {
			// get the distance between the letter outlines.
			data_row << y << ":" << pp->getlLetter()->rEdgeOffset[y] + pp->getrLetter()->lEdgeOffset[y] << " ";
		}
		data_row << endl;
		// too far after. First, put the pair ID, "f" for "far" and a "-1" for bad
		data_row << pp->getlLetter()->getLetterChar() << pp->getrLetter()->getLetterChar() << "f,-1 ";
		// now the actual data

		for(int y=0; y<pp->getlLetter()->xheight; y++) {
			// get the distance between the letter outlines.
			data_row << y << ":" << pp->getlLetter()->rEdgeOffset[y] + pp->getrLetter()->lEdgeOffset[y] + pp->getlLetter()->xheight/10 << " ";
		}
		data_row << endl;
		// too close after. First, put the pair ID, "c" for "close" and a "-1" for bad
		data_row << pp->getlLetter()->getLetterChar() << pp->getrLetter()->getLetterChar() << "c,1 ";
		// now the actual data
		for(int y=0; y<pp->getlLetter()->xheight; y++) {
			// get the distance between the letter outlines.
			data_row << y << ":" << pp->getlLetter()->rEdgeOffset[y] + pp->getrLetter()->lEdgeOffset[y] - pp->getlLetter()->xheight/10<< " ";
		}
		data_row << endl;
		//cout << data_row.str() << endl;

		r = (rand() % 10); // between 0 and 9
		if(r > 3) { // create training dataset
			trainingmatrix->push_back(data_row.str());

		} else { // create testing dataset
			// create too close, too far, and just right
			testingmatrix->push_back(data_row.str());
		}
	}
}





  /******************** MAIN.CPP ********************/


int main(void) {

  // Say hello
  cout << "Hi. Welcome to the Rhea Spacing Analyzer, version " << string(rhea_VERSION_MAJOR) << "." << rhea_VERSION_MINOR << "!" << endl;

  // Read in configuration
  ConfigReader* cR;
  cR = new ConfigReader();
  cR->readConfigFile("/home/sebastian/autokern/rhea/bin/rhea_config");

  // Create FreeType reader object
  FTengine* FTE;
  FTE = new FTengine();
  cout << "Loading font:" << FTE->preparePango(cR->getFontDesc(), cR->getPPEM()) << endl;
  
  string charset = cR->getCharset();

  cout << "Letters 0% analyzed.";
  vector<Letter*> letters;
  for(int i = 0; i < charset.length(); i++) {      // 97 = a, 122 = z
    cout << charset[i];
    letters.push_back(FTE->getLetter( (char) charset[i] ) );

    cout << "\rLetters " << 100*i/charset.length() << "% analyzed.";
  }
  cout << "Xheight:" << FTE->xheight << endl;
  cout << endl;
 
 // Create Pair Objects
  vector<Pair*> pairs;
  for(int i = 0; i < letters.size(); i++) {
    for(int j = 0; j < letters.size(); j++) {
      pairs.push_back(new Pair(letters[i], letters[j], FTE));
    }
  }
    
    /*
  vector<Triplet*> triplets;
  for(int i = 0; i < letters.size(); i++) {
    for(int j = 0; j < letters.size(); j++) {
      for(int k = 0; k < letters.size(); k++) {
	triplets.push_back(new Triplet(letters[i], letters[j], letters[k], FTE));
      }
    }
  }
*/


  cerr << "Now creating array" << endl;

  // Go through and run letterpair _ create datarow
/*
  std::vector<std::string> trainingmatrix, testingmatrix;
  run_pair_svm_horizontaldistances(&trainingmatrix, &testingmatrix, &triplets);
*/
  // dx, dy limited analysis
  std::vector<std::vector<int>> matrix;

  matrix.resize(charset.length()*charset.length());
  cout << "Matrix length: " << matrix.size() << endl;
  //run_pair_contourvar_analysis(&matrix, &pairs);
  run_pair_force_matrix(&matrix, &pairs);

  /*
  run_ydxdy_limited(&matrix, &pairs);

  
  run_pair_feature_analysis(&matrix, &pairs);
  run_pair_proxistem_analysis(&matrix, &pairs);


  run_letter_feature_analysis(&matrix, &letters);
  */
  /*
  std::vector<std::vector<std::vector<int>>> bubblematrix;
  std::vector<std::vector<int>> distmatrix;
  bubblematrix.resize(FTE->xheight);
  distmatrix.resize(FTE->xheight);
  
  run_pair_bubble_matrix_fill(&bubblematrix, &distmatrix, &pairs, &letters);

  // print all letter's bubble widths
  cout << "Done. Now printing the letter bubble widths:\n" << flush;
  int halfheight = (int) letters[10]->xheight/2;
  for(int i=0; i<letters.size(); i++) {
    cout << letters.at(i)->getLetterChar() << ":bwidth:" << letters.at(i)->getWidth() - letters[i]->lEdgeOffset[halfheight] + letters[i]->lBubbleExtent[halfheight] - letters[i]->rEdgeOffset[halfheight] + letters[i]->rBubbleExtent[halfheight] << endl;
  }
  for(int i=0; i<letters.size(); i++) {
    cout << letters.at(i)->getLetterChar() << ":bleft:" << -letters.at(i)->lEdgeOffset[halfheight] + letters[i]->lBubbleExtent[halfheight] << endl;
  }
  */
  //  /*

  // go through letter a and print it
  /*
  for(int y = 0; y < letters.at(0)->xheight; y++) {  
    for(int x = 0; x < letters.at(0)->getWidth(); x+=2) {
      cout << letters.at(0)->getPixel(y,x);
    }
    cout << endl;
  }
  */
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
/*  outdata.open("/tmp/testing");
    if(!outdata) {
      cerr << "Error: Could not create output file.\n";
      exit(1);
    }
    for(int i = 0; i < testingmatrix.size(); i++) {
      //for(int j = 0; j < trainingmatrix.at(1).size(); j++) {
        outdata << testingmatrix.at(i) << " ";
      //}
    }
    outdata << endl;
    outdata.close();
  //  */
    cout << "Done. Goodbye." << endl;
  return 0;
}
