# The Puka Game Engine

## OS Support

- Linux (Tier 0)
- Windows (Tier 1)
- OSX (Tier 2)

## Roadmap

- [x] Use `sol` instead of `luabridge`
- [x] Use `yyjson` instead of `rapidjson`
- [ ] Multithreading built-in components
- [ ] Multithreading lua components (along with rendering)
- [ ] Lua APIs to multithreading
- [ ] Global userdata in lua

## Multicore Lua

We run multiple lua VMs in N threads.

Each component will be mapped to a lua VM.

One VM can only update one component at one time (every VM has a lock and only one thread can access its state).

When user call `GetComponent` etc., the engine will return a "TableProxy" with fields set matching the real component.

After that, on reading/writing to a field of that component, lua will call the getter and setter functions in the engine, which will access those data in the real component (aftering acquiring the lock of the VM of that component).

## Rendering

Rendering is done in the main thread while the component updating is done in other threads.

## License

This project is distrubuted under the MIT license. You are free to use this project (not including all the libraries used) for any purpose at no cost.

### Libraries used

- [glm](https://github.com/g-truc/glm): MIT license
- [sol](https://github.com/ThePhD/sol2): MIT license
- [box2d](https://github.com/erincatto/box2d): MIT license
- [SDL 2](https://www.libsdl.org): z-lib license
- [Lua](https://www.lua.org/): MIT license
- [yyjson](https://github.com/ibireme/yyjson): MIT license
