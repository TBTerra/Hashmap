#pragma once
#include <stdint.h>
#include <vector>

bool ispowerof2(unsigned int x){
	return x && !(x & (x - 1));
}

template <typename K, typename V>
class HashNode{
	public:
		HashNode(const K &key, const V &value):_key(key),_value(value),_next(NULL){}
		K _key;
		V _value;
		HashNode<K,V>* _next;
};

template <typename K, typename V>
class HashMap{
	public:
		HashMap(uint32_t(*hashF)(K), uint32_t space=64){//construct
			if(!ispowerof2(space)){
				space=64;
			}
			hashFunc = hashF;
			capasity = space;
			size = 0;
			maxSize = 0.75f*(float)capasity;
			mask = capasity-1;
			table = new HashNode<K,V>*[capasity];
			for(uint32_t i=0;i<capasity;i++){
				table[i]=NULL;
			}
		}
		~HashMap(){//de-construct
			for(uint32_t i=0;i<capasity;i++){
				HashNode<K,V>* entry = table[i];
				while(entry != NULL){
					HashNode<K,V>* prev = entry;
					entry = entry->_next;
					delete prev;
				}
			}
			delete table;
		}
		
		bool contains(const K &key){
			uint32_t hash = hashFunc(key);
			HashNode<K,V>* entry = table[hash&mask];
			while(entry!=NULL){
				if(entry->_key == key)return true;
				entry = entry->_next;
			}
			return false;
		}
		
		void set(const K &key, const V &value){
			uint32_t hash = hashFunc(key);
			HashNode<K,V>* entry = table[hash&mask];
			HashNode<K,V>* prev = NULL;
			while((entry!=NULL)&&(entry->_key != key)){
				prev = entry;
				entry = entry->_next;
			}
			if(entry==NULL){
				entry = new HashNode<K,V>(key,value);
				if(prev==NULL){
					table[hash&mask] = entry;
				}else{
					prev->_next = entry;
				}
				size++;
				if(size>maxSize){
					resize();
				}
			}else{
				entry->_value = value;
			}
		}
		
		V* get(const K &key){
			uint32_t hash = hashFunc(key);
			HashNode<K,V>* entry = table[hash&mask];
			while(entry!=NULL){
				if(entry->_key == key){
					return &(entry->_value);
				}
				entry = entry->_next;
			}
			return NULL;
		}
		
		void remove(const K &key){
			uint32_t hash = hashFunc(key);
			HashNode<K,V>* entry = table[hash&mask];
			HashNode<K,V>* prev = NULL;
			while((entry!=NULL)&&(entry->_key != key)){
				prev = entry;
				entry = entry->_next;
			}
			if(entry==NULL){
				return;
			}else{
				if(prev==NULL){
					table[hash&mask] = entry->_next;
				}else{
					prev->_next = entry->_next;
				}
				delete entry;
				size--;
			}
		}
		
		uint32_t len(){
			return size;
		}
		
		uint32_t items(std::vector<K> &keys, std::vector<V> &values){
			keys.clear();
			values.clear();
			for(uint32_t i=0;i<capasity;i++){
				HashNode<K,V>* entry = table[i];
				while(entry != NULL){
					keys.push_back(entry->_key);
					values.push_back(entry->_value);
					entry = entry->_next;
				}
			}
		}
	private:
		void resize(){
			uint32_t oldCapacity = capasity;
			capasity *= 2;
			maxSize = 0.75f*(float)capasity;
			size = 0;
			mask = capasity-1;
			HashNode<K,V>* *oldTable = table;
			table = new HashNode<K,V>*[capasity];
			for(uint32_t i=0;i<capasity;i++){
				table[i]=NULL;
			}
			for(uint32_t i=0;i<oldCapacity;i++){
				HashNode<K,V>* entry = oldTable[i];
				while(entry != NULL){
					set(entry->_key,entry->_value);
					HashNode<K,V>* prev = entry;
					entry = entry->_next;
					delete prev;
				}
			}
			delete oldTable;
		}
		uint32_t(*hashFunc)(K);
		uint32_t capasity;
		uint32_t size;
		uint32_t mask;
		uint32_t maxSize;
		HashNode<K,V>* *table;
};