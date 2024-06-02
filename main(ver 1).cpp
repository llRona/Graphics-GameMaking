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

// 무작위 색상을 생성하는 함수
sf::Color getRandomColor() {
    return sf::Color(rand() % 256, rand() % 256, rand() % 256);
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
    } else {
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

// 게임을 실행하는 함수
bool runGame(sf::RenderWindow& window, sf::Font& font) {
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
    scoreText.setFillColor(sf::Color::Black);
    updateScoreText(scoreText, score);

    // 뷰 설정
    sf::View view = window.getDefaultView();
    float viewYOffset = 0;
    bool movingRight = true; // 블록이 오른쪽으로 이동 중인지 여부

    while (window.isOpen())
    {
        // 이벤트 처리
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space && !falling) {
                    falling = true; // 스페이스바를 누르면 블록이 낙하 시작
                }
            }
        }

        // 블록 이동 또는 낙하 처리
        if (!falling) {
            handleBlockMovement(block, movingRight, blockSpeed);
        } else {
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
        window.clear(sf::Color::White);
        window.setView(view); // 뷰 적용
        for (const auto& placedBlock : blocks) {
            window.draw(placedBlock);
        }
        window.draw(block);
        window.draw(floor);
        window.draw(line); // 선 그리기
        window.draw(scoreText); // 점수 텍스트 그리기
        window.display();
    }

    // 게임 오버 메시지 표시
    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(24);
    gameOverText.setFillColor(sf::Color::Black);
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

        window.clear(sf::Color::White);
        window.draw(gameOverText);
        window.display();
    }
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // 랜덤 시드 설정

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Block Stacking Game");
    window.setFramerateLimit(60);

    // 폰트 로드
    sf::Font font;
    if (!font.loadFromFile("fonts/arial.ttf")) { // 폰트 파일 경로 설정
        return -1; // 폰트 로드 실패 시 종료
    }

    // 게임 루프
    while (window.isOpen()) {
        if (!runGame(window, font))
            break; // 사용자가 종료를 선택하면 루프 종료
    }

    return 0;
}
