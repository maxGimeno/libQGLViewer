#ifndef QGLVIEWER_MANIPULATED_FRAME_H
#define QGLVIEWER_MANIPULATED_FRAME_H

#include "frame.h"
#include "mouseGrabber.h"
#include "qglviewer.h"

#include <QString>
#include <QTimer>
#include <QDateTime>

namespace qglviewer {
/*! \brief A ManipulatedFrame is a Frame that can be rotated and translated using the mouse.
  \class ManipulatedFrame manipulatedFrame.h QGLViewer/manipulatedFrame.h

  It converts the mouse motion into a translation and an orientation updates. A ManipulatedFrame is
  used to move an object in the scene. Combined with object selection, its MouseGrabber properties
  and a dynamic update of the scene, the ManipulatedFrame introduces a great reactivity in your
  applications.

  A ManipulatedFrame is attached to a QGLViewer using QGLViewer::setManipulatedFrame():
  \code
  init() { setManipulatedFrame( new ManipulatedFrame() ); }

  draw()
  {
	glPushMatrix();
	glMultMatrixd(manipulatedFrame()->matrix());
	// draw the manipulated object here
	glPopMatrix();
  }
  \endcode
  See the <a href="../examples/manipulatedFrame.html">manipulatedFrame example</a> for a complete
  application.

  Mouse events are normally sent to the QGLViewer::camera(). You have to press the QGLViewer::FRAME
  state key (default is \c Control) to move the QGLViewer::manipulatedFrame() instead. See the <a
  href="../mouse.html">mouse page</a> for a description of mouse button bindings.

  <h3>Inherited functionalities</h3>

  A ManipulatedFrame is an overloaded instance of a Frame. The powerful coordinate system
  transformation functions (Frame::coordinatesOf(), Frame::transformOf(), ...) can hence be applied
  to a ManipulatedFrame.

  A ManipulatedFrame is also a MouseGrabber. If the mouse cursor gets within a distance of 10 pixels
  from the projected position of the ManipulatedFrame, the ManipulatedFrame becomes the new
  QGLViewer::mouseGrabber(). It can then be manipulated directly, without any specific state key,
  object selection or GUI intervention. This is very convenient to directly move some objects in the
  scene (typically a light). See the <a href="../examples/mouseGrabber.html">mouseGrabber
  example</a> as an illustration. Note that QWidget::setMouseTracking() needs to be enabled in order
  to use this feature (see the MouseGrabber documentation).

  <h3>Advanced functionalities</h3>

  A QGLViewer can handle at most one ManipulatedFrame at a time. If you want to move several objects
  in the scene, you simply have to keep a list of the different ManipulatedFrames, and to activate
  the right one (using QGLViewer::setManipulatedFrame()) when needed. This can for instance be done
  according to an object selection: see the <a href="../examples/luxo.html">luxo example</a> for an
  illustration.

  When the ManipulatedFrame is being manipulated using the mouse (mouse pressed and not yet
  released), isManipulated() returns \c true. This might be used to trigger a specific action or
  display (as is done with QGLViewer::fastDraw()).

  The ManipulatedFrame also emits a manipulated() signal each time its state is modified by the
  mouse. This signal is automatically connected to the QGLViewer::update() slot when the
  ManipulatedFrame is attached to a viewer using QGLViewer::setManipulatedFrame().

  You can make the ManipulatedFrame spin() if you release the rotation mouse button while moving the
  mouse fast enough (see spinningSensitivity()). See also translationSensitivity() and
  rotationSensitivity() for sensitivity tuning. \nosubgrouping */
class QGLVIEWER_EXPORT ManipulatedFrame : public Frame, public MouseGrabber
{
#ifndef DOXYGEN
	friend class Camera;
	friend class ::QGLViewer;
#endif

	Q_OBJECT

public:
	ManipulatedFrame();
	/*! Virtual destructor. Empty. */
	virtual ~ManipulatedFrame() {}

	ManipulatedFrame(const ManipulatedFrame& mf);
	ManipulatedFrame& operator=(const ManipulatedFrame& mf);

Q_SIGNALS:
	/*! This signal is emitted when ever the ManipulatedFrame is manipulated (i.e. rotated or
	translated) using the mouse. Connect this signal to any object that should be notified.

	Note that this signal is automatically connected to the QGLViewer::update() slot, when the
	ManipulatedFrame is attached to a viewer using QGLViewer::setManipulatedFrame(), which is
	probably all you need.

	Use the QGLViewer::QGLViewerPool() if you need to connect this signal to all the viewers.

	See also the spun(), modified(), interpolated() and KeyFrameInterpolator::interpolated()
	signals' documentations. */
	void manipulated();

	/*! This signal is emitted when the ManipulatedFrame isSpinning().

	Note that for the QGLViewer::manipulatedFrame(), this signal is automatically connected to the
	QGLViewer::update() slot.

	Connect this signal to any object that should be notified. Use the QGLViewer::QGLViewerPool() if
	you need to connect this signal to all the viewers.

	See also the manipulated(), modified(), interpolated() and KeyFrameInterpolator::interpolated()
	signals' documentations. */
	void spun();

	/*! @name Manipulation sensitivity */
	//@{
public Q_SLOTS:
	/*! Defines the rotationSensitivity(). */
	void setRotationSensitivity(float sensitivity) { rotSensitivity_ = sensitivity; }
	/*! Defines the translationSensitivity(). */
	void setTranslationSensitivity(float sensitivity) { transSensitivity_ = sensitivity; }
	/*! Defines the spinningSensitivity(), in pixels per milliseconds. */
	void setSpinningSensitivity(float sensitivity) { spinningSensitivity_ = sensitivity; }
	/*! Defines the wheelSensitivity(). */
	void setWheelSensitivity(float sensitivity) { wheelSensitivity_ = sensitivity; }
public:
	/*! Returns the influence of a mouse displacement on the ManipulatedFrame rotation.

	Default value is 1.0. With an identical mouse displacement, a higher value will generate a
	larger rotation (and inversely for lower values). A 0.0 value will forbid ManipulatedFrame mouse
	rotation (see also constraint()).

	See also setRotationSensitivity(), translationSensitivity(), spinningSensitivity() and
	wheelSensitivity(). */
	float rotationSensitivity() const { return rotSensitivity_; }
	/*! Returns the influence of a mouse displacement on the ManipulatedFrame translation.

	Default value is 1.0. You should not have to modify this value, since with 1.0 the
	ManipulatedFrame precisely stays under the mouse cursor.

	With an identical mouse displacement, a higher value will generate a larger translation (and
	inversely for lower values). A 0.0 value will forbid ManipulatedFrame mouse translation (see
	also constraint()).

	\note When the ManipulatedFrame is used to move a \e Camera (see the ManipulatedCameraFrame
	class documentation), after zooming on a small region of your scene, the camera may translate
	too fast. For a camera, it is the Camera::pivotPoint() that exactly matches the mouse
	displacement. Hence, instead of changing the translationSensitivity(), solve the problem by
	(temporarily) setting the Camera::pivotPoint() to a point on the zoomed region (see the
	QGLViewer::RAP_FROM_PIXEL mouse binding in the <a href="../mouse.html">mouse page</a>).

	See also setTranslationSensitivity(), rotationSensitivity(), spinningSensitivity() and
	wheelSensitivity(). */
	float translationSensitivity() const { return transSensitivity_; }
	/*! Returns the minimum mouse speed required (at button release) to make the ManipulatedFrame
	  spin().

	See spin(), spinningQuaternion() and startSpinning() for details.

	Mouse speed is expressed in pixels per milliseconds. Default value is 0.3 (300 pixels per
	second). Use setSpinningSensitivity() to tune this value. A higher value will make spinning more
	difficult (a value of 100.0 forbids spinning in practice).

	See also setSpinningSensitivity(), translationSensitivity(), rotationSensitivity() and
	wheelSensitivity(). */
	float spinningSensitivity() const { return spinningSensitivity_; }
	/*! Returns the mouse wheel sensitivity.

	Default value is 1.0. A higher value will make the wheel action more efficient (usually meaning
	a faster zoom). Use a negative value to invert the zoom in and out directions.

	See also setWheelSensitivity(), translationSensitivity(), rotationSensitivity() and
	spinningSensitivity(). */
	float wheelSensitivity() const { return wheelSensitivity_; }
	//@}


	/*! @name Spinning */
	//@{
public:
	/*! Returns \c true when the ManipulatedFrame is spinning.

	During spinning, spin() rotates the ManipulatedFrame by its spinningQuaternion() at a frequency
	defined when the ManipulatedFrame startSpinning().

	Use startSpinning() and stopSpinning() to change this state. Default value is \c false. */
	bool isSpinning() const { return isSpinning_; }
	/*! Returns the incremental rotation that is applied by spin() to the ManipulatedFrame
	  orientation when it isSpinning().

	 Default value is a null rotation (identity Quaternion). Use setSpinningQuaternion() to change
	 this value.

	 The spinningQuaternion() axis is defined in the ManipulatedFrame coordinate system. You can use
	 Frame::transformOfFrom() to convert this axis from an other Frame coordinate system. */
	Quaternion spinningQuaternion() const { return spinningQuaternion_; }
public Q_SLOTS:
	/*! Defines the spinningQuaternion(). Its axis is defined in the ManipulatedFrame coordinate
	system. */
	void setSpinningQuaternion(const Quaternion& spinningQuaternion) { spinningQuaternion_ = spinningQuaternion; }
	virtual void startSpinning(int updateInterval);
	/*! Stops the spinning motion started using startSpinning(). isSpinning() will return \c false
	  after this call. */
	virtual void stopSpinning() { spinningTimer_.stop(); isSpinning_ = false; }
protected Q_SLOTS:
	virtual void spin();
private Q_SLOTS:
	void spinUpdate();
	//@}

	/*! @name Mouse event handlers */
	//@{
protected:
	virtual void mousePressEvent      (QMouseEvent* const event, Camera* const camera);
	virtual void mouseMoveEvent       (QMouseEvent* const event, Camera* const camera);
	virtual void mouseReleaseEvent    (QMouseEvent* const event, Camera* const camera);
	virtual void mouseDoubleClickEvent(QMouseEvent* const event, Camera* const camera);
	virtual void wheelEvent           (QWheelEvent* const event, Camera* const camera);
	//@}

public:
	/*! @name Current state */
	//@{
	bool isManipulated() const;
	/*! Returns the \c MouseAction currently applied to this ManipulatedFrame.

	  Will return QGLViewer::NO_MOUSE_ACTION unless a mouse button is being pressed
	  and has been bound to this QGLViewer::MouseHandler.

	 The binding between mouse buttons and key modifiers and MouseAction is set using
	 QGLViewer::setMouseBinding(Qt::Key key, Qt::KeyboardModifiers modifiers, Qt::MouseButton buttons, MouseHandler handler, MouseAction action, bool withConstraint).
	  */
	QGLViewer::MouseAction currentMouseAction() const { return action_; }
	//@}

	/*! @name MouseGrabber implementation */
	//@{
public:
	virtual void checkIfGrabsMouse(int x, int y, const Camera* const camera);
	//@}

	/*! @name XML representation */
	//@{
public:
	virtual QDomElement domElement(const QString& name, QDomDocument& document) const;
public Q_SLOTS:
	virtual void initFromDOMElement(const QDomElement& element);
	//@}

#ifndef DOXYGEN
protected:
	Quaternion deformedBallQuaternion(int x, int y, float cx, float cy, const Camera* const camera);

	QGLViewer::MouseAction action_;
	Constraint* previousConstraint_; // When manipulation is without Contraint.

	virtual void startAction(int ma, bool withConstraint=true); // int is really a QGLViewer::MouseAction
	void computeMouseSpeed(const QMouseEvent* const e);
	int mouseOriginalDirection(const QMouseEvent* const e);

	// Previous mouse position (used for incremental updates) and mouse press position.
	QPoint prevPos_, pressPos_;
#endif // DOXYGEN

private:
	// Sensitivity
	float rotSensitivity_;
	float transSensitivity_;
	float spinningSensitivity_;
	float wheelSensitivity_;

	// Mouse speed and spinning
	QTime last_move_time;
	float mouseSpeed_;
	int delay_;
	bool isSpinning_;
	QTimer spinningTimer_;
	Quaternion spinningQuaternion_;

	// Whether the SCREEN_TRANS direction (horizontal or vertical) is fixed or not.
	bool dirIsFixed_;

	// MouseGrabber
	bool keepsGrabbingMouse_;
};

} // namespace qglviewer

#endif // QGLVIEWER_MANIPULATED_FRAME_H
