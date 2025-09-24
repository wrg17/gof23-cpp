# Factory Method

- Category: Creational

---

## 1) Intent

The factory method pattern is used to abstract the creation of an object. Define an interface for creating an object,
but let subclasses decide which class to instantiate. Factory Method lets a class defer instantiation to subclasses.

## 2) Core Idea (How It Works)

- Define an interface for creating a secondary object.
- Define a concrete class that implements the interface.
- The factory class creates a concrete product class.

## 3) Roles

- Product: The interface for objects made by the factory
- Concrete Product: The implementation of the Product
- Creator: Defines the factory method that creates the product
- Concrete Creator: Overrides the factory method to create a concrete product

## 4) Typical Flow

1) Client uses the creator object in the context of a larger operation.
2) The client then delegates the creation of the product to the creator in the factory method.
3) The factory method creates a concrete product.
4) The client uses the product.

## 5) When to Use

- When the client doesn't know what class to instantiate.
- When the creation of an object should be delegated to a subclass.
- When creation logic can be vary between subclasses.

## 6) Trade-offs

- Pros:
    - Provides a specialized abstraction layer for object creation
    - Allows for polymorphism
    - Adds testability
- Cons:
    - Introduces unnecessary classes