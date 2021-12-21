add_rules("mode.debug", "mode.release")

set_description("turn CSV into ascii table")
set_license("MIT")
set_warnings("all", "error")
set_languages("c++17")
add_defines("NDEBUG")
if is_plat("windows") then
    set_optimize("fastest")
    add_cxflags("/EHsc")
    add_ldflags("/SAFESEH:NO")
    if is_mode("debug") then
        set_runtimes("MTd")
    else
        set_runtimes("MT")
    end
elseif is_plat("mingw") then
    set_optimize("faster")
else
    set_optimize("fastest")
    add_cxxflags("-ffast-math -s -Os -fno-stack-protector -fno-math-errno")
    add_ldflags("-Wl,-z,norelro -Wl,--hash-style=gnu")
end


add_requires("csv2", {system = false, configs = {cxflags = "-O3 -ffast-math -s -Os -fno-stack-protector -fno-math-errno"}})
add_requires("cxxopts", {system = false, configs = {cxflags = "-O3 -ffast-math -s -Os -fno-stack-protector -fno-math-errno"}})
add_requires("fmt")

target("csvtable")
    set_kind("binary")
    add_files("src/*.cc")
    add_packages("csv2")
    add_packages("cxxopts")
    add_packages("fmt")