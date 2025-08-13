#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    setSize (1040, 673);
    

    addAndMakeVisible(buttonContainer.get());

    buttonContainer->setListener(this);
    
    addAndMakeVisible(label);

    panelAddItem = make_unique<PanelAddItem>(sectionManager ,getWidth(), getHeight(), getWidth() / 3, getHeight() / 2.4f);
    panelAddItem->setListener(this);
    panelAddItem->setVisible(false);
    panelAddItem->setEnabled(false);
    
    statusAndRefactor = make_unique<StatusAndRefactorPanel>(sectionManager, getWidth(), getHeight(), getWidth() / 3, getHeight() / 2.f);
    statusAndRefactor->setListener(this);
    statusAndRefactor->setVisible(false);
    statusAndRefactor->setEnabled(false);
    
    sectionManager->load();

    leftPanelWidth = getWidth() / 7.2f;

    

    
}

MainComponent::~MainComponent()
{
    sectionManager = nullptr;
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    

    drawLogic(g);

    
}

void MainComponent::resized()
{
    
}


