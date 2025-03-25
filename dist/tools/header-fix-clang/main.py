# This program modifies all files so that the doxygen documentation starts inside the header
# guards indicated by the #ifndef and #define directives.
# If this is not properly done, clang-format will indent the C preprocessor directives that are
# inside the header guards.

import os
import re

RIOT_PATH = "/home/nico/uni/3/nfc-riot/RIOT/drivers/include"

# swaps areas 1 and 2, that have the lines numbers,
# in the content and returns the new content
def swap_lines(content: str, area_1: tuple, area_2: tuple) -> str:
    lines = content.split('\n')
    start_1, end_1 = area_1
    start_2, end_2 = area_2
    
    if not (0 <= start_1 <= end_1 < len(lines) and 0 <= start_2 <= end_2 < len(lines)):
        raise ValueError("Line numbers are out of range")
    if (start_1 <= start_2 <= end_1) or (start_2 <= start_1 <= end_2):
        raise ValueError("Areas cannot overlap")
    
    area_1_lines = lines[start_1:end_1 + 1]
    area_2_lines = lines[start_2:end_2 + 1]
    
    # Determine the order of areas to handle slicing correctly
    if start_1 < start_2:
        # Area 1 is before Area 2
        new_lines = (
            lines[:start_1] +              # Before area 1
            area_2_lines +                 # Area 2 goes where Area 1 was
            lines[end_1 + 1:start_2] +     # Between areas
            area_1_lines +                 # Area 1 goes where Area 2 was
            lines[end_2 + 1:]              # After area 2
        )
    else:
        # Area 2 is before Area 1
        new_lines = (
            lines[:start_2] +              # Before area 2
            area_1_lines +                 # Area 1 goes where Area 2 was
            lines[end_2 + 1:start_1] +     # Between areas
            area_2_lines +                 # Area 2 goes where Area 1 was
            lines[end_1 + 1:]              # After area 1
        )
    
    # Join lines back together with newlines
    return '\n'.join(new_lines)

def fix_header(file_path):
    with open(file_path, 'r') as file:
        print("Processing file: ", file_path)
        content: str = file.read()
        ifndef = find_ifndef(content)
        try:
            initial_doxygen_block = find_initial_doxygen_block(content)
        except:
            print(f'There is no doxygen start block for {file_path}')
            return None
        try:
            end_of_doxygen_block = find_end_of_doxygen_block(content)
        except:
            print(f'There is no doxygen end block for {file_path}') 
            return None
        endif = find_endif(content)

        # the ifndef block has size 2
        assert len(ifndef.split("\n")) == 2

        print(endif)
        # the endif block has size 1
        assert len(endif.split("\n")) == 1

        # the end of doxygen block has size 1
        assert len(end_of_doxygen_block.split("\n")) == 1

        # ATTENTION: we add another newline to the end of the ifndef block
        # to make sure that the ifndef block is separated from the doxygen block


        # find line numbers, start and end for each of the 4 blocks
        start_ifndef_no, end_ifndef_no = find_line_no_of_multiline_block(content, ifndef)
        start_initial_doxygen_block_no, end_initial_doxygen_block_no = find_line_no_of_multiline_block(content, initial_doxygen_block)
        start_end_of_doxygen_block_no, end_end_of_doxygen_block_no = find_line_no_of_multiline_block(content, end_of_doxygen_block)
        start_endif_no, end_endif_no = find_line_no_of_multiline_block(content, endif)
        

        # header_name = find_header_name(content)
        """
        print(f'Fixing {file_path} with header guard:\n{header_guard}')
        print(f'Initial doxygen block:\n{initial_doxygen_block}')
        print(f'End of doxygen block:\n{end_of_doxygen_block}')
        print(f'Endif:\n{endif}')
        print(f'Header name:\n{header_name}')
        """

        # if this order isn't adhered to, the files needs to be fixed
        if (start_ifndef_no < start_initial_doxygen_block_no < start_end_of_doxygen_block_no < start_endif_no):
            print('{file_path} is OK, no fixing needed')
            return None
        
        if (start_initial_doxygen_block_no < start_ifndef_no < start_endif_no < start_end_of_doxygen_block_no):
            print(f'Fixing {file_path}')
            content = swap_lines(content, (start_ifndef_no, end_ifndef_no), (start_initial_doxygen_block_no,end_initial_doxygen_block_no))
            content = swap_lines(content, (start_end_of_doxygen_block_no, end_end_of_doxygen_block_no), (start_endif_no, end_endif_no))
            print(f'Fixed {file_path}')
            return content 
        else:
            print(f'Error in {file_path}, cannot fix')
            print(f'ifndef: {start_ifndef_no} {end_ifndef_no}')
            print(f'initial doxygen block: {start_initial_doxygen_block_no} {end_initial_doxygen_block_no}')
            print(f'end of doxygen block: {start_end_of_doxygen_block_no} {end_end_of_doxygen_block_no}')
            print(f'endif: {start_endif_no} {end_endif_no}')
            return None
            
            

def find_header_name(content: str):
    header_guard = find_ifndef(content)
    header_name = re.search(r'#define\s+([A-Z_0-9]+\_H)', header_guard)
    if header_name is None:
        raise Exception('Header name not found')
    return header_name.group(1)
        
def find_ifndef(content: str):
    ifndef = re.search(r'#ifndef\s+[A-Z_0-9]+\s*\n#define\s+[A-Z_0-9]+\s*?', content)
    if ifndef is None:
        raise Exception('Header guard not found')
    return ifndef.group(0)

# the doxygen block must contain the @{ as start of the documentation
def find_initial_doxygen_block(content: str):
    doxygen_block = re.search(r'\/\*\*[\s\S]*?@\{[\s\S]*?\*\/', content)
    if doxygen_block is None:
        raise Exception('Start doxygen block not found')
    return doxygen_block.group(0)

# the doxygen documetation ends with a block that contains the @} directive
def find_end_of_doxygen_block(content: str):
    doxygen_block = re.search(r'\/\*\*[\s]*?@\}[\s]*?\*\/', content)
    if doxygen_block is None:
        raise Exception('End doxygen block not found')
    return doxygen_block.group(0)

def find_endif(content: str):
    endif = re.search(r'#endif[ \t]+\/\*.*?\*\/', content)
    if endif is None:
        raise Exception('End of header guard not found')
    return endif.group(0)

def find_line_no_of_multiline_block(content:str, block: str):
    block_lines = block.split("\n")
    size_of_block = len(block_lines)
    assert size_of_block > 0
    content_lines = content.split("\n")
    for (no, line) in enumerate(content_lines):
        if block_lines[0] != line:
            continue
        for (index, line_2) in enumerate(content_lines[no:]):
            if len(block_lines) - 1 == index:
                return (no, no + size_of_block - 1)
            if line_2 != block_lines[index]:
                break

    return -1

    

def test_swap_lines():
    content = """1 I like to dance
2 and swim
3 and shout and do
4 have fun
5 jippy
6 hahah
7 ok
8 yesss"""
    new_content = swap_lines(content, (0, 3), (6, 7))
    print(new_content)

def get_recursive_files(directory):
    ENDING = ".h"
    files = []
    for root, _, filenames in os.walk(directory):
        for filename in filenames:
            if filename.endswith(ENDING):
                files.append(os.path.join(root, filename))
    return files

def fix_all_headers(directory):
    files = get_recursive_files(directory)
    for file in files:
        new_content = fix_header(file)

        # overwrite the file with the new content
        if new_content is not None:
            with open(file, 'w') as f:
                f.write(new_content)

def main():
    fix_all_headers(RIOT_PATH)

if __name__ == "__main__":
    main()
