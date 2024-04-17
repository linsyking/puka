# The Puka Game Engine

Multi-platform 2D game engine with simplicity and concurrency in mind.

## OS Support

- Linux (Tier 0)
- Windows (Tier 1)
- OSX (Tier 2)

## Features

- Lua scripting language
- JSON game configuration file
- Multicore components

## Roadmap

- [x] Use `sol` instead of `luabridge`
- [x] Use `yyjson` instead of `rapidjson`
- [x] Multithreading built-in components
- [x] Multithreading lua components (along with rendering)
- [ ] Lua APIs to multithreading
- [ ] Lua APIs to store private/protected data (Other component cannot access/cannot write)
- [ ] Global userdata in lua

## License

This project is distrubuted under the MIT license. You are free to use this project (not including all the libraries used) for any purpose at no cost.

### Libraries used

- [glm](https://github.com/g-truc/glm): MIT license
- [sol](https://github.com/ThePhD/sol2): MIT license
- [box2d](https://github.com/erincatto/box2d): MIT license
- [SDL 2](https://www.libsdl.org): z-lib license
- [Lua](https://www.lua.org/): MIT license
- [yyjson](https://github.com/ibireme/yyjson): MIT license
