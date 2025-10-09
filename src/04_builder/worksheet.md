# Builder

- Category: Creational

---

## 1) Intent

The builder pattern is used to create complex objects through a series of operations rather than through a single call
like a constructor, factory method, or abstract factory.

## 2) Core Idea (How It Works)

- To make the algorithm for making a complex object independent of its parts
- To enable variability in the assembly of the parts

## 3) Roles

- Builder: Specifies an abstract interface for creating parts of the final product
- ConcreteBuilder:
    - Defines and tracks the build product
    - Implements stages of the builder interface to create parts of the final product
    - Provides an interface for getting the final product
- Director: Uses the builder interface to construct the product
- Product:
    - A complex object that is built by the concrete builder in parts
    - Contains all the parts of the final product with interfaces that are independent of the concrete builder

## 4) Typical Flow

1) The client builds a concrete builder
2) The client calls the director to construct the product using the concrete builder
3) The director references the builder interface to create parts of the final product
4) The client gets the final product from the builder

## 5) When to Use

- Use this pattern when a complex object needs to be built in stages
- Variability of object construction is required
- To improve the modularity of how complex objects are built and represented

## 6) Trade-offs

- Pros:
    - Improves the modularity of how complex objects are built
    - Allows variability of object construction
    - Isolates the construction of a complex object from its representation
- Cons:
    - This is a less direct approach to object creation
    - Adds indirection to the code
    - Adds architectural complexity and boilerplate code