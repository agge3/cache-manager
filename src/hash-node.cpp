template <typename K, typename V>
class HashNode {
public:
	HashNode(const K& key, const V& value) :
		_key(key), _value(value), _next(nullptr) {}
	K get_key() const { return _key; }
	V get_value() const { return _value; }
	void set_value(V value) { _value = value; }
	HashNode* get_next() const { return _next; }
	void set_next(HashNode* next) { _next = next; }
private:
	K key;
	V value;
	// Next bucket with same key.
	HashNode* _next;
};
