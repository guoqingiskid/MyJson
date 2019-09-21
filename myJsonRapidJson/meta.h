#ifndef MYJSON_RAPIDJSON_H
#define MYSON_RAPIDJSON_H
#include <type_traits>

namespace my_json
{
	//is_base_of��ģ�庯��ƥ����������̳�����ָ����ʽת������������ָ����ʽתΪ�����ƥ��������ߣ���������Ա����const���const���ٴ���ģ�庯������ͨ������
	//�������B�̳���D��operator const D*()�൱��operator const B*()�����host����ƥ�䵱Ȼ���ȿ��ǵڶ���oeprator const D*()��
	//��������һ������£�û�м̳й�ϵ����ôhost���棬�����һ��ƥ��̶ȸ��ߣ����������ģ�庯������ͨ����֮���ƥ��ʱ������ֱ�Ӻ��Ե����������ƥ�䵽������ͨ����
	//�ڶ�����������м̳й�ϵʱ��������ʽת����ԭ���������ѡ�����͸��ӽ���ģ�庯����
	template<typename B, typename D>
	struct IsBaseOfImpl
	{
		/*static_assert(sizeof(B) != 0);
		static_assert(sizeof(D) != 0);*/

		typedef char(&Yes)[2];
		typedef char(&No)[1];

		//������һ��ƥ�����A
		template<typename T>
		static Yes check(const D*, T);
		static No check(const B*, int);

		struct host
		{
			//������һ��ƥ�����B��ͨ���й���A��ƥ�����ȼ����ڹ���B������һ����Ϣ��������ָ�������ʽת��Ϊ����ָ��
			/* �൱����������жԱ� */
			operator const B*()const;				/* operator cosnt B*()const */
			operator const D*();					/* operator const B*() */
		};

		enum { Value = sizeof(check(host{}, 0)) == sizeof(Yes) };
	};

	template<typename T>
	struct Void { typedef void Type; };

	template<bool Var>
	struct BoolType
	{
		static const bool Value = Var;
		typedef BoolType Type;
	};

	typedef BoolType<true>  TrueType;
	typedef BoolType<false> FalseType;

	template<typename B, typename D>
	struct IsBaseOf : public BoolType<IsBaseOfImpl<B, D>::Value> {};

	template<bool C>
	struct SelectIfImpl
	{
		template<typename T1, typename T2>
		struct Apply
		{
			typedef T1 Type;
		};
	};
	template<>
	struct SelectIfImpl<false>
	{
		template<typename T1, typename T2>
		struct Apply
		{
			typedef T2 Type;
		};
	};

	template<bool C, typename T1, typename T2>
	struct SelectConditon : public SelectIfImpl<C>::Apply<T1, T2>{};

	template<typename C, typename T1, typename T2>
	struct SelectIf : public SelectConditon<C::Value, T1, T2> {};


	//���ʽ
	template<bool C1, bool C2>
	struct AddExprCond :public FalseType {};

	template<>
	struct AddExprCond<true, true> : public TrueType {};

	template<bool C1, bool C2>
	struct OrExprCond : public TrueType {};

	template<>
	struct OrExprCond<false, false> : public FalseType {};

	template<typename C1, typename C2>
	struct AndExpr : public AddExprCond<C1::Value, C2::Value> {};

	template<typename C1, typename C2>
	struct OrExpr : public OrExprCond<C1::Value, C2::Value> {};

	//AddConst��RemoveConst
	template<typename T>
	struct AddConst { typedef const T Type; };
	template<typename T>
	struct AddConst<const T> { typedef const T Type; };

	template<typename T>
	struct RemoveConst { typedef T type; };
	template<typename T>
	struct RemoveConst<const T> { typedef T Type; };

	//isPointer
	template<typename T>
	struct IsPointer : public FalseType {};
	template<typename T>
	struct IsPointer<T*> : public TrueType {};

	//EnableIf DisableIf
	template<bool Condition, typename T = void>
	struct EnableIfCond { typedef T Type; };
	template<typename T>
	struct EnableIfCond<false, T>{};

	template <bool Condition, typename T = void> 
	struct DisableIfCond { typedef T Type; };
	template <typename T> 
	struct DisableIfCond<true, T> { };

	template<typename C, typename T>
	struct EnableIf : public EnableIfCond<C::Value, T> {};

	template<typename C, typename T>
	struct DisableIf : public DisableIfCond<C::Value, T> {};

	
}
#endif
