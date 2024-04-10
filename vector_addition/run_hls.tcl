# Create a project
open_project -reset proj_vector_add

# Add design files
add_files vector_addition.cpp

# Add test bench files
add_files -tb vector_addition_tb.cpp

# Set the top-level function
set_top vector_add 

# Create a solution
open_solution -reset solution1

# Define technology and clock rate
set_part  {XC7Z010}
create_clock -period 10

# Simulate our design against testbench in C
csim_design

quit
