# Singleton

- Category: Creational

---

## 1) Intent

The singleton pattern is used for when there should only ever be exactly one instance of a class.

## 2) Core Idea (How It Works)

- The constructor is private, and there is a public static method to get the instance.
- Prevents copying or cloning.
- Controls the lifetime of the instance.
- Centralizes access to shared state.

## 3) Roles

- Singleton — The owner of the single instance.
- Client — The user of the singleton.

## 4) Typical Flow

1) Client calls the Singleton::instance() or equivalent.
2) Singleton creates the instance on the first call and returns the reference.
3) Client uses the instance.

## 5) When to Use

- A single centralized resource is needed.
    - Example: logger, cache, etc.

## 6) Trade-offs

- Pros:
    - Single access point
    - Controlled lifetime
    - Centralized access to shared state
- Cons:
    - Global state
    - Potential hard to mock
    - Can be a bottleneck
    - Potential for memory leaks