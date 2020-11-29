// BRS header only library
// made by Aide33/AideTechBot/Commander Nick

#ifndef BRS_BRICKADIA_H
#define BRS_BRICKADIA_H

static_assert(std::is_same_v<std::uint8_t, char> ||
	std::is_same_v<std::uint8_t, unsigned char>,
	"This library requires std::uint8_t to be implemented as char or unsigned char.");

#include <cstdint>
#include <tuple>
#include <fstream>
#include <streambuf>
#include <istream>
#include <vector>
#include <optional>
#include <codecvt>
#include <sstream>
#include <iomanip>
#include <memory>

// ZSTR implementation is copied below but its modified to use miniz
// I modified it on top of that so I didn't add a toggle cuz it wouldn't work.

/* STRICT_FSTREAM START */

#ifndef __STRICT_FSTREAM_HPP
#define __STRICT_FSTREAM_HPP

#include <cassert>
#include <fstream>
#include <string>

/**
 * This namespace defines wrappers for std::ifstream, std::ofstream, and
 * std::fstream objects. The wrappers perform the following steps:
 * - check the open modes make sense
 * - check that the call to open() is successful
 * - (for input streams) check that the opened file is peek-able
 * - turn on the badbit in the exception mask
 */
namespace strict_fstream
{

	/// Overload of error-reporting function, to enable use with VS.
	/// Ref: http://stackoverflow.com/a/901316/717706
	static std::string strerror()
	{
		std::string buff(80, '\0');
#ifdef _WIN32
		if (strerror_s(&buff[0], buff.size(), errno) != 0)
		{
			buff = "Unknown error";
		}
#elif (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE
		// XSI-compliant strerror_r()
		if (strerror_r(errno, &buff[0], buff.size()) != 0)
		{
			buff = "Unknown error";
		}
#else
		// GNU-specific strerror_r()
		auto p = strerror_r(errno, &buff[0], buff.size());
		std::string tmp(p, std::strlen(p));
		std::swap(buff, tmp);
#endif
		buff.resize(buff.find('\0'));
		return buff;
	}

	/// Exception class thrown by failed operations.
	class Exception
		: public std::exception
	{
	public:
		Exception(const std::string& msg) : _msg(msg) {}
		const char* what() const noexcept { return _msg.c_str(); }
	private:
		std::string _msg;
	}; // class Exception

	namespace detail
	{

		struct static_method_holder
		{
			static std::string mode_to_string(std::ios_base::openmode mode)
			{
				static const int n_modes = 6;
				static const std::ios_base::openmode mode_val_v[n_modes] =
				{
					std::ios_base::in,
					std::ios_base::out,
					std::ios_base::app,
					std::ios_base::ate,
					std::ios_base::trunc,
					std::ios_base::binary
				};

				static const char* mode_name_v[n_modes] =
				{
					"in",
					"out",
					"app",
					"ate",
					"trunc",
					"binary"
				};
				std::string res;
				for (int i = 0; i < n_modes; ++i)
				{
					if (mode & mode_val_v[i])
					{
						res += (!res.empty() ? "|" : "");
						res += mode_name_v[i];
					}
				}
				if (res.empty()) res = "none";
				return res;
			}
			static void check_mode(const std::string& filename, std::ios_base::openmode mode)
			{
				if ((mode & std::ios_base::trunc) && !(mode & std::ios_base::out))
				{
					throw Exception(std::string("strict_fstream: open('") + filename + "'): mode error: trunc and not out");
				}
				else if ((mode & std::ios_base::app) && !(mode & std::ios_base::out))
				{
					throw Exception(std::string("strict_fstream: open('") + filename + "'): mode error: app and not out");
				}
				else if ((mode & std::ios_base::trunc) && (mode & std::ios_base::app))
				{
					throw Exception(std::string("strict_fstream: open('") + filename + "'): mode error: trunc and app");
				}
			}
			static void check_open(std::ios* s_p, const std::string& filename, std::ios_base::openmode mode)
			{
				if (s_p->fail())
				{
					throw Exception(std::string("strict_fstream: open('")
						+ filename + "'," + mode_to_string(mode) + "): open failed: "
						+ strerror());
				}
			}
			static void check_peek(std::istream* is_p, const std::string& filename, std::ios_base::openmode mode)
			{
				bool peek_failed = true;
				try
				{
					is_p->peek();
					peek_failed = is_p->fail();
				}
				catch (std::ios_base::failure e) {}
				if (peek_failed)
				{
					throw Exception(std::string("strict_fstream: open('")
						+ filename + "'," + mode_to_string(mode) + "): peek failed: "
						+ strerror());
				}
				is_p->clear();
			}
		}; // struct static_method_holder

	} // namespace detail

	class ifstream
		: public std::ifstream
	{
	public:
		ifstream() = default;
		ifstream(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
		{
			open(filename, mode);
		}
		void open(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
		{
			mode |= std::ios_base::in;
			exceptions(std::ios_base::badbit);
			detail::static_method_holder::check_mode(filename, mode);
			std::ifstream::open(filename, mode);
			detail::static_method_holder::check_open(this, filename, mode);
			detail::static_method_holder::check_peek(this, filename, mode);
		}
	}; // class ifstream

	class ofstream
		: public std::ofstream
	{
	public:
		ofstream() = default;
		ofstream(const std::string& filename, std::ios_base::openmode mode = std::ios_base::out)
		{
			open(filename, mode);
		}
		void open(const std::string& filename, std::ios_base::openmode mode = std::ios_base::out)
		{
			mode |= std::ios_base::out;
			exceptions(std::ios_base::badbit);
			detail::static_method_holder::check_mode(filename, mode);
			std::ofstream::open(filename, mode);
			detail::static_method_holder::check_open(this, filename, mode);
		}
	}; // class ofstream

	class fstream
		: public std::fstream
	{
	public:
		fstream() = default;
		fstream(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
		{
			open(filename, mode);
		}
		void open(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
		{
			if (!(mode & std::ios_base::out)) mode |= std::ios_base::in;
			exceptions(std::ios_base::badbit);
			detail::static_method_holder::check_mode(filename, mode);
			std::fstream::open(filename, mode);
			detail::static_method_holder::check_open(this, filename, mode);
			detail::static_method_holder::check_peek(this, filename, mode);
		}
	}; // class fstream

} // namespace strict_fstream

#endif

/* STRICT_FSTREAM END */

/* ZSTR START */

//---------------------------------------------------------
// Copyright 2015 Ontario Institute for Cancer Research
// Written by Matei David (matei@cs.toronto.edu)
//---------------------------------------------------------

// Reference:
// http://stackoverflow.com/questions/14086417/how-to-write-custom-input-stream-in-c

#include <cassert>
#include <fstream>
#include <sstream>

#ifndef BRS_NO_INCLUDE_MINIZ
#define BRS_INCLUDED_MINIZ

/* MINIZ START */

#include "miniz.h"

/* MINIZ END */

#endif

namespace zstr
{

	/// Exception class thrown by failed zlib operations.
	class Exception
		: public std::exception
	{
	public:
		Exception(z_stream* zstrm_p, int ret)
			: _msg("zlib: ")
		{
			switch (ret)
			{
			case Z_STREAM_ERROR:
				_msg += "Z_STREAM_ERROR: ";
				break;
			case Z_DATA_ERROR:
				_msg += "Z_DATA_ERROR: ";
				break;
			case Z_MEM_ERROR:
				_msg += "Z_MEM_ERROR: ";
				break;
			case Z_VERSION_ERROR:
				_msg += "Z_VERSION_ERROR: ";
				break;
			case Z_BUF_ERROR:
				_msg += "Z_BUF_ERROR: ";
				break;
			default:
				std::ostringstream oss;
				oss << ret;
				_msg += "[" + oss.str() + "]: ";
				break;
			}
			_msg += zstrm_p->msg;
		}
		Exception(const std::string msg) : _msg(msg) {}
		const char* what() const noexcept { return _msg.c_str(); }
	private:
		std::string _msg;
	}; // class Exception

	namespace detail
	{

		class z_stream_wrapper
			: public z_stream
		{
		public:
			z_stream_wrapper(bool _is_input = true, int _level = Z_DEFAULT_COMPRESSION)
				: is_input(_is_input)
			{
				this->zalloc = Z_NULL;
				this->zfree = Z_NULL;
				this->opaque = Z_NULL;
				int ret;
				if (is_input)
				{
					this->avail_in = 0;
					this->next_in = Z_NULL;
					// i changed the last arg here(window bits) from 15 + 31, to the default value omegalul
					// idk why he hardcoded it, i found another zlib wrapper that did it this way
					ret = inflateInit2(this, Z_DEFAULT_WINDOW_BITS);
				}
				else
				{
					ret = deflateInit2(this, _level, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
				}
				if (ret != Z_OK) throw Exception(this, ret);
			}
			~z_stream_wrapper()
			{
				if (is_input)
				{
					inflateEnd(this);
				}
				else
				{
					deflateEnd(this);
				}
			}
		private:
			bool is_input;
		}; // class z_stream_wrapper

	} // namespace detail

	class istreambuf
		: public std::streambuf
	{
	public:
		istreambuf(std::streambuf* _sbuf_p,
			std::size_t _buff_size = default_buff_size, bool _auto_detect = true)
			: sbuf_p(_sbuf_p),
			zstrm_p(nullptr),
			buff_size(_buff_size),
			auto_detect(_auto_detect),
			auto_detect_run(false),
			is_text(false)
		{
			assert(sbuf_p);
			in_buff = new char[buff_size];
			in_buff_start = in_buff;
			in_buff_end = in_buff;
			out_buff = new char[buff_size];
			setg(out_buff, out_buff, out_buff);
		}

		istreambuf(const istreambuf&) = delete;
		istreambuf(istreambuf&&) = default;
		istreambuf& operator = (const istreambuf&) = delete;
		istreambuf& operator = (istreambuf&&) = default;

		virtual ~istreambuf()
		{
			delete[] in_buff;
			delete[] out_buff;
			if (zstrm_p) delete zstrm_p;
		}

		virtual std::streambuf::int_type underflow()
		{
			if (this->gptr() == this->egptr())
			{
				// pointers for free region in output buffer
				char* out_buff_free_start = out_buff;
				do
				{
					// read more input if none available
					if (in_buff_start == in_buff_end)
					{
						// empty input buffer: refill from the start
						in_buff_start = in_buff;
						std::streamsize sz = sbuf_p->sgetn(in_buff, buff_size);
						in_buff_end = in_buff + sz;
						if (in_buff_end == in_buff_start) break; // end of input
					}
					// auto detect if the stream contains text or deflate data
					if (auto_detect && !auto_detect_run)
					{
						auto_detect_run = true;
						unsigned char b0 = *reinterpret_cast<unsigned char*>(in_buff_start);
						unsigned char b1 = *reinterpret_cast<unsigned char*>(in_buff_start + 1);
						// Ref:
						// http://en.wikipedia.org/wiki/Gzip
						// http://stackoverflow.com/questions/9050260/what-does-a-zlib-header-look-like
						is_text = !(in_buff_start + 2 <= in_buff_end
							&& ((b0 == 0x1F && b1 == 0x8B)         // gzip header
								|| (b0 == 0x78 && (b1 == 0x01      // zlib header
									|| b1 == 0x9C
									|| b1 == 0xDA))));
					}
					if (is_text)
					{
						// simply swap in_buff and out_buff, and adjust pointers
						assert(in_buff_start == in_buff);
						std::swap(in_buff, out_buff);
						out_buff_free_start = in_buff_end;
						in_buff_start = in_buff;
						in_buff_end = in_buff;
					}
					else
					{
						// run inflate() on input
						if (!zstrm_p) zstrm_p = new detail::z_stream_wrapper(true);
						zstrm_p->next_in = reinterpret_cast<decltype(zstrm_p->next_in)>(in_buff_start);
						zstrm_p->avail_in = in_buff_end - in_buff_start;
						zstrm_p->next_out = reinterpret_cast<decltype(zstrm_p->next_out)>(out_buff_free_start);
						zstrm_p->avail_out = (out_buff + buff_size) - out_buff_free_start;
						int ret = inflate(zstrm_p, Z_NO_FLUSH);
						// process return code
						if (ret != Z_OK && ret != Z_STREAM_END) throw Exception(zstrm_p, ret);
						// update in&out pointers following inflate()
						in_buff_start = reinterpret_cast<decltype(in_buff_start)>(const_cast<unsigned char*>(zstrm_p->next_in));
						in_buff_end = in_buff_start + zstrm_p->avail_in;
						out_buff_free_start = reinterpret_cast<decltype(out_buff_free_start)>(zstrm_p->next_out);
						assert(out_buff_free_start + zstrm_p->avail_out == out_buff + buff_size);
						// if stream ended, deallocate inflator
						if (ret == Z_STREAM_END)
						{
							delete zstrm_p;
							zstrm_p = nullptr;
						}
					}
				} while (out_buff_free_start == out_buff);
				// 2 exit conditions:
				// - end of input: there might or might not be output available
				// - out_buff_free_start != out_buff: output available
				this->setg(out_buff, out_buff, out_buff_free_start);
			}
			return this->gptr() == this->egptr()
				? traits_type::eof()
				: traits_type::to_int_type(*this->gptr());
		}
	private:
		std::streambuf* sbuf_p;
		char* in_buff;
		char* in_buff_start;
		char* in_buff_end;
		char* out_buff;
		detail::z_stream_wrapper* zstrm_p;
		std::size_t buff_size;
		bool auto_detect;
		bool auto_detect_run;
		bool is_text;

		static const std::size_t default_buff_size = (std::size_t)1 << 20;
	}; // class istreambuf

	class ostreambuf
		: public std::streambuf
	{
	public:
		ostreambuf(std::streambuf* _sbuf_p,
			std::size_t _buff_size = default_buff_size, int _level = Z_DEFAULT_COMPRESSION)
			: sbuf_p(_sbuf_p),
			zstrm_p(new detail::z_stream_wrapper(false, _level)),
			buff_size(_buff_size)
		{
			assert(sbuf_p);
			in_buff = new char[buff_size];
			out_buff = new char[buff_size];
			setp(in_buff, in_buff + buff_size);
		}

		ostreambuf(const ostreambuf&) = delete;
		ostreambuf(ostreambuf&&) = default;
		ostreambuf& operator = (const ostreambuf&) = delete;
		ostreambuf& operator = (ostreambuf&&) = default;

		int deflate_loop(int flush)
		{
			while (true)
			{
				zstrm_p->next_out = reinterpret_cast<decltype(zstrm_p->next_out)>(out_buff);
				zstrm_p->avail_out = buff_size;
				int ret = deflate(zstrm_p, flush);
				if (ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR) throw Exception(zstrm_p, ret);
				std::streamsize sz = sbuf_p->sputn(out_buff, reinterpret_cast<decltype(out_buff)>(zstrm_p->next_out) - out_buff);
				if (sz != reinterpret_cast<decltype(out_buff)>(zstrm_p->next_out) - out_buff)
				{
					// there was an error in the sink stream
					return -1;
				}
				if (ret == Z_STREAM_END || ret == Z_BUF_ERROR || sz == 0)
				{
					break;
				}
			}
			return 0;
		}

		virtual ~ostreambuf()
		{
			// flush the zlib stream
			//
			// NOTE: Errors here (sync() return value not 0) are ignored, because we
			// cannot throw in a destructor. This mirrors the behaviour of
			// std::basic_filebuf::~basic_filebuf(). To see an exception on error,
			// close the ofstream with an explicit call to close(), and do not rely
			// on the implicit call in the destructor.
			//
			sync();
			delete[] in_buff;
			delete[] out_buff;
			delete zstrm_p;
		}
		virtual std::streambuf::int_type overflow(std::streambuf::int_type c = traits_type::eof())
		{
			zstrm_p->next_in = reinterpret_cast<decltype(zstrm_p->next_in)>(pbase());
			zstrm_p->avail_in = pptr() - pbase();
			while (zstrm_p->avail_in > 0)
			{
				int r = deflate_loop(Z_NO_FLUSH);
				if (r != 0)
				{
					setp(nullptr, nullptr);
					return traits_type::eof();
				}
			}
			setp(in_buff, in_buff + buff_size);
			return traits_type::eq_int_type(c, traits_type::eof()) ? traits_type::eof() : sputc(c);
		}
		virtual int sync()
		{
			// first, call overflow to clear in_buff
			overflow();
			if (!pptr()) return -1;
			// then, call deflate asking to finish the zlib stream
			zstrm_p->next_in = nullptr;
			zstrm_p->avail_in = 0;
			if (deflate_loop(Z_FINISH) != 0) return -1;
			deflateReset(zstrm_p);
			return 0;
		}
	private:
		std::streambuf* sbuf_p;
		char* in_buff;
		char* out_buff;
		detail::z_stream_wrapper* zstrm_p;
		std::size_t buff_size;

		static const std::size_t default_buff_size = (std::size_t)1 << 20;
	}; // class ostreambuf

	class istream
		: public std::istream
	{
	public:
		istream(std::istream& is)
			: std::istream(new istreambuf(is.rdbuf()))
		{
			exceptions(std::ios_base::badbit);
		}
		explicit istream(std::streambuf* sbuf_p)
			: std::istream(new istreambuf(sbuf_p))
		{
			exceptions(std::ios_base::badbit);
		}
		virtual ~istream()
		{
			delete rdbuf();
		}
	}; // class istream

	class ostream
		: public std::ostream
	{
	public:
		ostream(std::ostream& os)
			: std::ostream(new ostreambuf(os.rdbuf()))
		{
			exceptions(std::ios_base::badbit);
		}
		explicit ostream(std::streambuf* sbuf_p)
			: std::ostream(new ostreambuf(sbuf_p))
		{
			exceptions(std::ios_base::badbit);
		}
		virtual ~ostream()
		{
			delete rdbuf();
		}
	}; // class ostream

	namespace detail
	{

		template < typename FStream_Type >
		struct strict_fstream_holder
		{
			strict_fstream_holder(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
				: _fs(filename, mode)
			{}
			FStream_Type _fs;
		}; // class strict_fstream_holder

	} // namespace detail

	class ifstream
		: private detail::strict_fstream_holder< strict_fstream::ifstream >,
		public std::istream
	{
	public:
		explicit ifstream(const std::string& filename, std::ios_base::openmode mode = std::ios_base::in)
			: detail::strict_fstream_holder< strict_fstream::ifstream >(filename, mode),
			std::istream(new istreambuf(_fs.rdbuf()))
		{
			exceptions(std::ios_base::badbit);
		}
		virtual ~ifstream()
		{
			if (rdbuf()) delete rdbuf();
		}
	}; // class ifstream

	class ofstream
		: private detail::strict_fstream_holder< strict_fstream::ofstream >,
		public std::ostream
	{
	public:
		explicit ofstream(const std::string& filename, std::ios_base::openmode mode = std::ios_base::out)
			: detail::strict_fstream_holder< strict_fstream::ofstream >(filename, mode | std::ios_base::binary),
			std::ostream(new ostreambuf(_fs.rdbuf()))
		{
			exceptions(std::ios_base::badbit);
		}
		virtual ~ofstream()
		{
			if (rdbuf()) delete rdbuf();
		}
	}; // class ofstream

} // namespace zstr

/* ZSTR END */

// TODO: make all errors derivatives of std::err

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

	// TODO: define all the operations on all of the classes

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
		BRS::Direction direction;
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

	struct Bricks {
		BRS::Version version;
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
		std::vector<std::u16string> mods;
		std::vector<std::u16string> brickAssets;
		std::vector<Color> colors;
		std::vector<std::u16string> materials;
		std::vector<User> brickOwners;
	};

	class Reader
	{
		typedef std::vector<uint8_t> buffer;
		typedef std::vector<uint8_t>::iterator buffer_iterator;

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

		static buffer read_compressed(std::istream&);
		static buffer read_into_buffer(std::istream&, int32_t);

		std::optional<Header1> header1;
		std::optional<Header2> header2;
		std::optional<Bricks> bricks;
	public:

		Reader(std::string filepath);

		BRS::Version version;
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
		if (!check_magic(reader_) || reader_.fail())
		{
			throw "Invalid BRS file.";
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
		default:
			throw "Unsupported BRS version.";
		}

		gameVersion = 3642;
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
				throw "Invalid UCS-2 data size.";
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
				throw "Invalid UCS-2 data size.";
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

	template <typename T>
	inline std::vector<T> Reader::read_array(buffer_iterator& reader, T (*func)(buffer_iterator& r)){
		int32_t size = read_number<int32_t>(reader);
		std::vector<T> result;
		for(int i = 0; i < size; i++)
		{
			result.push_back(func(reader));
		}
		return result;
	}
	
	inline Reader::buffer Reader::read_into_buffer(std::istream& reader, int32_t size)
	{

		buffer buffer;
		for(int i = 0; i < size; i++)
		{
			char a;
			reader.read(&a, sizeof(a));
			buffer.push_back(static_cast<uint8_t>(a));
		}
		return buffer;
	}

	inline Reader::buffer Reader::read_compressed(std::istream& reader)
	{
		// represents compressed and uncompressed block sizes
		const int32_t uncompressed_size = read_int32(reader);
		const int32_t compressed_size = read_int32(reader);

		// Throw error for weird compression/uncompression sizes
		if (compressed_size < 0 || uncompressed_size < 0 || compressed_size >= uncompressed_size)
		{
			// TODO: fix this
			throw "Invalid compressed section size (comp: , uncomp: )";
		}

		// No compressed data? return those bytes
		if(compressed_size == 0)
		{
			// Decompress if there is data to decompress
			return read_into_buffer(reader, uncompressed_size);
		} else
		{
			// Decompress if there is data to decompress
			zstr::istreambuf zsbuf(reader.rdbuf(), compressed_size, true);
			std::istream s(&zsbuf);
			return read_into_buffer(s, uncompressed_size);
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
		buffer header1_raw = read_compressed(reader_);

		buffer_iterator it = header1_raw.begin();
		std::u16string map = read_string(it);
		std::u16string author_name = read_string(it);
		std::u16string description = read_string(it);
		uint64_t f = read_number<int64_t>(it);
		uint64_t s = read_number<int64_t>(it);
		UUID author_id = UUID(f, s);
		std::optional<User> host = std::nullopt;
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
		// represents compressed and uncompressed block sizes
		buffer header2_raw = read_compressed(reader_);

		buffer_iterator it = header2_raw.begin();
		std::vector<std::u16string> mods = read_array<std::u16string>(it, &Reader::read_string);
		std::vector<std::u16string> brick_assets = read_array<std::u16string>(it, &Reader::read_string);
		std::vector<Color> colors = read_array<Color>(it, &Reader::read_color);
		std::vector<std::u16string> materials = read_array<std::u16string>(it, &Reader::read_string);
		std::vector<User> brick_owners = read_array(it, read_user);

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

	}
#endif /* BRS_BRICKADIA_IMPLEMENTATION_H */

}

#endif /* BRS_BRICKADIA_H */
