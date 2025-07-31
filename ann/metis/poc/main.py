# VarTree the only data structures to manage variables (aka weights). Any
# sub-tree of VarTree is a VarTree. Any nodes are ordered.
#
# Given the tree structure, dtype and device can be inherited.
#
# VarTree supports path. Each Node, except root, has a unique name. So, a path
# can help users quickly walk to the tree, like.
#
#     tree.get_via_path("layer1.weight");
#
# or as simple as
#
#     tree.get_via_path("weight");
#
# Layer is pure computation. It is operated on a VarTree (likely a subtree). It
# could help to create the subtree but ok to use existing tree.
#
#
# VarTree can be marshalled and unmarshelled.
#
class VarTree:
    pass

   def get_or_create(name: str, dims: list[int], dtype: DType = f32):
       pass

class Dense:
    def __init__(self, sub_tree: VarTree, in_size: int, out_size: int):
        self.tree = sub_tree
        self.w = sefl.tree.get_or_create('weight', [in_size, out_size]

        pass

def main():
    print("Hello from poc!")


if __name__ == "__main__":
    main()
