// Command sync_git_repo runs `git pull --rebase` for every repository listed in
// ~/.git_repo_list (one path per line; empty lines ignored).
package main

import (
	"bufio"
	"errors"
	"fmt"
	"os"
	"os/exec"
	"os/signal"
	"path/filepath"
	"strings"
	"syscall"
)

const (
	configPath   = "~/.git_repo_list"
	maxRepoCount = 50
)

// ----------------------------------------------------------------------------
// Color printing.
// ----------------------------------------------------------------------------

const (
	colorError   = 31 // red
	colorSuccess = 32 // green
	colorFYI     = 33 // yellow
	colorInfo    = 36 // cyan
)

func cPrintf(color int, format string, args ...any) {
	fmt.Printf("\033[1;%dm"+format+"\033[0m", append([]any{color}, args...)...)
}

// ----------------------------------------------------------------------------
// Main.
// ----------------------------------------------------------------------------

var errTermSig = errors.New("terminated by signal")

func main() {
	repos, err := readRepoList(configPath)
	if err != nil {
		os.Exit(1)
	}
	if err := gitPullRepos(repos); err != nil {
		os.Exit(1)
	}
}

// ----------------------------------------------------------------------------
// Implementation.
// ----------------------------------------------------------------------------

// expandPath expands a leading `~` and any environment variables in path.
func expandPath(path string) (string, error) {
	path = os.ExpandEnv(path)
	if path == "~" || strings.HasPrefix(path, "~/") {
		home, err := os.UserHomeDir()
		if err != nil {
			return "", err
		}
		path = filepath.Join(home, path[1:])
	}
	return path, nil
}

// readRepoList reads the repo-list config file. A missing file yields an
// empty list and no error.
func readRepoList(path string) ([]string, error) {
	normalized, err := expandPath(path)
	if err != nil {
		cPrintf(colorError, "Error: not a valid path\n  Config File at: %s\n", path)
		return nil, err
	}

	f, err := os.Open(normalized)
	if errors.Is(err, os.ErrNotExist) {
		cPrintf(colorFYI, "Skip config file as not existed: %s\n", normalized)
		return nil, nil
	}
	if err != nil {
		cPrintf(colorError, "Failed to open config File at: %s\n", normalized)
		return nil, err
	}
	defer f.Close()

	var repos []string
	scanner := bufio.NewScanner(f)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		if len(repos) >= maxRepoCount {
			cPrintf(colorError, "Too many lines in config File at: %s\n", normalized)
			return nil, errors.New("too many repos")
		}
		repos = append(repos, line)
	}
	if err := scanner.Err(); err != nil {
		cPrintf(colorError, "Failed to read config File at: %s\n", normalized)
		return nil, err
	}
	return repos, nil
}

// gitPullRepos pulls each repo in order. Only fatal errors (invalid path or
// termination via signal, e.g. ctrl-c) stop the loop.
func gitPullRepos(repos []string) error {
	for _, repo := range repos {
		if err := gitPull(repo); err != nil {
			return err
		}
	}
	return nil
}

// gitPull invokes `git pull --rebase` for the repo at path.
func gitPull(path string) error {
	// Step 1: normalize the git repository path.
	normalized, err := expandPath(path)
	if err != nil {
		cPrintf(colorError, "Error: not a valid path\n  Repo at: %s\n", path)
		return err
	}

	// After this point, all errors except termination via signal are logged
	// and skipped.

	// Step 2: check existence.
	if _, err := os.Stat(normalized); err != nil {
		cPrintf(colorFYI, "Skip repository as not existed\n  Repo at: %s\n", normalized)
		return nil
	}

	// Step 3: pull repository.
	cPrintf(colorInfo, "Pulling: %s\n", normalized)

	switch err := runGitPull(normalized); {
	case err == nil:
		cPrintf(colorSuccess, "Success.\n")
	case errors.Is(err, errTermSig):
		cPrintf(colorError, "Error: interrupted by a signal.\n Repo at: %s\n", normalized)
		return err
	default:
		cPrintf(colorError, "Error: failed to pull the git repo.\n  Repo at: %s\n", normalized)
	}
	return nil
}

// runGitPull runs `git pull --rebase` in dir. Like system(3), SIGINT/SIGQUIT
// are held off in this process while the child runs, so ctrl-c only kills git
// and is then reported as errTermSig.
func runGitPull(dir string) error {
	sigs := make(chan os.Signal, 1)
	signal.Notify(sigs, os.Interrupt, syscall.SIGQUIT)
	defer signal.Stop(sigs)

	cmd := exec.Command("git", "pull", "--rebase")
	cmd.Dir = dir
	cmd.Stdin, cmd.Stdout, cmd.Stderr = os.Stdin, os.Stdout, os.Stderr

	err := cmd.Run()
	var exitErr *exec.ExitError
	if errors.As(err, &exitErr) {
		if ws, ok := exitErr.Sys().(syscall.WaitStatus); ok && ws.Signaled() {
			return errTermSig
		}
	}
	return err
}
