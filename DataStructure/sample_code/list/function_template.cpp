#include <iostream>
#include <string>

using namespace std;

//#define SWAP(t, a, b)     \
//do                        \
//{                         \
//	t c = a;              \
//	a = b;                \
//	b = c;                \
//}while(0)
//
//void Swap(int& a, int& b)
//{
//	int c = a;
//	a = b;
//	b = c;
//}

template<typename T>
void Swap(T& a, T& b) {
	T c = a;
	a = b;
	b = c;
}

template<typename T>
void Sort(T a[], int len) {
	for (int i = 0; i < len; i++) {
		for (int j = i; j < len; j++) {
//			cout << "a[i] = " << a[i] << " a[j] = " << a[j] << endl;
			if (a[i] > a[j]) {
				Swap(a[i], a[j]);
			}
		}
	}
}

template<typename T>
void Println(T a[], int len) {
	for (int i = 0; i < len; i++) {
		cout << a[i] << ", ";
	}

	cout << endl;
}

//int main() {
//	int a[5] = { 5, 4, 3, 2, 1 };
//	Println(a, 5);
//	Sort(a, 5);
//	Println(a, 5);
//
//	string s[5] = { "java", "c++", "pascal", "ruby", "basic" };
//	Println(s, 5);
//	Sort(s, 5);
//	Println(s, 5);
//
//	float ab[5] = { 5.2, 4.123, 3.123, 2.455, 1.535 };
//	Println(ab, 5);
//	Sort(ab, 5);
//	Println(ab, 5);
//
//	return 0;
//}

