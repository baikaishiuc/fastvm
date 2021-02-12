# First

This github include 2 project.

dobc: vmp decompiler

fastvm: ollvm decompiler

# build

git clone https://github.com/baikaishiuc/fastvm

cd fastvm

mkdir build

cd build

cmake ..

open vs2017 cmd in fastvm/build

msbuild fastvm.sln

## dobc run

这个示例，必须得release版本下运行，否则速度很慢，请参考这篇文章
1.	https://blog.csdn.net/wwwsssZheRen/article/details/81197460

dobc  ../../../Processors/ARM/data/languages/ARM8_le.sla ../../../data/vmp/360_1/libjiagu.so

这个会在当前目录下生成一个 ./libjiagu.so的同名文件夹，里面带有final.dot后缀的文件，就是优化的结果。在目录下，执行../gen.sh会生成对应的svg文件

默认提供了5个函数可以测试，都是libjiagu.so里的:

```
void dobc::plugin_dvmp360()
{
    funcdata *fd_main = find_func("_Z10__arm_a_21v");
    //funcdata *fd_main = find_func("_Z9__arm_a_1P7_JavaVMP7_JNIEnvPvRi");
    //funcdata *fd_main = find_func("_Z9__arm_a_2PcjS_Rii");
    //funcdata *fd_main = find_func("_ZN10DynCryptor9__arm_c_0Ev");
    //funcdata *fd_main = find_func("_ZN9__arm_c_19__arm_c_0Ev");
```


## fastvm Run

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
