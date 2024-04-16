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
      "src/",
      "ext/include",
      "ext/include/sol",
      "ext/include/lua",
      "ext/include/task_runner",
      "ext/include/yyjson"
   }

   files { "src/**.cpp", "ext/**.c", "ext/**.cpp" }

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
      postbuildcommands ("copy ext\\lib\\libsdl2\\dll\\* x64\\%{cfg.buildcfg}")
      defines { "SDL_MAIN_HANDLED" }

   filter "system:MacOSX"
      linkoptions {"-F ext/lib/libsdl2/Frameworks"}
      runpathdirs { "ext/lib/libsdl2/Frameworks"}
      links {
         "SDL2.framework",
         "SDL2_image.framework",
         "SDL2_ttf.framework",
         "SDL2_mixer.framework"
      }

   filter "system:Linux"
      links { "SDL2", "SDL2_ttf", "SDL2_image", "SDL2_mixer" }
      targetname "puka"
