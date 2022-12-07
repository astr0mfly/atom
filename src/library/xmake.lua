target("atom_math")
    add_files("**.cpp")
    if is_plat("linux", "macosx") then
        add_options("with_backtrace")
    end
    if not is_plat("windows") then
        add_options("fpic")
    end

    if is_mode("debug") then
        set_kind("shared")
        set_basename("atom_dyc_$(mode)_$(arch)")
        set_configvar("ATOM_SHARED", 1)
        add_defines("BUILDING_ATOM_SHARED")
        set_symbols("debug", "hidden")
    else
        set_kind("static")
        set_basename("atom_lib_$(mode)_$(arch)")
        set_configvar("ATOM_SHARED", 0)
    end

    set_configdir("..")
    add_configfiles("config.h.in", {filename = "library/config.h"})

    add_includedirs("..", {public = true})

    add_installfiles("*.h", {prefixdir = "include/library"})
    add_installfiles("*.hpp)", {prefixdir = "include/library"})