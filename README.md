
# Boundless Engine

"Boundless Engine" is a project I created using C++ in about 3 months on my spare time. It's a voxel engine experiment that uses OpenGL 4. This is my second time building a big 3D voxel engine, and it's a lot better than my first attempt back in 2014. 

This time it includes world generation using custom and super fast noise algorithms, a world made up of custom octree implementation and more tricks to allow a 16 km squared environment (!), LOD algorithms to allows for the large environment, OpenCL implementations to speed up octrees generation and cull them on the GPU, with over 120 steady FPS.

The magic happens in the face_cull algorithm and octree calculation to achieve the high FPS, see the `src/boundless-engine/cl` folder to understand more.

## Screenshots

Early tech demo screenshots -

![App Screenshot](https://i.imgur.com/D7BULrx.png)
![App Screenshot](https://i.imgur.com/AIIBGnD.png)
![App Screenshot](https://i.imgur.com/OvDHTzL.png)
![App Screenshot](https://s12.gifyu.com/images/SWvCf.gif)

