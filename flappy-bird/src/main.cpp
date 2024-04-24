#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <list>
#include <time.h>
using std::cout;


int main() {

	// sf::Sound needs a sf::SoundBuffer, while sf::Music doesn't
	sf::RenderWindow window(sf::VideoMode(400, 550), "Flappy bird");
	sf::Event event;

	sf::Font font;
	font.loadFromFile("C:\\Development\\Fonts\\Poppins\\Poppins-Bold.ttf");
	sf::Text start;
	start.setString("Space to start");
	start.setFillColor(sf::Color(0, 255, 150));
	start.setCharacterSize(25);
	start.setPosition(100, 150);
	start.setFont(font);

	sf::RectangleShape startBox(sf::Vector2f(205, 30));
	startBox.setFillColor(sf::Color(160,3,160));
	startBox.setPosition(90, 150);

	srand(time(0));

	sf::Texture background, base;
	background.loadFromFile("assets\\game\\background-day.png");
	sf::Sprite sBackground(background);
	sBackground.setScale(1.4, 1.1);

	base.loadFromFile("assets\\game\\base.png");
	sf::Sprite sBase(base);

	sf::Texture bird, birdFlapUp, birdFlapDown;
	bird.loadFromFile("assets\\game\\yellowbird-midflap.png");
	birdFlapUp.loadFromFile("assets\\game\\yellowbird-upflap.png");
	birdFlapDown.loadFromFile("assets\\game\\yellowbird-downflap.png");

	sf::Sprite sBird(bird);
	sBird.setPosition(50, 50);
	sBird.setScale(1.5, 1.5);

	int score = 0;
	float y = 200, scroll = 0, scrollPipe = 0.5;
	bool stopFalling = true, playing = false, flapUp = false;
	sf::Clock flapClock;

	sf::Texture pipe;
	pipe.loadFromFile("assets\\game\\pipe-green.png");
	sf::Sprite sPipe(pipe);
	std::list<sf::Sprite> pipes;
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
	pt.loadFromFile("assets\\sound\\point.wav");
	sf::Sound point(pt);
	point.setVolume(50);

	sf::RectangleShape rect(sf::Vector2f(200, 400));
	rect.setFillColor(sf::Color::Blue);
	rect.setPosition(0, 0);
	music.play();

	sf::Texture n;
	sf::Sprite numbers[10];
	for (int i = 0; i < 10; i++) {
		n.loadFromFile("assets\\UI\\Numbers\\" + std::to_string(i) + ".png");
		numbers[i] = sf::Sprite(n);
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
					}
					else if (!stopFalling && !flapUp) {
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

		if (numPipes < 4) {
			int randHeight = rand() % 50 + 130;
			sPipe.setTextureRect(sf::IntRect(0,0,sPipe.getGlobalBounds().width, randHeight));
			sPipe.setPosition(400+numPipes*300, randHeight);
			sPipe.setRotation(180);
			pipes.push_back(sPipe);
			numPipes++;
		}

		timer = clock.getElapsedTime().asMilliseconds();
		if (timer > (1/30)) {
			if (playing) {
				for (auto pipe = pipes.begin(); pipe != pipes.end();) {
					sf::Vector2f pos = pipe->getPosition();
					if (pos.x < 0) {
						score++;
						point.play();
						pipe = pipes.erase(pipe);
						numPipes--;
					} else {
						pipe->setPosition(pos.x - scrollPipe, pos.y);
						window.draw(*pipe);
						sf::Sprite downPipe = (*pipe);
						downPipe.setRotation(0);
						int randomHeight = (int)sPipe.getGlobalBounds().height % 30 + 130;
						downPipe.setTextureRect(sf::IntRect(0, 0, sPipe.getGlobalBounds().width, randomHeight));
						downPipe.setPosition(pos.x - scrollPipe - downPipe.getGlobalBounds().width, (window.getSize().y - randomHeight) - sBase.getGlobalBounds().height + 12);
						window.draw(downPipe);
						if (pipe->getGlobalBounds().intersects(sBird.getGlobalBounds()) || downPipe.getGlobalBounds().intersects(sBird.getGlobalBounds())) {
							score = 0;
							playing = false;
						}
						++pipe;
					}
				}
				sBird.setPosition(50, y);
				window.draw(sBird);
				if (!stopFalling) {
					y += 0.5;
					scroll++;
					if (scroll > 50) scroll = 0;
					if (y > 420) stopFalling = true;
				}
				if (flapUp) {
					y -= 0.5;
					if (flapClock.getElapsedTime().asSeconds() > 0.35) {
						flapUp = false;
						stopFalling = false;
						sBird.setTexture(birdFlapDown);
						sBird.setRotation(5);
					}
				}
			} else {
				window.draw(startBox);
				window.draw(start);
			}
			int tempScore = score, j = 0;
			while (tempScore /= 10) {
				int num = tempScore % 10;
				cout << num;
				numbers[num].setPosition(window.getSize().x - 50 - (j*50), 20);
				window.draw(numbers[num]);
				j++;
			}

			window.display();
			clock.restart();
			timer = 0;
		}
	}

	return 0;
}