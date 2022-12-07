-- plat
set_config("plat", os.host())

-- project
set_project("atom")

-- set xmake minimum version
set_xmakever("2.3.1")

-- set common flags
set_languages("c++20")
set_warnings("all")     -- -Wall
--set_symbols("debug")    -- dbg symbols
add_rules("mode.debug", "mode.release")

if is_plat("windows") then
    if is_mode("debug") then
        set_optimize("none")  -- faster: -O2  fastest: -Ox  none: -O0
        set_runtimes("MDd")
        add_cxflags("/EHa")
        add_ldflags("/SAFESEH:YES")
    else
        set_optimize("fastest")  -- faster: -O2  fastest: -Ox  none: -O0
        set_runtimes("MT")
        add_cxflags("/EHsc")
        add_ldflags("/SAFESEH:NO")
    end
elseif is_plat("mingw") then
    add_ldflags("-static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lwinpthread -Wl,-Bdynamic", {force = true})
    set_optimize("faster")
else
    set_optimize("faster")   -- faster: -O2  fastest: -O3  none: -O0
    --add_cxflags("-Wno-narrowing", "-Wno-sign-compare", "-Wno-strict-aliasing")
    if is_plat("macosx", "iphoneos") then
        add_cxflags("-fno-pie")
    end
end

if is_mode("debug") then
    add_defines("DEBUG")
end

set_warnings("all", "error")

option("with_backtrace")
    set_default(false)
    set_showmenu(true)
    set_description("build with libbacktrace, for stack trace on linux/mac")
option_end()

-- build with -fPIC
option("fpic")
    set_default(false)
    set_showmenu(true)
    set_description("build with -fPIC")
    add_cxflags("-fPIC")
option_end()

if has_config("with_backtrace") then
    add_requires("libbacktrace")
end

-- include dir
add_includedirs("include")

-- install header files
add_installfiles("(include/**)", {prefixdir = ""})

-- include sub-projects
includes("src")