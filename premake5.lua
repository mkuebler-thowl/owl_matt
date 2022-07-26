-- premake5.lua
workspace "OWL-Matt"
   configurations { "Debug", "Release" }
   platforms { "Win64"}

project "chess_engine"
   location "chess_engine"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++17"
   targetdir "bin/%{cfg.buildcfg}"
   objdir "bin-int/%{cfg.buildcfg}"

   files { 
      "%{prj.name}/src/**.hpp",
      "%{prj.name}/src/**.cpp",
   }

   includedirs {
      "external/spdlog",
   }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"
   
   filter { "platforms:Win64 "}
      system "Windows"
      architecture "x86_64"