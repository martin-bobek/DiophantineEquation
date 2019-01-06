#include <intrin.h>
#include <iostream>
#include <vector>

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

int main() {

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