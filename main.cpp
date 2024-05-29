#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

// 게임 설정 상수
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int BLOCK_SIZE = 30;
const float DROP_SPEED = 0.5f;

// 블록 클래스 정의
class Block {
public:
    // 블록 생성자: 위치와 속도를 설정
    Block(float x, float y, float speed)
        : speed(speed) {
        shape.setSize(sf::Vector2f(BLOCK_SIZE, BLOCK_SIZE));
        shape.setFillColor(sf::Color::Green);
        shape.setPosition(x, y);
    }

    // 블록 이동 함수
    void move(float offsetX, float offsetY) {
        shape.move(offsetX, offsetY);
    }

    // 블록의 모양 반환
    const sf::RectangleShape& getShape() const {
        return shape;
    }

    // 블록의 y 좌표 반환
    float getYPosition() const {
        return shape.getPosition().y;
    }

private:
    sf::RectangleShape shape; // 블록의 모양
    float speed; // 블록의 속도
};

// 블록을 초기화하는 함수
void initializeBlock(Block& block) {
    block = Block(WINDOW_WIDTH / 2, 0, DROP_SPEED);
}

// 현재 블록이 정착된 블록들과 충돌하는지 확인하는 함수
bool checkCollision(const Block& block, const std::vector<Block>& settledBlocks) {
    for (const auto& settledBlock : settledBlocks) {
        if (block.getShape().getGlobalBounds().intersects(settledBlock.getShape().getGlobalBounds())) {
            return true;
        }
    }
    return false;
}

int main() {
    // SFML 윈도우 생성
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "SFML Block Game");
    window.setFramerateLimit(60); // 프레임 제한

    std::srand(static_cast<unsigned int>(std::time(nullptr))); // 난수 시드 설정

    // 현재 블록 초기화
    Block currentBlock(WINDOW_WIDTH / 2, 0, DROP_SPEED);
    std::vector<Block> settledBlocks; // 정착된 블록들 저장

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close(); // 윈도우 닫기 이벤트 처리
        }

        // 좌우 이동 입력 처리
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) &&
            currentBlock.getShape().getPosition().x > 0) {
            currentBlock.move(-BLOCK_SIZE, 0); // 왼쪽으로 이동
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) &&
            currentBlock.getShape().getPosition().x < WINDOW_WIDTH - BLOCK_SIZE) {
            currentBlock.move(BLOCK_SIZE, 0); // 오른쪽으로 이동
        }

        // 블록 아래로 이동
        currentBlock.move(0, DROP_SPEED);

        bool blockSettled = false;

        // 블록이 바닥에 닿았는지 확인
        if (currentBlock.getYPosition() >= WINDOW_HEIGHT - BLOCK_SIZE) {
            blockSettled = true;
        }
        else {
            // 블록이 다른 블록과 충돌하는지 확인
            if (checkCollision(currentBlock, settledBlocks)) {
                blockSettled = true;
            }
        }

        // 블록이 정착되면 새로운 블록 생성
        if (blockSettled) {
            settledBlocks.push_back(currentBlock);
            initializeBlock(currentBlock);
        }

        // 윈도우 그리기
        window.clear();
        for (const auto& block : settledBlocks) {
            window.draw(block.getShape());
        }
        window.draw(currentBlock.getShape());
        window.display();
    }

    return 0;
}
