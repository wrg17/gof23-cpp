# Command

- Category: Behavioral

---

## 1) Intent

To abstract the execution of an action away from the caller.

## 2) Core Idea (How It Works)

The core idea is to capture a request as an object which enables parameterized execution of the action, into a separate
object. This enables the caller to request an action be performed without knowing how to do it and add behavior to its
execution, e.g., logging, undo, etc.

## 3) Roles

- Command: The interface for executing an action.
- Concrete Command: The implementation of the command interface.
- Client: Creates the concrete command.
- Invoker: Sets the command to be executed.
- Receiver: Performs the action when the command is executed.

## 4) Typical Flow

1) A client constructs a command object.
2) A client eventually passes the command object to an invoker (which can be the client).
3) The invoker requests the command to execute.
4) The command calls the receiver to perform the action.

## 5) When to Use

- Objects can be parameterized to execute different actions.
- Callback operations.
- To abstract away the order of execution by queuing, outliving the original request, etc.
- To support undo/redo.
- To support logging.
- To support transactions.

## 6) Trade-offs

- Pros:
    - Enables separation between the invoking object from the knowledge of how to execute the action.
    - Commands can be extended to support additional functionality.
    - Commands can be assembled into a composite command for complex operations.
    - Modular design.
- Cons:
    - Hysteresis may be introduced in the process of doing, undoing, and redoing. That is the undo may not be perfect
      and drift from a true function may occur.
    - Adding complexity when immediate execution would suffice.
    - Increases the number of classes.
    - Memory overhead for storing the command history.