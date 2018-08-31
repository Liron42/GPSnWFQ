#ifndef HASHTABLE_H
#define HASHTABLE_H

#define HASH_TABLE_SIZE 50

typedef struct DataItem {
	flow* data;
	int key;
	DataItem * next;

}DataItem;

flow *search(int key);
void insert(int key, int data);
flow* CreateNewFlow(int weight, float lastVal);


#endif /* HASHTABLE_H */