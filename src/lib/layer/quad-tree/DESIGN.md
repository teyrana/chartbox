



## Grid vs QuadTree vs N-Tree

Size of tree vs Width of measurment area....
- d === distance, in integers
- c === d*d, the total number of measurement cells

| Data Structure  |  Number of Nodes |  Overhead (fully-populated) 
|:------------|:---------------|:------------
| Grid                  |   --                       |       0 %
| QuadTree         |  (d^2 - 1)/3           |   266 %
| 16-Tree           |  (d^2 - 1)/15         |     53 %
| 32-Tree           |  (d^2 - 1)/31         |     25 %
| N-Ary Tree      |  (d^2 - 1)/(n - 1)   |     25 %

## Sample Depths

|   Width | Quadtree |  16-Tree | 32-Tree |
|:------|:------- |:------- |:-------|
|         2  |            1  |            1  |          1  |
|         4  |            2  |            1  |          1  |
|         8  |            3  |            1  |          1  |
|       16  |            4  |            1  |          1  |
|       32  |            5  |            2  |          1  |
|       64  |            6  |            2  |          2  |
|     128  |            7  |            2  |          2  |
|     256  |            8  |            2  |          2  |
|     512  |            9  |            3  |          2  |
|   1024  |          10  |            3  |          2  |
|   2048  |          11  |            3  |          3  |
|   4096  |          12  |            3  |          3  |
|   8192  |          13  |            4  |          3  |
| 16384  |          14  |            4  |          3  |
| 32768  |          15  |            4  |          3  |



## Cull-Ratio

How much of the tree can we expect to cull? ....

I have no idea.

