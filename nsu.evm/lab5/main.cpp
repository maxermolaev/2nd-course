#include <iostream>
#include <ctime>
#include <deque>
#include <numeric>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char* argv[]) {
    VideoCapture cam(0);
    if (!cam.isOpened()) {
        std::cerr << "Camera isn't open. \n";
        return -1;
    }
    int counter = 0;
    // Очередь для хранения времени обработки кадров
    const int fpsWindowSize = 15; // Размер скользящего окна
    std::deque<double> frameTimes; // Очередь с двумя концами для хранения времени обработки
    double FPS = 0.0; // Текущий FPS

    // Переменные для времени
    double total_time = 0, total_read_time = 0, total_process_time = 0, total_show_time = 0;

    while (true) {
        // Время начала обработки кадра
        std::clock_t start = std::clock();

        // Чтение кадра
        Mat image;
        if (!cam.read(image)) {
            std::cerr << "Camera is off. \n";
            return -1;
        }
        std::clock_t read = std::clock();

        // Обработка кадра
        for (int i = 0; i < image.rows; i++) {
            for (int j = 0; j < image.cols; j++) {
                int blue = image.at<Vec3b>(i, j)[0];
                if (blue < 100) {
                    image.at<Vec3b>(i, j)[0] = std::min(255, blue * 50);
                }
            }
        }
        flip(image, image, 1);
        std::clock_t process = std::clock();

        // Отображение FPS на изображении
        std::string fps_text = "FPS: " + std::to_string(static_cast<int>(FPS));
        putText(image, fps_text, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 255, 0), 2);

        // Отображение изображения
        imshow("Camera", image);
        char key = waitKey(33);
        if (key == 27) {
            break;
        }
        
        
        std::clock_t end = std::clock();
        
        // Рассчитываем FPS с использованием скользящего окна
        double frameTime = 1000.0 * (end - start) / CLOCKS_PER_SEC; // Время обработки текущего кадра (в мс)
        frameTimes.push_back(frameTime);
        if (frameTimes.size() > fpsWindowSize) {
            frameTimes.pop_front(); // Удаляем старые записи из очереди
        }

        // Рассчёт среднего времени кадра и FPS
        double averageFrameTime = std::accumulate(frameTimes.begin(), frameTimes.end(), 0.0) / frameTimes.size();
        FPS = 1000.0 / averageFrameTime; // FPS = 1000 / среднее время одного кадра
        counter++;


        // Подсчёт времени для каждого этапа
        total_time += end - start;
        total_read_time += read - start;
        total_process_time += process - read;
        total_show_time += end - process;
        
    }

    destroyAllWindows();

    // Вывод результатов
    std::cout << "FPS: " <<counter / (total_time / CLOCKS_PER_SEC) << "\n";
    std::cout << "read: " << total_read_time / total_time * 100 << "%\n";
    std::cout << "process: " << total_process_time / total_time * 100 << "%\n";
    std::cout << "show: " << total_show_time / total_time * 100 << "%\n";

    return 0;
}
