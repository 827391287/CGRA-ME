#!/usr/bin/env python3

'''
/*******************************************************************************
 * The software programs comprising "CGRA-ME" and the documentation provided
 * with them are copyright by its authors and the University of Toronto. Only
 * non-commercial, not-for-profit use of this software is permitted without ex-
 * plicit permission. This software is provided "as is" with no warranties or
 * guarantees of support. See the LICENCE for more details. You should have re-
 * ceived a copy of the full licence along with this software. If not, see
 * <http://cgra-me.ece.utoronto.ca/license/>.
 ******************************************************************************/
'''

import sys
import os
import string

def loop_parser(input_source, output_source, output_tag):

    #Generate absolute path from reletive path
    dir = os.path.dirname(os.path.realpath("__file__"))
    input_src_path = os.path.join(dir, input_source)
    output_src_path = os.path.join(dir, output_source)
    output_tag_path = os.path.join(dir, output_tag)

    input_src_f = open(input_src_path, 'r')
    output_src_f = open(output_src_path, 'w')
    output_tag_f = open(output_tag_path, 'w')

    tag_string = "//DFGLoop:"
    tag_count = 1

    output_src_f.write("__attribute__((noinline)) void DFGLOOP_TAG(int index);\n")

    for line in input_src_f:
        tag_loc = line.find(tag_string)
        if tag_loc != -1: #Found a tag
            tag_name = line[tag_loc + len(tag_string) : -1].strip()
            output_tag_f.write(str(tag_count) + ' ' + tag_name + '\n')
            output_src_f.write("DFGLOOP_TAG(" + str(tag_count) + ");\n")
            tag_count+=1
        else:
            output_src_f.write(line)

    input_src_f.close()
    output_src_f.close()
    output_tag_f.close()
    return

if __name__ == "__main__":
    loop_parser(sys.argv[1], sys.argv[2], sys.argv[3])
