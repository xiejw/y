// Command prompt prints the dynamic part of the bash PS1 prompt:
//
//	(<branch>[*|-?]) [--vim-- ]
//
// `*` marks pending git changes. When LITE_PROMPT is set, the (potentially
// slow) pending check is skipped and `-?` is printed instead. `--vim--` shows
// that a vim process is running in this terminal (e.g. suspended with ctrl-z).
package main

import (
	"bytes"
	"fmt"
	"os"
	"os/exec"
	"strings"
)

func main() {
	var b strings.Builder
	if name, ok := gitBranchName(); ok {
		b.WriteString("(" + name)
		if _, lite := os.LookupEnv("LITE_PROMPT"); lite {
			b.WriteString("-?") // one - to make it clear
		} else if gitPending() {
			b.WriteString("*")
		}
		b.WriteString(") ")
	}
	if vimRunInBg() {
		b.WriteString("--vim-- ")
	}
	fmt.Print(b.String())
}

// run executes the command and returns its stdout; stderr is discarded. A
// failed command (e.g. not in a git repo) yields whatever stdout it produced.
func run(name string, args ...string) []byte {
	out, _ := exec.Command(name, args...).Output()
	return out
}

// vimRunInBg reports whether any process attached to this terminal is vim.
//
// `ps -T` (macOS/BSD) lists only processes attached to the terminal on stdin,
// i.e. the shell's own tty, so a vim suspended with ctrl-z here is caught but
// one in another tmux pane is not. On Linux (procps), -T instead means "show
// threads", but the default selection is already "same user, same terminal",
// so it behaves about the same. The match is a plain substring, so any
// command line containing "vim" counts.
//
// exec.Command connects the child's stdin to /dev/null by default, which makes
// `ps -T` fail with "stdin: not a terminal", so pass our own stdin (the
// shell's tty) through explicitly.
func vimRunInBg() bool {
	cmd := exec.Command("ps", "-T")
	cmd.Stdin = os.Stdin
	out, _ := cmd.Output()
	return bytes.Contains(out, []byte("vim"))
}

// gitPending reports whether the working tree has uncommitted changes.
func gitPending() bool {
	return len(bytes.TrimSpace(run("git", "status", "-s"))) > 0
}

// gitBranchName returns the current branch name, or the short commit hash when
// HEAD is detached. ok is false outside a git repository.
//
// Why this is fast: `git symbolic-ref HEAD` only reads the single file
// .git/HEAD (e.g. "ref: refs/heads/main") and never touches the working tree
// or enumerates other refs, so its cost is constant regardless of repository
// size or branch count. By contrast, `git branch` lists every local branch
// (loose refs plus packed-refs) just to find the one marked `*`. The fallback
// `git rev-parse --short HEAD` is equally cheap: it resolves HEAD to a commit
// id and abbreviates it, again without scanning the working tree.
func gitBranchName() (string, bool) {
	// -q: exit silently (no stderr noise) when HEAD is not a symbolic ref,
	// i.e. detached HEAD.
	if name := strings.TrimSpace(string(run("git", "symbolic-ref", "--short", "-q", "HEAD"))); name != "" {
		return name, true
	}
	if hash := strings.TrimSpace(string(run("git", "rev-parse", "--short", "HEAD"))); hash != "" {
		return hash, true
	}
	return "", false
}
