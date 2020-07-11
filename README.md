# fastvm
arm virtual engine

# build 

git clone https://github.com/baikaishiuc/fastvm

cd fastvm

mkdir build

cd build

cmake ..

open vs2017 cmd in fastvm/build

msbuild fastvm.sln

# Run

fastvm -df 407d libmakeurl2.4.9.so

you could change df address to: 
342d
407d
2f5d
2635
2b3d
367d

That all is libmakeurl2.4.9.so function address, after command execute success, it will generate the same name so's directory. You can invoke gen.sh in directory to generate cfg.

You need install dot.exe at first
