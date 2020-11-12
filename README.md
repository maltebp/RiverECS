# River ECS
A simple [ECS](https://en.wikipedia.org/wiki/Entity_component_system) library written in C++17 for my game framework _[River](https://github.com/maltebp/River)_. While built for River, it's not dependant on it, and the library can be used in any project you like.


## Features
The library currently supports the following features:

  - Creation/deletion of entities
  - Adding, removal and retrieval of components for a given entity
  - Running callback on all entities with a given set of components
  - Cache-optimized storage of components (components of same type are stores in a continious, gap-free memory block)


This pretty much sums it up:
```c++
ECS::Domain domain;

auto entity = domain.createEntity();

auto comp = entity->addComponent<ComponentA>();
comp->someValue = 10;

domain.clean();

domain.forMatchingEntities<ComponentA>(
    [](Entity* e, ComponentA* a){
        std::cout << "someValue = " << a->someValue << std::endl;
    }
);

Output:
someValue = 10;
```

### Coming features?
It's not my goal to make a feature rich library, and thus most of the improvements I strive to make, are in terms of performance. You can see a list of things to be done in the [_Issues_](https://github.com/maltebp/RiverECS/issues) tab.


## Building the  project
The project is compiled with _C++17_ into a _static library_ using _Visual Studio 2019_.  
To build the library, open the `RiverECS.sln` file and build the `RiverECS` project. The outputs (`RiverECS.lib` and the folder with required headers) can be found in the `build` folder in the repository root.


## Using the library
To get access to necessary features, include the `ECS.h` header, and all classes and functions reside in the `River::ECS` namespace.
  
_Note:_ The code examples assumes the usage of `using namespace River`.


### Domain
The Domain class is used to create, store and query related entities and components, and is simple constructed as:

```c++
ECS::Domain domain;
```

 You may construct multiple domains, as each Domain will be completely independent from any other. This allows you to have seperate "worlds" of entities and components.

__Cleaning the Domain__  
The Domain should be _cleaned_: after each update cycle in your program by using the `clean` method:

```c++
// Update entities / components ... 

domain.clean();
```

The `clean` influences entities, components and queries in terms of creation and deletion, pointer validation and memory cleanup, as stated in below sections.


### Entity

___Create:___  
To create an Entity within a Domain:

```c++
ECS::Entity* entity = domain.createEntity();
```

___Destroy:___  
To delete an Entity you can do either of the two:

```c++
entity->destroy();

domain.destroyEntity(entity);
```

___Cleaning___
 - A destroyed Entity will only be marked as such, and won't actually be destroyed until the next cleaning
 - An Entity pointer is valid until the domain has been cleaned after the destruction of the Entity


### Components

___Defining Components___  
All classes and structs can function as components, as long as they:
 - Publicly extend the `Component` class
 - Provide a default constructor (can be the implicit default constructor)

```c++
struct MyComponent : public ECS::Component {
    int myData;
}
```

This is all you need to define a new component (no need for registering it manually in the system).  
The Component class provides the struct with an `id`, which can be used to query certain things in the domain.


___Adding Component___

```c++
MyComponent* comp = entity->addComponent<MyComponent>();
comp->myData = 10;
```

___Removing Component___

```c++
entity->removeComponent<MyComponent>();
```

___Cleaning___
 - __Important:__ A pointer to a component will only remain valid until the next cleaning
 - Components won't be removed from entity before cleaning



### Query
The primary (and currently only) way of querying entities in the domain is to use `forMatchingEntities`:

```c++
    domain.forMatchingEntities<ComponentA, ComponentB>(
        [](Entity* e, ComponentA* a, ComponentB* b){
        // Run your code
        }
    );
```

___Cleaning___
 - A newly created Entity will not be considered for any collective queries before cleaning
 - A newly created component will not be considered for any collective queries before cleaning



## Internal design considerations
A brief overview of some of the major design decisions I've made, and why:


 - _Complicated source_  
I have prioritized a simple usage of the library and its performance, over ease of maintainence of the code (as seen from the heavy usage of templating). I believe a few fast and simple features are better than many somewhat slow features in the case of an ECS.

 - _Registering Component types_  
 Component types are registered when they are referenced for the first time, and it may cause heavey restructuring of the memory in a Domain. However, I believe this to be worth it, as it's a cross Domain solution, and only has to be done once every run, and thus can be contained in some loading part.

 - _Cleaning of Domain_  
 The concept of cleaning the Domain was introduced to give user full control over when pointers to components and entities would be invalidated. I don't believe it will have a negative impact in terms of the small delay from creating an entity/component to the first time it can be referenced in a query.

 - _Entity component_  
Which types components an entity has, is stored in a custom-made bitset, and all these bitsets (one for each entity) are stored in continious memory. The fact that a component flag is only 1 bit, and the flags are continious in memory, makes the querying of entities quite fast.


## Test
Since the library is very central part of my game framework _River_, I find it essential to be able to confirm that the system works as intending at all times.  
As such, the library is unit tested using the  [Catch2](https://github.com/catchorg/Catch2), and all tests are present in the Visual Studio project `Tests` in the `Test` folder. The test project also contains a Sandbox mode to be used for running non-persistent tests, and this can be executed by changing UNIT_TESTS flag in `Main.cpp`.

