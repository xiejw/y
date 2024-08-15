import sys
assert sys.version_info >= (3, 10), "Use Python 3.10 or newer"

import os
from pathlib import Path

FILE_NAME = "~/.profile"
FILE_NAME = os.path.expanduser(FILE_NAME)

CurrentContent = Path(FILE_NAME).read_text()
TargetStr = """# xiejw/y/dotfiles
export DOTFILES=~/Workspace/y/dotfiles
source ~/Workspace/y/dotfiles/conf/bash_profile
"""

if CurrentContent.find(TargetStr) != -1:
    print("We are good for " + FILE_NAME)
else:
    f = open(FILE_NAME, 'a')
    f.write("\n" + TargetStr)
    print("Patched " + FILE_NAME)
