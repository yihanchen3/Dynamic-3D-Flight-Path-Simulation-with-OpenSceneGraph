#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32
#include <iostream>

#include <osg/Node>
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Material>
#include <osg/Image>
#include <osg/Texture2D>
#include <osg/BoundingSphere>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/TexGen>
#include <osg/TexEnv>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIEventAdapter>

#include <osg/PositionAttitudeTransform>
#include <osgViewer/ViewerEventHandlers>
#include <osg/MatrixTransform>
#include <OpenThreads/Thread>
#include <osg/LightSource>
#include <osg/Light>

// Function to create a node
osg::ref_ptr<osg::Node> CreateNode()
{
    osg::ref_ptr<osg::Group> _root = new osg::Group;

    // Define and read the elevation file
    // The actual elevation file name is: ASTGTM2_N34E110_dem.tif
    // It is a special TIFF format, GEOTiff
    // It is read using OSG's GDAL plugin, so .gdal is added to the path
    // The .gdal extension only needs to be added here, the actual elevation file extension does not need to be modified
    // osg::ref_ptr<osg::HeightField> heightMap = osgDB::readHeightFieldFile("E:\\OpenSourceGraph\\osgearth_install20190830\\data\\world.tif.gdal");
    osg::ref_ptr<osg::HeightField> heightMap = osgDB::readHeightFieldFile("E:\\OpenSourceGraph\\osgearth_install20190830\\data\\1\\1_0_0_5.tif.gdal");

    // Create a leaf node object
    osg::ref_ptr<osg::Geode> geode = new osg::Geode;

    if (heightMap != nullptr)
    {
        // Since the original data is too large, creating a 3D object would fail, so a new object is created
        // This is equivalent to thinning the data once. Of course, you could directly crop the original image using a special tool
        // Create a new HeightField object to copy heightMap
        osg::ref_ptr<osg::HeightField> heightMap1 = new osg::HeightField;
        // Copy some properties from the original object
        heightMap1->setOrigin(heightMap->getOrigin());
        heightMap1->setRotation(heightMap->getRotation());
        heightMap1->setSkirtHeight(heightMap->getSkirtHeight());
        // Set the interval in the XY direction to twice the original
        heightMap1->setXInterval(heightMap->getXInterval() * 2);
        heightMap1->setYInterval(heightMap->getYInterval() * 2);
        // Set the number of rows and columns of the new elevation data to half of the original
        heightMap1->allocate(heightMap->getNumColumns() / 2, heightMap->getNumRows() / 2);

        // Place the actual data values into the new object
        for (size_t r = 0; r < heightMap1->getNumRows(); ++r)
        {
            for (size_t c = 0; c < heightMap1->getNumColumns(); ++c)
            {
                // The loaded data has an XY direction interval of about 0.0002 (longitude and latitude offset), with 3600 grids, and the magnitude is too small, while the elevation values are often in the thousands. Without coordinate conversion (GPS to meters), the displayed result would be severely distorted. So, the height value is simply divided by 50000 (this was tested for this specific TIFF file; different elevation files might require different values)
                heightMap1->setHeight(c, r, heightMap->getHeight(c * 2, r * 2) / 500);
            }
        }

        // Add to the leaf node
        geode->addDrawable(new osg::ShapeDrawable(heightMap1));

        osg::ref_ptr<osg::Material> material = new osg::Material;
        material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
        material->setShininess(osg::Material::FRONT_AND_BACK, 60);

        osg::ref_ptr<osg::Texture2D> texture2D = new osg::Texture2D;
        // Set texture
        osg::ref_ptr<osg::Image> image1 = osgDB::readImageFile("D:\\image_1\\arm1.jpg");
        if (image1.valid())
        {
            texture2D->setImage(image1.get());
        }
        geode->getOrCreateStateSet()->setAttributeAndModes(material.get(), osg::StateAttribute::ON);
        geode->getOrCreateStateSe
