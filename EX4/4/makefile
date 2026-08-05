all: code_coverage run_code_coveage performance_profile run_performance_profile memcheck_valgrind run_memcheck_valgrind callgrind_kCachegrind_Valgrind run_callgrind_kCachegrind_Valgrind


code_coverage:
	g++ -fprofile-arcs -ftest-coverage -O0 *.cpp
run_code_coveage:
	./a.out -s 19 -v 5 -e 10
	lcov --capture --directory . --output-file coverage.info 
	genhtml coverage.info --output-directory out_coverage

performance_profile:
	g++ -pg -O1 *.cpp
run_performance_profile:
	./a.out -s 19 -v 5 -e 10
	gprof ./a.out gmon.out > gprof_report.txt

memcheck_valgrind:
	g++ -g -O0 *.cpp
run_memcheck_valgrind:
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=valgrind_memcheck.txt ./a.out -s 19 -v 5 -e 10

callgrind_kCachegrind_Valgrind:
	g++ -g -O0 *.cpp
run_callgrind_kCachegrind_Valgrind:
	valgrind --tool=callgrind ./a.out -s 19 -v 5 -e 10
	kcachegrind callgrind.out.*

clean:
	rm *.out *.gcda *.gcno *.info -r out_coverage *.txt *.out.*