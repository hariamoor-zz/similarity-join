

- Requirements

Implement SimilarityJoin(). You can add your own auxiliary function in simJoin.cc and simJoin.h

Read and Write functions are provided in main.cc. Please DO NOT modify main.cc (But you might want to change the input filenames while testing).


- Submission

Submit your simJoin.cc and simJoin.h via Canvas.

We will use our own main.cc and grading script to test your code. Make sure your code compile correctly with the provided makefile file


- Evaluation
When you finish your implementation, running SimilarityJoin should give you some outputs. The outputs include the number of result pairs. 


We also provided a binary code passjoin. It takes 2 parameters:
an integer threshold: tau
an input file: filename
input data format:
one string per line; ID is not recorded in the file, but please use the line index as the ID for each string. Note: START FROM 0! e.g., the first line has an ID of 0 and the second line has an ID of 1.

for example:
./passjoin 1 author-small
./passjoin 2 author-small

Your implementation should output the same number of result pairs as the reference binary code. For example, the above two commands should output 38 and 253 pairs, respectively.

We will grade based on the number of result pairs  and the runtime. The faster the better.

