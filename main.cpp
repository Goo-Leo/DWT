#include <iostream>
#include <stdexcept>
#include <vector>
using namespace std;

template <typename T>
class dynamic_time_wrap {
   public:
    int frame_size;
    T* raw_array;
    T* collected_arrey;
    vector<T> aligned_data;

    dynamic_time_wrap(int frame_size, T* raw_data, T* collected_data);
    ~dynamic_time_wrap();

   private:
    T cost_matrix(T* position_r, T* position_c);
    void run_time();
};

template <typename T>
dynamic_time_wrap<T>::dynamic_time_wrap(int frame_size, T* raw_data, T* collected_data) {
    this->frame_size = frame_size;
    this->raw_array = raw_data;
    this->collected_arrey = collected_data;
    this->aligned_data.size = frame_size * sizeof(T);
}
template <typename T>
dynamic_time_wrap<T>::~dynamic_time_wrap() = default;
template <typename T>
inline T* createArrayFromPointer(T * data, size_t size) {
    T* newArray = new T[size];
    for (size_t i = 0; i < size; ++i) {
        newArray[i] = data[i];
    }
    return newArray;
}
template <typename T>
T dynamic_time_wrap<T>::cost_matrix(T* position_r, T* position_c) {

    T* r_buffer = createArrayFromPointer(position_r, this.frame_size);
    T* c_buffer = createArrayFromPointer(position_c, this.frame_size);
    // 距离矩阵,两序列对应点的欧氏距离矩阵
    vector<vector<T>> distance(this->frame_size, vector<T>(this->frame_size));
    for (int i = 0; i < this->frame_size; i++) {
        for (int j = 0; j < this->frame_size; j++) {
            distance[i][j] = r_buffer[i] - c_buffer[j];
        }
    }
    // 损失矩阵
    vector<vector<T>> cost(this->frame_size, vector<T>(this->frame_size));
    for (int i = 0; i < frame_size; i++) {
        cost[0][i] = distance[0][i];
    }
    for (int i = 0; i < frame_size; i++) {
        cost[i][0] = distance[i][0];
    }
    for (int i = 1; i < frame_size; i++) {
        for (int j = 1; j < frame_size; j++) {
            cost[i][j] =
                std::min({cost[i - 1][j - 1], cost[i - 1][j], cost[i][j - 1]}) + distance[i][j];
            // (cost[i - 1][j - 1] < cost[i - 1][j])
            //     ? ((cost[i - 1][j - 1] < cost[i][j - 1]) ? cost[i - 1][j - 1] : cost[i][j - 1])
            //     : ((cost[i - 1][j] < cost[i][j - 1]) ? cost[i - 1][j] : cost[i][j - 1]);
        }
    }

    return cost[frame_size - 1][frame_size - 1];
}

template <typename T>
void dynamic_time_wrap<T>::run_time() {
    int* alignment = new (sizeof(this->*raw_array) / this->frame_size);  // 创建一个对齐窗口
    for (int i = 0; i < sizeof(*alignment); i++) {                           //
        int offset = (int)this.cost_matrix();  // 计算当前窗口的偏移量
        for (int j = 0; j < this->frame_size - offset; j++) {
            this->aligned_data.push_back(this.);
        }
    }
}


int main(void) {
    int raw_data[50] = {3, 6, 6, 7, 8, 0, 0, 0, 0, 0, 3, 6, 6, 7, 8, 0, 0, 0, 0, 0, 3, 6, 6, 7, 8,
                        0, 0, 0, 0, 0, 3, 6, 6, 7, 8, 0, 0, 0, 0, 0, 3, 6, 6, 7, 8, 0, 0, 0, 0, 0};
    int test_data[50] = {0, 0, 3, 6, 6, 7, 8, 0, 0, 0, 0, 3, 6, 6, 7, 8, 0, 0, 0, 0, 0, 3, 6, 6, 7,
                         8, 0, 0, 0, 0, 0, 3, 6, 6, 7, 8, 0, 0, 0, 0, 0, 3, 6, 6, 7, 8, 0, 0, 0, 0};
    // 这里窗口的宽度需要根据实际时延的大小来调整
    dynamic_time_wrap<int> processing(10, raw_data, test_data);

    return 0;
}