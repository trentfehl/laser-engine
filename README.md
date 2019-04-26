# laser-engine
Algorithmically generated laser show. Utilizes openFrameworks and addon ofxLaser.

<p align="center">
<img src="/media/nominal.gif" width="496" height="289">
</p>

### Behind The Scenes
At the core of this display is a B-Spline controlled by points that move freely around the
inside of a sphere. The gif below is a demo showing the spline in purple and toggling the display of the control polygon (green) and the boundary (red). The portion of the boundary that we're really looking is a cross section at z = 0. 

<p align="center">
<img src="/media/spline_details.gif" width="496" height="289">
</p>

### Rotations
The spline is rotated around an axis at a constant rate. The axis itself initializes in a random location and drifts. In
the first image below you can see the boundary that we saw above that was a slice through z=0 be rotated along with the
spline. I had to locally modify OpenFramework's ofPolyline scale() function to take scaling in the Z-axis for the spline
to not collapse when rotated so that the viewer was looking perpendicular to the rotated Z-axis.

<p align="center">
<img src="/media/rotation.gif" width="496" height="289">
</p>

### To-Do
- ~~Smooth rotation of spline.~~
- Rotation rate and rotation axis drift driven by audio BPM
- Dampen control point bounces off sphere boundary.

### Local Mods
I made a few changes to local repositories that I've checked out.
- Added rotateQuat() for ofPolyline -> [Issue](https://github.com/openframeworks/openFrameworks/issues/6267)
- Added Z-axis component support to ofPolyline's scale().
- ofxTinyspline was forked a long time ago, I just replaced the four core files with updated versions.
