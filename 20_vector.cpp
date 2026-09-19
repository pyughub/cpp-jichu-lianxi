#include <iostream>
#include <vector>
#include <numeric>
int main() {
    std::vector<int>scores = {85, 90, 78, 92};
    scores.push_back(88);
    std::cout << "所有成绩: ";
    for (int score : scores) {
        std::cout << score << " ";
    }
    std::cout << std::endl;
    int total = std::accumulate(scores.begin(), scores.end(), 0);
    double average = total * 1.0 / scores.size();
    std::cout << "平均成绩: " << average << std::endl;
    scores.erase(scores.begin() + 2);
    std::cout << "删除后成绩: ";
    for (int score : scores) {
        std::cout << score << " ";
    }
    std::cout << std::endl;
    return 0;
}
