#include <cstdlib>
#include <cassert>

int main()
{
    assert(std::system("./dijkstra < test_neg.txt > result_neg.txt") != 0);
    assert(std::system("cmp result_neg.txt expected_neg.txt") == 0);
    std::system("rm result_neg.txt");

    assert(std::system("./dijkstra < test_few.txt > result_few.txt") != 0);
    assert(std::system("cmp result_few.txt expected_few.txt") == 0);
    std::system("rm result_few.txt");

    assert(std::system("./dijkstra < test_many.txt > result_many.txt") != 0);
    assert(std::system("cmp result_many.txt expected_many.txt") == 0);
    std::system("rm result_many.txt");

    assert(std::system("./dijkstra < test_nonZero.txt > result_nonZero.txt") != 0);
    assert(std::system("cmp result_nonZero.txt expected_nonZero.txt") == 0);
    std::system("rm result_nonZero.txt");

    assert(std::system("./dijkstra < test_working.txt > result_working.txt") == 0);
    assert(std::system("cmp result_working.txt expected_working.txt") == 0);
    std::system("rm result_working.txt");

    return 0;
}
