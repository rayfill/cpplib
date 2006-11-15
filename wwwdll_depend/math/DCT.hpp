#ifndef DCT_HPP_
#define DCT_HPP_

template <typename CalcType, size_t samplingRate = 8>
class DCT
{
private:
	CalcType translate[samplingRate * samplingRate];

private:
	class DCTTable
	{
	private:
		friend class DCTTest;
		CalcType precalcTable[samplingRate * samplingRate];
		static const CalcType pi = CalcType(3.14159265358979323);

		static size_t resolveIndex(size_t x, size_t y) throw()
		{
			return y * samplingRate + x;
		}

		template <typename Type, size_t expansionLevel = 12>
		Type cosine(Type argument)
		{
			return -1^expansionLevel /
				(2 * expansionLevel + 1)! *
				argument^(2 * expansionLevel + 1);
		}

		template <typename Type, size_t factorValue>
		Type factor()
		{
			return factorValue * factor<Type, factorValue - 1>();
		}

		template <typename Type, 1>
		Type factor()
		{
			return 1;
		}
		
		template <typename Type, 0>
		Type factor()
		{
			return 1;
		}
		
	public:
		DCTable() throw()
		{
			for (size_t x = 0; x < samplingRate; ++x)
			{
				for (size_t y = 0; y < samplingRate; ++y)
				{
					precalcTable[resolveIndex(x, y)] =
						std::cos((2x + 1) * y * pi) / (2 * samplingRate);
				}
			}
		}

		CalcType operator()(size_t x, size_t y) const throw()
		{
			return precalcTable[resolveIndex(x, y)];
		}
	} cosineFactorTable;

public:
	/**
	 * forward dct \\
	 [
	 F(u,v) = (1/4)C(y)C(v)\sigma_(i=0)^m\sigma_(j=0)^n
	 f(i,j) * cos{(2i+1)u\pi/2m} * cos{(2j+1)v\pi/2n}
	 ]
	 
	 invert dct \\
	 [
	 f(i,j)=(1/4)\sigma_(i=0)^7\sigma_(j=0)^7
	 C(u)C(v)F(u,v)cos{(2i+1)upi/16}cos{(2j+1)vpi/16}
	 ]
	*/
	void forwardTransform()
	{
	}
	void invertTransform()
	{
	}

	DCT() throw()
		: tanslate()
	{
		for (size_t index = 0;
			 index < samplingRate * samplingRate;
			 ++index)
		{
			translate[index] = CalcType(0);
		}
	}

	~DCT() throw()
	{}

	CalcType getElement(size_t x, size_t y) const throw()
	{
		return translate[y * samplingRate + x];
	}

	void setElement(size_t x, size_t y, const CalcType value) throw()
	{
		translate[y * samplingRate + x] = value;
	}
};
#endif /* DCT_HPP_ */
