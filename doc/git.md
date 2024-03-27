Tags
----

```
# Create and push to remote
git tag -a v0.0.1 -m "demo tag"
git push origin v0.0.1

# List
git tag
git tag -l 'v*'  # -l <pattern>

# Checkout
git checkout tags/v2.2.1
```

Submodules
----------

Ref: https://git-scm.com/book/en/v2/Git-Tools-Submodules

```bash
# Add submodule
git submodule add https://github.com/abseil/abseil-cpp.git third_party/absl

# Clone project and initialize.
#
# one cmd `git clone --recurse-submodules ...`
# or      `git submodule update --init --recursive`
git clone ...
git submodule init
git submodule update


# Workflow after set up
git submodule update
```

Branches
--------

```
# Normal workflow
git switch <branch_name>
git push --set-upstream origin <branch_name>   # -u is same as --set-upstream
git push origin                                # push to remote
git pull --rebase -v

# Remote branch
git ls-remote <remote>
git branch -vv              # check  branch along with relationship of upstreams

# Delete
git branch -D <local-branch>
git push origin --delete <remote_branch_name>
```

Barebone Repo
-------------

```
# create bare from empty.
git init --bare test_repo.git
```

```
# Create bare from existing retro
cd repo
mv .git ../repo.git # renaming just for clarity
cd ..
rm -fr repo
cd repo.git
git config --bool core.bare true
```


Unfinished
----------

```
# Add an existing project to github
git remote add origin remote repository URL
git remote -v
git push -u origin master

## list conflicted files
​git diff --name-only --diff-filter=U

## branch graph
git log --graph --oneline --all

## follow change in  master
<current brach non-master> git rebase master
https://git-scm.com/docs/git-rebase

##merge branch "test" into master
git checkout master
git merge test
git branch -d test
git diff test..master

## Revert to an old commit
git revert --no-commit 0766c053..HEAD
git commit
This will revert everything from the HEAD back to the commit hash,




on xieqi

git config --global init.defaultBranch main


sudo mkdir /opt/git
sudo chown xiejw /opt/git/
cd /opt/git
git init --initial-branch=main --bare tax2022.git
chmod -R o-r tax2021.git/
chmod -R g-r tax2021.git/


on client
git remote add origin xieqi.org:/opt/git/tax2021.git


#  push local branch to an empty remote bare repo
[local ]git remote add origin xieqi.org:/opt/git/db.git
[remote ]git init --initial-branch=main --bare db.git
git push -u origin main



git clone --depth=1


Use `git diff` to check the difference between two external files (one could be inside repo).

git diff --color=always --word-diff file1 file2 | less -R
git diff --name-only

git log origin/master..HEAD
git log --oneline origin/master...HEAD

github ?ts=2 to format tab


Removing untracked files
    git clean -f -d
Always
  git pull --rebase

# git revert
git revert --no-commit 0766c053..HEAD
git commit
```
