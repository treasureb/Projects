#pragma once

#include"HuffmanTree.h"
#include<algorithm>
#pragma warning(disable:4996)

struct CharInfo
{
	unsigned char ch; //�ַ�
	string code;  //����
	long long count;  //���ִ���

	bool operator != (const CharInfo& info)
	{
		return info.count != count;
	}

	CharInfo operator+(const CharInfo& info)
	{
		CharInfo ret;
		ret.count = count + info.count;
		return ret;
	}

	bool operator < (const CharInfo& info)
	{
		return count < info.count;
	}

};

class FileCompress
{
public:
	typedef HuffmanTreeNode<CharInfo> Node;
	FileCompress()
	{
		for (int i = 0; i < 256; i++)
		{
			_infos[i].ch = i;
			_infos[i].count = 0;
		}
	}
	void Compress(const char* file)
	{
		//ͳ���ַ����ֵĴ���

		assert(file);
		FILE* fout = fopen(file, "r");
		assert(fout);

		char ch = fgetc(fout);
		while (ch != EOF)
		{
			_infos[ch].count++;
			ch = fgetc(fout);
		}

		//������������

		CharInfo invalid;
		invalid.ch = 0;
		HuffmanTree<CharInfo> tree(_infos, 256, invalid);

		//���ɹ���������
		string code;
		GetHuffmanCode(tree.GetRoot(),code);

		//ѹ��
		string compressFile = file;
		compressFile += ".huffman";
		FILE* fin = fopen(compressFile.c_str(),"w");
		fseek(fout, 0, SEEK_SET);//���ļ��ص��ʼ
		ch = fgetc(fout);
		char value = 0;
		int pos = 0;//�ж��Ƿ���һ���ֽ�
		while (ch != EOF)
		{
			string& code = _infos[ch].code;
			for (size_t i = 0; i < code.size(); ++i)
			{
				if (code[i] == '0')
				{
					value &= ~(1 << pos);
				}

				else if (code[i] == '1')
				{
					value |= (1 << pos);
				}

				else
				{
					assert(false);
				}

				++pos;
				
				if (pos == 8)
				{
					fputc(value, fin);
					pos = 0;
					value = 0;
				}
			}
			
			ch = fgetc(fout);
		}

		if (pos != 0)
		{
			fputc(value, fin);
		}
		fclose(fin);
		fclose(fout);
	}


	void UnCompress(const char* file)
	{
		assert(file);

		FILE* fout = fopen(file, "r");
		assert(fout);

		string uncompressFile = file;
		size_t pos = uncompressFile.rfind('.');
		uncompressFile.erase(pos, uncompressFile.size() - pos);
		uncompressFile += ".unhuffman";
		FILE* fin = fopen(uncompressFile.c_str(), "w");
		assert(fin);

		//�ؽ�huffmanTree

		CharInfo invalid;
		invalid.count = 0;
		HuffmanTree<CharInfo> tree(_infos, 256, invalid);
		Node *Root = tree.GetRoot();
		Node *cur = Root;
		long long count = Root->_weight.count;

		char value = fgetc(fout);
		while (value != EOF)
		{
			for (size_t pos = 0; pos < 8; ++pos)
			{
				if (value & (1 << pos))
					cur = cur->_Right;
				else
					cur = cur->_Left;

				if (cur->_Left == NULL && cur->_Right == NULL)
				{
					fputc(cur->_weight.ch, fin);
					cur = Root;
					if (--count == 0)
					{
						break;
					}
				}
			}

			value = fgetc(fout);
		}
		fclose(fout);
		fclose(fin);
	}

protected:

	void GetHuffmanCode(Node* Root, string code)//ͨ���ݹ���Ȼ�������򵥣�������Ϊ�ݹ�ÿһ�㶼ѹ��string�����ȥ������Ч�ʺܵ�
	{
		if (Root == NULL)
			return;

		if (Root->_Left == NULL && Root->_Right == NULL)
		{
			_infos[Root->_weight.ch].code = code;
			return;
		}

		GetHuffmanCode(Root->_Left, code+'0');//����string������+��֧�ּ�һ���ַ��������ַ�
		GetHuffmanCode(Root->_Right, code+'1');
	}
	//void GetHuffmanCode(Node *Root)
	//{
	//	if (Root == NULL)
	//		return;

	//	if (Root->_Left == NULL && Root->_Right == NULL)//�ж��Ƿ�ΪҶ�ӽڵ�
	//	{
	//		string& code = _infos[Root->_weight.ch].code;//���ʹ�ø�ֵ������Ļ�����Ƚ�ӷ��
	//		Node *cur = Root;
	//		Node *Parent = Root->_Parent;
	//		while (Parent)
	//		{
	//			if (cur == Parent->_Left)
	//			{
	//				code.push_back('0');
	//			}
	//			else
	//			{
	//				code.push_back('1');
	//			}
	//			cur = Parent;
	//			Parent = Parent->_Parent;
	//		}

	//		reverse(code.begin(), code.end());
	//	}

	//	GetHuffmanCode(Root->_Left);
	//	GetHuffmanCode(Root->_Right);
	//};
private:
	CharInfo _infos[256];  //��ÿ���ַ���ASCIIֵ��Ӧ������±�
};

void TestFileCompress()
{
	FileCompress fc;
	fc.Compress("Input.txt");
	fc.UnCompress("Input.txt.huffman");
}