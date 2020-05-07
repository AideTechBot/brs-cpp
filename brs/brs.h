// BRS header only library
// made by Aide33/AideTechBot/Commander Nick

#ifndef BRS_BRICKADIA_H
#define BRS_BRICKADIA_H

#include <stdint.h>
#include <tuple>
#include <ostream>

#ifdef __cplusplus
extern "C"
{
#endif

/* Public Interface */

/* Types */

// TODO: do derives on all of the functions

enum Direction : uint8_t
{
	XPositive,
	XNegative,
	YPositive,
	YNegative,
	ZPositive,
	ZNegative
};

enum Rotation : uint8_t
{
	Deg0,
	Deg90,
	Deg180,
	Deg270
};

class ColorMode
{
	// TODO: Set(u32)
	
	// TODO: Custom(Color)
};

class Color
{
	uint32_t value_;
public:
	Color() = delete;
	explicit Color(const uint32_t& val);
	explicit Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	operator uint32_t() const;
	Color& operator=(const uint32_t& val);
	friend std::ostream& operator<<(std::ostream& stream, const Color& obj);

	void setRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	uint8_t r();
	uint8_t g();
	uint8_t b();
	uint8_t a();
};

struct User
{
	// TODO: uuid
	std::string name;
};

struct Brick
{
    uint32_t asset_name_index;
    std::tuple<uint32_t, uint32_t, uint32_t> size;
    std::tuple<int32_t, int32_t, int32_t> position;
	Direction direction;
};

/* Functions */
int add(int a, int b);
int sub(int a, int b);

#ifdef BRS_BRICKADIA_IMPLEMENTATION_H

/* Implementation */

/* Types */

inline Color::Color(const uint32_t& val): value_(val)
{
}

inline Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a):
	value_(
		static_cast<uint32_t>(r) |
		static_cast<uint32_t>(g) << 8 |
		static_cast<uint32_t>(b) << 16 |
		static_cast<uint32_t>(a) << 24
	)
{
}

inline Color::operator uint32_t() const
{
	return value_;
}

inline Color& Color::operator=(const uint32_t& val)
{
	this->value_ = value_;
	return *this;
}

inline std::ostream& operator<<(std::ostream& os, const Color& obj)
{
	return os << obj.value_;
}

inline void Color::setRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	value_ =
		static_cast<uint32_t>(r) |
		static_cast<uint32_t>(g) << 8 |
		static_cast<uint32_t>(b) << 16 |
		static_cast<uint32_t>(a) << 24;
}

/* Functions */

int add(int a, int b)
{
    return a + b;
}

int sub(int a, int b)
{
    return a - b;
}

#endif /* BRS_BRICKADIA_IMPLEMENTATION_H */

#ifdef __cplusplus
}  // extern C
#endif

#endif /* BRS_BRICKADIA_H */
