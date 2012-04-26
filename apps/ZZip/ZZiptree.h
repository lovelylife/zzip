
#ifndef __TREE_H__
#define __TREE_H__

#include "ZZipFileObject.h"

#include<list>

template<class _Traits>
class _tree : public _tree_node<_Traits> {
public:
	typedef _tree<_Traits> _MyType;
	typedef _tree_node<_Traits> _MyBase;
	typedef typename _Traits::KeyType KeyType;
	typedef typename _Traits::ValueType ValueType;
//	typedef typename _Traits::KeyCompare KeyCompare;

	typedef typename _Traits::KeyCompare KeyCompare;
	typedef typename _MyBase::NodePtr NodePtr;
	typedef typename _MyBase::PathType PathType;

	_tree() 
		: _MyBase()
	{
		_init();
	}

public:
	NodePtr insert(const PathType& path, const ValueType& v) {
		NodePtr _Where = _where(path);
		if(0 == _Where) {
			// 构建节点
			_Where = _construct_path(path);
		}
		NodePtr _newnode = _buy_node(v, _Where);
		return _newnode;
	}

	NodePtr _where(const PathType& path) {
		// 查找path所在节点
		NodePtr node = _Root();
		PathType::const_iterator cit = path.begin();
		for(; cit != path.end() && (NULL != node); cit++) {
			if((*cit) == _Key(node)) {
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
		return _Header;
	}

	static const KeyType& _Key(NodePtr node) {
		return _MyBase::_Kf(_Value(node));
	}

	static const ValueType& _Value(NodePtr node) {
		return (*node)._Value;
	}

	static PathType _Path(NodePtr node) {
		NodePtr _P = _Parent(node);
		PathType stack;
		while(_P) {
			stack.push_back(_Key(_P));
			_P = _Parent(_P);
		}
		return stack;
	}

	static NodePtr& _Parent(NodePtr node) {
		return (*node)._Parent;
	}

	static NodePtr& _Left(NodePtr node) {
		return (*node)._Left;
	}

	static NodePtr _Right(NodePtr node) {
		return (*node)._Right;
	}

protected:

	void _init() {
		_Header = _buy_node();
	}

	NodePtr _construct_path(const PathType& path) {
		NodePtr _rootnode = _Root();
		NodePtr _wherenode = _rootnode;
		NodePtr _where_construct = _rootnode;
		PathType::const_iterator cit = path.begin();
		for(; cit != path.end() && (NULL != _wherenode); cit++) {
			// 是否存在（*cit）子节点
			NodePtr _temp_node = _wherenode;
			bool node_exist = false;
			while(NULL != _temp_node) {
				if((*cit) == _Key(_temp_node)) {
					// 该节点存在
					node_exist = true;
					break;
				}
				_temp_node = _Right(_temp_node); // 查找兄弟
			}

			if(!node_exist) {
				// 构造
				_where_construct = _wherenode;
				break;
			} else {
				_wherenode = _Left(_wherenode);
			}
		}

		// 从头cit和_where_construct 开始构造
		for(; cit != path.end(); cit++) {
			
			NodePtr _newnode = _buy_node(_MyBase::_Mv(*cit, 0), _where_construct);
			_where_construct = _newnode;
		}

		return _where_construct;
	}

	NodePtr _buy_node(const ValueType& v, NodePtr parent) {
		NodePtr _newnode = _MyBase::_buy(v, parent);
		if(parent) {
			if(_Left(parent)) {
				NodePtr n = _Left(parent);
				while(_Right(n)){
					n = _Right(n);
				};
				n->_Right = _newnode;
			} else {
				parent->_Left = _newnode;
			}
		}
		return _newnode;
	}

	NodePtr _buy_node() {
		NodePtr _newnode = _MyBase::_buy();
		_newnode->_Parent = 0;
		_newnode->_Left = 0;
		_newnode->_Right = 0;

		return _newnode;
	}

private:
	NodePtr _Header;
};

template<class Kty, class Vty>
class _tree_node_traits {
public:
	typedef Kty KeyType;
	typedef std::pair<Kty, Vty> ValueType;
	typedef std::list<KeyType> PathType;

	class KeyCompare : public binary_function<KeyType, KeyType, bool> {
	public:
		bool operator()(const KeyType& key1, const KeyType& key2) {
			return (key1 == key2);
		}
	};

	static const KeyType& _Kf(const ValueType& _Val)
	{	// extract key from element value
		return (_Val.first);
	}

	static ValueType _Mv(const KeyType& k, const Vty& v) {
		return std::make_pair(k, v);
	}
};

template<class Kty,	class Vty>
class ZZipTree 
	: public _tree< _tree_node_traits<Kty, Vty> > 
{
public:
	typedef ZZipTree<Kty, Kty> _MyType;
	typedef _tree< _tree_node_traits<Kty, Vty> > _MyBase;
	typedef typename _MyBase::KeyType KeyType;
	typedef typename _MyBase::ValueType ValueType;
	typedef typename _MyBase::PathType PathType;
	typedef typename _MyBase::KeyCompare KeyCompare;

	ZZipTree() : _MyBase() {}

	~ZZipTree() {}

};


#endif
