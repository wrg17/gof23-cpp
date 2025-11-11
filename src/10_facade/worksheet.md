# Facade

- Category: Structural

---

## 1) Intent

To provide a single interface for a subsystem.

## 2) Core Idea (How It Works)

- Abstract away the orchestration of subclasses to provide a single interface.
- Provides an api for the subsystem.
- Decouple clients from the internals of a subsystem.

## 3) Roles

- Facade:
    - Delegates requests to the correct subsystem component.
- Subsystem Classes:
    - Implements a subsystem functionality.
    - Has no knowledge of the facade.

## 4) Typical Flow

1) A client makes a request to the facade.
2) The facade delegates the request to the correct subsystem component(s).
3) The subsystem component(s) perform the requested action. Which may include multiple subsystems.
4) The facade returns the result to the client.

## 5) When to Use

- To create a simple interface to a complex subsystem.
- To hide the complexity of a subsystem.
- There are many dependencies between clients and implementation classes.
- To create subsystem layering.

## 6) Trade-offs

- Pros:
    - Reduce the number of objects that clients deal with.
    - Simplify the client code.
    - Encourages weak coupling between clients and implementation classes which often improves dependency structures
    - Reduces compilation dependencies.
- Cons:
    - Adds indirection.
    - Code bloat