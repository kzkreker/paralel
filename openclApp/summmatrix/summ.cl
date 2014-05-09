__kernel
void matrixMultiplication(__global float* A, __global float* B, __global float* C,  int width, int height )
{
    int i = get_global_id(0);
    int j = get_global_id(1); 
    int range = get_global_id(2); 
    
    float value=0;
    for ( int k = 0; k < width; k++)
    {
        value =  mad(A[mad(j, width, k) + range] , B[mad(k,height,i) + range], value);
    }
    C[i + width * j + range ] = value;
}
