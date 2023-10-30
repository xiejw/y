Submodules
----------

Ref: https://git-scm.com/book/en/v2/Git-Tools-Submodules
```bash
# add submodule
git submodule add https://github.com/abseil/abseil-cpp.git third_party/absl

# clone project
#
# one cmd `git clone --recurse-submodules ...`
# or      `git submodule update --init --recursive`
git clone ...
git submodule init
git submodule update


git submodule update

```
