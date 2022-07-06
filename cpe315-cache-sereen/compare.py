#!/usr/bin/python

"""
Script to compare decimal values. Run as ./compare.py [file1] [file2].
"""

import sys

MAX_DIFF = 1 ## The maximum deviation paired entries can have in input matrices.

## Counts the number of non-blank lines in a file. Resets the file pointer to
## the start of the file once it is finished counting the file's lines.
def count_non_blank_lines(file):

   non_blank_linecount = 0
   for line in file:
      if line.strip():
         non_blank_linecount += 1

   ## Reset file pointer to beginning of file.
   file.seek(0)

   return non_blank_linecount

def main():

   ## Only run if there are two file names as input.
   if len(sys.argv) == 3:

      ## Attempt to open the two input files.
      try:
         file_1 = open(sys.argv[1], "r")
         file_2 = open(sys.argv[2], "r")

         line_count = 0
         value_count = 0

         file_1_linecount = count_non_blank_lines(file_1)
         file_2_linecount = count_non_blank_lines(file_2)

         ## Compare number of lines in each file.
         if (file_1_linecount != file_2_linecount):
            print("Input files have different line counts, exiting.")
            sys.exit()

         ## Loop over each line in both files.
         for line_1, line_2 in zip(file_1, file_2):

            ## Quick check to ensure that both lines have the same number of elements.
            if len(line_1.split()) == len(line_2.split()):

               ## Loop over each pair of elements in each set of lines.
               for value1, value2 in zip(line_1.split(), line_2.split()):

                  ## Fail if the two values aren't within tolerance.
                  if abs(float(value1) - float(value2)) > MAX_DIFF:
                     print("Values " + value1 + " and " + value2
                        + " at [line %d: " %(line_count)\
                        + "index %d] " %(value_count) + "are outside the "\
                        + "tolerance of %d." %(MAX_DIFF))
                     sys.exit()

                  value_count += 1

            ## If the two lines to be compared are of unequal element count, exit.
            else:
               print("Line %d of the two files have different lengths, exiting." %(line_count))
               sys.exit()

            line_count += 1
            value_count = 0

         file_1.close()
         file_2.close()

         ## If successful to this point, print success message.
         print("All values within both files are within a tolerance of %d." %(MAX_DIFF))

      except IOError:
         print("Unable to open input files, exiting.")

   ## If incorrect number of inputs, print error and exit.
   else:
         print("Run as compare.py [file1] [file2].")

if __name__ == "__main__":
   main()
