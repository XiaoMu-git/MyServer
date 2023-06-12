#include "Allocter.h"
#include "MemoryPool.h"
#include "HighTimer.h"

class MyClass {
public:
	char* _name;
	int _age;
	int _score;

	MyClass(const char* name, int age, int score) {
		_name = new char[strlen(name) + 1];
		strcpy(_name, name);
		_age = age;
		_score = score;
	}

	~MyClass() {
		delete _name;
	}

	void print() {
		printf("name<%s>, age<%d>, score<%d>\n", _name, _age, _score);
		printf("%d\n", strlen(_name));
	}
};

int main() {
	MyClass* t = new MyClass("ÕÅÈý", 10, 10);
	t->print();
	delete t;

	return 0;
}