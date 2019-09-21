#ifndef MYJSON_RAPIDJSON_H
#define MYSON_RAPIDJSON_H
#include <type_traits>

namespace my_json
{
	//is_base_of：模板函数匹配能力：类继承链上指针隐式转换比如派生类指针隐式转为基类的匹配能力最高，其次是类成员函数const与非const，再次是模板函数与普通函数。
	//在这里，当B继承于D，operator const D*()相当于operator const B*()，因此host里面匹配当然优先考虑第二个oeprator const D*()。
	//分析：第一种情况下，没有继承关系，那么host里面，本身第一个匹配程度更高，但是相对于模板函数与普通函数之间的匹配时，可以直接忽略掉。因此最终匹配到的是普通函数
	//第二种情况，具有继承关系时，由于隐式转换的原因，因此最终选择类型更接近的模板函数。
	template<typename B, typename D>
	struct IsBaseOf
	{
		/*static_assert(sizeof(B) != 0);
		static_assert(sizeof(D) != 0);*/

		typedef char(&Yes)[2];
		typedef char(&No)[1];

		//这里是一组匹配规则A
		template<typename T>
		static Yes check(const D*, T);
		static No check(const B*, int);

		struct host
		{
			//这里是一组匹配规则B，通常有规则A的匹配优先级高于规则B，另外一条信息：派生类指针可以隐式转换为基类指针
			/* 相当于与下面进行对比 */
			operator const B*()const;				/* operator cosnt B*()const */
			operator const D*();					/* operator const B*() */
		};

		enum { Value = sizeof(check(host{}, 0)) == sizeof(Yes); };
	};

	template<typename T>
	struct Void { typedef void type; };

	template<bool Var>
	struct BoolType
	{
		static const bool Value = Var;
		typedef BoolType type;
	};

	typedef BoolType<true>  TrueType;
	typedef BoolType<false> FalseType;

	template<bool C>
	struct SelectIfImpl
	{
		template<typename T1, typename T2>
		struct Apply
		{
			typedef T1 type;
		};
	};
	template<>
	struct SelectIfImpl<false>
	{
		template<typename T1, typename T2>
		struct Apply
		{
			typedef T2 type;
		};
	};

	template<bool C, typename T1, typename T2>
	struct SelectConditon : public SelectIfImpl<C>::Apply<T1, T2>
	{

	};

	template<typename C, typename T1, typename T2>
	struct SelectIf : public SelectConditon<C::Value, T1, T2>
	{

	};

	//表达式
	template<bool C1, bool C2>
	struct AddExprCond :public FalseType
	{

	};

	template<>
	struct AddExprCond<true, true> : public TrueType
	{

	};

	template<bool C1, bool C2>
	struct OrExprCond : public TrueType
	{

	};

	template<>
	struct OrExprCond<false, false> : public FalseType
	{

	};


}




#endif
