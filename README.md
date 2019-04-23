# laser-engine
Algorithmically generated laser show. Utilizes openFrameworks and addon ofxLaser.

### Behind The Scenes
The core of this display is a B-Spline controlled by points that move freely around the
inside of a sphere. The gif below shows the spline in purple, the control polygon in 
green and the boundary in red. We're really looking at a cross section of the boundary 
at z = 0. 

![Alt Text](/media/spline_details_1.gif)  ![Alt Text](/media/spline_details_2.gif)

### To-Do
- Smooth rotation of spline
- Rotation rate and rotation axis drift driven by audio BPM
