#pragma once

#include"Heap.h"

template<class T>
struct HuffmanTreeNode
{
	HuffmanTreeNode* _Left;
	HuffmanTreeNode* _Right;
	HuffmanTreeNode* _Parent;//添加指向父亲的结点是为了获取Huffman编码方便
	T _weight;

	HuffmanTreeNode(T& data)
		:_Left(NULL)
		, _Right(NULL)
		, _Parent(NULL)
		, _weight(data)
	{}
};


template<class W>
class HuffmanTree
{
	typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree()
	{}

	HuffmanTree(W array[],size_t size,const W& invalid)
	{
		assert(array && size >= 0);
		_CreateHuffmanTree(array, size, invalid);
	}

	~HuffmanTree()
	{
		_DestroyHuffmantree(_Root);
	}

	 Node* GetRoot()const
	{
		return _Root;
	}

private:

	void _CreateHuffmanTree(W array[], size_t size, const W& invalid)
	{
		struct NodeCompareNode//按Node里面的权值比较，否则就是两个指针在比较了
		{
			bool operator()(Node* left, Node* right)
			{
				return left->_weight < right->_weight;
			}
		};

		Heap<Node* ,NodeCompareNode> hp;

		for (size_t index = 0; index < size; ++index)
		{
			if (array[index] != invalid)
				hp.Push(new Node(array[index]));
		}

		while (hp.Size() > 1)
		{
			Node *left = hp.Top();
			hp.Pop();
			Node *right = hp.Top();
			hp.Pop();

			Node *parent = new Node(left->_weight + right->_weight);//两个CharInfo类型，需要重载+
			left->_Parent = parent;
			right->_Parent = parent;
			parent->_Left = left;
			parent->_Right = right;

			hp.Push(parent);
		}
		if (!hp.Empty())
		_Root = hp.Top();
	}

	void _DestroyHuffmantree(Node* Root)
	{
		if (Root == NULL)
			return;

		if (Root->_Left)
			_DestroyHuffmantree(Root->_Left);

		if (Root->_Right)
			_DestroyHuffmantree(Root->_Right);

		delete Root;
		Root = NULL;
	}
private:
	HuffmanTree(const HuffmanTree<W>&);
	HuffmanTree<W>& operator=(const HuffmanTree<W>&);
private:
	Node *_Root;
};
