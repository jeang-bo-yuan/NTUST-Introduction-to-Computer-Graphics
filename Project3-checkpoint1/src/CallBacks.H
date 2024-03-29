/************************************************************************//**
     @file CallBacks.H

     - Author:
                  + Michael Gleicher, gleicher@cs.wisc.edu
     - Modifier
                  + Yu-Chi Lai, yu-chi@cs.wisc.edu
                  + Bo-Yuan Jeang, b11132021@mail.ntust.edu.tw
     
     - Comment:     Header file to define callback functions.
						define the callbacks for the TrainWindow

						these are little functions that get called when the 
						various widgets
						get accessed (or the fltk timer ticks). these 
						functions are used 
						when TrainWindow sets itself up.

     - Platform:    Visio Studio.Net 2003/2005

*************************************************************************/
#pragma once

// Preclarify to prevent the compiler error
class TrainWindow;
class Fl_Widget;

// Reset the train window
void resetCB(Fl_Widget*, TrainWindow* tw);
// Something change and thus we need to update the view
void damageCB(Fl_Widget*, TrainWindow* tw);

// Callback that adds a new point to the spline
// idea: add the point AFTER the selected point
void addPointCB(Fl_Widget*, TrainWindow* tw);


// Callback that deletes a point from the spline
void deletePointCB(Fl_Widget*, TrainWindow* tw);

// Callbacks for advancing/pulling back train
void forwCB(Fl_Widget*, TrainWindow* tw);
void backCB(Fl_Widget*, TrainWindow* tw);

// Idle callback: for run the step of the window
void runButtonCB(TrainWindow* tw);

// For load and save buttons
void loadCB(Fl_Widget*, TrainWindow* tw);
void saveCB(Fl_Widget*, TrainWindow* tw);

// roll the control points
// Rotate the selected control point  about x axis by one more degree
void rpxCB(Fl_Widget*, TrainWindow* tw);
// Rotate the selected control point  about x axis by one less degree
void rmxCB(Fl_Widget*, TrainWindow* tw);

// Rotate the selected control point  about the z axis one more degree
void rpzCB(Fl_Widget*, TrainWindow* tw);
// Rotate the selected control point  about the z axis one less degree
void rmzCB(Fl_Widget*, TrainWindow* tw);

/// @name Added by myself
/// @author Bo Yuan, Jeang
/// @{

/**
 * @brief Callback function when "add car" is clicked
 * @param tw - the trainwindow
 * @author Bo Yuan, Jeang
 */
void add_car_CB(Fl_Widget*, TrainWindow* tw);

/**
 * @brief Callback function when "delete car" is clicked
 * @param tw - the trainwindow
 * @author Bo Yuan, Jeang
 */
void delete_car_CB(Fl_Widget*, TrainWindow* tw);

/**
 * @brief Callback function when "spline type" is changed
 * @param tw - the trainwindow
 * @author Bo Yuan, Jeang
 */
void spline_change_CB(Fl_Widget*, TrainWindow* tw);

/// @}