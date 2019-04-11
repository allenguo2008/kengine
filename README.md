# Kengine

The Koala engine is a type-safe and self-documenting implementation of an Entity-Component-System (ECS), with a focus on runtime extensibility and compile-time type safety and clarity.

This new version features a greatly optimized memory architecture, better cache-efficiency, and a more advanced API.

![koala](koala.png)

## Installation

The project uses git submodules extensively, and must therefore be cloned recursively.

```
git clone --recursive git@github.com:phiste/kengine
```

Alternatively, if using this resository as a submodule of your own project, make sure to init, sync and update your submodules in order to make sure the "putils" folder isn't empty.

The engine requires a **C++17** compiler.

### Classes

* [Entity](Entity.md): represents an in-game entity. Is simply a container of `Components`
* [System](System.md): holds game logic. A `PhysicsSystem` might control the movement of `Entities`, for instance.
* [EntityManager](EntityManager.md): manages `Entities`, `Components` and `Systems`

Note that there is no `Component` class. Any type can be used as a `Component`, and dynamically attached/detached to `Entities`.

### Samples

These are pre-built, extensible and pluggable elements that can be used in any project, or to bootstrap a project.

##### Components

General purpose gamedev:
* [TransformComponent](common/components/TransformComponent.md): defines a `Entity`'s position and size
* [PhysicsComponent](common/components/PhysicsComponent.md): defines a `Entity`'s movement
* [InputComponent](common/components/InputComponent.md): lets `Entities` receive keyboard and mouse events

Behaviors:
* [BehaviorComponent](common/components/BehaviorComponent.md): defines a function to be called each frame for a `Entity`
* [LuaComponent](common/components/LuaComponent.md): defines the lua scripts to be run by the `LuaSystem` for a `Entity`
* [PyComponent](common/components/PyComponent.md): defines the Python scripts to be run by the `PySystem` for a `Entity`
* [CollisionComponent](common/components/CollisionComponent.md): defines a function to be called when a `Entity` collides with another

Debug tools:
* [AdjustableComponent](common/components/AdjustableComponent.md): lets users modify variables through a GUI (such as the [ImGuiAdjustableManagerSystem](common/systems/ImGuiAdjustableSystem.md))
* [ImGuiComponent](common/components/ImGuiComponent.md): lets `Entities` render debug elements using [ImGui](https://github.com/ocornut/imgui/)
* [DebugGraphicsComponent](common/components/DebugGraphicsComponent.hpp): lets a `Entity` be used to draw debug information (such as lines, rectangles or spheres)

Graphics:
* [GraphicsComponent](common/components/GraphicsComponent.md): provides graphical information about a `Entity`, such as its appearance
* [CameraComponent](common/components/CameraComponent.hpp): lets `Entities` be used as in-game cameras, to define a frustrum and position. Follows the same conventions as `TransformComponent`
* [GUIComponent](common/components/GUIComponent.md): lets `Entities` be used as GUI elements such as buttons, lists...)
* [LightComponent](common/components/LightComponent.md): lets `Entities` be used as in-game light sources (directional lights, point lights or spot lights)
* [ModelLoaderComponent](common/components/ModelLoaderComponent.md): provides a function to load an OpenGL model which will be stored in a `ModelInfoComponent` attached to the `Entity` by the `OpenGLSystem`. This `Entity` can then be referenced by other `ModelComponents` to indicate they wish to use this 3D model
* [ModelInfoComponent](common/components/ModelInfoComponent.md): holds OpenGL handles to a 3D model. Used by low-level 3D `Systems`
* [ModelComponent](common/components/ModelComponent.md): lets `Entities` specify the ID of another `Entity` holding a `ModelInfoComponent`, to be used as this `Entity`'s appearance. Used by model loading systems
* [ShaderComponent](common/components/ShaderComponent.md): lets `Entities` be used to introduce new OpenGL shaders
* [PolyVoxComponent](common/components/PolyVoxComponent.md): lets `Entities` be used to generate voxel-based models, drawn by the `PolyVoxSystem`
* [PolyVoxModelComponent](common/components/PolyVoxModelComponent.md): specifies that an `Entity`'s `ModelComponent` points to a `PolyVox` model (and should thus be drawn by the `PolyVoxSystem`'s shader)
* [TexturedModelComponent](common/components/TexturedModelComponent.md): specifies that an `Entity`'s `ModelComponent` points to a textured model (and should thus be drawn by a textured shader, such as that of the `AssImpSystem`)

##### Systems

* [BehaviorSystem](common/systems/BehaviorSystem.md): executes behaviors attached to `Entities`
* [CollisionSystem](common/systems/CollisionSystem.md): transfers collision notifications to `Entities`
* [ImGuiAdjustableSystem](common/systems/ImGuiAdjustableSystem.md): displays an ImGui window to edit `AdjustableComponents`
* [LogSystem](common/systems/LogSystem.md): logs messages
* [LuaSystem](common/systems/LuaSystem.md): executes lua scripts attached to an entity
* [PySystem](common/systems/PySystem.md): executes Python scripts attached to an entity
* [PhysicsSystem](common/systems/PhysicsSystem.md): moves entities in a framerate-independent way
* [SfSystem](common/systems/sfml/SfSystem.md): displays entities in an SFML render window
* [OpenGLSystem](common/systems/opengl/OpenGLSystem.md): displays entities in an OpenGL render window

All these systems are header-only, except for the `SfSystem` and the `OpenGLSystem`, which are plugins. To build them, set `KENGINE_SFML` and `KENGINE_OPENGL` to `true` in your `CMakeLists.txt`, respectively.

##### DataPackets

* [Log](common/packets/Log.hpp): received by the `LogSystem`, used to log a message
* [Collision](common/packets/Collision.hpp): sent by the `PhysicsSystem`, indicates a collision between two `Entities`
* [RegisterAppearance](common/packets/RegisterAppearance.hpp): received by the `SfSystem`, maps an abstract appearance to a concrete texture file.
* [GBuffer](common/packets/GBuffer.hpp): received by the `OpenGLSystem`, specifies the layout of the GBuffer (used in deferred shading)

These are datapackets sent from one `System` to another to communicate.

### Example

Here is a list of examples:

* [Bots painting a canvas](https://github.com/phisko/painter) (this has evolved into a bit more, and is now used as a toy project for 3D development)

Old API, deprecated:
* [Flappy bird clone](https://github.com/phisko/flappy_koala)
* [Tunnel game, dodging cubes](https://github.com/phisko/koala_tunnel)
* [Tower defense "game", not much at stake though](https://github.com/phisko/koala_defense)
* [Shoot'em up game](https://github.com/phisko/shmup)

Below is a commented main function that creates an entity and attaches some components to it, as well as a lua script. This should let you get an idea of what is possible using the kengine's support for reflection and runtime extensibility, as well as the compile-time clarity and type-safety that were the two motivations behind the project.

##### main.cpp

```cpp
#include <iostream>

#include "go_to_bin_dir.hpp"

#include "EntityManager.hpp"
#include "Entity.hpp"

#include "systems/LuaSystem.hpp"
#include "components/GraphicsComponent.hpp"
#include "components/TransformComponent.hpp"

// Simple system that outputs the transform and lua components of each entity that has them
class DebugSystem : public kengine::System<DebugSystem> {
public:
    DebugSystem(kengine::EntityManager & em) : System<DebugSystem>(em), _em(em) {}

    void execute() final {
        for (const auto & [e, transform, lua] : _em.getEntities<kengine::TransformComponent3f, kengine::LuaComponent>()) {
            std::cout << "Entity " << e.id << '\n';

            std::cout << "\tTransform: " << transform.boundingBox << '\n';

            std::cout << "\tScripts:" << '\n';
            for (const auto & script : lua.getScripts())
                std::cout << "\t\t[" << script << "]\n";

            std::cout << '\n';
        }
    }

private:
	kengine::EntityManager & _em;
};

int main(int, char **av) {
    // Go to the executable's directory to be next to resources and scripts
    putils::goToBinDir(av[0]);

    // Create an EntityManager
    kengine::EntityManager em; // Optionally, pass a number of threads as parameter (kengine::EntityManager em(4);)

    // Load the specified systems, and any plugin placed in the executable's directory
    //      If you specify 'KENGINE_SFML' as TRUE in your CMakeLists.txt, this will load the SfSystem
    em.loadSystems<DebugSystem, kengine::LuaSystem>(".");

    // To add a new system, simply add a DLL with a
    //      `ISystem *getSystem(kengine::EntityManager &em)`
    // To add a new system, simply add a DLL with a

    // Create an Entity and attach Components to it
    em += [](kengine::Entity e) {
        e += kengine::TransformComponent3f({ 42.f, 0.f, 42.f }); // Parameter is a Point3f for position
        e += kengine::LuaComponent({ "scripts/unit.lua" }); // Parameter is a vector of scripts
    };

	// Register types to be used in lua
    try {
        auto &lua = em.getSystem<kengine::LuaSystem>();
        lua.registerTypes<
            kengine::TransformComponent3f, putils::Point<float, 3>, putils::Rect<float, 3>,
            kengine::LuaComponent
        >();
    }
    catch (const std::out_of_range &) {} // If the LuaSystem wasn't found, ignore

    // Start game
    while (em.running)
        em.execute();

    return (EXIT_SUCCESS);
}
```

##### scripts/unit.lua

```lua
-- Simply modify component

local transform = self:getTransformComponent()
local pos = transform.boundingBox.topLeft
pos.x = pos.x + 1
```
