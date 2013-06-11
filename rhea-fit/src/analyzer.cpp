#include "analyzer.h"

Analyzer::Analyzer(std::tuple< std::map< char, Letter* >, int, int, int, int > rasterizedLetters,
    std::vector< Pair* > prefitPairs, int degree) {

  std::map< char, Letter* > letterDict = std::get<0>(rasterizedLetters);
  this->xheight = std::get<1>(rasterizedLetters);
  this->capheight = std::get<2>(rasterizedLetters);
  this->maxwidth = std::get<3>(rasterizedLetters);
  this->bdepth = std::get<4>(rasterizedLetters);
  this->degree = degree;
  this->prefitPairs = prefitPairs;
  this->nMonomials = pow(degree+1, 4);
}

Analyzer::Analyzer(string matrixFilename, int degree) {
  // first, load the matrix from the file.
  
  this->degree = degree;
  this->nMonomials = pow(degree+1, 4);
  int rowCounter = 0;

  std::ifstream csvfile(matrixFilename);
  this->eigenMatrix.resize( std::count(std::istreambuf_iterator<char>(csvfile), 
             std::istreambuf_iterator<char>(), '\n'), nMonomials);
  csvfile.clear();
  csvfile.seekg(0, ios::beg);
  this->eigenMatrix.setOnes();
  CSVRow row;
  while(row << csvfile)
  {
    for(int c=0; c<nMonomials; c++) {
      eigenMatrix(rowCounter, c) = atof(row[c].c_str());
    }
    rowCounter++;
  }
  csvfile.close();
}

void Analyzer::getMatrix(){
  // prepare openCL stuff
  // go through all letter pairs
  // create equation for every letter pair
  // add equation to matrix
  // clean up openCL stuff.

  // first, set up OpenCL stuff.
/* OpenCL structures */
  cl_device_id device;
  cl_context context;
  cl_program program;
  cl_kernel kernel;
  cl_command_queue queue;
  cl_int err;
 
  cout << "Calculating matrix row for " << flush;

  size_t num_workitems, num_workitems_per_group, num_groups;

  /* Create device and context */
  device = create_device();
  context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);

  if(err < 0) {
    perror("Couldn't create a context");
    exit(1);
  }
  // prepare the OpenCL environment
  program = build_program(context, device, "analyzer_kernel.cl");
  queue = clCreateCommandQueue(context, device, 0, &err);
  if(err < 0) {
    perror("Couldn't create a queue. Exiting.");
    exit(1);
  }
  // Create the kernel object for the field_kernel
  kernel = clCreateKernel(program, "analyzer_kernel", &err);
  if(err < 0) {
    perror("Couldn't create a kernel");
    cout << "Error number:" << err << endl;
    exit(1);
  };

  // create device buffer: pcp
  const int pcp_degree_size = 4000;
  const int pcp_offset = 1000;
  const int pcp_scale = 1000;
  const float divisor = pcp_scale/(float)xheight;
  float* pcpH;
  //pcpH = new float[pcp_degree_size * (degree + 1)];
  pcpH = (float*)calloc(sizeof(float) * pcp_degree_size * (degree + 1), sizeof(float));
  for(int d = 0; d < degree + 1; d++) {
    for(int i = 0; i<pcp_degree_size; i++) {
      pcpH[d * pcp_degree_size + i] = 
        pow((float)((i - pcp_offset)/(float)pcp_scale), (float)d);
    }
  }
  

  cl_mem pcpD = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
      sizeof(float)*pcp_degree_size*(degree+1), pcpH, &err);

  this->eigenMatrix.resize(prefitPairs.size(), nMonomials + 1);
  int rowCounter = 0;
  // now loop through all pairs and create rows for them.
  for(std::vector< Pair* >::iterator pi = prefitPairs.begin();
          pi != prefitPairs.end();
          ++pi) {
    int numPixelpairs = (*pi)->numPixelpairs;

    unsigned char* pixelpairsH;
    pixelpairsH = (unsigned char*)calloc(sizeof(unsigned char) * 7 * numPixelpairs, sizeof(unsigned char));
    //pixelpairsH = new unsigned char[7 * numPixelpairs];
    (*pi)->fillPixelpairs(pixelpairsH);
    
    float* matrixRowH;
    matrixRowH = (float*)calloc(sizeof(float) * nMonomials, sizeof(float));
    //matrixRowH = new float[nMonomials];
    //for(int i=0; i<nMonomials; i++) matrixRowH[i] = 0.0;  // initialize

    // create device buffers
    cl_mem pixelpairsD = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
        sizeof(unsigned char) * 7 * numPixelpairs, pixelpairsH, &err);
    cl_mem matrixRowD = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
        sizeof(float) * nMonomials, matrixRowH, &err);


    // calculate thread number, block size, block number
    // for now, calculate the entire field.
    num_workitems = numPixelpairs;
    num_workitems_per_group = 256; // CL_DEVICE_MAX_WORK_GROUP_SIZE?
    // pad it a little so we get to an even number

    num_workitems = num_workitems_per_group *
      ceil(num_workitems/num_workitems_per_group);
    num_groups = num_workitems/num_workitems_per_group;
   
/* __kernel void analyzer_kernel(
        __constant char *pairpixelsD,
        int nP,
        float divisor,  // = 1000.0/xheight
        __global float* matrixRowD,
        __global float* pcp, // pcp = precomputed powers lookup table (-1000 to +3000)
        int pcp_degree_size,
        int pcp_offset,
        __local float* monomialResult,
        int degree,
        int nMonomials
        ) {
*/

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &pixelpairsD);
    err |=clSetKernelArg(kernel, 1, sizeof(int), &num_workitems);
    err |=clSetKernelArg(kernel, 2, sizeof(float), &divisor);
    err |=clSetKernelArg(kernel, 3, sizeof(cl_mem), &matrixRowD);
    err |=clSetKernelArg(kernel, 4, sizeof(cl_mem), &pcpD);
    err |=clSetKernelArg(kernel, 5, sizeof(int), &pcp_degree_size);
    err |=clSetKernelArg(kernel, 6, sizeof(int), &pcp_offset);
    err |=clSetKernelArg(kernel, 7, sizeof(float)*nMonomials, NULL);
    err |=clSetKernelArg(kernel, 8, sizeof(int), &degree);
    err |=clSetKernelArg(kernel, 9, sizeof(int), &nMonomials);
    /* Enqueue kernel */

    cout << "\rCalculating matrix row for " << (*pi)->getlLetter()->getLetterChar() << 
        (*pi)->getrLetter()->getLetterChar() << " (" << (int)(100.0*rowCounter/prefitPairs.size()) << "\%)" << flush;
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &num_workitems,
        &num_workitems_per_group, 0, NULL, NULL); 
  
    // making the kernels finish
    if(clFinish(queue) < 0) {
      cout << "Error!" << clFinish(queue) << endl;
      exit(1);
    }

    err = clEnqueueReadBuffer(queue, matrixRowD, CL_TRUE, 0,
          nMonomials*sizeof(float), matrixRowH, 0, NULL, NULL);
    if(clFinish(queue) < 0) {
      cout << "Error!" << clFinish(queue) << endl;
      exit(1);
    }

    // get result back out of matrixRowH
    // use pointers as iterators for constructor
    //cout << "First monomial:" << matrixRowH[0] << endl;

    for(int i=0; i<nMonomials; i++) {
      if(isnan(matrixRowH[i])) {
        cout << "Found NaN: " << rowCounter << ", " << i << endl;
        eigenMatrix.row(rowCounter).setZero();
        continue;
      }
      eigenMatrix(rowCounter, i) = matrixRowH[i];
    }
    
    clReleaseMemObject(pixelpairsD);
    clReleaseMemObject(matrixRowD);
    free(pixelpairsH);
    free(matrixRowH);

    rowCounter++;
//if(rowCounter > 10) break;
  } // end for loop through all pairs
  cout << "\rCalculated matrix rows." << endl;
  free(pcpH);
  clReleaseMemObject(pcpD);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(queue);
  clReleaseProgram(program);
  clReleaseContext(context);

  // save matrix

  std::ofstream matrixfile("outmatrix.txt");
  matrixfile.unsetf( std::ios::floatfield );
  matrixfile.precision(15);
  if(matrixfile.is_open()) {
    matrixfile << eigenMatrix << endl;
    matrixfile.close();
  } 
}

void Analyzer::findAndStoreCoeffs(string coeffsFilename) {

  // get the coefficients out of the matrix
  eigenMatrix.bottomRows(1).setOnes();
  //cout << "matrix:" << eigenMatrix << endl;
  // create b vector
  Eigen::VectorXf b(eigenMatrix.rows());
  b.setZero();
  b.tail(1)(0) = -1000;
  Eigen::VectorXf x;
  Eigen::MatrixXf pinvm;
  pinvm = this->pinv(eigenMatrix, 1E-7);
  x = pinvm * b;

  std::ofstream outfile(coeffsFilename);
  if(outfile.is_open()) {
    for(int i=0; i<nMonomials; i++) {
      outfile << x(i) << endl;
    }
    outfile.close();
  }
}


/**
 * An SVD based implementation of the Moore-Penrose pseudo-inverse
 */
Eigen::MatrixXf Analyzer::pinv(Eigen::MatrixXf& m, double epsilon = 1E-9) {
	typedef Eigen::JacobiSVD<Eigen::MatrixXf> SVD;
	SVD svd(m, Eigen::ComputeThinU | Eigen::ComputeThinV);
	typedef SVD::SingularValuesType SingularValuesType;
	const SingularValuesType singVals = svd.singularValues();
	SingularValuesType invSingVals = singVals;
	for(int i=0; i<singVals.rows(); i++) {
		if(singVals(i) <= epsilon) {
			invSingVals(i) = 0.0; // FIXED can not be safely inverted
		}
		else {
			invSingVals(i) = 1.0 / invSingVals(i);
		}
	}
  
	return Eigen::MatrixXf(svd.matrixV() *
			invSingVals.asDiagonal() *
			svd.matrixU().transpose());
}
