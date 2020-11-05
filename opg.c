#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define sentenceSize 1024
#define ReduceFailed REException()
#define SignUndefined EException()
#define UnableToCompare EException()
#define OtherError EException()
#define Undefined 65535

typedef struct Stack {
	int base, top;
	char sign[sentenceSize];
};

typedef struct Sentence {
	int ptr;
	int len;
	char content[sentenceSize];
};

struct Stack stack;
struct Stack tmpStack;
struct Sentence s;
const int priorityMatrix[6][6] = {
	{1, -1, -1, 1, -1, 1},
	{1, 1, -1, 1, -1, 1},
	{-1, -1, -1, 0, -1, 1},
	{1, 1, Undefined, 1, Undefined, 1},
	{1, 1, Undefined, 1, Undefined, 1},
	{-1, -1, -1, -1, -1, Undefined}
};

// ��ʼ��
void init()
{
	stack.base = 0;
	stack.top = -1;
	memset(stack.sign, 0, sizeof(stack.sign));
	tmpStack.base = 0;
	tmpStack.top = -1;
	memset(tmpStack.sign, 0, sizeof(tmpStack.sign));
	s.ptr = 0;
	s.len = strlen(s.content);
	s.content[s.len] = '#';
}

// ��Լ
void DoReduce()
{
	printf("R\n");
}

// ��Լʧ��
void REException()
{
	printf("RE\n");
}

// ����ʶ����޷��ȽϷ������ȹ�ϵ��ջ���Ͷ������
void EException()
{
	printf("E\n");
}

// �Ƿ���Vt
bool isVt(char c)
{
	switch (c) 
	{
	case '+':
	case '*':
	case '(':
	case ')':
	case 'i':
	case '#':return true;
	default:return false;
	}
}

// �Ƿ���Vn
bool isVn(char c)
{
	switch (c)
	{
	case 'N':return true;
	default:return false;
	}
}

// ����
int findIndex(char vt)
{
	switch (vt)
	{
	case '+':return 0;
	case '*':return 1;
	case '(':return 2;
	case ')':return 3;
	case 'i':return 4;
	case '#':return 5;
	default:SignUndefined; return -1;
	}
}

// �Ѵ�С
int stacklen()
{
	return stack.top - stack.base + 1;
}

// ѹջ
void stackpush(char c)
{
	stack.top++;
	stack.sign[stack.top] = c;
}

// ����
void read(char c)
{
	printf("I%c\n", c);
	stackpush(c);
}

bool expect(char src, char dst)
{
	if (src == dst)
		return true;
	return false;
}

// ��ջ
char stackpop()
{
	if (stacklen() <= 0)
	{
		return '\0';
	}
	stack.top--;
	return stack.sign[stack.top + 1];
}

void tmpStackpush()
{
	tmpStack.sign[++tmpStack.top] = stack.sign[stack.top--];
}

void tmpStackpop()
{
	stack.sign[++stack.top] = tmpStack.sign[tmpStack.top--];
}

// ��ȡջ��Vt����
char stackgetVt()
{
	for (int i = stack.top; i >= stack.base; i--)
	{
		if (isVt(stack.sign[i]))
			return stack.sign[i];
	}
	return '#';
}

int stackgetNextVtIndex(int src)
{
	for (int i = src; i >= stack.base; i--)
	{
		if (isVt(stack.sign[i]))
			return i;
	}
	return -1;
}

// ��Լ�����ض���
bool reduceLPrime(int left, int right)
{
	if (left > right)
		return false;
	if (right - left == 2)
	{
		if (stack.sign[left] == 'N' && stack.sign[left + 1] == '+' && stack.sign[left + 2] == 'N')
		{
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpush();
			for (int i = left; i <= right; i++)
				stackpop();
			stackpush('N');
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpop();
			return true;
		}
		else if (stack.sign[left] == 'N' && stack.sign[left + 1] == '*' && stack.sign[left + 2] == 'N')
		{
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpush();
			for (int i = left; i <= right; i++)
				stackpop();
			stackpush('N');
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpop();
			return true;
		}
		else if (stack.sign[left] == '(' && stack.sign[left + 1] == 'N' && stack.sign[left + 2] == ')')
		{
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpush();
			for (int i = left; i <= right; i++)
				stackpop();
			stackpush('N');
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpop();
			return true;
		}
		else return false;
	}
	else if (right == left)
	{
		if (stack.sign[left] == 'i')
		{
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpush();
			stackpop();
			stackpush('N');
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpop();
			return true;
		}
		else
			return false;
	}
	return false;
}

// Ѱ�������ض���
bool findLPrime()
{
	char formerSign = '#';
	char currentSign = '#';
	int src = 0;
	int dst = 0;
	for (int i = stack.base; i <= stack.top; i++)
	{
		if (isVt(stack.sign[i]))
		{
			dst = i;
			currentSign = stack.sign[i];
			if (priorityMatrix[findIndex(formerSign)][findIndex(currentSign)] > 0)
			{
				// ��Լ
				if (src >= 1 && isVn(stack.sign[src - 1]))
					src--;
				if (dst <= stack.top - 1 && isVn(stack.sign[dst + 1]))
					dst++;
				return reduceLPrime(src, dst);
			}
			else if (priorityMatrix[findIndex(formerSign)][findIndex(currentSign)] < 0)
			{
				// �������
				src = i;
				formerSign = currentSign;
			}
		}
	}
	// ��Լ
	if (src >= 1 && isVn(stack.sign[src - 1]))
		src--;
	if (dst <= stack.top - 1 && isVn(stack.sign[dst + 1]))
		dst++;
	return reduceLPrime(src, dst);
}

// ��Լ
bool reduce()
{
	return findLPrime();
}

// �﷨����������
void analyseGrammar() 
{
	if (strlen(s.content) <= 0)
	{
		OtherError;
		return;
	}

	init();

	while (s.ptr < s.len || stacklen()>1 || stack.sign[stack.top] != 'N')
	{
		if (!isVt(s.content[s.ptr]))
		{
			SignUndefined;
			return;
		}
		else if (priorityMatrix[findIndex(stackgetVt())][findIndex(s.content[s.ptr])] == Undefined)
		{
			UnableToCompare;
			return;
		}
		else if (priorityMatrix[findIndex(stackgetVt())][findIndex(s.content[s.ptr])] <= 0)
		{
			// ѹջ
			read(s.content[s.ptr++]);
		}
		else
		{
			// ��Լ
			if (reduce())
			{
				printf("R\n");
				continue;
			}
			else
			{
				ReduceFailed;
				return;
			}
		}
	}

	return;
}

int main(int argc, void *argv)
{
	scanf("%s", &s.content);
	analyseGrammar();
	return 0;
}