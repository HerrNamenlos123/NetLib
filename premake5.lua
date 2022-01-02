
-- Utility functions
function appendTable(tableA, tableB)
    for _,v in ipairs(tableB) do 
        table.insert(tableA, v) 
    end
end

-- Main library project
project "NetLib"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"
    location "build"
    targetname "NetLib"
    targetdir "bin/%{cfg.buildcfg}"
    --system "Windows"
    --architecture "x86_64"

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
    files ({ "include/**" })




    -- Include and linker information for premake projects using this library
    NETLIB_INCLUDE_DIRS = {}
    appendTable(NETLIB_INCLUDE_DIRS, _includedirs)

    NETLIB_LINK_DIRS = {}
    appendTable(NETLIB_LINK_DIRS, _SCRIPT_DIR .. "/bin/%{cfg.buildcfg}/")
    --appendTable(NETLIB_LINK_DIRS, "C:/Program Files/OpenSSL-Win64/lib/VC/static/")

    NETLIB_LINKS = {}
    appendTable(NETLIB_LINKS, "asio")