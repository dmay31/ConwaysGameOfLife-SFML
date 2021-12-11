#include <SFML/Graphics.hpp>
#include <array>
#include <list>
#include <math.h>
#include <sstream>
#include <thread>

#define WIDTH   1000
#define HEIGHT  801
#define SPACING 2
#define ROW_SQ  ((int)( WIDTH/SPACING))
#define COL_SQ  ((int)( HEIGHT/SPACING))
#define TOT_SQ  ( ROW_SQ * COL_SQ )
#define GRIDLINE_CNT (((WIDTH/SPACING)+(HEIGHT/SPACING))*2)+5

class neighbors
{
public:
	int top;
	int top_left;
	int top_right;
	int left;
	int right;
	int bottom;
	int bottom_left;
	int bottom_right;

	neighbors() {
		top = -1;
		top_left = -1;
		top_right = -1;
		right = -1;
		left = -1;
		bottom = -1;
		bottom_left = -1;
		bottom_right = -1;
	}

	void toList(std::list<int> &aList) {
		aList.push_back(top);
		aList.push_back(top_left);
		aList.push_back(top_right);
		aList.push_back(bottom);
		aList.push_back(bottom_left);
		aList.push_back(bottom_right);
		aList.push_back(right);
		aList.push_back(left);

		aList.remove(-1);

	}
};

class Square
{
public:
	int get_index() {
		return((y * ROW_SQ) + x);
	}

	int x, y;
	bool alive;
	bool has_neighbor;
	static std::array<Square, TOT_SQ> map;

	Square() { x = 0; y = 0; alive = false; }

	~Square() {
		//printf("Square destructor\n");
		//this->kill();
	}

	Square(const Square &aSquare) { 
		x = aSquare.x; 
		y = aSquare.y; 
		alive = aSquare.alive;
	}

	Square(int aX, int aY, bool proto = false) { 
		this->x = aX; 
		this->y = aY; 

		if (!proto) {
			this->alive = true;
			map[get_index()] = *this;
		}
		else {
			this->alive = false;
		}
	}

	void kill() {
		map[get_index()].alive = false;
		this->alive = false;
	}

	void born() {
		map[get_index()].alive = true;
		this->alive = true;
	}

	bool operator==(const Square& val) {
		bool equal = true;
		equal &= this->x == val.x;
		equal &= this->y == val.y;

		return equal;
	}

	bool operator<(Square& val) {
		return(this->get_index() < val.get_index());
	}
	
	int get_active_neighbors(neighbors& neighborhood) {
		int count = 0;
		count += neighborhood.top != -1 ? map[neighborhood.top].alive == true ? 1 : 0 : 0;
		count += neighborhood.top_left != -1 ? map[neighborhood.top_left].alive == true ? 1: 0 : 0;
		count += neighborhood.top_right != -1 ? map[neighborhood.top_right].alive == true ? 1 : 0 : 0;
		count += neighborhood.bottom != -1 ? map[neighborhood.bottom].alive == true ? 1 : 0 : 0;
		count += neighborhood.bottom_left != -1 ? map[neighborhood.bottom_left].alive == true ? 1 : 0 : 0;
		count += neighborhood.bottom_right != -1 ? map[neighborhood.bottom_right].alive == true ? 1 : 0 : 0;
		count += neighborhood.left != -1 ? map[neighborhood.left].alive == true ? 1 : 0 : 0;
		count += neighborhood.right != -1 ? map[neighborhood.right].alive == true ? 1 : 0 : 0;

		return count;
	}

	void get_dead_neighbors(neighbors& neighborhood, std::list<Square>& aList) {
		if ((neighborhood.top != -1) && map[neighborhood.top].alive == false)
		{
			aList.push_back(map[neighborhood.top]);
		}
		if ((neighborhood.top_left != -1) && map[neighborhood.top_left].alive == false)
		{
			aList.push_back(map[neighborhood.top_left]);
		}
		if ((neighborhood.top_right != -1) && map[neighborhood.top_right].alive == false)
		{
			aList.push_back(map[neighborhood.top_right]);
		}
		if ((neighborhood.bottom != -1) && map[neighborhood.bottom].alive == false)
		{
			aList.push_back(map[neighborhood.bottom]);
		}
		if ((neighborhood.bottom_left != -1) && map[neighborhood.bottom_left].alive == false)
		{
			aList.push_back(map[neighborhood.bottom_left]);
		}
		if ((neighborhood.bottom_right != -1) && map[neighborhood.bottom_right].alive == false)
		{
			aList.push_back(map[neighborhood.bottom_right]);
		}
	}

	void get_neighbors(neighbors& neighborhood ) {
		//check right, left, up, down, up-left, up-right, down-left, down-right
		int active_cnt = 0;
		int my_idx = (y * ROW_SQ) + x;
		bool is_right = (my_idx != 0) && (((my_idx+1) % (ROW_SQ)) == 0);
		bool is_left = ((my_idx % ROW_SQ) == 0);
		bool is_top = (my_idx <= (ROW_SQ - 1));
		bool is_bottom = (my_idx >= (ROW_SQ*(COL_SQ - 1)));

		if (!is_top) {
			neighborhood.top = my_idx - ROW_SQ;

			if (!is_left) {
				neighborhood.top_left = my_idx - ROW_SQ - 1;
			}

			if (!is_right) {
				neighborhood.top_right = my_idx - ROW_SQ + 1;
			}
		}

		if (!is_left) {
			neighborhood.left = my_idx - 1;
		}

		if (!is_right) {
			neighborhood.right = my_idx + 1;
		}

		if (!is_bottom) {
			neighborhood.bottom = my_idx + ROW_SQ;

			if (!is_left) {
				neighborhood.bottom_left = my_idx + ROW_SQ - 1;
			}

			if (!is_right) {
				neighborhood.bottom_right = my_idx + ROW_SQ + 1;
			}

		}
	}
};

enum {
	GLIDERGUN,
	SQUARE,
	POINT,
	ACORN,
	ALL,

	MODE_CNT,

	DELETE
};

std::array<Square, TOT_SQ> Square::map;
static int mode = POINT;

void addGospersGliderGun(std::list<Square>& aList, int x, int y);
void addSquare(std::list<Square>& aList, int x, int y);
void addPoint(std::list<Square>& aList, int x, int y);
void addAcorn(std::list<Square>& aList, int x, int y);
void getCenter(std::list<Square>& aList, int& x, int& y);
void translate(std::list<Square>& aList, int x, int y);
void rotate(std::list<Square>& aList, int angle);
void addAll(std::list<Square>& aList);


int main()
{
	static int count = 0;
	static int xPosition = 0;
	static int yPosition = 0;
	static int draw_idx = 0;
	std::list<Square> active_squares;
	std::list<Square> proto_squares;
	std::list<Square> delete_squares;
	sf::Vector2i MousePosition;
	Square * MouseSquare = nullptr;
	sf::Font myFont;
	bool pauseState = false;
	int rotationValue = 0;

	active_squares.resize(0);
	proto_squares.resize(0);
	delete_squares.resize(0);

	const auto processor_count = std::thread::hardware_concurrency();
	printf("processor_count = %d\n", processor_count);

	if (!myFont.loadFromFile("Hello Avocado.ttf")) {
		printf("Failed to load FontFile\n");
	}
	
	sf::Text someText;
	someText.setFont(myFont);


	printf("HEIGHT: %d, WIDTH: %d, ROWS: %d, COLUMNS: %d TOTOAL: %d \n", HEIGHT, WIDTH, ROW_SQ, COL_SQ, TOT_SQ);
	
	int idx = 0;
	for (Square& square : Square::map) {
		square.x = idx % ROW_SQ;
		square.y = idx / ROW_SQ;
		idx++;
	}

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SML works", sf::Style::Titlebar | sf::Style::Close);
	
	sf::VertexArray lines(sf::Lines, GRIDLINE_CNT);

	int j=0;
	for (auto i = 0; i < (HEIGHT / SPACING) + 1; i++) {
		lines[j].position = sf::Vector2f(0, SPACING*i);
		lines[j++].color = sf::Color(50, 50, 50);
		lines[j].position = sf::Vector2f(WIDTH, SPACING*i);
		lines[j++].color = sf::Color(50, 50, 50);
		window.draw(lines);
	}

	lines[j].color = sf::Color(50, 50, 50);
	lines[j++].position = sf::Vector2f(1, 0);
	lines[j].color = sf::Color(50, 50, 50);
	lines[j++].position = sf::Vector2f(1, HEIGHT);

	for (auto i = 0; i < (WIDTH / SPACING) + 1; i++) {
		lines[j].position = sf::Vector2f(SPACING*i, 0);
		lines[j++].color = sf::Color(50, 50, 50);
		lines[j].position = sf::Vector2f(SPACING*i, HEIGHT);
		lines[j++].color = sf::Color(50, 50, 50);
		window.draw(lines);
	}

	//addGospersGliderGun(active_squares, 10, 20);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}

			if (event.type == sf::Event::MouseMoved) {
				MousePosition = sf::Mouse::getPosition(window);
			}

			if (event.type == sf::Event::MouseButtonPressed ) {
				switch (event.mouseButton.button) {
				case sf::Mouse::Left:
					if (proto_squares.size() > 0) {
						for (auto square : proto_squares )
						{
							square.born();
							active_squares.push_back(square);
						}
					}

					if (delete_squares.size() > 0) {
						for (auto square : delete_squares)
						{
							active_squares.remove(square);
						}
					}
					break;

				default:
					break;
				}
			}

			if (event.type == sf::Event::KeyPressed) {
				switch (event.key.code)
				{
				case sf::Keyboard::Enter:
					if (proto_squares.size() > 0) {
						for (auto square : proto_squares)
						{
							square.born();
							active_squares.push_back(square);
						}
					}

					if (delete_squares.size() > 0) {
						for (auto square : delete_squares)
						{
							active_squares.remove(square);
						}
					}
					break;

				case sf::Keyboard::Left:
				{
					auto p = sf::Mouse::getPosition(window);
					p.x -= SPACING;
					sf::Mouse::setPosition(p, window);
				}
					break;

				case sf::Keyboard::Right:
					{
					auto p = sf::Mouse::getPosition(window);
					p.x += SPACING;
					sf::Mouse::setPosition(p,window);
					}
					break;

				case sf::Keyboard::Up:
					{
					auto p = sf::Mouse::getPosition(window);
					p.y -= SPACING;
					sf::Mouse::setPosition(p, window);
					}
					break;

				case sf::Keyboard::Down:
					{
					auto p = sf::Mouse::getPosition(window);
					p.y += SPACING;
					sf::Mouse::setPosition(p, window);
					}
					break;

				case sf::Keyboard::D:
					if (true == pauseState)
					{
						mode = DELETE;
					}
					break;

				case sf::Keyboard::P:
					pauseState = !pauseState;
					if (false == pauseState) {
						mode = SQUARE;
					}
					break;

				case sf::Keyboard::R:
					rotationValue = (rotationValue + 90) % 360;
					break;

				case sf::Keyboard::C:
					for (auto square : active_squares) {
						square.kill();
					}
					active_squares.clear();

				case sf::Keyboard::Space:
					if (pauseState) {
						if (mode != DELETE) {
							mode = (++mode % MODE_CNT);
						}

					}
					break;

				default:
					break;
				}
			}

			if (pauseState) {
				proto_squares.clear();
				delete_squares.clear();
				switch (mode)
				{
				case POINT:
					addPoint(proto_squares, MousePosition.x / SPACING, MousePosition.y / SPACING);
					break;
				case GLIDERGUN:
					addGospersGliderGun(proto_squares, MousePosition.x / SPACING, MousePosition.y / SPACING);
					break;
				case ALL:
					addAll(proto_squares);
					break;
				case DELETE:
					addPoint(delete_squares, MousePosition.x / SPACING, MousePosition.y / SPACING);
					break;
				case ACORN:
					addAcorn(proto_squares, MousePosition.x / SPACING, MousePosition.y / SPACING);
					break;

				case SQUARE:
				default:
					addSquare(proto_squares, MousePosition.x / SPACING, MousePosition.y / SPACING);
					break;
				}

				rotate(proto_squares, rotationValue);
			}
			else {
				proto_squares.clear();
			}
		}
			
		window.clear();

		window.draw(lines);

		int quad_counter = 0;
		sf::VertexArray proto_quad(sf::Quads, proto_squares.size() * 4);
		for (auto square : proto_squares)
		{
			proto_quad[quad_counter].position = sf::Vector2f(SPACING*square.x, SPACING*square.y);
			proto_quad[quad_counter + 1].position = sf::Vector2f(SPACING*square.x, (SPACING*square.y) + SPACING);
			proto_quad[quad_counter + 2].position = sf::Vector2f((SPACING*square.x) + SPACING, (SPACING*square.y) + SPACING);
			proto_quad[quad_counter + 3].position = sf::Vector2f((SPACING*square.x) + SPACING, square.y*SPACING);
			proto_quad[quad_counter].color = sf::Color::Blue;
			proto_quad[quad_counter + 1].color = sf::Color::Blue;
			proto_quad[quad_counter + 2].color = sf::Color::Blue;
			proto_quad[quad_counter + 3].color = sf::Color::Blue;
			quad_counter += 4;
		}

		quad_counter = 0;
		sf::VertexArray delete_quad(sf::Quads, delete_squares.size() * 4);
		for (auto square : delete_squares) {
			delete_quad[quad_counter].position = sf::Vector2f(SPACING*square.x, SPACING*square.y);
			delete_quad[quad_counter + 1].position = sf::Vector2f(SPACING*square.x, (SPACING*square.y) + SPACING);
			delete_quad[quad_counter + 2].position = sf::Vector2f((SPACING*square.x) + SPACING, (SPACING*square.y) + SPACING);
			delete_quad[quad_counter + 3].position = sf::Vector2f((SPACING*square.x) + SPACING, square.y*SPACING);
			delete_quad[quad_counter].color = sf::Color::Yellow;
			delete_quad[quad_counter + 1].color = sf::Color::Blue;
			delete_quad[quad_counter + 2].color = sf::Color::Blue;
			delete_quad[quad_counter + 3].color = sf::Color::Blue;
			quad_counter += 4;

		}
		
		window.draw(delete_quad);
		window.draw(proto_quad);

		std::list<Square> dead_list;
		std::list<Square> remove_list;
		std::list<Square> born_list;
		sf::VertexArray quad(sf::Quads, active_squares.size() * 4);
		quad_counter = 0;

		for( auto square : active_squares )
		{
			if (square.alive) {
				quad[quad_counter].position = sf::Vector2f(SPACING*square.x, SPACING*square.y);
				quad[quad_counter + 1].position = sf::Vector2f(SPACING*square.x, (SPACING*square.y) + SPACING);
				quad[quad_counter + 2].position = sf::Vector2f((SPACING*square.x) + SPACING, (SPACING*square.y) + SPACING);
				quad[quad_counter + 3].position = sf::Vector2f((SPACING*square.x) + SPACING, square.y*SPACING);
				quad[quad_counter].color = sf::Color::Green;
				quad[quad_counter + 1].color = sf::Color::Green;
				quad[quad_counter + 2].color = sf::Color::Green;
				quad[quad_counter + 3].color = sf::Color::Green;
				quad_counter+=4;

				neighbors my_neighbors;
				square.get_neighbors(my_neighbors);

				int neighbor_count = square.get_active_neighbors(my_neighbors);
				if (neighbor_count < 2 || neighbor_count > 3) {
					remove_list.push_back(square);
				}

				//get the list of surrounding cells for each active cell
				square.get_dead_neighbors(my_neighbors, dead_list);
			}
		}
		someText.setCharacterSize(24);
		someText.setFillColor(sf::Color::Red);

		std::ostringstream oss;

		if (pauseState) {
			switch (mode)
			{
			case POINT:
				oss << "Add points";
				break;
			case GLIDERGUN:
				oss << "Add a Gosper Glider Gun";
				break;
			case ALL:
				oss << "Add All of them";
				break;
			case DELETE:
				oss << "Delete Squares";
				break;
			case SQUARE:
			default:
				oss << "Add a square";
				break;
			}
			{
				auto p = sf::Mouse::getPosition(window);
				int x = p.x / SPACING;
				int y = p.y / SPACING;
				oss << ": " << x << ", " << y;
			}
		}
		else {
			oss << "Alive: " << active_squares.size();
		}
		someText.setString(oss.str());
		window.draw(quad);
		window.draw(someText);
		window.display();

		//sf::sleep(sf::seconds(.5));

		dead_list.sort();
		dead_list.unique();
		for (Square square : dead_list)
		{
			neighbors my_neighbors;
			square.get_neighbors(my_neighbors);

			int neighbor_count = square.get_active_neighbors(my_neighbors);
			if (neighbor_count == 3) {
				born_list.push_back(square);
			}
		}

		if (!pauseState) {
			for (Square square : born_list)
			{
				square.born();
				active_squares.push_back(square);
			}

			for (Square square : remove_list)
			{
				square.kill();
				active_squares.remove(square);
			}
		}
		
	}
	return 0;
}

void rotate(std::list<Square>& aList, int angle) {

	int centerX, centerY;
	int fixedAngle;
	int m00, m01, m10, m11;
	fixedAngle = ((angle + 45) / 90) * 90;

	if (aList.size() <= 1) {
		return;
	}

	if (fixedAngle == 0) {
		return;
	}

	if (fixedAngle == 90) {
		m00 = 0;
		m01 = -1;
		m10 = 1;
		m11 = 0;
	} else

	if (fixedAngle == 180) {
		m00 = -1;
		m01 = 0;
		m10 = 0;
		m11 = -1;
	} else

	if (fixedAngle == 270) {
		m00 = 0;
		m01 = 1;
		m10 = -1;
		m11 = 0;
	}

	getCenter(aList, centerX, centerY);

	translate(aList, -centerX, -centerY);
	for (auto& square : aList) {
		int tempX = square.x;
		int tempY = square.y;
		square.x = (m00 * tempX) + (m01 * tempY);
		square.y = (m10 * tempX) + (m11 * tempY);
	}
	translate(aList, centerX, centerY);
}

void translate(std::list<Square>& aList, int x, int y) {


	for (auto& square : aList) {
		square.x += x;
		square.y += y;
	}
}

void getCenter(std::list<Square>& aList, int& x, int& y)
{
	int minX = INT_MAX;
	int minY = INT_MAX;
	int maxX = INT_MIN;
	int maxY = INT_MIN;

	for (auto square : aList) {
		if (square.x < minX) { minX = square.x; }
		if (square.x > maxX) { maxX = square.x; }
		if (square.y < minY) { minY = square.y; }
		if (square.y > maxY) { maxY = square.y; }
	}

	x = minX + ((maxX - minX) / 2);
	y = minY + ((maxY - minY) / 2);
}

void addPoint(std::list<Square>& aList, int x, int y) {
	aList.push_back(Square(0 + x, 0 + y, true));
}

void addSquare(std::list<Square>& aList, int x, int y) {
	aList.push_back(Square(0 + x, 0 + y, true));
	aList.push_back(Square(1 + x, 0 + y, true));
	aList.push_back(Square(0 + x, 1 + y, true));
	aList.push_back(Square(1 + x, 1 + y, true));
}

void addAll(std::list<Square>& aList)
{
	for (auto x = 0; x < ROW_SQ; x++) {
		for (auto y = 0; y < COL_SQ; y++) {
		    aList.push_back(Square(x, y, true));
		}
	}
}

void addAcorn(std::list<Square>& aList, int x, int y) {
	aList.push_back(Square(0 + x, 0 + y, true));
	aList.push_back(Square(1 + x, 0 + y, true));
	aList.push_back(Square(1 + x, 2 + y, true));
	aList.push_back(Square(3 + x, 1 + y, true));
	aList.push_back(Square(4 + x, 0 + y, true));
	aList.push_back(Square(5 + x, 0 + y, true));
	aList.push_back(Square(6 + x, 0 + y, true));
}

void addGospersGliderGun(std::list<Square>& aList, int x, int y) {

	int xMax = 35;
	int yMax = 8;

	x -= xMax / 2;
	y -= yMax / 2;

	//Left Square
	aList.push_back(Square(0 + x, 4 + y, true));
	aList.push_back(Square(0 + x, 5 + y, true));
	aList.push_back(Square(1 + x, 4 + y, true));
	aList.push_back(Square(1 + x, 5 + y, true));

	aList.push_back(Square(13 + x, 2 + y, true));
	aList.push_back(Square(12 + x, 2 + y, true));
	aList.push_back(Square(11 + x, 3 + y, true));

	aList.push_back(Square(10 + x, 4 + y, true));
	aList.push_back(Square(10 + x, 5 + y, true));
	aList.push_back(Square(10 + x, 6 + y, true));

	aList.push_back(Square(11 + x, 7 + y, true));
	aList.push_back(Square(12 + x, 8 + y, true));
	aList.push_back(Square(13 + x, 8 + y, true));

	aList.push_back(Square(14 + x, 5 + y, true));

	aList.push_back(Square(15 + x, 3 + y, true));
	aList.push_back(Square(16 + x, 4 + y, true));
	aList.push_back(Square(16 + x, 5 + y, true));
	aList.push_back(Square(16 + x, 6 + y, true));
	aList.push_back(Square(15 + x, 7 + y, true));

	aList.push_back(Square(17 + x, 5 + y, true));


	aList.push_back(Square(24 + x, 0 + y, true));
	aList.push_back(Square(24 + x, 1 + y, true));
	aList.push_back(Square(22 + x, 1 + y, true));

	aList.push_back(Square(20 + x, 2 + y, true));
	aList.push_back(Square(21 + x, 2 + y, true));
	aList.push_back(Square(20 + x, 3 + y, true));
	aList.push_back(Square(21 + x, 3 + y, true));
	aList.push_back(Square(20 + x, 4 + y, true));
	aList.push_back(Square(21 + x, 4 + y, true));

	aList.push_back(Square(22 + x, 5 + y, true));
	aList.push_back(Square(24 + x, 5 + y, true));
	aList.push_back(Square(24 + x, 6 + y, true));

	aList.push_back(Square(34 + x, 2 + y, true));
	aList.push_back(Square(35 + x, 2 + y, true));
	aList.push_back(Square(34 + x, 3 + y, true));
	aList.push_back(Square(35 + x, 3 + y, true));
}

