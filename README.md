# C++ Sqlite Wrapper
make access to sqlite easier.

## 1 Dependencies
Project dependencies which are used in the whole development process.
The install guides described are for Linux users. Windows users, please
refer to MinGW to execute the commands below.

if all tests pass with `make test` everything is installed correctly.

* make
  * install: `sudo apt-get install make`
* g++
  * install: `sudo apt-get install g++`
* sqlite3
  * download from: https://www.sqlite.org/download.html
    * I downloaded sqlite-autoconf-3310100.tar.gz.
  * extract
  * install instructions
    * `cd sqlite-autoconf-3310100`
    * `./configure --prefix=/usr/local`
    * `make`
    * `make install`
* gtest
  * download: `wget https://github.com/google/googletest/archive/release-1.10.0.tar.gz`
  * extract
  * install instructions (Ubuntu) @see https://stackoverflow.com/questions/13513905/how-to-set-up-googletest-as-a-shared-library-on-linux
    * `sudo apt-get install libgtest-dev`
    * `cd googletest-release-1.10.0`
    * `cmake -DBUILD_SHARED_LIBS=ON .`
      *  don't forget the dot at the end
      *  install cmake before: `sudo apt-get install cmake`
    * `make`
    * `cd googletest/`
    * `sudo cp -a include/gtest /usr/include`
      *  copy include files to system
    * `cd ../lib/`
    * `sudo cp -a libgtest_main.so libgtest.so /usr/lib/`
      *  copy lib files to system
    * `sudo ldconfig -v | grep gtest`
      *  check if linker is aware of gtest libs. there should be the following at the end<br />
        libgtest.so -> libgtest.so<br />
	      libgtest_main.so -> libgtest_main.so
* lcov
  * install: `sudo apt-get install lcov`

## 2 Build
* `make`

## 3 Run Application
* `make run`
  * build the application before

## 4 Tests
* Run the tests with `make test`.
* To get a code coverage report run `make lcov`.