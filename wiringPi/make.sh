export CCC="/home/groot/.software/raspi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc"
export envPath="/home/groot/Documents/ObjetCo/BNBR/wiringPi/outstatic"
export SYSROOT="/usr/arm-linux-gnueabi/lib"

# Install WiringPi
cd wiringPi
make clean
export DESTDIR="$envPath/rootRPI"
export PREFIX=""
export CC=$CCC
make -j$(nproc) V=1
make install

# Install WiringPiDev
cd ../devLib
make clean
export DESTDIR="$envPath/rootRPI"
export PREFIX=""
export CC="$CCC -I$DESTDIR/include -I$envPath/wiringPi"
make -j$(nproc) V=1
make install

# Install GPIO
cd ../gpio
make clean
export DESTDIR="$envPath/rootRPI"
export PREFIX=""
export CC="$CCC -I$envPath/wiringPi"
make -j$(nproc) V=1
make install
