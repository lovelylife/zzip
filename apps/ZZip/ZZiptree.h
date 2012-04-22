
#ifndef __TREE_H__
#define __TREE_H__

#include "ZZipFileObject.h"

using namespace std;

class _tree_node {
public:
	typedef _tree_node* NodePtr;
	typedef std::wstring KeyType;
	typedef std::pair<KeyType, RefPtr<ZZipFileObject> > ValueType;

	_tree_node(ValueType v, NodePtr Parent)
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

class _tree {
public:
	typedef typename _tree_node::KeyType KeyType;
	typedef typename _tree_node::ValueType ValueType;
	typedef typename _tree_node::NodePtr NodePtr;

	_tree() {}

public:
	NodePtr insert(const ValueType& v) {
		NodePtr _Where = _where(_Key(v));
		if(0 == _Where) {
			return 0;
		}
		_Where = _buy_node(v, NULL);
		return _Where;
	}
	
	NodePtr _where(const KeyType& key) {
		// 查找Key所在节点
	}
// operator
public:
	NodePtr _Root() const {
		return this;
	}

	static KeyType& _Key(NodePtr node) {
		return (*node)._Value.first;
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
