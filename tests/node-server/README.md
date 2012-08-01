# Intellistreets Footfall Demo
It's intended to demostrate a complete working example of the pedestrian tracking system.

## Architecture
Comprised to two pieces the camera capture service and Footfal App. They communicate through Intellifx Protocol over udp and http for passing images.

### Camera Capture Service
* Requirements
    * Small lightweight application that can capture images and run image processing techniquies.
    * Ability to be cross-compiled for Intellistreets end node platform and interduce the least amount of dependancies.
    * Ability to use the Intellistreets communication architecture. *See Todos.


* Implementation
	* C++ application using OpenCV 2.1 for basic processing. Links against the following OpenCV 2.1 libraries libcv, libcvaux, and libcxcore 
	* Uses the V3 Intellifx Communcation library for handling packet structures, however uses a UDP implementation for communcation layer for all data (excludes images)
	* For image transfers uses libjpg for compression and libcurl to HTTP Post to backend application.
	* USB camera interfaces does not use OpenCV implementation to remove the dependancies interduced by libhighgui. Uses libvl2 directly.


### Node.js Footfall App
* Requirements
	* Provide a service to allow backend camera capture to send data and images and store them for later use by a web app.
	* Provide a web 2.0 font-end for the client to see the camera's images and data.

* Implementation
	* Entirely written in Node.js to provide both the backend communcation service listening for new data from the cameras through udp. Provide a the front end web application.
	* Uses MongoDb and Node.js Mongoose to provide models for storing the camera analytics and image references to on disk files.
	* Uses Node.js express to handle http routes and view rendering using ejs templates.
	* Uses socket.io for communcation between web client for sending new information and states to the clients browser when cameras update. Also uses redis to store socket.io session and session data.
	
	

    


