
#ifndef __TREE_H__
#define __TREE_H__

#include <list>
#include "AcfDelegate.h"

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

		NodePtr _Parent;	// ˫�׽ڵ�
		NodePtr _Left;		// ���ӽڵ�
		NodePtr _Right;		// �ֵܽڵ�
		ValueType _Value;	// �󶨵�����
		bool _IsNil;			// �Ƿ���Ҷ�ڵ�
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
	// ����Ҷ�ڵ㣺 ���pathҶ�ڵ㲻�����򷵻ؿ�ָ�룬 ��insert֮ǰ���ж�path�Ƿ����
	NodePtr insert(PathType path, const MapType& v) {
		NodePtr _Where = Where(path, true);
		NodePtr _returnnode = NULL;
		KeyType key = (*path.rbegin());
		path.pop_back();
		if(NULL == _Where) { 
			_Where = kdir(path);
			// ����Ҷ�ڵ�
			_returnnode = _buy_node(std::make_pair(key, v), _Where, true);
		} else {
			// �ڵ��Ѿ�����
			_returnnode = _Where;
			_returnnode->_Value = std::make_pair(key, v);
		}
		return _returnnode;
	}

	NodePtr Where(PathType path, bool isleaf = false) {
		// ����path���ڽڵ�
		NodePtr node = _Root();
		// ����սڵ�
		PathType::iterator it = path.begin();
		for(; it != path.end(); it++) {
			if(*it == KeyType()) {
				path.erase(it);
			}
		}
		
		if(path.empty()) {
			if(isleaf) return NULL;
			else return node;
		}

		bool bfound = false;
		node = _Left(node);
		while(node) {
 			if((*path.begin()) == _Key(node)) {
				if(_IsLeaf(node)) { // Ҷ�ڵ�					
					bfound = isleaf;					
					break;
				} else { // ��Ҷ�ڵ�
					if(path.size() == 1) {
						// ���һ��
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
		if(!bfound) { return NULL; }
		return node;
	}

	// ����·����������һ����Ҷ�ڵ���ʧ��
	NodePtr kdir(const PathType& path) {
		NodePtr _where = _Root();
		NodePtr _where_construct = _where;
		PathType::const_iterator cit = path.begin();
		// ������
		for(; cit != path.end() && (NULL != _where); cit++) {
			// �����ӽڵ����ƥ��
			NodePtr _temp_node = _Left(_where);
			bool node_exist = false;
			while(NULL != _temp_node) {
				if((*cit) == _Key(_temp_node)) {
					// �ýڵ����
					node_exist = true;
					break;
				}
				// �����ֵ�
				_temp_node = _Right(_temp_node);
			}
			if(!node_exist) {
				break;
			} else {
				// �ӵ�ǰƥ��ڵ����ƥ��
				_where = _temp_node; 
			}
		}
		_where_construct = _where;
		
		// ��ͷcit��_where_construct ��ʼ�����Ҷ�ڵ�
		for(; cit != path.end(); cit++) {
			NodePtr _newnode = _buy_node(_MyBase::_Mv(*cit, MapType()), _where_construct, false);
			_where_construct = _newnode;
		}
		return _where_construct;
	}
	// ɾ���ڵ�
	void erase(const PathType& path, bool isleaf) {
		NodePtr _where = Where(path, isleaf);
		if(NULL  == _where ) {
			return;
		}
		// ���׽ڵ�, ���pΪ����Ϊ���ڵ㣬������ɾ��
		NodePtr p = _Parent(_where);
		if(NULL != p) {
			NodePtr prev = _Left(p);
			NodePtr next = _Right(_where);
			if(prev == _where) { // �Ǹ��׽ڵ�ĵ�һ���ӽڵ�
				p->_Left = next;				
			} else { // ���Ǹ��׽ڵ�ĵ�һ���ӽڵ�
				while(prev) {
					if(_Right(prev) == _where) {
						// �ҵ��ڵ�
						prev->_Right = next;
						break;
					}
					prev = _Right(prev);
				}
			}
			// ������ָ��գ�����Ҫɾ��
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
	// ����ֻ�и��ڵ�Ķ�����
	void _init() {
		_Header = _buy_node();
	}

	// ������
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
			if(_Left(parent)) { // �����ӽڵ�
				NodePtr child = _Left(parent);
				while(_Right(child)){
					child = _Right(child);
				};
				child->_Right = _newnode;
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
	typedef Acf::Delegate< bool(const PathType&, const ValueType&) > ValueTravEvent;
	typedef Acf::Delegate< bool(NodePtr) > NodeTravEvent;

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

// 	void Trav(const PathType& path, bool Recursive, ValueTravEvent evt) {
// 		NodePtr _Where = _MyBase::Where(path);
// 		_Trav(_Where, Recursive, evt);
// 	}

	void NodeTrav(const PathType& path, NodeTravEvent evt, bool Recursive = true) {
		NodePtr _Where = _MyBase::Where(path);
		_NodeTrav(_Left(_Where), evt, Recursive);
	}

private:
// 	bool _Trav(NodePtr node, bool Recursive, ValueTravEvent evt) {
// 		if(NULL == node) return false; 
// 		if(!evt(_Path(node), _Value(node))) {
// 			return false;
// 		}
// 
// 		if(Recursive) {
// 			_Trav(_Left(node), Recursive, evt);
// 		}
// 
// 		_Trav(_Right(node), Recursive, evt);
// 
// 		return true;
// 	}

	bool _NodeTrav(NodePtr node, NodeTravEvent evt, bool Recursive) {
		if(NULL == node) return false; 
		if(!evt(node)) { return false; }
		if(Recursive) {	_NodeTrav(_Left(node), evt, Recursive);	}
		_NodeTrav(_Right(node), evt, Recursive);

		return true;
	}

// 	void _PreOrder(NodePtr node) {
// 		if(node) {
// 			//TravEvent(_Value(node));
// 			_PreOrder(_Left(node));
// 			_PreOrder(_Right(node));
// 		}
// 	}
// 
// 	void _MidOrder(NodePtr node) {
// 		if(node) {
// 			_MidOrder(_Left(node));
// 			//TravEvent(_Value(node));
// 			_PreOrder(_Right(node));
// 		}
// 	}
// 
// 	void _BehindOrder(NodePtr node) {
// 		if(node) {
// 			_BehindOrder(_Left(node));
// 			_BehindOrder(_Right(node));
// 			//TravEvent(_Value(node));
// 		}
// 	}
};


#endif
