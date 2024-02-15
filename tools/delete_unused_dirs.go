package main

import (
	"io/fs"
	"log"
	"os"
	os_path "path"
	"path/filepath"
	"runtime/debug"
	"strings"
)

func main() {
	log.SetFlags(log.Ldate | log.Ltime | log.Lmicroseconds | log.Lshortfile)

	gw := &GabageWiper{
		RootDir: ".",
	}

	err := gw.Init()
	ValidateError(err)

	err = gw.Work()
	ValidateError(err)
}

type GabageWiper struct {
	RootDir       string
	candidateDirs map[string]bool
}

func (gw *GabageWiper) Init() error {
	gw.candidateDirs = make(map[string]bool)

	// ---------------------------------------------------------------------
	// emit ths dirs for the first round of workOnCandidates
	// ---------------------------------------------------------------------
	walkFunc := func(path string, d fs.DirEntry, err error) error {
		if !d.IsDir() {
			return nil
		}

		if filepath.Base(path) == ".git" {
			return fs.SkipDir
		}

		// We don't go into the code as the folder will be entirely
		// removed.
		if strings.HasPrefix(filepath.Base(path), ".build") {
			gw.candidateDirs[path] = true
			return fs.SkipDir
		}

		gw.candidateDirs[path] = true
		return nil
	}

	return filepath.WalkDir(gw.RootDir, walkFunc)
}

func (gw *GabageWiper) Work() error {
	for {
		workDone := gw.workOnCandidates()
		if workDone {
			break
		}
	}
	return nil
}

func (gw *GabageWiper) workOnCandidates() bool {
	workDone := true
	actions := make([]Action, 0)

	dirs := gw.candidateDirs
	gw.candidateDirs = make(map[string]bool)

	for dirToConsider, _ := range dirs {

		files, err := os.ReadDir(dirToConsider)
		ValidateError(err)

		// Rule 1: Empty Dir
		if len(files) == 0 {
			actions = append(actions, Action{
				kind: DelDirEmpty,
				path: dirToConsider,
			})
			workDone = false
			continue
		}

		// Rule 2: If the path starts with ".build", delete it
		if strings.HasPrefix(filepath.Base(dirToConsider), ".build") {
			actions = append(actions, Action{
				kind: DelDirAll,
				path: dirToConsider,
			})
			workDone = false
			continue
		}

		// Rule 3: If file a.out exists, delete it
		path_to_a_out := os_path.Join(dirToConsider, "a.out")
		if _, err := os.Stat(path_to_a_out); err == nil {
			actions = append(actions, Action{
				kind: DelFile,
				path: path_to_a_out,
			})
			workDone = false
			continue
		}
	}

	for _, act := range actions {
		log.Printf("%12v: %v", act.kind, act.path)
		switch act.kind {
		case DelDirEmpty:
			err := os.Remove(act.path)
			ValidateError(err)

			nextDirToWork := filepath.Dir(act.path)
			gw.candidateDirs[nextDirToWork] = true

		case DelDirAll:
			err := os.RemoveAll(act.path)
			ValidateError(err)

			nextDirToWork := filepath.Dir(act.path)
			gw.candidateDirs[nextDirToWork] = true

		case DelFile:
			err := os.Remove(act.path)
			ValidateError(err)

			nextDirToWork := filepath.Dir(act.path)
			gw.candidateDirs[nextDirToWork] = true
		default:
			panic("unsupported ActionKind")
		}
	}

	return workDone
}

// -----------------------------------------------------------------------------
// action kind ds.
// -----------------------------------------------------------------------------

type ActionKind int

const (
	DelDirEmpty ActionKind = iota
	DelDirAll
	DelFile
)

type Action struct {
	kind ActionKind
	path string
}

func (k ActionKind) String() string {
	switch k {
	case DelDirEmpty:
		return "DelDirEmpty"
	case DelDirAll:
		return "DelDirAll"
	case DelFile:
		return "DelFile"
	default:
		panic("unsupported ActionKind")
	}
}

// -----------------------------------------------------------------------------
// helper
// -----------------------------------------------------------------------------
func ValidateError(err error) {
	if err != nil {
		debug.PrintStack()
		log.Fatalf("unexpected err: %v", err)
	}
}
