Implementation of cache with comparison of cache friendly vs cache unfriendly matrix multiplication implementations.

To begin:
make clean && make; ./driver

It will enter an interactive user interface.

Within 0 <= idx < 2^8, you are able to read/write the index.
To read 5-th bit, enter "r 5" and press enter.
To write 1 to 5-th bit, enter "w 5 1" and press enter.
Enter "e" and press enter to end the interaction. The program will run a comparison between a cache friendly matrix multiplication and cache unfriendly matrix multiplication.

Result with n = 50:
Cache friendly:
fetch 16000000, miss 613500, miss rate: 3.83%
Cache unfriendly:
fetch 16000000, miss 1010000, miss rate: 6.31%

