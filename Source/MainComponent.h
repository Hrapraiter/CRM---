#pragma once

#include <JuceHeader.h>
#include "resourses/LogicBlock/SectionController.h"
#include "ctime"
#include <future>
using namespace juce;
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/


class EventListener {
public:
    virtual ~EventListener() = default;
    virtual void onUpdateAll() = 0;
    

    virtual void textChanged() = 0;

    virtual void PanelAddItem_On_Script(int index) = 0;
    virtual void PanelAddItem_Off_Script() = 0;

    virtual void StatusAndRefactor_On_Script(size_t index) = 0;
    virtual void StatusAndRefactor_Off_Script() = 0;
};

//допилить
class ObserverEvents : public juce::Component,public MessengeListener{
private:
    String messengeStatus = "[COMPLIED]";
    SectionManager* manager;
    EventListener* listener = nullptr;
    bool* isLoaded;
    TextEditor displayWindow;
public:
    ~ObserverEvents() {
        manager = nullptr;
        listener = nullptr;
        isLoaded = nullptr;
    }
    ObserverEvents(int x , int y , int width , int height , SectionManager* manager):manager(manager){

        setBounds(x, y, width, height);

        displayWindowCustomization();
        
        srand(static_cast<unsigned int>(time(nullptr)));
        
    }
    void setListener(EventListener* listener) { this->listener = listener; }
    void setBoolIsLoaded(bool& isLoaded) { this->isLoaded = &isLoaded; }
    static string NowTime() {
        time_t now = time(nullptr);
        tm* local_time = localtime(&now);
        
        return string(to_string(local_time->tm_hour) + ":" + to_string(local_time->tm_min) + ":" + to_string(local_time->tm_sec));
    }
private:
    
    void addMessenge(const String& messenge) {
        if (!*isLoaded)return;

        displayWindow.moveCaretToEnd();
        displayWindow.insertTextAtCaret(messenge + NowTime() + "\n\n");
        repaint();
    }
    void save()override {
        manager->save();
    }
    void isItemRemake(const string& itemName) override{
        String messenge = messengeStatus + String(L" изменение предмета ( ") + String(itemName) + String(L" ) ");
        addMessenge(messenge);
    }
    void isAddItemBySection(const string& itemName, const string& SectionName) override{
        String messenge = messengeStatus + String(L" добавление предмета ( ") + String(itemName) + String(L" ) в секцию ( ") + String(SectionName) + String(L" ) ");
        addMessenge(messenge);
    }
    void isItemDeleteFromSection(const string& itemName, const string& SectionName) override {
        String messenge = messengeStatus + String(L" удаление предмета ( ") + String(itemName) + String(L" ) из секции ( ") + String(SectionName) + String(L" )");
        addMessenge(messenge);
    }
    
    void isAddSection(const string& SectionName) override {
        String messenge = messengeStatus + String(L" добавление секции ( ") + String(SectionName) + String(L" ) ");
        addMessenge(messenge);
    }
    void isDeleteSection(const string& SectionName) override {
        String messenge = messengeStatus + String(L" удаление секции ( ") + String(SectionName) + String(L" ) ");
        addMessenge(messenge);
    }

    void isSave() override {
        String messenge = messengeStatus + String(L" сохранение. ");
        addMessenge(messenge);
    }
    void isLoad() override {
        String messenge = messengeStatus + String(L" загрузка сохранения. ");
        addMessenge(messenge);
        if (listener) { listener->onUpdateAll(); }
    }

    void displayWindowCustomization() {
        addAndMakeVisible(displayWindow);
        displayWindow.setBounds(getBounds());
        displayWindow.setMultiLine(true);
        displayWindow.setReadOnly(true);
        displayWindow.setScrollbarsShown(true);
        
    }
};

class LabelComponent : public juce::Component , juce::TextEditor::Listener {
private:
    EventListener* listener = nullptr;
    TextEditor searchLabel;
public:

    LabelComponent() = default;
    LabelComponent(int x , int y , int width ,const String& textIsEmpty) { 
        setBounds(x ,y ,width , 30);
        
        searchLabelCustomization(textIsEmpty);
    }

    ~LabelComponent() { 
        searchLabel.removeListener(this);
        searchLabel.setLookAndFeel(nullptr);
        listener = nullptr; 
    }

    void setListener(EventListener* listener) { this->listener = listener; }

    void searchLabelCustomization(const String& textIsEmpty) {
        addAndMakeVisible(searchLabel);
        searchLabel.setMultiLine(false);
        searchLabel.setReturnKeyStartsNewLine(false);
        searchLabel.setEscapeAndReturnKeysConsumed(false);
        searchLabel.setTextToShowWhenEmpty(textIsEmpty, Colours::grey);
        searchLabel.setSize(getWidth() , 30);
        searchLabel.addListener(this);

    }
    void clear() { searchLabel.clear(); }
    bool empty() { return searchLabel.isEmpty(); }

    String getText() { return searchLabel.getText(); }
private:

    void textEditorTextChanged(TextEditor& editor)override {
        if (&editor == &searchLabel) {
            if (listener) { listener->textChanged(); }
        }
    }
};

//будет время накидать красную подсветку в нижние 2 компонента и добавить в проект уведомления
class PanelAddItem :public juce::Component , juce::Button::Listener {
    private:

        EventListener* listener = nullptr;
        SectionManager* sectionManager;
        int panelIndex;

        Rectangle<int> shadowBackground;
        Rectangle<int> panelIteraction;

        unique_ptr<LabelComponent> labelInputName;
        unique_ptr<LabelComponent> labelInputPrice;
        unique_ptr<LabelComponent> labelInputQuantity;

        bool buttonAddedExpended_isPress;
        TextButton buttonAddedExpended;

        Image acceptImage = Image(ImageFileFormat::loadFrom(File("resourses/Images/accept.png")));
        Image undoImage = Image(ImageFileFormat::loadFrom(File("resourses/Images/undo.png")));
        ImageButton accept;
        ImageButton undo;

    public:
        ~PanelAddItem() { 
            accept.removeListener(this);
            undo.removeListener(this);
            buttonAddedExpended.removeListener(this);

            accept.setLookAndFeel(nullptr);
            undo.setLookAndFeel(nullptr);
            buttonAddedExpended.setLookAndFeel(nullptr);
            listener = nullptr;
            sectionManager = nullptr;
        }
        PanelAddItem() = default;
        PanelAddItem(SectionManager* sectionManager , int wight ,int height , int wightPanel , int heightPanel):sectionManager(sectionManager) {
            
            panelIndex = NULL;
            setSize(wight, height);

            shadowBackground.setBounds(0, 0, wight, height);
            panelIteraction.setBounds(wight / 2 - wightPanel/2, height / 2 - heightPanel/2, wightPanel, heightPanel);

            imageButtonsCustomization();
            panelsCreater();
            buttonAddedExpandedCustomization();

        }

        void setListener(EventListener* listener) { this->listener = listener; }
        void setPanelIndex(int index) { this->panelIndex = index; }

        void paint(Graphics& g)override {
            if (!this->isVisible())return;
            g.setColour(Colour(0.f, 0.f, 0.f, 0.45f));
            g.fillRect(shadowBackground);
            g.setColour(Colour(67, 87, 110));
            g.fillRect(panelIteraction);
        }


protected:

    void buttonClicked(Button* button)override {
        if (button == &undo) {

            labelInputName->clear();
            labelInputPrice->clear();
            labelInputQuantity->clear();

            labelInputPrice->setVisible(false);
            labelInputPrice->setEnabled(false);
            labelInputQuantity->setVisible(false);
            labelInputQuantity->setEnabled(false);

            buttonAddedExpended_isPress = true;

            if (listener) { listener->PanelAddItem_Off_Script(); }
        }
        if (button == &accept) {

            if (labelInputName->empty())return;
            else {
                if (labelInputPrice->empty() && labelInputQuantity->empty()) {
                    sectionManager->addItemToSection(panelIndex ,labelInputName->getText().toStdString());
                    labelInputName->clear();
                    if (listener) { listener->PanelAddItem_Off_Script(); }
                    return;
                }
                if (labelInputPrice->empty() || labelInputQuantity->empty())return;
                if (!isDouble(labelInputPrice->getText().toStdString()) || !isDouble(labelInputQuantity->getText().toStdString()))return;

                sectionManager->addItemToSection(
                    panelIndex,
                    labelInputName->getText().toStdString(),
                    stod(labelInputPrice->getText().toStdString()),
                    stod(labelInputQuantity->getText().toStdString())
                );
                labelInputName->clear();
                labelInputPrice->clear();
                labelInputQuantity->clear();
            }

            if (listener) { listener->PanelAddItem_Off_Script(); }
        }


        if (button == &buttonAddedExpended) {
            if (buttonAddedExpended_isPress) {

                buttonAddedExpended.setButtonText(L"Расширенный ↑");

                labelInputPrice->setVisible(true);
                labelInputPrice->setEnabled(true);
                labelInputQuantity->setVisible(true);
                labelInputQuantity->setEnabled(true);

                buttonAddedExpended_isPress = false;

                repaint();
            }
            else {
                buttonAddedExpended.setButtonText(L"Расширенный ↓");

                labelInputPrice->setVisible(false);
                labelInputPrice->setEnabled(false);
                labelInputQuantity->setVisible(false);
                labelInputQuantity->setEnabled(false);

                buttonAddedExpended_isPress = true;

                repaint();
            }
        }
    }


    bool isDouble(const string& str) {
        bool divider = false;
        for (int i = 0; i < str.size(); ++i) {
            if (str[i] == ',' || str[i] == '.') {
                if (!divider) { divider = true; continue; }
            }
            if (!(str[i] >= '0' && str[i] <= '9'))return false;
        }
        return true;
    }

    void imageButtonsCustomization() {

        addAndMakeVisible(accept);
        addAndMakeVisible(undo);

        accept.addListener(this);
        undo.addListener(this);

        accept.setImages(true, true, false,
            acceptImage , 1.f , Colours::transparentBlack,
            acceptImage , 0.7f , Colours::transparentBlack,
            acceptImage , 0.5f , Colours::transparentBlack
        );
        undo.setImages(true, true, false,
            undoImage, 1.f, Colours::transparentBlack,
            undoImage, 0.7f, Colours::transparentBlack,
            undoImage, 0.5f, Colours::transparentBlack
        );

        accept.setBounds(panelIteraction.getX() + panelIteraction.getWidth() - 40, panelIteraction.getY() + panelIteraction.getHeight() - 40, 40, 40);
        undo.setBounds(panelIteraction.getX(), panelIteraction.getY() + panelIteraction.getHeight() - 40, 40, 40);


    }
    
    void panelsCreater() {

        labelInputName = make_unique<LabelComponent>(
            static_cast<int>(panelIteraction.getX() + panelIteraction.getWidth() / 12),
            static_cast<int>(panelIteraction.getY() + panelIteraction.getHeight()/12),
            panelIteraction.getWidth()-(panelIteraction.getWidth() / 6),
            L"Имя предмета..." 
        );
        addAndMakeVisible(labelInputName.get());
        int indent = 50;
        labelInputPrice = make_unique<LabelComponent>(
            static_cast<int>(panelIteraction.getX() + panelIteraction.getWidth() / 12),
            static_cast<int>(panelIteraction.getY() + panelIteraction.getHeight()/12 + indent*2),
            panelIteraction.getWidth() - (panelIteraction.getWidth() / 6),
            L"Цена за шт..." 
        );
        addAndMakeVisible(labelInputPrice.get());
        labelInputQuantity = make_unique<LabelComponent>(
            static_cast<int>(panelIteraction.getX() + panelIteraction.getWidth() / 12),
            static_cast<int>(panelIteraction.getY() + panelIteraction.getHeight() / 12 + indent*3),
            panelIteraction.getWidth() - (panelIteraction.getWidth() / 6),
            L"Колличество..."
        );
        addAndMakeVisible(labelInputQuantity.get());

    }

    void buttonAddedExpandedCustomization() {

        addAndMakeVisible(buttonAddedExpended);
        buttonAddedExpended.setButtonText(L"Расширенный ↓");
        buttonAddedExpended.addListener(this);

        buttonAddedExpended.setBounds(
            panelIteraction.getX() + panelIteraction.getWidth()/2 - 75,
            labelInputPrice->getY() - (labelInputPrice->getY() - labelInputName->getY() - labelInputName->getHeight()) / 2 - 20,
            150,
            40
        );

        buttonAddedExpended_isPress = true;
        
        labelInputPrice->setVisible(false);
        labelInputPrice->setEnabled(false);
        labelInputQuantity->setVisible(false);
        labelInputQuantity->setEnabled(false);
    }
};

class StatusAndRefactorPanel : public juce::Component , juce::Button::Listener{
private:
    EventListener* listener = nullptr;
    SectionManager* sectionManager;

    int sectionIndex;
    size_t itemIndex;

    Rectangle<int> shadowBackground;
    Rectangle<int> panelIteraction;
    Rectangle<int> buttonPanel;

    Font font = Font(21.f,Font::bold);
    String name;
    String price;
    String quantity;

    Rectangle<int> hitBoxName;
    Rectangle<int> hitBoxPrice;
    Rectangle<int> hitBoxQuantity;

    unique_ptr<LabelComponent> labelInputName;
    unique_ptr<LabelComponent> labelInputPrice;
    unique_ptr<LabelComponent> labelInputQuantity;

    bool buttonRefactor_isPress;
    TextButton buttonRefactor;
    TextButton buttonItemDelete;
    TextButton buttonExitPanel;

    Image acceptImage = Image(ImageFileFormat::loadFrom(File("resourses/Images/accept.png")));
    Image undoImage = Image(ImageFileFormat::loadFrom(File("resourses/Images/undo.png")));
    ImageButton accept;
    ImageButton undo;
public:
    ~StatusAndRefactorPanel() {

        accept.removeListener(this);
        undo.removeListener(this);
        buttonRefactor.removeListener(this);
        buttonItemDelete.removeListener(this);
        buttonExitPanel.removeListener(this);

        accept.setLookAndFeel(nullptr);
        undo.setLookAndFeel(nullptr);
        buttonRefactor.setLookAndFeel(nullptr);
        buttonItemDelete.setLookAndFeel(nullptr);
        buttonExitPanel.setLookAndFeel(nullptr);
        listener = nullptr;
        sectionManager = nullptr;
    }
    StatusAndRefactorPanel(SectionManager* sectionManager, int wight, int height, int wightPanel, int heightPanel)
        : sectionManager(sectionManager) {

        itemIndex = NULL;
        setSize(wight, height);

        shadowBackground.setBounds(0, 0, wight, height);
        panelIteraction.setBounds(wight / 2 - wightPanel / 2, height / 2 - heightPanel / 2, wightPanel, heightPanel);
        

        imageButtonsCustomization();
        panelsCreater();
        buttonRefactorCustomization();
        hitBoxsCreater();
        buttonItemDeleteCustomization();
        buttonExitPanelCustomization();

        buttonPanel.setBounds(
            panelIteraction.getX() + panelIteraction.getWidth() + 2, panelIteraction.getY() - 5
            , buttonRefactor.getWidth() + 10, panelIteraction.getHeight() + 10);
    }
    void setListener(EventListener* listener) { this->listener = listener; }
    void setItemIndex(size_t index) {
        
        size_t tempItemIndex = index;
        sectionIndex = 0;
        auto& tempSectionCont = sectionManager->getItemSectionsContainer();
        for (int i = 0; i < tempSectionCont.size(); ++i) {
            if (tempItemIndex < tempSectionCont[i]->getItemsContainer().size()) {
                sectionIndex = i;
                break;
            }
            tempItemIndex -= tempSectionCont[i]->getItemsContainer().size();
        }
        itemIndex = tempItemIndex;
        auto& item = sectionManager->getItemSectionsContainer()[sectionIndex]->getItemsContainer()[itemIndex];

        
        stringGraphicsUpdate();

    }
    
    void paint(Graphics& g)override {
        if (!this->isVisible())return;

        g.setColour(Colour(0.f, 0.f, 0.f, 0.45f));
        g.fillRect(shadowBackground);
        g.setColour(Colour(67, 87, 110));
        g.fillRect(panelIteraction);
        g.setColour(Colour(58, 79, 105));
        g.fillRect(buttonPanel);

        g.setFont(font);
        g.setColour(Colour(39, 205, 242));
        g.drawText(name, hitBoxName, Justification::centredLeft, true);
        g.drawText(price, hitBoxPrice, Justification::centredLeft, true);
        g.drawText(quantity, hitBoxQuantity, Justification::centredLeft, true);
        

    }
private:
    void stringGraphicsUpdate() {
        auto& item = sectionManager->getItemSectionsContainer()[sectionIndex]->getItemsContainer()[itemIndex];
        name = String(L"Товар : " + String(item->getItemName()));
        price = String(L"Цена : " + String(item->getItemPrice()));
        quantity = String(L"Колличесво : " + String(item->getItemQuantity()));
    }
    void buttonClicked(Button* button)override {
        if (button == &undo) {

            labelInputName->clear();
            labelInputPrice->clear();
            labelInputQuantity->clear();

            visibleIteractonComponents(false);
            buttonRefactor_isPress = true;

        }
        if (button == &accept) {


            if (labelInputName->empty() && labelInputPrice->empty() && labelInputQuantity->empty())return;
            if (!isDouble(labelInputPrice->getText().toStdString()) || !isDouble(labelInputQuantity->getText().toStdString()))return;

            
            auto& item= sectionManager->getItemSectionsContainer()[sectionIndex]->getItemsContainer()[itemIndex];
            
            double price = 0;
            if (!labelInputPrice->getText().toStdString().empty())
                price = stod(labelInputPrice->getText().toStdString());

            double quantity = 0;
            if (!labelInputQuantity->getText().toStdString().empty())
                quantity = stod(labelInputQuantity->getText().toStdString());

            item->setItemRemake(
                labelInputName->getText().toStdString(),
                price,
                quantity
            );
            
            stringGraphicsUpdate();

            labelInputName->clear();
            labelInputPrice->clear();
            labelInputQuantity->clear();

            

            visibleIteractonComponents(false);
            buttonRefactor_isPress = true;
            
            repaint();

        }


        if (button == &buttonRefactor) {


            visibleIteractonComponents(buttonRefactor_isPress);

            buttonRefactor_isPress ? buttonRefactor_isPress = false : buttonRefactor_isPress = true;

            repaint();
        }
        if (button == &buttonItemDelete) {

            labelInputName->clear();
            labelInputPrice->clear();
            labelInputQuantity->clear();

            visibleIteractonComponents(false);
            buttonRefactor_isPress = true;

            sectionManager->getItemSectionsContainer()[sectionIndex]->deleteItemFromSection(itemIndex);
            if (listener)listener->StatusAndRefactor_Off_Script();
        }
        if (button == &buttonExitPanel) {

            labelInputName->clear();
            labelInputPrice->clear();
            labelInputQuantity->clear();

            visibleIteractonComponents(false);
            buttonRefactor_isPress = true;

            if (listener)listener->StatusAndRefactor_Off_Script();
            
        }
    }

    bool isDouble(const string& str) {
        if (str.empty())return true;
        bool divider = false;
        for (int i = 0; i < str.size(); ++i) {
            if (str[i] == ',' || str[i] == '.') {
                if (!divider) { divider = true; continue; }
            }
            if (!(str[i] >= '0' && str[i] <= '9'))return false;
        }
        return true;
    }
    void visibleIteractonComponents(const bool& is_Visible) {
        labelInputName->setVisible(is_Visible);
        labelInputName->setEnabled(is_Visible);

        labelInputPrice->setVisible(is_Visible);
        labelInputPrice->setEnabled(is_Visible);

        labelInputQuantity->setVisible(is_Visible);
        labelInputQuantity->setEnabled(is_Visible);

        accept.setVisible(is_Visible);
        accept.setEnabled(is_Visible);

        undo.setVisible(is_Visible);
        undo.setEnabled(is_Visible);
    }

    void imageButtonsCustomization() {

        addAndMakeVisible(accept);
        addAndMakeVisible(undo);

        accept.addListener(this);
        undo.addListener(this);

        accept.setImages(true, true, false,
            acceptImage, 1.f, Colours::transparentBlack,
            acceptImage, 0.7f, Colours::transparentBlack,
            acceptImage, 0.5f, Colours::transparentBlack
        );
        undo.setImages(true, true, false,
            undoImage, 1.f, Colours::transparentBlack,
            undoImage, 0.7f, Colours::transparentBlack,
            undoImage, 0.5f, Colours::transparentBlack
        );

        accept.setBounds(panelIteraction.getX() + panelIteraction.getWidth() - 40, panelIteraction.getY() + panelIteraction.getHeight() - 40, 40, 40);
        undo.setBounds(panelIteraction.getX(), panelIteraction.getY() + panelIteraction.getHeight() - 40, 40, 40);

        accept.setVisible(false);
        accept.setEnabled(false);

        undo.setVisible(false);
        undo.setEnabled(false);
    }

    void panelsCreater() {

        labelInputName = make_unique<LabelComponent>(
            static_cast<int>(panelIteraction.getX() + panelIteraction.getWidth() / 12),
            static_cast<int>(panelIteraction.getY() + panelIteraction.getHeight() / 4),
            panelIteraction.getWidth() - (panelIteraction.getWidth() / 6),
            L"Новое имя предмета..."
        );
        addAndMakeVisible(labelInputName.get());
        int indent = 80;
        labelInputPrice = make_unique<LabelComponent>(
            static_cast<int>(panelIteraction.getX() + panelIteraction.getWidth() / 12),
            static_cast<int>(panelIteraction.getY() + panelIteraction.getHeight() / 4 + indent * 1),
            panelIteraction.getWidth() - (panelIteraction.getWidth() / 6),
            L"Новая цена..."
        );
        addAndMakeVisible(labelInputPrice.get());
        labelInputQuantity = make_unique<LabelComponent>(
            static_cast<int>(panelIteraction.getX() + panelIteraction.getWidth() / 12),
            static_cast<int>(panelIteraction.getY() + panelIteraction.getHeight() / 4 + indent * 2),
            panelIteraction.getWidth() - (panelIteraction.getWidth() / 6),
            L"Новое колличество..."
        );
        addAndMakeVisible(labelInputQuantity.get());
        
        labelInputName->setVisible(false);
        labelInputName->setEnabled(false);

        labelInputPrice->setVisible(false);
        labelInputPrice->setEnabled(false);

        labelInputQuantity->setVisible(false);
        labelInputQuantity->setEnabled(false);
    }
    
    void buttonRefactorCustomization() {
        addAndMakeVisible(buttonRefactor);

        buttonRefactor.addListener(this);
        buttonRefactor.setButtonText(L"Редактировать");
        buttonRefactor.setBounds(panelIteraction.getX() + panelIteraction.getWidth() + 5, panelIteraction.getY(), 120, 30);

        buttonRefactor_isPress = true;
    }
    void hitBoxsCreater() {
        int workLength = panelIteraction.getWidth() - (labelInputName->getX() - panelIteraction.getX());
        hitBoxName = Rectangle<int>(labelInputName->getX(), labelInputName->getY() - 50, workLength, 50);
        hitBoxPrice = Rectangle<int>(labelInputPrice->getX(), labelInputPrice->getY() - 50, workLength, 50);
        hitBoxQuantity = Rectangle<int>(labelInputQuantity->getX(), labelInputQuantity->getY() - 50, workLength, 50);
    }
    void buttonItemDeleteCustomization() {
        addAndMakeVisible(buttonItemDelete);
        buttonItemDelete.setButtonText(L"Удалить");
        buttonItemDelete.addListener(this);
        buttonItemDelete.setBounds(buttonRefactor.getX(), buttonRefactor.getY() + buttonRefactor.getHeight() + 5,
            buttonRefactor.getWidth(), buttonRefactor.getHeight());

    }
    void buttonExitPanelCustomization() {
        addAndMakeVisible(buttonExitPanel);
        buttonExitPanel.setButtonText(L"Выход из панели");
        buttonExitPanel.addListener(this);
        buttonExitPanel.setBounds(
            buttonRefactor.getX(),
            panelIteraction.getY() + panelIteraction.getHeight() - buttonRefactor.getHeight(),
            buttonRefactor.getWidth(), buttonRefactor.getHeight());
    }
};


class ButtonContainer : public juce::Component , juce::Button::Listener, juce::ScrollBar::Listener {
private:
    TextButton buttonExit;

    TextButton buttonAddSection;

    vector<Image> imagesUndoAndAccept;
    ImageButton undoAddSection;
    ImageButton acceptAddSection;
    
    EventListener* listener = nullptr;

    SectionManager* sectionManager;
    
    
    LabelComponent labelSearch = LabelComponent(170, 20, 700, L"Название предмета или секции...");
    LabelComponent label = LabelComponent(840, 60, 200, L"Дайте название секции...");

    vector<unique_ptr<String>> sections;
    Image imageAddItem;

    vector<unique_ptr<ImageButton>> buttonsAddItem;
    vector<unique_ptr<TextButton>> buttonsDeleteSection;

    vector<unique_ptr<TextButton>> buttonsOpenItemStatus;

    vector <Rectangle<int>> sectionHitBoxs;
    vector <Rectangle<int>> itemHitBoxs;

    String debugLog;
    Rectangle<int> testBounds;
    String debug = "DEBUG";

    double userScrollPosition;
    double maxScrollPosition;

    Colour sectionFontColour = Colour(200 ,200 ,200);
    Colour sectionHitBoxColour = Colour(200 ,112 ,82);
    Font sectionFont = Font(25.f , Font::bold );

    ScrollBar scrollBar = ScrollBar(true);

    bool scrollBarVisible;

    size_t itemCount;
public:
    
    ~ButtonContainer() { 
        buttonExit.removeListener(this);
        scrollBar.removeListener(this);
        buttonAddSection.removeListener(this);
        undoAddSection.removeListener(this);
        acceptAddSection.removeListener(this);
        for (int i = 0; i < buttonsAddItem.size();++i) {
            buttonsAddItem[i]->removeListener(this);
            buttonsDeleteSection[i]->removeListener(this);
        }
        for (unique_ptr<TextButton>& button : buttonsOpenItemStatus) {
            button->removeListener(this);
        }
        
        
        buttonExit.setLookAndFeel(nullptr);
        scrollBar.setLookAndFeel(nullptr);
        buttonAddSection.setLookAndFeel(nullptr);
        undoAddSection.setLookAndFeel(nullptr);
        acceptAddSection.setLookAndFeel(nullptr);
        
        label.setLookAndFeel(nullptr);
        listener = nullptr;
        sectionManager = nullptr;
    }

    ButtonContainer(SectionManager* manager):sectionManager(manager) {


        setSize(1040, 673);

        itemCount = 0;

        testBounds = Rectangle<int>(getWidth() - 300, 90, 200, 60);

        
        addAndMakeVisible(labelSearch);

        buttonExitCustomization();
        
        scrollBarCustomization();

        buttonAddSectionCustomization();

        imageAddItem = Image(ImageFileFormat::loadFrom(File("resourses/Images/addItemImage.png")));

        
        

    }

    
    void setListener(EventListener* listener) { 
        this->listener = listener;
        labelSearch.setListener(listener);
    }
    
    void drawDebugTest(Graphics& g) {
        return;
        g.setColour(Colours::mediumvioletred);
        g.setFont(sectionFont);
        
        g.drawText(debug, testBounds.getX() + testBounds.getWidth() / 4, testBounds.getY() - testBounds.getHeight() / 2,
            testBounds.getWidth() / 2, testBounds.getHeight() / 2,
            Justification::centredBottom, true);
        g.drawRect(testBounds, 2.f);

        if (sectionManager->getItemSectionsContainer().empty())return;
        

        debugLog = String(maxScrollPosition);
        String debugLog1 = String();

        g.setColour(Colours::green);
        g.drawText(debugLog, testBounds, Justification::centredTop, true);
        g.drawText(debugLog1, testBounds, Justification::centredBottom, true);


    }
    void drawableSection(Graphics& g) {


        g.setFont(sectionFont);

        for (int i = 0; i < sectionHitBoxs.size(); i++) {
            if (sectionHitBoxs[i].getY() > 673 || sectionHitBoxs[i].getY() < -10)continue;

            g.setColour(sectionFontColour);
            g.drawText(" " + *sections[i].get(), sectionHitBoxs[i], Justification::centredLeft, true);

            g.setColour(sectionHitBoxColour);
            g.drawRect(sectionHitBoxs[i], 3.f);
        }
    }
    
    void updateAll() {
        
        string line = labelSearch.getText().toStdString();
        auto sectionsFinded = sectionManager->findSections_toLabel(line);
        auto itemsFinded = sectionManager->findItems_toLabel(line);

        
        if(line.empty()) {

            
            
            updateHitBoxItemAndSection();
            updateMaxScrollPosition();
            updateScrollBar();
            buttonViewRefactor();

            if (listener) { listener->onUpdateAll(); }
        }
        else {

            auto sectionsFindedItemIsSection = sectionManager->findSection_isFindedItem(itemsFinded);
            auto sectionsUnification = sectionManager->unificationSection(sectionsFinded, sectionsFindedItemIsSection);
            auto items = sectionManager->sortedFindedItemsByFindedSections(sectionsUnification, itemsFinded);

            SectionManager::clearVectorDynamicElems(sectionsFinded);
            SectionManager::clearVectorDynamicElems(sectionsFindedItemIsSection);

            refactorHitBoxIfDeletedOrAdded(sectionsUnification, itemsFinded);
            updateHitBoxItemAndSection(sectionsUnification, items);
            updateMaxScrollPosition(sectionsUnification, items);
            updateScrollBar();
            
            buttonViewRefactor(sectionsUnification,itemsFinded);

            if (listener) { listener->onUpdateAll(); }


            SectionManager::clearVectorDynamicElems(itemsFinded);
            SectionManager::clearVectorDynamicElems(sectionsUnification);
            for (vector<Item*>& itemCont : items) {
                SectionManager::clearVectorDynamicElems(itemCont);
            }
            items.clear();


        }

    }
    
    
    
private:
    
    void buttonClicked(Button* button) override{
        if (button == &buttonExit) {
            sectionManager->save();
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
        if (button == &buttonAddSection) {

            
            
            undoAddSection.setVisible(true);
            acceptAddSection.setVisible(true);
            label.setVisible(true);
            undoAddSection.setEnabled(true);
            acceptAddSection.setEnabled(true);
            label.setEnabled(true);
            
            
        }
        if (button == &undoAddSection) {
           
            undoAddSection.setVisible(false);
            acceptAddSection.setVisible(false);
            label.setVisible(false);
            undoAddSection.setEnabled(false);
            acceptAddSection.setEnabled(false);
            label.setEnabled(false);
            label.clear();
        }
        if (button == &acceptAddSection) {
            
            string inputSectionName = label.getText().toStdString();
            label.clear();

            undoAddSection.setVisible(false);
            acceptAddSection.setVisible(false);
            label.setVisible(false);
            undoAddSection.setEnabled(false);
            acceptAddSection.setEnabled(false);
            label.setEnabled(false);

            sectionManager->addItemSection(inputSectionName);

            updateAll();

            repaint();
        }
        
        // кнопки секции
        for (int i = 0; i < buttonsAddItem.size(); ++i) {
            if (!buttonsAddItem[i]->isVisible())continue;

            if (button == buttonsAddItem[i].get()) {
                
                if (listener)listener->PanelAddItem_On_Script(sectionManager->findItemSectionByName(sections[i]->toStdString()));
                updateAll();
                
                break;
            }

            if (button == buttonsDeleteSection[i].get()) {

                sectionManager->deleteItemSection(i);
                updateAll();

                break;
            }
        }
        for (size_t i = 0; i < buttonsOpenItemStatus.size(); ++i) {
            if (!buttonsOpenItemStatus[i]->isVisible())continue;
            
            if (button == buttonsOpenItemStatus[i].get()) {
                if (listener) { listener->StatusAndRefactor_On_Script(sectionManager->findItembyName(buttonsOpenItemStatus[i]->getButtonText().toStdString())); }
                break;
            }
        }
        
        
    }
    
    

    

    



    

    //Обычные методы для стандартной отрисовки НЕ ТРОГАТЬ!!!!!!
    void updateHitBoxItemAndSection() {
        auto& tempSectionContainer = sectionManager->getItemSectionsContainer();

        if (tempSectionContainer.empty()) {
            sectionHitBoxs.clear();
            itemHitBoxs.clear();
            return;
        }

        refactorHitBoxIfDeletedOrAdded(tempSectionContainer);


        float x = 0.0f;
        float y = 60.0f;
        
        size_t itemSum = 0;

        for (int i = 0; i < tempSectionContainer.size(); ++i) {
            x = 155;


            sectionHitBoxs[i].setBounds(x, y - userScrollPosition, 400, 35);
            int countItemRect = 0;

            x = 195 - 70;
            y += 45;

            auto& items = tempSectionContainer[i]->getItemsContainer();
            
            for (int j = 0; j < items.size(); ++j) {
                x += 115;
                itemHitBoxs[itemSum + j].setBounds(x, y - userScrollPosition, 110, 60);
                countItemRect++;

                if (countItemRect >= 6) {
                    y += 70;
                    x = 195 - 70;
                    countItemRect = 0;
                }
            }
            itemSum += items.size();
            y += 70;
        }
    }
    void refactorHitBoxIfDeletedOrAdded(const vector<ItemSection*>& tempSectionContainer) {

        if (tempSectionContainer.size() < sectionHitBoxs.size()) {
            int numDeletedSectionHitBoxs = sectionHitBoxs.size() - tempSectionContainer.size();
            for (int i = 0; i < numDeletedSectionHitBoxs; i++) {
                sectionHitBoxs.erase(sectionHitBoxs.begin());
            }
        }
        else if (tempSectionContainer.size() > sectionHitBoxs.size()) {
            int numAddedSectionHitBoxs = tempSectionContainer.size() - sectionHitBoxs.size();
            for (int i = 0; i < numAddedSectionHitBoxs; i++) {
                sectionHitBoxs.push_back(Rectangle<int>());
            }
        }
        itemCount = 0;
        for (int i = 0; i < tempSectionContainer.size(); ++i) {
            itemCount += tempSectionContainer[i]->getItemsContainer().size();
        }
        if (itemCount > itemHitBoxs.size()) {
            int numAddedItemHitBoxs = itemCount - itemHitBoxs.size();
            for (int i = 0; i < numAddedItemHitBoxs; ++i) {
                itemHitBoxs.push_back(Rectangle<int>());
            }
        }
        else if (itemCount < itemHitBoxs.size()) {
            int numDeletedItemHitBoxs = itemHitBoxs.size() - itemCount;
            for (int i = 0; i < numDeletedItemHitBoxs; i++) {
                itemHitBoxs.erase(itemHitBoxs.begin());
            }
        }
        
    }
    
    void buttonViewRefactor() {

        auto& tempSectionContainer = sectionManager->getItemSectionsContainer();

        if (tempSectionContainer.empty()) {

            sections.clear();
            buttonsAddItem.clear();
            buttonsDeleteSection.clear();
            buttonsOpenItemStatus.clear();

            return;
        }

        buttonsAddItem_And_Sections_Refactor(tempSectionContainer);

        buttonsDeleteSection_Refactor(tempSectionContainer);
        
        buttonsOpenItemStatus_Refactor(tempSectionContainer);
        

    }
    void buttonsAddItem_And_Sections_Refactor(const vector<ItemSection*>& tempSectionContainer) {

        // блок контроля популяции кнопок
        if (tempSectionContainer.size() < buttonsAddItem.size()) {
            int numDeletedButton = buttonsAddItem.size() - tempSectionContainer.size();
            for (int i = 0; i < numDeletedButton; ++i) {
                buttonsAddItem.erase(buttonsAddItem.begin());
            }
            
        }
        else if (tempSectionContainer.size() > buttonsAddItem.size()) {
            int numAddedItemButton = tempSectionContainer.size() - buttonsAddItem.size();
            for (int i = 0; i < numAddedItemButton; ++i) {
                buttonsAddItem.push_back(make_unique<ImageButton>());

                addAndMakeVisible(buttonsAddItem[buttonsAddItem.size() - 1].get());

                buttonsAddItem[buttonsAddItem.size() - 1]->setImages(false, true, true,
                    imageAddItem, 0.7f, Colours::transparentBlack,
                    imageAddItem, 0.5f, Colours::transparentBlack,
                    imageAddItem, 0.3f, Colours::transparentBlack
                );
                buttonsAddItem[buttonsAddItem.size() - 1]->addListener(this);
            }
            
        }

        // блок сортировки кнопок
        if (tempSectionContainer.size() != sections.size()) {

            sections.clear();
            for (int i = 0; i < tempSectionContainer.size(); ++i) {
                sections.push_back(make_unique<String>(tempSectionContainer[i]->getItemSectionName()));
            }
            
        }

        for (int i = 0; i < tempSectionContainer.size(); ++i) {

            buttonsAddItem[i]->setBounds(sectionHitBoxs[i].getX() + sectionHitBoxs[i].getWidth() + 30,
                sectionHitBoxs[i].getY(), 30, 30);

            if (buttonsAddItem[i]->getBounds().getY() >= -10 && buttonsAddItem[i]->getBounds().getY() <= 673) {

                buttonsAddItem[i]->setVisible(true);
                buttonsAddItem[i]->setEnabled(true);
            }
            else {
                buttonsAddItem[i]->setVisible(false);
                buttonsAddItem[i]->setEnabled(false);
            }
        }
    }
    void buttonsDeleteSection_Refactor(const vector<ItemSection*>& tempSectionContainer) {
        

        
        // // блок контроля популяции кнопок
        if (tempSectionContainer.size() < buttonsDeleteSection.size()) {
            int numDeletedButton = buttonsDeleteSection.size() - tempSectionContainer.size();
            for (int i = 0; i < numDeletedButton; ++i) {
                buttonsDeleteSection.erase(buttonsDeleteSection.begin());
            }
            
        }
        else if (tempSectionContainer.size() > buttonsDeleteSection.size()) {
            int numAddedButton = tempSectionContainer.size() - buttonsDeleteSection.size();
            for (int i = 0; i < numAddedButton; ++i) {
                buttonsDeleteSection.push_back(make_unique<TextButton>());
                addAndMakeVisible(buttonsDeleteSection[buttonsDeleteSection.size() - 1].get());
                buttonsDeleteSection[buttonsDeleteSection.size() - 1]->setButtonText(L"удалить секцию");
                buttonsDeleteSection[buttonsDeleteSection.size() - 1]->addListener(this);
            }
            
        }


        // блок сортировки кнопок
        for (int i = 0; i < buttonsDeleteSection.size(); ++i) {
            buttonsDeleteSection[i]->setBounds(buttonsAddItem[0]->getX() + buttonsAddItem[0]->getWidth() + 20, sectionHitBoxs[i].getY(), 60, 30);
            buttonsDeleteSection[i]->setVisible(buttonsAddItem[i]->isVisible());
            buttonsDeleteSection[i]->setEnabled(buttonsAddItem[i]->isEnabled());
        }
    }

    
    void buttonsOpenItemStatus_Refactor(const vector<ItemSection*>& tempSectionContainer) {

        //блок контроля популяции
        if (itemCount > buttonsOpenItemStatus.size()) {
            int numAdded = itemCount - buttonsOpenItemStatus.size();
            for (int i = 0; i < numAdded; ++i) {
                buttonsOpenItemStatus.push_back(make_unique<TextButton>());
                
                Item* tempItem = sectionManager->findItemFromNumCount(buttonsOpenItemStatus.size()-1);
                auto& button = buttonsOpenItemStatus[buttonsOpenItemStatus.size() - 1];

                button->setButtonText(tempItem->getItemName());
                button->addListener(this);

                button->setColour(TextButton::buttonColourId , Colour(63,61,73));
                button->setColour(TextButton::textColourOnId , Colour());
                button->setColour(TextButton::textColourOffId , Colour(76 , 161,255));

                addAndMakeVisible(button.get());
            }
            
            for (size_t i = 0; i < buttonsOpenItemStatus.size(); ++i) {
                Item* tempItem = sectionManager->findItemFromNumCount(i);
                buttonsOpenItemStatus[i]->setButtonText(tempItem->getItemName());

            }
            
            
        }
        //неоптимизированный костыль
        else if (itemCount < buttonsOpenItemStatus.size()) {
            int numDeleted = buttonsOpenItemStatus.size() - itemCount;
            for (int i = 0; i < numDeleted; ++i) {
                buttonsOpenItemStatus.erase(buttonsOpenItemStatus.begin());
                //deleteButtonOpenStatus_For_BlockRector(tempSectionContainer);
            }
            for (size_t i = 0; i < buttonsOpenItemStatus.size(); ++i) {
                Item* tempItem = sectionManager->findItemFromNumCount(i);
                buttonsOpenItemStatus[i]->setButtonText(tempItem->getItemName());
                
            }
            
        }
        //блок сортировки
        
        for (size_t i = 0; i < buttonsOpenItemStatus.size(); ++i) {
            
            buttonsOpenItemStatus[i]->setBounds(itemHitBoxs[i]);

            bool visible = (itemHitBoxs[i].getY() < 673 && itemHitBoxs[i].getY() > -10);

            buttonsOpenItemStatus[i]->setVisible(visible);
            buttonsOpenItemStatus[i]->setEnabled(visible);
            
            
        }
        
    }
    
    





    //перегруженые методы для обработки результатов поиска
    void updateHitBoxItemAndSection(vector<ItemSection*>& sections , vector<vector<Item*>>& items) {
        
        float x = 0.0f;
        float y = 60.0f;

        size_t itemSum = 0;
        

        for (int i = 0; i < sections.size(); ++i) {
            x = 155;


            sectionHitBoxs[i].setBounds(x, y - userScrollPosition, 400, 35);
            int countItemRect = 0;

            x = 195 - 70;
            y += 45;

            auto& itemCont = items[i];
            if (itemCont.empty())continue;
            for (int j = 0; j < itemCont.size(); ++j) {
                x += 115;
                itemHitBoxs[itemSum + j].setBounds(x, y - userScrollPosition, 110, 60);
                countItemRect++;

                if (countItemRect >= 6) {
                    y += 70;
                    x = 195 - 70;
                    countItemRect = 0;
                }
            }
            itemSum += itemCont.size();
            y += 70;
        }
    }
    void refactorHitBoxIfDeletedOrAdded(const vector<ItemSection*>& sections,const vector<Item*>& items) {

        if (sections.empty()) {
            sectionHitBoxs.clear();
            return;
        }

        if (sections.size() < sectionHitBoxs.size()) {
            int numDeletedSectionHitBoxs = sectionHitBoxs.size() - sections.size();
            for (int i = 0; i < numDeletedSectionHitBoxs; i++) {
                sectionHitBoxs.erase(sectionHitBoxs.begin());
            }
        }
        else if (sections.size() > sectionHitBoxs.size()) {
            int numAddedSectionHitBoxs = sections.size() - sectionHitBoxs.size();
            for (int i = 0; i < numAddedSectionHitBoxs; i++) {
                sectionHitBoxs.push_back(Rectangle<int>());
            }
        }
        
        if (items.empty()) {
            itemHitBoxs.clear();
            return;
        }
        if (items.size() > itemHitBoxs.size()) {
            int numAddedItemHitBoxs = items.size() - itemHitBoxs.size();
            for (int i = 0; i < numAddedItemHitBoxs; ++i) {
                itemHitBoxs.push_back(Rectangle<int>());
            }
        }
        else if (items.size() < itemHitBoxs.size()) {
            int numDeletedItemHitBoxs = itemHitBoxs.size() - items.size();
            for (int i = 0; i < numDeletedItemHitBoxs; i++) {
                itemHitBoxs.erase(itemHitBoxs.begin());
            }
        }

    }

    void buttonViewRefactor(const vector<ItemSection*>& sectionsFinded, const vector<Item*> items) {

        if (sectionsFinded.empty()) {

            sections.clear();
            buttonsAddItem.clear();
            buttonsDeleteSection.clear();
            buttonsOpenItemStatus.clear();

            return;
        }
        
        buttonsAddItem_And_Sections_Refactor_isFinded(sectionsFinded);

        buttonsDeleteSection_Refactor_isFinded(sectionsFinded);

        buttonsOpenItemStatus_Refactor(items);


    }
    void updateMaxScrollPosition(vector<ItemSection*>& sections, vector<vector<Item*>>& items) {
        
        if (sectionHitBoxs.empty() || sections.empty() && items.empty()) { maxScrollPosition = 0; return; }
        maxScrollPosition = 140;
        for (int i = 0; i < sectionHitBoxs.size() - 1; ++i) {

            int y1 = sectionHitBoxs[i].getY();
            int y2 = sectionHitBoxs[i + 1].getY();
            if (y1 < 0)y1 *= -1;
            if (y2 < 0)y2 *= -1;
            maxScrollPosition += sectionHitBoxs[i + 1].getY() - sectionHitBoxs[i].getY();

        }

        if (items[items.size() - 1].empty())return;

        int itemSizeLastSection = items[items.size() - 1].size();

        if (itemSizeLastSection > itemHitBoxs.size())return;

        maxScrollPosition += itemHitBoxs[itemHitBoxs.size() - itemSizeLastSection].getY() - sectionHitBoxs[sectionHitBoxs.size() - 1].getY();

        for (int i = itemHitBoxs.size() - itemSizeLastSection; i < itemHitBoxs.size(); i += 6) {
            if (i + 6 >= itemHitBoxs.size())return;
            int y1 = itemHitBoxs[i].getY();
            int y2 = itemHitBoxs[i + 6].getY();

            if (y1 < 0)y1 *= -1;
            if (y2 < 0)y2 *= -1;

            maxScrollPosition += y2 - y1;
        }

    }
    void buttonsAddItem_And_Sections_Refactor_isFinded(const vector<ItemSection*>& sectionsFinded) {

        // блок контроля популяции кнопок
        if (sectionsFinded.size() < buttonsAddItem.size()) {
            int numDeletedButton = buttonsAddItem.size() - sectionsFinded.size();
            for (int i = 0; i < numDeletedButton; ++i) {
                buttonsAddItem.erase(buttonsAddItem.begin());
            }
        }
        else if (sectionsFinded.size() > buttonsAddItem.size()) {
            int numAddedItemButton = sectionsFinded.size() - buttonsAddItem.size();
            for (int i = 0; i < numAddedItemButton; ++i) {
                buttonsAddItem.push_back(make_unique<ImageButton>());

                addAndMakeVisible(buttonsAddItem[buttonsAddItem.size() - 1].get());

                buttonsAddItem[buttonsAddItem.size() - 1]->setImages(false, true, true,
                    imageAddItem, 0.7f, Colours::transparentBlack,
                    imageAddItem, 0.5f, Colours::transparentBlack,
                    imageAddItem, 0.3f, Colours::transparentBlack
                );
                buttonsAddItem[buttonsAddItem.size() - 1]->addListener(this);
            }
        }
        
        if (sections.size() != sectionsFinded.size()) {

            sections.clear();
            for (int i = 0; i < sectionsFinded.size(); ++i) {
                sections.push_back(make_unique<String>(sectionsFinded[i]->getItemSectionName()));
            }
            
        }

        // блок сортировки кнопок
        for (int i = 0; i < buttonsAddItem.size(); ++i) {

            buttonsAddItem[i]->setBounds(sectionHitBoxs[i].getX() + sectionHitBoxs[i].getWidth() + 30,
                sectionHitBoxs[i].getY(), 30, 30);

            if (buttonsAddItem[i]->getBounds().getY() >= -10 && buttonsAddItem[i]->getBounds().getY() <= 673) {

                buttonsAddItem[i]->setVisible(true);
                buttonsAddItem[i]->setEnabled(true);
            }
            else {
                buttonsAddItem[i]->setVisible(false);
                buttonsAddItem[i]->setEnabled(false);
            }
        }
    }
    void buttonsDeleteSection_Refactor_isFinded(const vector<ItemSection*>& sectionsFinded) {



        // // блок контроля популяции кнопок
        if (sectionsFinded.size() < buttonsDeleteSection.size()) {
            int numDeletedButton = buttonsDeleteSection.size() - sectionsFinded.size();
            for (int i = 0; i < numDeletedButton; ++i) {
                buttonsDeleteSection.erase(buttonsDeleteSection.begin());
            }

        }
        else if (sectionsFinded.size() > buttonsDeleteSection.size()) {
            int numAddedButton = sectionsFinded.size() - buttonsDeleteSection.size();
            for (int i = 0; i < numAddedButton; ++i) {
                buttonsDeleteSection.push_back(make_unique<TextButton>());
                addAndMakeVisible(buttonsDeleteSection[buttonsDeleteSection.size() - 1].get());
                buttonsDeleteSection[buttonsDeleteSection.size() - 1]->setButtonText(L"удалить секцию");
                buttonsDeleteSection[buttonsDeleteSection.size() - 1]->addListener(this);
            }

        }


        // блок сортировки кнопок
        for (int i = 0; i < buttonsDeleteSection.size(); ++i) {
            buttonsDeleteSection[i]->setBounds(buttonsAddItem[0]->getX() + buttonsAddItem[0]->getWidth() + 20, sectionHitBoxs[i].getY(), 60, 30);
            buttonsDeleteSection[i]->setVisible(buttonsAddItem[i]->isVisible());
            buttonsDeleteSection[i]->setEnabled(buttonsAddItem[i]->isEnabled());
        }
    }

    void buttonsOpenItemStatus_Refactor(const vector<Item*>& items) {

        //блок контроля популяции
        if (items.size() > buttonsOpenItemStatus.size()) {
            int numAdded = items.size() - buttonsOpenItemStatus.size();
            for (int i = 0; i < numAdded; ++i) {
                buttonsOpenItemStatus.push_back(make_unique<TextButton>());

                Item* tempItem = sectionManager->findItemFromNumCount(buttonsOpenItemStatus.size() - 1);
                auto& button = buttonsOpenItemStatus[buttonsOpenItemStatus.size() - 1];

                button->setButtonText(tempItem->getItemName());
                button->addListener(this);

                button->setColour(TextButton::buttonColourId, Colour(63, 61, 73));
                button->setColour(TextButton::textColourOffId, Colour(76, 161, 255));

                addAndMakeVisible(button.get());
            }

            for (size_t i = 0; i < buttonsOpenItemStatus.size(); ++i) {
                Item* tempItem = sectionManager->findItemFromNumCount(i);
                buttonsOpenItemStatus[i]->setButtonText(tempItem->getItemName());

            }
            

        }
        //неоптимизированный костыль
        else if (items.size() < buttonsOpenItemStatus.size()) {
            int numDeleted = buttonsOpenItemStatus.size() - items.size();
            for (int i = 0; i < numDeleted; ++i) {
                buttonsOpenItemStatus.erase(buttonsOpenItemStatus.begin());
                //deleteButtonOpenStatus_For_BlockRector(tempSectionContainer);
            }
            for (size_t i = 0; i < buttonsOpenItemStatus.size(); ++i) {
                buttonsOpenItemStatus[i]->setButtonText(items[i]->getItemName());

            }
            
        }
        //блок сортировки
        
        for (size_t i = 0; i < buttonsOpenItemStatus.size(); ++i) {

            buttonsOpenItemStatus[i]->setBounds(itemHitBoxs[i]);

            bool visible = (itemHitBoxs[i].getY() < 673 && itemHitBoxs[i].getY() > -10);

            buttonsOpenItemStatus[i]->setVisible(visible);
            buttonsOpenItemStatus[i]->setEnabled(visible);


        }

    }

    




    void scrollBarMoved(ScrollBar* scrollBarListener, double newRangeStart) override {
        if (scrollBarListener == &scrollBar) {
            updateAll();
        }
    }
    void mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel) override {
        double newPos = scrollBar.getCurrentRangeStart() - wheel.deltaY * 200;
        newPos = jlimit(0.0, scrollBar.getMaximumRangeLimit(), newPos);
        userScrollPosition = newPos;
        scrollBar.setCurrentRangeStart(newPos);
        updateAll();
    }

    void updateMaxScrollPosition() {
        auto& tempSectionContainer = sectionManager->getItemSectionsContainer();

        if (sectionHitBoxs.empty() || tempSectionContainer.empty())return;
        maxScrollPosition = 140;
        for (int i = 0; i < sectionHitBoxs.size() - 1; ++i) {

            int y1 = sectionHitBoxs[i].getY();
            int y2 = sectionHitBoxs[i + 1].getY();
            if (y1 < 0)y1 *= -1;
            if (y2 < 0)y2 *= -1;
            maxScrollPosition += sectionHitBoxs[i + 1].getY() - sectionHitBoxs[i].getY();
            
        }
        
        if (tempSectionContainer[tempSectionContainer.size() - 1]->getItemsContainer().empty())return;
        
        int itemSizeLastSection = tempSectionContainer[tempSectionContainer.size() - 1]->getItemsContainer().size();

        if (itemSizeLastSection > itemHitBoxs.size())return;

        maxScrollPosition += itemHitBoxs[itemHitBoxs.size() - itemSizeLastSection].getY() - sectionHitBoxs[sectionHitBoxs.size()-1].getY();
        
        for (int i = itemHitBoxs.size() - itemSizeLastSection; i < itemHitBoxs.size(); i += 6) {
            if (i + 6 >= itemHitBoxs.size())return;
            int y1 = itemHitBoxs[i].getY();
            int y2 = itemHitBoxs[i + 6].getY();

            if (y1 < 0)y1 *= -1;
            if (y2 < 0)y2 *= -1;

            maxScrollPosition += y2 - y1;
        }
        
    }
    
    void updateScrollBar() {
        
        (maxScrollPosition >= 699) ? scrollBarVisible = true : scrollBarVisible = false;
        scrollBar.setVisible(scrollBarVisible);
        scrollBar.setEnabled(scrollBarVisible);

        scrollBar.setRangeLimits(0.0, maxScrollPosition);
        scrollBar.setCurrentRange(scrollBar.getCurrentRangeStart(), static_cast<double>(getHeight()), dontSendNotification);
        scrollBar.setSingleStepSize(25);
        userScrollPosition = scrollBar.getCurrentRangeStart();
    }
    

    void buttonExitCustomization() {
        addAndMakeVisible(buttonExit);


        buttonExit.setBounds(20, 600, 100, 40);
        buttonExit.setButtonText(L"Выход");

        buttonExit.addListener(this);
    }

    void buttonAddSectionCustomization() {
        addAndMakeVisible(buttonAddSection);

        addAndMakeVisible(label);

        addAndMakeVisible(undoAddSection);
        addAndMakeVisible(acceptAddSection);

        buttonAddSection.setBounds(900, 10, 100, 40);
        buttonAddSection.setButtonText(L"Добавить секцию");

        buttonAddSection.addListener(this);

        imagesUndoAndAccept.push_back(Image(ImageFileFormat::loadFrom(File("resourses/Images/undo.png"))));
        imagesUndoAndAccept.push_back(Image(ImageFileFormat::loadFrom(File("resourses/Images/accept.png"))));

        undoAddSection.setImages(false, true, true,
            imagesUndoAndAccept[0], 1.0f, Colours::transparentBlack,
            imagesUndoAndAccept[0], 0.8f, Colours::transparentBlack,
            imagesUndoAndAccept[0], 0.5f, Colours::transparentBlack
        );
        undoAddSection.addListener(this);
        undoAddSection.setBounds(840, 100, 30, 30);

        acceptAddSection.setImages(false, true, true,
            imagesUndoAndAccept[1], 1.0f, Colours::transparentBlack,
            imagesUndoAndAccept[1], 0.8f, Colours::transparentBlack,
            imagesUndoAndAccept[1], 0.5f, Colours::transparentBlack
        );
        acceptAddSection.addListener(this);
        acceptAddSection.setBounds(1000, 100, 30, 30);

        undoAddSection.setVisible(false);
        acceptAddSection.setVisible(false);
        label.setVisible(false);
        undoAddSection.setEnabled(false);
        acceptAddSection.setEnabled(false);
        label.setEnabled(false);
    }

    void scrollBarCustomization() {

        addAndMakeVisible(scrollBar);
        scrollBar.addListener(this);
        scrollBar.setAutoHide(false);

        scrollBar.setBounds(getWidth() - 18, 60, 14, getHeight() - 65);
        updateAll();

        scrollBarVisible = false;


    }

};

class MainComponent : public juce::Component , EventListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    SectionManager* sectionManager = &SectionManager::getInstance();
    ObserverEvents observer = ObserverEvents(5, 20, 130, 380 , sectionManager);

    unique_ptr<ButtonContainer> buttonContainer = make_unique<ButtonContainer>(sectionManager);

    unique_ptr<PanelAddItem> panelAddItem;
    
    unique_ptr<StatusAndRefactorPanel> statusAndRefactor;
    Colour panelsColour = Colour(61 , 58 ,75);
    
    std::future<void> type;

    float leftPanelWidth;

    

private:

    
    void drawLogic(juce::Graphics& g) {
        
        g.fillAll(juce::Colour(42, 40, 54));
        leftPanel(g , leftPanelWidth);

        buttonContainer->drawDebugTest(g);
        buttonContainer->drawableSection(g);

        searchPanel(g , leftPanelWidth - 0.1f);
        

        shadowBetweenPanels(g, leftPanelWidth - 0.1f);
        
        panelAddItem->paint(g);

        statusAndRefactor->paint(g);
    }
    
    void onUpdateAll() override {
        repaint();
    }
    
    void textChanged()override {
        buttonContainer->updateAll();
    }

    void PanelAddItem_On_Script(int index) override {

        panelAddItem->setPanelIndex(index);
        panelAddItem->setVisible(true);
        panelAddItem->setEnabled(true);
        addAndMakeVisible(panelAddItem.get());
        

        buttonContainer->updateAll();
        

    }
    void PanelAddItem_Off_Script() override {

        panelAddItem->setVisible(false);
        panelAddItem->setEnabled(false);

        buttonContainer->updateAll();
        
    }

    void StatusAndRefactor_Off_Script() override {
        
        statusAndRefactor->setVisible(false);
        statusAndRefactor->setEnabled(false);

        buttonContainer->updateAll();
        
        
    }
    void StatusAndRefactor_On_Script(size_t index) override {
        
        statusAndRefactor->setItemIndex(index);
        statusAndRefactor->setVisible(true);
        statusAndRefactor->setEnabled(true);
        addAndMakeVisible(statusAndRefactor.get());
        buttonContainer->updateAll();
        
        
    }

    void leftPanel(juce::Graphics& g , const float& width) {
        g.setColour(panelsColour);
        
        Rectangle<float> leftPanel(width, float(getHeight()));
        g.fillRect(leftPanel);
    }
    
    //незабыть вынести в отдельный класс
    void searchPanel(juce::Graphics& g , const float& pointX) {
        g.setColour(panelsColour);
        Rectangle<float> searchPanel(pointX, 0, getWidth(), getHeight() / 12.0f);
        g.fillRect(searchPanel);
    }
    
    
    void shadowBetweenPanels(juce::Graphics& g, const float& pointX) {
        Rectangle<float> shadowBetweenPanels(pointX, 0, getWidth() / 1100.f, getHeight());

        Colour start(0.06f, 0.06f, 0.06f, 0.35f);
        Colour end(0.06f, 0.06f, 0.06f, 0.01f);
        int numShadowRect = 24;

        for (int i = 0; i < numShadowRect; i++) {
            float proportion = float(i) / (numShadowRect - 1);
            Colour bandColour = start.interpolatedWith(end, proportion);

            g.setColour(bandColour);
            g.fillRect(shadowBetweenPanels);
            shadowBetweenPanels.translate(shadowBetweenPanels.getWidth(), 0);
        }
    }
    

    
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
