cd test/cmake-build-debug/CMakeFiles/MsbClientC_Test.dir/src

cp ../../../../../cmake-build-debug/CMakeFiles/MsbClientC.dir/src/MsbClientC.c.* .
cp ../../../../../cmake-build-debug/CMakeFiles/MsbClientC.dir/src/MsbObject.c.* .
cp ../../../../../cmake-build-debug/CMakeFiles/MsbClientC.dir/src/rest/uri.c.* .
cp ../../../../../cmake-build-debug/CMakeFiles/MsbClientC.dir/src/websocket/websocket.c.* .

#gcov -b -l -p -c *.gcno

lcov -t "result" -o ex_test.info -c -d .

lcov -r ex_test.info '/usr/include/*' '*test/*' -o ex_test2.info .

genhtml -o res ex_test2.info