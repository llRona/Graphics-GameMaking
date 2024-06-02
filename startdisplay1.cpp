#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

const int windowWidth = 600;
const int windowHeight = 800;
const int blockSize = 60;
const int initialBlockCount = 5;

// HSV에서 RGB로 변환하는 함수
sf::Color HSVtoRGB(float h, float s, float v) {
    int hi = static_cast<int>(std::floor(h / 60.0f)) % 6;
    float f = h / 60.0f - std::floor(h / 60.0f);
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (hi) {
    case 0: return sf::Color(v * 255, t * 255, p * 255);
    case 1: return sf::Color(q * 255, v * 255, p * 255);
    case 2: return sf::Color(p * 255, v * 255, t * 255);
    case 3: return sf::Color(p * 255, q * 255, v * 255);
    case 4: return sf::Color(t * 255, p * 255, v * 255);
    case 5: return sf::Color(v * 255, p * 255, q * 255);
    default: return sf::Color(255, 255, 255); // 이 경우는 없지만 기본값으로 설정
    }
}

// 색상을 가져오는 함수
sf::Color getRandomHSVColor() {
    static bool isHueInitialized = false; // h가 초기화되었는지 여부를 추적하는 변수
    static float h; // 초기 H 값을 저장하는 변수

    if (!isHueInitialized) {
        // h를 처음 한 번만 랜덤하게 초기화
        h = static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 360.0f);
        isHueInitialized = true;
    }

    h += 5.0f; // 예시로 5씩 더해줌
    if (h > 360.0f)
        h -= 360.0f; // 360도를 넘어가면 다시 0으로 돌아옴

    // 랜덤한 S(채도) 값 생성
    float min = 0.5f; // 최소 채도 값
    float max = 1.0f; // 최대 채도 값
    float s = min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min)); // min와 max 사이의 랜덤한 채도값 생성

    // 랜덤한 V(명도) 값 생성
    float v = min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min)); // min와 max 사이의 랜덤한 명도값 생성

    // HSV 값을 RGB로 변환하여 반환
    return HSVtoRGB(h, s, v);
}

// 텍스트 초기화 함수
sf::Text initializeText(const std::string& str, const sf::Font& font, int size, sf::Color color, float x, float y) {
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}

// HSV 색상 값을 조금씩 변경하는 함수
void changeHSV(float& hue, float& saturation, float& value, float& hueChange, float& saturationChange, float& valueChange) {
    hue += hueChange;
    if (hue >= 360.0f) {
        hue -= 360.0f; // 360도를 초과하면 다시 0도로 돌아감
    }
    saturation += saturationChange;
    if (saturation >= 1.0f || saturation <= 0.0f) {
        saturationChange *= -1; // 방향을 바꿈
    }
    value += valueChange;
    if (value >= 1.0f || value <= 0.1f) {
        valueChange *= -1; // 방향을 바꿈
    }
}

// 블록 스태킹 게임 함수
void runBlockStackingGame(sf::RenderWindow& window, float hueTop, float saturationTop, float valueTop,
    float hueBottom, float saturationBottom, float valueBottom, float hueSpeed, float saturationSpeed, float valueSpeed) {
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // 랜덤 시드 설정

    // 뷰 설정
    sf::View view = window.getDefaultView();
    float viewYOffset = -(windowHeight / 2 - (initialBlockCount / 2) * blockSize);

    view.setCenter(windowWidth / 2, windowHeight / 2 + viewYOffset);
    window.setView(view);

    sf::RectangleShape block(sf::Vector2f(blockSize, blockSize));
    block.setFillColor(getRandomHSVColor());
    block.setPosition((windowWidth - blockSize) / 2, 0); // 블록 초기 위치 설정

    sf::RectangleShape floor(sf::Vector2f(windowWidth, 5));
    floor.setFillColor(sf::Color::Black);
    floor.setPosition(0, windowHeight - 5);

    std::vector<sf::RectangleShape> blocks;

    // 초기 블록 5개 생성
    float initialYPosition = windowHeight / 2 - (initialBlockCount / 2) * blockSize;

    for (int i = 0; i < initialBlockCount; ++i) {
        sf::RectangleShape initialBlock(sf::Vector2f(blockSize, blockSize));
        initialBlock.setFillColor(getRandomHSVColor());
        initialBlock.setPosition((windowWidth - blockSize) / 2, initialYPosition + i * blockSize);
        blocks.push_back(initialBlock);
    }

    bool falling = false;
    float blockSpeed = 3.0f;
    float dropSpeed = 10.0f;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space)
                    falling = true;
            }
        }

        if (!falling) {
            // 블록 좌우 이동
            static bool movingRight = true;
            if (movingRight) {
                block.move(blockSpeed, 0);
                if (block.getPosition().x + blockSize >= windowWidth)
                    movingRight = false;
            }
            else {
                block.move(-blockSpeed, 0);
                if (block.getPosition().x <= 0)
                    movingRight = true;
            }
        }
        else {
            // 블록 떨어짐
            block.move(0, dropSpeed);

            // 블록 충돌 감지 및 쌓기
            for (const auto& placedBlock : blocks) {
                if (block.getGlobalBounds().intersects(placedBlock.getGlobalBounds())) {
                    block.setPosition(block.getPosition().x, placedBlock.getPosition().y - blockSize);
                    blocks.push_back(block);
                    block.setFillColor(getRandomHSVColor()); // 새 블록 색상 설정
                    block.setPosition((windowWidth - blockSize) / 2, viewYOffset + blockSize * 3.5);
                    falling = false;
                    viewYOffset -= blockSize; // 뷰 오프셋 조정
                    view.setCenter(windowWidth / 2, windowHeight / 2 + viewYOffset);
                    window.setView(view);
                    break;
                }
            }

            // 바닥에 닿으면 멈춤
            if (block.getPosition().y + blockSize >= windowHeight + viewYOffset - 5) {
                block.setPosition(block.getPosition().x, windowHeight + viewYOffset - blockSize - 5);
                blocks.push_back(block);
                block.setFillColor(getRandomHSVColor()); // 새 블록 색상 설정
                falling = false;
                viewYOffset -= blockSize; // 뷰 오프셋 조정
                view.setCenter(windowWidth / 2, windowHeight / 2 + viewYOffset);
                window.setView(view);
            }
        }

        // 매 프레임마다 HSV 값을 변경하고 그라데이션을 그림
        changeHSV(hueTop, saturationTop, valueTop, hueSpeed, saturationSpeed, valueSpeed); // HSV 색상 값을 변경
        changeHSV(hueBottom, saturationBottom, valueBottom, hueSpeed, saturationSpeed, valueSpeed); // HSV 색상 값을 변경

        window.clear();
        // 그라데이션 배경 그리기
        float gradientStartY = viewYOffset;
        for (int y = 0; y < windowHeight; ++y) {
            float t = static_cast<float>(y) / windowHeight; // 0.0 ~ 1.0 사이 값
            float hue = (1.0f - t) * hueTop + t * hueBottom;
            float saturation = (1.0f - t) * saturationTop + t * saturationBottom;
            float value = (1.0f - t) * valueTop + t * valueBottom;

            sf::Color bgColor = HSVtoRGB(hue, saturation, value);

            sf::RectangleShape rect(sf::Vector2f(windowWidth, 1));
            rect.setPosition(0, gradientStartY + y);
            rect.setFillColor(bgColor);
            window.draw(rect);
        }
        window.setView(view); // 뷰 적용
        for (const auto& placedBlock : blocks) {
            window.draw(placedBlock);
        }
        window.draw(block);
        window.draw(floor);
        window.display();
    }
}


int main() {
    // 윈도우 생성
    sf::RenderWindow window(sf::VideoMode(800, 600), "Stack Game");

    // 폰트 로드
    sf::Font font;
    if (!font.loadFromFile("28 Days Later.ttf")) {
        std::cerr << "ERROR" << std::endl;
        return -1;
    }

    // 텍스트 초기화
    sf::Text title = initializeText("Stack Game", font, 50, sf::Color::White, 250, 100);
    sf::Text startButton = initializeText("START", font, 30, sf::Color::White, 330, 300);
    sf::Text exitButton = initializeText("EXIT", font, 30, sf::Color::White, 350, 400);

    // 위쪽 색상 및 채도, 명도 설정
    float hueTop = 0.0f;
    float saturationTop = 0.01f;
    float valueTop = 0.9f;

    // 아래쪽 색상 및 채도, 명도 설정
    float hueBottom = 0.0f;
    float saturationBottom = 0.01f;
    float valueBottom = 0.1f;

    // 속도 설정
    float hueSpeed = 0.01f; // 색상 변화 속도
    float saturationSpeed = 0.0001f; // 채도 변화 속도
    float valueSpeed = 0.00005f;      // 명도 변화 속도

    // 메인 루프
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect startButtonRect = startButton.getGlobalBounds();

                    // "게임 시작" 버튼 클릭 확인
                    if (startButtonRect.contains(static_cast<sf::Vector2f>(mousePos))) {
                        std::cout << "게임 시작 버튼 클릭!" << std::endl;
                        window.setFramerateLimit(60); // 프레임 레이트 제한 설정
                        runBlockStackingGame(window, hueTop, saturationTop, valueTop, hueBottom, saturationBottom, valueBottom, hueSpeed, saturationSpeed, valueSpeed); // 블록 스태킹 게임으로 이동
                        return 0; // 게임 종료 시 프로그램 종료
                    }

                    // "종료" 버튼 클릭 확인
                    sf::FloatRect exitButtonRect = exitButton.getGlobalBounds();
                    if (exitButtonRect.contains(static_cast<sf::Vector2f>(mousePos))) {
                        std::cout << "종료 버튼 클릭!" << std::endl;
                        window.close();
                    }
                }
            }
        }

        // 화면 지우기
        window.clear();

        // 상단부터 하단까지 그라데이션 그리기
        for (int y = 0; y < window.getSize().y; ++y) {
            float t = static_cast<float>(y) / window.getSize().y; // 0.0 ~ 1.0 사이 값
            float hue = (1.0f - t) * hueTop + t * hueBottom;
            float saturation = (1.0f - t) * saturationTop + t * saturationBottom;
            float value = (1.0f - t) * valueTop + t * valueBottom;

            sf::Color bgColor = HSVtoRGB(hue, saturation, value);

            sf::RectangleShape rect(sf::Vector2f(window.getSize().x, 1));
            rect.setPosition(0, y);
            rect.setFillColor(bgColor);
            window.draw(rect);
        }

        // 텍스트 그리기
        window.draw(title);
        window.draw(startButton);
        window.draw(exitButton);

        // 윈도우에 그린 내용 표시
        window.display();

        // 매 프레임마다 HSV 값을 변경하고 그라데이션을 그림
        changeHSV(hueTop, saturationTop, valueTop, hueSpeed, saturationSpeed, valueSpeed); // HSV 색상 값을 변경
        changeHSV(hueBottom, saturationBottom, valueBottom, hueSpeed, saturationSpeed, valueSpeed); // HSV 색상 값을 변경
    }

    return 0;
}
