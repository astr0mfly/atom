target("atom_lib_math")
    set_kind("static")
    set_basename("atom_lib")
    add_files("**.c", "**.cc", "**.cpp", "**.cxx")
    if is_plat("linux", "macosx") then
        add_options("with_backtrace")
    end
    if not is_plat("windows") then
        add_options("fpic")
    end
    set_configvar("ATOM_SHARED", 0)
    add_configfiles("../../include/library/config.h.in", {filename = "../../include/library/config.h"})
    add_includedirs("..", {public = true})

target("atom_dyc_math")
    set_kind("shared")
    set_basename("atom_dyc")

    add_files("**.c", "**.cc", "**.cpp", "**.cxx")
    if is_plat("linux", "macosx") then
        add_options("with_backtrace")
    end
    if not is_plat("windows") then
        add_options("fpic")
    end

    set_symbols("debug", "hidden")
    add_defines("BUILDING_ATOM_SHARED")
    set_configvar("ATOM_SHARED", 1)

    add_configfiles("../../include/library/config.h.in", {filename = "../../include/library/config.h"})
    add_includedirs("..", {public = true})