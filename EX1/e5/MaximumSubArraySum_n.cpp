#include <iostream>
#include <vector>

void buildRandVector(std::vector<int>& arr, int seed, size_t length)
{
    srand(seed);
    for (size_t i = 0; i < length; i++)
    {   
        // range in [-74 , 25]
        int r = (rand() % (100)) - 74;
        arr.push_back(r);
    }
}

int maxSubArraySum_n(const std::vector<int>& arr) {
    if (arr.empty())
        return 0;

    int current_sum = arr[0];
    int max_sum = arr[0];

    for (size_t i = 1; i < arr.size(); i++) {
        current_sum = std::max(arr[i], current_sum + arr[i]);
        max_sum = std::max(max_sum, current_sum);
    }

    return max_sum;
}

int main(int argc, char *argv[])
{
    int seed = atoi(argv[1]);
    size_t length = atoi(argv[2]);
    std::vector<int> arr;
    buildRandVector(arr, seed, length);
    
    int result = maxSubArraySum_n(arr);

    std::cout << "Maximum Subarray Sum n  = " << result << std::endl;
    return 0;
}