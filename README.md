# laser-engine
Algorithmically generated laser show. Utilizes openFrameworks and addon ofxLaser.

### Behind The Scenes
At the core of this display is a B-Spline controlled by points that move freely around the
inside of a sphere. The gif below is a demo showing the spline in purple and toggling the display of the control polygon (green) and the boundary (red). The portion of the boundary that we're really looking is a cross section at z = 0. 

<p align="center">
<img src="/media/spline_details.gif" width="496" height="289">
</p>

### To-Do
- Smooth rotation of spline
- Rotation rate and rotation axis drift driven by audio BPM
