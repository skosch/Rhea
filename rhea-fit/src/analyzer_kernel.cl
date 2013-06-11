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
inline void atomicFloatAddLocal(volatile __local float *source, const float operand) {
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
    } while (atomic_cmpxchg((volatile __local unsigned int *)source, prevVal.intVal, newVal.intVal) != prevVal.intVal);
}

__kernel void analyzer_kernel(
        __global char *pixelpairsD,
        int nP,
        float divisor,  // = 1000.0/xheight
        __global float* matrixRowD,
        __constant float* pcp, // pcp = precomputed powers lookup table (-1000 to +3000)
        int pcp_degree_size,
        int pcp_offset,
        __local float* monomialResult,
        int degree,
        int nMonomials
        ) {

    // figure out what monomial we are from the global id
    const int gid = get_global_id(0);
    const int lid = get_local_id(0);
    if(gid >= nP) return;
    if(lid==0) {
        for(int i=0; i<nMonomials; i++) monomialResult[i] = 0.0f;
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    int x0 = ( pixelpairsD[7*gid] << 8 ) + pixelpairsD[7*gid + 1];
    int x1 = ( pixelpairsD[7*gid + 2] << 8 ) + pixelpairsD[7*gid + 3];
    int y0 = pixelpairsD[7*gid + 4];
    int y1 = pixelpairsD[7*gid + 5];

    int vdx = divisor*(x1 - x0);
    int vdy = divisor*abs(y1 - y0);
    int vyy = divisor*0.5*(y0 + y1);
    int vbl = divisor*pixelpairsD[7*gid + 6];
/*
    //precompute powers
    float vdxp[4];
    float vdyp[4];
    float vyyp[4];
    float vblp[4];

    for(int power=0; power < degree+1; power++) {
        vdxp[power] = pown(vdx, power); // can probably be replaced with hardcoded powers
        vdyp[power] = pown(vdy, power);
        vyyp[power] = pown(vyy, power);
        vblp[power] = pown(vbl, power);
    } */
    
    const int offs1 = degree + 1;
    const int offs2 = offs1 * offs1;
    const int offs3 = offs2 * offs1;    
    for(int edx = 0; edx <= degree; edx++) {
        for(int edy = 0; edy <= degree; edy++) {
            for(int eyy = 0; eyy <= degree; eyy++) {
                for(int ebl = 0; ebl <= degree; ebl++) {
                    float result = pcp[pcp_degree_size*edx+(vdx+pcp_offset)]
                        * pcp[pcp_degree_size*edy+(vdy+pcp_offset)]
                        * pcp[pcp_degree_size*eyy+(vyy+pcp_offset)]
                        * pcp[pcp_degree_size*ebl+(vbl+pcp_offset)];

                    atomicFloatAddLocal(&(monomialResult[ebl + offs1*eyy + offs2*edy +
                                offs3*edx]), result/1000000.0);
                }
            }
        }
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    // now copy shared memory
    if(lid == 0) {
        for(int i = 0; i < nMonomials; i++) {
           atomicFloatAdd(&matrixRowD[i], monomialResult[i]);
        }
    }
}

