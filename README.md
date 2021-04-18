The following build instructions assume you are on a fresh new Ubuntu 20.10 VM. I personally recommend VMware products because they make it painless to set up a new VM. I used VMware Fusion to test on a macOS host, or VMware Workstation to test on a Windows host. 

1. Install git <br/>
  $ sudo apt install git <br/>

2. Clone this repo  <br/>
  $ git clone https://github.com/ivanrodriguez3753/CoolCompilerProject.git <br/>
  
3. cd into the cloned repo and run the dependency script <br/>
  $ cd CoolCompilerProject <br/>
  $ chmod +x installDependenciesUbuntu20.10.sh <br/>
  $ ./installDependenciesUbuntu20.10.sh <br/>
  
4. All dependencies should be good to go. Now we can build the CoolCompilerProject (and test project) following the usual build-from-source steps: <br/>
  $ mkdir build <br/>
  $ cd build <br/>
  $ cmake .. <br/>
  $ make <br/>
  
5. To run the compiler, cd into ~/build/src and run the executable with the optional --ast flag and with the required filename.cl parameter. If the input file is well-formed, it will produce filename.ll <br/>
  $ cd src <br/>
  $ ./CoolCompilerProject hello-world.cl <br/>

6. To run the test project, copy the apporpriate reference compiler into a new file named "cool". The reference compiler and its destination are in ~root/tests/resources. Then run the tests executable <br/>
  $ cd ~root/tests/resources <br/>
  $ cp cool-linux-x64-64 cool <br/>
  $ chmod a+x cool
  $ cd ../../build/tests
  $ ./tests

7. Since the CoolCompiler targets LLVM, we need to take the output file and run it through clang <br/>
  $ clang hello-world.ll <br/>
  $ ./a.out <br/>
  
  
