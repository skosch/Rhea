#pragma OPENCL EXTENSION cl_khr_local_int32_base_atomics : enable

inline void atomicFloatAdd(volatile __global float *source, const float operand) {
    union {
        unsigned int intVal;
        float floatVal;
    } newVal;
    union {
        unsigned int intVal;
        float floatVal;
    } prevVal;

    do {
        prevVal.floatVal = *source;
        newVal.floatVal = prevVal.floatVal + operand;
    } while (atomic_cmpxchg((volatile __global unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);
}

__kernel void field_kernel( int numLetterPixels,
                           __global unsigned char *letterPixelsD,
                            int bdepth,
                            float xheight,
                            int field_width,
                           __global int *fieldD,
                            int degree,
                           __constant float *coeffsD,
                            int x1,
                            int y1,
                            int b,
                            float bactual,
                           __local float *resultbuffer,
                            int groupsize) {
  // figure out which b-value and letter pixel this thread 
  // is responsible for first.

  const int gid = get_global_id(0);
  const int lid = get_local_id(0);

  if(gid >= numLetterPixels) {
      resultbuffer[lid] = 0.0;
      return;
  }

  // get relationship to letterpixel in question.
  int ptroffs = 4 * gid;
  int x0 = (int)( letterPixelsD[ptroffs] << 8 ) + (int)letterPixelsD[ptroffs+1];
  int y0 = (int) letterPixelsD[ptroffs+2];
  int br = (int) letterPixelsD[ptroffs+3]; // right value of the letterpixel
  float vdx = (x1 - x0)/(float)xheight;
  float vdy = abs(y1 - y0)/(float)xheight;
  float vyy =  0.5*(y0 + y1)/(float)xheight;
  float vbl = (bactual+br)/(float)xheight;

  float result = 0.0;
  const int offs1 = degree + 1;
  const int offs2 = offs1 * offs1;
  const int offs3 = offs2 * offs1;
  for(int edx = 0; edx < degree+1; edx++) {
    for(int edy = 0; edy < degree+1; edy++) {
      for(int eyy = 0; eyy < degree+1; eyy++) {
        for(int eb = 0; eb < degree+1; eb++) {
           result += coeffsD[eb + offs1*eyy +
                            offs2*edy + offs3*edx]
                            * pown(vdx, edx)
                            * pown(vdy, edy)
                            * pown(vyy, eyy)
                            * pown(vbl, eb); 
        }
      }
    }
  }
  resultbuffer[lid] = result;
  barrier(CLK_LOCAL_MEM_FENCE);
  if(lid == 0) {
    float group_sum = 0.0;
    for(int i = 0; i < groupsize; i++) {
      group_sum += resultbuffer[i];
    }
    atomic_add(&(fieldD[ field_width * bdepth * y1 + bdepth * x1 + b ]), (int)(group_sum/10));
   // fieldD[ field_width * bdepth * y1 + bdepth * x1 + b ] = group_sum;
  }
  
}
                           

/*

__kernel void field_kernel(int nPairs,
                               __global unsigned char *pairsD,
                               int xheight,
                               int width,
                               int bdepth,
                               __global float *field,
                               const int degree,
                               __global float *coeffs
                               ) {

const int gid = get_global_id(0); // global memory
const int lid = get_local_id(0);  // local memory

// first make sure we're not in one of the padding threads
if(gid >= nPairs) return;

// for now, do this in a crude an inefficient way: calculate the whole 
// m'fing polynomial for the pixel-pixel-bdepth-tuple

// we should eventually try using a multivariate version
// of Horner's method to do this, but for now just loop
  int ptroffs = 8 * gid;
  int x0 = ( pairsD[ptroffs] << 8 ) + pairsD[ptroffs+1];
  int x1 = ( pairsD[ptroffs+2] << 8 ) + pairsD[ptroffs+3];
  int y0 = pairsD[ptroffs+4];
  int y1 = pairsD[ptroffs+5];
  int bl = pairsD[ptroffs+6]; // left black value of this pair
  int b_right = pairsD[ptroffs+7];
  float vdx = (x1 - x0)/(float)xheight;
  float vdy = abs(y1 - y0)/(float)xheight;
  float vyy =  0.5*(y0 + y1)/(float)xheight;
  float vbl = (bl)/(float)xheight;

float result = 0;
const int offs1 = degree + 1;
const int offs2 = offs1 * offs1;
const int offs3 = offs2 * offs1;
for(int edx = 0; edx < degree+1; edx++) {
        for(int edy = 0; edy < degree+1; edy++) {
                for(int eyy = 0; eyy < degree+1; eyy++) {
                        for(int eb = 0; eb < degree+1; eb++) {
                                result += coeffs[eb + offs1*eyy +
                                                    offs2*edy + offs3*edx] * pown(vdx, edx) *
                                pown(vdy, edy) * pown(vyy, eyy) * pown(vbl,
                                                                        eb); 
                                }
                                }
                                }
                                }

                                // now add the result to the appropriate field location
                                //atomicFloatAdd(&field[y1 * width * bdepth + x1 * bdepth + b_right], result);

                                                                                                    }
                                                                                                    

*/
