Number of approaches: 4

Approach 1
----------------

1:Idea: 
I created a (n/m)*(n/m) matrix, and each element is a pointer to the block at that position. 
Then I did matrix multiplication. I ignored the empty blocks. 
Time complexity for single thread:
O((n/m)^3) for matrix transversal 
+ time required to multiply each block with the required blocks


1:Why attempted: 
> Easy to parallelize the upper two loops. 
> No dependency issue in parallelization.
> No need to use any synchronization primitives.

1:Results: 
Number of threads |  Total Time taken (in seconds) | Time taken without read and write (in seconds)
1 |  27 seconds |  25 seconds
2 |  15 seconds |  13 seconds
4 |  9 seconds |  7 seconds
6 |  8 seconds |  6 seconds

1:Drawback:
> Memory overhead.
> We need to iterate O((n/m)^3) times.
> It is iterating at the indices of empty blocks too.


Approach 2 (Final Approach)
----------------

2:Idea:
To avoid the empty blocks, instead of creating a (n/m)*(n/m) matrix, I created adjacency lists of the blocks. 
> horizonal_list[i] contains the list of blocks that are in the same row as block i.
> vertical_list[i] contains the list of blocks that are in the same column as block i.
Now I did matrix multiplication. To calculate the output block at position (i,j), I multiplied all the blocks in the horizontal_list[i] and vertical_list[j] having the same column and row respectively.
> To ignore the use of locks, I used a (n/m)*(n/m) matrix (only upper half) containing pointers to the blocks.

2:Why attempted:
> This reduces the number of iterations to O(a1b1 + a2b2 + ... + anbn), where a1, a2, ..., an are the number of blocks in the horizontal_list[i] and b1, b2, ..., bn are the number of blocks in the vertical_list[j]. a1 + a2 + ... + an = k and b1 + b2 + ... + bn = k. So, the total number of iterations is O(k^2).
> This reduces the memory overhead also.
> Easy to parallelize the upper two loops.
> No dependency issue in parallelization.
> No need to use any synchronization primitives.

2:Results:
Number of threads |  Total Time taken (in seconds) | Time taken without read and write (in seconds)
1 | 11.7 seconds |  10 seconds
2 | 6.8 seconds |  5.2 seconds
4 | 4.3 seconds |  2.7 seconds
6 | 4 seconds |  2.36 seconds

> This is the best approach. It is faster than all the other approaches.

2:Drawback:
> Memory overhead of creating two adjacency lists.
> Memory overhead of creating a (n/m)*(n/m) matrix (only upper half) containing pointers to the output blocks.
> Overhead of creating the adjacency lists.
>> requires sorting the blocks in the horizontal_list[i] and vertical_list[j] according to their column and row respectively.


Approach 3
----------------

3:Idea:
Similar to approach 2, but instead of creating a (n/m)*(n/m) matrix (only upper half) containing pointers to the output blocks, I used a pushed the output blocks into a vector.
The push in the vector is not thread safe. So, I used a lock to protect the push operation.

3:Why attempted:
> Removes the memory overhead of creating a (n/m)*(n/m) matrix (only upper half) containing pointers to the output blocks.
> Rest of the properties are same as approach 2.

3:Results:
> A little slower than approach 2 (for input 2). because of the overhead of using a lock.
> This may work better if the output matrix is sparse.

3:Drawback:
> Same as approach 2 except second drawback.
> Overhead of using a lock.


Approach 4
----------------

4:Idea:
Multiplying each block with the required blocks and adding the result to the output block.
This requires use of locks to protect the output block.

4:Why attempted:
> No memory overhead, no computation overhead.

4:Results:
> Very slow compared to other approaches. 
> 30 seconds on input 2 for 1 thread.

4:Drawback:
> Not easy to parallelize. Requires use of locks.
> Highly cache unfriendly.


Final Approach: Approach 2
Results:
Number of threads |  Total Time taken (in seconds) | Time taken without read and write (in seconds)
1 | 11.7 seconds |  10 seconds
2 | 6.8 seconds |  5.2 seconds
4 | 4.3 seconds |  2.7 seconds
6 | 4 seconds |  2.36 seconds


Final scalability analysis:

Non-0 input blocks | Non-0 output blocks | 2 cores | 4 cores | 8 cores | 16 cores
----------------------------------------------------------------------------------
2^10
2^15
2^20
2^25

> CSS is not working past two days. That's why I am not able to compute scalability analysis
Scalability on my laptop:
time in ms
Non-0 input blocks | Non-0 output blocks | 2 cores | 4 cores | 8 cores  (time in ms)
----------------------------------------------------------------------
2^11               |         5839        |  136    |    92   | 112
2^12               |        11776        |  213    |   163   | 160
2^13               |        23458        |  424    |   250   | 210
2^14               |        200024       | 6804    |   4305  | 4001    



> All the tasks are independent so I have not created the task graph.