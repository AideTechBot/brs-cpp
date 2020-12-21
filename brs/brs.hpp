// BRS header only library
// made by Aide33/AideTechBot/Commander Nick

#ifndef BRS_BRICKADIA_H
#define BRS_BRICKADIA_H

// TODO: don't do this
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <cstdint>
#include <climits>
#include <tuple>
#include <fstream>
#include <streambuf>
#include <istream>
#include <vector>
#include <optional>
#include <variant>
#include <codecvt>
#include <sstream>
#include <iomanip>
#include <memory>

#include <cassert>
#include <fstream>
#include <sstream>

#ifndef BRS_NO_INCLUDE_MINIZ
#define BRS_INCLUDED_MINIZ

/* MINIZ START */

#include "miniz.h"

/* MINIZ END */

#endif


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
		AddedDateTime = 4,
		Alpha5 = 8
	};

	const Version VERSION_WRITE = Version::Alpha5;

	/* Types */

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

	struct BrickOwner : User
	{
    int32_t brick_count;
	};

	struct Brick
	{
		uint32_t asset_name_index;
		std::tuple<uint32_t, uint32_t, uint32_t> size;
		std::tuple<int32_t, int32_t, int32_t> position;
		BRS::Direction direction;
		BRS::Rotation rotation;
		bool collision;
		bool visibility;
		uint32_t material_index;
		std::optional<Color> color;
		std::optional<uint32_t> color_index;
		std::optional<uint32_t> owner_index;
	};

	class Exception
		: public std::exception
	{
	public:
		Exception(const std::string& msg) : _msg(msg) {}
		const char* what() const noexcept { return _msg.c_str(); }
	private:
		std::string _msg;
	}; 

	/* Bit Readers/Writers */

	typedef std::vector<uint8_t> buffer;
	typedef std::vector<uint8_t>::iterator buffer_iterator;
	typedef std::tuple<int32_t, int32_t, int32_t> int32_t_vector;
	typedef std::tuple<uint32_t, uint32_t, uint32_t> uint32_t_vector;

	class BitReader
	{
		uint32_t pos_;
		buffer buf_;

		bool is_aligned() const;
	public:
		BitReader() = delete;
		BitReader(buffer buf);

		bool is_empty() const;
		void align();
		bool read_bit();
		uint32_t read_uint32(uint32_t max);
		uint32_t read_uint32_packed();
		int32_t read_int32_packed();
		int32_t_vector read_int32_vector_packed();
		uint32_t_vector read_uint32_vector_packed();
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
		std::vector<std::u16string> mods;
		std::vector<std::u16string> brickAssets;
		std::vector<Color> colors;
		std::vector<std::u16string> materials;
    std::variant<std::vector<BrickOwner>,std::vector<User>> brickOwners;
	};

	class Reader
	{

		std::ifstream reader_;

		static bool check_magic(std::istream&);

		static int32_t read_int32(std::istream&);
		static int64_t read_int64(std::istream&);
		static uint8_t read_uint8(std::istream&);
		static uint16_t read_uint16(std::istream&);
		static uint32_t read_uint32(std::istream&);
		static uint64_t read_uint64(std::istream&);
		static std::u16string read_string(std::istream&);

		template <typename T>
		static T read_number(buffer_iterator&);
		template <typename T>
		static std::vector<T> read_array(buffer_iterator& reader, T (*func)(buffer_iterator& r));
		static std::u16string read_string(buffer_iterator&);
		static Color read_color(buffer_iterator&);
		static User read_user(buffer_iterator&);
		static BrickOwner read_brick_owner(buffer_iterator&);
		static std::vector<Brick> read_bricks(BRS::Version, BitReader&, uint32_t, uint32_t, int32_t);
		static Brick read_brick(BRS::Version, BitReader&, uint32_t, uint32_t);

		buffer read_compressed();
		static buffer read_into_buffer(std::istream&, int32_t);

		std::optional<Header1> header1;
		std::optional<Header2> header2;
		std::optional<std::vector<Brick>> bricks;
	public:

		Reader(std::string filepath);

		BRS::Version version;
		uint32_t gameVersion;

		bool loadedHeader1();
		bool loadedHeader2();
		bool loadedBricks();

		Header1 getHeader1();
		Header2 getHeader2();
		std::vector<Brick> getBricks();

		void readHeader1();
		void readHeader2();
		void readBricks();
	};

#ifdef BRS_BRICKADIA_IMPLEMENTATION_H

	/* Implementation */

	/* Bit Readers/Writers */

	inline BitReader::BitReader(buffer buf) :
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

	inline int32_t_vector BitReader::read_int32_vector_packed()
	{
		return int32_t_vector{
			read_int32_packed(),
				read_int32_packed(),
				read_int32_packed()
		};
	}

	inline uint32_t_vector BitReader::read_uint32_vector_packed()
	{
		return uint32_t_vector{
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

	inline Reader::Reader(std::string filepath) : reader_(std::ifstream(filepath, std::ios::in | std::ios::binary))
	{
		if (!check_magic(reader_) || reader_.fail())
		{
			throw BRS::Exception("Invalid BRS file.");
		}

		uint16_t v = read_uint16(reader_);
		switch (v)
		{
		case 1:
			version = BRS::Version::Initial;
			break;
		case 2:
			version = BRS::Version::MaterialsStoredAsNames;
			break;
		case 3:
			version = BRS::Version::AddedOwnerData;
			break;
		case 4:
			version = BRS::Version::AddedDateTime;
			break;
    case 8:
      version = BRS::Version::Alpha5;
      break;
		default:
			throw BRS::Exception("Unsupported BRS version.");
		}

		gameVersion = version >= BRS::Version::Alpha5 ? read_int32(reader_): 3642;
	}

	inline bool Reader::check_magic(std::istream& reader)
	{
		return read_uint8(reader) == BRS::BRS_MAGIC[0] && read_uint8(reader) == BRS::BRS_MAGIC[1] && read_uint8(reader) == BRS::BRS_MAGIC[2];
	}

	inline int32_t Reader::read_int32(std::istream& reader)
	{
		int32_t a;
		reader.read(reinterpret_cast<char*>(&a), sizeof(a));
		return a;
	}

	inline int64_t Reader::read_int64(std::istream& reader)
	{
		int64_t a;
		reader.read(reinterpret_cast<char*>(&a), sizeof(a));
		return a;
	}

	inline uint8_t Reader::read_uint8(std::istream& reader)
	{
		uint8_t a;
		reader.read(reinterpret_cast<char*>(&a), sizeof(a));
		return a;
	}

	inline uint16_t Reader::read_uint16(std::istream& reader)
	{
		uint16_t a;
		reader.read(reinterpret_cast<char*>(&a), sizeof(a));
		return a;
	}

	inline uint32_t Reader::read_uint32(std::istream& reader)
	{
		uint32_t a;
		reader.read(reinterpret_cast<char*>(&a), sizeof(a));
		return a;
	}

	inline uint64_t Reader::read_uint64(std::istream& reader)
	{
		uint64_t a;
		reader.read(reinterpret_cast<char*>(&a), sizeof(a));
		return a;
	}

	inline std::u16string Reader::read_string(std::istream& reader)
	{
		int32_t size = read_int32(reader);
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
				throw BRS::Exception("Invalid UCS-2 data size.");
			}

			std::u16string ws;
			for (int i = 0; i < size / 2; i++)
			{
				char16_t a;
				reader.read(reinterpret_cast<char*>(&a), sizeof(a));
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
				reader.read(&a, sizeof(a));
				ws += a;
			}

			// TODO: add error handling here for when the last byte isn't 0
			char last;
			reader.read(&last, sizeof(last));

			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
			return convert.from_bytes(ws);
		}
	}

	template <typename T>
	inline T Reader::read_number(buffer_iterator& reader)
	{
		uint8_t a[sizeof(T)];
		for(int i = 0; i < sizeof(T); i++)
		{
			a[i] = *(reader++);
		}

		T r = 0;
		for(int i = 0; i < sizeof(T); i++)
		{
			r = (r << 8) + a[sizeof(T)-1-i];
		}
		return r;
	}
	

	inline std::u16string Reader::read_string(buffer_iterator& reader)
	{
		int32_t size = read_number<int32_t>(reader);
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
				throw BRS::Exception("Invalid UCS-2 data size.");
			}

			std::u16string ws;
			for (int i = 0; i < size / 2; i++)
			{
				ws += static_cast<wchar_t>(read_number<uint16_t>(reader));
			}
			return ws;
		}
		else
		{
			std::string ws;
			for (int i = 0; i < size - 1; i++)
			{
				char a;
				ws += static_cast<char>(read_number<uint8_t>(reader));
			}

			// TODO: add error handling here for when the last byte isn't 0
			read_number<uint8_t>(reader);

			std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
			return convert.from_bytes(ws);
		}
	}

	inline Color Reader::read_color(buffer_iterator& reader)
	{
		uint8_t b = read_number<uint8_t>(reader);
		uint8_t g = read_number<uint8_t>(reader);
		uint8_t r = read_number<uint8_t>(reader);
		uint8_t a = read_number<uint8_t>(reader);
		return Color(r, g, b, a);
	}

	inline User Reader::read_user(buffer_iterator& reader)
	{
		uint64_t a = read_number<uint64_t>(reader);
		uint64_t b = read_number<uint64_t>(reader);
		std::u16string name = read_string(reader);
		return { UUID(a,b), name };
	}

	inline BrickOwner Reader::read_brick_owner(buffer_iterator& reader)
	{
		uint64_t a = read_number<uint64_t>(reader);
		uint64_t b = read_number<uint64_t>(reader);
		std::u16string name = read_string(reader);
    int32_t bricks = read_number<int32_t>(reader);
		return { UUID(a,b), name, bricks };
	}

	template <typename T>
	inline std::vector<T> Reader::read_array(buffer_iterator& reader, T (*func)(buffer_iterator& r)){
		int32_t size = read_number<int32_t>(reader);
		std::vector<T> result;
		result.reserve(size);
		for(int i = 0; i < size; i++)
		{
			result.push_back(func(reader));
		}
		return result;
	}
	
	inline BRS::buffer Reader::read_into_buffer(std::istream& reader, int32_t size)
	{

		buffer buffer;
		buffer.reserve(size);
		for(int i = 0; i < size; i++)
		{
			char a;
			reader.read(&a, sizeof(a));
			buffer.push_back(static_cast<uint8_t>(a));
		}
		return buffer;
	}

	inline std::vector<Brick> Reader::read_bricks(BRS::Version version, BitReader& bit_reader, uint32_t brick_asset_count, uint32_t color_count, int32_t brick_count)
	{
		std::vector<Brick> result;
		result.reserve(brick_count);
		for(int32_t i = 0; i < brick_count; i++)
		{
			result.push_back(read_brick(version, bit_reader, brick_asset_count, color_count));
		}
		return result;
	}

	inline Brick Reader::read_brick(BRS::Version version, BitReader& r, uint32_t brick_asset_count, uint32_t color_count)
	{
		r.align();
		uint32_t asset_name_index = r.read_uint32(brick_asset_count > 2 ? brick_asset_count : 2);
		uint32_t_vector size = r.read_bit() ? r.read_uint32_vector_packed() : uint32_t_vector{ 0, 0, 0 };
		int32_t_vector position = r.read_int32_vector_packed();
		uint8_t orientation = static_cast<uint8_t>(r.read_uint32(24));

		BRS::Direction direction = static_cast<BRS::Direction>((orientation >> 2) % 6);
		BRS::Rotation rotation = static_cast<BRS::Rotation>(orientation & 0b11);
		
		bool collision = r.read_bit();
		bool visibility = r.read_bit();
		uint32_t material_index = r.read_bit() ? r.read_uint32_packed() : 1;
		std::optional<Color> color;
		std::optional<uint32_t> color_index;
		if(r.read_bit())
		{
			int bytes = 8;
			uint8_t blue = r.read_uint32(2 ^ bytes);
			uint8_t green = r.read_uint32(2 ^ bytes);
			uint8_t red = r.read_uint32(2 ^ bytes);
			uint8_t alpha = r.read_uint32(2 ^ bytes);
			color = std::optional<Color>{ Color(red, green, blue, alpha) };
		} else
		{
			color_index = std::optional<uint32_t>{ r.read_uint32(color_count) };
		}
		uint32_t oi = version >= BRS::Version::AddedOwnerData ? r.read_uint32_packed() : 0;
		std::optional<uint32_t> owner_index = oi ? std::optional<uint32_t>{oi - 1} : std::nullopt;
		Brick result = {
			asset_name_index,
			size,
			position,
			direction,
			rotation,
			collision,
			visibility,
			material_index,
			color,
			color_index,
			owner_index
		};
		return result;
	}

	inline BRS::buffer Reader::read_compressed()
	{
		// represents compressed and uncompressed block sizes
		uint32_t uncompressed_size = read_uint32(reader_);
		uint32_t compressed_size = read_uint32(reader_);

		// Throw error for weird compression/uncompression sizes
		if (compressed_size < 0 || uncompressed_size < 0 || compressed_size >= uncompressed_size)
		{
			throw BRS::Exception("Invalid compressed section size for block");
		}

		// No compressed data? return those bytes
		if(compressed_size == 0)
		{
			// Decompress if there is data to decompress
			return read_into_buffer(reader_, uncompressed_size);
		}

		uint8_t *src = new uint8_t[compressed_size];

		reader_.read(reinterpret_cast<char*>(src), compressed_size);

		mz_ulong us = static_cast<mz_ulong>(uncompressed_size);
		buffer result;
		result.reserve(uncompressed_size);
		int cmp_status = uncompress(result.data(), &us, src, static_cast<mz_ulong>(compressed_size));

		delete[] src;

		if(cmp_status != Z_OK)
		{
			throw Exception("Something went wrong with the decompression.");
		}

		return result;
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
		throw BRS::Exception("Header 1 not already loaded.");
	}

	inline Header2 Reader::getHeader2()
	{
		if (header2.has_value()) {
			return header2.value();
		}
		throw BRS::Exception("Header 2 not already loaded.");
	}

	inline std::vector<Brick> Reader::getBricks()
	{
		if (bricks.has_value()) {
			return bricks.value();
		}
		throw BRS::Exception("Bricks not already loaded.");
	}

	inline void Reader::readHeader1()
	{
		buffer header1_raw = read_compressed();
		buffer_iterator it = header1_raw.begin();

		std::u16string map = read_string(it);
		std::u16string author_name = read_string(it);
		std::u16string description = read_string(it);
		uint64_t f = read_number<int64_t>(it);
		uint64_t s = read_number<int64_t>(it);
		UUID author_id = UUID(f, s);
		std::optional<User> host;
    if(version >= BRS::Version::Alpha5) {
      std::u16string name = read_string(it);
      uint64_t a = read_number<int64_t>(it);
      uint64_t b = read_number<int64_t>(it);
      UUID id = UUID(a, b);
      host = User{ id, name };
    } else {
      host = std::nullopt;
    }
		std::optional<uint64_t> save_time;
		if (version >= BRS::Version::AddedDateTime) {
			save_time = read_number<int64_t>(it);
		}
		else
		{
			save_time = std::nullopt;
		}

		int32_t brick_count = read_number<int32_t>(it);

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
		buffer header2_raw = read_compressed();
		buffer_iterator it = header2_raw.begin();

		std::vector<std::u16string> mods = read_array<std::u16string>(it, &Reader::read_string);
		std::vector<std::u16string> brick_assets = read_array<std::u16string>(it, &Reader::read_string);
		std::vector<Color> colors = read_array<Color>(it, &Reader::read_color);
		std::vector<std::u16string> materials;
    if(version >= BRS::Version::MaterialsStoredAsNames) {
      materials = read_array<std::u16string>(it, &Reader::read_string);
    } else {
      materials.reserve(4);
      materials[0] = u"BMC_Hologram";
      materials[1] = u"BMC_Plastic";
      materials[2] = u"BMC_Glow";
      materials[3] = u"BMC_Metallic";
    }
    std::variant<std::vector<BrickOwner>, std::vector<User>> brick_owners;
    if(version >= BRS::Version::Alpha5) {
      brick_owners = read_array(it, read_brick_owner);
    } else {
      brick_owners = read_array(it, read_user);
    }

		Header2 result = {
		 mods,
		 brick_assets,
		 colors,
		 materials,
		 brick_owners
		};
		header2 = std::optional<Header2>{ result };
	}

	inline void Reader::readBricks()
	{
		buffer bricks_raw = read_compressed();

		buffer_iterator it = bricks_raw.begin();

		Header1 h1 = getHeader1();
		Header2 h2 = getHeader2();

		BitReader bitReader(bricks_raw);

		std::vector<Brick> result = read_bricks(version, bitReader, h2.brickAssets.size(), h2.colors.size(), h1.brickCount);
		bricks = std::optional<std::vector<Brick>>{ result };
	}
#endif /* BRS_BRICKADIA_IMPLEMENTATION_H */

}

#endif /* BRS_BRICKADIA_H */
