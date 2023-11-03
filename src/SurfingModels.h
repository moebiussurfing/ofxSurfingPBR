/*

	TODO

	store a scale/rot/pos on each model 
		on a vector
		this allows browse models 
		maintaining transforms
		add gizmo

*/

//--

#pragma once
#include "ofMain.h"

// Optional
// requires to check previously
// the sanity of the files you pretend to load!
// bc some could freeze the app or too big!

class SurfingModels {
public:
	SurfingModels();
	~SurfingModels();

public:
	void setup(string path = "") {
		setupDir(path);

		nameFile.setSerializable(false);

		parameters.setName("MODELS");
		parameters.add(nextBang);
		parameters.add(prevBang);
		parameters.add(indexFile);
		parameters.add(nameFile);

		listenerIndexFile = indexFile.newListener([this](int & i) {
			static int i_ = -1;
			if (i != i_) { //changed
				i_ = i;
				if (i < dir.size()) {
					nameFile = dir.getName(i);

					pathModel = dir.getPath(i);
					loadBang.trigger();
				}
			}
		});

		listenerNext = nextBang.newListener([this](void) {
			next();
		});

		listenerPrevious = prevBang.newListener([this](void) {
			previous();
		});
	}

	void setupDir(string path = "") {

		if (path == "") path = "models/";
		// bin/data/models/

		pathModels = path;
		//supported formats
		dir.allowExt("ply");
		dir.allowExt("fbx");
		dir.allowExt("obj");
		dir.listDir(path);
		dir.sort();

		indexFile.setMax(dir.size() - 1);
	}

	const string getFilename() {
		string s = "NONE";
		if (indexFile < dir.size()) s = dir.getName(indexFile.get());
		return s;
	}

	const string getFilenamesList() {
		string s = "";
		for (size_t i = 0; i < dir.size(); i++) {
			bool b = (i == indexFile);
			s += b ? "* " : "  ";
			s += ofToString(i);
			s += " ";
			s += dir.getName(i);
			s += "\n";
		}
		return s;
	}

	void next() {
		if (dir.size() > 0) {
			indexFile.setWithoutEventNotifications(indexFile.get() + 1);
			indexFile.set(indexFile % dir.size());
		}
	}

	void previous() {
		if (dir.size() > 0) {
			if (indexFile.get() > 0) {
				indexFile.setWithoutEventNotifications(indexFile.get() - 1);
				indexFile.set(indexFile % dir.size());
			} else {
				if (indexFile.get() <= 0) {
					indexFile.set(dir.size() - 1);
				}
			}
		}
	}

private:
	ofDirectory dir;

public:
	ofParameterGroup parameters;

	ofParameter<string> nameFile { "Name", "" };
	ofParameter<int> indexFile { "File", 0, 0, 0 };
	ofParameter<void> nextBang { "Next" };
	ofParameter<void> prevBang { "Previous" };

	////TODO. store each model ofNode/transforms/gizmo
	//ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
	//ofParameter<float> yPos { "PosY", 0, -1.f, 1.f };

public:
	ofParameter<void> loadBang { "LoadBang" };
	// to be listened from parent the scope!
	// then will load the model getting the path:
	// pathModel

private:
	ofEventListener listenerIndexFile;
	ofEventListener listenerNext;
	ofEventListener listenerPrevious;

public:
	string pathModel = ""; //to get the path for the model. ready to load!
	string pathModels = ""; //for display only
};

SurfingModels::SurfingModels() {
}

SurfingModels::~SurfingModels() {
}

/*

	EXAMPLE

	// .h

	#include "SurfingModels.h"

	SurfingModels surfingModels;
	ofEventListener listenerLoadModel;
	

	// .cpp
	
	// setup

	surfingModels.setup();

	// callback to trig loading.
	// model path is ready on surfingModels.pathModel!
	listenerLoadModel = surfingModels.loadBang.newListener([this](void) {
		string path = surfingModels.pathModel;
		this->loadModel(path);
	});
	
	gui.add(surfingModels.parameters);
	
	// keyPressed
	if (key == OF_KEY_RIGHT) {
		surfingModels.next();
	}
	if (key == OF_KEY_LEFT) {
		surfingModels.previous();
	}
	
*/