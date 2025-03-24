Calling 'make' generates the executable 'driver' which performs the operations described for the assignment.

'cipher.c' has been modified to receive an array of strings, which it then incrementally encodes using a Caesar cipher defined in 'polybius.h'.

Calling './driver' will generate 10,000 random strings and output them to 'string_output.txt'. 

Every time './driver' is called, a new set of random strings is generated and an output file named 'output_****_******.txt' will be generated containing the converted strings and their original counterparts.

Calling './driver' will print a cipher key to console, which can then be used to manually verify the encoding provided.

The 'time.h' library is used to dynamically generate filenames based on the time and date of execution. If it is called twice in a second, it may overwrite an existing file. 