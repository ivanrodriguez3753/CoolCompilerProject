#flex
sudo apt install flex=2.6.4-8

#make
sudo apt install make=4.3-4ubuntu1

#bison
wget ftp.gnu.org/gnu/bison/bison-3.7.2.tar.gz
tar -xf bison-3.7.2.tar.gz
cd bison-3.7.2
./configure
make
sudo make install
cd ../

#cmake
sudo apt install cmake-3.16.3-3ubuntu2

#clang
sudo apt install clang-11=1:11.0.0-2

#libz (for the C++ LLVMIR API)
sudo apt install zlib1g-dev=1:1.2.11.dfsg-2ubuntu4

