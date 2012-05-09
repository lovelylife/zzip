
#ifndef __TREE_H__
#define __TREE_H__

#include <list>
#include "utils/AcfDelegate.h"

template<class _Traits>
class _tree_node : public _Traits {
public:
	struct _node;
	typedef _node* NodePtr;
	typedef _tree_node<_Traits> _MyType;
	typedef typename _Traits::KeyType KeyType;
	typedef typename _Traits::ValueType ValueType;
	typedef typename _Traits::PathType PathType;
//	typedef typename _Traits::KeyCompare KeyCompare;

	struct _node {
		_node(const ValueType& v, NodePtr Parent, bool isnil)
			: _Value(v)	, _IsNil(isnil), _Parent(Parent), _Left(0)	, _Right(0)
		{

		}

		_node() 
			:_Parent(0), _Left(0), _Right(0), _IsNil(false) {
		}

		NodePtr _Parent;	// 双亲节点
		NodePtr _Left;		// 孩子节点
		NodePtr _Right;		// 兄弟节点
		ValueType _Value;	// 绑定的数据
		bool _IsNil;			// 是否是叶节点
	};

	_tree_node() {}
	~_tree_node() {}

	NodePtr _buy(const ValueType& v, NodePtr Parent, bool isnil) {
		NodePtr _newnode = new _node(v, Parent, isnil);
		return _newnode;
	}

	NodePtr _buy() {
		NodePtr _newnode = new _node();
		return _newnode;
	}
};



template<class _Traits>
class _tree : public _tree_node<_Traits> {
public:
	typedef _tree<_Traits> _MyType;
	typedef _tree_node<_Traits> _MyBase;
	typedef typename _Traits::KeyType KeyType;
	typedef typename _Traits::ValueType ValueType;
	typedef typename _Traits::MapType MapType;

//	typedef typename _Traits::KeyCompare KeyCompare;
	typedef typename _MyBase::NodePtr NodePtr;
	typedef typename _MyBase::PathType PathType;

	_tree() 
		: _MyBase()
	{
		_init();
	}

	~_tree() {
		_tidy();
		_destory(_Header);
	}

public:
	// 插入叶节点： 如果path叶节点不存在则返回空指针， 在insert之前先判断path是否存在
	NodePtr insert(PathType path, const MapType& v) {
		NodePtr _Where = Where(path, true);
		NodePtr _returnnode = NULL;
		KeyType key = (*path.rbegin());
		path.pop_back();
		if(NULL == _Where) { 
			_Where = kdir(path);
			// 构建叶节点
			_returnnode = _buy_node(std::make_pair(key, v), _Where, true);
		} else {
			// 节点已经存在
			_returnnode = _Where;
			_returnnode->_Value = std::make_pair(key, v);
		}
		return _returnnode;
	}

	NodePtr Where(PathType path, bool isleaf = false) {
		// 查找path所在节点
		NodePtr node = _Left(_Root());
		if(NULL == node) return node;
		//PathType::const_iterator cit = path.begin();
		bool bfound = false;
		if(path.size() < 1) {
			if(isleaf) return NULL;
			else return node;
		}

		while(node) {
 			if((*path.begin()) == _Key(node)) {
				if(_IsLeaf(node)) { // 叶节点					
					bfound = isleaf;					
					break;
				} else { // 非叶节点
					if(path.size() == 1) {
						// 最后一个
						bfound = !isleaf;
						break;
					} else {
						path.pop_front();
						if(path.size() < 1) break;
						node = _Left(node);
					}
				}
			} else {
				node = _Right(node);
			}
		}
		
		if(!bfound) {
			return NULL;
		}

		return node;
	}

	// 构造路径，如果最后一个是叶节点则失败
	NodePtr kdir(const PathType& path) {
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
			if(!node_exist) {	// 构造				
				_where_construct = _wherenode;
				break;
			} else {
				_wherenode = _Left(_wherenode);
			}
		}
		
		// 从头cit和_where_construct 开始构造非叶节点
		for(; cit != path.end(); cit++) {
			NodePtr _newnode = _buy_node(_MyBase::_Mv(*cit, MapType()), _where_construct, false);
			_where_construct = _newnode;
		}
		return _where_construct;
	}
	// 删除节点
	void erase(const PathType& path, bool isleaf) {
		NodePtr _where = Where(path, isleaf);
		if(NULL  == _where ) {
			return;
		}
		// 父亲节点, 如果p为空则为根节点，不允许删除
		NodePtr p = _Parent(_where);
		if(NULL != p) {
			NodePtr prev = _Left(p);
			NodePtr next = _Right(_where);
			if(prev == _where) { // 是父亲节点的第一个子节点
				p->_Left = next;				
			} else { // 不是父亲节点的第一个子节点
				while(prev) {
					if(_Right(prev) == _where) {
						// 找到节点
						prev->_Right = next;
						break;
					}
					prev = _Right(prev);
				}
			}
			// 右子树指向空，不需要删除
			_where->_Right = 0;
			_destory(_where);
		}
	}

	void clear() {

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
		PathType stack;
		if(NULL != node) {
			NodePtr _P = _Parent(node);
			while(_P && _Parent(_P) ) {
				stack.push_front(_Key(_P));
				_P = _Parent(_P);
			}
			stack.push_back(_Key(node));
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

	static bool _IsLeaf(NodePtr node) {
		return (*node)._IsNil;
	}
protected:
	// 构造只有根节点的二叉树
	void _init() {
		_Header = _buy_node();
	}

	// 清理树
	void _tidy() {
		NodePtr _myheader = _Root();
		_destory(_Left(_myheader));
		_destory(_Right(_myheader));
		_myheader->_Left = 0;
		_myheader->_Right = 0;	
	}

	void _destory(NodePtr node) {
		if(node) {
			_destory(_Left(node));
			_destory(_Right(node));
			delete node;
		}
	}

	NodePtr _buy_node(const ValueType& v, NodePtr parent, bool isnil) {
		NodePtr _newnode = _MyBase::_buy(v, parent, isnil);
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
	typedef Vty MapType;
	typedef std::list<KeyType> PathType;

	static const KeyType& _Kf(const ValueType& _Val)
	{	// extract key from element value
		return (_Val.first);
	}

	static ValueType _Mv(const KeyType& k, const MapType& v) {
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
	typedef Acf::Delegate< bool(const ValueType&) > TravCallBack;

	class TravClass {
	public:
		TravClass();
		~TravClass();
	};

	typedef bool (*Travsor)(NodePtr, void*);
//	typedef typename _MyBase::KeyCompare KeyCompare;

	ZZipTree() 
		: _MyBase() {
	}

	~ZZipTree() {
	}

	bool Find(const PathType& path, MapType& out) {
		NodePtr node = _MyBase::Where(path, true);
		if(NULL == node) {
			return false;
		}
		out = _Value(node).second;
		return true;
	}

	void Trav(const PathType& path, Travsor tv, void* arg, bool Recursive) {
		NodePtr _Where = _MyBase::Where(path);
		_Trav(_Where, tv, arg, Recursive);
	}

private:
	bool _Trav(NodePtr node, Travsor tv, void* arg, bool Recursive) {
		if(NULL == node) return false; 
		if(tv) {
			if(!tv(node, arg)) {
				return false;
			}
		}
		if(Recursive) {
			_Trav(_Left(node), tv, arg, Recursive);
		}

		_Trav(_Right(node), tv, arg, Recursive);

		return true;
	}

	void _PreOrder(NodePtr node) {
		if(node) {
			TravEvent(_Value(node));
			_PreOrder(_Left(node));
			_PreOrder(_Right(node));
		}
	}

	void _MidOrder(NodePtr node) {
		if(node) {
			_MidOrder(_Left(node));
			TravEvent(_Value(node));
			_PreOrder(_Right(node));
		}
	}

	void _BehindOrder(NodePtr node) {
		if(node) {
			_BehindOrder(_Left(node));
			_BehindOrder(_Right(node));
			TravEvent(_Value(node));
		}
	}

public:
	TravCallBack TravEvent;

};


#endif
