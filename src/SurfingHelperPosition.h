#pragma once
#include "ofMain.h"

#include "ofxSurfingPBRConstants.h"

//----

class PointCartesian {
private:
	// Define spherical to Cartesian conversion function
	glm::vec3 sphericalToCartesian(float r, float theta, float phi) {
		ofLogNotice("ofxSurfingPBR") << "sphericalToCartesian()";

		return glm::vec3(
			r * sin(theta) * cos(phi),
			r * sin(theta) * sin(phi),
			r * cos(theta));
	}

	// Define Cartesian to spherical conversion function
	void cartesianToSpherical(const glm::vec3 & cartesian, ofParameter<float> & distance, ofParameter<float> & latitude, ofParameter<float> & longitude) {
		ofLogNotice("ofxSurfingPBR") << "cartesianToSpherical()";

		float r = glm::length(cartesian);
		float theta = acos(cartesian.z / r);
		float phi = atan2(cartesian.y, cartesian.x);

		// Update spherical coordinates only if they differ from the current ones
		if (abs(distance.get() - r) > 0.001) {
			if (distance != r) distance.set(r);
		}
		if (abs(latitude.get() - theta) > 0.001) {
			if (latitude != r) latitude.set(theta);
		}
		if (abs(longitude.get() - phi) > 0.001) {
			if (longitude != r) longitude.set(phi);
		}
	}

public:
	ofParameterGroup parameters;
	ofParameterGroup paramsSpherical;
	ofParameter<glm::vec3> position;
	ofParameter<float> distance;
	ofParameter<float> latitude;
	ofParameter<float> longitude;

	glm::vec3 position_ { -1.f, -1.f, -1.f };
	float distance_ = -1;
	float latitude_ = -1;
	float longitude_ = -1;

private:
	string name = "";
	ofEventListener eParams;
	const float sz = SURFING__PBR__SCENE_SIZE_UNIT * 1.0f;
	bool bAttending = false;

public:
	PointCartesian() { }
	~PointCartesian() { }

	void setup(const string & name = "") {
		ofLogNotice("ofxSurfingPBR") << "PointCartesian::setup() " << name;

		this->name = name;
		setupParameters();
		setupCallbacks();
	}

	void set(const glm::vec3 & position) {
		this->position.set(position);
	}

	const glm::vec3 get() {
		return position.get();
	}

	operator glm::vec3() const {
		return position.get();
	}

	float x() const {
		return position.get().x;
	}

	float y() const {
		return position.get().y;
	}

	float z() const {
		return position.get().z;
	}

private:
	void setupParameters() {
		ofLogNotice("ofxSurfingPBR") << "PointCartesian::setupParameters() " << name;

		position.set("Cartesian", glm::vec3(), glm::vec3(-sz), glm::vec3(sz));
		distance.set("Distance", 0, 0, sz);
		//distance.set("Distance", 0, 0, std::sqrt(3) * sz);
		longitude.set("Longitude", 0, -PI, PI);
		latitude.set("Latitude", 0, -PI, PI);

		string n = "Position";
		if (name != "") n = name + " " + n;

		paramsSpherical.setName("Spherical");
		paramsSpherical.add(distance);
		paramsSpherical.add(latitude);
		paramsSpherical.add(longitude);

		parameters.setName(n);
		parameters.add(position);
		parameters.add(paramsSpherical);
	}

	void setupCallbacks() {
		ofLogNotice("ofxSurfingPBR") << "PointCartesian::setupCallbacks() " << name;

		eParams = parameters.parameterChangedE().newListener([this](ofAbstractParameter & e) {
			if (bAttending) return;

			string n = e.getName();

			if (e.isOfType<glm::vec3>()) {
				if (n == position.getName() && position.get() != position_) {
					ofLogNotice("ofxSurfingPBR") << "PointCartesian::Changed " << name << " " << n << ": " << e;
					bAttending = true;
					cartesianToSpherical(position, distance, latitude, longitude);
					bAttending = false;
					position_ = position;
				}
			}

			else if (e.isOfType<float>()) {
				if (n == distance.getName() && distance.get() != distance_) {
					ofLogNotice("ofxSurfingPBR") << "PointCartesian::Changed " << name << " " << n << ": " << e;
					bAttending = true;
					position = sphericalToCartesian(distance, latitude, longitude);
					bAttending = false;
					distance_ = distance;
				}

				else if (n == latitude.getName() && latitude.get() != latitude_) {
					ofLogNotice("ofxSurfingPBR") << "PointCartesian::Changed " << name << " " << n << ": " << e;
					bAttending = true;
					position = sphericalToCartesian(distance, latitude, longitude);
					bAttending = false;
					latitude_ = latitude;
				}

				else if (n == longitude.getName() && longitude.get() != longitude_) {
					ofLogNotice("ofxSurfingPBR") << "PointCartesian::Changed " << name << " " << n << ": " << e;
					bAttending = true;
					position = sphericalToCartesian(distance, latitude, longitude);
					bAttending = false;
					longitude_ = longitude;
				}
			}
		});
	}
};
