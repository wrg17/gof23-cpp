# Adapter

- Category: Structural

---

## 1) Intent

To translate from a target interface expected by clients to an underlying adaptee interface.

## 2) Core Idea (How It Works)

The adapter translates from a target interface to an adaptee interface. This is subdivided into two patterns:

1) object adapter: Uses composition to link the client and adaptee by implementing the target and holding a reference to
   the adaptee. This enables a base type adaptee class to be used.
2) class adapter: Use inheritance to link the client and adaptee by inheriting from the target and referencing the
   adaptee's methods by multiple inheritance.

## 3) Roles

- Client: The client that makes use of the target.
- Target: The client's interface for the adaptee.
- Adapter: The translator between the target and adaptee.
- Adaptee: The underlying resource.

## 4) Typical Flow

1) The client calls the target
2) The target is the interface of the adapter.
3) The adapter calls the adaptee.

## 5) When to Use

1) When an existing class interface does not meet the client's requirements.
2) To make a class reusable in unforeseen circumstances.
3) An object adapter can reference a base type adaptee class providing clients with a single consistent target
   interface.

## 6) Trade-offs

General Pattern Trade-offs:

- Pros:
    - Allows a swap of adaptee classes.
- Cons:
    - Adds complexity for translation between the client and adaptee. When just calling the adaptee may suffice

Class Adapter:

- Pros:
    - Allows the overriding of adaptee methods.
    - No extra indirection.
- Cons:
    - Inheritance takes place on a concrete adaptee class which prevents a single adapter for all adaptee classes.

Object Adapter:

- Pros:
    - Enables a single adapter for all adaptee classes.
    - Add functionality to adaptees at once
- Cons:
    - Challenging to override adaptee.