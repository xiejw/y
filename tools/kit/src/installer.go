package src

import (
	"fmt"
	"io"
	"log"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
)

// Env holds paths used during installation.
type Env struct {
	BuildDir string // ~/Workspace/build
	UsrDir   string // ~/Workspace/build/usr
	BinDir   string // ~/Workspace/build/usr/bin
	LibDir   string // ~/Workspace/build/usr/lib
}

// NewEnv initializes the build environment from the user's home directory.
func NewEnv() (*Env, error) {
	home, err := os.UserHomeDir()
	if err != nil {
		return nil, err
	}
	build := filepath.Join(home, "Workspace", "build")
	usr := filepath.Join(build, "usr")
	return &Env{
		BuildDir: build,
		UsrDir:   usr,
		BinDir:   filepath.Join(usr, "bin"),
		LibDir:   filepath.Join(usr, "lib"),
	}, nil
}

// Tool represents a tool that can be installed.
type Tool struct {
	Name    string
	Bin     string   // binary filename to check in BinDir
	Lib     string   // library filename to check in LibDir (if set, takes precedence over Bin)
	Deps    []string // names of tools that must be installed before this one
	Install func(env *Env) error
}

// IsInstalled reports whether the tool's binary or library exists in the expected location.
func (t *Tool) IsInstalled(env *Env) bool {
	if t.Lib != "" {
		_, err := os.Stat(filepath.Join(env.LibDir, t.Lib))
		return err == nil
	}
	_, err := os.Stat(filepath.Join(env.BinDir, t.Bin))
	return err == nil
}

// run executes cmd in dir, streaming output to stdout/stderr.
func run(dir, name string, args ...string) error {
	cmd := exec.Command(name, args...)
	cmd.Dir = dir
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	return cmd.Run()
}

// runEnv is like run but prepends extra environment variables.
func runEnv(dir string, extraEnv []string, name string, args ...string) error {
	cmd := exec.Command(name, args...)
	cmd.Dir = dir
	cmd.Env = append(os.Environ(), extraEnv...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	return cmd.Run()
}

// copyFile copies src to dst with the given file mode.
func copyFile(src, dst string, mode os.FileMode) error {
	in, err := os.Open(src)
	if err != nil {
		return err
	}
	defer in.Close()

	out, err := os.OpenFile(dst, os.O_WRONLY|os.O_CREATE|os.O_TRUNC, mode)
	if err != nil {
		return err
	}
	defer out.Close()

	_, err = io.Copy(out, in)
	return err
}

// GoConfig configures a Go-based tool installation.
type GoConfig struct {
	RepoURL string // git clone URL
	BinName string // binary name (output name and target filename in BinDir)
}

// GoInstall creates an installer for tools built with 'go build'.
//
// Layout under BuildDir:
//
//	<BinName>/
//	  <BinName>/   <- git clone
//	    .build/    <- removed after install
//
// Example (glow):
//
//	~/Workspace/build/glow/
//	  git clone https://github.com/charmbracelet/glow.git
//	  cd glow && go build -o .build/glow ./
//	  mv .build/glow ~/Workspace/build/usr/bin/glow
func GoInstall(cfg GoConfig) func(*Env) error {
	return func(env *Env) error {
		workDir := filepath.Join(env.BuildDir, cfg.BinName)
		if err := os.MkdirAll(workDir, 0755); err != nil {
			return fmt.Errorf("mkdir: %w", err)
		}

		srcDir := filepath.Join(workDir, cfg.BinName)
		if _, err := os.Stat(srcDir); os.IsNotExist(err) {
			if err := run(workDir, "git", "clone", cfg.RepoURL); err != nil {
				return fmt.Errorf("git clone: %w", err)
			}
		}

		buildOut := filepath.Join(srcDir, ".build", cfg.BinName)
		if err := run(srcDir, "go", "build", "-o", buildOut, "./"); err != nil {
			return fmt.Errorf("go build: %w", err)
		}

		if err := os.Rename(buildOut, filepath.Join(env.BinDir, cfg.BinName)); err != nil {
			return fmt.Errorf("install: %w", err)
		}

		return os.RemoveAll(filepath.Join(srcDir, ".build"))
	}
}

// CConfig configures a C autoconf-based tool installation.
type CConfig struct {
	WorkDir    string   // subdirectory name under BuildDir
	TarURL     string   // tarball download URL
	ExtractDir string   // directory name created by tar extraction (e.g. "inetutils-2.6")
	ConfigArgs []string // extra args passed to ./configure (--prefix is added automatically)

	// ConfigureCmd overrides the configure script. Defaults to "./configure".
	ConfigureCmd string

	// InstallBin is the relative path to the binary within ExtractDir.
	// If set, the binary is copied to BinDir instead of running 'make install'.
	InstallBin string
	// BinName overrides the installed filename. Defaults to basename of InstallBin.
	BinName string

	CFLAGS  string // optional; set as env var before ./configure
	LDFLAGS string // optional; set as env var before ./configure

	SingleJob bool // if true, build with -j1 to avoid OOM
}

// CAutoconf creates an installer for C tools using ./configure && make.
//
// Layout under BuildDir:
//
//	<WorkDir>/
//	  <tarball>
//	  <ExtractDir>/  <- source tree (make clean runs here)
//
// Example (git, standard autoconf):
//
//	~/Workspace/build/git/
//	  curl -OL https://…/git-2.51.0.tar.gz
//	  tar xf git-2.51.0.tar.gz
//	  cd git-2.51.0 && ./configure --prefix=~/Workspace/build/usr
//	  make -j && make install && make clean
//
// Example (cmake, ConfigureCmd="./bootstrap"):
//
//	~/Workspace/build/cmake/
//	  curl -OL https://…/cmake-4.1.1.tar.gz
//	  tar xf cmake-4.1.1.tar.gz
//	  cd cmake-4.1.1 && ./bootstrap --prefix=~/Workspace/build/usr
//	  make -j && make install && make clean
func CAutoconf(cfg CConfig) func(*Env) error {
	tarFile := filepath.Base(cfg.TarURL)
	return func(env *Env) error {
		workDir := filepath.Join(env.BuildDir, cfg.WorkDir)
		if err := os.MkdirAll(workDir, 0755); err != nil {
			return fmt.Errorf("mkdir: %w", err)
		}

		tarPath := filepath.Join(workDir, tarFile)
		if _, err := os.Stat(tarPath); os.IsNotExist(err) {
			if err := run(workDir, "curl", "-OL", cfg.TarURL); err != nil {
				return fmt.Errorf("curl: %w", err)
			}
		}

		srcDir := filepath.Join(workDir, cfg.ExtractDir)
		if _, err := os.Stat(srcDir); os.IsNotExist(err) {
			if err := run(workDir, "tar", "xf", tarPath); err != nil {
				return fmt.Errorf("tar: %w", err)
			}
		}

		configCmd := cfg.ConfigureCmd
		if configCmd == "" {
			configCmd = "./configure"
		}
		configArgs := append([]string{"--prefix=" + env.UsrDir}, cfg.ConfigArgs...)
		var extraEnv []string
		if cfg.CFLAGS != "" {
			extraEnv = append(extraEnv, "CFLAGS="+cfg.CFLAGS)
		}
		if cfg.LDFLAGS != "" {
			extraEnv = append(extraEnv, "LDFLAGS="+cfg.LDFLAGS)
		}
		if len(extraEnv) > 0 {
			if err := runEnv(srcDir, extraEnv, configCmd, configArgs...); err != nil {
				return fmt.Errorf("configure: %w", err)
			}
		} else {
			if err := run(srcDir, configCmd, configArgs...); err != nil {
				return fmt.Errorf("configure: %w", err)
			}
		}

		makeJobs := "-j"
		if cfg.SingleJob {
			makeJobs = "-j1"
		}
		if err := run(srcDir, "make", makeJobs); err != nil {
			return fmt.Errorf("make: %w", err)
		}

		if cfg.InstallBin != "" {
			binName := cfg.BinName
			if binName == "" {
				binName = filepath.Base(cfg.InstallBin)
			}
			src := filepath.Join(srcDir, cfg.InstallBin)
			dst := filepath.Join(env.BinDir, binName)
			if err := copyFile(src, dst, 0755); err != nil {
				return fmt.Errorf("install binary: %w", err)
			}
		} else {
			if err := run(srcDir, "make", "install"); err != nil {
				return fmt.Errorf("make install: %w", err)
			}
		}

		return run(srcDir, "make", "clean")
	}
}

// BinaryDownloadConfig configures installing a prebuilt binary.
type BinaryDownloadConfig struct {
	BinName string            // installed filename in BinDir
	URLs    map[string]string // "<GOARCH>-<GOOS>" (e.g. "arm64-darwin") -> download URL
}

// BinaryDownload creates an installer that downloads a prebuilt binary for the
// current platform ("<GOARCH>-<GOOS>"). Platforms without a URL are an error.
//
// Layout under BuildDir:
//
//	<BinName>/
//	  <downloaded file>  <- moved into BinDir
//
// Example (fd on arm64):
//
//	~/Workspace/build/fd/
//	  curl -fL -o fd https://…/fd_arm64_darwin
//	  chmod 755 fd && mv fd ~/Workspace/build/usr/bin/fd
func BinaryDownload(cfg BinaryDownloadConfig) func(*Env) error {
	return func(env *Env) error {
		url, ok := cfg.URLs[runtime.GOARCH+"-"+runtime.GOOS]
		if !ok {
			return fmt.Errorf("no prebuilt binary for %s-%s", runtime.GOARCH, runtime.GOOS)
		}

		workDir := filepath.Join(env.BuildDir, cfg.BinName)
		if err := os.MkdirAll(workDir, 0755); err != nil {
			return fmt.Errorf("mkdir: %w", err)
		}

		tmp := filepath.Join(workDir, cfg.BinName)
		if err := run(workDir, "curl", "-fL", "-o", tmp, url); err != nil {
			return fmt.Errorf("curl: %w", err)
		}
		if err := os.Chmod(tmp, 0755); err != nil {
			return fmt.Errorf("chmod: %w", err)
		}
		if err := os.Rename(tmp, filepath.Join(env.BinDir, cfg.BinName)); err != nil {
			return fmt.Errorf("install: %w", err)
		}
		return nil
	}
}

// CMakeBuildConfig configures a CMake-based tool installation.
type CMakeBuildConfig struct {
	WorkDir    string   // subdirectory name under BuildDir
	TarURL     string   // tarball download URL
	ExtractDir string   // directory name created by tar extraction
	CMakeArgs  []string // extra -D flags (install prefix is added automatically)
	SingleJob  bool     // if true, build with -j1 to avoid OOM
}

// CMakeBuild creates an installer for tools built with CMake.
//
// Layout under BuildDir:
//
//	<WorkDir>/
//	  <tarball>
//	  <ExtractDir>/  <- source tree
//	    build/       <- removed after install
//
// Example (ninja):
//
//	~/Workspace/build/ninja/
//	  curl -OL https://…/ninja-1.13.1.tar.gz
//	  tar xf ninja-1.13.1.tar.gz
//	  cd ninja-1.13.1
//	  cmake -B build -S . -DCMAKE_INSTALL_PREFIX=~/Workspace/build/usr
//	  cmake --build build && cmake --install build
//	  rm -rf build/
func CMakeBuild(cfg CMakeBuildConfig) func(*Env) error {
	tarFile := filepath.Base(cfg.TarURL)
	return func(env *Env) error {
		workDir := filepath.Join(env.BuildDir, cfg.WorkDir)
		if err := os.MkdirAll(workDir, 0755); err != nil {
			return fmt.Errorf("mkdir: %w", err)
		}

		tarPath := filepath.Join(workDir, tarFile)
		if _, err := os.Stat(tarPath); os.IsNotExist(err) {
			if err := run(workDir, "curl", "-OL", cfg.TarURL); err != nil {
				return fmt.Errorf("curl: %w", err)
			}
		}

		srcDir := filepath.Join(workDir, cfg.ExtractDir)
		if _, err := os.Stat(srcDir); os.IsNotExist(err) {
			if err := run(workDir, "tar", "xf", tarPath); err != nil {
				return fmt.Errorf("tar: %w", err)
			}
		}

		cmakeArgs := append([]string{"-B", "build", "-S", ".", "-DCMAKE_INSTALL_PREFIX=" + env.UsrDir}, cfg.CMakeArgs...)
		if err := run(srcDir, "cmake", cmakeArgs...); err != nil {
			return fmt.Errorf("cmake configure: %w", err)
		}

		buildArgs := []string{"--build", "build"}
		if cfg.SingleJob {
			buildArgs = append(buildArgs, "--parallel", "1")
		}
		if err := run(srcDir, "cmake", buildArgs...); err != nil {
			return fmt.Errorf("cmake build: %w", err)
		}

		if err := run(srcDir, "cmake", "--install", "build"); err != nil {
			return fmt.Errorf("cmake install: %w", err)
		}

		return os.RemoveAll(filepath.Join(srcDir, "build"))
	}
}

// InstallToolsWithFilter installs every tool in tools that is not already
// installed, along with its Deps. If filter is non-empty, only the tool with
// that name is considered. Exits the process on the first failure.
func InstallToolsWithFilter(env *Env, tools []*Tool, filter string) {
	toolsByName := map[string]*Tool{}
	for _, t := range tools {
		toolsByName[t.Name] = t
	}

	var installTool func(t *Tool) error
	installTool = func(t *Tool) error {
		for _, depName := range t.Deps {
			dep := toolsByName[depName]
			if dep == nil {
				log.Fatalf("unknown dep %q for %s", depName, t.Name)
			}
			if dep.IsInstalled(env) {
				continue
			}
			fmt.Printf("[install] %s (dep of %s)\n", dep.Name, t.Name)
			if err := installTool(dep); err != nil {
				log.Fatalf("[error]   %s: %v", dep.Name, err)
			}
			fmt.Printf("[done]    %s\n", dep.Name)
		}
		return t.Install(env)
	}

	for _, t := range tools {
		if filter != "" && t.Name != filter {
			continue
		}
		if t.IsInstalled(env) {
			dir := env.BinDir
			if t.Lib != "" {
				dir = env.LibDir
			}
			fmt.Printf("[skip]    %s (already in %s)\n", t.Name, dir)
			continue
		}
		fmt.Printf("[install] %s\n", t.Name)
		if err := installTool(t); err != nil {
			log.Fatalf("[error]   %s: %v", t.Name, err)
		}
		fmt.Printf("[done]    %s\n", t.Name)
	}

	fmt.Printf("\n")
	fmt.Printf("=== --- Notes --- ===\n")
	fmt.Printf("export PATH=%s:$PATH\n", env.BinDir)
	fmt.Printf("export LD_LIBRARY_PATH=%s:$LD_LIBRARY_PATH\n", env.LibDir)
}
