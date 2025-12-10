# Flyweight

- Category: Structural

---

## 1) Intent

Fine-grained objects can be maintained efficiently via sharing.

## 2) Core Idea (How It Works)

- Dividing state information into intrinsic and extrinsic categories allows sharing.
- Intrinsic state is immutable so it can be shared.
- Extrinsic state is mutable and unique to each object and must be managed by the client.
- A flyweight factory creates flyweights and caches existing ones.

## 3) Roles

- Flyweight:
    - Defines an interface for interacting with extrinsic state.
- Concrete Flyweight:
    - Implements Flyweight interface and stores intrinsic state. It must be sharable.
- Unshared Concrete Flyweight:
    - Implements Flyweight interface and stores intrinsic state. It must not be sharable.
- Flyweight Factory:
    - Creates and manages flyweights.
    - Maintains a cache of flyweights.
- Client:
    - Maintains extrinsic state
    - Maintains a reference to flyweights.

## 4) Typical Flow

1) The client requests a flyweight from the factory.
2) The factory checks the cache for an existing flyweight.
3) If a flyweight is found, it is returned. Otherwise, a new flyweight is created and added to the cache.
4) The client uses the flyweight.

## 5) When to Use

- The application uses a large number of similar objects.
- Memory usage can be reduced by sharing intrinsic state.
- Many groups of objects can be maintained efficiently via sharing.
- Object identity is not needed, which enables sharing objects in a cache.

## 6) Trade-offs

- Pros:
    - Reduce memory usage by sharing intrinsic state across objects.
    - Reduce the total number of instances through pooling.
- Cons:
    - Adds complexity through the extrinsic state management.
    - Object communication is complicated when a mutatable state, e.g., a parent pointer, must be externally managed.