#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int GRID_SIZE = 50;

struct Player {
    sf::Sprite sprite;

    Player(const sf::Texture& texture) {
        sprite.setTexture(texture);
        sprite.setScale(50.0f / texture.getSize().x, 50.0f / texture.getSize().y);
        sprite.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - GRID_SIZE);
    }

    void move(const sf::Vector2f& direction) {
        sf::Vector2f newPosition = sprite.getPosition() + sf::Vector2f(direction.x * GRID_SIZE, direction.y * GRID_SIZE);

        if (newPosition.x >= 0 && newPosition.x <= WINDOW_WIDTH - GRID_SIZE &&
            newPosition.y >= 0 && newPosition.y <= WINDOW_HEIGHT - GRID_SIZE) {
            sprite.setPosition(newPosition);
        }
    }
};

struct Obstacle {
    sf::RectangleShape shape;
    sf::Texture texture;
    int direction;

    Obstacle(float x, float y, int direction, const sf::Texture& texture) : direction(direction) {
        shape.setSize(sf::Vector2f(GRID_SIZE * 2, GRID_SIZE));
        shape.setPosition(x, y);
        shape.setTexture(&texture);
    }

    void move(float deltaTime) {
        float speed = 100.0f * direction * deltaTime;
        sf::Vector2f newPosition = shape.getPosition() + sf::Vector2f(speed, 0);

        if (newPosition.x > WINDOW_WIDTH) {
            newPosition.x = -shape.getSize().x;
        } else if (newPosition.x + shape.getSize().x < 0) {
            newPosition.x = WINDOW_WIDTH;
        }

        shape.setPosition(newPosition);
    }
};

struct Coin {
    sf::CircleShape shape;

    Coin(float x, float y) {
        shape.setRadius(GRID_SIZE / 4.0f);
        shape.setFillColor(sf::Color::Yellow);
        shape.setPosition(x + GRID_SIZE / 4.0f, y + GRID_SIZE / 4.0f);
    }

    sf::FloatRect getBounds() const {
        return shape.getGlobalBounds();
    }
};

void showGameOverWindow(int score) {
    sf::RenderWindow gameOverWindow(sf::VideoMode(400, 200), "Game Over");

    sf::Font font;
    if (!font.loadFromFile("D:\\Projects\\Croosy\\assets\\font.ttf")) {
        return;
    }

    sf::Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setString("Game Over!");
    gameOverText.setCharacterSize(36);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(100, 30);

    std::ostringstream scoreStream;
    scoreStream << "Your score: " << score;

    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setString(scoreStream.str());
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(100, 100);

    while (gameOverWindow.isOpen()) {
        sf::Event event;
        while (gameOverWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                gameOverWindow.close();
            }
        }

        gameOverWindow.clear();
        gameOverWindow.draw(gameOverText);
        gameOverWindow.draw(scoreText);
        gameOverWindow.display();
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Crossy Road - Infinite Scrolling");
    window.setFramerateLimit(60);

    // Загрузка текстуры фона (дороги)
    sf::Texture roadTexture;
    if (!roadTexture.loadFromFile("D:\\Projects\\Croosy\\assets\\road_texture.jpg")) {
        return -1;
    }
    roadTexture.setRepeated(true);

    sf::RectangleShape roadBackground(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    roadBackground.setTexture(&roadTexture);
    roadBackground.setTextureRect(sf::IntRect(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT));

    // Загрузка текстуры игрока
    sf::Texture playerTexture;  
    if (!playerTexture.loadFromFile("D:\\Projects\\Croosy\\assets\\pers.png")) {
        return -1;
    }
    Player player(playerTexture);

    std::vector<Obstacle> obstacles;
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Загрузка текстур машин
    std::vector<sf::Texture> carTextures(3);
    if (!carTextures[0].loadFromFile("D:\\Projects\\Croosy\\assets\\car1.png") ||
        !carTextures[1].loadFromFile("D:\\Projects\\Croosy\\assets\\car2.png") ||
        !carTextures[2].loadFromFile("D:\\Projects\\Croosy\\assets\\car3.png")) {
        return -1;
    }

    // Генерация начальных препятствий
    for (int i = 1; i < WINDOW_HEIGHT / GRID_SIZE; i++) {
        float y = i * GRID_SIZE;
        float x = (std::rand() % (WINDOW_WIDTH / GRID_SIZE)) * GRID_SIZE;
        int direction = (std::rand() % 2 == 0) ? -1 : 1;
        const sf::Texture& randomTexture = carTextures[std::rand() % carTextures.size()];
        obstacles.emplace_back(x, y, direction, randomTexture);
    }

    // Монеты
    std::vector<Coin> coins;
    for (int i = 0; i < 5; ++i) {
        float x = (std::rand() % (WINDOW_WIDTH / GRID_SIZE)) * GRID_SIZE;
        float y = (std::rand() % (WINDOW_HEIGHT / GRID_SIZE)) * GRID_SIZE;
        coins.emplace_back(x, y);
    }

    sf::Clock clock;
    float playerTimeSinceLastMove = 0.0f;
    const float playerMoveInterval = 0.3f;
    float backgroundOffset = 0.0f;

    int score = 0;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        float deltaTime = clock.restart().asSeconds();
        playerTimeSinceLastMove += deltaTime;

        // Движение игрока
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && playerTimeSinceLastMove > playerMoveInterval) {
            player.move({0.0f, -1.0f});
            backgroundOffset += GRID_SIZE;
            score += 10;
            playerTimeSinceLastMove = 0.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && playerTimeSinceLastMove > playerMoveInterval) {
            player.move({0.0f, 1.0f});
            backgroundOffset -= GRID_SIZE;
            score = std::max(0, score - 10);
            playerTimeSinceLastMove = 0.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && playerTimeSinceLastMove > playerMoveInterval) {
            player.move({-1.0f, 0.0f});
            playerTimeSinceLastMove = 0.0f;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && playerTimeSinceLastMove > playerMoveInterval) {
            player.move({1.0f, 0.0f});
            playerTimeSinceLastMove = 0.0f;
        }

        // Перемещение препятствий
        for (auto& obstacle : obstacles) {
            obstacle.move(deltaTime);
        }

        // Детекция столкновений
        for (auto& obstacle : obstacles) {
            if (player.sprite.getGlobalBounds().intersects(obstacle.shape.getGlobalBounds())) {
                showGameOverWindow(score);
                window.close();
            }
        }

        // Сбор монет
        coins.erase(std::remove_if(coins.begin(), coins.end(),
            [&](const Coin& coin) {
                if (player.sprite.getGlobalBounds().intersects(coin.getBounds())) {
                    score += 50;
                    return true; // Удалить монету
                }
                return false;
            }), coins.end());

        // Обновление фона
        roadBackground.setTextureRect(sf::IntRect(0, static_cast<int>(backgroundOffset) % WINDOW_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT));

        // Отрисовка
        window.clear();
        window.draw(roadBackground);
        window.draw(player.sprite);
        for (const auto& obstacle : obstacles) {
            window.draw(obstacle.shape);
        }
        for (const auto& coin : coins) {
            window.draw(coin.shape);
        }
        window.display();
    }

    return 0;
}
