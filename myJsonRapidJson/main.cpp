#include "meta.h"
#include "pow10.h"
#include <stdio.h>
#include <stdint.h>
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

#define RAPIDJSON_ALIGN(x) (((x) + static_cast<size_t>(7u)) & ~static_cast<size_t>(7u))
int main()
{
	uint32_t t = ~7u;
	int q = static_cast<size_t>(30u) & ~static_cast<size_t>(15u);
	int s = RAPIDJSON_ALIGN(10);
	return 0;
}