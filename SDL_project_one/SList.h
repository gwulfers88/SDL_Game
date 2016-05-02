/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/12/2016
	--------------------------
	File:	SList.h

	Desc: Singly Linked List.
*/

#ifndef SLIST_H
#define SLIST_H

#include <iostream>

using namespace std;

template<class type>
struct Node
{
	type value;
	uint32 index;
	Node<type>* next;
};

template<class type>
class SList
{
public:
	SList()
	{
		count = 0;
		tail = head = 0;
	}

	~SList()
	{
		//Clear all data!!!
		Clear();
	}

	void Clear()
	{
		if(head)
		{
			Node<type>* current = head;
			Node<type>* next = head;

			while(next != 0)
			{
				next = next->next;
				delete current;
				current = next;
			}

			count = 0;
			head = 0;
			tail = 0;
		}
	}

	void Insert(type newVal)
	{
		Node<type> *newNode = new Node<type>();
		newNode->value = newVal;
		newNode->next = 0;
		newNode->index = count;

		if(!head)
		{
			head = newNode;
			tail = head;
			count++;
		}
		else
		{
			tail->next = newNode;
			tail = newNode;
			count++;
		}		
	}

	// TODO (George): Make better delete function (double Pointers)
	void RemoveItem( uint32 index )
	{
		if(head)
		{
			Node<type>* removeNode = head;

			if(head->index == index)
			{
				if(head->next)
				{
					head = head->next;
				}

				count--;
				delete removeNode;
			}

			Node<type>* iter = head;

			while(iter->next != 0)
			{
				if(iter->index == index)
				{
					break;
				}
			}

			removeNode = iter;

			if(iter->next)
			{
				iter = iter->next;
			}

			count--;
			delete removeNode;
		}
	}

	uint32 GetCount()
	{
		return count;
	}

	void Print()
	{
		if(head)
		{
			for(Node<type>* iter = head; iter != 0; iter = iter->next)
			{
				cout << "" << iter->value << endl;
			}
		}
	}

	type GetByType(type value)
	{
		if(head)
		{
			Node<type>* iter = head;

			while(iter)
			{
				if(iter->value == value)
				{
					return iter->value;
				}

				iter = iter->next;
			}
		}

		return 0;
	}

	type GetByIndex(uint32 index)
	{
		if(head)
		{
			Node<type>* iter = head;

			while(iter)
			{
				if(iter->index == index)
				{
					return iter->value;
				}

				iter = iter->next;
			}
		}

		return 0;
	}

private:
	Node<type>* head;
	Node<type>* tail;

	uint32 count;
};

#endif