#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <SDL.h>
#include "Constants.h"
#include <vector>

class Obstacle {
public:
    Obstacle(int x, int y, int w, int h);

    void update(float deltaTime); // Di chuyển chướng ngại vật
    void render(SDL_Renderer* renderer, const std::vector<SDL_Texture*>& textures);

    SDL_Rect getRect() const;      // Lấy hình chữ nhật để kiểm tra va chạm
    float getSpeed() const;        // Lấy tốc độ
    bool isPassed() const;         // Kiểm tra đã vượt qua chưa

    void increaseSpeed();          // Tăng tốc độ (sau mỗi mốc điểm)
    void setPassed(bool val);      // Đánh dấu đã vượt qua
    void setTextureIndex(int index); // Đặt chỉ số texture
    void setX(float newX);         // Cập nhật lại vị trí X (dùng để reset vật cản)

private:
    SDL_Rect obstacleRect; // Vị trí và kích thước chướng ngại vật
    float xPos;            // Tọa độ X thực (dạng float để xử lý mượt)
    float speed;           // Tốc độ di chuyển
    bool passed;           // Đã vượt qua nhân vật chính chưa
    int textureIndex;      // Loại texture hiển thị
};

#endif // OBSTACLE_H
