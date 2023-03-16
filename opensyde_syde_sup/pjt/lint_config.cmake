#set(PC_LINT_CONFIG_DIR "./../temp_CMakeLists_Debug/pclint")

set(PC_LINT_CONFIG_DIR "pclint_config")


function(add_pc_lint target)

    # get the include_directories of the target
    get_target_property(target_includes ${target} INCLUDE_DIRECTORIES)
    
    # get the linked libraries of current target, if there are any
    get_target_property(link_libs ${target} LINK_LIBRARIES)
    
    # following eachfor-loop in case there is more than one linked library
    set(link_libs_directories_tmp)
    set(link_libs_directories_all)
    # go through the link_libs list and check if the current item is an actual target.
    foreach (lib_tmp ${link_libs})
        # if it's true get the INCLUDE_DIRECTORIES of the link_lib and append to list
        # if false, do nothing, we do not need this lib, as it is 3rd party
        if(TARGET ${lib_tmp})
            get_target_property(link_libs_directories_tmp ${lib_tmp} INCLUDE_DIRECTORIES)
            foreach (lib ${link_libs_directories_tmp})
                list(APPEND link_libs_directories_all ${lib})
            endforeach (lib)
        endif()
    endforeach (lib_tmp)
        
    # merge link_libs and target_includes in one list
    set(all_includes)

    # join the lists with target_includes and link_libs in one
    foreach (dir ${target_includes})
        list(APPEND all_includes ${dir})
    endforeach (dir)
    foreach (dir ${link_libs_directories_all})
        list(APPEND all_includes ${dir})
    endforeach (dir)

    # make the directories "indirects" for pclint
    # set(all_includes_transformed)
    # foreach (dir ${all_includes})
        # list(APPEND all_includes_transformed -i${dir})
    # endforeach (dir)
    
    # list of all source files
    set(sources)
    foreach(sourcefile ${ARGN})
    # only include c and cpp files
        if( sourcefile MATCHES \\.c$|\\.cxx$|\\.cpp$ )
            # make filename absolute
            get_filename_component(sourcefile_abs ${sourcefile} ABSOLUTE)
            # add absolute filename to list
            list(APPEND sources ${sourcefile_abs})
        endif()
    endforeach()
    
    # beautify the lists for output
    string(REPLACE ";" "\n" all_includes "${all_includes}")
    string(APPEND all_includes "\n")
    
    string(REPLACE ";" "\n" sources "${sources}")
    string(APPEND sources "\n")
    
    # generate two output files per build target needed for lint, put in a separate folder
    file(GENERATE OUTPUT "${PC_LINT_CONFIG_DIR}/${target}/${target}_lint_include_dirs.txt" CONTENT "${all_includes}")
    file(GENERATE OUTPUT "${PC_LINT_CONFIG_DIR}/${target}/${target}_lint_sources.txt" CONTENT "${sources}")
    
endfunction(add_pc_lint)
