# Proxy

- Category: Structural

---

## 1) Intent

To provide a placeholder for another object.

## 2) Core Idea (How It Works)

A proxy acts as a surrogate or placeholder that controls access to another object.

Different proxy types serve different purposes:

- **Virtual Proxy**: Enables lazy initialization.
- **Protection Proxy**: Controls access rights.
- **Remote Proxy**: Represents an object in a different address space.
- **Smart Reference**: Adds supplementary behavior.

## 3) Roles

- Subject: The common interface for both RealSubject and Proxy.
- Proxy: Maintains a reference to a RealSubject object.
- RealSubject: The underlying object.

## 4) Typical Flow

1) A client uses the subject interface to request a service from the proxy that it cannot distinguish from the real
   subject.
2) The proxy may perform additional operations before/after forwarding, e.g., logging, lazy initialization, etc.
3) The proxy checks if the RealSubject exists; if not (virtual proxy), it creates it.
4) The proxy forwards the request to the real subject.
5) The real subject performs the requested service and returns the result to the proxy.
6) The proxy returns the result to the client.

## 5) When to Use

- When you need lazy initialization.
- When you need access controls.
- When working with remote objects in a separate address space.
- When you want to add supplementary behavior to an object.

## 6) Trade-offs

- Pros:
    - Standardizes meta-behaviors.
    - Enables lazy initialization.
    - Secure access to the real object.
- Cons:
    - Adds indirection and potential performance overhead.
    - Adds complexity through additional classes.
    - The first request may have high latency (for virtual proxies with lazy initialization).