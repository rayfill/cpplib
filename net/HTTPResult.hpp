#ifndef HTTPRESULT_HPP_
#define HTTPRESULT_HPP_
#include <string>
#include <stdexcept>
#include <map>
#include <vector>
#include <limits>
#include <Socket/Socket.hpp>
#include <net/HTTPProperty.hpp>
#include <text/LexicalCast.hpp>
#include <util/Predicate.hpp>

class ResponseError : public std::runtime_error
{
public:
	ResponseError(const char* reason = "bad response"):
		std::runtime_error(reason)
	{}

	virtual ~ResponseError() throw()
	{}

	const char* what() const throw()
	{
		return std::runtime_error::what();
	}
};

template <size_t BufferSize = 4096>
class HTTPResult
{
	friend class HTTPResultTest;

private:
	Property<Predicate::IgnoreCaseComparator> properties;
	std::vector<unsigned char> resource;
	std::string statusReason;
	int statusCode;

	std::string readLine(Socket& socket)
	{
		std::string result;

		char readBuffer;
		for (;;)
		{
			socket.readAsync(&readBuffer, 1);

			if (readBuffer == '\n' &&
				result.size() > 0 &&
				result[result.size() - 1] == '\r')
			{
				result.insert(result.end(), readBuffer);
				break;
			}
			else 
				result.insert(result.end(), readBuffer);

		}

		return result;
	}

	void readStatus(Socket& socket)
	{
		const std::string statusLine = readLine(socket);
		statusCode = lexicalCast<int>(statusLine.substr(9, 3));
		statusReason = statusLine;
	}

	void readHeader(Socket& socket)
	{
		for (;;)
		{
			const std::string header = readLine(socket);
			if (header == "\r\n")
				break;

			std::pair<std::string,std::string> headerPair =
				parseHeader(header);
			properties.set(headerPair.first.c_str(), headerPair.second);
		}
	}

	std::pair<std::string, std::string> parseHeader(const std::string& header)
	{
		if (header.length() < 5 ||
			header[header.length() - 2] != '\r' ||
			header[header.length() - 1] != '\n')
			throw ResponseError((std::string("Header length or CR/LF "
											 "is invalid: ") +
								 header).c_str());

		const std::string::size_type separatePos = header.find(":");
		std::string key = header.substr(0, separatePos);
		if (separatePos == std::string::npos)
			throw ResponseError((std::string("Header separate token "
											 "missing: ") +
								 header).c_str());

		std::string value =
			header.substr(separatePos + 1, header.length() - key.length() - 3);
		const std::string::size_type startPos = value.find_first_not_of(" ");
		
		if (startPos != std::string::npos)
			value = value.substr(startPos);

		return std::make_pair(key, value);
	}

	void readResource(Socket& socket, const size_t totalSize)
	{
		unsigned char readBuffer[BufferSize];

		size_t readSize = 0;
		size_t totalReadSize = 0;
		
		while (totalSize > totalReadSize)
		{
			readSize = 
				socket.readAsync(readBuffer, 
								 (totalSize - totalReadSize) < BufferSize ?
								 (totalSize - totalReadSize) : BufferSize);

			totalReadSize += readSize;
			resource.insert(resource.end(),
							readBuffer, readBuffer + readSize);
		}
	}

public:
	HTTPResult():
		properties(), resource(), statusReason(), statusCode()
	{}

	HTTPResult(const HTTPResult& rhs):
		properties(rhs.properties),
		resource(rhs.resource),
		statusReason(rhs.statusReason),
		statusCode(rhs.statusCode)
	{}

	HTTPResult& operator=(const HTTPResult& rhs)
	{
		properties = rhs.properties;
		resource = rhs.resource;
		statusReason = rhs.statusReason;
		statusCode = rhs.statusCode;

		return *this;
	}

	virtual ~HTTPResult() throw()
	{}

	int getStatusCode() const
	{
		return statusCode;
	}

	std::string getStatusReason() const
	{
		return statusReason;
	}

	Property<Predicate::IgnoreCaseComparator> getResponseHeaders() const
	{
		return properties;
	}

	std::vector<unsigned char> getResource() const
	{
		return resource;
	}

	void readHeadResponse(Socket& socket)
	{
		readStatus(socket);
		readHeader(socket);
	}

	void readGetResponse(Socket& socket)
	{
		readStatus(socket);
		readHeader(socket);

		if ((this->getStatusCode() >= 100 &&
			this->getStatusCode() < 200) ||
			this->getStatusCode() == 304) // Not Modified
			return;

		if (properties.get("Transfer-Encoding") == "chunked")
		{
			// chunk mode.
			size_t chunkSize;
			do 
			{
				std::string chunkSizeHeader = readLine(socket);
				const std::string::size_type tokenTerm =
					chunkSizeHeader.find_first_not_of("0123456789ABCDEFabcdef");

				chunkSize =
					hexLexicalCast<size_t>(chunkSizeHeader.substr(0, tokenTerm));
				readResource(socket, chunkSize);

				if (readLine(socket) != "\r\n")
					throw  ResponseError();

			} while (chunkSize != 0);
		}
		else if (properties.get("Content-Length") != "")
		{
			const size_t contentLength =
				lexicalCast<size_t>(properties.get("Content-Length"));
		
			readResource(socket, contentLength);
		}
		else
		{
			// HTTP/1.0 version read contents.
			try
			{
				readResource(socket, std::numeric_limits<size_t>::max());
			}
			catch (ConnectionClosedException& )
			{
			}
		}
	}
};

#endif /* HTTPRESULT_HPP_ */

