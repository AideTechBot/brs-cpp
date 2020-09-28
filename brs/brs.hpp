// BRS header only library
// made by Aide33/AideTechBot/Commander Nick

#ifndef BRS_BRICKADIA_H
#define BRS_BRICKADIA_H

#include <cstdint>
#include <tuple>
#include <fstream>
#include <vector>
#include <optional>
#include <codecvt>
#include <sstream>
#include <iomanip>

namespace BRS {

	/* Public Interface */

	/* Constants */

	const uint8_t BRS_MAGIC[3] = { 66, 82, 83 };
	const uint32_t BRS_MAX_VERSION = 4;
	const uint32_t BRS_LATEST_VERSION = 4;

	enum Version : uint16_t {
		Initial = 1,
		MaterialsStoredAsNames = 2,
		AddedOwnerData = 3,
		AddedDateTime = 4
	};

	const Version VERSION_WRITE = Version::AddedDateTime;

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

		void set_RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
		uint8_t r();
		uint8_t g();
		uint8_t b();
		uint8_t a();
	};

	class UUID
	{
		uint8_t bytes_[16];
	public:
		UUID(uint64_t f, uint64_t s);
		std::string toString() const;
		friend std::ostream& operator<<(std::ostream& stream, const UUID& obj);
		friend bool operator==(const UUID& lhs, const UUID& rhs);
		friend bool operator!=(const UUID& lhs, const UUID& rhs);
	};

	struct User
	{
		UUID uuid;
		std::u16string name;
	};

	struct Brick
	{
		uint32_t asset_name_index;
		std::tuple<uint32_t, uint32_t, uint32_t> size;
		std::tuple<int32_t, int32_t, int32_t> position;
		Direction direction;
	};

	/* Bit Readers/Writers */

	class BitReader
	{
		uint32_t pos_;
		std::vector<uint8_t> buf_;

		bool is_aligned() const;
	public:
		BitReader() = delete;
		BitReader(std::vector<uint8_t> buf);

		bool is_empty() const;
		void align();
		bool read_bit();
		uint32_t read_uint32(uint32_t max);
		uint32_t read_uint32_packed();
		int32_t read_int32_packed();
		std::tuple<int32_t, int32_t, int32_t> read_int32_vector_packed();
		std::tuple<uint32_t, uint32_t, uint32_t> read_uint32_vector_packed();
	};

	class Bricks {
	public:
		Version version;
		BitReader bitReader;
		uint32_t brickAssetNum;
		uint32_t colorNum;
		int32_t brickCount;
		int32_t index;
	};

	struct Header1 {
		std::u16string map;
		User author;
		std::u16string description;
		std::optional<User> host;
		std::optional<uint64_t> saveTime;
		int32_t brickCount;
	};

	struct Header2 {
		std::vector<std::string> mods;
		std::vector<std::string> brickAssets;
		std::vector<Color> colors;
		std::vector<std::string> materials;
		std::vector<User> brickOwners;
	};

	class Reader
	{
		std::ifstream reader_;
		bool checkMagic();
		int32_t read_int32();
		int64_t read_int64();
		uint8_t read_uint8();
		uint16_t read_uint16();
		uint32_t read_uint32();
		uint64_t read_uint64();
		std::u16string read_string();

		std::optional<Header1> header1;
		std::optional<Header2> header2;
		std::optional<Bricks> bricks;
	public:

		Reader(std::string filepath);

		Version version;
		uint32_t gameVersion;

		bool loadedHeader1();
		bool loadedHeader2();
		bool loadedBricks();

		Header1 getHeader1();
		Header2 getHeader2();
		Bricks getBricks();

		void readHeader1();
		void readHeader2();
		void readBricks();
	};

#ifdef BRS_BRICKADIA_IMPLEMENTATION_H

	/* Implementation */

	/* Bit Readers/Writers */

	inline BitReader::BitReader(std::vector<uint8_t> buf) :
		pos_(0),
		buf_(std::move(buf))
	{}

	inline bool BitReader::is_aligned() const
	{
		return (pos_ % 8) == 0;
	}

	inline bool BitReader::is_empty() const
	{
		return pos_ >= buf_.size() * 8;
	}

	inline void BitReader::align()
	{
		pos_ = (pos_ + 7) & ~0x07;
	}

	inline bool BitReader::read_bit()
	{
		bool bit = (buf_[pos_ >> 3] & (1 << (pos_ & 7))) != 0;
		pos_++;
		return bit;
	}

	inline uint32_t BitReader::read_uint32(uint32_t max)
	{
		uint32_t value = 0;
		uint32_t mask = 1;

		while ((value + mask) < max && mask != 0)
		{
			if (read_bit())
			{
				value |= mask;
			}
			mask *= 2;
		}

		return value;
	}

	inline uint32_t BitReader::read_uint32_packed()
	{
		uint32_t value = 0;

		for (uint32_t i = 0; i < 5; i++) {
			bool has_next = read_bit();
			uint32_t part = 0;
			for (uint32_t bit_shift = 0; bit_shift < 7; bit_shift++) {
				part |= static_cast<uint32_t>(read_bit()) << bit_shift;
			}
			value |= part << (7 * i);
			if (!has_next) {
				break;
			}
		}

		return value;
	}

	inline int32_t BitReader::read_int32_packed()
	{
		uint32_t value = read_uint32_packed();
		return static_cast<int32_t>(value);
	}

	inline std::tuple<int32_t, int32_t, int32_t> BitReader::read_int32_vector_packed()
	{
		return std::tuple<int32_t, int32_t, int32_t>{
			read_int32_packed(),
				read_int32_packed(),
				read_int32_packed()
		};
	}

	inline std::tuple<uint32_t, uint32_t, uint32_t> BitReader::read_uint32_vector_packed()
	{
		return std::tuple<uint32_t, uint32_t, uint32_t>{
			read_uint32_packed(),
				read_uint32_packed(),
				read_uint32_packed()
		};
	}

	/* Types */

	inline Color::Color(const uint32_t& val) : value_(val)
	{
	}

	inline Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) :
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

	inline void Color::set_RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		value_ =
			static_cast<uint32_t>(r) |
			static_cast<uint32_t>(g) << 8 |
			static_cast<uint32_t>(b) << 16 |
			static_cast<uint32_t>(a) << 24;
	}

	UUID::UUID(uint64_t f, uint64_t s)
	{
		bytes_[7] = (f >> 56) & 0xFF;
		bytes_[6] = (f >> 48) & 0xFF;
		bytes_[5] = (f >> 40) & 0xFF;
		bytes_[4] = (f >> 32) & 0xFF;
		bytes_[3] = (f >> 24) & 0xFF;
		bytes_[2] = (f >> 16) & 0xFF;
		bytes_[1] = (f >> 8) & 0xFF;
		bytes_[0] = f & 0xFF;

		bytes_[15] = (s >> 56) & 0xFF;
		bytes_[14] = (s >> 48) & 0xFF;
		bytes_[13] = (s >> 40) & 0xFF;
		bytes_[12] = (s >> 32) & 0xFF;
		bytes_[11] = (s >> 24) & 0xFF;
		bytes_[10] = (s >> 16) & 0xFF;
		bytes_[9] = (s >> 8) & 0xFF;
		bytes_[8] = s & 0xFF;
	}

	// TODO: optimize this by calculating it only once
	inline std::string UUID::toString() const
	{
		uint8_t temp[16];
		for (int chunk = 0; chunk < 4; chunk++)
		{
			for (int i = 0; i < 2; i++) {
				temp[chunk * 4 + i] = bytes_[chunk * 4 + 3 - i];
				temp[chunk * 4 + 3 - i] = bytes_[chunk * 4 + i];
			}
		}

		std::stringstream stream;
		// I'd rather copy paste then make a loop to fit the -'s at the right places
		stream << std::hex << std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[0])
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[1])
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[2])
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[3])
			<< "-"
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[4])
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[5])
			<< "-"
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[6])
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[7])
			<< "-"
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[8])
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[9])
			<< "-"
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[10])
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[11])
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[12])
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[13])
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[14])
			<< std::setfill('0') << std::setw(2) << std::right << static_cast<int>(temp[15]);

		return stream.str();
	}

	inline bool operator==(const UUID& lhs, const UUID& rhs)
	{
		for (int i = 0; i < 16; i++)
		{
			if (lhs.bytes_[i] != rhs.bytes_[i])
			{
				return false;
			}
		}
		return true;
	}

	inline bool operator!=(const UUID& lhs, const UUID& rhs)
	{
		for (int i = 0; i < 16; i++)
		{
			if (lhs.bytes_[i] != rhs.bytes_[i])
			{
				return true;
			}
		}
		return false;
	}

	inline std::ostream& operator<<(std::ostream& strm, const UUID& a) {
		return strm << a.toString();
	}

	/* Functions */

	/* BRS Readers */

	Reader::Reader(std::string filepath) : reader_(std::ifstream(filepath, std::ios::in | std::ios::binary))
	{
		if (!checkMagic() || reader_.fail())
		{
			throw "Invalid BRS file.";
		}

		uint16_t v = read_uint16();
		switch (v)
		{
		case 1:
			version = Initial;
			break;
		case 2:
			version = MaterialsStoredAsNames;
			break;
		case 3:
			version = AddedOwnerData;
			break;
		case 4:
			version = AddedDateTime;
			break;
		default:
			throw "Unsupported BRS version.";
		}

		gameVersion = 3642;
	}

	inline bool Reader::checkMagic()
	{
		return read_uint8() == BRS_MAGIC[0] && read_uint8() == BRS_MAGIC[1] && read_uint8() == BRS_MAGIC[2];
	}

	inline int32_t Reader::read_int32()
	{
		int32_t a;
		reader_.read(reinterpret_cast<char*>(&a), sizeof(a));
		return a;
	}

	inline int64_t Reader::read_int64()
	{
		int64_t a;
		reader_.read(reinterpret_cast<char*>(&a), sizeof(a));
		return a;
	}

	inline uint8_t Reader::read_uint8()
	{
		uint8_t a;
		reader_.read(reinterpret_cast<char*>(&a), sizeof(a));
		return a;
	}

	uint16_t Reader::read_uint16()
	{
		uint16_t a;
		reader_.read(reinterpret_cast<char*>(&a), sizeof(a));
		return a;
	}

	inline uint32_t Reader::read_uint32()
	{
		uint32_t a;
		reader_.read(reinterpret_cast<char*>(&a), sizeof(a));
		return a;
	}

	inline uint64_t Reader::read_uint64()
	{
		uint64_t a;
		reader_.read(reinterpret_cast<char*>(&a), sizeof(a));
		return a;
	}

	inline std::u16string Reader::read_string()
	{
		int32_t size = read_int32();
		bool isUCS2 = false;
		if (size < 0)
		{
			isUCS2 = true;
			size = -size;
		}

		if (isUCS2)
		{
			if (size % 2 != 0)
			{
				throw "Invalid UCS-2 data size.";
			}

			std::u16string ws;
			for (int i = 0; i < size / 2; i++)
			{
				char16_t a;
				reader_.read(reinterpret_cast<char*>(&a), sizeof(a));
				ws += a;
			}
			return ws;
		}
		else
		{
			std::string ws;
			for (int i = 0; i < size - 1; i++)
			{
				char a;
				reader_.read(&a, sizeof(a));
				ws += a;
			}

			// TODO: add error handling here for when the last byte isn't 0
			char last;
			reader_.read(&last, sizeof(last));

			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
			return convert.from_bytes(ws);
		}
	}

	inline bool Reader::loadedHeader1()
	{
		return header1.has_value();
	}

	inline bool Reader::loadedHeader2()
	{
		return header2.has_value();
	}

	inline bool Reader::loadedBricks()
	{
		return bricks.has_value();
	}

	inline Header1 Reader::getHeader1()
	{
		if (header1.has_value()) {
			return header1.value();
		}
		throw "Header 1 not already loaded.";
	}

	inline Header2 Reader::getHeader2()
	{
		if (header2.has_value()) {
			return header2.value();
		}
		throw "Header 2 not already loaded.";
	}

	inline Bricks Reader::getBricks()
	{
		if (bricks.has_value()) {
			return bricks.value();
		}
		throw "Bricks not already loaded.";
	}

	inline void Reader::readHeader1()
	{
		// represents compressed and uncompressed block sizes
		uint32_t uncompressed_size = read_int32();
		uint32_t compressed_size = read_int32();

		std::u16string map = read_string();
		std::u16string author_name = read_string();
		std::u16string description = read_string();
		uint64_t f = read_int64();
		uint64_t s = read_int64();
		UUID author_id = UUID(f, s);
		std::optional<User> host = std::nullopt;
		std::optional<uint64_t> save_time;
		if (version >= Version::AddedDateTime) {
			save_time = read_int64();
		}
		else
		{
			save_time = std::nullopt;
		}

		int32_t brick_count = read_int32();

		Header1 result = {
			map,
		{ author_id, author_name },
			description,
			host,
			save_time,
			brick_count
		};
		header1 = std::optional<Header1>{ result };
	}

	inline void Reader::readHeader2()
	{
	}

	inline void Reader::readBricks()
	{

	}

};

#endif /* BRS_BRICKADIA_IMPLEMENTATION_H */

#endif /* BRS_BRICKADIA_H */
