# Composite

- Category: Structural

---

## 1) Intent

Recursively compose objects into tree structures to represent part-whole hierarchies.

## 2) Core Idea (How It Works)

- Define nodes that can be composed into a tree structure.
- Enables operations to delegate responsibility to the tree structure.

## 3) Roles

- Component: An interface for a node
- Leaf: A leaf node, that is a node reference no others, in the tree structure.
- Composite: A node that references other nodes in the tree structure.
- Client: Uses the component interface to manipulate objects in the tree structure.

## 4) Typical Flow

1) A client creates a composite.
2) A client creates a leaf.
3) The leaf is added to the composite.
4) This composite may be added to another and so on.
5) The client does operations on the root or relative roots with operations propagated to children until the leaves, at
   which point parents aggregate the subtree results.

## 5) When to Use

- There is a recursive relationship between objects.

## 6) Trade-offs

- Pros:
    - Makes adding new kinds of components straightforward.
    - Simplifies clients by creating uniformity of composite interfaces.
    - Recursive functionality.
- Cons:
    - Components may become overly general with unclear responsibilities.