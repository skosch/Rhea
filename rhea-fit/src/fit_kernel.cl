__kernel void field_kernel(int nPairs,
    __global unsigned char *pairsD,
    __global unsigned char *pairs_leftblackD,
    output) {
  
  const int gid = get_global_id(0); // global memory
  const int lid = get_local_id(0);  // local memory



}


