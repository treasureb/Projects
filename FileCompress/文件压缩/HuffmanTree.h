#pragma once

#include"Heap.h"

template<class T>
struct HuffmanTreeNode
{
	HuffmanTreeNode* _Left;
	HuffmanTreeNode* _Right;
	HuffmanTreeNode* _Parent;
	T _w;

	HuffmanTreeNode(const T& data)
		:_Left(NULL)
		, _Right(NULL)
		, _Parent(NULL)
		, _weight(data)
	{}
};


template<class T>
class HuffmanTree
{
	typedef HuffmanTreeNode<T> Node;
public:
	HuffmanTree()
	{}
	HuffmanTree(const T& array,size_t size,const T invalid)
	{
		_CreateHuffmanTree(array, size, invalid);
	}

	~HuffmanTree()
	{
		_DestroyHuffmantree(_root);
	}

	const Node* GetRoot()const
	{
		return _Root;
	}

private:

	void _CreateHuffmanTree(const T array[], size_t size, const T invalid)
	{
		assert(array);
		struct CompareNode
		{
			bool operator()(const Node* left, const Node* right)
			{
				return left->_w < right->_w;
			}
		};

			Heap<Node* CompareNode> hp;

		for (size_t index = 0; index < size; ++index)
		{
			if (invalid != array[index])
				Heap::h
		}
	}

	void _DestroyHuffmantree(Node* Root);
private:
	Node *_Root;
	T _invalid;
};
