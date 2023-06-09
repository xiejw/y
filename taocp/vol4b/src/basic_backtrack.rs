pub fn backtrack<F>(v: &mut Vec<usize>, f: &F) -> usize
where
    F: Fn(&Vec<usize>) -> bool,
{
    return 0;
}

#[cfg(test)]
mod tests {
    use crate::basic_backtrack::backtrack;
    #[test]
    fn backtrack_empty() {
        let mut v = vec![1, 2];
        assert_eq!(backtrack(&mut v, &|x| true), 0);
    }
}
