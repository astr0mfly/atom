-- include sub-projects
includes("library")

target("atom_hello")
    set_kind("binary")
    set_basename("atom")
    add_files("**.cpp")
    
    if is_plat("linux", "macosx") then
        add_options("with_backtrace")
    end
    if not is_plat("windows") then
        add_options("fpic")
    end
    set_exceptions("cxx")


    set_installdir("$(projectdir)/output/$(mode)")
    set_rundir("$(projectdir)/output/$(mode)/bin/")
    
    if is_mode("debug") then
        add_defines("BUILDING_ATOM_SHARED")
    end
    add_deps("atom_math")