#include <string>
#include <vector>
#include <memory>
using namespace std;

class MessengeListener {
public:
	
	virtual void isItemRemake(const string& itemName) = 0;
	virtual void isAddItemBySection(const string& ItemName , const string& SectionName) = 0;
	virtual void isItemDeleteFromSection(const string& ItemName, const string& SectionName) = 0;

	virtual void isAddSection(const string& SectionName) = 0;
	virtual void isDeleteSection(const string& SectionName) = 0;

	virtual void isSave() = 0;
	virtual void isLoad() = 0;
	
	virtual void save() = 0;
};
class Item {
private:
	MessengeListener* listener = nullptr;
	string name;
	string id;
	double price;
	double quantity;
	
public:
	~Item() {
		listener = nullptr;
	}
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
		if(price >= 0)
			this->price = price;
		if(quantity >= 0)
			this->quantity = quantity;

		if (listener) { listener->isItemRemake(this->name); listener->save(); }
	}
	
	void setListener(MessengeListener* listener) { this->listener = listener; }
};
class ItemSection {
private:
	vector<Item*> items;
	string name;
	string id;
	MessengeListener* listener = nullptr;
	
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
		listener = nullptr;
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

	void setListener(MessengeListener* listener) { 
		this->listener = listener;
		if (items.empty())return;
		for (const auto item : items) {
			item->setListener(listener);
		}
	}
	
	void addItemToSection(string itemName) {

		if (itemName.empty() || itemName == "nullptr") return;
		
		items.push_back(new Item(itemName, id.append(to_string(items.size()))));
		items[items.size() - 1]->setListener(listener);
		if (listener) { listener->isAddItemBySection(itemName, this->name); listener->save();}
	}
	void addItemToSection(string itemName, double price, double quantity) {

		if (itemName.empty() || itemName == "nullptr") return;
		
		size_t quantity_longInt = quantity;
		items.push_back(new Item(itemName, id.append(to_string(items.size())) , price , quantity_longInt));
		items[items.size() - 1]->setListener(listener);
		if (listener) { listener->isAddItemBySection(itemName, this->name); listener->save();}
	}

	
	
	void deleteItemFromSection(const size_t& item) {
		if (item < 0 || item >= items.size())return;
		string tempItemName = items[item]->getItemName();
		delete items[item];
		items.erase(items.begin() + item);
		
		if (listener) { listener->isItemDeleteFromSection(tempItemName, this->name); listener->save();}
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