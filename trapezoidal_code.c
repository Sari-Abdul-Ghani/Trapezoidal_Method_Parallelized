#include <mpi.h>
#include <stdio.h>
#include <math.h>

// Function to evaluate the curve (y = f(x))
float f(float x) {
    return x * x ; // Example: y = x^2
}


// Function to compute the area of a trapezoid
float trapezoid_area(float a, float b, float d) { 
    float area = 0;
    for (float x = a; x < b; x+=d) {
        area += f(x) + f(x+d);
    }
    
    return area * d / 2.0f;
}


int main(int argc, char** argv) {

    //INITIALIZATIONS


    int rank, size;
    float a = 0.0f, b = 1.0f;  // Limits of integration
    int n; //number of intervals
    float start, end, local_area, total_area; // start and end are local boundaries calculated for each processor
    double serial_start, parallel_start ; //serial_start and parallel_start are the starting time of the serial and parallel timer respectively
    double serial_end, parallel_end; //serial_end and parallel_end are the time at which  the serial and parallel timer  ends respectively
    double serial_time, parallel_time, speedup; // speedup is equal to the serial_time over parallel_time;
    
    MPI_Init(&argc, &argv); // Initialize MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Get number of processes

    if (rank == 0) {
        // Get the number of intervals from the user
        printf("Enter the number of intervals: ");
        scanf("%d", &n);
    }




    //PARALLEL APPROACH



    MPI_Barrier(MPI_COMM_WORLD); //To ensure all processors start at the same time
    parallel_start = MPI_Wtime();

    // Broadcast the number of intervals to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // Calculate the interval size for each process
    float d = (b - a) / n; // delta
    float region = (b - a)/ size;
    
    // Calculate local bounds for each process
    start = a + rank * region;
    end = start + region;
    
    // Each process calculates the area of its subinterval
    local_area = trapezoid_area(start, end, d);
    
    // Reduce all local areas to the total area on the root process
    MPI_Reduce(&local_area, &total_area, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes are finished at the same time
    parallel_end = MPI_Wtime();
    parallel_time = parallel_end - parallel_start;

    
    if (rank == 0) {
        printf("\nUSING PARALLEL APPROACH:\n\n");
        printf("The total area under the curve is: %f\n", total_area);
        printf("The total time is: %f\n\n\n", parallel_time);
    }
    


    //SEQUENTIAL APPROACH

    // Here I implement the sequential time, where I assign only 1 processor (With rank 0) to process all the area between a and b, and then 
    // print the time taken and the result. 
    if (rank == 0) {
        serial_start = MPI_Wtime();
        total_area =  trapezoid_area( a, b, d);
        serial_end = MPI_Wtime();
        serial_time = serial_end - serial_start;
        printf("USING SEQUENTIAL APPROACH:\n\n");
        printf("The total area under the curve is: %f\n", total_area);
        printf("The total time is: %f\n\n\n", serial_time);

        printf("The speedUp is: %f\n\n", serial_time/parallel_time);
    }z

    MPI_Finalize(); // Finalize MPI
    return 0;
}
