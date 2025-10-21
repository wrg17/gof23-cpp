# Prototype

- Category: Creational

---

## 1) Intent

Create new objects by copying a "prototype"

## 2) Core Idea (How It Works)

- An object that has a method for creating a copy of itself (clone) effectively has a factory method for itself.
- The ability to clone prevents a parallel factory hierarchy.

## 3) Roles

- Product: An interface with a method for cloning.
- Concrete Product: The implementation of the interface.
- Client: Uses the product interface to get a concrete product.

## 4) Typical Flow

1) Client has an object of type Product.
2) Client calls the clone method on the Product.
3) Client uses the cloned object which is the concrete product under the hood.

## 5) When to Use

- When the classes to instantiate are determined at runtime.
- To avoid a parallel factory hierarchy.
- When you'd want to dynamically define a shape from a few pieces of state and then "hydrate" it repeatedly.

## 6) Trade-offs

- Pros:
    - Add an remove shapes for hydrating at runtime
    - Reduces subclassing
    - Configure with classes dynamically
- Cons:
    - All instances must implement clone which can be challenging depending on the complexity of the object.