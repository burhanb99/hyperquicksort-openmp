# Parallel in memory implementation of hyperquicksort / hypercubequicksort algorithm.	

## If the heap/stack memory is exceeded, segfault is returned. 

## If you have any suggestions/improvisations/bugs please add a PR or create an issue

Build from source:<br />
`g++ random_array_generator.cpp -o random`

`gcc -fopenmp hyperquicksort.c -lm`

Run:<br />
`./random <output_filename> <number_of_array_elements>`

`./a.out <inputfile.txt> <outputfile.txt> <num_threads>`