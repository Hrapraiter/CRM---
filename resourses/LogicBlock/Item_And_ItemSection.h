#include <string>
#include <vector>
#include <memory>
using namespace std;

class Item {
private:
	string name;
	string id;
	double price;
	double quantity;
	
public:

	bool operator==(Item& obj) {
		if (this->id == obj.id)return true;
		else return false;
	}
	
	Item(string name, string id) :name(name), id(id) , price(0),quantity(1){}
	Item(string name ,string id , double price , double quantity):name(name),id(id),price(price),quantity(quantity){}

	const string& getItemName() const{ return name; }
	const string& getItemID()const { return id; }
	const double& getItemPrice()const { return price; }
	const double& getItemQuantity()const { return quantity; }

	void setItemRemake(string itemName, double price, double quantity) {
		if (!itemName.empty())
			this->name = itemName;
		if(price > 0)
			this->price = price;
		if(quantity > 0)
			this->quantity = quantity;
	}
	
};
class ItemSection {
private:
	vector<Item*> items;
	string name;
	string id;

	
public:

	bool operator==(ItemSection& obj) {
		if (this->id == obj.id)return true;
		else return false;
	}
	~ItemSection() {
		for (Item * item:items) {
			delete item;
		}
		items.clear();
	}
	ItemSection(string name, string id) :name(name), id(id){}


	Item* getItemByName(const string& itemName) {
		for (auto& item : items) {
			if (item->getItemName() == itemName)return item;
		}
		return nullptr;
	}
	const string& getItemSectionName() const{ return name; }
	const string& getItemSectionID() const{ return id; }
	vector<Item*>& getItemsContainer() { return items; }

	void setItemSectionName(const string& itemSectionName) { this->name = itemSectionName; }

	
	
	void addItemToSection(string itemName) {

		if (itemName.empty() || itemName == "nullptr") return;
		
		items.push_back(new Item(itemName, id.append(to_string(items.size()))));
	}
	void addItemToSection(string itemName, double price, double quantity) {

		if (itemName.empty() || itemName == "nullptr") return;
		
		size_t quantity_longInt = quantity;
		items.push_back(new Item(itemName, id.append(to_string(items.size())) , price , quantity_longInt));
	}

	
	
	void deleteItemFromSection(const size_t& item) {
		if (item < 0 || item >= items.size())return;
		delete items[item];
		items.erase(items.begin() + item);
		
	}


	
private:
	bool itemIsSection(Item* itemFind) {
		if (itemFind->getItemID() == "-1") return-1;
		for (int i = 0; i < items.size(); i++) {
			if (*itemFind == *items[i])return i;
		}

		return -1;
	}
	
};