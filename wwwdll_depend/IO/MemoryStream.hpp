#ifndef MEMORYSTREAM_HPP_
#define MEMORYSTREAM_HPP_

#include <iostream>
#include <vector>

// @todo 読み書き両対応への実装の変更。
// bufferのdeque化か読み出し/書き込み位置をメンバとして持つか・・・
template <typename BaseType, typename Traits = std::char_traits<BaseType> >
class BasicMemoryStreamBuf : public std::basic_streambuf<BaseType, Traits>
{
private:
	std::vector<BaseType> buffer;

public:
	BasicMemoryStreamBuf()
	{
		std::basic_streambuf<BaseType, Traits>::setbuf(0, 0);
	}
	
	~BasicMemoryStreamBuf()
	{
	}

	std::vector<BaseType> getBuffer() const
	{
		return buffer;
	}

	void setBuffer(std::vector<BaseType>& source)
	{
		buffer.erase(buffer.begin(), buffer.end());

		for(size_t offset = source.size();
			offset > 0;
			--offset)
			buffer.push_back(source[offset - 1]);
	}

protected:
	std::streampos seekoff(
		std::streamoff /*off*/,
		std::ios_base::seekdir /*dir*/,
		int /*nMode = std::ios::in | std::ios::out*/)
	{
		return Traits::eof();
	}

	std::streampos seekpos(
		std::streampos /*pos*/,
		int /*nMode = std::ios::in | std::ios::out*/)
	{
		return Traits::eof();
	}

	int overflow(int nChar = Traits::eof())
	{
		buffer.push_back(static_cast<char>(nChar));
		return 0;
	}

	int uflow()
	{
		if (buffer.empty())
			return Traits::eof();

		char result = buffer.back();
		buffer.pop_back();

		return Traits::to_int_type(result);
	}

	int underflow()
	{
		return buffer.back();
	}
};

template <typename T, typename Traits=std::char_traits<T> >
class BasicMemoryStream : public std::basic_iostream<T, Traits>
{
private:
	BasicMemoryStreamBuf<T, Traits>* stub;

public:
	BasicMemoryStream() :
			std::basic_iostream<T, Traits>(NULL),
			stub(new BasicMemoryStreamBuf<T, Traits>())
	{
		this->init(stub);
	}

	~BasicMemoryStream()
	{
		delete stub;
	}

	std::vector<T> getMemory() const
	{
		return stub->getBuffer();
	}
	
	void setMemory(std::vector<T>& source)
	{
		stub->setBuffer(source);
	}

};

typedef BasicMemoryStream<char> MemoryStream;
 
#endif /* MEMORYSTREAM_HPP_ */
