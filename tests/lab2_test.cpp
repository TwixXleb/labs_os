#include <gtest/gtest.h>
#include <cstdlib>
#include <ctime>

extern "C" {
#include <utils.h>

int countOfActiveThreads = 0;
int maxCountOfThreads = 0;
pthread_mutex_t mutex;
}

TEST(merge, test)
{
int input[] = {2, 4, 6, 8, 10, 1, 3, 5, 7, 9};
auto data = static_cast<int *>(malloc(sizeof(int) * 10));

for (int i = 0; i < 10; ++i) {
data[i] = input[i];
}

int expected[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

Merge(data, 0, 4, 9);

for (int i = 0; i < 10; ++i) {
ASSERT_EQ(data[i], expected[i]);
}

free(data);
}

TEST(consistentSort, test)
{
int input[] = {2, 1, 9, 7, 5, 6, 4, 10, 3, 8};
auto data = static_cast<int *>(malloc(sizeof(int) * 10));

for (int i = 0; i < 10; ++i) {
data[i] = input[i];
}

int expected[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

MergeData mergeData = {data, 0, 9};

MergeSort(&mergeData);

for (int i = 0; i < 10; ++i) {
ASSERT_EQ(data[i], expected[i]);
}

free(data);
}

TEST(speedAndParallelCorrectness, test)
{
if (pthread_mutex_init(&mutex, nullptr) != 0) {
perror("Could not initialize mutex!\n");
}

auto size = 1000000;
auto consistentData = static_cast<int *>(malloc(sizeof(int) * size));
auto parallelData = static_cast<int *>(malloc(sizeof(int) * size));

for (int i = 0; i < size; ++i) {
consistentData[i] = size - i;
parallelData[i] = size - i;
}

MergeData mergeConsistentData = {consistentData, 0, size - 1};
MergeData mergeParallelData = {parallelData, 0, size - 1};

clock_t startSimple = clock();

maxCountOfThreads = 0;
MergeSort(&mergeConsistentData);

clock_t endSimple = clock();

clock_t startParallel = clock();

maxCountOfThreads = 5;
MergeSort(&mergeParallelData);

clock_t endParallel = clock();

for (int i = 0; i < size; ++i) {
ASSERT_EQ(consistentData[i], parallelData[i]);
}

free(consistentData);
free(parallelData);
pthread_mutex_destroy(&mutex);

ASSERT_TRUE(endParallel - startParallel < endSimple - startSimple);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}