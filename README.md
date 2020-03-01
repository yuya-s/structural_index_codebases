# Structural Index for Conjunctive Path Queries

We provide codebase for evaluating index techniques for conjunctive path queries.
You can use this code to easily evaluate state-of-the-art indexes for conjunctive path queries on edge-labeled graphs.


## Setup

- CMake version up to 3.7

- [google dense hashing](https://github.com/sparsehash/sparsehash)

- We tested the codebase only on ubuntu 16.04. So, we are not sure whether other OS and other versions work well.

## Contents
The following are contents in this repository.
### constructing index
Our codebase provides four indexes: structural index, workload-aware structural index, path index, and workload-ware path index.

### query processing
We provide query processing algorithm with (workload-aware) structural and path indexes. Also, query processing without any indexes, which is based on bread-first-search.
Query processing with structural index also includes maintenance.

## Input Format
### Graph

The top of the line is triplet 'the number of vertices', 'the number of edges', and 'the number of labels'.
Then, it lists edges which are triplets 'vertexId1' 'vertexId2' and 'labelID'.

The following is an example of graph data.
> 100 200 8
>
> 0 1 0
>
> 0 2 1
>
> ...

Please see a file in 'data' directory for a concrete example. The data is from [robots dataset](http://tinyurl.com/gnexfoy).

### Query
Our query is following conjunctive path queries, which have 'label', 'composition', 'intersection', 'inverse', and 'identity'.
Since our codes do not have parser, we need to give a query parsing tree by ourselves.

- label is represented by 'labelId'
- composition is represented by 'j'
- intersection is represented by 'c'
- inverse is represented by 'label Id + # of labels'
- identity is represented by 'id'

Query '0->0->1->1' can be written as follows when structural or path index stores paths with 2 length.
> j
>
> 0 0
>
> 1 1

composition(identity(conjunction(inverse of (0->1), inverse of (2->3))), 2->3) can be written as follows when the number of lebels are four and indexes store paths with 2 length.
> j
>
> c_id
>
> 4 5
>
> 6 7
>
> 2 3

Please see a file in 'workload' directory for more examples.
Here, Query parser and optimization are a part of our future work.

### Workload
Workloads are the set of queries. Please add '!' at the end of queries.
Please see a file in 'workload' directory for more examples.


## Authors

This is a joint project between Yuya Sasaki and Makoto Onizuka (Osaka University) and George Fletcher (Eindhoven University of Technology).

Contact person: Yuya Sasaki, sasaki@ist.osaka-u.ac.jp

http://www-bigdata.ist.osaka-u.ac.jp/professor/sasaki/en/index.html

Don't hesitate to send us an e-mail or report an issue, if something is broken (and it shouldn't be) or if you have further questions.
