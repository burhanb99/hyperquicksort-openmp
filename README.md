# H1 Parallel in memory implementation of hyperquicksort / hypercubequicksort algorithm.	

##H2 If the heap/stack memory is exceeded, segfault is returned. 

##H2 If you have any suggestions/improvisations/bugs please add a PR or create an issue

Build from source:
`g++ random_array_generator.cpp -o random`

`gcc -fopenmp hyperquicksort.c -lm`

Run:
`./random <output_filename> <number_of_array_elements>`

`>./a.out <inputfile.txt> <outputfile.txt> <num_threads>`