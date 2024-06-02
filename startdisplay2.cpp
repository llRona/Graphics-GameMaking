#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>

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

int main() {
    // 윈도우 생성
    sf::RenderWindow window(sf::VideoMode(800, 600), "Game Start Screen");

    // 폰트 로드
    sf::Font font;
    if (!font.loadFromFile("28 Days Later.ttf")) {
        std::cerr << "Error: Could not load font 'Roboto-Regular.ttf'" << std::endl;
        return -1;
    }

    // 게임 제목 텍스트 설정
    sf::Text title("My Game", font, 50);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setPosition(250, 100);

    // "Start Game" 버튼 텍스트 설정
    sf::Text startButton("Start Game", font, 30);
    startButton.setFillColor(sf::Color::White);
    startButton.setPosition(330, 300);

    // "EXIT" 메뉴 텍스트 설정
    sf::Text exitButton("EXIT", font, 30);
    exitButton.setFillColor(sf::Color::White);
    exitButton.setPosition(350, 400);

    // 위쪽 색상 및 채도, 명도 설정
    float hueTop = 0.0f;
    float saturationTop = 0.7f;
    float valueTop = 0.8f;

    // 아래쪽 색상 및 채도, 명도 설정
    float hueBottom = 0.0f;
    float saturationBottom = 0.3f;
    float valueBottom = 0.4f;

    // 속도 설정
    float hueSpeed = 0.01f; // 색상 변화 속도
    float saturationSpeed = 0.0002f; // 채도 변화 속도
    float valueSpeed = 0.0001f;      // 명도 변화 속도

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

                    // "Start Game" 버튼 클릭 확인
                    if (startButtonRect.contains(static_cast<sf::Vector2f>(mousePos))) {
                        std::cout << "Start Game button clicked!" << std::endl;
                        window.close();
                    }

                    // "EXIT" 버튼 클릭 확인
                    sf::FloatRect exitButtonRect = exitButton.getGlobalBounds();
                    if (exitButtonRect.contains(static_cast<sf::Vector2f>(mousePos))) {
                        std::cout << "Exit button clicked!" << std::endl;
                        window.close();
                    }
                }
            }
        }

        // 화면 지우기
        window.clear();

        // 상단부터 하단까지 그라데이션 그리기
        for (int y = 0; y < window.getSize().y; ++y) {
            // 현재 y 위치에 해당하는 색상 계산
            float t = static_cast<float>(y) / window.getSize().y; // 0.0 ~ 1.0 사이 값
            float hue = (1.0f - t) * hueTop + t * hueBottom;
            float saturation = (1.0f - t) * saturationTop + t * saturationBottom;
            float value = (1.0f - t) * valueTop + t * valueBottom;

            // 현재 y 위치에 대한 배경 색상 계산
            sf::Color bgColor = HSVtoRGB(hue, saturation, value);

            // 현재 y 위치에 사각형 그리기
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

        hueTop += hueSpeed; // 다음 색상으로 이동
        if (hueTop >= 360.0f) {
            hueTop -= 360.0f; // 360도를 초과하면 다시 0도로 돌아감
        }
        hueBottom += hueSpeed; // 다음 색상으로 이동
        if (hueBottom >= 360.0f) {
            hueBottom -= 360.0f; // 360도를 초과하면 다시 0도로 돌아감
        }
        // 명도와 채도 업데이트
        saturationTop += saturationSpeed;
        if (saturationTop >= 1.0f || saturationTop <= 0.0f) {
            saturationSpeed *= -1; // 방향을 바꿈
        }

        valueTop += valueSpeed;
        if (valueTop >= 1.0f || valueTop <= 0.5f) {
            valueSpeed *= -1; // 방향을 바꿈
        }

        saturationBottom += saturationSpeed;
        if (saturationBottom >= 1.0f || saturationBottom <= 0.0f) {
            saturationSpeed *= -1; // 방향을 바꿈
        }

        valueBottom += valueSpeed;
        if (valueBottom >= 1.0f || valueBottom <= 0.0f) {
            valueSpeed *= -1; // 방향을 바꿈
        }
    }

    return 0;
}
