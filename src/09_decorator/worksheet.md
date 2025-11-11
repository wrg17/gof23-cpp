# Decorator

- Category: Structural

---

## 1) Intent

Allow for dynamic addition of responsibilities to an object.

## 2) Core Idea (How It Works)

- Attach additional responsibilities to an object dynamically.
- Enables combinations of extendable functionality without subclassing.
- Implement an interface to wrap other iterations of the interface to add functionality and forward to the stack of
  wrappers.

## 3) Roles

- Component: Defines an interface for objects to include in a composition.
- Concrete Component: Implements the Component interface.
- Decorator: Defines an interface for adding further responsibilities to a referenced component.
- Concrete Decorator: Implements the additional responsibilities

## 4) Typical Flow

1) Create a concrete component
2) Use conditions to situationally extend the functionality of the component.

## 5) When to Use

- When you want to add responsibilities to an object dynamically.
- When you want to extend functionality without subclassing.
- To avoid an explosion of subclasses when many combinations of functionality are needed.

## 6) Trade-offs

- Pros:
    - Flexibility of functionality.
    - Dynamic composition.
    - Modularity
- Cons:
    - Unclear what functionalities are added.
    - Many small classes.
    - Wrapping can make references unclear
    - Excessive indirection
    - More memory-intensive