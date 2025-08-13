#include <fstream>
#include"Item_And_ItemSection.h"
using namespace std;



class SectionManager {
private:

	vector<ItemSection*> itemSections;
	const string filename = "save.json";


	SectionManager() = default;
	SectionManager(const SectionManager&) = delete;
	SectionManager& operator=(const SectionManager&) = delete;
	

public:
	
	~SectionManager() {
		clearObject();
	}
	void clearObject() {
		for (ItemSection* section : itemSections) {
			delete section;
		}
		itemSections.clear();
	}

	static SectionManager& getInstance() {
		static SectionManager instance;
		return instance;
		
	}

	vector<ItemSection*>& getItemSectionsContainer() { return itemSections; }
	
	
	void save() {
		ofstream out(filename);
		if (out.is_open()) {
			out << "{\n";
			for (int i = 0; i < itemSections.size(); ++i) {
				out << "	\"section" + to_string(i) +"\" : {\n\n";
				auto& itemCont = itemSections[i]->getItemsContainer();
				

				(itemCont.empty()) ? out << "	\"sectionName\" : \"" << itemSections[i]->getItemSectionName() << "\"\n" :
					out << "	\"sectionName\" : \"" << itemSections[i]->getItemSectionName() << "\",\n";

				out << "	""\n\n";
				
				for (size_t j = 0; j < itemCont.size(); ++j) {
					out << "		\"itemName"+ to_string(j)+"\" : \"" << itemCont[j]->getItemName() << "\",\n";
					out << "		\"itemPrice" + to_string(j) + "\" : " << itemCont[j]->getItemPrice() << ",\n";
					out << "		\"itemQuantity" + to_string(j) + "\" : " << itemCont[j]->getItemQuantity() << "\n";
					(j + 1 == itemCont.size()) ? out << "\n" : out << ", \n\n";
				}

				(i + 1 == itemSections.size()) ? out << "	}\n" : out << "	},\n\n";
			}
			out << "}";
			
		}
		out.close();

	}
	void load() {
		ifstream in(filename);
		if (in.is_open()) {
			string line;

			string itemName;
			string itemPrice;
			string itemQuantity;

			size_t section = 0;
			while (getline(in , line))
			{
				if (line.find("\"section" + to_string(section) + "\" : {") != string::npos) {
					section++;
				}
				else if (line.find("\"sectionName") != string::npos) {
					addItemSection(getValue(line));
				}
				else if (line.find("\"itemName") != string::npos) {
					itemName = getValue(line);
				}
				else if (line.find("\"itemPrice") != string::npos) {
					itemPrice = getValue(line);
				}
				else if (line.find("\"itemQuantity") != string::npos) {
					itemQuantity = getValue(line);
				}
				if (!itemName.empty() && !itemPrice.empty() && !itemQuantity.empty()) {
					itemSections[itemSections.size() - 1]->addItemToSection(itemName, stod(itemPrice), stod(itemQuantity));
					itemName.clear();
					itemPrice.clear();
					itemQuantity.clear();
				}
			}
		}
		else {
			
		}
		in.close();
	}


	string getValue(const string& line) {
		size_t startIndex = line.find(": ") + 2;
		string value;
		for (startIndex; startIndex < line.size(); ++startIndex) {
			if (line[startIndex] == ',')return value;
			if (line[startIndex] == '\"')continue;
			value.push_back(line[startIndex]);
		}
		return value;
	}
	
	Item* findItemFromNumCount(const size_t& index) {
		if (itemSections.empty())return nullptr;
		size_t tempItemIndex = index;
        int sectionIndex = 0;
        auto& tempSectionCont = getItemSectionsContainer();
        for (int i = 0; i < tempSectionCont.size(); ++i) {
            if (tempItemIndex < tempSectionCont[i]->getItemsContainer().size()) {
                sectionIndex = i;
                break;
            }
            tempItemIndex -= tempSectionCont[i]->getItemsContainer().size();
        }
		return itemSections[sectionIndex]->getItemsContainer()[tempItemIndex];
	}
	
	void addItemToSection(const size_t& indexSection, string itemName , double price , double quantity) {
		if (indexSection <0 || indexSection > itemSections.size())return;
		for (auto itemSection : itemSections) {
			auto& itemCont = itemSection->getItemsContainer();
			for (auto item : itemCont) {
				if (item->getItemName() == itemName) {
					item->setItemRemake(string(), price + item->getItemPrice(), quantity + item->getItemQuantity());
					return;
				}
			}
		}
		itemSections[indexSection]->addItemToSection(itemName , price ,quantity);
	}
	void addItemToSection(const size_t& indexSection,string itemName) {
		if (indexSection <0 || indexSection > itemSections.size())return;
		for (auto itemSection : itemSections) {
			auto& itemCont = itemSection->getItemsContainer();
			for (auto item : itemCont) {
				if (item->getItemName() == itemName)return;
			}
		}
		itemSections[indexSection]->addItemToSection(itemName);
	}
	void addItemSection(string itemSectionName) {
		if (itemSectionName.empty() || itemSectionName == "nullptr") return;
		for (const ItemSection* itemSection : itemSections) {
			if (itemSection->getItemSectionName() == itemSectionName)return;
		}
		itemSections.push_back(new ItemSection(itemSectionName, to_string(itemSections.size())));
	}
	
	void deleteItemSection(ItemSection* itemSection) {
		if (ItemSectionFromContainer(itemSection) == -1)return;
		itemSections.erase(itemSections.begin() + ItemSectionFromContainer(itemSection));
	}
	void deleteItemSection(const int& index) {
		if (index < 0 || index >= itemSections.size())return;
		delete itemSections[index];
		itemSections.erase(itemSections.begin() + index);
	}





	vector<ItemSection*> findSections_toLabel(const string& line) {
		vector<ItemSection*> sectionNames;
		if (line.empty() || itemSections.empty())return sectionNames;
		
		for (ItemSection* section : itemSections) {
			if (section->getItemSectionName().find(line) != string::npos)sectionNames.push_back(section);
		}
		return sectionNames;
	}
	vector<Item*> findItems_toLabel(const string& line) {
		vector<Item*> items;
		if (line.empty() || itemSections.empty())return items;
		for (ItemSection* section : itemSections) {
			auto& itemCont = section->getItemsContainer();
			if (itemCont.empty())continue;
			for (Item* item : itemCont) {
				if (item->getItemName().find(line) != string::npos)items.push_back(item);
			}
		}
		return items;
	}
	vector<ItemSection*> findSection_isFindedItem(const vector<Item*>& items) {
		vector<ItemSection*> sections;
		if (items.empty() || itemSections.empty())return sections;
		for (ItemSection* section : itemSections) {
			for (const Item* item : items) {
				if (itemIsSection(section, item)) {
					sections.push_back(section);
					break;
				}
				
			}
		}
		return sections;
	}
	vector<vector<Item*>> sortedFindedItemsByFindedSections(vector<ItemSection*>& section, vector<Item*>& items) {
		vector<vector<Item*>> itemsSorted;
		if (items.empty()) {
			itemsSorted.resize(section.size());
			return itemsSorted;
		}
			
		
		for (size_t i = 0; i < section.size(); ++i) {
			itemsSorted.push_back(vector<Item*>());
			auto& itemCont = section[i]->getItemsContainer();
			if (itemCont.empty())continue;
			for (size_t j = 0; j < items.size(); ++j) {
				if (itemIsSection(section[i], items[j])) {
					itemsSorted[itemsSorted.size() - 1].push_back(items[j]);
				}
				
			}
		}
		return itemsSorted;
	}


	vector<ItemSection*> unificationSection(vector<ItemSection*>& sections0, vector<ItemSection*>& sections1) {

		if (sections0.empty())return sections1;
		else if (sections1.empty())return sections0;

		for (ItemSection* section0 : sections0) {
			for (size_t i = 0; i < sections1.size(); ++i) {
				if (*section0 == *sections1[i]) {
					sections1[i] = nullptr;
					sections1.erase(sections1.begin() + i);
					break;
				}
			}
		}
		for (size_t i = 0; i < sections1.size(); ++i) {
			sections0.push_back(sections1[i]);
		}
		
		return sections0;
	}

	template<class Object>
	static void clearVectorDynamicElems(vector<Object*> objects) {
		for (Object* object : objects) {
			object = nullptr;
		}
		objects.clear();
	}
	size_t findItembyName(const string& name) {
		size_t itemIndex = 0;
		for (size_t i = 0; i < itemSections.size(); ++i) {
			auto& itemCont = itemSections[i]->getItemsContainer();
			for (size_t j = 0; j < itemCont.size(); ++j) {
				if (itemCont[j]->getItemName() == name)return itemIndex + j;
			}
			itemIndex += itemCont.size();
			
		}
		return -1;
	}
	size_t findItemSectionByName(const string& itemSectionName) {
		for (size_t i = 0; i < itemSections.size();++i) {
			if (itemSections[i]->getItemSectionName() == itemSectionName)return i;
		}
		return -1;
	}

private:
	bool itemIsSection(ItemSection* itemSect ,const Item* item) {
		auto& items = itemSect->getItemsContainer();
		if (items.empty())return false;
		for (const Item* itemT : items) {
			if (item->getItemID() == itemT->getItemID())return true;
		}
		return false;
	}

	int ItemSectionFromContainer(ItemSection* itemSectionFind) {
		if (itemSectionFind == nullptr)return -1;
		for (int i = 0; i < itemSections.size(); i++) {
			if (*itemSectionFind == *itemSections[i])return i;
		}
		
		return -1;
	}
	
};