#include <Windows.h>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <math.h>
#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>
#include <osgGA/AnimationPathManipulator>
#include <osgViewer/ViewerEventHandlers>

#include <osg/Node>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/Math>
#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/Object>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osgUtil/Optimizer>


using namespace std;

// Path animation control event
class AnimationEventHandler : public osgGA::GUIEventHandler
{
public:
	AnimationEventHandler(osgViewer::Viewer &vr) :viewer(vr) {}

	// Event handling
	virtual bool handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &)
	{
		// Create an animation update callback object
		osg::ref_ptr<osg::AnimationPathCallback> animationPathCallback = new osg::AnimationPathCallback();

		osg::ref_ptr<osg::Group> group = dynamic_cast<osg::Group*>(viewer.getSceneData());

		// Get the animation properties of the node
		animationPathCallback = dynamic_cast<osg::AnimationPathCallback*>(group->getChild(0)->getUpdateCallback());

		switch (ea.getEventType())
		{
		case (osgGA::GUIEventAdapter::KEYDOWN):
		{
			if (ea.getKey() == 'p')
			{
				// Pause
				animationPathCallback->setPause(true);
				return true;
			}
			if (ea.getKey() == 'k')
			{
				// Start
				animationPathCallback->setPause(false);
				return true;
			}
			if (ea.getKey() == 'r')
			{
				// Reset
				animationPathCallback->reset();
				return true;
			}

			break;
		}

		default:
			break;
		}
		return false;
	}

public:
	osgViewer::Viewer &viewer;
};

// Create Path
osg::ref_ptr<osg::AnimationPath> creatAnimationPath(const std::vector<std::vector<double>>& pos0)
{
	// Create a Path object
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath();
	// Set the animation mode to LOOP
	animationPath->setLoopMode(osg::AnimationPath::LOOP);

	// Roll angle
	float roll = osg::inDegrees(10.0f);
	float diff_xz, diff_y;
	float angle_xz = osg::inDegrees(0.0f) ,angle_y = osg::inDegrees(0.0f);
	// Time offset
	float time = 0.0f;
	float time_delta = 0.14f;

	for (int i = 0; i < pos0.size() - 1; i++)
	{	
		// Key point position
		osg::Vec3 position(pos0[i][0]-3.4, pos0[i][1], pos0[i][2]-2);

		// Key point angle - horizontal
		if (pos0[i + 1][0] - pos0[i][0] == 0)
		{
			if (pos0[i + 1][1] - pos0[i][1] == 0)
				diff_xz = 0;
			else if (pos0[i + 1][1] - pos0[i][1] > 0)
				diff_xz = 1.707;
			else if (pos0[i + 1][1] - pos0[i][1] < 0)
				diff_xz = -1.707;
		}
	    else if (pos0[i + 1][0] - pos0[i][0] < 0)
			diff_xz = -atan((pos0[i + 1][1] - pos0[i][1]) / (pos0[i + 1][0] - pos0[i][0]));
		else
			diff_xz = atan((pos0[i + 1][1] - pos0[i][1])/ (pos0[i + 1][0] - pos0[i][0]));

		diff_xz -= angle_xz;
		angle_xz += diff_xz / 8;

		// Key point angle - vertical
		if (pos0[i + 1][0] - pos0[i][0] == 0 && pos0[i + 1][1] - pos0[i][1] == 0)
		{
			if (pos0[i + 1][2] - pos0[i][2] > 0)
				diff_y = 1.71;
			else if (pos0[i + 1][2] - pos0[i][2] < 0)
				diff_y = -1.71;
			else
				diff_y = 0;
		}
		else if (pos0[i + 1][2] - pos0[i][2] < 0)
			diff_y = atan((pos0[i + 1][2] - pos0[i][2]) /  sqrt((pos0[i + 1][0] - pos0[i][0])*(pos0[i + 1][0] - pos0[i][0])+(pos0[i + 1][1] - pos0[i][1])*(pos0[i + 1][1] - pos0[i][1])));
		else
			diff_y = atan((pos0[i + 1][2] - pos0[i][2]) / sqrt((pos0[i + 1][0] - pos0[i][0])*(pos0[i + 1][0] - pos0[i][0]) + (pos0[i + 1][1] - pos0[i][1])*(pos0[i + 1][1] - pos0[i][1])));
		
		diff_y -= angle_y;
		angle_y += diff_y / 15;
		
		osg::Quat rotation_xy(osg::Quat (angle_xz, osg::Vec3(0.0, 0.0, 1.0)));
		osg::Quat rotation_z(osg::Quat(-angle_y, osg::Vec3(sin(angle_xz), cos(angle_xz), 0.0)));
		rotation_z *= rotation_xy;

		// Insert the Path, push the key points and time into it to form the Path
		animationPath->insert(time, osg::AnimationPath::ControlPoint(position, rotation_z));

		time += time_delta;
	}

	// Return the Path
	return animationPath.get();
}



// Function to create a model and place it in the scene at multiple positions
void createAndPlaceModel(const std::string& modelPath, const std::vector<osg::Vec3>& positions, osg::Group* root, double scale = 0.001, const osg::Quat& rotation = osg::Quat()) {
	// Load the model
	osg::ref_ptr<osg::Node> model = osgDB::readNodeFile(modelPath);
	if (!model) {
		std::cerr << "Error: Could not load model: " << modelPath << std::endl;
		return;
	}

	// Enable rescaling for the model
	model->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);

	// Get the bounding sphere to correctly center the model
	const osg::BoundingSphere& bound = model->getBound();

	// Loop through each position and create a transform for the model
	for (const auto& position : positions) {
		osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform();
		transform->setMatrix(osg::Matrix::translate(-bound.center()) *
			osg::Matrix::scale(scale, scale, scale) *
			osg::Matrix::rotate(rotation) *
			osg::Matrix::translate(position));
		transform->addChild(model.get());

		// Add the transformed model to the scene graph
		root->addChild(transform.get());
	}
	std::cout << positions.size() << modelPath << "'model loaded." << std::endl;
}


int main()
{
	string filename = "../../../keypoint.txt";  // Replace with your actual filename
	ifstream file(filename);
	string line;
	vector<vector<double>> pos0;

	// Read the positions from the file
	while (getline(file, line)) {
		istringstream iss(line);
		vector<double> pos(3);
		if (iss >> pos[0] >> pos[1] >> pos[2]) {
			pos0.push_back(pos);
		}
	}

	// Display all positions in pos0
	for (size_t i = 0; i < pos0.size(); ++i) {
		cout << "pos0[" << i << "] = ("
			<< pos0[i][0] << ", "
			<< pos0[i][1] << ", "
			<< pos0[i][2] << ")"
			<< endl;
	}
	system("pause");
	

	// Main part
	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
	osg::ref_ptr<osg::Group> root = new osg::Group();

	// default path: ${ProjectDir}\out\build\x64-Debug
	string default_path = "../../../materials/";

	osg::ref_ptr<osg::Node> helicopter = osgDB::readNodeFile(default_path + "helicopter2.3ds");	// Read the helicopter model
	helicopter->getOrCreateStateSet()->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);

	// Get the bounding box to determine the rotation center of the animation
	const osg::BoundingSphere &bs = helicopter->getBound();
	osg::Vec3 position = bs.center();
	// Scaling factor, if the scaling is inappropriate, the model will not be visible
	float size = 0.001;
	
	// Create the path
	int numpoint = 181;
	osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath();
	animationPath = creatAnimationPath(pos0);
	osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();

	// OSG ensures that only STATIC data can be rendered
	mt->setMatrix(osg::Matrix::translate(-bs.center()+osg::Vec3(0 ,0 ,0)) * osg::Matrix::scale(size, size, size) * osg::Matrix::rotate(osg::inDegrees(-180.0f), 0.0f, 0.0f, 1.0f));
	mt->addChild(helicopter.get());

	osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
	// Set the update callback
	pat->setUpdateCallback(new osg::AnimationPathCallback(animationPath.get(), 0.0f, 1.0f));
	pat->addChild(mt.get());

	root->addChild(pat.get());

	cout << "aircraft loaded" << endl;


	
	// Import terrain model
	osg::ref_ptr<osg::Node> terrian = osgDB::readNodeFile(default_path + "landscape/3dterrian.3ds"); 
	terrian->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	
	const osg::BoundingSphere &cs = terrian->getBound();
	osg::ref_ptr<osg::MatrixTransform> mt_terrian = new osg::MatrixTransform();
	mt_terrian->setMatrix(osg::Matrix::translate(-cs.center() + osg::Vec3(0, 0, 0)) * osg::Matrix::scale(4,2,0.5) * osg::Matrix::rotate(osg::inDegrees(0.0f), 1.0f, 0.0f, 0.0f) * osg::Matrix::translate(osg::Vec3(0, 50,-5)));
	mt_terrian->addChild(terrian.get());

	root->addChild(mt_terrian.get());

	cout << "terrian loaded" << endl;
	
	// Import and place Transimission Tower models (1-5)

	string towerModelPath = default_path + "TransmissionTower.3ds";
	vector<osg::Vec3> towerPositions = {
		osg::Vec3(20, 65, -1),
		osg::Vec3(20, 75, -1),
		osg::Vec3(40, 65, -1),
		osg::Vec3(40, 75, -1),
		//osg::Vec3(190, 60, -1)
	};
	createAndPlaceModel(towerModelPath, towerPositions, root.get(), 0.001, osg::Quat(osg::inDegrees(-180.0f), osg::Vec3(1.0f, 0.0f, 0.0f)));

	// Import and place Tree models (1-18)

	string treeModelPath = default_path + "tree.3ds";

	// Tree positions extracted from the original snippet
	vector<osg::Vec3> treePositions = {
		osg::Vec3(60, 60, -3),
		osg::Vec3(60, 65, -3),
		osg::Vec3(60, 70, -3),
		osg::Vec3(60, 75, -3),
		osg::Vec3(60, 80, -3),
		osg::Vec3(60, 85, -3),
		osg::Vec3(65, 62.5, -3),
		osg::Vec3(65, 67.5, -3),
		osg::Vec3(65, 72.5, -3),
		osg::Vec3(65, 77.5, -3),
		osg::Vec3(65, 82.5, -3),
		osg::Vec3(70, 60, -3),
		osg::Vec3(70, 65, -3),
		osg::Vec3(70, 70, -3),
		osg::Vec3(70, 75, -3),
		osg::Vec3(70, 80, -3),
	};

	// Create and place tree models using the createAndPlaceModel function
	createAndPlaceModel(treeModelPath, treePositions, root.get(), 0.012, osg::Quat());

	
	string aptModelPath = default_path + "apartment.3ds";

	// High-rise positions extracted from the original snippet
	vector<osg::Vec3> aptPositions = {
		osg::Vec3(120, 60, -2),
		osg::Vec3(120, 70, -2),
		osg::Vec3(120, 80, -2),
		osg::Vec3(135, 65, -2),
		osg::Vec3(135, 75, -2),
		osg::Vec3(135, 85, -2),
		osg::Vec3(135, 55, -2),
		osg::Vec3(150, 60, -2),
		osg::Vec3(150, 70, -2),
		osg::Vec3(150, 80, -2)
	};

	createAndPlaceModel(aptModelPath, aptPositions, root.get(), 0.015, osg::Quat(osg::inDegrees(-90.0f), osg::Vec3(0.0f, 0.0f, 1.0f)));
	



	string lowriseModelPath = default_path + "lowrise.3ds";
	// Low-rise positions extracted from the original snippet
	vector<osg::Vec3> lowrisePositions = {
		osg::Vec3(80, 70, -3),
		osg::Vec3(90, 70, -3),
		osg::Vec3(100, 70, -3),
		osg::Vec3(110, 70, -3)
	};

	// Apply the low-rise model placements
	createAndPlaceModel(lowriseModelPath, lowrisePositions, root.get(), 0.09, osg::Quat());

	
	// Optimizing scene data
	osgUtil::Optimizer optimizer;
	optimizer.optimize(root.get());

	viewer->setSceneData(root.get());

	// Add path animation
	viewer->addEventHandler(new AnimationEventHandler(*(viewer.get())));
	
    // auto pAnimationMani = new osgGA::AnimationPathManipulator("AnimationPath.txt");

	osgGA::AnimationPathManipulator *animationPathMp = new osgGA::AnimationPathManipulator();

	osg::AnimationPath* _animationPath = new osg::AnimationPath;

	osg::Quat q1(osg::DegreesToRadians(0.0), osg::Y_AXIS);
	osg::Quat q2(osg::DegreesToRadians(90.0), osg::X_AXIS);
	//osg::Quat q3(osg::DegreesToRadians(-5.0), osg::Y_AXIS);
	q1 = q1*q2;


	for (int i = 0; i < pos0.size() - 1; i++) {
		_animationPath->insert(0.14 * i, osg::AnimationPath::ControlPoint(osg::Vec3d(pos0[i][0], pos0[i][1] - 60, pos0[i][2]), q1));
	}
	
	// Set the path loop
	_animationPath->setLoopMode(osg::AnimationPath::LOOP);
	animationPathMp->setAnimationPath(_animationPath);
	viewer->setCameraManipulator(animationPathMp);

	viewer->addEventHandler(new osgViewer::WindowSizeHandler);



	
	viewer->run();

	return 0;

}