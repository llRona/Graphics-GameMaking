#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

const int windowWidth = 600;
const int windowHeight = 800;
const int blockSize = 60;
const int initialBlockCount = 5;

sf::Color getRandomColor() {
    return sf::Color(rand() % 256, rand() % 256, rand() % 256);
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // 랜덤 시드 설정

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Block Stacking Game");
    window.setFramerateLimit(60);

    sf::RectangleShape block(sf::Vector2f(blockSize, blockSize));
    block.setFillColor(getRandomColor());
    block.setPosition((windowWidth - blockSize) / 2, 0);

    sf::RectangleShape floor(sf::Vector2f(windowWidth, 5));
    floor.setFillColor(sf::Color::Black);
    floor.setPosition(0, windowHeight - 5);

    std::vector<sf::RectangleShape> blocks;

    // 초기 블록 5개 생성
    for (int i = 0; i < initialBlockCount; ++i) {
        sf::RectangleShape initialBlock(sf::Vector2f(blockSize, blockSize));
        initialBlock.setFillColor(getRandomColor());
        initialBlock.setPosition((windowWidth - blockSize) / 2, windowHeight - (i + 1) * blockSize - 5);
        blocks.push_back(initialBlock);
    }

    bool falling = false;
    float blockSpeed = 3.0f;
    float dropSpeed = 10.0f;

    // 뷰 설정
    sf::View view = window.getDefaultView();
    float viewYOffset = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Space)
                    falling = true;
            }
        }

        if (!falling)
        {
            // 블록 좌우 이동
            static bool movingRight = true;
            if (movingRight)
            {
                block.move(blockSpeed, 0);
                if (block.getPosition().x + blockSize >= windowWidth)
                    movingRight = false;
            }
            else
            {
                block.move(-blockSpeed, 0);
                if (block.getPosition().x <= 0)
                    movingRight = true;
            }
        }
        else
        {
            // 블록 떨어짐
            block.move(0, dropSpeed);

            // 블록 충돌 감지 및 쌓기
            for (const auto& placedBlock : blocks)
            {
                if (block.getGlobalBounds().intersects(placedBlock.getGlobalBounds()))
                {
                    block.setPosition(block.getPosition().x, placedBlock.getPosition().y - blockSize);
                    blocks.push_back(block);
                    block.setFillColor(getRandomColor()); // 새 블록 색상 설정
                    block.setPosition((windowWidth - blockSize) / 2, viewYOffset);
                    falling = false;
                    viewYOffset -= blockSize; // 뷰 오프셋 조정
                    view.setCenter(windowWidth / 2, windowHeight / 2 + viewYOffset);
                    window.setView(view);
                    break;
                }
            }

            // 바닥에 닿으면 멈춤
            if (block.getPosition().y + blockSize >= windowHeight + viewYOffset - 5)
            {
                block.setPosition(block.getPosition().x, windowHeight + viewYOffset - blockSize - 5);
                blocks.push_back(block);
                block.setFillColor(getRandomColor()); // 새 블록 색상 설정
                block.setPosition((windowWidth - blockSize) / 2, viewYOffset);
                falling = false;
                viewYOffset -= blockSize; // 뷰 오프셋 조정
                view.setCenter(windowWidth / 2, windowHeight / 2 + viewYOffset);
                window.setView(view);
            }
        }

        window.clear(sf::Color::White);
        window.setView(view); // 뷰 적용
        for (const auto& placedBlock : blocks)
        {
            window.draw(placedBlock);
        }
        window.draw(block);
        window.draw(floor);
        window.display();
    }

    return 0;
}
