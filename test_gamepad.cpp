#include "JuceLibraryCode/JuceHeader.h"

Component* createMainContentComponent();

class gui : public JUCEApplication
{
public:
	const String getApplicationName() override { return ProjectInfo::projectName; }
	const String getApplicationVersion() override { return ProjectInfo::versionString; }
	bool moreThanOneInstanceAllowed() override { return true; }
	void initialise(const String& commandLine) override { mainWindow = new MainWindow(getApplicationName()); }
	void shutdown() override { mainWindow = nullptr; }
	void systemRequestedQuit() override { quit(); }

	void anotherInstanceStarted(const String& commandLine) override { }

	class MainWindow : public DocumentWindow
	{
	public:
		MainWindow(String name) : DocumentWindow(name, Desktop::getInstance().getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId), DocumentWindow::allButtons)
		{
			setUsingNativeTitleBar(true);
			setContentOwned(createMainContentComponent(), true);
			setResizable(true, true);

			centreWithSize(getWidth(), getHeight());
			setVisible(true);
		}

		void closeButtonPressed() override
		{
			JUCEApplication::getInstance()->systemRequestedQuit();
		}
	};

private:
	ScopedPointer<MainWindow> mainWindow;
};

int main()
{
	
}
START_JUCE_APPLICATION(gui)