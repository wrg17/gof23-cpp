# Abstract Factory

- Category: Creational

---

## 1) Intent

The abstract factory pattern provides an interface for creating related objects without determining their
concrete classes in the caller.

## 2) Core Idea (How It Works)

The main idea is to enable algorithms to use a base class or family of classes with a shared interface, while the logic
for determining the concrete class is abstracted away.

## 3) Roles

- Abstract Factory: The interface for creating abstract products.
- Concrete Factory: Implements the creation of concrete products.
- Abstract Product: The interface for products.
- Concrete Product: Defines a product to be created by the Concrete Factory and implements the Abstract Product.
- Client: Uses the interfaces given by Abstract Factory and Abstract Product.

## 4) Typical Flow

1) Client asks the abstract factory for an abstract product to use in its algorithm.
2) The abstract factory invokes concrete factories.
3) A concrete factory makes a concrete product.
4) The client uses the concrete products through the interface of the abstract product.

## 5) When to Use

- A system needs independence of product creation, composition, and representation.
- A system needs one of multiple families of products.
- A system needs to enforce a family of related products are used together.
- A system needs to only reveal interfaces and not implementations.

## 6) Trade-offs

- Pros:
    - The client can be independent of the details of the implementation.
    - The client can use the same algorithm for different products.
    - Maintains a clean separation between the client and the concrete classes.
    - Enforces consistency between a family of products.
- Cons:
    - Extending the abstract factory for new products is challenging because each concrete factory must be modified.