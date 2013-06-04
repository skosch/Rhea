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
#include <math.h>

#include "pngwriter.h"
#include "optionparser.h"
#include "readargs.h"
#include "rhea_config.h"
#include "letter.h"
#include "pair.h"
#include "csvrow.h"
#include "openclfuncs.h"

using namespace std;

enum optionIndex{ UNKNOWN, HELP, RASTERIZE, ANALYZE, FIT, 
  LETTERS, XHEIGHT, PPEM, FONT, RASTERIZED, PREFIT, BDEPTH, MAXDELTAX,
  PAIRS, COEFFS };
const option::Descriptor usage[] {
  { UNKNOWN, 0, "", "", option::Arg::None, "USAGE:\n"},
    { HELP, 0, "h", "help", option::Arg::None, "Print usage and exit." },
    { RASTERIZE, 0, "r", "rasterize", option::Arg::None, "rasterize"},
    { ANALYZE, 0, "a", "analyze", option::Arg::None, "analyze"},
    { FIT, 0, "f", "fit", option::Arg::None, "fit"},
    { LETTERS, 0, "", "letters", option::Arg::Optional, "letters for rasterize"},
    { XHEIGHT, 0, "", "xheight", option::Arg::Optional, "xheight"},
    { PPEM, 0, "", "ppem", option::Arg::Optional, "ppem (default: 150)"},
    { FONT, 0, "", "font", option::Arg::Optional, "font description for rasterize"},
    { RASTERIZED, 0, "", "rasterized", option::Arg::Optional, "rasterized csv"},
    { PREFIT, 0, "", "prefit", option::Arg::Optional, "prefitted distances csv"}, 
    { BDEPTH, 0, "", "bdepth", option::Arg::Optional, "number of b steps"},
    { MAXDELTAX, 0, "", "maxdeltax", option::Arg::Optional, "max xdelta from right edge of left letter to find field size"},
    { PAIRS, 0, "", "pairs", option::Arg::Optional, "pairs for analyze/fit"},
    { COEFFS, 0, "", "coeffs", option::Arg::Optional, "coeffs for fit"},
    { 0, 0, 0, 0, 0, 0} // garbage catch-all
};
// --rasterize --letters abcDEF --font "Crimson 30"
// --analyze --rasterized Crimson30.csv --pairs abcDEF
// --fit --rasterized Crimson30.csv --pairs abcDEF --coeffs coeffs.csv


void run_pair_force_matrix(vector<vector<float>> *matrix, vector<Pair*> *pairs, const int degree) {
  for(int i=0; i<matrix->size(); i++) {
    matrix->at(i).resize(pow((degree+1),4));
    //the following initialization not needed since we write directly
    //vector<int>(pow((degree+1),4),0).swap(matrix->at(i));
  }

  cout << "0% done." << flush;
  for(int i = 0; i < pairs->size(); i++) {
    pairs->at(i)->fillPairPolynomialForceMatrix(matrix, i, degree);
    //pairs->at(i)->fillPairEdgeForceMatrix(matrix, i, 10,10,10);
    //pairs->at(i)->fillPairForceMatrix(matrix, i);
    cout << "\r" << (int) (100*i/pairs->size()) << "% done (" << i << "/" << pairs->size() << ")" << flush;
  }
  cout << "\nDone. Saving matrix to output file ....\n";
} 


/******************** MAIN.CPP ********************/

int main(int argc, char* argv[]) {


  // Parse command line arguments.
  // The following would work:
  // rasterize --letters abcDEF --font "Crimson 30"
  // analyze --rasterized Crimson30.csv --pairs abcDEF
  // fit --rasterized Crimson30.csv --pairs abcDEF --coeffs coeffs.csv

  argc -= (argc > 0);
  argv += (argc > 0);
  option::Stats  stats(usage, argc, argv);
  option::Option* options = new option::Option[stats.options_max];
  option::Option* buffer  = new option::Option[stats.buffer_max];
  option::Parser parse(usage, argc, argv, options, buffer);

  if(parse.error()) {
    cout << "Couldn't parse arguments. Exiting." << endl;
    return 1;
  }

  if(options[HELP] || argc == 0) {
    option::printUsage(std::cout, usage);
    return 0;
  }

  const float xheight = (float)intFromArg(options[XHEIGHT].arg, "xheight");
  const int bdepth = intFromArg(options[BDEPTH].arg, "bdepth", 20, options[FIT]);
  const int maxdeltax = intFromArg(options[MAXDELTAX].arg, "maxdeltax", 100, options[FIT]);
  const int degree = 3;

  if(options[RASTERIZE]) {
    cout << "rasterize!" << endl;
    return 0;
  }

  /* We're not rasterizing, so we're going to be doing something with existing
   * letters. Get'em rasterized letters into letter and pair objects,
   * and start preparing the OpenCL stuff.
   */

  /**** CREATE LETTER OBJECTS ****/
  vector<Letter*> letters;
  vector<Pair*> pairs;

  // open rasterizer file
  if(options[RASTERIZED].arg == NULL) {
    cout << "No rasterized CSV file given. Exiting." << endl;
    return 1;
  }

  map<char, Letter*> letterDict;

  int capheight = 0;

  // read in the letters from RASTERIZED
  string filename(options[RASTERIZED].arg);
  ifstream csvfile(filename);
  CVSRow row;
  while(csvfile >> row)
  {
    // go through rasterized.csv and read in letter objects
    vector<bool> content;
    for(int i=0; i<row[4].length(); i++) {
      content.push_back(row[4][i] == '1' ? true : false);
    }
    Letter* l = new Letter(row[0][0], content, atoi(row[1].c_str()),
        atoi(row[2].c_str()), atoi(row[3].c_str()), xheight, 0);
    letterDict[row[0][0]] = l;

    if(l->height > capheight) capheight = l->height;
  }
  csvfile.close();

  /**** INITIALIZE OPENCL STUFF ****/

  /* OpenCL structures */
  cl_device_id device;
  cl_context context;
  cl_program program;
  cl_kernel kernel;
  cl_command_queue queue;
  cl_int err;
  size_t num_workitems, num_workitems_per_group, num_groups;

  /* Create device and context */
  device = create_device();
  context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);

  if(err < 0) {
    perror("Couldn't create a context");
    exit(1);   
  }

  /**** RUN ANALYZE ****/

  if(options[ANALYZE]) {
    // Say hello
    cout << "Hi. Welcome to the Rhea Analysis Tool, version " << string(rhea_VERSION_MAJOR) << "." << rhea_VERSION_MINOR << "!" << endl;

    filename = options[PREFIT].arg;
    ifstream csvfile(filename);
    CVSRow row;
    while(csvfile >> row)
    {
      // read in pairs from prefit.csv
      // format: xy,15
      pairs.push_back(new Pair(letterDict[(char)row[0][0]],
            letterDict[(char)row[0][1]], atoi(row[1].c_str())));
    }
    csvfile.close();
    return 0;
  }

  /********************* RUN FIT *********************/

  if(options[FIT]) {
    // Say hello
    cout << "Hi. Welcome to the Rhea Letterfitting Tool, version " << string(rhea_VERSION_MAJOR) << "." << rhea_VERSION_MINOR << "!" << endl;

    // create fresh pair objects from the letters
    for(map<char, Letter*>::const_iterator lL = letterDict.begin();
        lL != letterDict.end();
        lL++) {
      for(map<char, Letter*>::const_iterator rL = letterDict.begin();
          rL != letterDict.end();
          rL++) {
        pairs.push_back(new Pair(lL->second, rL->second, 0));
      }
    }

    // read in coefficients for fitting from the coeffs.csv file
    filename = options[COEFFS].arg;
    ifstream csvfile(filename);
    CVSRow row;
    float* coeffs = NULL;
    coeffs = (float*)malloc(sizeof(float) * pow(degree + 1, 4));
    int i = 0;
    while(csvfile >> row)
    {
      // read in pairs from coeffs.csv
      // format: 14\n3\n425.1623\n-0.005123 
      coeffs[i] = atof(row[0].c_str());
      i++;
    }
    csvfile.close();

    // prepare the OpenCL environment
    program = build_program(context, device, "field_kernel.cl");
    queue = clCreateCommandQueue(context, device, 0, &err);
    if(err < 0) {
      perror("Couldn't create a queue. Exiting.");
      exit(1);
    }
    // Create the kernel object for the field_kernel
    kernel = clCreateKernel(program, "field_kernel", &err);
    if(err < 0) {
      perror("Couldn't create a kernel");
      exit(1);
    };

    cl_mem coeffsD;

    // calculate a field for every letter
    Letter* L;
    int field_width, field_size;
    cout << " -- ";
    for(map<char, Letter*>::const_iterator lL = letterDict.begin();
        lL != letterDict.end();
        lL++) {

      L = lL->second; 
      cout << "\rNow calculating the field: " << L->getLetterChar() << endl;


      field_width = L->width + maxdeltax;
      field_size = field_width * capheight * bdepth;


      // create host and device buffers

      //unsigned char* pairsH = NULL;
      //L->createFieldPairs(num_workitems, pairsH, maxdeltax, capheight, bdepth);

      unsigned char* letterPixelsH = NULL;
      letterPixelsH = (unsigned char*)calloc(4*sizeof(unsigned char)*L->numPixels, sizeof(unsigned char));

      L->createLetterPixels(letterPixelsH);

      int* fieldH = NULL;
      fieldH = (int*)calloc(field_size*sizeof(int), sizeof(int));

      cl_mem fieldD = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
          field_size * sizeof(int), fieldH, &err);
      //      cl_mem pairsD = clCreateBuffer(context, CL_MEM_READ_ONLY,
      //         num_workitems * 7 * sizeof(unsigned char), pairsH, &err);
      cl_mem letterPixelsD = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
          L->numPixels * 4 * sizeof(unsigned char), letterPixelsH, &err);
      coeffsD =clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
          pow(degree + 1, 4) * sizeof(float), coeffs, &err);

      if(err < 0) {
        perror("Couldn't create buffers");
      }

      // calculate thread number, block size, block number
      // for now, calculate the entire field.
      num_workitems = L->numPixels;
      num_workitems_per_group = 64; // CL_DEVICE_MAX_WORK_GROUP_SIZE?
      // pad it a little so we get to an even number

      num_workitems = num_workitems_per_group *
        ceil(num_workitems/num_workitems_per_group);
      num_groups = num_workitems/num_workitems_per_group;
      cout << "Work items (pairs): " << num_workitems << endl;

      // use one field pixel for one kernel call

      for(int fieldb = 0; fieldb < bdepth; fieldb++) {
        float b = 2.5 * xheight * fieldb / (float)bdepth;  // CHANGE THIS IF NECESSARY
        // represents the actual right black pixels, in this case up to 
        for(int fieldy = 0; fieldy < capheight; fieldy++) {
          for(int fieldx = 0; fieldx < field_width; fieldx++) {

            err = clSetKernelArg(kernel, 0, sizeof(int), &num_workitems);
            err |=clSetKernelArg(kernel, 1, sizeof(cl_mem), &letterPixelsD);
            err |=clSetKernelArg(kernel, 2, sizeof(int), &bdepth);
            err |=clSetKernelArg(kernel, 3, sizeof(float), &xheight);
            err |=clSetKernelArg(kernel, 4, sizeof(int), &field_width);
            err |=clSetKernelArg(kernel, 5, sizeof(cl_mem), &fieldD);
            err |=clSetKernelArg(kernel, 6, sizeof(int), &degree);
            err |=clSetKernelArg(kernel, 7, sizeof(cl_mem), &coeffsD);
            err |=clSetKernelArg(kernel, 8, sizeof(int), &fieldx);
            err |=clSetKernelArg(kernel, 9, sizeof(int), &fieldy);
            err |=clSetKernelArg(kernel, 10, sizeof(int), &fieldb);
            err |=clSetKernelArg(kernel, 11, sizeof(float), &b);
            err |=clSetKernelArg(kernel, 12, sizeof(float) * num_workitems_per_group, NULL);
            err |=clSetKernelArg(kernel, 13, sizeof(int), &num_workitems_per_group);
            /* Enqueue kernel */
            err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &num_workitems, 
                &num_workitems_per_group, 0, NULL, NULL);

          } // end for fieldx
        } // end for fieldy
        int finish = clFinish(queue);
        cout << "FieldB: " << fieldb << "Status after kernel:" << clFinish(queue) << endl;
        if(finish < 0) {
          exit(1);
        }
      } // end for fieldb


      if(err < 0) {
        cout << "Error number:" << err << endl;
        perror("Couldn't enqueue the kernel");
        exit(1);
      }

      // get status
      cout << "Status:" << clFinish(queue) << endl;

      /* Read the kernel's output */
      err = clEnqueueReadBuffer(queue, fieldD, CL_TRUE, 0, 
          field_size*sizeof(int), fieldH, 0, NULL, NULL);
      if(err < 0) {
        cout << "Error number:" << err << endl;
        perror("Couldn't read the buffer");
      }

      for(int pl = 0; pl < L->numPixels; pl++) {
        fieldH[field_width*bdepth*L->blackpixels[pl].y + bdepth*L->blackpixels[pl].x + 0] = 0;
      }


      /* now print the field, b=0, to a pixbuf file. */
      cout << "Now putting into files" << endl;
      stringstream outfilename;
      outfilename << "./outfield_" << L->getLetterChar();

      ofstream outdata;
      outdata.open(outfilename.str());
      // do
      if(!outdata) {
        cerr << "Error: Could not create output file.\n";
        exit(1);
      }
      for(int fieldy = 0; fieldy < capheight; fieldy++) {
        for(int fieldx = 0; fieldx < field_width; fieldx++) {

          outdata << fieldH[field_width*bdepth*fieldy + bdepth*fieldx + 0] << ",";
          // png.plot(fieldx, fieldy,  ... do something with it!
        }
        outdata << endl;
      }
      outdata.close();
      cout << "Done. Goodbye." << endl;

      cout << "Field value at (0, 0, 2): " << fieldH[field_width*bdepth*2 + bdepth*2 + 0] << endl;

      cout << "Now store that field in the letter." << endl;

      L->storeField(fieldH, field_width, capheight, bdepth);

      //free(fieldH);
      free(letterPixelsH);


      clReleaseMemObject(fieldD);
      clReleaseMemObject(letterPixelsD);

      cout << "Memory released." << endl << endl;

    } // end of loop through all letters L
    clReleaseMemObject(coeffsD);
    free(coeffs);


    // now loop through all letter pairs and adjust them.
    float theta = 1.0;
    int epsilon = 5;
    for(map<char, Letter*>::const_iterator lLi = letterDict.begin();
        lLi != letterDict.end();
        lLi++) {
      Letter* lL = lLi->second; 
      int lLfw = lL->field_width;
      int lLfh = lL->field_height;
      int lLfd = lL->field_depth;
      int lLw = lL->width;
      for(map<char, Letter*>::const_iterator rLi = letterDict.begin();
          rLi != letterDict.end();
          rLi++) {
        Letter* rL = rLi->second; 
        
        int distance = (int)(xheight);  // starting value
        // calculate initial force
        int oldforce = 0;
        for(int pr = 0; pr < rL->numPixels; pr++) {
          oldforce += lL->field[lLfw*lLfd*rL->blackpixels[pr].y + lLfd*(lLw + distance + rL->blackpixels[pr].x) 
                                + min(rL->bls[pr], bdepth)];
        }
        
        while(true) {
          // adjust distance
          distance -= (int) (theta);

          // calculate new force
          int newforce = 0;
          for(int pr = 0; pr < rL->numPixels; pr++) {
            newforce += lL->field[lLfw*lLfd*rL->blackpixels[pr].y + lLfd*(lLw + distance + rL->blackpixels[pr].x) 
                                  + min(rL->bls[pr], bdepth)];
          }

          cout << "Distance:" << distance << "Force " << newforce << endl;
         /* if(abs(newforce) < abs(oldforce) || abs(newforce) <= epsilon) {
            oldforce = newforce;
          } else {
            cout << "increasing at force=" << newforce << endl;
            break;
          } */
          if(distance < -xheight) break;
        }
        cout << "Distance between " << lL->getLetterChar() << " and " << rL->getLetterChar() << ": " << distance << endl;

      }
    }




  } // end of if [FIT]

  /**************** delete stuff ******************/
  delete[] options;
  delete[] buffer;

  /* Deallocate resources */
  clReleaseKernel(kernel);
  clReleaseCommandQueue(queue);
  clReleaseProgram(program);
  clReleaseContext(context);

  return 0;
}
/*

   cerr << "Now creating array" << endl;
   std::vector<std::vector<float>> matrix;

   matrix.resize(pairs.size());
   cout << "Matrix length: " << matrix.size() << endl;
   run_pair_force_matrix(&matrix, &pairs, 3);

   cout << "Now putting into files" << endl;
   ofstream outdata;
   outdata.open("./output/polynomial_matrix");
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
   }*/
