
#ifndef __TREE_H__
#define __TREE_H__

#include "ZZipFileObject.h"

#include <list>

template<class Traits>
class _tree_node {
public:
	typedef _tree_node* NodePtr;
	typedef typename Traits::KeyType KeyType;
	typedef typename Traits::ValueType ValueType;
	typedef typename Traits::PathType PathType;

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

template<class Kty, class Vty>
class _tree_node_traits {
public:
	typedef Kty KeyType;
	typedef Vty ValueType;
	typedef std::list<KeyType> PathType;

	class IsKeyEqual {
	public:
		bool operator()(KeyType key1, KeyType key2) {
			return (key1 == key2);
		}
	};
};

template<class Kty, 
	class Vty>
class _tree : public _tree_node< _tree_node_traits<Kty, Vty> > {
public:
	typedef typename _tree_node::KeyType KeyType;
	typedef typename _tree_node::ValueType ValueType;
	typedef typename _tree_node::NodePtr NodePtr;
	typedef typename _tree_node::PathType PathType;
	typedef _tree_node_traits::IsKeyEqual KeyCompare;

	_tree() {}

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
		while(path.size() && (NULL != node) ) {
			PathType::const_iterator cit = path.begin();
			if(KeyCompare(*cit, _Key(node))) {
				node = _Left(node); // 找孩子
			} else {
				node = _Right(node); // 找兄弟
			}

			path.pop_front();
		}

		if((path.size() == 0) && (NULL != node)) {
			return node;
		}
	}

// operator
public:
	NodePtr _Root() const {
		return this;
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

#endif
