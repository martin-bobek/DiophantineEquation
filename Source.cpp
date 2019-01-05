#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class Sqrt {
public:
	size_t operator()(size_t square);

	static constexpr size_t NoRoot = std::numeric_limits<size_t>::max();
private:
	void expand();
	
	std::vector<size_t> squares = { 0 };
};

size_t maxX(size_t maxD);
size_t minX(size_t D);

int main() {
	Sqrt root;

	for (size_t i = 0; i < 100; i++) {
		size_t val = root(i);
		std::cout << i << " -> " << ((val == Sqrt::NoRoot) ? "[]" : std::to_string(val)) << std::endl;
	}
	
	size_t D;
	std::cin >> D;
	
	size_t x = maxX(D);
	std::cout << "Maximum X for D <= " << D << " is " << x << std::endl;
}

size_t maxX(size_t maxD) {
	size_t maxX = 0;
	size_t root = 2, square = root * root;

	for (size_t D = 2; D <= maxD; D++) {
		if (D == square) {
			root++;
			square = root * root;
			continue;
		}

		size_t X = minX(D);
		if (X > maxX)
			maxX = X;
	}

	return maxX;
}
size_t minX(size_t D) {
	for (size_t x = 2;; x++) {
		for (size_t y = 1;; y++) {
			size_t lhs = x * x;
			size_t rhs = D * y * y + 1;
			if (lhs == rhs) {
				std::cout << '(' << x << ")^2 - " << D << "*(" << y << ")^2 = 1" << std::endl;
				return x;
			}
			if (lhs < rhs)
				break;
		}
	}
}

size_t Sqrt::operator()(size_t square) {
	while (square > squares.back())
		expand();

	auto low = std::lower_bound(squares.begin(), squares.end(), square);

	return (*low == square) ? (low - squares.begin()) : NoRoot;
}
void Sqrt::expand() {
	size_t oldSize = squares.size();
	squares.reserve(2 * oldSize);
	squares.resize(squares.capacity());
	
	for (size_t i = oldSize; i < squares.size(); i++)
		squares[i] = i * i;
}
