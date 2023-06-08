pub mod dancing_link {

    use std::boxed::Box;

    #[repr(C)]
    union HeadIdOrCount {
        col_head_id: usize,
        col_count: usize,
    }

    struct Node<T> {
        id: usize,
        data: Option<Box<T>>,

        // value is the index of the cell it points to
        l: usize,
        r: usize,
        u: usize,
        d: usize,
        col: HeadIdOrCount,
    }

    impl<T> Node<T> {
        // TODO: for best performance, we should inline alloc all nodes and set
        // them in-place.
        fn new(id: usize) -> Node<T> {
            Node {
                id: id,
                data: None,
                l: id,
                r: id,
                u: id,
                d: id,
                col: HeadIdOrCount { col_count: 0 },
            }
        }
    }

    pub struct Table<T> {
        num_col_heads: usize,
        num_nodes: usize,
        nodes: Vec<Node<T>>,
    }

    impl<T: Default> Table<T> {
        pub fn new(capacity: usize) -> Table<T> {
            assert!(capacity >= 1);
            let mut nodes = Vec::<Node<T>>::with_capacity(capacity);
            nodes.push(Node::new(0));
            Table {
                num_col_heads: 0,
                num_nodes: 1,
                nodes: nodes,
            }
        }
    }

    #[cfg(test)]
    mod tests {
        use crate::dancing_link::Table;
        #[test]
        fn new_table() {
            let t = Table::<()>::new(1);
            assert_eq!(t.num_nodes, 1);
            assert_eq!(t.num_col_heads, 0);

            let n = &t.nodes[0];
            assert_eq!(n.id, 0);
            assert_eq!(n.data, None);
            assert_eq!(n.l, 0);
            assert_eq!(n.r, 0);
            assert_eq!(n.u, 0);
            assert_eq!(n.d, 0);
            assert_eq!(unsafe { n.col.col_count }, 0);
        }
    }
}

fn main() {
    println!("Hello, world!");
}
