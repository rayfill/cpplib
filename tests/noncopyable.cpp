
class noncopyable
{
private:
	noncopyable(const noncopyable&);
	const noncopyable& operator=(const noncopyable&);
public:
	noncopyable() {}
	virtual ~noncopyable() {}
};


class noncopyTest : public boost::noncopyable
{
public:
//	noncopyTest& operator=(const noncopyTest&) { return *this; }
};

int main()
{
	noncopyTest a;
	noncopyTest b;// = a;
	b = a;

	return 0;
}
