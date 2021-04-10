# KD-tree-implementation
Implementation of KD tree to find from a dataset of points, the k nearest neighbors (perform KNN query) to a particular query point. KD tress reduce the execution time for KNN query significantly because it doesn't have to traverse through all the dataset points (mostly) to find k nearest neighbors while naive algorithm calculates distance of the query point from all the dataset points and then sorts them and finds the first k points. In the worst case, time complexity of KNN search query can match the naive algorithm. 
Steps to execute the code:
> Go to the respective directory where code file is present
> run the command: g++ code.cpp
> run the command: ./a
