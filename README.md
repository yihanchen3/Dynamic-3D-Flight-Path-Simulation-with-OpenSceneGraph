# **Dynamic 3D Flight Path Simulation with OpenSceneGraph**

This project is a flight animation simulator using OpenSceneGraph (OSG) to render 3D models and animate them along predefined paths. The simulator creates a scene with various 3D objects, including aircrafts, trees, buildings, and power towers, and animates an aircraft model through this environment performing obstacle-avoiding cruise based on a calculated set of key points.

![Screenshot](animation_example.png")


## **Technical Details**

### **Prerequisites**

- **Visual C++ 2022**: Ensure that Visual Studio 2022 is installed with C++ support.
- **CMake**: This project uses Cmake to configure and build management (`CMakeLists.txt`).
- **OpenSceneGraph 3.6.5**: Utilize the precompiled package for 64-bit Debug Version from [Objexx](https://objexx.com/OpenSceneGraph.html).

### **Installation and Setup**

1. **Clone the Repository**:
    ```sh
    git clone https://github.com/yourusername/flight-animation-simulator.git
    cd flight-animation-simulator
    ```

2. **Install OpenSceneGraph**:
    - Download and extract the precompiled OpenSceneGraph 3.6.5 package for Visual C++ 2022 from [Objexx](https://objexx.com/OpenSceneGraph.html).
    - Place the download **bin, include, lib** folders under `osg` folder, ensure the OSG libraries are accessible in Cmake project path.

3. **Configure and Build the Project with CMake**:
    - Modify the `CMakeLists.txt` with your library and exectuable path baed on your project path if needed.
    - Build the Cmake using Visual Studio or Command Line.

### **Running the Simulation**

1. **Prepare 3D Models**:
    - Ensure that the `materials` folder contains the required 3D models (`helocopter2.3ds` is used for the aircraft and `3dterrian.3ds` for the terrain in current setting)
    - Models are loaded from this folder during the simulation.

2. **Define Flight Path**:
    - Create a `keypoint.txt` file in the project root directory with the coordinates for the aircraft’s flight path.

3. **Run the Simulation**:
    - Debug using Visual Studio or Execute the compiled program using command line:
    ```sh
    ./out/build/x64-Debug/osg_cruise_simulation.exe
    ```

4. **Control the Animation**:
    - `p`: Pause the animation.
    - `k`: Start/Resume the animation.
    - `r`: Reset the animation.

## **Project Structure**

- **`flight.cpp`**: Core C++ code for rendering the 3D environment and controlling the animation.
- **`materials/`**: Contains all 3D models and terrians used in the simulation.
- **`osg/`**: Stores the necessary OpenSceneGraph libraries.
- **`CMakeLists.txt`**: Configuration file for building the project with CMake.
- **`keypoint.txt`**: (User-provided) Defines the helicopter’s flight path coordinates.
- **`gaocheng.cpp`**: This other code sets up an OSG viewer that loads and displays a downsampled heightmap terrain model from DEM with applied textures and lighting.