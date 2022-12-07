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

-- include sub-projects
includes("library")