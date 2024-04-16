workspace "puka"
   architecture "x64"
   configurations { "Release" }

project "puka"
   kind "ConsoleApp"
   language "C++"
   targetdir ""
   cppdialect "C++17"
   targetname "puka"

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
      links { "SDL2", "SDL2_ttf", "SDL2_image", "SDL2_mixer" }
      libdirs {
         "ext/libsdl2/lib"
      }
      postbuildcommands ("copy ext\\libsdl2\\dll\\* x64\\%{cfg.buildcfg}")
      defines { "SDL_MAIN_HANDLED" }

   filter "system:MacOSX"
      linkoptions {"-F ext/libsdl2/Frameworks"}
      runpathdirs { "ext/libsdl2/Frameworks"}
      links {
         "SDL2.framework",
         "SDL2_image.framework",
         "SDL2_ttf.framework",
         "SDL2_mixer.framework"
      }

   filter "system:Linux"
      links { "SDL2", "SDL2_ttf", "SDL2_image", "SDL2_mixer" }
      targetname "puka"
