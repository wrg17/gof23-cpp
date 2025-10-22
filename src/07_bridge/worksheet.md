# Bridge

- Category: Structural

---

## 1) Intent

To make an abstraction and implementation independent.

## 2) Core Idea (How It Works)

A bridge is the pattern where an interface holds a reference to its implementation. This enables the interface to be
subclassed without the implementation being affected and vice versa. Note that this is similar to the adapter pattern,
but the bridge is usually a prior design decision and the adapter is a post-design decision.

## 3) Roles

- Abstraction: An abstract interface with a reference to an implementor
- RefinedAbstraction: Extends the abstract interface
- Implementor: An interface of the implementations used by the abstraction. The operation can be primitives while the
  abstraction defines higher order operations.
- ConcreteImplementor: Implements the interface of the implementor.

## 4) Typical Flow

1) A client requests an operation from the abstraction.
2) The abstraction checks that it has a reference to an implementor.
3) The abstraction delegates the operation to the implementor.
4) The implementor performs the operation.
5) The implementor returns the result to the abstraction.
6) The abstraction returns the result to the client.

## 5) When to Use

- When the interface and implementation should support parallel evolution.
- To make the client independent of the interfaces underlying implementation.
- To allow implementation to be extended without affecting the client.

## 6) Trade-offs

- Pros:
    - Adds independence between the interface and implementation.
    - Allows the implementation to be extended without affecting the client.
    - Allows the interface to be subclassed without affecting the implementation.
    - The implementation can be changed at runtime.
    - Changes to implementations don't require recompiling the abstraction and its clients.
    - Division of responsibilities.
- Cons:
    - Potentially increases complexity.