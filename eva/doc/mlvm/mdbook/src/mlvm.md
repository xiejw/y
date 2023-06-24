# MLVM -- The Raw Machine

The MLVM is a virtual machine that I want to, and will, use for my own projects
for next decades. It might not fit others' needs; but I can understand and
reason about it. It is _simple_ and _efficient_.

It does not do any checks--behave
like a machine. And it provides raw accesses to the underlying stack, so it is
possible to do in-place operations and swap pointers, which could be dangerous
but certainly are critical for performance. Memory management is in user's
hands, and auto-grad is not integrated--yet it is just simple and fast.

## Design

The first level of the VM is designed to be a system library (along with a
runtime), but not a compiler tool.
It is super simple, easy to understand, and super efficient. Toward efficiency,
there will be no check, just like an ordinary machine. So caller should be very
confident about its correctness. A programming language and compiler built on
top of the VM is highly recommended, but not required.

Another design point is: How to define the model for parallelism. In particular,
how to use the multi-core system to gain performance. Two ways are possible. The
first one is offloading the computation to co-processors. With that, we need to
introduce async value, multi-threads for heavy operations e.g., matmuls. The
known challenges are also obvious: How to avoid cache line miss during processor
switch, and how to design a scheduling system which handles cheap operations
well---a chain of cheap operations might put most of the processors idle.

The alternative is to design the VM for single processor execution only, and do
it very well. Caller can launch multiple VM's each for single processor and
adjust the affinity if needed. With that, MPI style collective are needed. For
CPU within same OS process, we can provide a shared memory implementation for
the collective to avoid data copying.

## Memory Management

When the VM was prototyped, Tensors `struct vm_tensor` were designed to be owned
by the VM.  This was not necessary--the idea was motivated by the file system in
UNIX, so the handles can be provided to user land and it will not be confused
when to free the resources. Users are expected to free the resources when not
need them at future points. When call `vmFree`, the VM will free all attached
resources also, so users can rely on this to avoid free all handles if they are
lazy. This is OK as most of the Tensors have lifetime spanning as long as the
VM. All prototyped modeling examples actually statically allocate all Tensors
used for all training steps, which saves significant time to malloc/free,
repeatedly but unnecessarily. In case of sharing backing buffer across VMs,
swapping pointers should be fine.

Shapes, on the other hand, are slightly different. They were designed to be
reused, not only between VMs, but also across Tensors. So, the APIs are not
coupled with VM, in contrast to Tensors. In addition, they were designed to base
on reference count and, in future, can point to the same instance if the dimes
are same. During modeling writings, it becomes obvious that user code needs to
create too many shapes and free all of them in dozens of lines at the end of the
program. To make this easier, we provide `vmShapeNew` API to create a shape,
which can be reused in the two situations before; the VM is just responsible to
call `spDefRef` during vmFree stage, which is nice.

