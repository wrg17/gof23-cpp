# Interpreter Pattern

- Category: Behavioral

---

## 1) Intent

To define a grammar for interpreting a language. Convert an expression into behaviors.

## 2) Core Idea (How It Works)

The interpreter pattern defines a grammar for interpreting a language. The grammar consists of nodes that perform a
function on the relevant phrase. The nodes are organized in a tree structure with base cases at the leaves.

## 3) Roles

- Abstract Expression: An abstract interpret operation common to all nodes in the syntax tree.
- Terminal Expression: A concrete interpret operation for a leaf node in the syntax tree.
- Non-Terminal Expression: A concrete interpret operation for a parent node in the syntax tree.
- Context: Information globally available to the interpreter. That can be read and written while interpreting.
- Client: Invokes the interpretation process. Builds the syntax tree and passes it to the interpreter.

## 4) Typical Flow

1) The client builds or is given the syntax tree.
2) The client sets up necessary context.
3) The client invokes the interpreter.
4) The non-terminal expressions are invoked recursively.
5) The terminal expressions are invoked as a base case.
6) The interpreter operates on each node using and updating the context as necessary.
7) The result is returned to the client.

## 5) When to Use

- To define a broad mapping for diverse inputs.
- Use when the grammar of the language is simple
- Performance is not critical
- To build domain specific languages.

## 6) Trade-offs

- Pros:
    - Creates a general framework for a problem domain.
    - Easy to extend.
    - Flexible.
    - Each grammar rule can be its own class.
- Cons:
    - Complex grammars are hard to maintain.
    - Performance may be lost for generalization.
    - Can lead to many small classes.