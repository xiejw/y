# vim: ft=help

--------------------------------------------------------------------------------
sds~

sds (sds_t) is the "simple dynamic string", simplified from
github.com/antirez/sds.

- Added EmtpyWithCap API to avoid double growing.
- sdsCatPrintf will try to write to the remaining buffer if possible to avoid
  the extra copy.

--------------------------------------------------------------------------------
vec~

vec (vec_t) is a (very) simple strong-type container. It is motivated by sds,
and has the following (similar) advantages:

- It is a pointer to an auto-grow array. So user can use vec[i] to
  access the elements efficiently. It just feels like a pointer.
  The type information is just correct.

- Cache line friendly. Under the hood, it is a c array.

- If the space is not enough, it will self-grow. So, passing
  pointer to the vec (type vec_t(<basic_type>)*) around if it
  needs to change.

- The size and cap are recorded in the space ahead of the first element but
  invisible to users. Helper methods are provided.

For simplicity, ownership is on user side. 'vecFree' just frees the resources
for the buffer it allocates. All elements are freed by users.

For example, if users put two heap-allocated objects in vec[0] and vec[1],
followed by vecSetSzie(vec, 1), this is a potential memeory leak as the second
item needs some care.

--------------------------------------------------------------------------------
list~

This is a general list strucutre.

- Similar to the redis design, user can see the free func, so during listFree
  call, all nodes will be passed to this user free func to release resource.
  This is very convenient.

- Improvement on redis list implementation, for builtin date types, say int or
  float, the value is embeded in the node so an extra memory load (including
  heap alloc/dealloc) could be avoided.

--------------------------------------------------------------------------------
dict~

This is a general dictionary data strucutre. The code is forked from redis and
simplified to remove the incremental rehashing. It is a nice feature but I dont
need.

One more change is the value type. The dict in redis has a very similar design
for entry value (not key), which is a union of values. This implementation
  replaces it with the `struct value_t`.

`value_t` is also useful for boxing the int or float for keys. Some allocations
or deallocations cost will be paid. Slab allocation can be provided in future if
needed.
