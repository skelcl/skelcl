all: build/libgtest.a

build:
	mkdir build
	( cd build; export PATH=${PATH}:/usr/local/bin ; cmake ../../../libraries/gtest -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CXX_FLAGS="-stdlib=libc++ -DGTEST_HAS_TR1_TUPLE=0" )

build/libgtest.a: build
	( cd build; make )


