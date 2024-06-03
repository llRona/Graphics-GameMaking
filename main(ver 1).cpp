#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

const int windowWidth = 600;
const int windowHeight = 800;
const int blockSize = 60;
const int initialBlockCount = 5;
const float initialLineY = 700; // 초기 수평선의 y 위치

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

// 무작위 색상을 생성하는 함수
sf::Color getRandomColor() {
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
    float min = 0.75f; // 최소 값
    float max = 1.0f; // 최대 값
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

// 그라데이션 배경을 그리는 함수
void drawGradient(sf::RenderWindow& window, float& hueTop, float& saturationTop, float& valueTop,
    float& hueBottom, float& saturationBottom, float& valueBottom, float viewYOffset) {
    for (int y = 0; y < window.getSize().y; ++y) {
        float t = static_cast<float>(y) / window.getSize().y; // 0.0 ~ 1.0 사이 값
        float hue = (1.0f - t) * hueTop + t * hueBottom;
        float saturation = (1.0f - t) * saturationTop + t * saturationBottom;
        float value = (1.0f - t) * valueTop + t * valueBottom;

        sf::Color bgColor = HSVtoRGB(hue, saturation, value);

        sf::RectangleShape rect(sf::Vector2f(window.getSize().x, 1));
        rect.setPosition(0, viewYOffset + y);
        rect.setFillColor(bgColor);
        window.draw(rect); float viewYOffset;
    }
}


// 초기 블록을 설정하는 함수
void initializeBlocks(std::vector<sf::RectangleShape>& blocks) {
    for (int i = 0; i < initialBlockCount; ++i) {
        sf::RectangleShape initialBlock(sf::Vector2f(blockSize, blockSize));
        initialBlock.setFillColor(getRandomColor());
        initialBlock.setPosition((windowWidth - blockSize) / 2, windowHeight - (i + 1) * blockSize - 5);
        blocks.push_back(initialBlock);
    }
}

// 점수 텍스트를 업데이트하는 함수
void updateScoreText(sf::Text& scoreText, int score) {
    std::ostringstream ss;
    ss << "Score: " << score;
    scoreText.setString(ss.str());
}

// 블록의 좌우 이동을 처리하는 함수
void handleBlockMovement(sf::RectangleShape& block, bool& movingRight, float blockSpeed) {
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

// 블록의 낙하 및 충돌을 처리하는 함수
bool handleBlockFalling(sf::RectangleShape& block, std::vector<sf::RectangleShape>& blocks, sf::View& view, float dropSpeed, int& score, float& viewYOffset, sf::RectangleShape& line) {
    block.move(0, dropSpeed);

    // 다른 블록과 충돌 시 처리
    for (const auto& placedBlock : blocks) {
        if (block.getGlobalBounds().intersects(placedBlock.getGlobalBounds())) {
            block.setPosition(block.getPosition().x, placedBlock.getPosition().y - blockSize);
            blocks.push_back(block);
            block.setFillColor(getRandomColor());
            block.setPosition((windowWidth - blockSize) / 2, viewYOffset);
            viewYOffset -= blockSize;
            view.setCenter(windowWidth / 2, windowHeight / 2 + viewYOffset);
            score++;
            line.move(0, -blockSize); // 선의 위치도 블록 크기만큼 이동
            return false; // 낙하 종료
        }
    }

    // 바닥과 충돌 시 처리
    if (block.getPosition().y + blockSize >= windowHeight + viewYOffset - 5) {
        block.setPosition(block.getPosition().x, windowHeight + viewYOffset - blockSize - 5);
        blocks.push_back(block);
        block.setFillColor(getRandomColor());
        block.setPosition((windowWidth - blockSize) / 2, viewYOffset);
        viewYOffset -= blockSize;
        view.setCenter(windowWidth / 2, windowHeight / 2 + viewYOffset);
        score++;
        line.move(0, -blockSize); // 선의 위치도 블록 크기만큼 이동
        return false; // 낙하 종료
    }

    return true; // 계속 낙하
}

    // 시작화면 함수
bool startScreen(sf::RenderWindow& window, sf::Font& font, float& hueTop, float& saturationTop, float& valueTop,
    float& hueBottom, float& saturationBottom, float& valueBottom, float hueSpeed, float saturationSpeed, float valueSpeed, float& viewYOffset) {
    // 텍스트 초기화
    sf::Text title = initializeText("Stack Game", font, 50, sf::Color::White, windowWidth / 2, windowHeight / 4 + viewYOffset);
    sf::Text startButton = initializeText("START", font, 30, sf::Color::White, windowWidth / 2, windowHeight / 2 + viewYOffset);
    sf::Text exitButton = initializeText("EXIT", font, 30, sf::Color::White, windowWidth / 2, windowHeight / 2 + 100 + viewYOffset);

    // 텍스트의 원점을 중앙으로 설정하여 중앙 정렬을 쉽게 만듦
    title.setOrigin(title.getLocalBounds().width / 2, title.getLocalBounds().height / 2);
    startButton.setOrigin(startButton.getLocalBounds().width / 2, startButton.getLocalBounds().height / 2);
    exitButton.setOrigin(exitButton.getLocalBounds().width / 2, exitButton.getLocalBounds().height / 2);

    // 버튼의 초기 위치 설정
    float buttonYOffset = viewYOffset; // 버튼의 Y 위치를 뷰의 Y 오프셋과 동일하게 설정
    startButton.setPosition(windowWidth / 2, windowHeight / 2 + buttonYOffset);
    exitButton.setPosition(windowWidth / 2, windowHeight / 2 + 100 + buttonYOffset);

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
                        return true; // 게임 시작 신호 반환
                    }

                    // "종료" 버튼 클릭 확인
                    sf::FloatRect exitButtonRect = exitButton.getGlobalBounds();
                    if (exitButtonRect.contains(static_cast<sf::Vector2f>(mousePos))) {
                        window.close();
                        return false; // 종료 신호 반환
                    }
                }
            }
        }
        // 화면 지우기
        window.clear();

        // 그라데이션 배경 그리기
        drawGradient(window, hueTop, saturationTop, valueTop, hueBottom, saturationBottom, valueBottom, viewYOffset);

        // 텍스트 그리기
        window.draw(title);
        window.draw(startButton);
        window.draw(exitButton);

        // 윈도우에 그린 내용 표시
        window.display();

        // 매 프레임마다 HSV 값을 변경하고 그라데이션을 그림
        changeHSV(hueTop, saturationTop, valueTop, hueSpeed, saturationSpeed, valueSpeed); // HSV 색상 값을 변경
        changeHSV(hueBottom, saturationBottom, valueBottom, hueSpeed, saturationSpeed, valueSpeed);

        // 텍스트의 위치가 변경될 때마다 버튼도 같이 이동
        title.setPosition(windowWidth / 2, windowHeight / 4 + viewYOffset);
        startButton.setPosition(windowWidth / 2, windowHeight / 2 + buttonYOffset);
        exitButton.setPosition(windowWidth / 2, windowHeight / 2 + 100 + buttonYOffset);
    }

    return false; // 윈도우가 닫힌 경우 false 반환
}




bool runGame(sf::RenderWindow& window, sf::Font& font, float& hueTop, float& saturationTop, float& valueTop,
    float& hueBottom, float& saturationBottom, float& valueBottom, float hueSpeed, float saturationSpeed, float valueSpeed, float& viewYOffset) {

    // 초기 블록 설정
    sf::RectangleShape block(sf::Vector2f(blockSize, blockSize));
    block.setFillColor(getRandomColor());
    block.setPosition((windowWidth - blockSize) / 2, 0);

    // 바닥 설정
    sf::RectangleShape floor(sf::Vector2f(windowWidth, 5));
    floor.setFillColor(sf::Color::Black);
    floor.setPosition(0, windowHeight - 5);

    // 블록 벡터 초기화
    std::vector<sf::RectangleShape> blocks;
    initializeBlocks(blocks);

    // 수평선 설정
    sf::RectangleShape line(sf::Vector2f(windowWidth, 2));
    line.setFillColor(sf::Color::Red);
    line.setPosition(0, initialLineY);

    bool falling = false; // 블록이 낙하 중인지 여부
    float blockSpeed = 3.0f; // 블록의 좌우 이동 속도
    float dropSpeed = 10.0f; // 블록의 낙하 속도
    int score = 0; // 점수 변수

    // 점수 표시를 위한 텍스트 설정
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    updateScoreText(scoreText, score);

    // 뷰 설정
    sf::View view = window.getDefaultView();
    viewYOffset = 0;
    bool movingRight = true; // 블록이 오른쪽으로 이동 중인지 여부

    while (window.isOpen()) {
        // 이벤트 처리
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space && !falling) {
                    falling = true; // 스페이스바를 누르면 블록이 낙하 시작
                }
            }
        }

        // 블록 이동 또는 낙하 처리
        if (!falling) {
            handleBlockMovement(block, movingRight, blockSpeed);
        }
        else {
            falling = handleBlockFalling(block, blocks, view, dropSpeed, score, viewYOffset, line);
        }

        // 게임 오버 조건 확인
        if (falling && block.getPosition().y + blockSize > line.getPosition().y) {
            break; // 게임 오버
        }

        // 점수 텍스트 업데이트
        updateScoreText(scoreText, score);

        // 점수 텍스트 위치 업데이트
        sf::Vector2f viewCenter = view.getCenter();
        scoreText.setPosition(viewCenter.x + windowWidth / 2 - 150, viewCenter.y + windowHeight / 2 - 50);

        // 화면 그리기
        window.clear();

        drawGradient(window, hueTop, saturationTop, valueTop, hueBottom, saturationBottom, valueBottom, viewYOffset);

        window.setView(view); // 뷰 적용
        for (const auto& placedBlock : blocks) {
            window.draw(placedBlock);
        }
        window.draw(block);
        window.draw(floor);
        window.draw(line); // 선 그리기
        window.draw(scoreText); // 점수 텍스트 그리기
        window.display();

        // 매 프레임마다 HSV 값을 변경하고 그라데이션을 그림
        changeHSV(hueTop, saturationTop, valueTop, hueSpeed, saturationSpeed, valueSpeed); // HSV 색상 값을 변경
        changeHSV(hueBottom, saturationBottom, valueBottom, hueSpeed, saturationSpeed, valueSpeed); // HSV 색상 값을 변경
    }

    // 게임 오버 메시지 표시
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(24);
    gameOverText.setFillColor(sf::Color::White);
    gameOverText.setString("                 Game Over! \nPress R to Restart or Q to Quit");

    // 게임 오버 텍스트 위치 업데이트
    sf::Vector2f viewCenter = view.getCenter();
    gameOverText.setPosition(viewCenter.x - 150, viewCenter.y);

    while (true) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::R)
                    return true; // 재시작
                if (event.key.code == sf::Keyboard::Q)
                    return false; // 종료
            }
        }

        window.clear();
        drawGradient(window, hueTop, saturationTop, valueTop, hueBottom, saturationBottom, valueBottom, viewYOffset);
        // 매 프레임마다 HSV 값을 변경하고 그라데이션을 그림
        window.draw(gameOverText);
        window.display();

        // 매 프레임마다 HSV 값을 변경하고 그라데이션을 그림
        changeHSV(hueTop, saturationTop, valueTop, hueSpeed, saturationSpeed, valueSpeed); // HSV 색상 값을 변경
        changeHSV(hueBottom, saturationBottom, valueBottom, hueSpeed, saturationSpeed, valueSpeed); // HSV 색상 값을 변경
    }
}


int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // 랜덤 시드 설정

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Block Stacking Game");
    window.setFramerateLimit(60);

    // 폰트 로드
    sf::Font font;
    if (!font.loadFromFile("28 Days Later.ttf")) { // 폰트 파일 경로 설정
        return -1; // 폰트 로드 실패 시 종료
    }
    float viewYOffset = 0;
    // 위쪽 색상 및 채도, 명도 설정
    float hueTop = 0.0f;
    float saturationTop = 0.01f;
    float valueTop = 0.9f;

    // 아래쪽 색상 및 채도, 명도 설정
    float hueBottom = 0.0f;
    float saturationBottom = 0.01f;
    float valueBottom = 0.1f;

    // 속도 설정
    float hueSpeed = 0.1f; // 색상 변화 속도
    float saturationSpeed = 0.001f; // 채도 변화 속도
    float valueSpeed = 0.0005f;      // 명도 변화 속도

    while (window.isOpen()) {
        if (startScreen(window, font, hueTop, saturationTop, valueTop, hueBottom, saturationBottom, valueBottom, hueSpeed, saturationSpeed, valueSpeed, viewYOffset)) {
            window.setFramerateLimit(60); // 프레임 레이트 제한 설정
            runGame(window, font, hueTop, saturationTop, valueTop, hueBottom, saturationBottom, valueBottom, hueSpeed, saturationSpeed, valueSpeed, viewYOffset);
        }
        else {
            break; // 종료 신호 반환 시 루프 종료
        }
    }

    return 0;
}
