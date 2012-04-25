
#ifndef __TREE_H__
#define __TREE_H__

#include "ZZipFileObject.h"

#include <list>

template<class _Traits>
class _tree_node {
public:
	typedef _tree_node<_Traits> _MyType;
	typedef typename _Traits::KeyType KeyType;
	typedef typename _Traits::ValueType ValueType;
	typedef typename _Traits::PathType PathType;
	typedef typename _Traits::KeyCompare KeyCompare;
	typedef _MyType* NodePtr;

	_tree_node(const ValueType& v, NodePtr Parent)
	: _Value(v)
	, _Parent(Parent)
	, _Left(0)
	, _Right(0)
	{

	}

public:
	NodePtr _Parent;
	NodePtr _Left;
	NodePtr _Right;
	ValueType _Value;
};



template<class _Traits>
class _tree : public _tree_node<_Traits> {
public:
	typedef _tree<_Traits> _MyType;
	typedef _tree_node<_Traits> _MyBase;
	typedef typename _Traits::KeyType KeyType;
	typedef typename _Traits::ValueType ValueType;
//	typedef typename _Traits::KeyCompare KeyCompare;

	typedef typename _MyBase::KeyCompare KeyCompare;
	typedef typename _MyBase::NodePtr NodePtr;
	typedef typename _MyBase::PathType PathType;

	_tree() : _MyBase(NULL, NULL){}

public:
	NodePtr insert(const PathType& path, const ValueType& v) {
		NodePtr _Where = _where(path);
		if(0 == _Where) {
			return 0;
		}
		_Where = _buy_node(v, NULL);
		return _Where;
	}

	NodePtr _where(const PathType& path) {
		// 查找path所在节点
		NodePtr node = _Root();
		PathType::const_iterator cit = path.begin();
		for(; cit != path.end(); cit++) {
			if(KeyCompare(*cit, _Key(node))) {
				node = _Left(node); // 找孩子
			} else {
				node = _Right(node); // 找兄弟
			}
		}

		return node;
	}

// operator
public:
	NodePtr _Root() {
		return static_cast<NodePtr>(this);
	}

	static KeyType& _Key(NodePtr node) {
		return (*node)._Value.first;
	}

	static PathType& _Path(NodePtr node) {
		NodePtr _P = _Parent(node);
		PathType stack;
		while(_P) {
			stack.push_back(_Key(_P));
			_P = _Parent(_P);
		}
		return stack;
	}

	static NodePtr _Parent(NodePtr node) {
		return (*node)._Parent;
	}

	static NodePtr _Left(NodePtr node) {
		return (*node)._Left;
	}

	static NodePtr _Right(NodePtr node) {
		return (*node)._Right;
	}

protected:
	NodePtr _buy_node(const ValueType& v, NodePtr parent) {
		NodePtr _where = new _tree_node(v, parent);
		return _where;
	}
};

template<class Kty, class Vty>
class _tree_node_traits {
public:
	typedef Kty KeyType;
	typedef Vty ValueType;
	typedef std::list<KeyType> PathType;

	class KeyCompare {
	public:
		bool operator()(const KeyType& key1, const KeyType& key2) {
			return (key1 == key2);
		}
	};
};

template<class Kty,
	class Vty>
class ZZipTree : public _tree< _tree_node_traits<Kty, Vty> > 
{
public:
	typedef ZZipTree<Kty, Kty> _MyType;
	typedef _tree< _tree_node_traits<Kty, Vty> > _MyBase;
	typedef typename _MyBase::KeyType KeyType;
	typedef typename _MyBase::ValueType Value;
	typedef typename _MyBase::PathType PathType;
	typedef typename _MyBase::KeyCompare KeyCompare;

	ZZipTree() : _MyBase() {}

	~ZZipTree() {}

};


#endif
