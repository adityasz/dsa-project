#ifndef MAX_HEAP_H
#define MAX_HEAP_H

class heap {
public:
	heap()       : sz(0) {}
	heap(int sz) : sz(0) { v = std::vector<int>(sz); }

	void push(int);
	void pop();

	inline int size() { return sz; }
	inline int top();

	inline std::vector<int>::iterator begin() { return v.begin(); }
	inline std::vector<int>::iterator end()   { return v.end(); }

private:
	int sz;
	std::vector<int> v;

	void heapify(int);

	inline int parent(int i) { return (i - 1) / 2; }
	inline int left(int i)   { return 2 * i + 1; }
	inline int right(int i)  { return 2 * i + 2; }
};

int heap::top()
{
	if (sz == 0)
		return -1;
	return v.front();
}

void heap::push(int e)
{
	int i = sz;

	if (sz == v.size())
		v.resize(2 * sz + 1);
	v[sz++] = e;

	while (i > 0 && v[parent(i)] < v[i]) {
		std::swap(v[parent(i)], v[i]);
		i = parent(i);
	}
}

void heap::pop()
{
	if (sz == 0)
		return;
	std::swap(v[0], v[sz - 1]);
	sz--;
	heapify(0);
}

void heap::heapify(int i)
{
	if (left >= size)
		return;
	
	if (right < size) {
		int c = std::max({v[i], v[left(i)], v[right(i)]});

		if (v[i] == c)
			return;
		if (v[left(i)] == c) {
			std::swap(v[i], v[left(i)]);
			heapify(left(i));
			return;
		}
		std::swap(v[i], v[right(i)]);
		heapify(right(i));
		return;
	}

	if (v[i] >= v[left(i)])
		return;
	std::swap(v[i], v[left(i)]);
}

#endif
