
-- Utility functions
function appendTable(tableA, tableB)
    for _,v in ipairs(tableB) do 
        table.insert(tableA, v) 
    end
end

-- Include the subprojects
include "modules/asio"
include "modules/spdlog"

-- Main library project
project "NetLib"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    location "build"
    targetdir "bin/%{cfg.buildcfg}"
    targetname "%{prj.name}"
    
    filter { "platforms:x86" }
        architecture "x86"

    filter { "platforms:x64" }
        architecture "x86_64"

    filter "configurations:Debug"
        defines { "DEBUG", "_DEBUG", "NDEPLOY" }
        runtime "Debug"
        symbols "On"
        optimize "Off"

    filter "configurations:Release"
        defines { "NDEBUG", "NDEPLOY" }
        runtime "Release"
        symbols "On"
        optimize "On"

    filter "configurations:Deploy"
        defines { "NDEBUG", "DEPLOY" }
        runtime "Release"
        symbols "Off"
        optimize "On"

    filter {}


    -- Include directories
    local _includedirs = { 
        _SCRIPT_DIR .. "/include"
    }
    includedirs (_includedirs)

    
    -- Main source files
    files ({ "include/**", "src/**" })

    -- Asio dependency
    dependson "asio"
    includedirs (ASIO_INCLUDE_DIRS)

    -- spdlog dependency
    dependson "spdlog"
    includedirs (SPDLOG_INCLUDE_DIRS)
    defines (SPDLOG_DEFINES)




    -- Include and linker information for premake projects using this library
    NETLIB_INCLUDE_DIRS = {}
    appendTable(NETLIB_INCLUDE_DIRS, _includedirs)

    NETLIB_LINK_DIRS = {}
    appendTable(NETLIB_LINK_DIRS, _SCRIPT_DIR .. "/bin/%{cfg.buildcfg}/")
    appendTable(NETLIB_LINK_DIRS, SPDLOG_LINK_DIRS)

    NETLIB_LINKS = { "NetLib" }
    appendTable(NETLIB_LINKS, ASIO_LINKS)
    appendTable(NETLIB_LINKS, SPDLOG_LINKS)
    if not os.istarget("windows") then
        appendTable(NETLIB_LINKS, { "pthread" })
    end

    NETLIB_DEFINES = {}

    -- Forward the include directories if wanted
    -- A library can set NETLIB_WANT_SPDLOG to true to use the NetLib's spdlog module
    if (NETLIB_WANT_SPDLOG) then
        appendTable(NETLIB_INCLUDE_DIRS, SPDLOG_INCLUDE_DIRS)
        appendTable(NETLIB_DEFINES, SPDLOG_DEFINES)
    end
