/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.1.5

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "MainComponent.h"

//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
MainComponent::MainComponent (): 
    state(TransportState::Stopped),
    thumbnailCache(5), 
    thumbnail(512, formatManager, thumbnailCache)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]
   

   
    openFileButton.reset (new juce::TextButton ("Open file button"));
    addAndMakeVisible (openFileButton.get());
    openFileButton->setButtonText (TRANS("Open..."));
    openFileButton->onClick = [this]() {openFileButtonClicked(); };
    openFileButton->setColour (juce::TextButton::buttonColourId, juce::Colours::grey);

    openFileButton->setBounds (32, 24, 150, 24);

    stopButton.reset (new juce::TextButton ("Stop button"));
    addAndMakeVisible (stopButton.get());
    stopButton->setButtonText (TRANS("Stop"));
    stopButton->onClick = [this]() { stopButtonClicked(); };
    stopButton->setColour (juce::TextButton::buttonColourId, juce::Colours::brown);
    stopButton->setEnabled(false);

    stopButton->setBounds (368, 24, 150, 24);

    playButton.reset (new juce::TextButton ("Play button"));
    addAndMakeVisible (playButton.get());
    playButton->setButtonText (TRANS("Play"));
    playButton->onClick = [this]() { playButtonClicked(); };
    playButton->setColour (juce::TextButton::buttonColourId, juce::Colours::green);
    playButton->setEnabled(false);

    playButton->setBounds (200, 24, 150, 24);

    /*
    pauseButton.reset(new juce::TextButton("Pause button"));
    addAndMakeVisible(playButton.get());
    pauseButton->setButtonText(TRANS("Pause"));
    pauseButton->onClick = [this]() { playButtonClicked(); };
    pauseButton->setColour(juce::TextButton::buttonColourId, juce::Colour(0xffffb242));
    pauseButton->setEnabled(false);

    pauseButton->setBounds(400, 24, 150, 24);
    */
    formatManager.registerBasicFormats();       // [1]
    transportSource.addChangeListener(this);   // [2]
    thumbnail.addChangeListener(this);
    setAudioChannels(0, 2);
    startTimer(40);

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);
    
  
    

    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

MainComponent::~MainComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    openFileButton = nullptr;
    stopButton = nullptr;
    playButton = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}



//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));
    juce::Rectangle<int> thumbnailBounds(10, 100, getWidth() - 20, getHeight() - 120);

    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}
void MainComponent::timerCallback()
{
    repaint();
}
void MainComponent::paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::white);
    g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void MainComponent::paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colours::white);
    g.fillRect(thumbnailBounds);

    g.setColour(juce::Colours::red);                               // [8]
    auto audioLength = (float)thumbnail.getTotalLength();
    thumbnail.drawChannels(g,                                      // [9]
        thumbnailBounds,
        0.0,                                    // start time
        thumbnail.getTotalLength(),             // end time
        1.0f);                                  // vertical zoom
    g.setColour(juce::Colours::green);

    auto audioPosition = (float)transportSource.getCurrentPosition();
    auto drawPosition = (audioPosition / audioLength) * (float)thumbnailBounds.getWidth()
        + (float)thumbnailBounds.getX();                                // [13]
    g.drawLine(drawPosition, (float)thumbnailBounds.getY(), drawPosition,
        (float)thumbnailBounds.getBottom(), 2.0f);                              // [14]
}

void MainComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}


void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source) {
    if (source == &transportSource) {
        if (transportSource.isPlaying())
            changeState(TransportState::Playing);
        else if ((state == TransportState::Stopping) || (state == TransportState::Playing))
            changeState(TransportState::Stopped);
        else if (TransportState::Pausing == state)
            changeState(TransportState::Paused);
    }
    if (source == &thumbnail)      
        repaint();
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) 
{
    if (readerSource.get() == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    transportSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::changeState(TransportState newState)
{
    
    if (state != newState) {
        state = newState;
    
        switch (state)
        {
        case MainComponent::TransportState::Stopped:
            stopButton->setEnabled(false);
            playButton->setButtonText("Play");
            stopButton->setButtonText("Stop");
            transportSource.setPosition(0.0);
            break;
        case MainComponent::TransportState::Starting:
            transportSource.start();
            break;
        case MainComponent::TransportState::Playing:
            playButton->setButtonText("Pause");
            stopButton->setButtonText("Stop");
            stopButton->setEnabled(true);
            break;
        case MainComponent::TransportState::Stopping:
            transportSource.stop();
            break;
        case MainComponent::TransportState::Pausing:
            transportSource.stop();
            break;
        case MainComponent::TransportState::Paused:
            playButton->setButtonText("Resume");
            stopButton->setButtonText("Return to Zero");
            break;
 
        }
    }
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::releaseResources()
{
    transportSource.releaseResources();
}

void MainComponent::openFileButtonClicked() {
    chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...",
        juce::File{}, "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc) {
        auto file = fc.getResult();
        if (file != juce::File{}) {
            auto* reader = formatManager.createReaderFor(file);
            if (reader != nullptr) {
                auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
                transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
                playButton->setEnabled(true);
                thumbnail.setSource(new juce::FileInputSource(file));
                readerSource.reset(newSource.release());
            }
        }
    
    });

}

void MainComponent::playButtonClicked()
{
    if ((state == TransportState::Stopped) || (state == TransportState::Paused))
        changeState(TransportState::Starting);
    else if (state == TransportState::Playing)
        changeState(TransportState::Pausing);

}

void MainComponent::stopButtonClicked()
{
    if (state == TransportState::Paused)
        changeState(TransportState::Stopped);
    else
        changeState(TransportState::Stopping);

}


//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="MainComponent" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <TEXTBUTTON name="open file button" id="d5ae0f6b72d1308b" memberName="openFileButton"
              virtualName="" explicitFocusOrder="0" pos="32 24 150 24" bgColOff="ff808080"
              buttonText="Open..." connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Stop button" id="36e3029f15710548" memberName="stopButton"
              virtualName="" explicitFocusOrder="0" pos="368 24 150 24" bgColOff="ffa52a2a"
              buttonText="Stop" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <TEXTBUTTON name="Play Button" id="c0fe59e4116c1c53" memberName="playBotton"
              virtualName="" explicitFocusOrder="0" pos="200 24 150 24" bgColOff="ff008000"
              buttonText="Play" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

