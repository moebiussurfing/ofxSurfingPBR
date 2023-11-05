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

#include "ofxSurfingHelpersLite.h"
#include <functional>
using callback_t = std::function<void()>;

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

		//--

		setupParams();

		//--

		// autoSaver
		//register the local save function to be called when saving is required.
		callback_t f = std::bind(&SurfingModels::save, this);
		autoSaver.setFunctionSaver(f);
		//parameters.add(autoSaver.bEnable);

		load();
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

		if (dir.size() == 0) {
			ofLogError("SurfingModels") << "setupDir() path or files not found!";
			return;
		}

		indexFile.setMin(0);
		indexFile.setMax(dir.size() - 1);
		indexFile.set(0);

		//--

		//nodes.clear();
		//for (size_t i = 0; i < dir.size(); i++) {
		//	ofParameter<ofNode> node_;
		//	string name = dir.getName(i);
		//	node_.set(name, n, nmin, nmax);
		//	nodes.emplace_back(node_);
		//}

		transforms.clear();
		for (size_t i = 0; i < dir.size(); i++) {

			transforms.emplace_back(transform);
		}
	}

	void setupParams() {
		nameFile.setSerializable(false);

		parameters.setName("MODELS");
		parameters.add(nextBang);
		parameters.add(prevBang);
		parameters.add(indexFile);
		parameters.add(nameFile);

		transformParams.setName("Transforms");
		transformParams.add(transform.scale);
		transformParams.add(transform.yPos);
		transformParams.add(transform.yRot);

		//--

		// there's not implemented ofParams for ofNodes !
		//n.setPosition(0, 0, 0);
		//float u = SURFING__SCENE_SIZE_UNIT;
		//nmin.setPosition(-u, -u, -u);
		//nmax.setPosition(u, u, u);
		//node.set("Actor", n, nmin, nmax);
		//transformParams.add(node);

		parameters.add(transformParams);
		transformParams.setSerializable(false);

		parameters.add(bHelp);
		parameters.add(vReset);

		//--

		listenerIndexFile = indexFile.newListener([this](int & i) {
			static int i_ = -1;
			if (i != i_) { //check that changed
				//store prev index before change
				if (i_ >= 0 && i_ < dir.size()) {
					//transforms[i_] = transform;
					transforms[i_].scale.set(transform.scale);
					transforms[i_].yPos.set(transform.yPos);
					transforms[i_].yRot.set(transform.yRot);
				}

				i_ = i;

				//load new index
				if (i < dir.size()) {
					doProcessIndex(i);
				}
			}
		});

		listenerNext = nextBang.newListener([this](void) {
			next();
		});
		listenerPrevious = prevBang.newListener([this](void) {
			previous();
		});

		listenerReset = vReset.newListener([this](void) {
			doReset();
		});
	}

	void doProcessIndex(int i) {

		nameFile = dir.getName(i);

		pathModel = dir.getPath(i);
		loadBang.trigger();

		//if (i < nodes.size()) {
		//	node = nodes[i];
		//}

		if (i < transforms.size()) {
			//transform = transforms[i];
			transform.scale.set(transforms[i].scale);
			transform.yPos.set(transforms[i].yPos);
			transform.yRot.set(transforms[i].yRot);
		}

		buildHelp();
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
			s += b ? ">" : " ";
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
	ofParameter<int> indexFile { "File", -1, -1, -1 };
	ofParameter<void> nextBang { "Next" };
	ofParameter<void> prevBang { "Previous" };

	ofParameter<void> vReset { "Reset" };

	void doReset() { 
	}
	
private:
	ofEventListener listenerReset;
	
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

	SurfingAutoSaver autoSaver;
	string path = "SurfingModels.json";

	void save() {
		ofxSurfing::saveSettings(parameters, path);
	}
	void load() {
		autoSaver.pause();
		ofxSurfing::loadSettings(parameters, path);
		autoSaver.start();
	}

	//--

	//TODO.
	// store each model ofNode/transforms/gizmo

private:
	struct Transform {
		ofParameter<float> scale { "Scale", 0, -1.f, 1.f };
		ofParameter<float> yPos { "y Pos", 0, -1.f, 1.f };
		ofParameter<float> yRot { "y Rot", 0, -1.f, 1.f };
	};
	Transform transform;
	vector<Transform> transforms;

public:
	ofParameterGroup transformParams;

	//public:
	//	//TODO
	//	ofParameter<ofNode> node;
	//	vector<ofParameter<ofNode>> nodes;

	//private:
	//	ofNode n;
	//	ofNode nmin;
	//	ofNode nmax;

private:
	string sHelp;
	void buildHelp() {
		sHelp = "";
		sHelp += "MODEL\n";
		sHelp += "\n";
		sHelp += this->getFilename() + "\n";
		sHelp += "\n";
		sHelp += "      BROWSE\n";
		sHelp += "UP    Prev\n";
		sHelp += "DOWN  Next\n";
		sHelp += "\n";
		sHelp += this->getFilenamesList();
	}

public:
	ofParameter<bool> bHelp { "Help", true };

	void drawHelp() {
		if (!bHelp) return;
		ofxSurfing::ofDrawBitmapStringBox(sHelp, ofxSurfing::SURFING_LAYOUT_BOTTOM_RIGHT);
	}
};

SurfingModels::SurfingModels() {
}

SurfingModels::~SurfingModels() {
	//TODO: fix auto saver!
	save();
}

//------

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