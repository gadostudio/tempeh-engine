use crate::file::open_file;
use imgui::{im_str, ImStr, ImString, Selectable, TreeNode, Ui};
use std::collections::{HashMap, HashSet, VecDeque};
use std::ffi::OsStr;
use std::fs;
use std::path::{Iter, Path, PathBuf};

#[derive(Debug)]
pub struct FileTree(pub HashMap<PathBuf, Option<Box<FileTree>>>);

impl FileTree {
    pub fn new() -> Self {
        Self(HashMap::new())
    }

    pub fn from<P: Into<PathBuf>>(path: P) -> Self {
        let mut queue = VecDeque::new();
        queue.push_back(PathBuf::from(path.into()));

        let mut filetree = FileTree::new();

        while !queue.is_empty() {
            let current_entry = queue.pop_front().unwrap();
            for entry in fs::read_dir(current_entry).unwrap() {
                let entry = entry.unwrap();
                let path = entry.path();
                filetree.insert(path.clone());
                if path.is_dir() && path.file_name().unwrap() != OsStr::new(".git") {
                    queue.push_back(path);
                }
            }
        }

        filetree
    }

    fn insert_(&mut self, mut iter: Iter<'_>) -> bool {
        if let Some(name) = iter.next() {
            let key = PathBuf::from(name);
            if let Some(mut val_) = self.0.get_mut(&key) {
                if val_.is_none() {
                    *val_ = Some(Box::new(FileTree::new()));
                }
                val_.as_mut().unwrap().insert_(iter);
            } else {
                let mut file_tree = FileTree::new();
                let is_end = !file_tree.insert_(iter);
                self.0.insert(
                    key,
                    if is_end {
                        None
                    } else {
                        Some(Box::new(file_tree))
                    },
                );
            }
            true
        } else {
            false
        }
    }

    fn insert(&mut self, name: PathBuf) {
        self.insert_(name.iter());
    }
}

pub fn imgui_file_tree(ui: &Ui, file_tree: &FileTree, path: &mut PathBuf) {
    for (entry, child) in file_tree.0.iter() {
        path.push(entry);
        let entry_str = entry.as_os_str().to_str().unwrap().to_owned();
        let entry_imstr = ImString::from(entry_str);
        if child.is_none() {
            if Selectable::new(&entry_imstr).selected(false).build(ui) {
                open_file(path.clone());
            }
        } else {
            TreeNode::new(&entry_imstr).build(&ui, || {
                imgui_file_tree(ui, child.as_ref().unwrap().as_ref(), path);
            });
        }
        path.pop();
    }
}
