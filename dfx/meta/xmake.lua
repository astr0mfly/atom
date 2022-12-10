option("with_lib")
    if is_mode("debug") then
        add_defines("BUILDING_ATOM_SHARED")
    end
    add_deps("atom_math")

target("meta")
    set_kind("binary")
    set_default(false)
    --set_options("with_lib")
    add_files("*.cpp")
    if is_plat("windows") then
        add_cxflags("/Zc:preprocessor")
    end
    add_includedirs("$(projectdir)/dfx/internal", {public = true})