#include <cmath>
#include <iostream>
#include <valarray>
#include <vector>
using namespace std;

template<typename T>
class dynamic_time_wrap {
public:
    int frame_size;
    T *raw_array;
    T *collected_array;
    vector<T> aligned_data;

    dynamic_time_wrap(int frame_size, T *raw_data, T *collected_data);

    ~dynamic_time_wrap();

    void run_time();

private:
    void alignWindow(T *position_r, T *position_c);

    vector<std::pair<T, T> > find_dtw_path(const std::vector<std::vector<T> > &cost);
};

template<typename T>
dynamic_time_wrap<T>::dynamic_time_wrap(int frame_size, T *raw_data, T *collected_data) {
    this->frame_size = frame_size;
    this->raw_array = raw_data;
    this->collected_array = collected_data;
}

template<typename T>
dynamic_time_wrap<T>::~dynamic_time_wrap() = default;

template<typename T>
std::vector<std::pair<T, T> > dynamic_time_wrap<T>::find_dtw_path(const std::vector<std::vector<T> > &cost) {
    int i = cost.size() - 1;
    int j = cost[0].size() - 1;
    std::vector<std::pair<T, T> > path;

    // 回溯得到最优路径
    while (i > 0 || j > 0) {
        path.emplace_back(i, j);
        if (i == 0) {
            --j;
        } else if (j == 0) {
            --i;
        } else {
            if (cost[i - 1][j - 1] <= cost[i - 1][j] && cost[i - 1][j - 1] <= cost[i][j - 1]) {
                --i;
                --j;
            } else if (cost[i - 1][j] < cost[i][j - 1]) {
                --i;
            } else {
                --j;
            }
        }
    }
    path.emplace_back(0, 0);
    std::reverse(path.begin(), path.end()); //反转路径
    return path;
}

template<typename T>
inline T *createArrayFromPointer(T *data, size_t size) {
    T *newArray = new T[size];
    for (size_t i = 0; i < size; ++i) {
        newArray[i] = data[i];
    }
    return newArray;
}

template<typename T>
void dynamic_time_wrap<T>::alignWindow(T *position_r, T *position_c) {

    std::vector<T> r_buffer(position_r, position_r + frame_size);
    std::vector<T> c_buffer(position_c, position_c + frame_size);
    // 距离矩阵,两序列对应点的欧氏距离矩阵
    vector<vector<T> > distance(frame_size, vector<T>(frame_size));
    for (int i = 0; i < frame_size; i++) {
        for (int j = 0; j < frame_size; j++) {
            distance[i][j] = std::pow(r_buffer[i] - c_buffer[j], 2);
        }
    }
    // 损失矩阵
    vector<vector<T> > cost(frame_size, vector<T>(frame_size));
    cost[0][0] = distance[0][0];
    for (int i = 1; i < frame_size; ++i) {
        cost[i][0] = cost[i - 1][0] + distance[i][0];
        cost[0][i] = cost[0][i - 1] + distance[0][i];
    }

    for (int i = 1; i < frame_size; i++) {
        for (int j = 1; j < frame_size; j++) {
            cost[i][j] = std::min({cost[i - 1][j - 1], cost[i - 1][j], cost[i][j - 1]}) + distance[i][j];
            // (cost[i - 1][j - 1] < cost[i - 1][j])
            //     ? ((cost[i - 1][j - 1] < cost[i][j - 1]) ? cost[i - 1][j - 1] : cost[i][j - 1])
            //     : ((cost[i - 1][j] < cost[i][j - 1]) ? cost[i - 1][j] : cost[i][j - 1]) + distance[i][j];
        }
    }
    auto path = this->find_dtw_path(cost);

    std::vector<T> aligned_window(frame_size);
    for (const auto &[i, j]: path) {
        aligned_window[i] = c_buffer[j];
    }
    aligned_data.insert(aligned_data.end(), aligned_window.begin(), aligned_window.end());
}

template<typename T>
void dynamic_time_wrap<T>::run_time() {
    for (int i = 0; i < 50 / frame_size; i++) {
        alignWindow(raw_array, collected_array);
        raw_array += frame_size;
        collected_array += frame_size;
    }
}


int main() {
    int raw_data[50] = {
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3
    };
    int test_data[50] = {
        0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
        3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3
    };
    // 这里窗口的宽度需要根据实际时延的大小来调整
    dynamic_time_wrap<int> processing(7, raw_data, test_data);
    processing.run_time();
    for (int i: processing.aligned_data) {
        std::cout << i << ',';
    }
    return 0;
}
