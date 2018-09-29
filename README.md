# Interactive Wall Version 2
### Requirements
- Windows 8/8.1
- A USB 3.0 port
- A Kinect V2
- Kinect V2 SDK: http://www.microsoft.com/en-us/download/details.aspx?id=44561
- Visual Studio 2013 (full or express verison)
- ofxBlobTracker (CCV blob tracking): https://github.com/patriciogonzalezvivo/ofxBlobTracker
- ofxTuio (optional TUIO Client): https://github.com/patriciogonzalezvivo/ofxTuio

### To set up and compile project

1. Download this OpenFrameworks Fork: https://github.com/liquidzym/openFrameworks/tree/VS2013
2. Download this Kinect V2 addon: https://github.com/elliotwoods/ofxKinectForWindows2
3. Extract OpenFrameworks to a known place.
4. Extract ofxKinectForWindows2 to **(OpenFrameworks root)/addons** & rename to just "ofxKinectForWindows2"
5. Repeat previous step for ofxBlobTracker and ofxTuio
6. Repeat step 4. for the custom ofxOsc folder provided with this repository
7. Copy this project into **(OpenFramworks root)/apps/myApps/**  
(make sure you create the "myApps" folder, if it's not there)
8. go to the **(OpenFrameworks root)\addons\ofxKinectForWindows2\ofxKinectForWindows2Lib** folder, edit "ofxKinectForWindows2Lib.vcxproj" and modify lines 21, 27 to "<PlatformToolset>v120</PlatformToolset>" (it is v110, which will only compile with Visual Studio 2012)

9. Compile, you will get an error in the "Proceses.h" file, comments out the three lines causing the error.

10. Compile & enjoy!

### Receive Touch Events on Windows 8/8.1

This program currently sends OSC messages of touch events over port 12345. Any program that has a TUIO/OSC client can recieve the events. 

TouchProxy - https://touchproxy.codeplex.com/

