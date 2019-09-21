#include "meta.h"
#include <stdio.h>
template<typename B, typename D> struct IsBaseOfImpl {
	typedef char(&Yes)[1];
	typedef char(&No)[2];

	template <typename T>
	static Yes Check(const D*, T);
	static No  Check(const B*, int);

	struct Host {	
		operator const B*()const;
		operator const D*();
	};

	enum { Value = (sizeof(Check(Host{}, (int)100)) == sizeof(Yes)) };
};

class Base
{

};
class Dir : public Base
{
	
};

int main()
{
	bool test1 = IsBaseOfImpl<Base, Dir>::Value;
	bool test2 = IsBaseOfImpl<int, Dir>::Value;
	fprintf(stdout, "test1: %d\ttest2 : %d\n", test1, test2);
	return 0;
}