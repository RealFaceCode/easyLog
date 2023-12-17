cmake_minimum_required(VERSION 3.5)
project(easyLog VERSION 0.1.0 LANGUAGES C CXX)



add_executable(multi multi.cxx)
target_compile_features(multi PRIVATE cxx_std_20)
add_test(NAME multi COMMAND multi)

add_executable(fmt fmt.cxx)
target_compile_features(fmt PRIVATE cxx_std_20)
add_test(NAME fmt COMMAND fmt)

# # create the testing file and list of tests
# create_test_sourcelist (Tests
#     CommonCxxTests.cxx
#     multi.cxx
#     fmt.cxx
# )
# 
# # add the executable
# add_executable (CommonCxxTests ${Tests})
# target_compile_features(CommonCxxTests PRIVATE cxx_std_20)
# 
# # remove the test driver source file
# set (TestsToRun ${Tests})
# remove (TestsToRun CommonCxxTests.cxx)
# 
# # Add all the ADD_TEST for each test
# foreach (test ${TestsToRun})
#     get_filename_component (TName ${test} NAME_WE)
#     add_test (NAME ${TName} COMMAND CommonCxxTests ${TName})
# endforeach ()