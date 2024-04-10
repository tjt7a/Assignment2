# Create a project
open_project -reset proj_matrix_multiply

# Add design files
add_files matrix_multiply.cpp

# Add test bench files
add_files -tb matrix_multiply_tb.cpp

# Set the top-level function
set_top matrix_multiply

# ########################################################
# Create a solution
open_solution -reset solution1

# Define technology and clock rate
set_part  {XC7Z010}
create_clock -period 10

csim_design

csynth_design 
cosim_design

quit
