#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>

#define WIDTH 800
#define HEIGHT 600

class InputManager {
public:
	static bool isKeyPressed(sf::Keyboard::Key key) {
		return sf::Keyboard::isKeyPressed(key);
	}
};


class StaticBody : public sf::RectangleShape {
public:
	StaticBody(sf::Vector2f pos = { 0.f, 0.f }, sf::Vector2f size = { 100.f, 100.f }, sf::Color color = sf::Color::White) {
		this->setPosition(pos);
		this->setSize(size);
		this->setFillColor(color);
	}
};


class World {
public:
	World(std::vector<StaticBody> staticBodies, sf::RenderTarget& target) {
		this->staticBodies = staticBodies;
		view = target.getDefaultView();
	}
	void setBackgroundColor(sf::Color color = sf::Color::White) {
		backgroundColor = color;
	}
	void apply(sf::RenderTarget& target) {
		target.clear(this->backgroundColor);
	}
	float getDeltaTime() {
		return this->clock.restart().asSeconds();
	}

	void onEvent(const std::optional<sf::Event> event) {
		if (event) {

			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				if (keyPressed->scancode == sf::Keyboard::Scancode::NumpadPlus) {
					// Not Implemented
				}
				else if (keyPressed->scancode == sf::Keyboard::Scancode::NumpadMinus) {
					// Not Implemented
				}
			}
		}
	}

	sf::Color backgroundColor;
	sf::Clock clock;
	sf::Vector2f gravity{ 0.f, 10.f };
	sf::View view;
	std::vector<StaticBody> staticBodies;
};


class Player : public sf::RectangleShape {
public:
	Player(World& world) : world{ world } {
		this->setPosition({ WIDTH / 2, HEIGHT / 2 });
		this->setSize({ 50.f, 50.f });
		this->setFillColor(sf::Color::White);
	}
	sf::Vector2f velocity{ 0.f, 0.f };
	float acceleration{ 1000.f };
	float deacceleration{ 300.f };
	float moveSpeed = 1000.f;
	float currentSpeed = 0.f;
	float jump = 2500.f;
	sf::Angle normal = sf::degrees(-90);
	World& world;


	void onStart() {
		// We are not going to do anything here...
	}

	void onUpdate(float deltaTime) {
		float horizontal = (
			InputManager::isKeyPressed(sf::Keyboard::Key::D) -
			InputManager::isKeyPressed(sf::Keyboard::Key::A)
			);

		if (horizontal != 0) {

			currentSpeed = std::lerp(currentSpeed, moveSpeed * horizontal, acceleration * deltaTime);
		}
		else {
			currentSpeed = std::lerp(currentSpeed, 0, deacceleration * deltaTime);

		}


		velocity.x = currentSpeed;


		if (InputManager::isKeyPressed(sf::Keyboard::Key::Space) && isGrounded()) {
			this->velocity = sf::Vector2{
			std::cos(this->normal.asRadians()),
			std::sin(this->normal.asRadians())
			} *jump;
		}

		// DEBUG
		if (InputManager::isKeyPressed(sf::Keyboard::Key::Down)) {
			this->velocity = sf::Vector2{
				0.f,
				500.f
			};
		}


		if (this->isGrounded()) {
			if (this->velocity.y > 0) {
				this->velocity.y = 0;
			}
		}
		else {
			velocity += world.gravity * static_cast<float>(!this->isGrounded());
		}

		this->move(velocity * deltaTime);

	}

	bool isGrounded() {
		return static_cast<bool>(this->onCollision().first);
	}

	std::pair<std::optional<sf::FloatRect>, StaticBody> onCollision() {
		for (auto& staticBody : this->world.staticBodies) {
			return std::pair<std::optional<sf::FloatRect>, StaticBody>(this->getGlobalBounds().findIntersection(staticBody.getGlobalBounds()), staticBody);
		}
	}

};

int main()
{
	auto window = sf::RenderWindow(sf::VideoMode({ WIDTH, HEIGHT }), "SFML Platformer Game");

	std::vector<Player> players;
	std::vector<StaticBody> staticBodies;

	StaticBody ground{ {0.f, HEIGHT - 100.f}, {WIDTH * 10, 100.f}, sf::Color(98, 175, 96) };

	staticBodies.assign({ ground });
	World world(staticBodies, window);
	Player player{ world };
	players.push_back(player);

	world.setBackgroundColor(sf::Color(112, 148, 206));
	while (window.isOpen())
	{
		float deltaTime = world.getDeltaTime();
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
			world.onEvent(event);
		}
		window.setView(world.view);

		window.setTitle("SFML Platformer Game | FPS: " + std::to_string(static_cast<int>(1 / deltaTime)));

		for (Player& player : players)
			player.onUpdate(deltaTime);

		world.apply(window);
		for (auto& staticBody : staticBodies)
			window.draw(staticBody);
		for (auto& player : players)
			window.draw(player);
		window.display();
	}
}
