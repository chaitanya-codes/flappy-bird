#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <list>
#include <time.h>
using std::cout;

int main() {

	// sf::Sound needs a sf::SoundBuffer, while sf::Music doesn't
	sf::RenderWindow window(sf::VideoMode(400, 550), "Flappy bird");
	sf::Event event;

	sf::Font font;
	font.loadFromFile("assets/fonts/Poppins-Bold.ttf");
	sf::Text start;
	start.setString("Space to start");
	start.setFillColor(sf::Color(0, 255, 150));
	start.setCharacterSize(25);
	start.setPosition(100, 150);
	start.setFont(font);

	sf::RectangleShape startBox(sf::Vector2f(205, 30));
	startBox.setFillColor(sf::Color(160, 3, 160));
	startBox.setPosition(90, 150);

	srand(time(0));

	sf::Texture background, base;
	background.loadFromFile("assets/game/background-day.png");
	sf::Sprite sBackground(background);
	sBackground.setScale(1.4, 1.1);

	base.loadFromFile("assets/game/base.png");
	sf::Sprite sBase(base);

	sf::Texture bird, birdFlapUp, birdFlapDown;
	bird.loadFromFile("assets/game/yellowbird-midflap.png");
	birdFlapUp.loadFromFile("assets/game/yellowbird-upflap.png");
	birdFlapDown.loadFromFile("assets/game/yellowbird-downflap.png");

	sf::Sprite sBird(bird);
	sBird.setPosition(50, 50);
	sBird.setScale(1.5, 1.5);

	int score = 0;
	float y = 200, scroll = 0, scrollPipe = 0.4;
	bool stopFalling = true, playing = false, flapUp = false;
	sf::Clock flapClock;

	sf::Texture pipe;
	pipe.loadFromFile("assets/game/pipe-green.png");
	sf::Sprite sPipe(pipe);
	std::list<sf::Sprite> pipes;
	std::list<sf::Sprite> displayNumbers;
	int numPipes = 0;
	/*sf::SoundBuffer buffer;
	buffer.loadFromFile("scott-buckley-moonlight.mp3");

	sf::Sound sound;
	sound.setBuffer(buffer);*/

	sf::Music music;
	if (!music.openFromFile("assets/scott-buckley-moonlight.mp3")) {
		std::cout << "Error";
	}

	sf::SoundBuffer pt;
	pt.loadFromFile("assets/sound/point.wav");
	sf::Sound point(pt);
	point.setVolume(50);

	sf::RectangleShape rect(sf::Vector2f(200, 400));
	rect.setFillColor(sf::Color::Blue);
	rect.setPosition(0, 0);
	music.play();

	std::vector<sf::Sprite> numbers(10);
	std::vector<sf::Texture> numTextures(10);
	for (int i = 0; i < 10; i++) {
		cout << "Loaded assets/UI/Numbers/" + std::to_string(i) + ".png" << std::endl;
		numTextures[i].loadFromFile("assets/UI/Numbers/" + std::to_string(i) + ".png");
		numbers[i].setTexture(numTextures[i]);
	}

	sf::Clock clock;
	int timer = 0;
	while (window.isOpen()) {
		window.clear(sf::Color::Green);
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				playing = false;
				music.stop();
				window.close();
				exit(0);
			} else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::M) {
					if (music.getStatus() == music.Paused) music.play();
					else music.pause();
				} else if (event.key.code == sf::Keyboard::Space) {
					if (!playing) {
						y = 210;
						playing = true;
						stopFalling = false;
						numPipes = 0;
						pipes.clear();
					} else if (!stopFalling && !flapUp) {
						stopFalling = true;
						sBird.setTexture(birdFlapUp);
						sBird.setRotation(-5);
						flapUp = true;
						flapClock.restart();
					}
				}
			}
		}
		window.draw(sBackground);

		for (int i = 0; i < 2; i++) {
			sBase.setPosition(335 * i - scroll, 450);
			window.draw(sBase);
		}

		timer = clock.getElapsedTime().asMilliseconds();
		if (timer > (1.0/30)) {
			if (playing) {
				if (numPipes < 4) {
					int randHeight = ((rand() % 10) + 4) * 20;
					sPipe.setTextureRect(sf::IntRect(0, 0, sPipe.getGlobalBounds().width, randHeight));
					sPipe.setPosition(400 + numPipes * 400, (window.getSize().y - randHeight) - sBase.getGlobalBounds().height + 12);
					pipes.push_back(sPipe);
					numPipes++;

					sf::Sprite downPipe(sPipe);
					int width = downPipe.getGlobalBounds().width;
					int randHeight2 = (60 * (int)((rand() % 2) + 1));
					int height = 300 - (int)downPipe.getGlobalBounds().height;
					height = std::vector<int>{ height, randHeight2 <= height ? randHeight2 : height }[rand() % 2];
					downPipe.setRotation(180);
					downPipe.setTextureRect(sf::IntRect(0, 0, width, height));
					downPipe.setPosition(sPipe.getPosition().x - scrollPipe + width, height);
					pipes.push_back(downPipe);
				}

				for (auto pipe = pipes.begin(); pipe != pipes.end();) {
					sf::Vector2f pos = pipe->getPosition();
					if (pos.x < 0-sPipe.getGlobalBounds().width) {
						score++;
						point.play();
						pipe = pipes.erase(pipe); // Set pipe iterator to next pipe
						pipe = pipes.erase(pipe);
						numPipes--;
						displayNumbers.clear();
						int tempScore = score;
						while (tempScore > 0) {
							int num = tempScore % 10;
							displayNumbers.push_back(numbers[num]);
							tempScore /= 10;
						}
					} else {
						pipe->setPosition((pos.x - scrollPipe) * 1, pos.y);
						window.draw(*pipe);
						
						if (pipe->getGlobalBounds().intersects(sBird.getGlobalBounds())) {
							score = 0;
							displayNumbers.clear();
							playing = false;
						}
						++pipe;
					}
				}
				sBird.setPosition(50, y);
				window.draw(sBird);

				scroll += 0.4;
				if (scroll > base.getSize().x / 2) scroll = 0;

				if (!stopFalling) {
					y += 0.3;
					if (y > 420) stopFalling = true;
				}
				if (flapUp) {
					y -= 0.4;
					if (flapClock.getElapsedTime().asSeconds() > 0.35) {
						flapUp = false;
						stopFalling = false;
						sBird.setTexture(birdFlapDown);
						sBird.setRotation(5);
					}
				}
				int i = 0;

				for (auto number : displayNumbers) {
					number.setPosition(350 - i*20, 20); // Shift digits in ten's place
					window.draw(number);
					i++;
				}
			} else {
				window.draw(startBox);
				window.draw(start);
			}

			window.display();
			clock.restart();
			timer = 0;
		}
	}

	return 0;
}