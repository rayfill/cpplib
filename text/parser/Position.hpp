#ifndef POSITION_HPP_
#define POSITION_HPP_

class Position
{
private:
	int line;
	int column;

public:
	Position():
		line(), column()
	{}

	Position(int line_, int column_):
		line(line_), column(column_)
	{}

	Position(const Position& source):
		line(source.line), column(source.column)
	{}

	~Position()
	{}

	Position& operator=(const Position& source)
	{
		if (this != &source)
		{
			this->line = source.line;
			this->column = source.column;
		}
		return *this;
	}


};

#endif /* POSITION_HPP_ */
