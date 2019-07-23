cd test/cmake-build-debug/CMakeFiles/MsbClientC_Test.dir/src

cp ../../../../../cmake-build-debug/CMakeFiles/MsbClientC.dir/src/MsbClientC.c.* .
cp ../../../../../cmake-build-debug/CMakeFiles/MsbClientC.dir/src/MsbObjekt.c.* .
cp ../../../../../cmake-build-debug/CMakeFiles/MsbClientC.dir/src/rest/uri.c.* .
cp ../../../../../cmake-build-debug/CMakeFiles/MsbClientC.dir/src/websocket/websocket.c.* .

#gcov -b -l -p -c *.gcno

lcov -t "result" -o ex_test.info -c -d .

lcov -r ex_test.info '*sput.h*' -o ex_test2.info .
lcov -r ex_test2.info '/usr/include/*' -o ex_test3.info .s
lcov -r ex_test3.info '*test.c*' -o ex_test4.info .s

genhtml -o res ex_test4.info