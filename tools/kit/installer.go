package main

import (
	"fmt"
	"io"
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

		if err := run(srcDir, "make", "-j"); err != nil {
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

// RustConfig configures a Rust toolchain installation.
type RustConfig struct {
	Version string // e.g. "1.91.0"
}

// rustTargetTriple returns the Rust target triple for the current platform.
func rustTargetTriple() (string, error) {
	switch runtime.GOOS + "/" + runtime.GOARCH {
	case "darwin/arm64":
		return "aarch64-apple-darwin", nil
	case "darwin/amd64":
		return "x86_64-apple-darwin", nil
	case "linux/amd64":
		return "x86_64-unknown-linux-gnu", nil
	case "linux/arm64":
		return "aarch64-unknown-linux-gnu", nil
	default:
		return "", fmt.Errorf("no Rust target triple for %s/%s", runtime.GOOS, runtime.GOARCH)
	}
}

// RustInstall creates an installer for Rust using a pre-built tarball from static.rust-lang.org.
//
// Layout under BuildDir:
//
//	rust/
//	  rust-<Version>-<triple>.tar.xz
//	  rust-<Version>-<triple>/
//	    install.sh
//
// Rust installs to <UsrDir>/rust/, and cargo/rustc/rustup are symlinked into BinDir.
func RustInstall(cfg RustConfig) func(*Env) error {
	return func(env *Env) error {
		triple, err := rustTargetTriple()
		if err != nil {
			return fmt.Errorf("unsupported platform: %w", err)
		}
		tarName := fmt.Sprintf("rust-%s-%s.tar.xz", cfg.Version, triple)
		extractDir := fmt.Sprintf("rust-%s-%s", cfg.Version, triple)
		url := "https://static.rust-lang.org/dist/" + tarName

		workDir := filepath.Join(env.BuildDir, "rust")
		if err := os.MkdirAll(workDir, 0755); err != nil {
			return fmt.Errorf("mkdir: %w", err)
		}

		tarPath := filepath.Join(workDir, tarName)
		if _, err := os.Stat(tarPath); os.IsNotExist(err) {
			if err := run(workDir, "curl", "-OL", url); err != nil {
				return fmt.Errorf("curl: %w", err)
			}
		}

		srcDir := filepath.Join(workDir, extractDir)
		if _, err := os.Stat(srcDir); os.IsNotExist(err) {
			if err := run(workDir, "tar", "xf", tarPath); err != nil {
				return fmt.Errorf("tar: %w", err)
			}
		}

		rustPrefix := filepath.Join(env.UsrDir, "rust")
		if err := run(srcDir, "./install.sh", "--prefix="+rustPrefix); err != nil {
			return fmt.Errorf("install.sh: %w", err)
		}
		// rustfmt is not included in the default rust distribution; install it separately.
		if err := run(srcDir, "./install.sh", "--prefix="+rustPrefix, "--components=rustfmt-preview"); err != nil {
			return fmt.Errorf("install.sh rustfmt-preview: %w", err)
		}

		for _, bin := range []string{"cargo", "rustc", "rustup"} {
			src := filepath.Join(rustPrefix, "bin", bin)
			if _, err := os.Stat(src); os.IsNotExist(err) {
				continue
			}
			dst := filepath.Join(env.BinDir, bin)
			os.Remove(dst)
			if err := os.Symlink(src, dst); err != nil {
				return fmt.Errorf("symlink %s: %w", bin, err)
			}
		}

		return nil
	}
}

// RustCargoConfig configures a Rust cargo-based tool installation.
type RustCargoConfig struct {
	RepoURL string // git clone URL
	BinName string // binary name in target/release/ and BinDir
	RepoDir string // git clone directory name; defaults to BinName if empty
}

// RustCargoInstall creates an installer for tools built with 'cargo build --release'.
//
// Layout under BuildDir:
//
//	<BinName>/
//	  <BinName>/   <- git clone
//	    target/    <- removed by cargo clean
//
// Example (fd):
//
//	~/Workspace/build/fd/
//	  git clone https://github.com/sharkdp/fd.git
//	  cd fd && cargo build --release --locked
//	  mv target/release/fd ~/Workspace/build/usr/bin/fd
//	  cargo clean
func RustCargoInstall(cfg RustCargoConfig) func(*Env) error {
	return func(env *Env) error {
		workDir := filepath.Join(env.BuildDir, cfg.BinName)
		if err := os.MkdirAll(workDir, 0755); err != nil {
			return fmt.Errorf("mkdir: %w", err)
		}

		repoDir := cfg.RepoDir
		if repoDir == "" {
			repoDir = cfg.BinName
		}
		srcDir := filepath.Join(workDir, repoDir)
		if _, err := os.Stat(srcDir); os.IsNotExist(err) {
			if err := run(workDir, "git", "clone", cfg.RepoURL); err != nil {
				return fmt.Errorf("git clone: %w", err)
			}
		}

		if err := run(srcDir, "cargo", "build", "--release", "--locked"); err != nil {
			return fmt.Errorf("cargo build: %w", err)
		}

		src := filepath.Join(srcDir, "target", "release", cfg.BinName)
		dst := filepath.Join(env.BinDir, cfg.BinName)
		if err := os.Rename(src, dst); err != nil {
			return fmt.Errorf("install: %w", err)
		}

		return run(srcDir, "cargo", "clean")
	}
}

// CMakeBuildConfig configures a CMake-based tool installation.
type CMakeBuildConfig struct {
	WorkDir    string   // subdirectory name under BuildDir
	TarURL     string   // tarball download URL
	ExtractDir string   // directory name created by tar extraction
	CMakeArgs  []string // extra -D flags (install prefix is added automatically)
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

		if err := run(srcDir, "cmake", "--build", "build"); err != nil {
			return fmt.Errorf("cmake build: %w", err)
		}

		if err := run(srcDir, "cmake", "--install", "build"); err != nil {
			return fmt.Errorf("cmake install: %w", err)
		}

		return os.RemoveAll(filepath.Join(srcDir, "build"))
	}
}
