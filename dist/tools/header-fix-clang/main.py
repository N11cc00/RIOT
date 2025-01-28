# This program modifies all files so that the doxygen documentation starts inside the header
# guards indicated by the #ifndef and #define directives.
# If this is not properly done, clang-format will indent the C preprocessor directives that are
# inside the header guards.

import os
import re

def fix_header(file_path):
    with open(file_path, 'r') as file:
        content: str = file.read()
        header_guard = find_header_guard(content)
        initial_doxygen_block = find_initial_doxygen_block(content)
        end_of_doxygen_block = find_end_of_doxygen_block(content)
        endif = find_endif(content)

        # find line numbers
        header_guard_no = find_line_no_of_multiline_block(content, header_guard)
        initial_doxygen_block_no = find_line_no_of_multiline_block(content, initial_doxygen_block)
        end_of_doxygen_block_no = find_line_no_of_multiline_block(content, end_of_doxygen_block)
        endif_no = find_line_no_of_multiline_block(content, endif)
        

        header_name = find_header_name(content)
        print(f'Fixing {file_path} with header guard:\n{header_guard}')
        print(f'Initial doxygen block:\n{initial_doxygen_block}')
        print(f'End of doxygen block:\n{end_of_doxygen_block}')
        print(f'Endif:\n{endif}')
        print(f'Header name:\n{header_name}')

        # if this order isn't adhered to, the files needs to be fixed
        if not (header_guard_no < initial_doxygen_block_no < end_of_doxygen_block_no < endif_no):
            print("NEEDS FIXING")
            # TODO: implement swapping function that moves around the blocks

def find_header_name(content: str):
    header_guard = find_header_guard(content)
    header_name = re.search(r'#define\s+([A-Z_0-9]+\_H)\n', header_guard)
    if header_name is None:
        raise Exception('Header name not found')
    return header_name.group(1)
        
def find_header_guard(content: str):
    header_guard = re.search(r'#ifndef\s+[A-Z_0-9]+\s*\n#define\s+[A-Z_0-9]+\s*?\n', content)
    if header_guard is None:
        raise Exception('Header guard not found')
    return header_guard.group(0)

# the doxygen block must contain the @{ as start of the documentation
def find_initial_doxygen_block(content: str):
    doxygen_block = re.search(r'\/\*\*[\s\S]*?@\{[\s\S]*?\*\/\n', content)
    if doxygen_block is None:
        raise Exception('Doxygen block not found')
    return doxygen_block.group(0)

# the doxygen documetation ends with a block that contains the @} directive
def find_end_of_doxygen_block(content: str):
    doxygen_block = re.search(r'\/\*\*[\s]*?@\}[\s]*?\*\/\n', content)
    if doxygen_block is None:
        raise Exception('Doxygen block not found')
    return doxygen_block.group(0)

def find_endif(content: str):
    endif = re.search(r'#endif\s+\/\*.*?\*\/\n', content)
    if endif is None:
        raise Exception('End of header guard not found')
    return endif.group(0)

def find_line_no_of_multiline_block(content:str, block: str):
    block_lines = block.split("\n")
    content_lines = content.split("\n")
    for (no, line) in enumerate(content_lines):
        if block_lines[0] != line:
            continue
        for (index, line_2) in enumerate(content_lines[no:]):
            if len(block_lines) - 1 == index:
                return no
            if line_2 != block_lines[index]:
                break

    return -1


# find the line number of the initial doxygen block
def find_line_no_initial_doxygen_block(content:str, block):
    pass
    


def main():
    FILE_PATH = "/home/nico/uni/3/nfc-riot/RIOT/drivers/include/hd44780.h"
    fix_header(FILE_PATH)

if __name__ == "__main__":
    main()
