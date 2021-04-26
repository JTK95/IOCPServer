#pragma once

#include "pch.h"

//----------------------------------------------------------------------
// MY STL
//----------------------------------------------------------------------
template <typename T>
class CList
{
public:
	struct Node
	{
		T data;
		Node* next;
		Node* prev;
	};

	class CIterator
	{
	public:
		// 인자로 들어온 Node 포인터를 저장
		CIterator(Node* node = nullptr)
			:node(node)
		{}

		// 현재 노드를 다음 노드로 이동 (전위)
		CIterator& operator++()
		{
			this->node = this->node->next;

			return *this;
		}

		// (후위)
		CIterator& operator++(int)
		{
			CIterator temp = this->node;
			this->node = this->node->next;

			return temp;
		}

		CIterator& operator--()
		{
			this->node = this->node->prev;

			return *this;
		}

		CIterator& operator--(int)
		{
			CIterator temp = this->node;
			this->node = this->node->prev;

			return *this;
		}

		bool operator==(const CIterator& iter)
		{
			return this->node == iter.node;
		}

		bool operator!=(const CIterator& iter) const
		{
			return this->node != iter.node;
		}

		// 현재 노드의 데이터를 뽑음
		T& operator*() const
		{
			return this->node->data;
		}

		// 노드 반환
		Node* getNode()
		{
			return this->node;
		}

		Node* operator->() const
		{
			return this->node;
		}

		/*T operator->()
		{
			return this->node->data;
		}*/

	private:
		Node* node;
	};


public:
	// List 초기화
	CList()
		:nodeCount(0)
	{
		this->head.data = 0;
		this->head.prev = nullptr;
		this->head.next = &(this->tail);

		this->tail.data = 0;
		this->tail.prev = &(this->head);
		this->tail.next = nullptr;
	}

	~CList()
	{}

	// 첫번째 노드를 가리키는 CIterator 리턴
	CIterator begin()
	{
		CIterator begin(this->head.next);

		return begin;
	}

	// 맨끝 노드를 가리키는 CIterator 리턴
	CIterator end()
	{
		CIterator end(&(this->tail));

		return end;
	}

	// 맨 앞 노드 추가
	void push_front(T data)
	{
		Node* newNode = new Node;

		newNode->data = data;
		(this->nodeCount)++;

		newNode->prev = &(this->head);
		newNode->next = this->head.next;
		newNode->prev->next = newNode;
		newNode->next->prev = newNode;
	}

	void push_back(T data)
	{
		Node* newNode = new Node;

		newNode->data = data;
		(this->nodeCount)++;

		newNode->prev = this->tail.prev;
		newNode->next = &(this->tail);
		newNode->prev->next = newNode;
		newNode->next->prev = newNode;
	}

	void pop_front()
	{
		Node* pNode = this->head.next;

		if (pNode != &(this->tail))
		{
			pNode->data = 0;
			(this->nodeCount)--;

			pNode->next->prev = &(this->head);
			pNode->prev->next = pNode->next;

			delete pNode;
		}
	}

	void pop_back()
	{
		Node* pNode = this->tail.prev;

		if (pNode != &(this->head))
		{
			pNode->data = 0;
			(this->nodeCount)--;

			pNode->next->prev = pNode->prev;
			pNode->prev->next = &(this->tail);

			delete pNode;
		}
	}

	/*void remove(T Data)
	{
		CList<T>::CIterator iter;
		for (iter = ListInt.begin(); iter != ListInt.end(); ++iter)
		{
			if (*iter == Data)
				erase(iter);
		}
	}*/

	// 내부 노드 싸그리 지우기
	void clear()
	{
		while (this->nodeCount != 0)
		{
			pop_front();
		}
	}

	// 노드 갯수
	int size() const
	{
		return this->nodeCount;
	}

	// CIterator의 그 노드를 지움 그리고 지운 노드의 다음 노드를 가리키게 하는 CIterator 리턴
	CIterator erase(CIterator iter)
	{
		CIterator temp = iter;
		Node* pNode = temp.getNode();
		temp++;

		pNode->data = 0;
		pNode->next->prev = pNode->prev;
		pNode->prev->next = pNode->next;
		delete pNode;

		return temp;
	}

private:
	int nodeCount;
	Node head;
	Node tail;
};