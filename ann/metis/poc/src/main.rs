// VarTree the only data structures to manage variables (aka weights). Any
// sub-tree of VarTree is a VarTree. Any nodes are ordered.
//
// Given the tree structure, dtype and device can be inherited.
//
// VarTree supports path. Each Node, except root, has a unique name. So, a path
// can help users quickly walk to the tree, like.
//
//     tree.get_via_path("layer1.weight");
//
// or as simple as
//
//     tree.get_via_path("weight");
//
// Layer is pure computation. It is operated on a VarTree (likely a subtree). It
// could help to create the subtree but ok to use existing tree.
//
//
// VarTree can be marshalled and unmarshelled.
//
pub mod core {
    use std::sync::{Arc, Mutex};

    pub struct Var {}

    pub enum VarNode {
        SubTree(Box<VarTree>),
        VarNode(Arc<Mutex<Var>>),
    }

    pub struct VarTree {
        nodes: Vec<(String, VarNode)>,
    }

    impl VarTree {
        pub fn new() -> VarTree {
            VarTree { nodes: vec![] }
        }

        pub fn get_via_path(&self, path: &str) -> Option<&VarNode> {
            for c in path.split('.').into_iter() {
                println!("path {c}");
            }
            None
        }
    }
}

fn main() {
    let tree = core::VarTree::new();
    tree.get_via_path("a.b");
    println!("Hello, world!");
}
