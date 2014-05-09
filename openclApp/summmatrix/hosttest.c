#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <math.h>
#include <sys/time.h>
 
 
#include <CL/cl.h>
 
#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)

void shellsort(float *a,int n); 

int main(int argc, char* argv[])
{	int N;
	N = atoi(argv[1]);
	int width  = N; 
	int height = N;
  
	int i,j,k,r;
	long delta_ms; 
	gsl_rng * rgen = gsl_rng_alloc(gsl_rng_taus);
	double a,b;
 	a=31,15;
 	b=-1,444445;
	struct timeval T1, T2;

	float * A = (float *)malloc(sizeof(float)*width*height);
	float * B = (float *)malloc(sizeof(float)*width*height);
	float * C = (float *)malloc(sizeof(float)*width*height);
	float * Res = (float *)malloc(sizeof(float)*width);
	float * D= (float *)malloc(sizeof(float)*width*height);

	cl_device_id device_id = NULL;
	cl_context context = NULL;
	cl_command_queue command_queue = NULL;
	cl_mem memobjA = NULL;
	cl_mem memobjB = NULL;
	cl_mem memobjC = NULL;
	cl_mem memobjRes = NULL;
	cl_mem rowA = NULL;
	cl_mem colC = NULL;
	cl_program program = NULL;
	cl_kernel kernelMatrixMult = NULL;
	cl_kernel kernelVectMult = NULL;
	cl_kernel kernelVectSred = NULL;
	cl_platform_id platform_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_int ret;

	//char string[MEM_SIZE];

	FILE *fp;
	char fileName[] = "./multi.cl";
	char *source_str;
	size_t source_size;
	int row = width;
	int col = height;

	/* Load the source code containing the kernel*/
	fp = fopen(fileName, "r");
	if (!fp) {
		printf("Failed to load kernel.\n");
		exit(1);
	}

	source_str = (char*)malloc(MAX_SOURCE_SIZE);
	source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose( fp );

	/* Get Platform and Device Info */
	ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
	ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
	/* Create OpenCL context */
	context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
	/* Create Command Queue */
	command_queue = clCreateCommandQueue(context, device_id, 0, &ret);
	/* Create Kernel Program from the source */
	program = clCreateProgramWithSource(context, 1, (const char **)&source_str,(const size_t *)&source_size, &ret);
	/* Build Kernel Program */
	ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
	/* Create OpenCL Kernel */
	kernelMatrixMult = clCreateKernel(program, "matrixMultiplication", &ret);
	kernelVectMult = clCreateKernel(program, "matrixVectorMultiplication", &ret);
	kernelVectSred = clCreateKernel(program, "matrixVectorSred", &ret);

	/* Create Memory Buffer */
	memobjA = clCreateBuffer(context, CL_MEM_READ_WRITE, width * height * sizeof(float), NULL, &ret);
	memobjB = clCreateBuffer(context, CL_MEM_READ_WRITE, width * height * sizeof(float), NULL, &ret);
	memobjC = clCreateBuffer(context, CL_MEM_READ_WRITE, width * height * sizeof(float), NULL, &ret);
	memobjRes = clCreateBuffer(context, CL_MEM_READ_WRITE, width * sizeof(float), NULL, &ret);
	rowA = clCreateBuffer(context, CL_MEM_READ_WRITE,  sizeof(int), NULL, &ret);
	colC = clCreateBuffer(context, CL_MEM_READ_WRITE,  sizeof(int), NULL, &ret);

	gettimeofday(&T1, NULL);
        printf("Started\n");

	for(r=0; r<2; r++){
		//generate matrix
		printf("Matrix A Result  %i\n|",r);
		for(i = 0;i < width; i++) {
			for(j=0;j<height;j++) {
			    *(A+i*height+j) = gsl_ran_gamma(rgen,a,b);;
			    printf("%f|",*(A+i*height+j));
			}
			printf("\n");
		}

		// Copy the lists A and B to their respective memory buffers
		ret = clEnqueueWriteBuffer(command_queue,memobjA, CL_TRUE, 0, width * height * sizeof(float), A, 0, NULL, NULL);;
		ret = clEnqueueWriteBuffer(command_queue, rowA, CL_TRUE, 0, sizeof(int), &row, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(command_queue, colC, CL_TRUE, 0, sizeof(int), &col, 0, NULL, NULL);	
		/* Set OpenCL Kernel Arguments */
		ret = clSetKernelArg(kernelVectMult, 0, sizeof(cl_mem), (void *)&memobjA);
		ret = clSetKernelArg(kernelVectMult, 1, sizeof(cl_mem), (void *)&memobjB);
		ret = clSetKernelArg(kernelVectMult, 2, sizeof(int), (void *)&row);
		ret = clSetKernelArg(kernelVectMult, 3, sizeof(int), (void *)&col);
		/* Execute OpenCL Kernel */
		size_t globalThreads[2] = {width, height};
		size_t localThreads[2] = {1,1};
		clEnqueueNDRangeKernel(command_queue, kernelVectMult, 2, NULL, globalThreads, localThreads, NULL, 0, NULL);
		/* Copy results from the memory buffer */
		ret = clEnqueueReadBuffer(command_queue, memobjB, CL_TRUE, 0, width * height * sizeof(float),B, 0, NULL, NULL);
		
		printf("Matrix Mult Result  %i\n|",r);		
		for(i = 0;i < width; i++) {
			for(j=0;j<height;j++) {
			    printf("%f|",*(B+i*height+j));
			}
			printf("\n");
		}

		// Copy the lists A and B to their respective memory buffers
		ret = clEnqueueWriteBuffer(command_queue,memobjA, CL_TRUE, 0, width * height * sizeof(float), A, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(command_queue, memobjB, CL_TRUE, 0, width * height * sizeof(float), B, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(command_queue, rowA, CL_TRUE, 0, sizeof(int), &row, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(command_queue, colC, CL_TRUE, 0, sizeof(int), &col, 0, NULL, NULL);	
		/* Set OpenCL Kernel Arguments */
		ret = clSetKernelArg(kernelMatrixMult, 0, sizeof(cl_mem), (void *)&memobjA);
		ret = clSetKernelArg(kernelMatrixMult, 1, sizeof(cl_mem), (void *)&memobjB);
		ret = clSetKernelArg(kernelMatrixMult, 2, sizeof(cl_mem), (void *)&memobjC);
		ret = clSetKernelArg(kernelMatrixMult, 3, sizeof(int), (void *)&row);
		ret = clSetKernelArg(kernelMatrixMult, 4, sizeof(int), (void *)&col);
		/* Execute OpenCL Kernel */
		clEnqueueNDRangeKernel(command_queue, kernelMatrixMult, 2, NULL, globalThreads, localThreads, NULL, 0, NULL);
		/* Copy results from the memory buffer */
		ret = clEnqueueReadBuffer(command_queue, memobjC, CL_TRUE, 0, width * height * sizeof(float), C, 0, NULL, NULL);

		printf("Matrix Result GPU - multiplication %i\n|",r);
		for(i = 0;i < width; i++) {
			for(j=0;j<height;j++) {
			    printf("%f|",*(C + i*height + j));
			}
			printf("\n");
		}

		// Copy the lists A and B to their respective memory buffers
		ret = clEnqueueWriteBuffer(command_queue,memobjC, CL_TRUE, 0, width * height * sizeof(float), C, 0, NULL, NULL);
		ret = clEnqueueWriteBuffer(command_queue, rowA, CL_TRUE, 0, sizeof(int), &row, 0, NULL, NULL);	
		/* Set OpenCL Kernel Arguments */
		ret = clSetKernelArg(kernelVectSred, 0, sizeof(cl_mem), (void *)&memobjC);
		ret = clSetKernelArg(kernelVectSred, 1, sizeof(cl_mem), (void *)&memobjRes);
		ret = clSetKernelArg(kernelVectSred, 2, sizeof(int), (void *)&row);		
		/* Execute OpenCL Kernel */
		size_t global_item_size = 4;
		size_t local_item_size = 1;
		/* Execute OpenCL kernel as data parallel */
		ret = clEnqueueNDRangeKernel(command_queue, kernelVectSred, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
		//ret =  clEnqueueNDRangeKernel(command_queue, kernelVectSred, 2, NULL, globalThreads, localThreads, NULL, 0, NULL);
		/* Copy results from the memory buffer */
		ret = clEnqueueReadBuffer(command_queue, memobjRes, CL_TRUE, 0, width * sizeof(float),Res, 0, NULL, NULL);

		printf("Matrix Result GPU - Res vector %i\n|",r);
		for(i = 0;i < width; i++) {
			printf("%f|",*(Res+i));
		}
		printf("\n");

		shellsort(Res,width);
		
		printf("Matrix Sorted %i\n|",r);

		for(i = 0;i < width; i++) {
			printf("%f|",*(Res+i));
		}
		printf("\n");

	}
 	printf("\nStopt\n");
        gettimeofday(&T2, NULL);
    	delta_ms =  1000*(T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec)/1000;
    	printf("\nN=%d. Milliseconds passed: %ld\n", N, delta_ms);

	ret = clFlush(command_queue);
	ret = clFinish(command_queue);
	ret = clReleaseKernel(kernelVectMult);
	ret = clReleaseKernel(kernelMatrixMult);
	ret = clReleaseKernel(kernelVectSred);
	ret = clReleaseProgram(program);
	ret = clReleaseMemObject(memobjA);
	ret = clReleaseMemObject(memobjB);
	ret = clReleaseMemObject(memobjC);
	ret = clReleaseCommandQueue(command_queue);
	ret = clReleaseContext(context);
	free(source_str);	
	return 0;
}

void shellsort(float  *a,int n)
{
	int j,i,m;
	float  mid;
	for(m = n/2;m>0;m/=2){
		for(j = m;j < n;j++){
			for(i=j-m;i>=0;i-=m){
				if(*(a + i + m)>=*(a + i))
					break;
				else{
					mid = *(a+i);
					*(a + i) = *(a + i + m);
					*(a + i + m) = mid;
				}
			}
		}
	}
}
