# Linux Kernel Validation Suite

The Linux Kernel Validation Suite (LKVS) is a Linux Kernel test suite. It is a working project created by the Intel Core Linux Kernel Val Team.
The purpose is to improve the quality of Intel-contributed Linux Kernel code. Furthermore, it shows the customer how to use or validate the features.

More details please refer to following.

## Features
  * [cet(Control flow Enhancement Technology)](cet/README.md)
  * [cstate](cstate/README.md)
  * [Intel_TH(Trace Hub)](th/README.md)
  * [Intel_PT](pt/README.md)
  * [UMIP(User-Mode Instruction Prevention)](umip/README.md)
  * [xsave](xsave/README.md)
  * [IFS(In Field Scan)](ifs/README.md)
  * [FRED](fred/README.md) (in progress)
  * [guest-test](guest-test/README.md)
  * [IFS](ifs/README.md)
  * [ISST](isst/README.md)
  * [PMU](pmu/README.md)
  * [RAPL](rapl/README.md)
  * [SDSI](sdsi/README.md)
  * [splitlock](splitlock/README.md)
  * [tdx-compliance](tdx-compliance/README.md)
  * [thermal](thermal/README.md)
  * [topology](topology/README.md)
  * [ufs](ufs/README.md)
  * [workload-xsave](workload-xsave/README.md)
  * [AMX](state-components-validation-utilities/amx/README.md)
  * [DSA](dsa/README.md)

# Compile from sources
## Compile the whole project (NOT recommended)

Usually, it needs various dependencies if you're trying to compile the whole project locally.
```
git clone https://github.com/intel/lkvs.git
cd lkvs
make
```

Each sub-project should detail its particular way, if any. There are some known dependency issues.
### Known dependency issue locally
* Intel PT
Cases of Intel PT need a 3rd part library libipt.
```
export GIT_SSL_NO_VERIFY=true
git clone http://github.com/intel/libipt.git
cd libipt && cmake . && make install
```

* CET
CET tests have 32-bit compatible tests, which need install 32-bit architect support. But the 32-bit architect support is not a mandetory for CET tests.

#### Install 32-bit architect
##### Ubuntu OS
```
dpkg --add-architecture i386
dpkg --print-foreign-architectures
apt-get update
apt-get install gcc-11 make libelf1 gcc-multilib g++-multilib
```
##### Other OSs
Do not support 32-bit architect on other distributions.

#### Make the driver for cet_driver:
##### Ubuntu OS
For Ubuntu and so on deb package related OS:
```
dpkg -i linux-xxx-image_TARGET_VERSION.deb
dpkg -i linux-xxx-headers_TARGET_VERSION.deb
dpkg -i linux-xxx-dev_TARGET_VERSION.deb
dpkg -i linux-xxx-tools_TARGET_VERSION.deb

Boot up with target kernel version.
cd cet/cet_driver
make
```

##### CentOS/OpenEuler/Anolis
For CentOS and so on rpm package related OS:
Install the devel and headers package for target kernel and boot up
with target kernel.
```
rpm -ivh --force kernel-TARGET_VERSION.rpm
rpm -ivh --force kernel-devel-TARGET_VERSION.rpm
rpm -ivh --force kernel-headers-TARGET_VERSION.rpm

Boot up with target kernel version.
cd cet_driver
make
```

## Alternativly compile the whole project with Docker
```
make docker_image
make docker_make
```
Note. If you're behind a proxy, please export the local env variable `https_proxy` before executing `make`
```
export https_proxy=https://proxy-domain:port
```
## Compile a single test case
** This is the recommended way **

```
cd lkvs/<test>
make
```

# Run tests

There're two ways to run the tests.

## Binary/Shell directly

Normally, there're one or more executable binaries or scirpts in a sub-project, once it is compiled successfully. The easiest way is to run them directly.

## Case runner

**runtests** is a straightforward test runner that can execute a single or a set of test cases and redirect the log.

There are 2 ways to pass test to **runtests**:
  1. `-c`: Pass test cmdline.
  2. `-f <component/tests*>`: the `tests*` file under each component folder records the detailed test cmd lines.

Output of tests can be saved in a file using `-o` option.

Examples:

```
$ ./runtests -f <cmdfile>
$ ./runtests -f <cmdfile> -o <logfile>
$ ./runtests -c <cmdline>
$ ./runtests -c <cmdline> -o <logfile>
```

# Report a problem

Submit an [issue](https://github.com/intel/lkvs/issues) or initiate a discussion.

# Contribute rule

## Coding style
Any pull request are welcomed, the canonical patch format please refer to the Kernel [patches submitting](https://www.kernel.org/doc/html/latest/process/submitting-patches.html)

## Naming Rules
### Feature Names
The folders located in the root directory are referred to as "features." When naming a feature, please follow the following rule:
* Use a unified format for the feature name: <lowercase>-<info>
e.g. :cet, cstate, tdx-compliance, tdx-osv-sanity.

### Tests File:
* Use the file name 'tests' for default `tests` if only one tests is needed.
* Use the filename 'guest-tests' for guest tests.

### Entry Point of Guest Tests
Use feature.guest_test_executor.sh as the entry point for guest tests.


# License
See [LICENSE](https://github.com/intel/lkvs/blob/main/LICENSE) for details.
