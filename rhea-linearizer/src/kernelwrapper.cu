#include <stdio.h>
#include <cuda.h>
#include "math.h"

typedef
union
{
  int32_t i;
  struct
  {
    int16_t lo; // endian-specific!
    int16_t hi;
  };
} fixed_point;

void checkCUDAError(const char* msg);

__global__ void kernel(char *pairpixelsD, int nP, int width, unsigned char *pairsD,
    float* outmD, int degree, const int xheight, int nPairs) {

  // just use global memory for now
  // get threadID:
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if(idx >= nPairs) return;
  // first, get the first and second pixel from pairsD
  unsigned char *pairPtr = &pairsD[6*idx];
  int x0 = ( pairPtr[0] << 8 ) + pairPtr[1];
  int x1 = ( pairPtr[2] << 8 ) + pairPtr[3];
  int y0 = pairPtr[4];
  int y1 = pairPtr[5];
 
  // calculate the first three variables
  float vdx = (x1 - x0)/(float)xheight;
  float vdy = (y1 - y0)/(float)xheight;
  float vyy = 0.5*(y0 + y1)/(float)xheight;

  // now calculate amount of black
  int btotal = 0;
  int bblack = 0;
  fixed_point f;

  if(abs(y1-y0) < abs(x1-x0)) {
    int x;
    int32_t m=((int32_t)(y1-y0)<<16)/(x1-x0);

    f.i=y0<<16;
    for (x=x0;x<=x1;x++,f.i+=m)
    {
      fixed_point g=f;
      g.i+=32767;
      btotal++;

      bblack += pairpixelsD[width * g.hi + x];
    }
  } else {
    int y;
    int32_t m=((int32_t)(x1-x0)<<16)/(y1-y0);

    f.i=x0<<16;
    for (y=y0;y<=y1;y++,f.i+=m)
    {
      fixed_point g=f;
      g.i+=32767;
      btotal++;
      bblack += pairpixelsD[width * y + g.hi];
    }
  }

  float vbl = bblack/(float)btotal;

  // now calculate monomial results and store
  // should maybe be done locally and then pushed to global memory
  for(int evdx = 0; evdx <= degree; evdx++) {
    for(int evdy = 0; evdy <= degree; evdy++) {
      for(int evyy = 0; evyy <= degree; evyy++) {
        for(int evbl = 0; evbl <= degree; evbl++) {
          outmD[evbl + degree*evyy + (degree*degree)*evdy +
            (degree*degree*degree)*evdx] += powf(vdx, evdx) + powf(vdy, evdy)
            + powf(vyy, evyy) + powf(vbl, evbl);
        }
      }
    }
  }
}

void kernel_wrapper(char* pairpixelsH, int width, int height, unsigned char*
    pairsH, int nPairs, float* outmH, const int degree, const int nMonomials,
    const int xheight) {

  // create matrix with both letters in it
  // one-dimensional, with one byte per pixel, going from bottom to top,
  // left to right.
  // also, create matrix for polynomial output on device
  float *outmD;
  cudaMalloc((void**) &outmD, nMonomials*sizeof(float));
  cudaMemset(outmD, 0.f, nMonomials*sizeof(float));

  // copy matrix into CUDA memory
  char *pairpixelsD;
  cudaMalloc((void**) &pairpixelsD, width*height*sizeof(char));
  cudaMemcpy(pairpixelsD, pairpixelsH, width*height*sizeof(char), cudaMemcpyHostToDevice);

  // copy list of pixel pairs into CUDA memory
  unsigned char* pairsD;
  cudaMalloc((void**) &pairsD, nPairs*6*sizeof(unsigned char));
  
  checkCUDAError("Couldn't create pairs");
  cudaMemcpy(pairsD, pairsH, nPairs*6*sizeof(unsigned char), cudaMemcpyHostToDevice);
  checkCUDAError("Couldn't copy pairs");
  // create tons of threads with ID composed of left and right
  int nThreadsPerBlock = 512;
  int nBlocks = ceil(nPairs/(float)nThreadsPerBlock);

  // calculate number of shared memory bytes needed
  printf("xheight: %d\n", xheight);
  // call the kernel
  kernel<<< nBlocks, nThreadsPerBlock >>>( pairpixelsD, width*height, width, pairsD, outmD, degree, xheight, nPairs);
  cudaThreadSynchronize();
  checkCUDAError("Kernel Failed!");

  // copy over results
  cudaMemcpy(outmH, outmD, nMonomials*sizeof(float), cudaMemcpyDeviceToHost);

  // finally: free memory
  cudaFree(pairpixelsD);
  cudaFree(pairsD);
  cudaFree(outmD);

}

void checkCUDAError(const char *msg)
{
  cudaError_t err = cudaGetLastError();
  if( cudaSuccess != err)
  {
    fprintf(stderr, "Cuda error: %s: %s.\n", msg, cudaGetErrorString( err) );
    exit(EXIT_FAILURE);
  }
}
