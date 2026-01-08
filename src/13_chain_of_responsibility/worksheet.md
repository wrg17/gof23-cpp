# Chain of Responsibility

- Category: Behavioral

---

## 1) Intent

Connect multiple handlers through references, passing a request to be potentially handled at each until the end of a
chain or terminal handler.

## 2) Core Idea (How It Works)

When a client does not know a request handler a priori, the client can pass the request to the chain of handlers. The
first handler of this chain can then decide whether to handle the request or pass it to the next handler in the chain,
and so on.

## 3) Roles

- Handler: The interface for request handlers, that may optionally implement a successor reference.
- Concrete Handler: Implements a request handler and knows how and when to pass to a successor.
- Client: Passes a request to a concrete handler.

## 4) Typical Flow

1) The client passes a request to the chain of handlers.
2) The first handler in the chain checks whether it can handle the request.
3) If the handler can handle the request:
    - The handler handles the request
    - This may terminate the chain or pass the request to the next handler.

   Otherwise:
    - The request is passed to the next handler or falls off the chain unhandled.

## 5) When to Use

Use this pattern when:

- The request handler is not known a priori. For example, an event occurs and the handler is determined dynamically.
- Multiple handlers can handle a request.

## 6) Trade-offs

- Pros:
    - Coupling between client and handlers can be reduced or abstracted away.
    - New handlers can be added to the chain without affecting existing code.
    - The handler chain can be dynamically modified at runtime.
- Cons:
    - Receipt of a request may be dropped if no handler is available.
    - The chain of handlers may become too long and complex.
    - Context to determine which handler is appropriate may become convoluted.