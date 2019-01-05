#include <iostream>

size_t maxX(size_t maxD);
size_t minX(size_t D);

int main() {
	size_t D;
	std::cin >> D;
	
	std::cout << "Maximum X for D <= " << D << " is " << maxX(D) << std::endl;
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
			if (lhs == rhs)
				return x;
			if (lhs < rhs)
				break;
		}
	}
}
