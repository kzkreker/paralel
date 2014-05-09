__kernel
void matrixMultiplication(__global float* A, __global float* B, __global float* C,  int width, int height ) {
	int i = get_global_id(0);
	int j = get_global_id(1);
	float value=0;

	for ( int k = 0; k < width; k++) {
		value = value + A[mad(j,width,k)] * B[mad(k,height,i)];
	}
	C[mad(width,j,i)] = value;
}

__kernel void matrixVectorMultiplication(__global float* A, __global float* B, int width, int height ) {
	int i = get_global_id(0);
	int j = get_global_id(1);

	B[mad(width,j,i)] = A[mad(j ,width,i)]*5;
}

__kernel void matrixVectorSred(__global float* C, __global float* Res, int width) {
	int i = get_global_id(0);
	float sred=0;
	float num = 0;
	Res[i]=0;

	for (int j = 0; j < width; j++ ){
		sred = sred + C[mad(j,width,i)];
	}
	
	sred  = sred/width;

	for (int j = 0; j < width; j++ ){
		num = C[mad(j, width,i)] - sred;
		Res[i] =  mad(num , num , Res[i]) ;
	}

	Res[i]= Res[i] /width;	
}
