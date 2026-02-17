# Iterator

- Category: Behavioral

---

## 1) Intent

Provide a way to access and traverse elements of an aggregation without exposing its underlying representation.

## 2) Core Idea (How It Works)

Have aggregate objects return an object (the iterator) that can be used to traverse the aggregate's contents. By using
polymorphism, the iterator can be used to traverse varied data structures independent of client logic.

## 3) Roles

- Iterator: Defines the interface for accessing and traversing an aggregate object.
- Concrete Iterator: Implements the iterator interface and maintains a reference to a concrete aggregate object and
  traversal state.
- Aggregate: Defines an interface for creating an iterator object. Houses a collection.
- Concrete Aggregate: Implements the aggregate interface with the function to return the concrete iterator.

## 4) Typical Flow

1) A client requests an iterator from an aggregate object.
2) The aggregate object returns an iterator object.
3) The client uses the iterator to traverse the aggregate's contents.

## 5) When to Use

- Use when traversal of an aggregate object is required, but the structure of the elements doesn't need to be known.
- To simplify the traversal logic in clients.
- To support multiple traversals of aggregate objects concurrently.

## 6) Trade-offs

- Pros:
    - Provides a simple traversal interface.
    - Allows clients to traverse different aggregate structures without changing their code.
    - Allows concurrent traversal of aggregate objects.
- Cons:
    - More overhead than directly traversing continuous arrays.
    - Iterators might cease being valid if the aggregate object is modified during traversal.
    - Memory for maintaining traversal state.