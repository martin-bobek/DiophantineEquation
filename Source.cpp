#include <algorithm>
#include <cmath>
#include <intrin.h>
#include <iostream>
#include <vector>

class QuadIrrat {
public:
	QuadIrrat() {}
	QuadIrrat(signed num, unsigned den) : num(num), den(den) {}
	unsigned ExtractCoeff(unsigned rootFloor);
	void Invert(unsigned radicand);
	bool operator==(const QuadIrrat &rhs) const { return (num == rhs.num) && (den == rhs.den); }
	bool operator!=(const QuadIrrat &rhs) const { return !(*this == rhs); }
private:
	signed num;
	unsigned den;
};
class ContinuedRoot {
public:
	ContinuedRoot(unsigned maxRadicand);
	bool SetRadicand(unsigned radicand);
	void ComputeContFrac();
	unsigned FracPeriod() const { return (unsigned)coeffs.size() - 1; }
private:
	unsigned rootFloor() const;

	unsigned radicand, root;
	QuadIrrat blockStart;
	std::vector<unsigned> coeffs;
	std::vector<unsigned> squares;
};
class BigNum {
public:
	BigNum() : qwords(1) {}
	BigNum(uint64_t num) : qwords({ num }) {}
	BigNum(const BigNum &) = default;
	BigNum(BigNum &&) = default;
	BigNum &operator=(const BigNum &) = default;
	BigNum &operator=(BigNum &&) = default;
	BigNum operator+(const BigNum &rhs) const;
	BigNum &operator*=(uint64_t rhs);
	uint64_t operator/=(uint64_t rhs);
	bool operator==(uint64_t rhs) const { return (qwords.size() == 1) && (qwords[0] == rhs); }
	bool operator!=(uint64_t rhs) const { return !(*this == rhs); }
	friend std::ostream &operator<<(std::ostream &os, BigNum num);
private:
	static constexpr size_t SIZE = 0;
	BigNum(size_t size, size_t) : qwords(size) {}
	
	std::vector<uint64_t> qwords;
};
template <typename T>
class Rational {
public:
	Rational(T num, T den) : num(num), den(den) {}
	template <typename U> Rational &operator+=(U rhs);
	Rational &Inv();
	template <typename U> friend std::ostream &operator<<(std::ostream &os, const Rational<U> &value);
	
	size_t NumDigitSum() const { return num.DigitSum(); }
private:
	T num, den;
};

int main() {
	unsigned radicand = 13;
	ContinuedRoot root(radicand);
	root.SetRadicand(radicand);
	root.ComputeContFrac();
	std::cout << "Period of sqrt(" << radicand << ") is " << root.FracPeriod() << std::endl;
}

template <typename T> template <typename U> Rational<T> &Rational<T>::operator+=(U rhs) {
	T den_rhs = den;
	den_rhs *= rhs;
	num = den_rhs + num;
	return *this;
}
template <typename T> Rational<T> &Rational<T>::Inv() {
	std::swap(num, den);
	return *this;
}
template <typename T> std::ostream &operator<<(std::ostream &os, const Rational<T> &value) {
	return os << value.num << '/' << value.den;
}

size_t BigNum::operator/=(uint64_t rhs) {
	size_t norm = std::numeric_limits<uint64_t>::max() / rhs;
	rhs *= norm;
	*this *= norm;
	
	uint64_t remainder = 0;
	for (size_t i = qwords.size(); i-- > 0;) {
		__asm__ (
			"divq %[div]"
			: [qou] "+a" (qwords[i]), [rem] "+d" (remainder)
			: [div] "rm" (rhs)
		);
	}
	
	if (qwords.back() == 0 && qwords.size() > 1)
		qwords.pop_back();
	
	return remainder / norm;
}
BigNum BigNum::operator+(const BigNum &rhs) const {
	bool thisLonger = qwords.size() > rhs.qwords.size();
	const BigNum &longer = thisLonger ? *this : rhs;
	const BigNum &shorter = thisLonger ? rhs : *this;
	
	BigNum result(longer.qwords.size() + 1, SIZE);
	size_t index;
	uint8_t carry = 0;
	
	for (index = 0; index < shorter.qwords.size(); index++)
		carry = _addcarry_u64(carry, shorter.qwords[index], longer.qwords[index], (unsigned long long *)&result.qwords[index]);
	
	for (; index < longer.qwords.size(); index++)
		carry = _addcarry_u64(0, carry, longer.qwords[index], (unsigned long long *)&result.qwords[index]);
	
	if (carry != 0)
		result.qwords[index]++;
	else
		result.qwords.pop_back();
	
	return result;
}
BigNum &BigNum::operator*=(uint64_t rhs) {
	unsigned __int128 temp;
	uint64_t high;
	uint8_t carry = 0;
	
	qwords[0] = temp = (unsigned __int128)qwords[0] * rhs;
	high = temp >> 64;
	
	for (size_t i = 1; i < qwords.size(); i++) {
		temp = (unsigned __int128)qwords[i] * rhs;
		carry = _addcarry_u64(carry, temp, high, (unsigned long long *)&qwords[i]);
		high = temp >> 64;
	}
	
	high += carry;
	if (high != 0)
		qwords.push_back(high);
	
	return *this;
}
std::ostream &operator<<(std::ostream &os, BigNum num) {
	std::vector<char> digits;
	digits.reserve(20 * num.qwords.size());
	
	do {
		unsigned char remainder = (num /= 10);
		digits.push_back(remainder + '0');
	} while (num != 0);
	
	for (size_t i = digits.size(); i-- > 0;)
		os << digits[i];
	
	return os;
}

ContinuedRoot::ContinuedRoot(unsigned maxRadicand) : squares((unsigned)std::ceil(std::sqrt(maxRadicand))) {
	for (unsigned i = 1; i <= squares.size(); i++)
		squares[i - 1] = i * i;
}
bool ContinuedRoot::SetRadicand(unsigned radicand) {
	this->radicand = radicand;
	root = rootFloor();
	if (squares[root - 1] == radicand)
		return false;
	coeffs.clear();
	return true;
}
unsigned ContinuedRoot::rootFloor() const {
	auto it = std::upper_bound(squares.begin(), squares.end(), radicand);
	return (unsigned)(it - squares.begin());
}
void ContinuedRoot::ComputeContFrac() {
	QuadIrrat irrat(0, 1);
	
	do {
		coeffs.push_back(irrat.ExtractCoeff(root));
		irrat.Invert(radicand);

		if (coeffs.size() == 1)
			blockStart = irrat;
	} while ((coeffs.size() == 1) || (irrat != blockStart));
}

unsigned QuadIrrat::ExtractCoeff(unsigned rootFloor) {
	unsigned coeff = (rootFloor + num) / den;
	num -= coeff * den;
	return coeff;
}
void QuadIrrat::Invert(unsigned radicand) {
	den = (radicand - num * num) / den;
	num = -num;
}
