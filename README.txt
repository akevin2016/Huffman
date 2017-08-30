
Kevin Almeida
keralmei@ucsc.edu

CS 12B: Data Structures with Prof. Darrell Long
Assignment 4
Data compress

Data compression and decompression using huffman encoding.
Uses various data structures.

Encodes into a binary file. Does not work well with very small files,
necessary inclusion of a tree in file causes encoded file to be bigger
than the unencoded small file.

Compile encode and decode using:
make

Or separately:
make encode
make decode

Run using:
./encode -i: -o:(optional) -v(optional)
-i is followed by input filename (required)
-o is followed by output filename (optional, defaults to standard output)
-v specifies Verbose mode

./decode -i: -o:(optional) -v(optional)
