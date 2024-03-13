set -e

# Set VERSION
# VERSION=6.8

wget https://cdn.kernel.org/pub/linux/kernel/v6.x/linux-${VERSION}.tar.xz
tar xvf linux-${VERSION}.tar.xz
rm -rf linux-*-src
rm -rf linux
mv linux-${VERSION} linux-${VERSION}-src
ln -sf linux-${VERSION}-src linux
cp *-config linux/.config
mv *-config configs
cd linux
make olddefconfig
cp .config ../linux-${VERSION}-config
time make -j1
sudo make modules_install
sudo ~/Workspace/y/tools/kernel_install.sh
