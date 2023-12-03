#!/bin/bash

# Specify directories
source_directory="./engine/resources/shaders/"
target_directory="./build/engine/Debug/resources/"

root=$(pwd)


# function get_shader_includes {
#     included_files=()
#     while IFS= read -r line; do
#         echo $line
#         if [[ $line =~ ^#include\s+"(.*)"\s*$ ]]; then
#             included_files+=(${BASH_REMATCH[1]})
#         fi
#     done < $1
#     return $included_files
# }


# Iterate through the items in the directory
for item in "$source_directory"*; do
    # Check if the item is a file
    if [ -f "$item" ]; then
        filename=$(basename "$item")
        directory=$(dirname "$item")
        # path=$(realpath "$item")
        # temp_dir=$(pwd)
        # cd $(dirname "$item")
        # path=$(pwd)
        # cd $temp_dir

        # echo "path: $path"
        # shader_includes=($(get_shader_includes $item))
        # # echo "${shader_includes[*]}"

        # for include in "${shader_includes[@]}"; do
        #     echo $include
        # done

        cd $(dirname "$item")
        glslc "-c" "$filename"

        new_filename=${filename//\./-}
        mv "$filename.spv" "$new_filename.spv"

        cd "$root"
        mv "$directory/$new_filename.spv" "$target_directory"
        # glslc .\cube.frag -o cube-frag.spv
    fi
done

# read -p "Press Enter to continue..."




# Add items to the associative array
# associative_array["key1"]="value1"

# # Remove an item from the associative array
# unset associative_array["key2"]

# # Print the associative array
# for key in "${!associative_array[@]}"; do
#     echo "$key: ${associative_array[$key]}"
# done



# Create an empty array
# array=()

# # Add items to the array
# array+=("item1")

# # Create an empty associative array
# declare -A associative_array

# # Remove an item from the array
# unset array[1]

# # Print the array
# echo "${array[@]}"