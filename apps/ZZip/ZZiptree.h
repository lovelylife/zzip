
#ifndef __TREE_H__
#define __TREE_H__

#include <list>

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
	}

public:
	// ���path�������򷵻ؿ�ָ�룬 ��insert֮ǰ���ж�path�Ƿ����
	NodePtr insert(const PathType& path, const ValueType& v) {
		NodePtr _Where = where(path);
		if(NULL == _Where) { // ����·���ڵ�
			_Where = kdir(path); 
		}
		// ����Ҷ�ڵ�
		NodePtr _newnode = _buy_node(v, _Where, true);
		return _newnode;
	}

	NodePtr where(const PathType& path) {
		// ����path���ڽڵ�
		NodePtr node = _Root();
		PathType::const_iterator cit = path.begin();
		for(; cit != path.end() && (NULL != node); cit++) {
			if((*cit) == _Key(node)) {
				node = _Left(node); // �Һ���
			} else {
				node = _Right(node); // ���ֵ�
			}
		}
		return node;
	}

	// ����·����������һ����Ҷ�ڵ���ʧ��
	NodePtr kdir(const PathType& path) {
		NodePtr _rootnode = _Root();
		NodePtr _wherenode = _rootnode;
		NodePtr _where_construct = _rootnode;
		PathType::const_iterator cit = path.begin();
		for(; cit != path.end() && (NULL != _wherenode); cit++) {
			// �Ƿ���ڣ�*cit���ӽڵ�
			NodePtr _temp_node = _wherenode;
			bool node_exist = false;
			while(NULL != _temp_node) {
				if((*cit) == _Key(_temp_node)) {
					// �ýڵ����
					node_exist = true;
					break;
				}
				_temp_node = _Right(_temp_node); // �����ֵ�
			}
			if(!node_exist) {	// ����				
				_where_construct = _wherenode;
				break;
			} else {
				_wherenode = _Left(_wherenode);
			}
		}
		
		// ��ͷcit��_where_construct ��ʼ�����Ҷ�ڵ�
		for(; cit != path.end(); cit++) {
			NodePtr _newnode = _buy_node(_MyBase::_Mv(*cit, MapType()), _where_construct, false);
			_where_construct = _newnode;
		}
		return _where_construct;
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

	static bool _IsLeaf(NodePtr) {
		return (*node)._IsNil;
	}
protected:
	// ����ֻ�и��ڵ�Ķ�����
	void _init() {
		_Header = _buy_node();
	}

	// ������
	void _tidy() {
		_destory(_Root());
		_Header = 0;
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
//	typedef typename _MyBase::KeyCompare KeyCompare;

	ZZipTree() 
		: _MyBase() {
	}

	~ZZipTree() {
	}
};


#endif
