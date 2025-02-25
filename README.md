# The Puka Game Engine

A game engine with **simplicity** and **concurrency** in mind.

Users write lua codes, the engine parallelize them!

Watch [this video](https://www.youtube.com/watch?v=sGGZ147X8B0) to see a demonstration of the game engine.

## OS Support

- Linux (Tier 0)
- Windows (Tier 1)
- OSX (Tier 2)

## Features

- Lua scripting language
- JSON game configuration file
- Multicore components

## Core idea

Each component is assigned to a thread. Each thread has a lua virtual machine (VM).

When running lua (user) code, the VM lock is always acquired.

To access components in other VMs, we need a "third tmp VM" to pass the value.

We use the `ComponentProxy` to proxy the access to lua components.

For example, we want to get a component from VM2 to VM1 (assume this operation is done in VM1), the process is like this:

1. Release VM1 lock
2. Acquire VM2 lock
3. Acquire VM_tmp lock
4. Deep copy component from VM2 to VM_tmp
5. Release VM_tmp lock
6. Acquire VM1 lock
7. Acquire VM_tmp lock
8. Deep copy component from VM_tmp to VM1
9. Release VM_tmp lock

**Q**: Why not directly deep copy the component from VM2 to VM1?

**A**: Because that operation requires acquiring both VM locks, which may lead to deadlock.

The core mechanism "deep copy" is done by:

```lua
lua_ref_raw copy(lua_ref_raw &obj, sol::state &target) {
    sol::type tp = obj.get_type();
    if (tp == sol::type::number) {
        if (obj.is<int>()) {
            return sol::make_object(target, obj.as<int>());
        } else {
            return sol::make_object(target, obj.as<double>());
        }
    } else if (tp == sol::type::boolean) {
        return sol::make_object(target, obj.as<bool>());
    } else if (tp == sol::type::string) {
        return sol::make_object(target, obj.as<std::string>());
    } else if (tp == sol::type::userdata) {
        if (obj.is<ComponentProxy>()) {
            return sol::make_object(target, obj.as<ComponentProxy>());
        } else if (obj.is<Actor *>()) {
            return sol::make_object(target, obj.as<Actor *>());
        } else if (obj.is<RigidbodyComponent *>()) {
            return sol::make_object(target, obj.as<RigidbodyComponent *>());
        } else {
            std::cerr << "warning: userdata type not registered\n";
        }
    } else if (tp == sol::type::function) {
        return sol::make_object(target, obj.as<sol::function>());
    } else if (tp == sol::type::table) {
        sol::table t     = obj.as<sol::table>();
        sol::table tcopy = target.create_table();
        for (auto it = t.begin(); it != t.end(); ++it) {
            auto [key, val] = *it;
            tcopy.set(copy(key, target), copy(val, target));
        }
        return tcopy;
    }
    return {};
}
```

## Demonstration source code

### Fib

```lua
return {
  fib = 0,
  fibb = function(self, n)
    if n <= 1 then
      return n
    end
    return self:fibb(n - 1) + self:fibb(n - 2)
  end,

  OnStart = function(self)
    Debug.Log(self.key)
    Debug.Log(self:fibb(self.fib))
    Application.Quit()
  end
}
```

### Write-back

```lua
return {
  itbl = {
    a = 1,
    b = 2,
    c = 3
  },
  func = function(self)
    for k, v in pairs(self.itbl) do
      Debug.Log(k .. " " .. v)
    end
  end,
  OnStart = function(self)
    if self.key == "c1" then
      self.other = self.actor:GetComponentByKey("c2")
      local data = self.other:get()
      data.itbl = {999,999}
      self.other:wb()
    end
  end,

  OnUpdate = function(self)
    local frame = Application.GetFrame()
    self:func()
    if frame == 10 then
      Application.Quit()
    end
  end
}
```

## Roadmap

- [x] Use `sol` instead of `luabridge`
- [x] Use `yyjson` instead of `rapidjson`
- [x] Multithreading built-in components
- [x] Multithreading lua components (along with rendering)
- [ ] Lua APIs to multithreading
- [ ] Lua APIs to store private/protected data (Other component cannot access/cannot write)
- [ ] Global userdata in lua
- [ ] Error handling
- [ ] Defer rendering commands
- [ ] Rendering thread
- [ ] Optimize memory, garbage collection

## License

This project is distrubuted under the MIT license. You are free to use this project (not including all the libraries used) for any purpose at no cost.

### Libraries used

- [glm](https://github.com/g-truc/glm): MIT license
- [sol](https://github.com/ThePhD/sol2): MIT license
- [box2d](https://github.com/erincatto/box2d): MIT license
- [SDL 2](https://www.libsdl.org): z-lib license
- [Lua](https://www.lua.org/): MIT license
- [yyjson](https://github.com/ibireme/yyjson): MIT license
