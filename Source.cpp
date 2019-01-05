#include <array>
#include <iostream>

size_t minX(size_t D);

int main() {
	std::array<size_t, 5> Ds = { 2, 3, 5, 6, 7 };

	for (auto D : Ds)
		std::cout << "(D = " << D << ") -> (X = " << minX(D) << ')' << std::endl;
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
