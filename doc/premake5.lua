workspace "game_engine"
   architecture "x64"
   configurations { "Release" }

project "game_engine"
   kind "ConsoleApp"
   language "C++"
   targetdir ""
   cppdialect "C++17"

   includedirs {
      "ext/",
      "ext/lua",
      "ext/sol",
      "ext/box2d/include",
      "ext/box2d/src",
      "src/",
      "ext/yyjson/src",
      "ext/task_runner/include"
   }

   files { "src/**.hpp", "src/**.cpp", "ext/lua/*.c", "ext/box2d/src/**.cpp", "ext/yyjson/src/*.c", "ext/task_runner/src/*.cpp" }

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

   filter "system:Windows"
      targetdir ("x64/%{cfg.buildcfg}")
      objdir ("x64/obj/%{cfg.buildcfg}")
      targetname "game_engine_win"
      links { "SDL2", "SDL2_ttf", "SDL2_image", "SDL2_mixer" }
      libdirs {
         "ext/libsdl2/lib"
      }
      postbuildcommands ("copy ext\\libsdl2\\dll\\* x64\\%{cfg.buildcfg}")
      defines { "GE_WINDOWS", "SDL_MAIN_HANDLED" }

   filter "system:MacOSX"      
      defines { "GE_MACOSX" }
      linkoptions {"-F ext/libsdl2/Frameworks"}
      runpathdirs { "ext/libsdl2/Frameworks"}
      links {
         "SDL2.framework",
         "SDL2_image.framework",
         "SDL2_ttf.framework",
         "SDL2_mixer.framework"
      }
      targetname "game_engine_osx"

   filter "system:Linux"
      defines { "GE_LINUX" }
      links { "SDL2", "SDL2_ttf", "SDL2_image", "SDL2_mixer" }
      toolset "clang"
      targetname "game_engine_linux"
