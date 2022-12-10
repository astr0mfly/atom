option("with_lib")
    if is_mode("debug") then
        add_defines("BUILDING_ATOM_SHARED")
    end
    add_deps("atom_math")

target("unitest")
    set_kind("binary")
    set_default(false)
    set_options("with_lib")
    add_files("*.cpp")
    
    add_includedirs("$(projectdir)/dfx", {public = true})
